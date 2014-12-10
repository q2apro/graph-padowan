/* Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
//WARNING: Compiler bug!
//This file must have "Expand inline functions" disabled under Debugging in the
//options, also for Release build. Else you get the linker error:
//[ILINK32 Error] Fatal: Illegal SEGMENT fixup index in module 'TFuncData.cpp'
//---------------------------------------------------------------------------
#include "Func32.h"
#include "Func32Impl.h"
#pragma hdrstop
#include <boost/spirit/include/phoenix1_operators.hpp>
#include <boost/spirit/include/phoenix1_primitives.hpp>
#include <boost/function.hpp>
#include <limits>
//---------------------------------------------------------------------------
namespace Func32
{
struct TFuncTable
{
	const wchar_t *FuncName;
	boost::function1<bool, unsigned> ArgCountValid;
	const wchar_t *Definition;
	const wchar_t *Dif;
	TFuncTable(const wchar_t *AFuncName, const boost::function1<bool, unsigned> &AArgCountValid, const wchar_t *ADefinition = NULL, const wchar_t *ADif = NULL)
		: FuncName(AFuncName), ArgCountValid(AArgCountValid), Definition(ADefinition), Dif(ADif) {}
};

using phoenix::arg1;
const boost::function1<bool, unsigned> Dummy;

//---------------------------------------------------------------------------
inline const TFuncTable& FuncTable(TIdent Ident)
{
static const TFuncTable Table[] = {
/*CodeSin*/         TFuncTable(L"sin",   arg1 == 1, NULL, L"cos(x)*dx"),
/*CodeCos*/         TFuncTable(L"cos",   arg1 == 1, NULL, L"-sin(x)*dx"),
/*CodeTan*/         TFuncTable(L"tan",   arg1 == 1, NULL, L"(1+tan(x)^2)*dx"),
/*CodeASin*/        TFuncTable(L"asin",  arg1 == 1, NULL, L"1/sqrt(1-x^2)*dx"),
/*CodeACos*/        TFuncTable(L"acos",  arg1 == 1, NULL, L"-1/sqrt(1-x^2)*dx"),
/*CodeATan*/        TFuncTable(L"atan",  arg1 == 1, NULL, L"1/(1+x^2)*dx"),

/*CodeLog*/         TFuncTable(L"log",   arg1 == 1, NULL, L"1/(x*ln(10))*dx"),
/*CodeLn*/          TFuncTable(L"ln",    arg1 == 1, NULL, L"1/x*dx"),
/*CodeExp*/         TFuncTable(L"exp",   arg1 == 1, NULL, L"exp(x)*dx"),
/*CodeSqr*/         TFuncTable(L"sqr",   arg1 == 1, NULL, L"2x*dx"),
/*CodeSqrt*/        TFuncTable(L"sqrt",  arg1 == 1, NULL, L"0.5/sqrt(x)*dx"),
/*CodeFact*/        TFuncTable(L"fact",  arg1 == 1),
/*CodeSign*/        TFuncTable(L"sign",  arg1 == 1),
/*CodeU*/           TFuncTable(L"u",     arg1 == 1),
/*CodeNeg*/         TFuncTable(L"",      Dummy,     NULL, L"-dx"), //Argument and name is not used
/*CodeSinh*/        TFuncTable(L"sinh",  arg1 == 1, NULL, L"cosh(x)*dx"),
/*CodeCosh*/        TFuncTable(L"cosh",  arg1 == 1, NULL, L"sinh(x)*dx"),
/*CodeTanh*/        TFuncTable(L"tanh",  arg1 == 1, NULL, L"1/cosh(x)^2*dx"),
/*CodeASinh*/       TFuncTable(L"asinh", arg1 == 1, NULL, L"1/sqrt(x^2+1)*dx"),
/*CodeACosh*/       TFuncTable(L"acosh", arg1 == 1, NULL, L"1/sqrt(x^2-1)*dx"),
/*CodeATanh*/       TFuncTable(L"atanh", arg1 == 1, NULL, L"1/(1-x^2)*dx"),
/*CodeAbs*/         TFuncTable(L"abs",   arg1 == 1, NULL, L"sign(x)*dx"),
/*CodeArg*/         TFuncTable(L"arg",   arg1 == 1),
/*CodeConj*/        TFuncTable(L"conj",  arg1 == 1),
/*CodeRe*/          TFuncTable(L"re",    arg1 == 1),
/*CodeIm*/          TFuncTable(L"im",    arg1 == 1),
/*CodeTrunc*/       TFuncTable(L"trunc", arg1 == 1),
/*CodeFract*/       TFuncTable(L"fract", arg1 == 1),
/*CodeCeil*/        TFuncTable(L"ceil",  arg1 == 1),
/*CodeFloor*/       TFuncTable(L"floor", arg1 == 1),
/*CodeNot*/         TFuncTable(L"not",   arg1 == 1),
/*CodeGamma*/       TFuncTable(L"gamma", arg1 == 1),
/*CodeW*/           TFuncTable(L"w",     arg1 == 1, NULL, L"W(x)/(x*(1+W(x)))*dx"),
/*CodeZeta*/        TFuncTable(L"zeta",  arg1 == 1),

