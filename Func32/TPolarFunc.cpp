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
#include <memory>
#include <limits>
#include <iomanip>
#define PI    3.141592653589793238462643L

namespace Func32
{
//---------------------------------------------------------------------------
/** Default constructor creates an empty polar function.
 */
TPolarFunc::TPolarFunc() : FuncData(new TFuncData)
{
}
//---------------------------------------------------------------------------
/** Construcor to create TPolarfunc object and initialize it from a string
 *  \param Text: The r(t) as a string
 *  \param Variable: The name of the function variable. Defaults to "t".
 *  \param ATrigonometry: Choose to use radians or degrees for trignometric functions. Defaults to Radian.
 *  \throw EParseError: Thrown on parse errors
 */
TPolarFunc::TPolarFunc(const std::wstring &Text, const std::wstring &Variable, TTrigonometry ATrigonometry)
  : TBaseFunc(ATrigonometry), FuncData(new TFuncData(Text, Variable))
{
}
//---------------------------------------------------------------------------
/** Construcor to create TPolarfunc object and initialize it from a string
 *  \param Text: The r(t) as a string
 *  \param Variable: The name of the function variable.
 *  \param SymbolList: List of custom functions/constants.
 *  \param ATrigonometry: Choose to use radians or degrees for trignometric functions. Defaults to Radian.
 *  \throw EParseError: Thrown on parse errors
 */
TPolarFunc::TPolarFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList, TTrigonometry ATrigonometry)
  : TBaseFunc(ATrigonometry), FuncData(new TFuncData(Text, Variable, SymbolList))
{
}
//---------------------------------------------------------------------------
/** Ensures that the reference count of FuncData is 1 and DifData is invalidated.
 */
inline void TPolarFunc::Unique()
{
  if(!FuncData.unique())
    FuncData.reset(new TFuncData(*FuncData));
  DifData.reset();
}
//---------------------------------------------------------------------------
/** Assigns a text string to the object
 *  \param Text: Function text string to convert
 *  \param Variable: The name of the variable. Defaults to "t".
 *  \throw EParseError: Thrown if parsing fails
 */
void TPolarFunc::SetFunc(const std::wstring &Text, const std::wstring &Variable)
{
  FuncData.reset(new TFuncData(Text, Variable));
}
//---------------------------------------------------------------------------
/** Assigns a text string to the object
 *  \param Text: Function text string to convert
 *  \param Variable: The name of the variable.
 *  \param SymbolList: List of custom functions/constants.
 *  \throw EParseError: Thrown if parsing fails
 */
void TPolarFunc::SetFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList)
{
  FuncData.reset(new TFuncData(Text, Variable, SymbolList));
}
//---------------------------------------------------------------------------
/** Calculate the x-coordinate coresponding to the angle t. This does not throw any exceptions.
 *  \param t: Angle to calculate x-coordinate at.
 *  \param E: Error information filled at return.
 *  \return x-coordinate evaluated at the given angle.
 */
long double TPolarFunc::CalcX(long double t, ECalcError &E) const
{
  return FuncData->Calc(&t, Trigonometry, E) * std::cos(Trigonometry == Radian ? t : t*(PI/180));
}
//---------------------------------------------------------------------------
/** Calculate the y-coordinate coresponding to the angle t. This does not throw any exceptions.
 *  \param t: Angle to calculate y-coordinate at.
 *  \param E: Error information filled at return.
 *  \return y-coordinate evaluated at the given angle.
 */
long double TPolarFunc::CalcY(long double t, ECalcError &E) const
{
  return FuncData->Calc(&t, Trigonometry, E) * std::sin(Trigonometry == Radian ? t : t*(PI/180));
}
//---------------------------------------------------------------------------
/** Returns the function evaluated for the given argument.
 *  \param t: Angle to evaluate the function at.
 *  \return The result of the evaluation.
 *  \throws ECalcError: Thrown on error
 */
long double TPolarFunc::CalcR(long double t) const
{
  return FuncData->Calc(&t, Trigonometry);
}
//---------------------------------------------------------------------------
/** Finds and returns a pointer to a new object with the first derivative of the function.
 *  \throw EFuncError: If the differentiation fails.
 */
