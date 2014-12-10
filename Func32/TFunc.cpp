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
#include <memory>
namespace Func32
{
//---------------------------------------------------------------------------
/** Constructor to create TFunc object. The default constructor creates an empty function.
 */
TFunc::TFunc() : FuncData(new TFuncData)
{
}
//---------------------------------------------------------------------------
/** Creates a TFunc object from a string containing the function text.
 *  \param Text: String to parse
 *  \param AVariable: Variable name. Defaults to "x".
 *  \param ATrigonometry: Choose to use radians or degrees for trigonometric functions. Defaults to Radian.
 *  \throw EParseError: Thrown if parsing fails
 */
TFunc::TFunc(const std::wstring &Text, const std::wstring &AVariable, TTrigonometry ATrigonometry)
  : TBaseFunc(ATrigonometry), FuncData(new TFuncData(Text, std::vector<std::wstring>(1, AVariable)))
{
}
//---------------------------------------------------------------------------
/** Creates a TFunc object from a string and a symbol list.
 *  \param Text: String to parse
 *  \param AVariable: Variable name.
 *  \param SymbolList: List of custom functions/constants
 *  \param ATrigonometry: Choose to use radians or degrees for trigonometric functions. Defaults to Radian.
 *  \throw EParseError: Thrown if parsing fails
 */
TFunc::TFunc(const std::wstring &Text, const std::wstring &AVariable, const TSymbolList &SymbolList, TTrigonometry ATrigonometry)
  : TBaseFunc(ATrigonometry), FuncData(new TFuncData(Text, AVariable, SymbolList))
{
}
//---------------------------------------------------------------------------
/** Creates a TFunc object from a number.
 *  \param Value: Number to create function from.
 */
TFunc::TFunc(long double Value) : FuncData(new TFuncData)
{
  FuncData->Add(Value);
}
//---------------------------------------------------------------------------
/** Makes sure the reference count for FuncData is 1.
 *  DifData is cleared because this function is always called because we want to change FuncData, which invalidates DifData.
 */
inline void TFunc::Unique()
{
  if(!FuncData.unique())
    FuncData.reset(new TFuncData(*FuncData));
  DifData.reset();
}
//---------------------------------------------------------------------------
/** Assigns a text string to the object
 *  \param Text: Function text string to convert
 *  \param Variable: The functions variable.
 *  \throw EParseError: Thrown if parsing fails
 */
void TFunc::SetFunc(const std::wstring &Text, const std::wstring &Variable)
{
  FuncData.reset(new TFuncData(Text, Variable)); //Make new data
}
//---------------------------------------------------------------------------
/** Assigns a text string to the object
 *  \param Text: Function text string to convert
 *  \param Variable: The functions variable.
 *  \param SymbolList: List of custom functions/constants to be used by the function.
 *  \throw EParseError: Thrown if parsing fails
 */
void TFunc::SetFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList)
{
  FuncData.reset(new TFuncData(Text, std::vector<std::wstring>(1, Variable), SymbolList)); //Make new data
}
//---------------------------------------------------------------------------
/** Clears the function. After this the function will be empty.
 */
void TFunc::Clear()
{
  FuncData.reset(new TFuncData);
  Trigonometry = Radian;
}
//---------------------------------------------------------------------------
/** Returns true if the function is empty.
 */
bool TFunc::IsEmpty() const
{
  return FuncData->IsEmpty();
}
//---------------------------------------------------------------------------
/** Just returns the parameter. This cannot fail.
 *  \param x: The x value returned.
 *  \param E: Class with eror information. Not used as this function cannot fail.
 */
long double TFunc::CalcX(long double x, ECalcError &E) const
{
  E.ErrorCode = ecNoError;
  return x;
}
//---------------------------------------------------------------------------
/** Just returns the parameter. This cannot fail.
 *  \param x: The x value returned.
 *  \param E: Class with eror information. Not used as this function cannot fail.
 */
TComplex TFunc::CalcX(TComplex x, ECalcError &E) const
{
  return x;
}
//---------------------------------------------------------------------------
/** Calculates the y-coordinate for the given x-coordinate.
 *  \param x: The x-ccordinate to evaluate the function for.
 *  \param E: Class with error information on return.
 */