/*CodeCsc*/         TFuncTable(L"csc",   arg1 == 1, L"1/sin(x)", L"-csc(x)*cot(x)*dx"),
/*CodeSec*/         TFuncTable(L"sec",   arg1 == 1, L"1/cos(x)", L"sec(x)*tan(x)*dx"),
/*CodeCot*/         TFuncTable(L"cot",   arg1 == 1, L"1/tan(x)", L"-csc(x)^2*dx"),
/*CodeACsc*/        TFuncTable(L"acsc",  arg1 == 1, L"asin(1/x)", L"-1/(abs(x)*sqrt(x^2-1))*dx"),
/*CodeASec*/        TFuncTable(L"asec",  arg1 == 1, L"acos(1/x)", L"1/(abs(x)*sqrt(x^2-1))*dx"),
/*CodeACot*/        TFuncTable(L"acot",  arg1 == 1, L"if(x, atan(1/x), acos(0))", L"-1/(1+x^2)*dx"),
/*CodeCsch*/        TFuncTable(L"csch",  arg1 == 1, L"1/sinh(x)", L"-csch(x)*coth(x)*dx"),
/*CodeSech*/        TFuncTable(L"sech",  arg1 == 1, L"1/cosh(x)", L"-sech(x)*tanh(x)*dx"),
/*CodeCoth*/        TFuncTable(L"coth",  arg1 == 1, L"1/tanh(x)", L"-csch(x)^2*dx"),
/*CodeACsch*/       TFuncTable(L"acsch", arg1 == 1, L"asinh(1/x)", L"-1/(abs(x)*sqrt(1+x^2))*dx"),
/*CodeASech*/       TFuncTable(L"asech", arg1 == 1, L"acosh(1/x)", L"-1/(abs(x)*sqrt(1-x^2))*dx"),
/*CodeACoth*/       TFuncTable(L"acoth", arg1 == 1, L"atanh(1/x)", L"1/(1-x^2)*dx"),

/*CodeAdd*/         TFuncTable(L"",      Dummy, NULL, L"dx+dx2"),
/*CodeSub*/         TFuncTable(L"",      Dummy, NULL, L"dx-dx2"),
/*CodeMul*/         TFuncTable(L"",      Dummy, NULL, L"dx*x2+x*dx2"),
/*CodeDiv*/         TFuncTable(L"",      Dummy, NULL, L"(dx*x2-x*dx2)/x2^2"),
/*CodePow*/         TFuncTable(L"",      Dummy, NULL, L"x2*x^(x2-1)*dx+x^x2*ln(x)*dx2"),
/*CodeCompare1*/    TFuncTable(L"",      Dummy),
/*CodeAnd*/         TFuncTable(L"",      Dummy),
/*CodeOr*/          TFuncTable(L"",      Dummy),
/*CodeXor*/         TFuncTable(L"",      Dummy),
/*CodeMod*/         TFuncTable(L"mod",   arg1 == 2),
/*CodeRound*/       TFuncTable(L"round", arg1 == 2),
/*CodeLogB*/        TFuncTable(L"logb",  arg1 == 2, NULL, L"dx/(ln(x2)*x) - (ln(x)*dx2)/(x2*ln(x2)^2)"),
/*CodeRoot*/        TFuncTable(L"root",  arg1 == 2, NULL, L"root(x, x2) * (dx2/(x2*x) - ln(x2)/x^2*dx)"),
/*CodeBeta*/        TFuncTable(L"beta",  arg1 == 2),

