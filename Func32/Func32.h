/** \mainpage
 *  \section License
 *  \verbatim
 * Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * Func32 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Func32; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * \endverbatim
 * \ref license "Full license"\n
 * The author can be contacted via email at func32@padowan.dk
 *
 * \section Description
 * The C++ library %Func32 is used for calculations on mathematical functions. The
 * library conatins a number of classes in a polymorphic structure, which makes it
 * easy to do evaluations on different types of functions.

 * \section Use
 * Applications that use %Func32 must include %Func32.h and link to the source files
 * in the library or a library file created from the source files. The Spirit
 * parser library is necessary to compile %Func32. Spirit can be downloaded from
 * http://spirit.sourceforge.net or as part of Boost library collection from
 * http://boost.org. %Func32 has been tested with Spirit 1.6.1. Note that the whole
 * %Func32 library has been placed in namespace Func32.
 */
/** \page license License
 *  \verbinclude COPYING
 */
//---------------------------------------------------------------------------
#ifndef Func32H
#define Func32H
//---------------------------------------------------------------------------
#ifdef _DEBUG
	#define BOOST_ENABLE_ASSERT_HANDLER //Assertion handler defined in application
	namespace Func32 {void LogDebugString(const char *Str, const char *Str2);}
	//_DEBUG_ERROR2 is used in Dinkumware STL
	#define _DEBUG_ERROR2(mesg, where) Func32::LogDebugString("error " mesg " at ", where)
#else
	#define BOOST_DISABLE_ASSERTS
	#ifdef __BCPLUSPLUS__
		//Nasty hack: C++ Builder does not inline functions with throw specifications
		#undef throw
		#define throw(...)
	#endif
#endif
#include <vector>
#include <complex>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

//Save options; set data alignment to 64 bit; use smallest data for enum
#pragma option push -a8 -b-

namespace Func32
{

/** This enumeration descibes the different kind of errors that may occur in the
 *  library. Sometime the function TBaseFunc::ErrorCode() is used to report
 *  errors, but most function will throw an EFuncError exception or a descendent
 *  from EFuncError. EFuncError contains an ErrorCode variable indicating the
 *  type of error.
 */
enum TErrorCode
{
  ecNoError        =  0, //!< No error

  //Calculation errors
  ecPowCalcError    =  1, //!< Error while power raising
  ecTanError        =  2, //!< Tangent to 90°+p180°
  ecFactError       =  3, //!< fact must be a positive integer
  ecLogError        =  4, //!< Logarihm to number equal or less than zero
  ecSqrtError       =  5, //!< Sqrt on a negative number'
  ecComplexError    =  6, //!< Some error ocoured because the value had an imaginary part
  ecDivByZero       =  7, //!< Division by zero
  ecArcError        =  8, //!< Inverse trogonometric function out of range [-1;1]
  ecNotDefError     =  9, //!< Position not defined; returned by Sign(x) or U(x) if x=0, or by Undef
  ecATanhError      = 10, //!< atanh(x) is only defined for x=]-1;1] for real numbers
  ecACoshError      = 11, //!< acosh(x) is undefined for z < 1 for real numbers
  ecArgError        = 12, //!< arg(0) is undefined
  ecEvalFailed      = 13, //!< A complex function (W(z) for example) failed to evaluate an accurate result
  ecTotalLoss       = 14, //!< Argument would produce function result with total loss of significant digits, such as sin(10e70)
  ecSymbolNotFound  = 15, //!< Symbol does not exist or has wrong number of arguemnts; A custom function/constant is not updated
  ecRecusionLimit   = 16, //!< Recursive function has been called too many times
  ecHugeValReturned = 17, //!< Overflow: A function result was too large, e.g. sinh(20000)
  ecExtFuncError    = 18, //!< A call to an external custom function failed.

