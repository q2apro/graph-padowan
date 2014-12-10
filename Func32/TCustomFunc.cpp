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
#include <iomanip>

namespace Func32
{
//---------------------------------------------------------------------------
TComplex TBaseCustomFunc::DynCall(TDynData<TComplex> &DynData) const
{
	return Call(DynData.Args, DynData.Trigonometry, DynData.Error.ErrorCode, DynData.Error.Str);
}
//---------------------------------------------------------------------------
long double TBaseCustomFunc::DynCall(TDynData<long double> &DynData) const
{
	return Call(DynData.Args, DynData.Trigonometry, DynData.Error.ErrorCode, DynData.Error.Str);
}
//---------------------------------------------------------------------------
/** Default constructor to create an empty custom function
 */
TCustomFunc::TCustomFunc() : FuncData(new TFuncData)
{
}
//---------------------------------------------------------------------------
/** Constructor to create a custom function from a string and named arguments.
 *  \param Text: The string to containing the function, for example "a*sin(b)"
 *  \param AArgs: A vector for strings with the argument names, for example {"a", "b"}
 *  \param Trig: Indicates weither trigonometric functions uses radians or degrees. Defaults to Radian.
 *  \throw EParseError if parsing fails
 */
TCustomFunc::TCustomFunc(const std::wstring &Text, const std::vector<std::wstring> &AArgs, TTrigonometry Trig)
  : FuncData(new TFuncData(Text, AArgs)), Args(AArgs), Trigonometry(Trig)
{
}
//---------------------------------------------------------------------------
/** Constructor to create a custom function from a string with arguments and custom functions/constants.
 *  \param Text: The string to containing the function, for example "a*sinc(b)"
 *  \param AArgs: A vector for strings with the argument names, for example {"a", "b"}
 *  \param SymbolList: A Symbol list with custom functions, for example the "sinc" function.
 *  \param Trig: Indicates weither trigonometric functions uses radians or degrees.
 *  \throw EParseError if parsing fails
 */
TCustomFunc::TCustomFunc(const std::wstring &Text, const TArgType &AArgs, const TSymbolList &SymbolList, TTrigonometry Trig)
  : FuncData(new TFuncData(Text, AArgs, SymbolList)), Args(AArgs), Trigonometry(Trig)
{
}
//---------------------------------------------------------------------------
/** Constructor creating a custom function from a value.
 *  \param Value: A numeric value to create function from.
 */
TCustomFunc::TCustomFunc(long double Value)
  : FuncData(new TFuncData), Trigonometry(Radian)
{
  FuncData->Add(Value);
}
//---------------------------------------------------------------------------
/** Constructor creating a custom function from a complex value.
 *  \param Value: A complex number to create function from.
 */
TCustomFunc::TCustomFunc(const TComplex &Complex)
  : FuncData(new TFuncData), Trigonometry(Radian)
{
  SetFunc(Complex);
}
//---------------------------------------------------------------------------
/** Assigns new data to the function.
 *  \param Text: The string to containing the function, for example "a*sin(b)"
 *  \param AArgs: A vector for strings with the argument names, for example {"a", "b"}
 *  \throw EParseError if parsing fails
 */
void TCustomFunc::SetFunc(const std::wstring &Text, const std::vector<std::wstring> &AArgs)
{
  std::vector<std::wstring> Temp(AArgs); // To make the function strong exception safe
  FuncData.reset(new TFuncData(Text, AArgs));
  Args.swap(Temp);
}
//---------------------------------------------------------------------------
/** Assigns new data to the function.
 *  \param Text: The string to containing the function, for example "a*sin(b)"
 *  \param AArgs: A vector for strings with the argument names, for example {"a", "b"}
 *  \param SymbolList: A Symbol list with custom functions, for example the "sinc" function.
 *  \throw EParseError if parsing fails
 */
void TCustomFunc::SetFunc(const std::wstring &Text, const TArgType &AArgs, const TSymbolList &SymbolList)
{
  std::vector<std::wstring> Temp(AArgs); // To make the function strong exception safe
  FuncData.reset(new TFuncData(Text, AArgs, SymbolList));
  Args.swap(Temp);
}
//---------------------------------------------------------------------------
/** Assigns a value to the function.
 *  \param Values: The value assigned to the function
 */
void TCustomFunc::SetFunc(long double Value)
{
  Unique();
  FuncData->Clear();
  FuncData->Add(Value);
}
//---------------------------------------------------------------------------
/** Assigns a value to the function.
 *  \param Values: The value assigned to the function
 */
void TCustomFunc::SetFunc(const TComplex &Complex)
{
  Unique();
  FuncData->Clear();
  if(imag(Complex))
  {
    FuncData->Add(CodeAdd);
    FuncData->Add(CodeMul);
    FuncData->Add(imag(Complex));
    FuncData->Add(Codei);
  }
  FuncData->Add(real(Complex));
}
//---------------------------------------------------------------------------
/** Evaluates the function for a given set of arguments.
 *  \param Values: Arguments to pass into function. Values.size() must equal Args.size()
 *  \return Result of the evaluation
 *  \throw EFuncError if invalid number of arguments are passed.
 *  \throw ECalcError if calculation fails
 */
long double TCustomFunc::Calc(const std::vector<long double> &Values) const
{
  if(Values.size() != Args.size())
    throw EFuncError(ecInvalidArgCount);
  return FuncData->Calc(Values.empty() ? NULL : &Values[0], Trigonometry);
}
//---------------------------------------------------------------------------
/** Evaluates the function for a given set of arguments.
 *  \param Values: Arguments to pass into function. Values.size() must equal Args.size()
 *  \param E: Filled with error information on return.
 *  \return Result of the evaluation. Invalid on errors.
 */
long double TCustomFunc::Calc(const std::vector<long double> &Values, ECalcError &E) const
{
  if(Values.size() != Args.size())
  {
    E.ErrorCode = ecInvalidArgCount;
    return 0;
  }
  return FuncData->Calc(&Values[0], Trigonometry, E);
}
//---------------------------------------------------------------------------
/** Evaluates the function for a given set of arguments.
 *  \param Values: Arguments to pass into function. Values.size() must equal Args.size()
 *  \return Result of the evaluation
 *  \throw EFuncError if invalid number of arguments are passed.
 *  \throw ECalcError if calculation fails
 */
TComplex TCustomFunc::Calc(const std::vector<TComplex> &Values) const
{
  if(Values.size() != Args.size())
    throw EFuncError(ecInvalidArgCount);
  return FuncData->Calc(Values.empty() ? NULL : &Values[0], Trigonometry);
}
//---------------------------------------------------------------------------
/** Evaluates the function for a given set of arguments.
 *  \param Values: Arguments to pass into function. Values.size() must equal Args.size()
 *  \param E: Filled with error information on return.
 *  \return Result of the evaluation. Invalid on errors.
 */
TComplex TCustomFunc::Calc(const std::vector<TComplex> &Values, ECalcError &E) const
{
  if(Values.size() != Args.size())
  {
    E.ErrorCode = ecInvalidArgCount;
    return 0;
  }
  return FuncData->Calc(&Values[0], Trigonometry, E);
}
//---------------------------------------------------------------------------
/** Differentiate the function with the given argument. For example the function f(a,b,c)=a+b*sin(c) differentiated with
 *  argument 2 will be df/dc = b*cos(c)
 *  \param Arg: Argument to differentiate with.
 *  \return A new custom function created from differentiating *this.
 *  \throw EFuncError on any error differentiating.
 */
TCustomFunc TCustomFunc::MakeDif(unsigned Arg) const
{
  return TCustomFunc(FuncData->MakeDif(TElem(CodeArgument, Arg, 0), Trigonometry), Args, Trigonometry);
}
//---------------------------------------------------------------------------
/** Makes the objects data unique. The reference count of FuncData will be 1.
 */
void TCustomFunc::Unique()
{
  if(!FuncData.unique())
    FuncData.reset(new TFuncData(*FuncData));
}
//---------------------------------------------------------------------------
/** Converts the custom function to a standard function (TFunc). The argument indicated by the number in Variable will be
 *  replaced with the variable in the result, while Values will replace the rest of the arguments.
 *  For example the function f(a,b,c)=a+b*sin(c) with Values={5,10} and Variabel=2 is convereted to f(x)=5+10*sin(x)
 *  \param Values: Vector of numbers to replace the arguments. Values.size() must equal Args.size()-1.
 *  \param Variable: The argument to convert to the variable. Requirement: Variable < Args.size().
 *  \throw EFuncError on any error.
 */
TFunc TCustomFunc::ConvToFunc(const std::vector<long double> &Values, unsigned Variable) const
{
  if(Values.size() != Args.size() - 1)
    throw EFuncError(ecInvalidArgCount);
  if(Variable >= Args.size())
    throw EFuncError(ecInvalidValue);
  boost::shared_ptr<TFuncData> TempData(new TFuncData(*FuncData));
  std::vector<long double>::const_iterator Iter = Values.begin();
  for(unsigned I = 0; I < Args.size(); I++)
    if(I != Variable)
      TempData->Replace(TElem(CodeArgument, I, 0), TElem(CodeNumber, *Iter++));
  return TFunc(TempData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Converts the function to a text string.
 *  \param Decimals: Number of decimals in numbers.
 *  \return A text string, for example "10*sinc(a)+b")
 */
std::wstring TCustomFunc::MakeText(unsigned Decimals) const
{
  std::wostringstream Stream;
  FuncData->MakeText(Args, Stream, Decimals);
  return Stream.str();
}
//---------------------------------------------------------------------------
/** Update the function from the Symbol list.
 *  \param SymbolList: List of custom functions and constants.
 */
bool TCustomFunc::Update(const TSymbolList &SymbolList)
{
  Unique();
  return FuncData->Update(SymbolList);
}
//---------------------------------------------------------------------------
/** Indicates if the function is empty, an equation, an inequality or just a function.
 */
TFunctionType TCustomFunc::GetFunctionType() const
{
  if(FuncData->IsEmpty())
    return ftEmpty;
  TIdent Ident = FuncData->Front().Ident;
  if(Ident == CodeCompare1)
    return boost::any_cast<TCompareMethod>(FuncData->Front().Value) == cmEqual ? ftEquation : ftInequality;
  if(Ident == CodeCompare2 || Ident == CodeAnd || Ident == CodeOr || Ident == CodeXor)
    return ftInequality;
  return ftFunction;
}
//---------------------------------------------------------------------------
/** If the function is a relation it will be converted to a function with the right side subtracted from the left side.
 *  For example "x=y" will be converted to "x-y"
 */
void TCustomFunc::RemoveRelation()
{
  if(FuncData->IsEmpty())
    throw EFuncError(ecNoFunc);

  TIdent Ident = FuncData->Front().Ident;
  if(Ident == CodeCompare1)
    FuncData->Front() = TElem(CodeSub);
}
//---------------------------------------------------------------------------
/** Returns true if the function is empty.
 */
bool TCustomFunc::IsEmpty() const
{
  return FuncData->IsEmpty();
}
//---------------------------------------------------------------------------
/** Interface used when the custom function is called internal.
 */
long double TCustomFunc::DynCall(class TDynData<long double> &DynData) const
{
  return FuncData->CalcF(DynData);
}
//---------------------------------------------------------------------------
/** Interface used when the custom function is called internal.
 */
TComplex TCustomFunc::DynCall(TDynData<TComplex> &DynData) const
{
  return FuncData->CalcF(DynData);
}
//---------------------------------------------------------------------------
/** Returns true if the custom function dependens on a given symbol name.
 *  \param SymbolName: Name of symbol to check for.
 */
bool TCustomFunc::IsDependent(const std::wstring &SymbolName) const
{
  return FuncData->IsDependent(ToLower(SymbolName));
}
//---------------------------------------------------------------------------
} //namespace Func32