/*CodeRange*/       TFuncTable(L"range", arg1 == 3),
/*CodeIntegrate*/   TFuncTable(L"integrate", arg1 == 3),
/*CodeSum*/         TFuncTable(L"sum",   phoenix::val(false), NULL, L"sum(dx,Dummy,x2,x3)"),
/*CodeProduct*/     TFuncTable(L"product", phoenix::val(false)),
/*CodeCompare2*/    TFuncTable(L"",      arg1 == 3),
/*CodePowDiv*/      TFuncTable(L"",      Dummy, NULL, L"x2/x3*x^((x2-x3)/x3)*dx + x^(x2/x3)*ln(x)*(dx2*x3-x2*dx3)/x3^2"),

/*CodeMin*/         TFuncTable(L"min",   arg1 >= 2),
/*CodeMax*/         TFuncTable(L"max",   arg1 >= 2),
/*CodeIf*/          TFuncTable(L"if",    arg1 >= 2),
/*CodeIfSeq*/       TFuncTable(L"ifseq", arg1 >= 2),
/*CodeCustom*/      TFuncTable(L"",      Dummy),
/*CodeDNorm*/       TFuncTable(L"dnorm", arg1 == 1 || arg1 == 3, L"exp(-sqr(x-x2)/(2sqr(x3))) / (x3*sqrt(2pi))"),
};

	assert(Ident >= FirstFunction1P && Ident <= LastFunction);
	return Table[Ident - FirstFunction1P];
}
//---------------------------------------------------------------------------
/** Return name of function corresponding to the given ident.
 *  Ident: Id to lookup.
 */
const wchar_t* FunctionName(TIdent Ident)
{
	return FuncTable(Ident).FuncName;
}
//---------------------------------------------------------------------------
/** Return name of function/constant in the element.
 *  \param Elem: Element to find name for.
 */
const wchar_t* FunctionName(const TElem &Elem)
{
	if(Elem.Ident == CodeCustom)
		return Elem.Text.c_str();
	return FunctionName(Elem.Ident);
}
//---------------------------------------------------------------------------
/** Return definition of function given by Ident. Might return NULL.
 *  \param Ident: Function to return definition for.
 */
const wchar_t* FunctionDefinition(TIdent Ident)
{
	return FuncTable(Ident).Definition;
}
//---------------------------------------------------------------------------
/** Return differentiation of function given by Ident. The differentiation is done at the first call.
 *  \param Ident: Identifies the function to return the first derivative for.
 */
const TFuncData& GetDif(TIdent Ident)
{
	static std::vector<TFuncData> Table;
	assert(Ident >= FirstFunction1P && Ident <= LastFunction);
	if(Table.empty())
	{
		TSymbolList SymbolList;
		SymbolList.Add(L"dx");
		SymbolList.Add(L"x2");
		SymbolList.Add(L"dx2");
		SymbolList.Add(L"x3");
		SymbolList.Add(L"dx3");

		Table.reserve(LastFunction-FirstFunction1P+1);
		std::vector<std::wstring> Variable(1, L"x");

		for(unsigned I = FirstFunction1P; I <= LastFunction; I++)
			if(FuncTable(static_cast<TIdent>(I)).Dif)
				Table.push_back(TFuncData(FuncTable(static_cast<TIdent>(I)).Dif, Variable, SymbolList));
			else
        Table.push_back(TFuncData());
  }
	return Table[Ident - FirstFunction1P];
}

