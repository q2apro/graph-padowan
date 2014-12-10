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
/** Swap the base part of the function with another function.
 *  \param Func: Function to swap with.
 */
void TBaseFunc::SwapBase(TBaseFunc &Func)
{
  std::swap(Trigonometry, Func.Trigonometry);
}
//---------------------------------------------------------------------------
/** Assignment operator. Assigns the base part of F to *this.
 *  \param F: Function to assign to this.
 */
const TBaseFunc& TBaseFunc::operator=(const TBaseFunc &F)
{
  Trigonometry = F.Trigonometry;
  return *this;
}
//---------------------------------------------------------------------------
/** Evaluate the function for s.
 *  \param s: Value to evaluate the function for.
 *  \return The x- and y-coordinate in a TCoord structure.
 */
TCoord<long double> TBaseFunc::Calc(long double s) const
{
  ECalcError E;
  TCoord<long double> Result = Calc(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
/** Evaluate the function for s using complex numbers.
 *  \param s: Value to evaluate the function for.
 *  \return The x- and y-coordinate in a TCoord structure.
 */
TCoord<TComplex> TBaseFunc::Calc(TComplex s) const
{
  ECalcError E;
  TCoord<TComplex> Result = Calc(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
/** Evaluate the x-coordinate of the function for s.
 *  \param s: The value to evaluate the function for.
 *  \return The x-coordinate of the function
 */
long double TBaseFunc::CalcX(long double s) const
{
  ECalcError E;
  long double Result = CalcX(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
/** Evaluate the x-coordinate of the function for s using complex numbers.
 *  \param s: The value to evaluate the function for.
 *  \return The x-coordinate of the function
 */
TComplex TBaseFunc::CalcX(TComplex s) const
{
  ECalcError E;
  TComplex Result = CalcX(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
/** Evaluate the y-coordinate of the function for s.
 *  \param s: The value to evaluate the function for.
 *  \return The y-coordinate of the function
 */
long double TBaseFunc::CalcY(long double s) const
{
  ECalcError E;
  long double Result = CalcY(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
/** Evaluate the y-coordinate of the function for s using complex numbers.
 *  \param s: The value to evaluate the function for.
 *  \return The y-coordinate of the function
 */
TComplex TBaseFunc::CalcY(TComplex s) const
{
  ECalcError E;
  TComplex Result = CalcY(s, E);
  if(E.ErrorCode)
    throw E;
  return Result;
}
//---------------------------------------------------------------------------
} //namespace Func32