  //Parse errors
	ecOperatorError   = 50, //!< Operator cannot be placed here. Example "*5"
  ecLiteralExpected = 51, //!< A string literal was expected. Used for sum, product and integrate.
	ecNoEndPar        = 55, //!< End of parenthese missing
  ecArgCountError   = 56, //!< Invalid number of arguments when parsing function call
  ecInvalidCompare  = 57, //!< Compare operator misplaced. Only two compare operators allowed. "5<x<y<10" will give the error.
  ecInvalidNumber   = 58, //!< Number expected
  ecEmptyString     = 59, //!< String is empty
  ecCommaError      = 60, //!< Comma not allowed here; Use dot as decimal separator
  ecInvalidEndPar   = 61, //!< Unexpected end of parenthesis; Example: sin();  x^)
  ecFactorExpected  = 63, //!< A factor (Number, constant or function) was expected
  ecParAfterConst   = 64, //!< Parameter after constant or variable not allowed
  ecExpressionExp   = 65, //!< Expression expected
  ecUnknownVar      = 66, //!< Unknown variable found
  ecUnknownChar     = 67, //!< Unknown character found
  ecUnexpectedEnd   = 68, //!< The end of the expression was unexpected; Something else was expected here
  ecInvalidRelation = 69, //!< The expression is not a valid relation; Compare operator (=, <, <=, >, >=) missing.
  ecParseError      = 70, //!< Error parsing function text; Exact cause unknown

  //Other errors
  ecOverflow        = 71, //!< A calculation resulted in an overflow
  ecInvalidArgCount = 72, //!< Too few or too many arguments passed to a TCustomFunc instance
  ecInvalidValue    = 73, //!< An invalid value was used in the calculation
  ecTooFewPoints    = 74, //!< Not enough points for calculation
  ecIllegalName     = 75, //!< Built-in function names cannot be used for custom function names
  ecRecursiveDif    = 76, //!< Cannot differentiate recursve functions
  ecInvalidWeight   = 77, //!< Invalid number of wight values parsed to TrendLine()
  ecNoFunc          = 78, //!< No function defined
  ecNotDifAble      = 79, //!< Function cannot be differentiated
  ecErrorType       = 85, //!< The function passed as parameter is not correct type
  ecCalcError       = 86, //!< Not specified error under calculation; Overflow, division by zero, etc.
  ecBadGuess        = 87, //!< Too many iterations before solution found; bad guess
  ecNoResult        = 88, //!< There was no valid result, eg. from a trend line
  ecNoAccurateResult= 89, //!< A result with the required accuracy could not be found
  ecInternalError   = 99  //!< Unknown error
};

//---------------------------------------------------------------------------
/*! \brief This class and its decendents are only used for throwing exceptions. This
 *  specific class is used when one of the decendents doesn't fit.
 */
struct EFuncError : public std::exception
{
  TErrorCode ErrorCode; //!<The error code indicating the precise cause of the error.
  std::wstring Str;      //!< A text supplement to the error code; Can be an unknown variable name
  EFuncError(TErrorCode AErrorCode) : ErrorCode(AErrorCode) {}
  EFuncError(TErrorCode AErrorode, const std::wstring &AStr) : ErrorCode(AErrorode), Str(AStr) {}
  ~EFuncError() throw() {} //Required by g++ 3.3.3
  const char* what() const throw() {return "Func32::EFuncError";}
};

//!Type used to indicate where an error occured.
enum TErrorFunc
{
  efFunc,   //!< Error in standard function (TFunc)
  efParamX, //!< Error in X-part of parameter function (TParamFunc)
  efParamY, //!< Error in Y-part of parameter function (TParamFunc)
  efPolar   //!< Error in polar function (TPolarFunc)
};

//! Thrown when an error occured while parsing function.
struct EParseError : public EFuncError
{
  TErrorFunc ErrorFunc; //!< Indicates what type of function error occured
  int ErrorPos;         //!< Indicates position in text where error occured
  EParseError(TErrorCode AErrorCode, int AErrorPos, const std::wstring &AStr)
    : EFuncError(AErrorCode, AStr), ErrorPos(AErrorPos), ErrorFunc(efFunc) {}
  EParseError(TErrorCode AErrorCode=ecNoError, int AErrorPos=-1)
    : EFuncError(AErrorCode), ErrorPos(AErrorPos), ErrorFunc(efFunc) {}
  const char* what() const throw() {return "Func32::EParseError";}
};

//!Thrown when an error occured while calculating.
struct ECalcError : public EFuncError
{
  ECalcError(TErrorCode AErrorCode = ecNoError) : EFuncError(AErrorCode) {}
  ECalcError(TErrorCode AErrorCode, const std::wstring &Str) : EFuncError(AErrorCode, Str) {}
  const char* what() const throw() {return "Func32::ECalcError";}
};

template<typename T> class TCoordSet;
//!Struct with an (x,y) coordinate
template<typename T>
struct TCoordBase
{
  T x;
  T y;
};

template<typename T>
struct TCoord :  public TCoordBase<T>
{
  TCoord() {TCoordBase<T>::x=0; TCoordBase<T>::y=0;}
  TCoord(const T &X, const T &Y) {TCoordBase<T>::x=X; TCoordBase<T>::y=Y;}
  TCoord(const TCoordSet<T> &Set);
  TCoord(const TCoordBase<T> &Set) {TCoordBase<T>::x=Set.x; TCoordBase<T>::y=Set.y;}
};

//!Function used to create an (x,y) coordinate
template<typename T> inline TCoord<T> MakeCoord(const T &x, const T &y)
{
  return TCoord<T>(x, y);
}

template<typename T>
inline std::wostream& operator<<(std::wostream &Stream, const TCoord<T> &Coord)
{
  return Stream << Coord.x << ',' << Coord.y;
}

template<typename T>
inline std::wistream& operator>>(std::wistream &Stream, TCoord<T> &Coord)
{
  Stream >> Coord.x;
  Stream.ignore(); //Ignore ','
  return Stream >> Coord.y;
}

//---------------------------------------------------------------------------
//!The type Complex is used for all calculations with complex numbers.
typedef std::complex<long double> TComplex;

//!Type that indicates if calculations are done with radians or degrees
enum TTrigonometry {Radian, Degree};

class TFunc;
class TSymbolList;
class TFuncData;

/** \brief This is an abstract class used as base class for TFunc, TParamFunc and
 *  TPolarFunc. The class defines the general function interface as vitual
 *  functions, but it also has some variables and non-virtual functions.
 */
class TBaseFunc
{
protected:
  //!This variable indicates if calculations should use radians or degrees in trigonometric functions.
  TTrigonometry Trigonometry;