//---------------------------------------------------------------------------
/** Verifies if the number of arguments is valid for the given function.
 *  \param Ident: Function to validate argument count for.
 *  \param Args: Argument count to verify.
 */
bool ArgCountValid(TIdent Ident, unsigned Args)
{
  if(Ident < FirstFunction1P)
    return Args == 0;
  return FuncTable(Ident).ArgCountValid(Args);
}
//---------------------------------------------------------------------------
bool TElem::operator ==(const TElem &E) const
{
	if(Ident != E.Ident)
		return false;
	switch(Ident)
	{
		case CodeNumber:
			return IsEqual(boost::any_cast<long double>(Value), boost::any_cast<long double>(E.Value));

		case CodeCustom:
			return Text == E.Text;

		case CodeConst:
			return *boost::any_cast<boost::shared_ptr<long double> >(Value) ==
				*boost::any_cast<boost::shared_ptr<long double> >(E.Value);
		default:
			return Arguments == E.Arguments;
	}
}
//---------------------------------------------------------------------------
/** Copy another function into List and replaces all arguments with copies of custom functions.
 *  This is used as part of differentiation.
 *  \param List: Data vector to copy elements into.
 *  \param Iter: Iterator pointing to first element in function to copy
 *  \param Args: Value of arguments. Arg0 is replaced by Args[0], etc.
 *  \todo Throw exception on recursive functions
 */
void TFuncData::CopyReplaceArgs(std::vector<TElem> &List, TConstIterator Iter, const std::vector<std::vector<TElem> > &Args)
{
	DEBUG_LOG(std::wclog << L"CopyReplaceArgs: " << MakeText(Iter));
	DEBUG_LOG(for(unsigned I = 0; I < Args.size(); I++) std::wclog << L";   Arg" << I << L"=" << MakeText(Args[I].begin()));
	DEBUG_LOG(std::wclog << std::endl);

	const static std::vector<std::vector<TElem> > Dummy;
	TConstIterator End = FindEnd(Iter);
	for(; Iter != End; ++Iter)
		if(Iter->Ident == CodeCustom && Iter->Arguments > 0)
		{
			std::vector<std::vector<TElem> > NewArgs(Iter->Arguments);
			TConstIterator Iter2 = Iter + 1;
			for(unsigned I = 0; I < NewArgs.size(); I++)
      {
				CopyReplaceArgs(NewArgs[I], Iter2, Args);
        Iter2 = FindEnd(Iter2);
      }
      boost::shared_ptr<TBaseCustomFunc> Func = boost::any_cast<boost::shared_ptr<TBaseCustomFunc> >(Iter->Value);
			CopyReplaceArgs(List, Func->GetFuncData()->Data.begin(), NewArgs);
      Iter = Iter2 - 1;
    }
		else if(Iter->Ident == CodeArgument && !Args.empty())
      CopyReplaceArgs(List, Args[Iter->Arguments].begin(), Dummy);
		else
			List.push_back(*Iter);
}
//---------------------------------------------------------------------------
/** Copy another function into List and replaces all arguments that matches Elem
 *	with the elements in the range [First;Last[
 *  This is used as part of differentiation.
 *  \param List: Data vector to copy elements into.
 *	\param Iter: Points to the first part of the function to copy.
 *	\param Elem: The element to look for.
 *  \param First: Start of the range to replace Elem with.
 *  \param Last: One after the last element top replace Elem with.
 */
