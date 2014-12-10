/* Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
/* Compiler bug:
 * When compiling under C++ Builder XE, you will get this error:
 * [BCC32 Error] closures.hpp(237): E2299 Cannot generate template specialization from 'impl::closure_frame_holder<FrameT>'
 * To fix it, change line 237 in closures.hpp to:
 * impl::closure_frame_holder<closure_frame<ClosureT> >& frame;
 * See also QC report #79813
 */
#include "Func32.h"
#include "Func32Impl.h"
#pragma hdrstop
#include <stack>

//#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_OUT std::clog
#define BOOST_SPIRIT_CLOSURE_LIMIT 4
#define PHOENIX_LIMIT 4

#pragma option -vi- //Disable inline expansion to fix several compiler bugs in BCC 5.6.4

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_exceptions.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/bind.hpp>
#include <boost/spirit/include/classic_distinct.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
//---------------------------------------------------------------------------
#ifdef _DEBUG
std::ostream& operator<<(std::ostream& Stream, const std::deque<Func32::TElem> &D)
{
	Stream << "deque(";
	for(std::deque<Func32::TElem>::const_iterator Iter = D.begin(); Iter != D.end(); ++Iter)
		Stream << Iter->Ident;
	Stream << ")" << std::endl;
  return Stream;
}
#endif
//---------------------------------------------------------------------------
namespace Func32
{
using namespace boost::spirit::classic;
using namespace phoenix;

typedef symbols<TElem, wchar_t> TSymbols;
const struct TFuncSymbols : TSymbols
{
  TFuncSymbols()
  {
    for(int I = FirstFunction1P; I <= LastFunction; I++)
    {
      TIdent Ident = static_cast<TIdent>(I);
      const wchar_t *Name = FunctionName(Ident);
      if(*Name)
				add(Name, Ident);
    }
  }
} DefaultFuncSymbols;
//---------------------------------------------------------------------------
//Predefined constants. Variables are added later. Case sensitive
const struct TCaseSymbols : public TSymbols
{
  TCaseSymbols()
  {
     add(L"e",  Codee)
        (L"i",  Codei);
  }
} Symbols;
//---------------------------------------------------------------------------
//Case sensitive symbols
const struct TNoCaseSymbols : symbols<TElem, wchar_t>
{
  TNoCaseSymbols()
	{
     add(L"pi",    CodePi)
        (L"rand",  CodeRand)
        (L"undef", CodeUndef)
        (L"inf",   CodeInf);
  }
} DefaultNoCaseSymbols;
//---------------------------------------------------------------------------
//Special functions that take an expression as first argument and a variable name as the second
const struct TSpecialFuncSymbols : symbols<TElem, wchar_t>
{
	TSpecialFuncSymbols()
	{
		 add(L"integrate", CodeIntegrate)
				(L"sum",       CodeSum)
				(L"product",   CodeProduct);
	}
} SpecialFuncSymbols;
//---------------------------------------------------------------------------
const struct TCompareSymbols : symbols<TElem>
{
	TCompareSymbols()
	{ add("=",  cmEqual)
			 ("<>", cmNotEqual)
			 ("<",  cmLess)
			 (">",  cmGreater)
			 ("<=", cmLessEqual)
			 (">=", cmGreaterEqual);
	}
} CompareSymbols;
//---------------------------------------------------------------------------
//User defined symbols
struct TUserSymbols : symbols<std::wstring>
{
	TUserSymbols(const TSymbolList *Symbols)
	{
		if(Symbols)
		{
			TSymbolList::TConstIterator Begin = Symbols->Begin();
			TSymbolList::TConstIterator End = Symbols->End();
			for(TSymbolList::TConstIterator Iter = Begin; Iter != End; ++Iter)
				add(Iter->first.c_str(), Iter->first);
		}
	}
};
//---------------------------------------------------------------------------
struct TContext : boost::spirit::classic::closure<TContext, std::deque<TElem>, unsigned, symbols<TElem, wchar_t>, wchar_t>
{
	member1 List;
	member2 Arg;
	member3 Symbols;
	member4 EndBracket;
};
//---------------------------------------------------------------------------
struct TContext2 : boost::spirit::classic::closure<TContext2, wchar_t>
{
	member1 EndBracket;
};
//---------------------------------------------------------------------------
void ThrowParseError(const wchar_t *Begin, const EParseError &E)
{
	throw parser_error<EParseError, const wchar_t*>(Begin, E);
}
//---------------------------------------------------------------------------
//Action: Throws an error from the place where the action is invoked
class TDoError
{
	TErrorCode ErrorCode;
public:
	TDoError(TErrorCode AErrorCode): ErrorCode(AErrorCode) {}
	void operator()(const wchar_t *Begin, const wchar_t*) const
	{
		if(ErrorCode == ecUnknownChar)
			ThrowParseError(Begin, EParseError(ecUnknownChar, 0, std::wstring(1, *Begin)));
		ThrowParseError(Begin, ErrorCode);
	}
};
//---------------------------------------------------------------------------
//Action: Called when an operator is found
struct TDoOperator
{
	TIdent Ident;
	TContext::member1 &Elements;
	TDoOperator(TContext::member1 &AElements, TIdent Operator) : Ident(Operator), Elements(AElements) {}
	void operator()(const std::deque<TElem> &List) const
	{
		//This should be handled in the simplify code.
		//a^(b/c) will be converted to CodePowDiv(a,b,c) instead of CodePow(a, CodeDiv(b,c))
		if(Ident == CodePow && List[0].Ident == CodeDiv)
		{
			Elements().push_front(CodePowDiv);
			Elements().insert(Elements().end(), List.begin()+1, List.end());
		}
		//a^2 will be converted to CodeSqr(a) instead of CodePow(a,2)
		else if(Ident == CodePow && List[0] == 2)
			Elements().push_front(CodeSqr);
		else
		{
			Elements().push_front(Ident);
			Elements().insert(Elements().end(), List.begin(), List.end());
		}
	}

