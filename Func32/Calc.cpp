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
#pragma warn -8072
#include <cmath>
#include <limits>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/complex/asin.hpp>
#include <boost/math/complex/acos.hpp>
#include <boost/math/complex/atan.hpp>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include <boost/tr1/complex.hpp>
#define BOOST_THREAD_USE_LIB //Needed by thread.hpp
#include <boost/thread.hpp>
//Disable Warnings: "Condition is always true/false", "Unreachable code"
#pragma warn -8008
#pragma warn -8066
//---------------------------------------------------------------------------
#define EULER 2.718281828459045235360287L
#define PI    3.141592653589793238462643L
namespace Func32
{
//The using derectives are necessary because names in std:: will not be used if they are found in Func32::
using namespace std;
using std::pow;
using std::fmod;
using std::asin;
using std::acos;
using std::atan;
using std::floor;
using std::ceil;
using std::log;
using std::sqrt;

const unsigned MaxRecursion = 100; //Maximum number of recursive calls allowed

/** Delay creation of table until first use. The rest of the library must be initialized first
 *  \param Ident: Id to get data for.
 */
inline const TFuncData& FunctionTable(TIdent Ident)
{
//Table of functions defined by equations
	static std::vector<TFuncData> Table;
	if(Table.empty())
		for(TIdent Ident = CodeCsc; Ident <= CodeACoth; Ident = static_cast<TIdent>(Ident+1))
			Table.push_back(TFuncData(FunctionDefinition(Ident)));

	BOOST_ASSERT(Ident >= CodeCsc && Ident <= CodeACoth);
	return Table[Ident - CodeCsc];
}
//---------------------------------------------------------------------------
/** Returns true if a complex is finite for both real and imaginary part.
 *  \param C: Complex number to check.
 */
template<typename T>
inline bool IsFinite(const std::complex<T> &C)
{
	return boost::math::isfinite(C.real()) && boost::math::isfinite(C.imag());
}
//---------------------------------------------------------------------------
/** Returns true if a number is finite.
 *  \param C: Number to check.
 */
template<typename T>
inline bool IsFinite(const T &C)
{
	return boost::math::isfinite(C);
}
//---------------------------------------------------------------------------
/** Generic operator! which returns true if the complex number is 0
 *  \param C: Complex number to check
 */
template<typename T>
inline bool operator!(const std::complex<T> &C)
{
	return !real(C) && !imag(C);
}
//---------------------------------------------------------------------------
/** Returns the conjugate of a real number;
 *  Exists for generic use with conj(std::complex<T>)
 *  \param x: Number which will be returned.
 */
inline long double Conj(long double x)
{
	return x;
}
//---------------------------------------------------------------------------
/** Returns the conjugate of a complex number.
 *  \param x: Complex number to conjugate.
 */
inline TComplex Conj(TComplex x)
{
  return conj(x);
}
//---------------------------------------------------------------------------
/** Returns the inverse hyperbolic sine to x
 *  \param x: Number to calculate inverse hyperbolic sine on.
 */
template<typename T>
inline T asinh(T x)
{
  return log(x + sqrt(x*x + 1.0L));
}
//---------------------------------------------------------------------------
/** Returns the inverse hyperbolic cosine to x
 *  \param x: Number to calculate inverse hyperbolic cosine on
 */
template<typename T>
inline T acosh(T x)
{
  T Temp = sqrt(x*x - 1.0L);
  return log(x + Temp);
}
//---------------------------------------------------------------------------
/** Returns the inverse hyperbolic tangent to x
 *  \param x: Number to calculate inverse hyperbolic tangent on.
 */
template<typename T>
T atanh(T x)
{
	if(x != 1.0L)
	{
		T Result = log((1.0L + x) / (1.0L - x));
		return Result * 0.5L;
	}
	return std::numeric_limits<long double>::quiet_NaN();
}
//---------------------------------------------------------------------------
/** Returns the inverse sine to x
 *  \param C: Complex number to calculate inverse sine on
 */
template<typename T>
inline std::complex<T> asin(const std::complex<T> &C)
{
	return boost::math::asin(std::complex<double>(C));
}
//---------------------------------------------------------------------------
/** Returns the inverse cosine to x
 *  \param C: Complex number to calculate inverse cosine on.
 */
template<typename T>
inline std::complex<T> acos(const std::complex<T> &C)
{
	return boost::math::acos(std::complex<double>(C));
}
//---------------------------------------------------------------------------
/** Returns the inverse tangent to x
 *  \param C: Complex number to calculate inverse tangent on
 */
template<typename T>
inline std::complex<T> atan(const std::complex<T> &C)
{
	return boost::math::atan(std::complex<double>(C));
}
//---------------------------------------------------------------------------
/** Truncate the argument, i.e. returns x without the decimals.
 *  \param C: Number to truncate
 */
template<typename T>
inline T Trunc(const T &C)
{
  return static_cast<long long>(C);
}
//---------------------------------------------------------------------------
/** Truncate a complex number, i.e. drop the decimals on both real and imaginary parts.
 *  \param C: Complex number to truncate.
 */
template<typename T>
inline std::complex<T> Trunc(const std::complex<T> &C)
{
  return std::complex<T>(Trunc(C.real()), Trunc(C.imag()));
}
//---------------------------------------------------------------------------
/** Returns the complex number where ceil is calculated on both real and imaginary part.
 *  \param C: Complex number to ceil.
 */
template<typename T>
inline std::complex<T> ceil(const std::complex<T> &C)
{
  return std::complex<T>(std::ceil(C.real()), std::ceil(C.imag()));
}
//---------------------------------------------------------------------------
/** Returns the complex number where floor is calculated on both real and imaginary part.
 *  \param C: Complex number to floor.
 */
template<typename T>
inline std::complex<T> floor(const std::complex<T> &C)
{
  return std::complex<T>(std::floor(C.real()), std::floor(C.imag()));
}
//---------------------------------------------------------------------------
/** Returns C1 modulo C2 where C1 and C2 are complex numbers.
 *  \param C1: The complex dividend.
 *  \param C2: The complex divisor.
 *  \return The complex remainder of the modulo.
 *  NaN is retunrned if either C1 or C2 has an imaginary part.
 */
template<typename T>
inline std::complex<T> fmod(const std::complex<T> &C1, const std::complex<T> &C2)
{
	if(C1.imag() || C2.imag())
		return std::numeric_limits<long double>::quiet_NaN();
	return std::fmod(C1.real(), C2.real());
}
//---------------------------------------------------------------------------
/** Return a complex number with the minimum real part and minimum imaginary part
 *  \param C1, C2: Complex numbers to find minimum of.
 */
template<typename T>
inline std::complex<T> Minimum(const std::complex<T> &C1, const std::complex<T> &C2)
{
	return std::complex<T>(std::min(C1.real(), C2.real()), std::min(C1.imag(), C2.imag()));
}
//---------------------------------------------------------------------------
/** Return a number with the minimum real part and minimum imaginary part
 *  \param C1, C2: Numbers to find minimum of.
 */
template<typename T>
inline T Minimum(const T &C1, const T &C2)
{
	return C1 < C2 ? C1 : C2;
}
//---------------------------------------------------------------------------
/** Return a complex number with the maximum real part and maximum imaginary part
 *  \param C1, C2: Complex numbers to find maximum of.
 */
template<typename T>
inline std::complex<T> Maximum(const std::complex<T> &C1, const std::complex<T> &C2)
{
	return std::complex<T>(std::max(C1.real(), C2.real()), std::max(C1.imag(), C2.imag()));
}
//---------------------------------------------------------------------------
/** Return a number with the maximum real part and maximum imaginary part
 *  \param C1, C2: Numbers to find maximum of.
 */
template<typename T>
inline T Maximum(const T &C1, const T &C2)
{
	return C1 > C2 ? C1 : C2;
}
//---------------------------------------------------------------------------
/** Returns Number rounded to the given number of decimals
 *  \param Number: Number to round
 *  \param Decimals: Decimals to round to.
 */
template<typename T>
inline T Round(const T &Number, int Decimals)
{
  T DecMul = pow(10.0, Decimals);
	return Trunc(Number * DecMul + (Number < 0 ? -0.5L : 0.5L)) / DecMul;
}
//---------------------------------------------------------------------------
/** Returns a complex number rounded to the given number of decimals
 *  \param Number: Complex number to round
 *  \param Decimals: Decimals to round to.
 */
template<typename T>
inline std::complex<T> Round(const std::complex<T> &Number, int Decimals)
{
  return std::complex<T>(Round(real(Number), Decimals), Round(imag(Number), Decimals));
}
//---------------------------------------------------------------------------
/** Calculate Greatest Common Divisor.
 *  \param a, b: Numbers to find greatest common divisor for.
 */
int CalcGcd(int a, int b)
{
  if(b > a)
    return CalcGcd(b, a);

  if(b == 0)
    return a;

  return CalcGcd(b, a%b);
}
//---------------------------------------------------------------------------
/** Return factorial of x, i.e. x!
 *  \param x: Number to calculate factorial of.
 */
long double Fact(unsigned x)
{
  static std::vector<long double> List; //Cache for Fact(0)..Fact(100)
  if(List.empty())
  {
    List.reserve(101);
    List.push_back(1); //Fact(0) = 1
		for(unsigned I = 1; I <= 100; I++)
      List.push_back(List.back() * I);
  }

  if(x <= 100)
    return List[x];

  long double Result = List[100];
  do
    Result *= x;
  while(--x > 100);
  return Result;
}
//---------------------------------------------------------------------------
/** Calculates the gamma function ( n! = Gamma(n+1) ), Gamma(z) is not defined for z=0
 *  Implemented as Lanczos approximation from description on http://www.rskey.org/gamma.htm
 *  The alogirthm is a modification of the one found in Numerical Recipes in C
 *  \param z: The argument to the Gamma function
 */
template<typename T>
T Gamma(const T &z)
{
  if(real(z) < 0)
    return T(PI)/(z * Gamma(-z) * sin(-T(PI)*z));

  const int N = 7;
  long double q[N] = {75122.6331530, 80916.6278952, 36308.2951477, 8687.24529705, 1168.92649479, 83.8676043424,	2.50662827511};
  T Sum = 0;
  for(int n = 0; n < N; n++)
		Sum += q[n]*pow(z, n);

  T Product = 1;
  for(unsigned n = 0; n < N; n++)
    Product *= z + T(n);

  T Result = Sum / Product * pow(z + T(5.5), z + T(0.5)) * exp(-z - T(5.5));
  return Result;
}
//---------------------------------------------------------------------------
/** An implementation of the Omega function (Lambert's W function) where the
 *  inverse is given by f(W)=W*e^W
 *  W(z) is defined for all complex numbers except for real values z < -1/e
 *  \param z: A real or complex number to evaluate W(z) for
 *  \param ErrorCode: Set to an error code if evaluation fails. Unchanged otherwise
 *  \return The omega function W evaluated for z.
 */
template<typename T>
T Omega(T z, TErrorCode &ErrorCode)
{
	const double eps = 0.00001; //Accuracy
	T p, w;
	if(!z)
		return 0.0;
	if(abs(z) < 1)
	{
		p = sqrt(2.0L * (EULER * z + 1.0L));
		w = -1.0L + p-p*p/3.0L + 11.0L/72.0*p*p*p;
	}
	else
		w = log(z);

	if(!IsFinite(w))
	{
		ErrorCode = ecComplexError; //Not defined for z<-1/e when we are not using complex numbers
		return -1;
	}

	if(abs(z) > 3)
		w = w-log(w);
	else if(w == -1.0L) //Definition from MathWorld
		return -1;

	for(int i = 0; i < 20; i++)
	{
		T e = exp(w);
		T t = w*e-z;
		t = t/(e*(w+1.0L) - 0.5L*(w+2.0L)*t/(w+1.0L));
		w = w-t;
		if(abs(t) < eps*(1.0+abs(w)))
			return w;
	}

	ErrorCode = ecEvalFailed; //No accurate solution found
	return -1;
}
//---------------------------------------------------------------------------
/** Calculates the binomial coefficient of n and k.
 *  \param n, k: These are the input binomial coefficients.
 */
long double Binomial(unsigned n, unsigned k)
{
	return Fact(n) / (Fact(n-k) * Fact(k));
}
//---------------------------------------------------------------------------
/** Helper function for the zeta function.
 */
const int ZetaN = 50; //Increase to increase precision of Zerta() function
long double ZetaHelper(unsigned k)
{
  long double Result = 0;
  for(int j = k ; j <= ZetaN; j++)
    Result += Fact(ZetaN + j - 1) * pow(4.0, j) / (Fact(ZetaN - j) * Fact(2*j));
  return ZetaN * Result;
}
//---------------------------------------------------------------------------
/** Calculate Zeta(s)
 *  See http://mathworld.wolfram.com/RiemannZetaFunction.html
 *  and http://numbers.computation.free.fr/Constants/Miscellaneous/zetaevaluations.pdf
 *  \param s: Parameter to zeta function
 *  \param ErrorCode: Set to an error code in case of error.
 */
template<typename T>
T Zeta(T s, TErrorCode &ErrorCode)
{
  if(real(s) == 1)
  {
		ErrorCode = ecNotDefError;
    return 0;
  }

  if(s == 0.0L) //Special case that we cannot handle else
    return -0.5L;

  if(real(s) < 0.5)
    return pow(T(2), s) * pow(T(PI), s-1.0L) * sin(PI/2 * s) * Gamma(1.0L - s) * Zeta(1.0L - s, ErrorCode);

  T Result = 0;
  for(unsigned k = 1; k <= ZetaN; k++)
    Result += ((k % 2) ? ZetaHelper(k) : -ZetaHelper(k)) / pow(T(k), s);
  return 1.0L / (ZetaHelper(0) * (1.0L - pow(T(2.0L), 1.0L-s))) * Result;
}
//---------------------------------------------------------------------------
/** Evaluate the normal distribution with given mean and standard deviation for the value x
 *  \param x: The variate
 *  \param Mean: Mean
 *  \param Deviation: Standard deviation. May not be 0.
 */
long double NormalDist(long double x, long double Mean, long double Deviation)
{
  const long double Sqrt2_SqrtPi = 0.564189583547756286948L * 0.707106781186547524401L; //1/sqrt(pi)*1/sqrt(2)
  return std::exp(-(x-Mean)*(x-Mean) / (2*Deviation*Deviation)) * Sqrt2_SqrtPi / Deviation;
}
//---------------------------------------------------------------------------
/** Compare t1 and t2 the way given by Compare
 *  \param t1, t2: Numbers to compare
 *  \param Compare: Specifies how the two numbers are compared.
 *  \param ErrorCode: Set to an error code if it is not possible to compare complex numbers.
 */
template<typename T>
bool LowLevelCompare(const T &t1, const T &t2, TCompareMethod Compare, TErrorCode &ErrorCode)
{
  if(Compare != cmEqual && Compare != cmNotEqual)
    if(imag(t1) || imag(t2))
			ErrorCode = ecComplexError;
  switch(Compare)
  {
    case cmEqual:         return t1 == t2;
    case cmNotEqual:      return t1 != t2;
    case cmLess:          return real(t1) < real(t2);
    case cmGreater:       return real(t1) > real(t2);
    case cmLessEqual:     return real(t1) <= real(t2);
    case cmGreaterEqual:  return real(t1) >= real(t2);
    default:              ErrorCode = ecInternalError; return 0; //Error
  }
}
//---------------------------------------------------------------------------
/** Compare two long double numbers.
 *  The numbers are converted to double before they are compared to ensure the least
 *  significant bits are not compared. This should ensure that 0.01*20 = 0.2
 *  \param t1, t2: Numbers to compare
 *  \param Compare: Specifies how the two numbers are compared.
 *  \param ErrorCode: Set to an error code if it is not possible to compare complex numbers.
 */
inline bool Compare(const long double &t1, const long double &t2, TCompareMethod Compare, TErrorCode &ErrorCode)
{
  return LowLevelCompare(static_cast<double>(t1), static_cast<double>(t2), Compare, ErrorCode);
}
//---------------------------------------------------------------------------
/** Compare two std::complex<long double> numbers.
 *  The numbers are converted to std::complex<double> before they are compared to
 *  ensure the least significant bits are not compared. This should ensure that 0.01*20 = 0.2
 *  \param t1, t2: Numbers to compare
 *  \param Compare: Specifies how the two numbers are compared.
 *  \param ErrorCode: Set to an error code if it is not possible to compare complex numbers.
 */
inline bool Compare(const TComplex &t1, const TComplex &t2, TCompareMethod Compare, TErrorCode &ErrorCode)
{
  return LowLevelCompare(static_cast<std::complex<double> >(t1), static_cast<std::complex<double> >(t2), Compare, ErrorCode);
}
//---------------------------------------------------------------------------
/** Returns a^(b/c) for real numbers
 *  \param a,b,c: Numbers to calculate on. c may not be 0
 */
long double PowDiv(const long double &a, const long double &b, const long double &c)
{
	if(!b) //Define: 0^0 = 1
    return 1;

  int bInt = b;
  int cInt = c;

  //If a is negative and both b and c are integers
  if(a < 0 && bInt == b && cInt == c)
  {
    //Reduce the fraction till either b or c is odd
    while((bInt & 0x01) == 0 && (cInt & 0x01) == 0)
      bInt >>= 1, cInt >>= 1;

    if(cInt & 0x01)
    {
      //If both b and c are odd integers, a is negative (and we are using real numbers)
      if(bInt & 0x01)
        return -pow(-a, static_cast<long double>(bInt)/cInt);
      //If b is even, c is odd, and a is negative
      else
        return pow(-a, static_cast<long double>(bInt)/cInt);
    }
  }
  return pow(a, b/c);
}
//---------------------------------------------------------------------------
/** Returns a^b for complex numbers
 */
template<typename T>
std::complex<T> Pow(const std::complex<T> &a, const std::complex<T> &b)
{
  if(!b) //Define: 0^0 = 1
		return 1;

	//pow(0, n) is undefined if n has an imaginary part or is real and less than 0
	if(!a && !imag(b) && real(b) > 0)
		return 0; //pow(0,b) seems to give problems with complex numbers

	if(!a && (real(b) < 0 || imag(b))) //pow(complex(0), complex(-2.8,0)) crashed with bcc 6.13 (C++ Builder 2009)
		return std::numeric_limits<long double>::quiet_NaN();

  //Calculations with complex numbers may return a complex number if Temp<0,
  //e.g. (-2)^2 = 4+i4.3368E-19; Because of this
  //evaluating f(x)=x/(x^2-4) results in f(-2)=4.6117E+18i
  if(!imag(a) && !imag(b) && Trunc(b) == b)
    return pow(real(a), real(b));

  //Calculations with complex numbers may return a complex number if a<0,
  //and b is not an integer
  return pow(a, b);
}
//---------------------------------------------------------------------------
/** Raise a to the power of b for real numbers.
 */
template<typename T>
inline T Pow(const T &a, const T &b)
{
  if(!b)
		return 1;
  return pow(a, b);
}
//---------------------------------------------------------------------------
/** Returns a^(b/c) for real numbers
 *  \param a,b,c: Numbers to calculate on. c may not be 0
 */
template<typename T>
inline std::complex<T> PowDiv(const std::complex<T> &a, const std::complex<T> &b, const std::complex<T> &c)
{
  return Pow(a, b/c);
}
//---------------------------------------------------------------------------
template<typename T> struct TComplexTrait
{
	static const bool HasImagUnit = false;
  static const T ImagUnit() {return 0;}
};
template<typename T> struct TComplexTrait<std::complex<T> >
{
  static const bool HasImagUnit = true;
  static const std::complex<T> ImagUnit() {return std::complex<T>(0, 1);}
};
//---------------------------------------------------------------------------
/** Evaluate Data with x as variable and returns the result
 *  \param DynData: Contains information used for the evaluation
 *  \return The result of the evaluation
 */
template<typename T>
T TFuncData::CalcF(TDynData<T> &DynData) const
{
  try
  {
    if(Data.empty())
    {
			DynData.Error.ErrorCode = ecNoFunc;
			return 0;
		}
		DynData.Error.ErrorCode = ecNoError;
		TConstIterator Iter = Data.begin();
		return CalcFunc(Iter, DynData);
	}
	catch(...)
	{
    DynData.Error.ErrorCode = ecCalcError;
		return std::numeric_limits<long double>::quiet_NaN();
  }
}

//---------------------------------------------------------------------------
/** Evaluate from the place pointed to by Iter with x as variable and returns the result
 *  \param Iter: Iterator pointing at the element to start the evaluation. Iter is updated to point to one after the last element.
 *  \param DynData: Contains information used for the evaluation.
 *  \return The result of the evaluation.
 */
template<typename T>
T TFuncData::CalcFunc(TConstIterator &Iter, TDynData<T> &DynData)
{
	TErrorCode &ErrorCode = DynData.Error.ErrorCode;
	const TElem &Elem = *Iter++;
	switch(Elem.Ident)
	{
		case CodeNumber:
//			return boost::any_cast<long double>(Elem.Value);
			return *boost::unsafe_any_cast<long double>(&Elem.Value);

    case CodeArgument:
      BOOST_ASSERT(DynData.Args);
			return DynData.Args[Elem.Arguments];

    case CodePi:
      return PI;

    case Codee:
      return EULER;

    case Codei:
      if(TComplexTrait<T>::HasImagUnit)
        return TComplexTrait<T>::ImagUnit();
      ErrorCode = ecComplexError;
      return 0;

    case CodeUndef:
      ErrorCode = ecNotDefError;
      return 0;

    case CodeRand:
      //This will generate the same sequence if it is called from a new thread each time
      //unless randomize() is called each time a new thread starts
      return rand()/ static_cast<long double>(RAND_MAX);

    case CodeInf:
      return std::numeric_limits<long double>::infinity();

    case CodeConst:
      return *boost::any_cast<boost::shared_ptr<long double> >(Elem.Value);

		case CodeIntegrate:
		{
			//Check for backward compatibility
			long double *Constant = Elem.Value.empty() ? NULL :
				&*boost::any_cast<boost::shared_ptr<long double> >(Elem.Value);
			TConstIterator F = Iter;
			Iter = FindEnd(Iter);
			T Min = real(CalcFunc(Iter, DynData));
			T Max = real(CalcFunc(Iter, DynData));
			if(imag(Min) != 0 || imag(Max) != 0)
				ErrorCode = ecComplexError;
			if(ErrorCode)
				return 0;
			return Integrate(F, real(Min), real(Max), 1E-3, DynData, Constant);
    }

    case CodeSum:
		case CodeProduct:
    {
			boost::shared_ptr<long double> Variable = boost::any_cast<boost::shared_ptr<long double> >(Elem.Value);
      TConstIterator F = Iter;
      Iter = FindEnd(Iter);
			T Min = CalcFunc(Iter, DynData);
			T Max = CalcFunc(Iter, DynData);
      if(imag(Min) || imag(Max))
      {
        ErrorCode = ecComplexError;
        return 0;
      }
			T Sum = Elem.Ident == CodeSum ? 0 : 1;
			for(long double i = real(Min); i <= real(Max); i++)
			{
				TConstIterator Temp = F;
				*Variable = i;
				if(Elem.Ident == CodeSum)
					Sum += CalcFunc(Temp, DynData);
				else
          Sum *= CalcFunc(Temp, DynData);
      }
      return Sum;
    }

    case CodeCustom:
		{
			if(++DynData.Recursion > MaxRecursion)
			{
				ErrorCode = ecRecusionLimit;
				return 0;
			}

			T *Values = Elem.Arguments > 0 ? new T[Elem.Arguments] : NULL;
			for(unsigned I = 0; I < Elem.Arguments; I++)
			{
				Values[I] = CalcFunc(Iter, DynData);
				if(ErrorCode != ecNoError)
					return 0;
			}
			const boost::shared_ptr<TBaseCustomFunc> &Func = *boost::unsafe_any_cast<boost::shared_ptr<TBaseCustomFunc> >(&Elem.Value);
			if(Func)
			{
				const T *OldArgs = DynData.Args;
				DynData.Args = Values;
				T Result = Func->DynCall(DynData);
				DynData.Args = OldArgs;
				DynData.Recursion--;
				delete[] Values;
				return Result;
			}

			ErrorCode = ecSymbolNotFound;
			DynData.Error.Str = Elem.Text;
			delete[] Values;
			return 0;
		}

		case CodeOr:
		{
			T Temp = CalcFunc(Iter, DynData);
      //An error is handles as evaluation to 0
      if(!!Temp && ErrorCode == ecNoError)
      {
        Iter = FindEnd(Iter);
        return 1;
      }
      ErrorCode = ecNoError;
			Temp = CalcFunc(Iter, DynData);
      if(!!Temp && ErrorCode == ecNoError)
        return 1;
      ErrorCode = ecNoError;
			return 0;
    }
  }

	T Temp = CalcFunc(Iter, DynData);
  if(ErrorCode != ecNoError)
    return 0;                    //! \bug I think we may need to adjust Iter here
	switch(Elem.Ident)
  {
    case CodeAdd:
			return Temp + CalcFunc(Iter, DynData);

		case CodeSub:
			return Temp - CalcFunc(Iter, DynData);

    case CodeMul:
		{
			T Temp2 = CalcFunc(Iter, DynData);
      return Temp * Temp2;
    }

    case CodeDiv:
    {
			T Temp2 = CalcFunc(Iter, DynData);
      if(!!Temp2)
        return Temp / Temp2;
      if(!ErrorCode)
        ErrorCode = ecDivByZero;
      return 0;
		}

    case CodePow:
    {
			T Temp2 = CalcFunc(Iter, DynData);
			Temp = Pow(Temp, Temp2);
			if(!IsFinite(Temp))
				ErrorCode = ecPowCalcError;
			return Temp;
		}

		case CodeRoot: //y=root(n,x)
		{
			T Temp2 = CalcFunc(Iter, DynData);
      if(!Temp)  //Cannot raise to root 0
      {
				ErrorCode = ecDivByZero;
        return 0;
      }

      Temp = PowDiv(Temp2, T(1), Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecPowCalcError;
      return Temp;
    }

		case CodePowDiv:
		{  //Temp^(Temp2/Temp3)
			T Temp2 = CalcFunc(Iter, DynData);
			T Temp3 = CalcFunc(Iter, DynData);
			if(!Temp3)  //Cannot raise to root 0
			{
				ErrorCode = ecDivByZero;
				return 0;
			}

			Temp = PowDiv(Temp, Temp2, Temp3);
			if(!IsFinite(Temp))
				ErrorCode = ecPowCalcError;
      return Temp;
    }

		case CodeMod:
    {
			T Temp2 = CalcFunc(Iter, DynData);
			Temp = fmod(Temp, Temp2);
			if(!IsFinite(Temp))
				ErrorCode = ecComplexError;
			//The C++ implementation doesn't follow the documentation, which also seems to be wrong.
			//We want the sign of mod(m,n) to be the same as the sign of n.
			if(!!Temp && (real(Temp) >= 0) != (real(Temp2) >= 0))
				return Temp + Temp2;
			return Temp;
		}
		case CodeRound:
		{
			T Temp2 = CalcFunc(Iter, DynData);
      if(imag(Temp2))
			{
        ErrorCode = ecComplexError;
        return std::numeric_limits<long double>::quiet_NaN();
      }
      return Round(Temp, static_cast<int>(real(Temp2)));
    }

    case CodeNeg:
      return -Temp;

    case CodeSin:
    case CodeCos:
    case CodeTan:
			if(DynData.Trigonometry == Degree)
        Temp *= PI / 180;
			switch(Elem.Ident)
      {
        case CodeSin: Temp = sin(Temp); break;
        case CodeCos: Temp = cos(Temp); break;
        case CodeTan:
          Temp = tan(Temp);  //Notice that this will throw an exception for PI/2 when using comlex numbers
					if(!IsFinite(Temp))
						ErrorCode = ecTanError;
			}

			if(!IsFinite(Temp) && !ErrorCode)
				ErrorCode = ecTotalLoss; //We should also check the value of errno
      return Temp;

		case CodeASin:
    case CodeACos:
    case CodeATan:
    case CodeArg:
      switch(Elem.Ident)
      {
        case CodeASin:
          Temp = asin(Temp); break;
        case CodeACos:
          Temp = acos(Temp); break;
        case CodeATan:
          Temp = atan(Temp); break;
        case CodeArg:
					Temp = arg(Temp);
					if(!IsFinite(Temp))
					{
						ErrorCode = ecArgError;
						return 0;
					}
			}
			if(!IsFinite(Temp))
				ErrorCode = ecArcError;
      if(DynData.Trigonometry == Degree)
        Temp *= (180 / PI);
      return Temp;

    case CodeLog:
      Temp = log10(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecLogError;
			return Temp;

		case CodeLn:
			Temp = log(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecLogError;
			return Temp;

    case CodeLogB:
    {
			T Temp2 = CalcFunc(Iter, DynData);
      if(ErrorCode)
				return 0;
      if(imag(Temp2))
			{
        ErrorCode = ecComplexError;
				return 0;
      }
      Temp = log(Temp) / log(Temp2);
			if(!IsFinite(Temp) || real(Temp2) <= 0)
				ErrorCode = ecLogError;
			return Temp;
		}

		case CodeSqr:
			return Temp * Temp;

		case CodeExp:
			Temp = exp(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecPowCalcError;
			return Temp;

		case CodeSqrt:
			Temp = sqrt(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecSqrtError;
			return Temp;

		case CodeFact:
			if(!imag(Temp) && !fmod(real(Temp), 1) && real(Temp) >= 0)
				return Fact(static_cast<unsigned>(real(Temp)));
			ErrorCode = ecFactError;
			return 0;

		case CodeSign:
      if(!Temp) //if(Temp == 0)
        return 0;
			if(TComplexTrait<T>::HasImagUnit)
        return Temp / abs(Temp); //As defined by MathWorld
      return real(Temp) < 0 ? -1 : 1; //Optimized version for real numbers

    case CodeU:
      if(imag(Temp))
        ErrorCode = ecNotDefError;
      return real(Temp) < 0 ? 0 : 1;

		case CodeSinh:
      if(real(Temp) < -8191) //BCC 5.6.4 has an error. It returns a positive number for sinh(x) when x < -8191
        ErrorCode = ecHugeValReturned;
      Temp = sinh(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecHugeValReturned;
			return Temp;

		case CodeCosh:
			Temp = cosh(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecHugeValReturned;
      return Temp;

    case CodeTanh:  return tanh(Temp);
		case CodeASinh: return asinh(Temp);
		case CodeACosh:
			Temp = acosh(Temp);
			if(!IsFinite(Temp))
        ErrorCode = ecACoshError;
      return Temp;

		case CodeATanh:
			Temp = atanh<T>(Temp);
			if(!IsFinite(Temp))
				ErrorCode = ecATanhError;
      return Temp;

    case CodeAbs:   return abs(Temp);
		case CodeConj:  return Conj(Temp);
    case CodeRe:    return real(Temp);
    case CodeIm:    return imag(Temp);
    case CodeTrunc: return Trunc(Temp);
    case CodeFract: return Temp - Trunc(Temp);
    case CodeCeil:  return ceil(Temp);
    case CodeFloor: return floor(Temp);
		case CodeMin:
			for(unsigned I = 1; I < Elem.Arguments; I++)
				Temp = Minimum(Temp, CalcFunc(Iter, DynData));
			return Temp;

    case CodeMax:
			for(unsigned I = 1; I < Elem.Arguments; I++)
				Temp = Maximum(Temp, CalcFunc(Iter, DynData));
			return Temp;

    case CodeRange:
			return Maximum(Temp, Minimum(CalcFunc(Iter, DynData), CalcFunc(Iter, DynData)));

    case CodeCompare1:
    {
			T Temp2 = CalcFunc(Iter, DynData);
      return Compare(Temp, Temp2, boost::any_cast<TCompareMethod>(Elem.Value), ErrorCode);
    }

    case CodeCompare2:
    {
			T Temp2 = CalcFunc(Iter, DynData);
			T Temp3 = CalcFunc(Iter, DynData);
      std::pair<TCompareMethod,TCompareMethod> Comp = boost::any_cast<std::pair<TCompareMethod,TCompareMethod> >(Elem.Value);
      return Compare(Temp, Temp2, Comp.first, ErrorCode) && Compare(Temp2, Temp3, Comp.second, ErrorCode);
    }

    case CodeNot:
      return !Temp;

    case CodeAnd:
			return !!Temp & !!CalcFunc(Iter, DynData); //Do not use logical and because of hort-circuit rule

    case CodeXor:
			return !!Temp ^ !!CalcFunc(Iter, DynData);

    case CodeCsc:
    case CodeSec:
    case CodeCot:
    case CodeASec:
    case CodeACsc:
    case CodeACot:
    case CodeCsch:
    case CodeSech:
    case CodeCoth:
    case CodeASech:
    case CodeACsch:
    case CodeACoth:
    {
      const T *OldArgs = DynData.Args;
      DynData.Args = &Temp;
			Temp = FunctionTable(Elem.Ident).CalcF(DynData);
      DynData.Args = OldArgs;
      return Temp;
    }

    case CodeGamma:
      if(!Temp)
			{
        ErrorCode = ecNotDefError;
        return 0;
      }
      return Gamma(Temp);

		case CodeBeta:
    {
			T Temp2 = CalcFunc(Iter, DynData);
      return Gamma(Temp) * Gamma(Temp2) / Gamma(Temp + Temp2); //Gamma will never return 0
    }

    case CodeW:
      return Omega(Temp, ErrorCode);

    case CodeZeta:
      return Zeta(Temp, ErrorCode);

    case CodeIf:
    case CodeIfSeq:
      for(unsigned I = 1; I < Elem.Arguments; I += 2)
      {
				if(!!Temp)
        {
					Temp = CalcFunc(Iter, DynData);
          while(++I < Elem.Arguments)
						Iter = FindEnd(Iter);
          return Temp;
        }

        Iter = FindEnd(Iter);
        if(I+1 < Elem.Arguments)
					Temp = CalcFunc(Iter, DynData);
      }
			if(Elem.Arguments % 2 == 0)
				ErrorCode = ecNotDefError;
      return Temp;

    case CodeDNorm:
    {
      T Temp2;
      T Temp3;
      if(Elem.Arguments == 1)
        Temp2 = 0, Temp3 = 1;
      else
      {
				Temp2 = CalcFunc(Iter, DynData);
				if(ErrorCode)
					return 0;
				Temp3 = CalcFunc(Iter, DynData);
			}
			if(!ErrorCode && (imag(Temp) || imag(Temp2) || imag(Temp3)))
				ErrorCode = ecComplexError;
			if(!ErrorCode && real(Temp3) <= 0)
				ErrorCode = ecNotDefError;
			if(ErrorCode)
				return 0;
			return NormalDist(real(Temp), real(Temp2), real(Temp3));
		}

		default:
			ErrorCode = ecInternalError;
			return std::numeric_limits<long double>::quiet_NaN();
	}
}
//---------------------------------------------------------------------------
/** Call back function called by the GNU Scientific Library (GSL) to evaluate a
 *  function.
 */
template<typename T>
double TFuncData::CalcGSLFunc(double x, void *Params)
{
	TGSLFunction<T> *Function = reinterpret_cast<TGSLFunction<T>*>(Params);
	T X = x;
	const T *OldArgs = Function->DynData.Args; //Workaround for backward compatibility
	if(Function->Value) //Check for backward compatibility
		*Function->Value = x;
	else
		Function->DynData.Args = &X;
	Function->DynData.Error.ErrorCode = ecNoError;
	TConstIterator Iter = Function->Func;
	T Result = CalcFunc(Iter, Function->DynData);
	Function->DynData.Args = OldArgs;
	if(Function->DynData.Error.ErrorCode != ecNoError || imag(Result) != 0)
		return std::numeric_limits<double>::quiet_NaN();
	return real(Result);
}
//---------------------------------------------------------------------------
boost::recursive_mutex Mutex;
/** Returns the numeric integrale of the functions pointed to by Func from Min to Max.
 *  The integral is calculated by the GNU Scientific Library (GSL)
 *  It applies the Gauss-Kronrod 21-point integration rule adaptively until the estimated
 *  relative error is less than give in RelError.
 *  \param Func: Iterator pointing to the place to start the evaluation.
 *  \param Min:  Start value
 *  \param Max:  End value
 *  \param RelError: Estimated relative error to stop integration at.
 *  \param Trigonometry: Used to tell if we should use radians or degrees for trigonometric functions.
 *  \param ErrorCode: Filled with error information on return.
 */
 template<typename T>
 double TFuncData::Integrate(TConstIterator Func, double Min, double Max, double RelError, TDynData<T> &DynData, long double *Value)
{
	const int MaxSubIntervals = 1000;
	if(Max < Min)
		return -Integrate(Func, Max, Min, RelError, DynData, Value);
	TGSLFunction<T> Function(Func, DynData, Value);
	gsl_integration_workspace *w = gsl_integration_workspace_alloc(MaxSubIntervals);
	gsl_function F = {CalcGSLFunc<T>, &Function};

	//! \todo Find a metter way to do this without locking
	boost::lock_guard<boost::recursive_mutex> Guard(Mutex);

	double Result, Error;
	unsigned ErrorResult;
	if(boost::math::isfinite(Min) && boost::math::isfinite(Max))
		ErrorResult = gsl_integration_qags(&F, Min, Max, 0, RelError, MaxSubIntervals, w, &Result, &Error);
	else if(!boost::math::isfinite(Min) && !boost::math::isfinite(Max))
		ErrorResult = gsl_integration_qagi(&F, 0, RelError, MaxSubIntervals, w, &Result, &Error);
	else if(!boost::math::isfinite(Min))
		ErrorResult = gsl_integration_qagil(&F, Max, 0, RelError, MaxSubIntervals, w, &Result, &Error);
	else
		ErrorResult = gsl_integration_qagiu(&F, Min, 0, RelError, MaxSubIntervals, w, &Result, &Error);
	gsl_integration_workspace_free(w);
	if(ErrorResult == GSL_EROUND)    //Should we give a warning?
		DynData.Error.ErrorCode = ecNoError; //ecNoAccurateResult;
	else if(ErrorResult != 0)
		DynData.Error.ErrorCode = ecNoResult;
	else if(!boost::math::isfinite(Result))
		DynData.Error.ErrorCode = ecNoResult;
	else if(Result != 0 && Error / Result > RelError)
		DynData.Error.ErrorCode = ecNoAccurateResult;
	else
		DynData.Error.ErrorCode = ecNoError;
	if(Value != NULL)
  	*Value = 0; //Leave Value set 0 zero when unused to make compare possible.
	return Result;
}
//---------------------------------------------------------------------------
/** Returns the numeric integrale of the functions pointed to by Func from Min to Max.
 *  The integral is calculated by the GNU Scientific Library (GSL)
 *  It applies the Gauss-Kronrod 21-point integration rule adaptively until it cannot
 *  get a better result.
 *  \param Func: Iterator pointing to the place to start the evaluation.
 *  \param Min:  Start value. May be -INF
 *  \param Max:  End value. May be INF
 *  \param RelError: Estimated relative error to stop integration at.
 *  \param Trigonometry: Used to tell if we should use radians or degrees for trigonometric functions.
 *  \throw ECalcError: If the maximum relative error specified in RelError is not reached.
 */
double TFuncData::Integrate(double Min, double Max, double RelError, TTrigonometry Trigonometry) const
{
	long double Value;
	ECalcError Error;
	TDynData<long double> DynData(&Value, Trigonometry, Error);
	double Result = Integrate(Data.begin(), Min, Max, RelError, DynData, &Value);
	if(Error.ErrorCode != ecNoError)
    throw Error;
  return Result;
}
//---------------------------------------------------------------------------
/** Called by the GNU Scientific Library on errors.
 */
void GSL_ErrorHandler(const char * reason, const char * file, int line, int gsl_errno)
{
	DEBUG_LOG(std::wclog << reason << std::endl);
}
//---------------------------------------------------------------------------
class TInitGSL
{
public:
	TInitGSL(){gsl_set_error_handler(GSL_ErrorHandler);}
} InitGSL;
//---------------------------------------------------------------------------
} //namespace Func32

//Math error handler
//Called on any math errors; Do not set errno. It is very slow
#ifdef __BORLANDC__
int _RTLENTRY _matherrl(_exceptionl *a)
{
	using namespace std;
//  DEBUG_LOG(std::wclog << "Math error: " << a->name << "(" << a->arg1 << ", " << a->arg2 << ")" << std::endl);
	if(a->type == UNDERFLOW) //Convert underflow errors to 0
		a->retval = 0;
	else
		a->retval = std::numeric_limits<long double>::quiet_NaN();
	return 1;
}
//---------------------------------------------------------------------------
//Math error handler
//Called on any math errors;
int _matherr(_exception *a)
{
	//Bug in RTL (cosl.asm) cosl() will call _matherr() instead of _matherrl() on error
	//Because of this a->arg1 is also wrong
	a->retval = std::numeric_limits<double>::quiet_NaN();
	return 1;
}
#endif
//---------------------------------------------------------------------------
//Instantiate versions of the template calculation functions for
//long double and std::complex<long double>
//Ignore by Doxygen
#ifndef DOXYGEN
template long double Func32::TFuncData::CalcF<long double>(TDynData<long double>&) const;
template Func32::TComplex Func32::TFuncData::CalcF<Func32::TComplex>(TDynData<Func32::TComplex>&) const;
#endif

