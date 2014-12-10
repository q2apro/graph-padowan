/* Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Func32.h"
#include "Func32Impl.h"
#pragma hdrstop
//---------------------------------------------------------------------------
namespace Func32
{
const unsigned MaxDifLevel = 10; //The maximum number of call levels allowed to avoid recursive functions

/** Diferentiate data and return the result in a new object.
 *  WARNING: Do not call for recursive functions as it will crash with a stack overflow
 *  \param Var: Variable to differenciate with respect to
 *  \param Trigonometry: Choose to differentiate trigonometric functions as radians or degrees.
 */
boost::shared_ptr<TFuncData> TFuncData::MakeDif(const TElem &Var, TTrigonometry Trigonometry) const
{
  if(Data.empty())
    throw EFuncError(ecNoFunc);

  if(CheckRecursive())
    throw EFuncError(ecRecursiveDif);

  try
  {
    boost::shared_ptr<TFuncData> Temp(new TFuncData);
		CopyReplaceArgs(Temp->Data, Data.begin(), std::vector<std::vector<TElem> >());
    DEBUG_LOG(std::wclog << L"f(x)=" << MakeText(Temp->Data.begin()) << std::endl);
    boost::shared_ptr<TFuncData> Dest(new TFuncData);
    Dest->AddDif(Temp->Data.begin(), Var, Trigonometry, 0);

    //It is sometimes necesarry to optimize. For example d(x^2) needs an ln(x) optimized away
    DEBUG_LOG(std::wclog << L"Before simplify: f'(x)=" << MakeText(Dest->Data.begin()) << std::endl);
    Dest->Simplify();
    DEBUG_LOG(std::wclog << L"After simplify: f'(x)=" << MakeText(Dest->Data.begin()) << std::endl);
		Dest->Simplify();
    DEBUG_LOG(std::wclog << L"After simplify: f'(x)=" << MakeText(Dest->Data.begin()) << std::endl);
    return Dest;
  }
  catch(boost::bad_any_cast &E)
  {
    throw EFuncError(ecInternalError);
  }
}
//---------------------------------------------------------------------------
/** Returns an iterator to the element following Iter. Jumps over parameters.
 *  \param Iter: Iterator to find next element for.
 */
std::vector<TElem>::const_iterator FindEnd(std::vector<TElem>::const_iterator Iter)
{
	unsigned Arguments = FunctionArguments(*Iter);
	++Iter;
	for(unsigned I = 0; I < Arguments; I++)
		Iter = FindEnd(Iter);
	return Iter;
}
//---------------------------------------------------------------------------
/** Check if one of the parameters pointed to by Iter contains Elem.
 *  \param Iter: Iterator to function to check.
 *	\param Elem: The element to search for.
 */
bool ContainsElem(std::vector<TElem>::const_iterator Iter, const TElem &Elem)
{
	std::vector<TElem>::const_iterator End = FindEnd(Iter);
	for(;Iter < End; ++Iter)
		if(*Iter == Elem)
			return true;
	return false;
}
//---------------------------------------------------------------------------
/** Saves the first derivative of the sequence starting at Iter in the back *this
 *  \param Iter: Iterator to function to differentiate.
 *  \param Var: Variable to differentiate with respect to.
 *  \param Trigonemetry: Differentiate trigonometric functions as radians og degrees.
 *  \param Level: Indicates the number os times the function has been called recursive. To prevent infinite loops.
 *  \throw EFuncError: Thrown if differentiation fails.
 */