  void operator ()(wchar_t Ch) const
  {
    Elements().push_back(Ident);
  }
};
//---------------------------------------------------------------------------
struct TDoNegate
{
	TContext::member1 &Elements;
	TDoNegate(TContext::member1 &AElements) : Elements(AElements) {}
	void operator()(const std::deque<TElem> &List) const
	{
		if(List.front().Ident == CodeNumber)
			Elements().push_back(-boost::any_cast<long double>(List.front().Value));
		else
		{
			Elements().push_front(CodeNeg);
			Elements().insert(Elements().end(), List.begin(), List.end());
		}
	}
};
//---------------------------------------------------------------------------
struct TDoFuncSymbol
{
	TContext::member1 &Container;
	const TContext::member2 &Arg;

	TDoFuncSymbol(TContext::member1 &AContainer, const TContext::member2 &AArg)
		: Container(AContainer), Arg(AArg) {}
	void operator()(const wchar_t *Begin, const wchar_t*) const
	{
		TElem &Elem = Container().front();
		unsigned ActualArg = Arg();

		if(Elem.Ident == CodeCustom)
		{
			if(Elem.Arguments != ActualArg)
			 ThrowParseError(Begin, EParseError(ecArgCountError, -1, FunctionName(Elem)));
		}
		else
		{
			if(!ArgCountValid(Elem.Ident, ActualArg))
				ThrowParseError(Begin, EParseError(ecArgCountError, -1, FunctionName(Elem.Ident)));
			Elem.Arguments = ActualArg;
		}
	}
};
//---------------------------------------------------------------------------
//Policies for real parser. Exponential 'E' must be in upper case
template <typename T>
struct TFuncURealParserPolicies : public ureal_parser_policies<T>
{
	BOOST_STATIC_CONSTANT(bool, allow_trailing_dot = false);
	template <typename ScannerT>
	static typename parser_result<chlit<>, ScannerT>::type
	parse_exp(ScannerT& scan)
	{
		return ch_p('E').parse(scan);
	}
};
//---------------------------------------------------------------------------
//Parser used for parsing unsigned real numbers. Example: 5.457E-87
const real_parser<long double, TFuncURealParserPolicies<long double> >
	FuncUReal_p = real_parser<long double, TFuncURealParserPolicies<long double> >();

//Assertion: Throws an exception if the parser does not succed
const assertion<EParseError> AssertFactor_p(ecFactorExpected);
const assertion<EParseError> AssertExpression_p(ecExpressionExp);
const assertion<EParseError> AssertEndPar_p(ecNoEndPar);
const assertion<EParseError> AssertLiteral_p(ecLiteralExpected);

template <typename T, typename T2>
class TPushFront
{
	T &Container;
	const T2 &Data;
public:
	explicit TPushFront(T& AContainer, const T2 &AData) : Container(AContainer), Data(AData) {}