  TBaseFunc(TTrigonometry ATrigonometry = Radian)
    : Trigonometry(ATrigonometry) {}
  TBaseFunc(const TBaseFunc &Func)
    : Trigonometry(Func.Trigonometry) {}

  void SwapBase(TBaseFunc &Func); //throw()
  const TBaseFunc& operator=(const TBaseFunc &F);

  //! Ensures that data are unique and no differentiation is buffered
  virtual void Unique() =0;

public:
  virtual ~TBaseFunc() {}

  //!Returns a new copy of the actual class instance
  virtual TBaseFunc* Clone() const =0;

  //NOTE: Trigonometry may change the differentiation
  void SetTrigonometry(TTrigonometry ATrigonometry) {Unique(); Trigonometry = ATrigonometry;}
  TTrigonometry GetTrigonometry() const {return Trigonometry;}

  virtual TCoord<long double> Calc(long double, ECalcError &E) const =0; //throw()
  virtual TCoord<TComplex> Calc(TComplex, ECalcError &E) const =0; //throw()

  virtual long double CalcX(long double, ECalcError &E) const =0; //throw()
  virtual TComplex CalcX(TComplex, ECalcError &E) const =0; //throw()

  virtual long double CalcY(long double, ECalcError &E) const =0; //throw()
  virtual TComplex CalcY(TComplex, ECalcError &E) const =0; //throw()

  TCoord<long double> Calc(long double) const;
  TCoord<TComplex> Calc(TComplex) const;

  long double CalcX(long double) const;
  TComplex CalcX(TComplex) const;

  long double CalcY(long double) const;
  TComplex CalcY(TComplex) const;

  /** Calculates the slope of the function at s. The calculation differentiating the function.
   *  \param s: Position to calculate slope at.
   *  \return The slope at s; +INF or -INF if slope is vertical depending of direction
   *  \throw ECalcError: Thrown when calculation fails
   *  \throw EFuncError: Thrown with ecNotDifAble if differentiation fails
   */
  virtual long double CalcSlope(long double s) const =0;

  /** This creates the first derivative of the function and returns a pointer to
   *  created function. Ownership is passed to the caller. The functions is of
   *  the same type as the original.
   *  \throw EFuncError: Thrown if function could not be differentiated
   */
  virtual TBaseFunc* MakeDifPtr() const =0;

  //! \overload
  virtual TBaseFunc* MakeDifPtr(const std::wstring &Var) const =0;

  //!This will clear the function. Any calculations on the function before it is set will result in an error.
  virtual void Clear()=0;

  //!Returns true if the function has not been set. Any calculations on an empty function will result in an error.
  virtual bool IsEmpty() const =0; //throw()