void TFuncData::CopyReplace(std::vector<TElem> &List, TConstIterator Iter, const TElem &Elem, TConstIterator First, TConstIterator Last)
{
//	DEBUG_LOG(std::wclog << L"CopyReplace: " << MakeText(Iter));
//	DEBUG_LOG(for(unsigned I = 0; I < Args.size(); I++) std::wclog << L";   Arg" << I << L"=" << MakeText(Args[I].begin()));
//	DEBUG_LOG(std::wclog << std::endl);

	TConstIterator End = FindEnd(Iter);
	for(; Iter != End; ++Iter)
		if(*Iter == Elem)
		{
			List.insert(List.end(), First, Last);
		}
		else
      List.push_back(*Iter);
}
//---------------------------------------------------------------------------
/** Replace all occurences of OldElem with NewElem.
 *  \param OldElem: Elements to replace.
 *  \param NewElem: Element to replace with.
 */
void TFuncData::Replace(const TElem &OldElem, const TElem &NewElem)
{
	std::replace(Data.begin(), Data.end(), OldElem, NewElem);
}
//---------------------------------------------------------------------------
/** Update data to use functions/constants in a new symbol list. This will loop through all the elements, and replace all
 *  custom functions/constants with ones with the same name in the symbol list.
 *  \param SymbolList: List of symbols to replace exiting ones with.
 */
bool TFuncData::Update(const TSymbolList &SymbolList)
{
  bool Result = true;
  for(std::vector<TElem>::iterator Iter = Data.begin(); Iter != Data.end(); ++Iter)
    if(Iter->Ident == CodeCustom)
    {
      boost::shared_ptr<TBaseCustomFunc> Func = SymbolList.Get(Iter->Text);
      if(!Func || Iter->Arguments != Func->ArgumentCount()) //The number of arguments must match
      {
        Result = false;
        Iter->Value = boost::shared_ptr<TBaseCustomFunc>();
      }
      else
        Iter->Value = Func;
    }
	return Result;
}
//---------------------------------------------------------------------------
/** Add a copy of a TFuncData object to the end of this object.
 *  \param FuncData: Data to add.
 *  \throw EFuncError: Thrown if FuncData is empty.
 */
void TFuncData::Add(const TFuncData &FuncData)
{
  if(FuncData.IsEmpty())
    throw EFuncError(ecNoFunc);
  Data.insert(Data.end(), FuncData.Data.begin(), FuncData.Data.end());
}
//---------------------------------------------------------------------------
bool TFuncData::CheckRecursive() const
{
  std::vector<const TFuncData*> FuncStack;
  FuncStack.push_back(this);
  return CheckRecursive(FuncStack);
}
//---------------------------------------------------------------------------
bool TFuncData::CheckRecursive(std::vector<const TFuncData*> &FuncStack) const
{
  for(std::vector<TElem>::const_iterator Iter = Data.begin(); Iter != Data.end(); ++Iter)
    if(Iter->Ident == CodeCustom)
      try
      {
        boost::shared_ptr<TBaseCustomFunc> Func = boost::any_cast<boost::shared_ptr<TBaseCustomFunc> >(Iter->Value);
        if(std::find(FuncStack.begin(), FuncStack.end(), Func->GetFuncData().get()) != FuncStack.end())
          return true;

        FuncStack.push_back(Func->GetFuncData().get());
        if(Func->GetFuncData()->CheckRecursive(FuncStack))
          return true;
        FuncStack.pop_back();
      }
      catch(EFuncError &E)
      {
        E.Str = FunctionName(*Iter);
        throw;
      }

  return false;
}
//---------------------------------------------------------------------------
/** Check if a symbol name is referenced directly or indirectly.
  * \param SymbolName: Name of symbol to check for. SymbolName must be in lower case.
  */
bool TFuncData::IsDependent(const std::wstring &SymbolName) const
{
	TConstIterator End = Data.end();
	for(TConstIterator Iter = Data.begin(); Iter != End; ++Iter)
    if(Iter->Ident == CodeCustom)
    {
      if(Iter->Text == SymbolName)
        return true;
      const boost::shared_ptr<TBaseCustomFunc> &Func = *boost::unsafe_any_cast<boost::shared_ptr<TBaseCustomFunc> >(&Iter->Value);
      if(Func)
        if(Func->IsDependent(SymbolName))
          return true;
    }
  return false;
}
//---------------------------------------------------------------------------
} //namespace Func32