TPolarFunc* TPolarFunc::MakeDifPtr() const
{
  if(!DifData.get())
    DifData = FuncData->MakeDif(CodeArgument, Trigonometry);
  return new TPolarFunc(DifData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Finds and returns a pointer to a new object with the first derivative of the function.
 *  \param Var: Name of variabel to differentiate with respect to.
 *  \throw EFuncError: If the differentiation fails.
 */
TPolarFunc* TPolarFunc::MakeDifPtr(const std::wstring &Var) const
{
  return new TPolarFunc(FuncData->MakeDif(TElem(CodeCustom, Var), Trigonometry), Trigonometry);
}
//---------------------------------------------------------------------------
/** Calculate an return the first derivative of the function.
 *  \throw EFuncError: If the differentiation fails.
 */
TPolarFunc TPolarFunc::MakeDif() const
{
  if(!DifData.get())
    DifData = FuncData->MakeDif(CodeArgument, Trigonometry);
  return TPolarFunc(DifData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Calculate an return the first derivative of the function.
 *  \param Var: Name of variable/constant to differentiate with respect to.
 *  \throw EFuncError: If the differentiation fails.
 */
TPolarFunc TPolarFunc::MakeDif(const std::wstring &Var) const
{
  return TPolarFunc(FuncData->MakeDif(TElem(CodeCustom, Var), Trigonometry), Trigonometry);
}
//---------------------------------------------------------------------------
/** Clears the function. afterwards it's empty.
 */
void TPolarFunc::Clear()
{
  FuncData.reset(new TFuncData);
  Trigonometry = Radian;
}
//---------------------------------------------------------------------------
/** Returns true if the function is empty.
 */
bool TPolarFunc::IsEmpty() const
{
  return FuncData->IsEmpty();
}
//---------------------------------------------------------------------------
/** Converts from internal format to at text string.
 *  \param Decimals: Number of decimals in numbers.
 *  \param Variable: Variable name to use in the text.
 */
std::wstring TPolarFunc::MakeText(const std::wstring &Variable, unsigned Decimals) const
{
  std::wostringstream Stream;
  FuncData->MakeText(std::vector<std::wstring>(1, Variable), Stream, Decimals);
  return Stream.str();
}
//---------------------------------------------------------------------------
//! Assigns the content of Func to this object
TPolarFunc& TPolarFunc::operator=(const TPolarFunc &Func)
{
  TBaseFunc::operator=(Func);

  FuncData = Func.FuncData;
  return *this;
}
//---------------------------------------------------------------------------
/** Assign Function to this object.
 *  \param F: Function to assign.
 *  \throw EFuncError: If F is not of type TPolarFunc.
 */
void TPolarFunc::Assign(const TBaseFunc &F)
{
  if(const TPolarFunc *Func = dynamic_cast<const TPolarFunc*>(&F))
    *this = *Func;
  else
    throw EFuncError(ecErrorType);
}
//---------------------------------------------------------------------------
//! Compares this object to the content of Func and returns true if they are equal.
bool TPolarFunc::operator==(const TPolarFunc &Func) const
{
  return FuncData == Func.FuncData || *FuncData == *Func.FuncData;
}
//---------------------------------------------------------------------------
//! Compares this object to the content of Func and returns true if they are different.
bool TPolarFunc::operator!=(const TPolarFunc &Func) const
{
  return !(*this == Func);
}
//---------------------------------------------------------------------------
/** Returns the function evaluated for the given argument.
 *  \param t: Value to replace the variable in the function.
 *  \return The result of the evaluation.
 *  \throws ECalcError: Thrown on error
 */
long double TPolarFunc::operator()(long double t) const
{
  return CalcR(t);
}
//---------------------------------------------------------------------------
//Returns the complex x-coordinate coresponding to the angle t
TComplex TPolarFunc::CalcX(TComplex t, ECalcError &E) const
{
  return FuncData->Calc(&t, Trigonometry, E) * cos(Trigonometry == Radian ? t : t*(PI/180));
}
//---------------------------------------------------------------------------
//Returns the complex y-coordinate coresponding to the angle t
TComplex TPolarFunc::CalcY(TComplex t, ECalcError &E) const
{
  return FuncData->Calc(&t, Trigonometry, E) * sin(Trigonometry == Radian ? t : t*(PI/180));
}
//---------------------------------------------------------------------------
/** Evaluate (x,y) pair for the function.
 *  \param t: Angle to evaluate at.
 *  \param E: Contains error information on return.
 *  \return Structure with the (x,y) pair.
 */
TCoord<long double> TPolarFunc::Calc(long double t, ECalcError &E) const
{
  long double r = FuncData->Calc(&t, Trigonometry, E);
  if(Trigonometry == Degree)
    t *= PI / 180;
  return TCoord<long double>(r * std::cos(t), r * std::sin(t));
}
//---------------------------------------------------------------------------
/** Evaluate (x,y) pair for the function using complex numbers.
 *  \param t: Angle to evaluate at.
 *  \param E: Contains error information on return.
 *  \return Structure with the (x,y) pair.
 */
TCoord<TComplex> TPolarFunc::Calc(TComplex t, ECalcError &E) const
{
  TComplex r = FuncData->Calc(&t, Trigonometry, E);
  if(Trigonometry == Degree)
    t *= PI / 180;
  return TCoord<TComplex>(r * cos(t), r * sin(t));
}
//---------------------------------------------------------------------------
/** Create a TFunc object with the x-part of the function.
 */
TFunc TPolarFunc::ConvXToFunc() const
{
  return TFunc(FuncData, Trigonometry) * TFunc(L"cos x", L"x", Trigonometry);
}
//---------------------------------------------------------------------------
/** Create a TFunc object with the y-part of the function.
 */
TFunc TPolarFunc::ConvYToFunc() const
{
  return TFunc(FuncData, Trigonometry) * TFunc(L"sin x", L"x", Trigonometry);
}
//---------------------------------------------------------------------------
//! Converts the polar function to a standard function.
TFunc TPolarFunc::ConvToFunc() const
{
  return TFunc(FuncData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Optimize the internal data for the function.
 */
void TPolarFunc::Simplify()
{
  Unique();
  FuncData->Simplify();
}
//---------------------------------------------------------------------------
//! Swaps the function with the content of Func. This cannot throw.
void TPolarFunc::Swap(TPolarFunc &Func)
{
  SwapBase(Func);
  FuncData.swap(Func.FuncData);
}
//---------------------------------------------------------------------------
/** Calculate the slope as an angle at the given angle.
 *  \param t: Angle to calculate slope at. This is in radians or degrees dependend on the trigonometry setting.
 *  \return Angle in radians. The trigonometry setting is ignored.
 *  \throw EFuncError: Thrown if the function cannot be differentiated.
 *  \throw ECalcError: Thrown if evaluation fails.
 */
long double TPolarFunc::CalcAngleSlope(long double t) const
{
  TCoord<long double> R  = Calc(t);
  TCoord<long double> dR = MakeDif().Calc(t);
  long double dx = dR.x - (Trigonometry == Degree ? PI/180 * R.y : R.y);
  long double dy = dR.y + (Trigonometry == Degree ? PI/180 * R.x : R.x);
  return std::atan2(dy, dx);
}
//---------------------------------------------------------------------------
/** Returns the length of a part of the curve.
 *  \param tMin: Start angle.
 *  \param tMax: End angle.
 *  \param RelError: Max estimated error that will throw an exception.
 *  \return Length of curve from tMin to tMax.
 *  \throw EFuncError: Thrown when differentiation fails.
 *  \throw ECalcError: Throw if evaluation fails.
 */
double TPolarFunc::CalcArc(double tMin, double tMax, double RelError) const
{
  TFunc r(ConvToFunc());
  return sqrt(pow(r, 2) + pow(r.MakeDif(), 2)).CalcArea(tMin, tMax, RelError);
}
//---------------------------------------------------------------------------
/** Calculate the area between origo and the function from tMin to tMax
 *  \param tMin: Angle indicating start of range
 *  \param tMax: Angle indicating end of range
 *  \param RelError: Max estimated error that will throw an exception.
 *  \return The calculated area from tMin to tMax.
 *  \throw EFuncError if the max estimated relative error could not be reached.
 */
double TPolarFunc::CalcArea(double tMin, double tMax, double RelError) const
{
  return pow(ConvToFunc(), 2).CalcArea(tMin, tMax, RelError) / 2;
}
//---------------------------------------------------------------------------
/** Calculate the slope at the given angle.
 *  \param t: Angle to calculate slope at. This is in radians or degrees dependend on the trigonometry setting.
 *  \return The slope at the given angle.
 *  \throw EFuncError: Thrown if the function cannot be differentiated.
 *  \throw ECalcError: Thrown if evaluation fails.
 */
long double TPolarFunc::CalcSlope(long double t) const
{
  TCoord<long double> R  = Calc(t);
  TCoord<long double> dR = MakeDif().Calc(t);

  long double dx = dR.x - (Trigonometry == Degree ? PI/180 * R.y : R.y);
  long double dy = dR.y + (Trigonometry == Degree ? PI/180 * R.x : R.x);

  if(dx == 0)
    return dy > 0 ? +std::numeric_limits<long double>::infinity() : -std::numeric_limits<long double>::infinity();
  return dy/dx;
}
//---------------------------------------------------------------------------
/** Updates the function from the given symbol list.
 *  \param SymbolList: List of custom functions/constants.
 */
bool TPolarFunc::Update(const TSymbolList &SymbolList)
{
  Unique();
  return FuncData->Update(SymbolList);
}
//---------------------------------------------------------------------------
/** Check if a symbol is referenced by the function directly or indirectly.
 *  \param SymbolName: Name of symbol to check for.
 */
bool TPolarFunc::IsDependent(const std::wstring &SymbolName) const
{
  return FuncData->IsDependent(ToLower(SymbolName));
}
//---------------------------------------------------------------------------
} //namespace Func32