  //!Calculates the area of the function from sMin to sMax. The exact area depends on the type of function
  virtual double CalcArea(double sMin, double sMax, double RelError=1E-4) const =0;

  //!Calculates the length of the function from sMin to sMax along the curve
  //!using Simpson's formula. n is the number of steps, and n must be even.
  virtual double CalcArc(double sMin, double sMax, double RelError=1E-4) const =0;

  //!Returns a standard function that define the x-part of the function.
  virtual TFunc ConvXToFunc() const =0;

  //!Returns a standard function that defines the y-part of the function.
  virtual TFunc ConvYToFunc() const =0;

  //!Reduce function expressions for faster calculations. This is called
  //!implicitly for creation of the first derivative.
  virtual void Simplify() =0;

  /** This returns the slope of the function as an angle represented in radians.
   *  \param s: The position on function to calculate slope
   *  \return The angular slope in radians at s. 0 is horizontal right, and
   *  counter clockwize is positive. The value is in the range -PI to PI.
   *  \throw ECalcError on calculation error
   */
  virtual long double CalcAngleSlope(long double s) const =0;

  //!Assigns F to the function.
  //!\throw EFuncError: Thrown with ErrorCode=ecErrorType if the two functions are not of the same type.
  virtual void Assign(const TBaseFunc &F) = 0;

  virtual bool Update(const TSymbolList &SymbolList) =0;

  //!Check if a symbol is referenced by the function directly or indirectly
  virtual bool IsDependent(const std::wstring &SymbolName) const =0;
};
//---------------------------------------------------------------------------
//!The TFunc class is derived from TBaseFunc. The class models a standard function y=f(x).
class TFunc : public TBaseFunc
{
  friend TFunc pow(const TFunc&, const TFunc&);
  friend TFunc log(const TFunc&);
  friend TFunc sqrt(const TFunc&);
  friend class TBaseFunc;
  friend class TParamFunc;
  friend class TPolarFunc;
  friend class TFuncData;

  boost::shared_ptr<TFuncData> FuncData;
  mutable boost::shared_ptr<TFuncData> DifData;
  void Unique();

public:
  using TBaseFunc::CalcX;
  using TBaseFunc::CalcY;

  TFunc();
  //!Copy constructor creates a TFunc object from another function
  TFunc(const TFunc &Func) : TBaseFunc(Func), FuncData(Func.FuncData) {}
  TFunc(long double Value);
  TFunc(const std::wstring &Text, const std::wstring &Variable = L"x", TTrigonometry ATrigonometry = Radian);
  TFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList, TTrigonometry ATrigonometry = Radian);
  TFunc(const boost::shared_ptr<TFuncData> &AFuncData, TTrigonometry ATrigonometry) : TBaseFunc(ATrigonometry), FuncData(AFuncData) {}

  TFunc* Clone() const {return new TFunc(*this);}

  void SetFunc(const std::wstring &Text, const std::wstring &Variable = L"x");
  void SetFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList);

  long double CalcX(long double x, ECalcError &E) const; //throw()
  TComplex CalcX(TComplex, ECalcError &E) const; //throw()

  long double CalcY(long double x, ECalcError &E) const; //throw()
  TComplex CalcY(TComplex, ECalcError &E) const; //throw()

  TCoord<long double> Calc(long double, ECalcError &E) const; //throw()
  TCoord<TComplex> Calc(TComplex, ECalcError &E) const; //throw()

  long double CalcSlope(long double x) const;
  double CalcArea(double xMin, double xMax, double RelError) const;
  double CalcArc(double xMin, double xMax, double RelError) const;
  long double CalcAngleSlope(long double s) const;

  TFunc* MakeDifPtr() const;
  TFunc* MakeDifPtr(const std::wstring &Var) const;
  TFunc MakeDif() const;
  TFunc MakeDif(const std::wstring &Var) const;

  void Clear();
  bool IsEmpty() const; //throw()
  std::wstring MakeText(const std::wstring &Variable = L"x", unsigned Decimals=8) const;

  void Simplify();
  void Swap(TFunc &Func); //throw()

  TFunc ConvXToFunc() const {return TFunc(L"x", L"x", Trigonometry);}
  TFunc ConvYToFunc() const {return *this;}

  bool Update(const TSymbolList &SymbolList);
  bool IsDependent(const std::wstring &SymbolName) const;

  //Overloaded operators
  TFunc& operator=(const TFunc &Func);
  void Assign(const TBaseFunc &F);
  friend const TFunc operator+(TFunc Func1,const TFunc &Func2);
  friend const TFunc operator-(TFunc Func1,const TFunc &Func2);
  friend const TFunc operator*(TFunc Func1,const TFunc &Func2);
  friend const TFunc operator/(TFunc Func1,const TFunc &Func2);
  TFunc& operator+=(const TFunc &Func);
  TFunc& operator-=(const TFunc &Func);
  TFunc& operator*=(const TFunc &Func);
  TFunc& operator/=(const TFunc &Func);
  bool operator==(const TFunc &Func) const;
  bool operator!=(const TFunc &Func) const;
  long double operator()(long double x) const;
  TComplex operator()(TComplex x) const;
  friend std::wostream& operator<<(std::wostream &os, const TFunc &Func);
};
//---------------------------------------------------------------------------
/*! \brief The TParamFunc class is derived from TBaseFunc. It models a parameter
 *  function f(t)=x(t),y(t).
 */
