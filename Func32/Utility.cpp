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
#include <vector>
#include <complex>
#include <string>
#include <map>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
namespace Func32
{
//---------------------------------------------------------------------------
/** Evaluates a function expression using real numbers.
 *  \param Expr: Expression to evaluate; Is not allowed to contain any variables
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
long double Eval(const std::wstring &Expr, TTrigonometry Trig)
{
  TFunc Func(Expr, L"", Trig);
  return Func(0);
}
//---------------------------------------------------------------------------
/** Evaluates a function expression using real numbers.
 *  \param Expr: Expression to evaluate; Is not allowed to contain any variables
 *  \param SymbolList: List of custom functions/constants
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
long double Eval(const std::wstring &Expr, const TSymbolList &SymbolList, TTrigonometry Trig)
{
  TFunc Func(Expr, L"", SymbolList, Trig);
  return Func(0);
}
//---------------------------------------------------------------------------
/** Evaluates a function expression using real numbers.
 *  \param Expr: Expression to evaluate
 *  \param x:    This is the value of the variable if such is found
 *  \param Var:  This is the name of the variable
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
long double Eval(const std::wstring &Expr, long double x, const std::wstring &Var, TTrigonometry Trig)
{
  TFunc Func(Expr, Var, Trig);
  return Func(x);
}
//---------------------------------------------------------------------------
/** Evaluates a function expression using complex numbers.
 *  \param Expr: Expression to evaluate; Is not allowed to contain any variables
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
TComplex EvalComplex(const std::wstring &Expr, TTrigonometry Trig)
{
  TFunc Func(Expr, L"", Trig);
  return Func.CalcY(TComplex(0));
}
//---------------------------------------------------------------------------
/** Evaluates a function expression using complex numbers.
 *  \param Expr: Expression to evaluate; Is not allowed to contain any variables
 *  \param SymbolList: List of custom functions/constants.
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
TComplex EvalComplex(const std::wstring &Expr, const TSymbolList &SymbolList, TTrigonometry Trig)
{
  TFunc Func(Expr, L"", SymbolList, Trig);
  return Func.CalcY(TComplex(0));
}
//---------------------------------------------------------------------------
/** Evaluates a function expression using complex numbers.
 *  \param Expr: Expression to evaluate
 *  \param x:    This is the value of the variable if such is found
 *  \param Var:  This is the name of the variable
 *  \param Trig: Indicates weither trignometric functions are calculated in radians or degrees
 *  \return The result of the evaluated expression
 *  \throw EParseError: Thrown if an error occurs while parsing the expression
 *  \throw ECalcError: Thrown if a calculation error occurs
 */
TComplex EvalComplex(const std::wstring &Expr, TComplex x, const std::wstring &Var, TTrigonometry Trig)
{
  TFunc Func(Expr, Var, Trig);
  return Func.CalcY(TComplex(x));
}
//---------------------------------------------------------------------------
inline long double Sqr(long double x)
{
  return x*x;
}
//---------------------------------------------------------------------------
//Calculate the perp dot product
inline long double Perp(const TCoord<long double> &u, const TCoord<long double> &v)
{
  return u.x * v.y - u.y * v.x;
}
//---------------------------------------------------------------------------
/** Find crossing between two lines
 *  \param p1: First point in the line p1-p2
 *  \param p2: Second point in the line p1-p2
 *  \param q1: First point in the line q1-q2
 *  \param q2: Second point in the line q1-q2
 */
bool IsCrossing(const TCoord<long double> &p1, const TCoord<long double> &p2, const TCoord<long double> &q1, const TCoord<long double> &q2)
{
  TCoord<long double> u(p2.x - p1.x, p2.y - p1.y);  //Vector from p1 to p2
  TCoord<long double> v(q2.x - q1.x, q2.y - q1.y); //Vector from q1 to q2
  TCoord<long double> w(p1.x - q1.x, p1.y - q1.y); //vector from q1 to p1

  long double D = Perp(u, v);

//  if(std::abs(D) < 1E-30) //It is difficult to peek a good number here
  if(D == 0)
    return false; //The lines are parallel

  long double sI = Perp(v, w) / D;
  long double tI = Perp(u, w) / D;

  return sI >= 0 && sI <= 1 && tI >= 0 && tI <= 1;
}
//---------------------------------------------------------------------------
/** Find crossing between two functions. A range for the crossing must already
 *  be known, and there must be exactly one crossing in the interval. A binary
 *  search is used to locate the crossing.
 *  \param Func1: First function to find crossing between. The crossing must be located in the range [Min1;Max1]
 *  \param Min1: Start range for Func1 to check.
 *  \param Max1: End range for Func1 to check.
 *  \param Func2: Second function to find crossing between. The crossing must be located in the range [Min2;Max2]
 *  \param Min2: Start range for Func2 to check.
 *  \param Max2: End range for Func2 to check.
 */
long double FindCrossing(const TBaseFunc &Func1, long double Min1, long double Max1, const TBaseFunc &Func2, long double Min2, long double Max2)
{
  TCoord<long double> p1 = GetReal(Func1.Calc(TComplex(Min1)));
  TCoord<long double> p2 = GetReal(Func1.Calc(TComplex(Max1)));
  TCoord<long double> q1 = GetReal(Func2.Calc(TComplex(Min2)));
  TCoord<long double> q2 = GetReal(Func2.Calc(TComplex(Max2)));

  const int MaxIterations = 100;
  int IterLeft = MaxIterations;

  while(--IterLeft > 0)
  {
    long double s = (Max1 + Min1) / 2;
    long double t = (Max2 + Min2) / 2;

    //Continue until we cannot find a more accurate result
    if(s == Max1 || s == Min1 || t == Max2 || t == Min2)
      break;

    TCoord<long double> p = GetReal(Func1.Calc(TComplex(s)));
    TCoord<long double> q = GetReal(Func2.Calc(TComplex(t)));

    if(IsCrossing(p1, p, q1, q))
      Max1 = s, p2 = p, Max2 = t, q2 = q;
    else if(IsCrossing(p2, p, q1, q))
      Min1 = s, p1 = p, Max2 = t, q2 = q;
    else if(IsCrossing(p1, p, q2, q))
      Max1 = s, p2 = p, Min2 = t, q1 = q;
    else if(IsCrossing(p2, p, q2, q))
      Min1 = s, p1 = p, Min2 = t, q1 = q;
    else
      return s;
  }

  return (Max1 + Min1) / 2;
}
//---------------------------------------------------------------------------
class TFuncIterator
{
  TBaseFunc *Func;
  long double Value;
  long double Delta;

public:
  TFuncIterator(TBaseFunc &AFunc, long double AValue, long double ADelta) : Func(&AFunc), Value(AValue), Delta(ADelta) {}
  TFuncIterator(long double AValue) : Func(NULL), Value(AValue), Delta(0) {}
  TFuncIterator& operator++() {Value += Delta; return *this;}
  TCoord<long double> operator*() const {return Func->Calc(Value);}
  bool operator<(const TFuncIterator &Other) const {return Value < Other.Value;}
};
//---------------------------------------------------------------------------
/** Used to analyze a function for crossings with one of the axes
 *  \param Func:        The function to analyze
 *  \param Min:         The start of the interval to analyze
 *  \param Max:         The end of the interval to analyze
 *  \param Steps:       Number of calculations done in the interval to identify a crossing
 *  \param Tol:         Tolerance for how close the result is to the axis.
 *  \param AnalyseType: Indicates for which axis crossings are analyzed for
 *  \return A vector with the coordinates where the function are crossing the axis.
 */
std::vector<TCoordSet<> > AnalyseFunction(const TBaseFunc &Func, long double Min, long double Max, unsigned Steps, long double Tol, TAnalyseType AnalyseType)
{
  const unsigned MaxCount = 100; //Max number of iteration to avoid infinite loop
  const long double dt = (Max - Min) / Steps;
  long double (TBaseFunc::*Eval)(long double, ECalcError&) const;

  switch(AnalyseType)
  {
    case atXAxisCross:
      Eval = &TBaseFunc::CalcY;
      break;

    case atYAxisCross:
      Eval = &TBaseFunc::CalcX;
      break;
  }

  bool LastResult = true;
  std::vector<TCoordSet<> > Data;
  ECalcError E;
  TErrorCode LastError = ecInternalError; //Initialize to error to prevent detecting first point as a crossing
  for(long double t = Min; t <= Max; t += dt)
  {
    bool Result = (Func.*Eval)(t, E) > 0;
    if(Result != LastResult && !E.ErrorCode && !LastError)
    {
      long double sMin = t - dt;
      long double sMax = t;
      long double s;
      unsigned Count = MaxCount; //Count down to avoid infinite loop
      do
      {
        s = (sMax + sMin) / 2;
        long double Value = (Func.*Eval)(s, E);

        if(E.ErrorCode)
          break; //Ignore crossing if an error was found while improving accuracy

        if((Value > 0) == LastResult)
          sMin = s;
        else
          sMax = s;
      } while(std::abs(sMax-sMin) > Tol && --Count > 0);

      if(!E.ErrorCode)
        Data.push_back(TCoordSet<>(s, Func.CalcX(s), Func.CalcY(s)));
    }
    LastResult = Result;
    LastError = E.ErrorCode;
  }
  return Data;
}
//---------------------------------------------------------------------------
std::vector<std::wstring> FindUnknowns(const std::wstring &Str)
{
  std::vector<std::wstring> Vector;
  std::size_t Pos = Str.find('$');
  while(Pos != std::string::npos)
  {
    unsigned Pos2;
    for(Pos2 = Pos + 1; Pos2 < Str.size(); Pos2++)
      if(!std::isalnum(Str[Pos2]))
        break;

    if(Pos2 != Pos + 1)
    {
      std::wstring Name = Str.substr(Pos, Pos2 - Pos);
      if(std::find_if(Vector.begin(), Vector.end(), TCompareStringNoCase(Name)) == Vector.end())
        Vector.push_back(Name);
    }
    Pos = Str.find('$', Pos + 1);
  }

  return Vector;
}
//---------------------------------------------------------------------------
std::wstring ToLower(const std::wstring &Str)
{
  std::wstring Result;
  for(unsigned I = 0; I < Str.size(); I++)
    Result += std::tolower(Str[I]);
  return Result;
}
//---------------------------------------------------------------------------
bool IsValidName(const std::wstring &Name)
{
  if(Name.empty())
    return false;

  if(!std::isalpha(Name[0]) && Name[0] != '$' && Name[0] != '_')
    return false;

  for(unsigned I = 1; I < Name.size(); I++)
    if(!std::isalnum(Name[I]) && Name[I] != '_')
      return false;

  for(int I = FirstFunction1P; I <= LastFunction; I++)
  {
    TIdent Ident = static_cast<TIdent>(I);
    const wchar_t *Str = FunctionName(Ident);
    if(Str && Str == Name)
      return false;
  }

  if(Name == L"i" || Name == L"e" || Name == L"pi" || Name == L"rand" || Name == L"undef" || Name == L"inf")
    return false;

  return true;
}
//---------------------------------------------------------------------------
bool IsEqual(long double a, long double b)
{
	if(boost::math::isnan(a) && boost::math::isnan(b))
		return true;

	if(!boost::math::isfinite(a) || !boost::math::isfinite(b))
		return a == b;

	int a_exp, b_exp, exp;
	frexp(a, &a_exp);
	frexp(b, &b_exp);
	frexp(a - b, &exp);
	return IsZero(a-b) || (a_exp == b_exp && std::abs(exp - a_exp) > 40);
}
//---------------------------------------------------------------------------
void LogDebugString(const char *Str, const char *Str2)
{
	std::clog << Str << Str2 << std::endl;
	BOOST_ASSERT(0);
}
//---------------------------------------------------------------------------
} //namespace Func32