long double TFunc::CalcY(long double x, ECalcError &E) const
{
 return FuncData->Calc(&x, Trigonometry, E);
}
//---------------------------------------------------------------------------
/** Calculates the y-coordinate using complex numbers for the given x-coordinate.
 *  \param x: The x-ccordinate to evaluate the function for.
 *  \param E: Class with error information on return.
 */
TComplex TFunc::CalcY(TComplex x, ECalcError &E) const
{
  return FuncData->Calc(&x, Trigonometry, E);
}
//---------------------------------------------------------------------------
/** Returns a pointer to a new TFunc object, which is the first derivative of this function.
 *  \throw EFuncError: If differentiation fails.
 */
TFunc* TFunc::MakeDifPtr() const
{
  if(!DifData)
    DifData = FuncData->MakeDif(CodeArgument, Trigonometry);
  return new TFunc(DifData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Returns a pointer to a new TFunc object, which is the first derivative with respect to the given variable name.
 *  \param Var: Name of variabel/constant to differentiate with respect to.
 *  \throw EFuncError: If differentiation fails.
 */
TFunc* TFunc::MakeDifPtr(const std::wstring &Var) const
{
  return new TFunc(FuncData->MakeDif(TElem(CodeCustom, Var), Trigonometry), Trigonometry);
}
//---------------------------------------------------------------------------
/** Returns the first derivative of the function. This is an alternative to MakeDifPtr().
 *  \throw EFuncError: Thrown if function could not be differentiated.
 */
TFunc TFunc::MakeDif() const
{
  if(!DifData)
    DifData = FuncData->MakeDif(CodeArgument, Trigonometry);
  return TFunc(DifData, Trigonometry);
}
//---------------------------------------------------------------------------
/** Returns the first derivative of the function. This is an alternative to MakeDifPtr().
 *  \param Var: Name of variabel/constant to differentiate with respect to.
 *  \throw EFuncError: Thrown if function could not be differentiated.
 */
TFunc TFunc::MakeDif(const std::wstring &Var) const
{
  return TFunc(FuncData->MakeDif(TElem(CodeCustom, Var), Trigonometry), Trigonometry);
}
//---------------------------------------------------------------------------
/** Converts from internal format to at text string.
 *  \param Variable: Name to use as the variable. The default is "x".
 *  \param Decimals: Number of decimals in numbers.
 *  \return The function as a text string.
 */
std::wstring TFunc::MakeText(const std::wstring &Variable, unsigned Decimals) const
{
  std::wostringstream Stream;
  FuncData->MakeText(std::vector<std::wstring>(1, Variable), Stream, Decimals);
  return Stream.str();
}
//---------------------------------------------------------------------------
/** Assigns another function to the object.
 *  \param Func: Function to assign;
 */
TFunc& TFunc::operator=(const TFunc &Func)
{
  TBaseFunc::operator=(Func);

  FuncData = Func.FuncData;
  return *this;
}
//---------------------------------------------------------------------------
/** Assign function to the object.
 *  \param F: Function to assign.
 *  \throw EFuncError: Thrown if F is not of type TFunc.
 */
void TFunc::Assign(const TBaseFunc &F)
{
  if(const TFunc *Func = dynamic_cast<const TFunc*>(&F))
    *this = *Func;
  else
    throw EFuncError(ecErrorType);
}
//---------------------------------------------------------------------------
/** The function returns a TFunc that is Func1 and Func2 added.
 */
TFunc const operator+(TFunc Func1, const TFunc &Func2)
{
  //Add data from Func2 to temporary function Func1
  Func1 += Func2;
  return Func1;
}
//---------------------------------------------------------------------------
/** The function returns a TFunc that is Func1 and Func2 subtracted.
 */
TFunc const operator-(TFunc Func1, const TFunc &Func2)
{
  //Add data from Func2 to temporary function Func1
  Func1 -= Func2;
  return Func1;
}
//---------------------------------------------------------------------------
/** The function returns a TFunc that is Func1 and Func2 multiplied.
 */
TFunc const operator*(TFunc Func1, const TFunc &Func2)
{
  //Add data from Func2 to temporary function Func1
  Func1 *= Func2;
  return Func1;
}
//---------------------------------------------------------------------------
/** The function returns a TFunc that is the division of Func1 by Func2.
 */
TFunc const operator/(TFunc Func1, const TFunc &Func2)
{
  //Add data from Func2 to temporary function Func1
  Func1 /= Func2;
  return Func1;
}
//---------------------------------------------------------------------------
/** Adds Func to this TFunc and assigns the result to this TFunc
 */
TFunc& TFunc::operator+=(const TFunc &Func)
{
  //Add data from Func
  Unique();
  if(!FuncData->IsEmpty())
    FuncData->AddFront(CodeAdd);
  FuncData->Add(*Func.FuncData);
  return *this;
}
//---------------------------------------------------------------------------
/** Subtracts Func from this TFunc and assigns the result to this TFunc
 */
TFunc& TFunc::operator-=(const TFunc &Func)
{
  //Add data from Func
  Unique();
  FuncData->AddFront(FuncData->IsEmpty() ? CodeNeg : CodeSub);
  FuncData->Add(*Func.FuncData);
  return *this;
}
//---------------------------------------------------------------------------
/** Multiplies Func with this TFunc and assigns the result to this TFunc
 */
TFunc& TFunc::operator*=(const TFunc &Func)
{
  //Add data from Func
  Unique();
  FuncData->AddFront(CodeMul);
  FuncData->Add(*Func.FuncData);
  return *this;
}
//---------------------------------------------------------------------------
/** Divide this Tfunc with Func and assigns the result to this TFunc
 */
TFunc& TFunc::operator/=(const TFunc &Func)
{
  //Add data from Func
  Unique();
  FuncData->AddFront(CodeDiv);
  FuncData->Add(*Func.FuncData);
  return *this;
}
//---------------------------------------------------------------------------
/** Compares Func to this object and returns true if they are equal.
 */
bool TFunc::operator==(const TFunc &Func) const
{
  //Check they reference the same object; If not compare the objects
  return FuncData == Func.FuncData || *FuncData == *Func.FuncData;
}
//---------------------------------------------------------------------------
/** Compares Func to this object and returns true if they are different.
 */
bool TFunc::operator!=(const TFunc &Func) const
{
  //return false if the two memory blocks are equal
  return !(*this == Func);
}
//---------------------------------------------------------------------------
/** This is the same as the CalcY function
 */
long double TFunc::operator()(long double x) const
{
  return CalcY(x);
}
//---------------------------------------------------------------------------
/** This is the same as the CalcY function
 */
TComplex TFunc::operator()(TComplex x) const
{
  return CalcY(x);
}
//---------------------------------------------------------------------------
//Optimize the internal data for the function
void TFunc::Simplify()
{
  Unique();
  FuncData->Simplify();
}
//---------------------------------------------------------------------------
/** Swap the function with another function. This cannot throw any exceptions.
 *  \param Func: Function to swap content with
 */
void TFunc::Swap(TFunc &Func)
{
  SwapBase(Func);
  FuncData.swap(Func.FuncData);
}
//---------------------------------------------------------------------------
/** Return Func1 to the power of Func2.
 *  Variable and radian settings will be taken from Func1
 */
TFunc pow(const TFunc &Func1, const TFunc& Func2)
{
  TFunc Func;
  Func.FuncData->Add(CodePow);
  Func.FuncData->Add(*Func1.FuncData);
  Func.FuncData->Add(*Func2.FuncData);
  Func.Trigonometry = Func1.Trigonometry;
  return Func;
}
//---------------------------------------------------------------------------
/** Returns the function with the natural logarithm to Func1
 */
TFunc log(const TFunc &Func1)
{
  TFunc Func;
  Func.FuncData->Add(CodeLn);
  Func.FuncData->Add(*Func1.FuncData);
  Func.Trigonometry = Func1.Trigonometry;
  return Func;
}
//---------------------------------------------------------------------------
/** Returns the square root of the function
 */
TFunc sqrt(const TFunc &Func1)
{
  TFunc Func;
  Func.FuncData->Add(CodeSqrt);
  Func.FuncData->Add(*Func1.FuncData);
  Func.Trigonometry = Func1.Trigonometry;
  return Func;
}
//---------------------------------------------------------------------------
/** Convert the objects to text and add it to the stream
 */
std::wostream& operator<<(std::wostream &os, const TFunc &Func)
{
  Func.FuncData->MakeText(std::vector<std::wstring>(1, L"x"), os, os.precision());
  return os;
}
//---------------------------------------------------------------------------
/** Functions text is retrieve from the stream. The rest of the line is parsed as the function text.
 *  \throw EParseError: Thrown if parsing fails.
 */
std::wistream& operator>>(std::wistream &is, TFunc &Func)
{
  std::wstring Str;
  std::getline(is, Str);
  Func.SetFunc(Str);
  return is;
}
//---------------------------------------------------------------------------
/** Calculate the slope as an angle at the given x-coordinate.
 *  \param x: x-coordinate to calculate slope at.
 *  \return Angle in radians. The trigonometry setting is ignored.
 *  \throw EFuncError: Thrown if the function cannot be differentiated.
 *  \throw ECalcError: Thrown if evaluation fails.
 */
long double TFunc::CalcAngleSlope(long double x) const
{
  return std::atan(MakeDif().CalcY(x));
}
//---------------------------------------------------------------------------
/** Calculates the area under curve between xMin and xMax
 *  \param xMin: Start of range
 *  \param xMax: End of range
 *  \param RelError: Max estimated error that will throw an exception.
 *  \return The calculated area.
 *  \throw EFuncError if the max estimated relative error could not be reached.
 */
double TFunc::CalcArea(double xMin, double xMax, double RelError) const
{
  return FuncData->Integrate(xMin, xMax, RelError, Trigonometry);
}
//---------------------------------------------------------------------------
/** Returns the length of the curve between x=xMin and x=xMax made by the function.
 *  \param xMin: Start of range
 *  \param xMax: End of range
 *  \param RelError: Max estimated error that will throw an exception.
 *  \return The calculated path.
 *  \throw EFuncError if the max estimated relative error could not be reached.
 */
double TFunc::CalcArc(double xMin, double xMax, double RelError) const
{
  return sqrt(1 + pow(MakeDif(), 2)).CalcArea(xMin, xMax, RelError);
}
//---------------------------------------------------------------------------
/** Calculate slope at given x-ccordinate.
 *  \param x: x-coordinate to evaluate at.
 *  \return Slope at give coordinate.
 *  \throw EFuncError: Thrown if the function cannot be differentiated.
 *  \throw ECalcError: Thrown if evaluation fails.
 */
long double TFunc::CalcSlope(long double x) const
{
  if(!DifData)
    DifData = FuncData->MakeDif(CodeArgument, Trigonometry);
  return DifData->Calc(&x, Trigonometry);
}
//---------------------------------------------------------------------------
/** Evaluate (x,y) pair for the function.
 *  \param x: x-coordinate to evaluate at.
 *  \param E: Contains error information on return.
 *  \return Structure with the (x,y) pair.
 */
TCoord<long double> TFunc::Calc(long double x, ECalcError &E) const
{
  return TCoord<long double>(x, FuncData->Calc(&x, Trigonometry, E));
}
//---------------------------------------------------------------------------
/** Evaluate (x,y) pair for the function using complex numbers.
 *  \param x: x-coordinate to evaluate at.
 *  \param E: Contains error information on return.
 *  \return Structure with the (x,y) pair.
 */
TCoord<TComplex> TFunc::Calc(TComplex x, ECalcError &E) const
{
  return TCoord<TComplex>(x, FuncData->Calc(&x, Trigonometry, E));
}
//---------------------------------------------------------------------------
/** Update the function from the symbol list.
 */
bool TFunc::Update(const TSymbolList &SymbolList)
{
  Unique();
  return FuncData->Update(SymbolList);
}
//---------------------------------------------------------------------------
/** Check if a symbol is referenced by the function directly or indirectly.
 *  \param SymbolName: Name of symbol to check for.
 */
bool TFunc::IsDependent(const std::wstring &SymbolName) const
{
  return FuncData->IsDependent(ToLower(SymbolName));
}
//---------------------------------------------------------------------------
} //namespace Func32