class TParamFunc:public TBaseFunc
{
  boost::shared_ptr<TFuncData> xFuncData;
  boost::shared_ptr<TFuncData> yFuncData;
  mutable boost::shared_ptr<TFuncData> xDifData;
  mutable boost::shared_ptr<TFuncData> yDifData;

  void Unique();
  void EnsureDif() const;

public:
  using TBaseFunc::CalcX;
  using TBaseFunc::CalcY;

  TParamFunc();
  TParamFunc(const std::wstring &xText, const std::wstring &yText, const std::wstring &Variable = L"t", TTrigonometry ATrigonometry = Radian);
  TParamFunc(const std::wstring &xText, const std::wstring &yText, const std::wstring &Variable, const TSymbolList &SymbolList, TTrigonometry ATrigonometry = Radian);
  TParamFunc(const TParamFunc &Func) : TBaseFunc(Func), xFuncData(Func.xFuncData), yFuncData(Func.yFuncData) {}
  TParamFunc(long double xValue, long double yValue);
  explicit TParamFunc(const TFunc &Func);
  TParamFunc(const boost::shared_ptr<TFuncData> &AxFuncData, const boost::shared_ptr<TFuncData> &AyFuncData, TTrigonometry ATrigonometry)
    : TBaseFunc(ATrigonometry), xFuncData(AxFuncData), yFuncData(AyFuncData) {}

	TParamFunc* Clone() const {return new TParamFunc(*this);}
	void SetFunc(const std::wstring &xText, const std::wstring &yText, const std::wstring &Variable = L"t");
	void SetFunc(const std::wstring &xText, const std::wstring &yText, const std::wstring &Variable, const TSymbolList &SymbolList);

	long double CalcX(long double t, ECalcError &E) const; //throw()
	TComplex CalcX(TComplex, ECalcError &E) const; //throw()

  long double CalcY(long double t, ECalcError &E) const; //throw()
  TComplex CalcY(TComplex, ECalcError &E) const; //throw()

  TCoord<long double> Calc(long double, ECalcError &E) const; //throw()
  TCoord<TComplex> Calc(TComplex, ECalcError &E) const; //throw()

  long double CalcSlope(long double t) const;
  double CalcArea(double sMin, double sMax, double RelError) const;
  double CalcArc(double tMin, double tMax, double RelError) const;
  long double CalcAngleSlope(long double s) const;

  TParamFunc* MakeDifPtr() const;
  TParamFunc* MakeDifPtr(const std::wstring &Var) const;
  TParamFunc MakeDif() const;
  TParamFunc MakeDif(const std::wstring &Var) const;

  void Clear();
  bool IsEmpty() const; //throw()
  std::wstring MakeXText(const std::wstring &Variable = L"t", unsigned Decimals=8) const;
  std::wstring MakeYText(const std::wstring &Variable = L"t", unsigned Decimals=8) const;

  TFunc ConvXToFunc() const;
  TFunc ConvYToFunc() const;

  void Swap(TParamFunc &Func); //throw()
  void Simplify();