	template <typename T3>
	void operator()(const T3 &Val) const
	{
		Container().push_front(Data(Val));
	}

	void operator()(long double Value) const
	{
		Container().push_front(Value);
		if(!boost::math::isfinite(Value)) //Number is too large
			throw EParseError(ecInvalidNumber, 0);
	}
};

template <typename T, typename T2>
inline TPushFront<T, T2> PushFront(T& t, const T2 &t2)
{
	return TPushFront<T, T2>(t, t2);
}

template <typename T>
inline TPushFront<T, actor<argument<0> > > PushFront(T& t)
{
	return TPushFront<T, actor<argument<0> > >(t, arg1);
}

class TAssign
{
TContext::member1 &Container;
public:
TAssign(TContext::member1 &AContainer) : Container(AContainer) {}
	void operator()(const TElem &Elem) const
	{
		Container().clear();
		Container().push_back(Elem);
	}
};

class TPushBack
{
	TContext::member1 &Container;
public:
	TPushBack(TContext::member1 &AContainer) : Container(AContainer) {}
	void operator()(const std::deque<TElem> &List) const
	{
		Container().insert(Container().end(), List.begin(), List.end());
	}
};

class TConvertRelation
{
	TContext::member1 &Container;
public:
	TConvertRelation(TContext::member1 &AContainer) : Container(AContainer) {}
	void operator()(const TElem &Elem) const
	{
		Container().front().Ident = CodeCompare2;
		Container().front().Value =
			std::make_pair(boost::any_cast<TCompareMethod>(Container().front().Value),
			boost::any_cast<TCompareMethod>(Elem.Value));
	}
};

class TSpecialAddVar
{
	symbols<TElem, wchar_t> &Symbols;
	TContext::member1 &Container;
public:
	TSpecialAddVar(TContext::member1 &AContainer, symbols<TElem, wchar_t> &ASymbols)
		: Container(AContainer), Symbols(ASymbols) {}
	void operator()(const wchar_t *Begin, const wchar_t *End) const
	{
		boost::shared_ptr<long double> Ptr(new long double(0));
		std::wstring Str = ToLower(std::wstring(Begin, End));
		Symbols.add(Str.begin(), Str.end(), TElem(CodeConst, Ptr, Str));
		Container().front().Value = Ptr;
		Container().front().Text = Str;
	}
};

//Valid symbols in addition to 0..9, a..z and A..Z.
static const std::wstring ValidChars = L"+-*/^=_.,()[]{} \x2212\x03C0";
//---------------------------------------------------------------------------
/** Parse the string and store the result. The function is strong exception safe.
 *  \param Str: The string to parse
 *  \param Args: Argument names to the function
 *  \param SymbolList: List of global symbols
 *  \throw EParseError on parsing errors
 */
void TFuncData::Parse(const std::wstring &Str, const std::vector<std::wstring> &Args, const TSymbolList *SymbolList)
{
	TSymbols FuncSymbols = DefaultFuncSymbols;
	TSymbols NoCaseSymbols = DefaultNoCaseSymbols;
	TSymbols TempVariables;

	//Add arguments before symbol names, so they take precedens.
	for(unsigned I = 0; I < Args.size(); ++I)
		NoCaseSymbols.add(ToLower(Args[I]).c_str(), TElem(CodeArgument, I, 0));

	if(SymbolList)
	{ //WARNING: Do not remove {} (bcc 5.6.4 bug)
		for(TSymbolList::TConstIterator Iter = SymbolList->Begin(); Iter != SymbolList->End(); ++Iter)
			if(!Iter->second || Iter->second->ArgumentCount() == 0)
			{
				//Don't add a function/constant with same name as an argument
				if(find(NoCaseSymbols, Iter->first.c_str()) == NULL)
					NoCaseSymbols.add(Iter->first.c_str(), TElem(CodeCustom, Iter->first, 0, Iter->second));
			}
			else
				FuncSymbols.add(Iter->first.c_str(), TElem(CodeCustom, Iter->first, Iter->second->ArgumentCount(), Iter->second));
	}

	rule<wide_phrase_scanner_t, TContext::context_t> Term, Expression, Factor, Constant,
		Function, Parentheses, Power, FactorSeq, Sum, Neg, Relation, SpecialFunc;
	rule<wide_phrase_scanner_t, TContext2::context_t> Bracket;
	rule<wide_phrase_scanner_t> Literal, MinusSign, DummyExpression;

	//DummyExpression accepts everything until it finds a comma. It is used to jump
	//over the first expression in a special function like sum.
	DummyExpression = *(comment_nest_p('(', ')') | comment_nest_p('[', ']') | ~ch_p(','));

	Literal = alpha_p >> *alnum_p;
	MinusSign = '-' | ch_p(L'\x2212'); //0x2212 = Unicode symbol Minus

	//A constant may not be followed by a alpha-numeric character
	//A constant may not be followed a a parenthesis
	Constant =
			lexeme_d[(as_lower_d[
								TempVariables[TAssign(Constant.List)]
							| NoCaseSymbols[TAssign(Constant.List)]
							]
							| Symbols[TAssign(Constant.List)]
							) >> eps_p - (alnum_p | '_')] >> !(+ch_p('('))[TDoError(ecParAfterConst)]
      | ch_p(L'\x03C0')[TDoOperator(Constant.List, CodePi)];

	Bracket =	ch_p('(')[Bracket.EndBracket = ')'] |
						ch_p('[')[Bracket.EndBracket = ']'] |
						ch_p('{')[Bracket.EndBracket = '}'];

	//A function name may not be followd by character og digit, because they should be part of the name.
	//If an alphanumeric character is found afterwards, the pushed function is popped
	Function =
			lexeme_d[as_lower_d[FuncSymbols[PushFront(Function.List)][Function.Arg = 1] >> eps_p - (alnum_p | '_')]] >>
					(   Bracket[Function.EndBracket = arg1] >>
							Expression[TPushBack(Function.List)] >>
							*(',' >> AssertExpression_p(Expression)[TPushBack(Function.List)])[++Function.Arg] >>
							AssertEndPar_p(f_ch_p(Function.EndBracket))
					|   FactorSeq[TPushBack(Function.List)]
					)[TDoFuncSymbol(Function.List, Function.Arg)];

	SpecialFunc =
			SpecialFuncSymbols[PushFront(SpecialFunc.List)] >>
			Bracket[SpecialFunc.EndBracket = arg1] >>
					(
							((DummyExpression >> ',' >> Literal[SpecialFunc.Symbols = TempVariables][TSpecialAddVar(SpecialFunc.List, TempVariables)]) >> nothing_p)
					|   AssertExpression_p(Expression)[TPushBack(SpecialFunc.List)][var(TempVariables) = SpecialFunc.Symbols] >>
							',' >> Literal >> ',' >>
							AssertExpression_p(Expression[TPushBack(SpecialFunc.List)] >> ',') >>
							AssertExpression_p(Expression)[TPushBack(SpecialFunc.List)] >>
 							f_ch_p(SpecialFunc.EndBracket)
					);

	Parentheses = Bracket[Parentheses.EndBracket = arg1] >>
								AssertExpression_p(Expression)[Parentheses.List = arg1] >>
								AssertEndPar_p(f_ch_p(Parentheses.EndBracket));

	Relation =
			Sum[Relation.List = arg1] >>
							!(CompareSymbols[PushFront(Relation.List)] >> AssertExpression_p(Sum)[TPushBack(Relation.List)] >>
							!(CompareSymbols[TConvertRelation(Relation.List)] >> AssertExpression_p(Sum)[TPushBack(Relation.List)]))
					;

	Expression =
			Relation[Expression.List = arg1] >>
				 *(   lexeme_d[as_lower_d["and"] >> eps_p - (alnum_p | '_')] >> AssertExpression_p(Relation[TDoOperator(Expression.List, CodeAnd)])
					|   lexeme_d[as_lower_d["or"] >> eps_p - (alnum_p | '_')] >> AssertExpression_p(Relation[TDoOperator(Expression.List, CodeOr)])
					|   lexeme_d[as_lower_d["xor"] >> eps_p - (alnum_p | '_')] >> AssertExpression_p(Relation[TDoOperator(Expression.List, CodeXor)])
					) >> !Literal[TDoError(ecUnknownVar)];

  Sum =
      Term[Sum.List = arg1] >>
				 *(   '+' >> AssertFactor_p(Term)[TDoOperator(Sum.List, CodeAdd)]
					|   MinusSign >> AssertFactor_p(Term)[TDoOperator(Sum.List, CodeSub)]
					);

  FactorSeq = AssertFactor_p(Neg[FactorSeq.List = arg1] >> *Factor[TDoOperator(FactorSeq.List, CodeMul)]);

  Neg =   *ch_p('+') >>
          ( (MinusSign >> AssertFactor_p(Neg[TDoNegate(Neg.List)]))
            | Factor[Neg.List = arg1]
          );

	Term =
			Neg[Term.List = arg1] >>
				 *Factor[TDoOperator(Term.List, CodeMul)] >>
				 *(   ('*' >> FactorSeq[TDoOperator(Term.List, CodeMul)])
					|   ('/' >> FactorSeq[TDoOperator(Term.List, CodeDiv)])
          );

  Factor =
      (   SpecialFunc[Factor.List = arg1]
      |   Function[Factor.List = arg1]
      |   Constant[Factor.List = arg1]
			|   FuncUReal_p[PushFront(Factor.List)] >> !(+ch_p('.'))[TDoError(ecInvalidNumber)]
			|   Parentheses[Factor.List = arg1]
			)   >> !('^' >> FactorSeq[TDoOperator(Factor.List, CodePow)])
			;

  BOOST_SPIRIT_DEBUG_RULE(Expression);
  BOOST_SPIRIT_DEBUG_RULE(Relation);
	BOOST_SPIRIT_DEBUG_RULE(Term);
	BOOST_SPIRIT_DEBUG_RULE(Sum);
	BOOST_SPIRIT_DEBUG_RULE(FactorSeq);
	BOOST_SPIRIT_DEBUG_RULE(Factor);
	BOOST_SPIRIT_DEBUG_RULE(Parentheses);
	BOOST_SPIRIT_DEBUG_RULE(Power);
	BOOST_SPIRIT_DEBUG_RULE(Function);
	BOOST_SPIRIT_DEBUG_RULE(SpecialFunc);
	BOOST_SPIRIT_DEBUG_RULE(Constant);
	BOOST_SPIRIT_DEBUG_RULE(Neg);
	BOOST_SPIRIT_DEBUG_RULE(DummyExpression);
	BOOST_SPIRIT_DEBUG_RULE(MinusSign);
	BOOST_SPIRIT_DEBUG_RULE(Literal);
	BOOST_SPIRIT_DEBUG_RULE(Bracket);

	//Set pointers to start and end of string.
	//Ignore ending spaces as the parser doesn't seem to handle this.
	const wchar_t *Begin = &Str[0];
	const wchar_t *End = &Str[0] + Str.find_last_not_of(L" ") + 1;
	if(Begin == End)
		throw EParseError(ecEmptyString);

	try
	{
		//Parse expression and ignore spaces
		std::deque<TElem> Temp;
		parse_info<const wchar_t*> Info = parse(Begin, End, Expression[var(Temp) = arg1], space_p);
		DEBUG_LOG(std::clog.flush());
		std::vector<TElem> Temp2(Temp.begin(), Temp.end());

		if(!Info.full)
		{
			if(std::isalpha(*Info.stop))
			{
				const wchar_t *Ch;
				for(Ch = Info.stop; std::isalnum(*Ch) || *Ch == L'_'; ++Ch);
				throw EParseError(ecUnknownVar, Info.stop - Begin, std::wstring(Info.stop, Ch));
			}
			if(*Info.stop == ',')
				throw EParseError(ecCommaError, Info.stop - Begin);
			if(*Info.stop == 0)
				throw EParseError(ecUnexpectedEnd, Info.stop - Begin);
			if(!std::isalnum(*Info.stop) && ValidChars.find_first_of(*Info.stop) == std::wstring::npos)
				throw EParseError(ecUnknownChar, Info.stop - Begin, std::wstring(1, *Info.stop));
			if(std::wstring(L"+-/*^\x2212").find_first_of(*Info.stop) != std::string::npos)
				throw EParseError(ecOperatorError, Info.stop - Begin, std::wstring(1, *Info.stop));
			if(*Info.stop == ')')
				throw EParseError(ecInvalidEndPar, Info.stop - Begin);
			if(*Info.stop == '.' || std::isdigit(*Info.stop))
				throw EParseError(ecInvalidNumber, Info.stop - Begin);
			if(*Info.stop == '<' || *Info.stop == '>' || *Info.stop == '=')
				throw EParseError(ecInvalidCompare, Info.stop - Begin);
			throw EParseError(ecParseError, Info.stop - Begin);
		}

  //      DEBUG_LOG(std::clog << MakeText(Temp2.begin()) << std::endl);
    Data.swap(Temp2);
  }
  //Should not be necesarry. Bug in BCB6? Error on "sin ¤" is not caught without
	catch(parser_error<const EParseError, const wchar_t*> &E)
  {
    HandleParseError(E.descriptor, E.where, E.where - Begin);
  }
  catch(parser_error<EParseError, const wchar_t*> &E)
  {
		HandleParseError(E.descriptor, E.where, E.where - Begin);
  }
  catch(EFuncError &E)
  {
    throw;
  }
  catch(...)
  {
    throw EFuncError(ecInternalError);
  }
}
//---------------------------------------------------------------------------
void TFuncData::HandleParseError(const EParseError &E, const wchar_t* Where, unsigned Pos)
{
	if(E.ErrorCode != ecArgCountError && std::isalpha(*Where))
	{
		const wchar_t *Ch;
		for(Ch = Where; std::isalnum(*Ch); ++Ch);
		throw EParseError(ecUnknownVar, Pos, std::wstring(Where, Ch));
	}

//Convert to a comma error if the error was detected at a comma (eg. "(0,8)")
	if(*Where == ',')
		throw EParseError(ecCommaError, Pos);

	if(!std::isalnum(*Where) && ValidChars.find_first_of(*Where) == std::wstring::npos && *Where != L'\0')
		throw EParseError(ecUnknownChar, Pos, std::wstring(1, *Where));

	throw EParseError(E.ErrorCode, Pos, E.Str);
}
//---------------------------------------------------------------------------
} //namespace Func32