void TFuncData::AddDif(TConstIterator Iter, const TElem &Var, TTrigonometry Trigonometry, unsigned Level)
{
  if(*Iter == Var)
    Data.push_back(TElem(CodeNumber, 1));
  else if(Iter->Ident == CodeRand)
		throw EFuncError(ecNotDifAble, L"rand");
  else if(IsConstant(*Iter))
    Data.push_back(TElem(CodeNumber, 0.0));
	else
	{
		if(!ContainsElem(Iter, Var))
		{
			Data.push_back(TElem(CodeNumber, 0.0));
			return;
		}
		switch(Iter->Ident)
		{
			case CodeIf:
			case CodeIfSeq:
			{
				//f(x)=if(a1,b1,a2,b2, ... , an,bn [,c])
				//f'(x)=if(a1,b1',a2,b2', ... , an, bn' [,c'])
				Data.push_back(*Iter); //CodeIf with same number of arguments
				unsigned Arguments = FunctionArguments(*Iter);
				++Iter;
				for(unsigned I = 0; I < Arguments-1; I++)
				{
					TConstIterator End = FindEnd(Iter);
					if(I % 2)
						AddDif(Iter, Var, Trigonometry, Level);
					else
						Data.insert(Data.end(), Iter, End);
					Iter = End;
				}
				AddDif(Iter, Var, Trigonometry, Level);
				break;
			}

			case CodeMin:
			case CodeMax:
			{
				//f(x)=min(a1,a2,a3, ... , an) f'(x)=if(a1<a2 and a1<a3 ...,a1', a2<a1 and a2<a3 ...,a2',
				unsigned Arguments = Iter->Arguments;
				Data.push_back(TElem(CodeIf, 2*Arguments-1, 0));
				TConstIterator Param = Iter + 1;
				for(unsigned I = 0; I < Arguments-1; I++)
				{
					TConstIterator End = FindEnd(Param);
					for(unsigned J = 0; J < Arguments-2; J++)
						Data.push_back(CodeAnd);

					TConstIterator Param2 = Iter+1;
					for(unsigned J = 0; J < Arguments; J++)
					{
						if(J != I)
						{
							Data.push_back(TElem(Iter->Ident == CodeMin ? cmLess : cmGreater));
							Data.insert(Data.end(), Param, End);
							Data.insert(Data.end(), Param2, FindEnd(Param2));
						}
						Param2 = FindEnd(Param2);
					}

					AddDif(Param, Var, Trigonometry, Level);
					Param = End;
				}
				AddDif(Param, Var, Trigonometry, Level);
				break;
			}

			case CodeCustom:
			{
				if(Level > MaxDifLevel)
					throw EFuncError(ecRecursiveDif);
				boost::shared_ptr<TBaseCustomFunc> Func = boost::any_cast<boost::shared_ptr<TBaseCustomFunc> >(Iter->Value);
				if(Func)
					AddDif(Func->GetFuncData()->Data.begin(), Var, Trigonometry, Level + 1);
				else
					throw EFuncError(ecSymbolNotFound, Iter->Text);
				break;
			}
			case CodeDNorm:
			{
				std::vector<std::wstring> ArgNames;
				ArgNames.push_back(L"x");
				ArgNames.push_back(L"x2");
				ArgNames.push_back(L"x3");
				TFuncData Temp(FunctionDefinition(CodeDNorm), ArgNames);
				TFuncData Temp2;
				std::vector<std::vector<TElem> > Args(3);
				CopyReplaceArgs(Args.front(), Iter + 1, std::vector<std::vector<TElem> >());
				if(Iter->Arguments > 2)
				{
					TConstIterator Iter2 = FindEnd(Iter + 1);
					CopyReplaceArgs(Args[1], Iter2, std::vector<std::vector<TElem> >());
					CopyReplaceArgs(Args[2], FindEnd(Iter2), std::vector<std::vector<TElem> >());
				}
				else
				{
					Args[1].push_back(TElem(CodeNumber, 0.0));
					Args[2].push_back(TElem(CodeNumber, 1.0));
				}
				CopyReplaceArgs(Temp2.Data, Temp.Data.begin(), Args);
				AddDif(Temp2.Data.begin(), Var, Trigonometry, Level + 1);
				break;
			}

			case CodeMod:
				if(ContainsElem(FindEnd(Iter+1), Var))
					throw EFuncError(ecNotDifAble, FunctionName(CodeMod));
        AddDif(Iter+1, Var, Trigonometry, Level + 1);
				break;

			default:
			{
				if(Trigonometry == Degree)
				{
					//Sin, Cos, Tan, Csc, Sec, Cot must be multiplied with PI/180 when differentiated using degrees
					if((Iter->Ident >= CodeSin && Iter->Ident <= CodeTan) || (Iter->Ident >= CodeCsc && Iter->Ident <= CodeCot))
					{
						Data.push_back(CodeMul);
						Data.push_back(CodeDiv);
						Data.push_back(CodePi);
						Data.push_back(180);
					}
					//ASin, ACos, ATan, ACsc, ASec, ACot must be multiplied with 180/PI when differentiated using degrees
					else if((Iter->Ident >= CodeASin && Iter->Ident <= CodeATan) || (Iter->Ident >= CodeACsc && Iter->Ident <= CodeACot))
					{
						Data.push_back(CodeMul);
						Data.push_back(CodeDiv);
						Data.push_back(180);
						Data.push_back(CodePi);
					}
				}

				const TFuncData &DifData = GetDif(Iter->Ident);
				if(Iter->Ident == CodeIntegrate)
				{
					//f(x)=integrate(g(x,s), s, a(x), b(x))
					//f'(x)=g(x,b(x))*db(x)/dx - g(x,a(x))*da(x)/dx + integrate(dg(x,s)/dx, s, a(x), b(x))
					TConstIterator From = FindEnd(Iter + 1);
					TConstIterator To = FindEnd(From);
					TConstIterator End = FindEnd(To);
					TElem Elem = *Iter;
					Elem.Ident = CodeConst;
					if(!Iter->Text.empty())
						Data.push_back(CodeAdd);
					Data.push_back(CodeSub);
					Data.push_back(CodeMul);
					CopyReplace(Data, Iter + 1, Elem, To, End);
					AddDif(To, Var, Trigonometry, Level);

					Data.push_back(CodeMul);
					CopyReplace(Data, Iter + 1, Elem, From, To);
					AddDif(From, Var, Trigonometry, Level);
					if(!Iter->Text.empty())
					{ //Backward compatibility:
						//f(x)=integrate(g(x), a(x), b(x))
						//f'(x)=g(x,b(x))*db(x)/dx - g(x,a(x))*da(x)/dx
						Data.push_back(*Iter);
						AddDif(Iter + 1, Var, Trigonometry, Level);
						Data.insert(Data.end(), From, End);
          }
				}
				else if(DifData.IsEmpty())
					throw EFuncError(ecNotDifAble, FunctionName(*Iter));

				TConstIterator End = DifData.Data.end();
				TConstIterator FirstPar = Iter;             //Start of first parenthesis
				++FirstPar;
				TConstIterator SecondPar = FindEnd(FirstPar); //Start of second parenthesis
				TConstIterator ThirdPar;
				if(FunctionArguments(*Iter) >= 2)
					ThirdPar = FindEnd(SecondPar); //Start of third parenthesis

				TConstIterator Begin = DifData.Data.begin();
				if(Iter->Ident == CodeSum) //Use the original CodeSum instead of the one from DifData
				{
					Data.push_back(*Iter);
					Begin++;
				}

				for(TConstIterator Elem = Begin; Elem != End; ++Elem)
					if(Elem->Ident == CodeArgument)
						Data.insert(Data.end(), FirstPar, SecondPar);
					else if(*Elem == TElem(CodeCustom, L"dx"))
						AddDif(FirstPar, Var, Trigonometry, Level);
					else if(*Elem == TElem(CodeCustom, L"x2"))
						Data.insert(Data.end(), SecondPar, ThirdPar);
					else if(*Elem == TElem(CodeCustom, L"dx2"))
						AddDif(SecondPar, Var, Trigonometry, Level);
					else if(*Elem == TElem(CodeCustom, L"x3"))
						Data.insert(Data.end(), ThirdPar, FindEnd(ThirdPar));
					else if(*Elem == TElem(CodeCustom, L"dx3"))
						AddDif(ThirdPar, Var, Trigonometry, Level);
					else
						Data.push_back(*Elem);
				}
		}
	}
}
//---------------------------------------------------------------------------
} //namespace Func32