  bool Update(const TSymbolList &SymbolList);
  bool IsDependent(const std::wstring &SymbolName) const;

//Overloaded operators
  TParamFunc& operator=(const TParamFunc &Func);
  void Assign(const TBaseFunc &F);
  friend const TParamFunc operator+(TParamFunc Func1,const TParamFunc &Func2);
  friend const TParamFunc operator-(TParamFunc Func1,const TParamFunc &Func2);
  friend const TParamFunc operator*(long double Value, const TParamFunc &Func);
  friend const TParamFunc operator/(long double Value, const TParamFunc &Func);
  const TParamFunc operator*(long double Value) const;
  const TParamFunc operator/(long double Value) const;
  TParamFunc& operator+=(const TParamFunc &Func);
  TParamFunc& operator-=(const TParamFunc &Func);
  TParamFunc& operator*=(long double Value);
  TParamFunc& operator/=(long double Value);
  bool operator==(const TParamFunc &Func) const;
  bool operator!=(const TParamFunc &Func) const;
};
//---------------------------------------------------------------------------
//! The TPolarFunc class is derived from TBaseFunc. It models a polar function r(t).
class TPolarFunc : public TBaseFunc
{
  boost::shared_ptr<TFuncData> FuncData;
  mutable boost::shared_ptr<TFuncData> DifData;

  void Unique();

public:
  using TBaseFunc::Calc;
  using TBaseFunc::CalcX;
  using TBaseFunc::CalcY;

  TPolarFunc();
  TPolarFunc(const std::wstring &Text, const std::wstring &Variable = L"t", TTrigonometry ATrigonometry = Radian);
  TPolarFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList, TTrigonometry ATrigonometry = Radian);
  TPolarFunc(const TPolarFunc &Func) : TBaseFunc(Func), FuncData(Func.FuncData) {}
  TPolarFunc(const boost::shared_ptr<TFuncData> &AFuncData, TTrigonometry ATrigonometry) : TBaseFunc(ATrigonometry), FuncData(AFuncData) {}

  TPolarFunc* Clone() const {return new TPolarFunc(*this);}
  void SetFunc(const std::wstring &Text, const std::wstring &Variable = L"t");
  void SetFunc(const std::wstring &Text, const std::wstring &Variable, const TSymbolList &SymbolList);

  long double CalcX(long double t, ECalcError &E) const; //throw()
  TComplex CalcX(TComplex, ECalcError &E) const; //throw()

  long double CalcY(long double t, ECalcError &E) const; //throw()
  TComplex CalcY(TComplex, ECalcError &E) const; //throw()

  TCoord<long double> Calc(long double, ECalcError &E) const; //throw()
  TCoord<TComplex> Calc(TComplex, ECalcError &E) const; //throw()

  long double CalcSlope(long double t) const;
  long double CalcR(long double t) const;
  double CalcArc(double tMin, double tMax, double RelError) const;
  double CalcArea(double tMin, double tMax, double RelError) const;
  long double CalcAngleSlope(long double s) const;

  TPolarFunc* MakeDifPtr() const;
  TPolarFunc* MakeDifPtr(const std::wstring &Var) const;
  TPolarFunc MakeDif() const;
  TPolarFunc MakeDif(const std::wstring &Var) const;

  void Clear();
  bool IsEmpty() const; //throw()
  std::wstring MakeText(const std::wstring &Variable = L"t", unsigned Decimals=8) const;

  TFunc ConvXToFunc() const;
  TFunc ConvYToFunc() const;
  TFunc ConvToFunc() const;

  void Swap(TPolarFunc &Func); //throw()
  void Simplify();
  void Assign(const TBaseFunc &F);
  bool Update(const TSymbolList &SymbolList);
  bool IsDependent(const std::wstring &SymbolName) const;

//Overloaded operators
  TPolarFunc& operator=(const TPolarFunc &Func);
  bool operator==(const TPolarFunc &Func) const;
  bool operator!=(const TPolarFunc &Func) const;
  long double operator()(long double t) const;
};
//---------------------------------------------------------------------------
typedef std::vector<std::wstring> TArgType;
enum TFunctionType {ftEmpty, ftFunction, ftEquation, ftInequality};

template<typename T> class TDynData;
class TBaseCustomFunc
{
public:
  virtual long double DynCall(class TDynData<long double> &DynData) const;
  virtual TComplex DynCall(TDynData<TComplex> &DynData) const;
  virtual const boost::shared_ptr<TFuncData>& GetFuncData() const {throw EFuncError(ecNotDifAble);}
  virtual bool Update(const TSymbolList &SymbolList) {return true;}
  virtual unsigned ArgumentCount() const =0;
  virtual long double Call(const long double *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const {return 0;}
  virtual TComplex Call(const TComplex *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const {return 0;}
  virtual bool IsDependent(const std::wstring &SymbolName) const {return false;}
};

class TCustomFunc : public TBaseCustomFunc
{
  boost::shared_ptr<TFuncData> FuncData;
  TArgType Args;
  TTrigonometry Trigonometry;

  TCustomFunc(const boost::shared_ptr<TFuncData> &AFuncData, const TArgType &AArgs, TTrigonometry ATrigonometry)
    : FuncData(AFuncData), Args(AArgs), Trigonometry(ATrigonometry) {}
  void Unique();

public:
  TCustomFunc();
  TCustomFunc(const std::wstring &Text, const TArgType &AArgs, TTrigonometry Trig = Radian);
  TCustomFunc(const std::wstring &Text, const TArgType &AArgs, const TSymbolList &SymbolList, TTrigonometry Trig = Radian);
  TCustomFunc(long double Value);
  TCustomFunc(const TComplex &Complex);

  void SetFunc(const std::wstring &Text, const TArgType &AArgs);
  void SetFunc(const std::wstring &Text, const TArgType &AArgs, const TSymbolList &SymbolList);
  void SetFunc(long double Value);
  void SetFunc(const TComplex &Value);
  long double Calc(const std::vector<long double> &Values) const;
  long double Calc(const std::vector<long double> &Values, ECalcError &E) const;
  TComplex Calc(const std::vector<TComplex> &Values) const;
  TComplex Calc(const std::vector<TComplex> &Values, ECalcError &E) const;
  TCustomFunc MakeDif(unsigned Arg) const;
  const TArgType& GetArguments() const {return Args;}
  void SetTrigonometry(TTrigonometry Trig) {Trigonometry = Trig;}
  TTrigonometry GetTrigonometry() const {return Trigonometry;}
  TFunc ConvToFunc(const std::vector<long double> &Values, unsigned Variable) const;
  std::wstring MakeText(unsigned Decimals=8) const;
  bool Update(const TSymbolList &SymbolList);
  TFunctionType GetFunctionType() const;
  void RemoveRelation();
  bool IsEmpty() const;
  unsigned ArgumentCount() const {return Args.size();}

  long double operator()(const std::vector<long double> &Values) const {return Calc(Values);}
  long double DynCall(class TDynData<long double> &DynData) const;
  TComplex DynCall(TDynData<TComplex> &DynData) const;
  const boost::shared_ptr<TFuncData>& GetFuncData() const {return FuncData;}
  bool IsDependent(const std::wstring &SymbolName) const;
};

typedef boost::shared_ptr<TBaseCustomFunc> TBaseCustomFuncPtr;
typedef boost::shared_ptr<TCustomFunc> TCustomFuncPtr;

class TSymbolList
{
  std::map<std::wstring, TBaseCustomFuncPtr> List;

public:
  typedef std::map<std::wstring, TBaseCustomFuncPtr>::iterator TIterator;
  typedef std::map<std::wstring, TBaseCustomFuncPtr>::const_iterator TConstIterator;

  const TBaseCustomFuncPtr& Add(const std::wstring &Key);
  TCustomFuncPtr Add(const std::wstring &Key, const std::wstring &Value, const TArgType &Args = TArgType());
  void Add(const std::wstring &Key, const boost::shared_ptr<TBaseCustomFunc> &CustomFunc);
  TCustomFuncPtr Add(const std::wstring &Key, const TComplex &Value);
  TConstIterator Begin() const {return List.begin();}
  TIterator Begin() {return List.begin();}
  TConstIterator End() const {return List.end();}
  TIterator End() {return List.end();}
  TBaseCustomFuncPtr Get(const std::wstring &Key) const;
  void Clear();
  bool Empty() const {return List.empty();}
  bool Exists(const std::wstring &Key) const;
  void Erase(const std::wstring &Key);
  unsigned Size() const {return List.size();}
  void Swap(TSymbolList &SymbolList) {List.swap(SymbolList.List);}
  void Update() {for(TIterator Iter = List.begin(); Iter != List.end(); ++Iter) Iter->second->Update(*this);}
  TSymbolList& operator+=(const TSymbolList &SymbolList) {List.insert(SymbolList.List.begin(), SymbolList.List.end()); return *this;}
  TSymbolList operator+(const TSymbolList &SymbolList) const {return TSymbolList(*this) += SymbolList;}
  TSymbolList& operator-=(const TSymbolList &SymbolList);
  TSymbolList operator-(const TSymbolList &SymbolList) const {return TSymbolList(*this) -= SymbolList;}
};
//---------------------------------------------------------------------------
inline long double GetReal(TComplex Number)
{
	if(imag(Number))
		throw ECalcError(ecComplexError);
	return Number.real();
}
//---------------------------------------------------------------------------
inline long double GetReal(long double Number)
{
	return Number;
}
//---------------------------------------------------------------------------
template<typename T>
inline TCoord<long double> GetReal(const TCoord<T> &Coord)
{
  return TCoord<long double>(GetReal(Coord.x), GetReal(Coord.y));
}
//---------------------------------------------------------------------------
typedef TCoord<double> TDblPoint;

//! Type use with the TrendLine() function to find a trend in a point series.
enum TTrendType
{
  ttLinear,       //!< Linear trend (y = a*x + b)
  ttLogarithmic,  //!< Logarithmic trend (y = a*ln(x) + b)
  ttPolynomial,   //!< Polynomial trend (y = an*x^n + ... + a3*x^3 + a2*x^2 + a1*x + a0). The order is given as argument to TrendLine().
  ttPower,        //!< Power trend (y = a*x^b)
  ttExponential,  //!< Exponential trend (y = a*b^x)
};

TFunc TrendLine(Func32::TTrendType Type, const std::vector<TDblPoint> &Points, const std::vector<double> &Weights, unsigned N);
TFunc TrendLine(TTrendType Type, const std::vector<TDblPoint> &Points, const std::vector<double> &Weights, unsigned N, double Intercept);
TParamFunc MovingAverage(const std::vector<TDblPoint> &Points, unsigned N);
double Regression(const std::vector<TDblPoint> &Points, const TCustomFunc &Func, std::vector<long double> &Values, const std::vector<double> &Weights=std::vector<double>(), unsigned MaxIter = 100, double Tol = 1E-6);
double CalcSSQ(const std::vector<TDblPoint> &Points, const TFunc &Func);
double LinearCorrelation(const std::vector<TDblPoint> &Points);
double Correlation(const std::vector<TDblPoint> &Points, const TFunc &Func);

std::wistream& operator>>(std::wistream &is, TFunc &Func);

long double Eval(const std::wstring &Expr, TTrigonometry Trig = Radian);
long double Eval(const std::wstring &Expr, const TSymbolList &SymbolList, TTrigonometry Trig = Radian);
long double Eval(const std::wstring &Expr, long double x, const std::wstring &Var = L"x", TTrigonometry Trig = Radian);
TComplex EvalComplex(const std::wstring &Expr, TTrigonometry Trig = Radian);
TComplex EvalComplex(const std::wstring &Expr, const TSymbolList &SymbolList, TTrigonometry Trig = Radian);
TComplex EvalComplex(const std::wstring &Expr, TComplex x, const std::wstring &Var = L"x", TTrigonometry Trig = Radian);

long double FindCrossing(const TBaseFunc &Func1, long double Min1, long double Max1, const TBaseFunc &Func2, long double Min2, long double Max2);
TArgType FindUnknowns(const std::wstring &Str);
bool IsValidName(const std::wstring &Name);

template<typename T=long double>
struct TCoordSet
{
  T t;
  T x;
  T y;
  TCoordSet() : t(0), x(0), y(0) {}
  TCoordSet(T at, T ax, T ay) : t(at), x(ax), y(ay) {}
  TCoordSet(T at, const TCoord<T> &Coord) : t(at), x(Coord.x), y(Coord.y) {}
};
template<typename T>
inline TCoord<T>::TCoord(const TCoordSet<T> &Set) {TCoordBase<T>::x=Set.x; TCoordBase<T>::y=Set.y;}

template<typename T=long double>
class TEvalCoordSet
{
  const TBaseFunc &Func;
public:
  TEvalCoordSet(const TBaseFunc &AFunc) : Func(AFunc) {}
  TCoordSet<T> operator()(const TCoordSet<T> &CoordSet) {return TCoordSet<T>(CoordSet.t, Func.Calc(CoordSet.t));}
};

enum TAnalyseType {atXAxisCross, atYAxisCross};
std::vector<TCoordSet<> > AnalyseFunction(const TBaseFunc &Func, long double Min, long double Max, unsigned Steps, long double Tol, TAnalyseType AnalyseType);
} //namespace Func32
//---------------------------------------------------------------------------
#pragma option pop
#include "Func32Impl.h"  //Workaround for compiler bug described in QC #89260; Remove as soon as possible
#endif



