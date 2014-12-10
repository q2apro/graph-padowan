//---------------------------------------------------------------------------
#pragma hdrstop
#include "../Func32.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <limits>
#include <cmath>
#include <boost/math/special_functions/fpclassify.hpp>
#include <cstdio>
//---------------------------------------------------------------------------
using namespace Func32;
using namespace std;

#define EULER 2.718281828459045235360287
#define PI    3.141592653589793238462643
#define SQRT_2    0.707106781186547524401

#define SQRT2 1.41421356237
#define DEFAULT_FPU_CONTROL EM_INVALID | EM_DENORMAL | EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT | IC_AFFINE | RC_NEAR | PC_64
#define FPU_MASK MCW_EM | MCW_IC | MCW_RC | MCW_PC

const long double NaN = numeric_limits<long double>::quiet_NaN(); //0.0/0.0;
const long double INF = numeric_limits<long double>::infinity();
inline long double real(long double x) {return x;}
inline long double imag(long double x) {return 0;}
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
//Workaround for a bug in C++ Builder XE
int _RTLENTRY _EXPFUNC std::iswspace(wchar_t c)
{
	return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n';
}

int _RTLENTRY _EXPFUNC std::iswdigit(wint_t c)
{
	return c >= L'0' && c <= L'9';
}

#endif
//---------------------------------------------------------------------------
long double StrToDouble(const char *Str)
{
	long double Number;
	sscanf(Str, "%Lf", &Number);
	return Number;
}
//---------------------------------------------------------------------------
inline bool IsZero(long double a)
{
	return a == 0 || abs(a) <= 0.0000001L * (abs(a) + 1);
}
//---------------------------------------------------------------------------
inline bool IsZero(TComplex c)
{
	return ::IsZero(real(c)) && ::IsZero(imag(c));
}
//---------------------------------------------------------------------------
inline bool IsEqual(long double a, long double b)
{
	if(boost::math::isnan(a) && boost::math::isnan(b))
		return true;

	if(!boost::math::isfinite(a) || !boost::math::isfinite(b))
		return a == b;

	int a_exp, b_exp, exp;
	frexp(a, &a_exp);
	frexp(b, &b_exp);
	frexp(a - b, &exp);
  return ::IsZero(a-b) || (a_exp == b_exp && std::abs(exp - a_exp) > 40);
}
//---------------------------------------------------------------------------
inline bool IsEqual(TComplex a, TComplex b)
{
	return ::IsEqual(real(a), real(b)) && ::IsEqual(imag(a), imag(b));
}
//---------------------------------------------------------------------------
std::wstring ToWString(const std::string &Str)
{
	return std::wstring(Str.begin(), Str.end());
}
//---------------------------------------------------------------------------
bool CompareFunc(const TFunc &f1, const TFunc &f2, double x)
{
	ECalcError E1;
	ECalcError E2;
	long double y1 = f1.CalcY(x, E1);
	long double y2 = f2.CalcY(x, E2);
	if(E1.ErrorCode != E2.ErrorCode)
		return false;
	if(E1.ErrorCode == ecNoError)
		return ::IsEqual(y1, y2);
	return true;
}
//---------------------------------------------------------------------------
bool CompareFunc(const TFunc &f1, const TFunc &f2)
{
	double x[] = {-1000, -100, -10, -1, 0, 1, 10, 100, 1000};
	for(unsigned I = 0; I < sizeof(x)/sizeof(x[0]); I++)
    if(!CompareFunc(f1, f2, x[I]))
      return false;
  return true;
}
//---------------------------------------------------------------------------
void TestText(const std::wstring &Str, const TSymbolList &SymbolList = TSymbolList())
{
	std::wstring Str2;
	try
	{
		TFunc Func(Str, L"x", SymbolList);
		Str2 = Func.MakeText();
		TFunc Func2(Str2, L"x", SymbolList);
		if(Func != Func2)
		{
			std::wcerr << "Failed to convert function back to text!" << std::endl;
			std::wcerr << "Function: " << Str << std::endl;
			std::wcerr << "Result:   " << Str2 << std::endl << std::endl << std::endl;
		}
	}
	catch(EFuncError &E)
	{
		std::wcerr << "Failed to convert function back to test!" << std::endl;
		std::wcerr << "Function:  " << Str << std::endl;
		std::wcerr << "Result:    " << Str2 << std::endl;
		std::wcerr << "ErrorCode: " << E.ErrorCode << std::endl << std::endl;
	}
}
//---------------------------------------------------------------------------
template<typename T>
void Test(const std::wstring &Str, T x, T y, TTrigonometry Trig = Radian, const TSymbolList &SymbolList=TSymbolList())
{
	try
	{
		TFunc Func(Str, L"x", SymbolList);
		Func.SetTrigonometry(Trig);
		T f = Func.CalcY(x);
		if(!::IsEqual(f, y))
    {
      wcerr << "Function:     " << Str << std::endl;
      wcerr << "x:            " << setprecision(10) << x << std::endl;
      wcerr << "Evaluated to: " << setprecision(10) << f << std::endl;
      wcerr << "Expected:     " << setprecision(10) << y << std::endl << std::endl;
    }
  }
	catch(EFuncError &E)
	{
		wcerr << "Function:  " << Str << std::endl;
		wcerr << "x:         " << setprecision(10) << x << std::endl;
		wcerr << "ErrorCode: " << E.ErrorCode << std::endl ;
		wcerr << "Expected:  " << setprecision(10) << y << std::endl << std::endl;
	}
}
//---------------------------------------------------------------------------
void Test(const std::wstring &Str, long double x, long double y, TTrigonometry Trig = Radian, const std::wstring &ConvToText = L"", const TSymbolList &SymbolList=TSymbolList())
{
	Test<long double>(Str, x, y, Trig, SymbolList);
	Test<TComplex>(Str, x, y, Trig, SymbolList);
	TestText(ConvToText.empty() ? Str : ConvToText, SymbolList);
}
//---------------------------------------------------------------------------
void Test(const std::string &Str, long double x, long double y, TTrigonometry Trig = Radian, const std::string &ConvToText = "", const TSymbolList &SymbolList=TSymbolList())
{
	Test(ToWString(Str), x, y, Trig, ToWString(ConvToText), SymbolList);
}
//---------------------------------------------------------------------------
template<typename T>
void TestError(const std::wstring &Str, T x, const Func32::EFuncError &Error, TTrigonometry Trig = Radian)
{
	try
	{
		TFunc Func(Str);
		Func.SetTrigonometry(Trig);
		T f = Func.CalcY(x);

		wcerr << "Function:       " << Str << std::endl;
		wcerr << "x:              " << setprecision(10) << x << std::endl;
		wcerr << "Evaluated to:   " << setprecision(10) << f << std::endl;
		wcerr << "Expected error: " << Error.ErrorCode << std::endl << std::endl;
	}
	catch(EFuncError &E)
	{
		if(E.ErrorCode != Error.ErrorCode || (!Error.Str.empty() && E.Str != Error.Str))
		{
			wcerr << "Function:       " << Str << std::endl;
			wcerr << "x:              " << setprecision(10) << x << std::endl;
			wcerr << "Error code:     " << E.ErrorCode << std::endl;
			wcerr << "Expected error: " << Error.ErrorCode << std::endl << std::endl;
		}
	}
}
//---------------------------------------------------------------------------
void TestError(const std::wstring &Str, long double x, const EFuncError &Error, TTrigonometry Trig = Radian)
{
	TestError<long double>(Str, x, Error, Trig);
	TestError<TComplex>(Str, x, Error, Trig);
}
//---------------------------------------------------------------------------
void TestError(const std::string &Str, long double x, const EFuncError &Error, TTrigonometry Trig = Radian)
{
	TestError(ToWString(Str), x, Error, Trig);
}
//---------------------------------------------------------------------------
void TestTrendLine(Func32::TTrendType Type, const std::vector<TDblPoint> &P, const std::vector<double> &W, unsigned N, double Intercept, const std::wstring &Str)
{
	try
	{
		TFunc Func(Str);
		TFunc Result = TrendLine(Type, P, W, N, Intercept);
		if(!CompareFunc(Result, Func))
		{
			wcerr << "-- Trendline --" << endl;
			wcerr << "Expected trendline: f(x)=" << Str << std::endl;
 			wcerr << "Evaluated to:       f(x)=" << std::setprecision(15) << Result << std::endl << std::endl;

		}
	}
	catch(EFuncError &E)
	{
		wcerr << "-- Trendline --" << endl;
		wcerr << "Expected trendline: f(x)=" << Str << std::endl;
		wcerr << "Error code:     " << E.ErrorCode << std::endl << std::endl;
	}
}
//---------------------------------------------------------------------------
void TestCustomTrendLine(const std::wstring &Model, const std::vector<TDblPoint> &P, const std::vector<double> &W, const std::wstring &Str)
{
  try
  {
    std::vector<std::wstring> Unknowns = FindUnknowns(Model);
		std::vector<long double> Values(Unknowns.size(), 1);
    Unknowns.insert(Unknowns.begin(), L"x");
		TCustomFunc Func(Model, Unknowns);
    Regression(P, Func, Values, W);
    std::wstring Str2 = Func.ConvToFunc(Values, 0).MakeText(L"x", 5);
    if(Str != Str2)
    {
      wcerr << "-- Custom trendline --" << endl;
			wcerr << "Model:              f(x)=" << Model << std::endl;
			wcerr << "Expected trendline: f(x)=" << Str << std::endl;
			wcerr << "Evaluated to:       f(x)=" << Str2 << std::endl << std::endl;
		}
	}
  catch(EFuncError &E)
	{
    wcerr << "-- Custom trendline --" << endl;
    wcerr << "Model:      f(x)=" << Model << std::endl;
		wcerr << "Error code: " << E.ErrorCode << std::endl << std::endl;
  }
}
//---------------------------------------------------------------------------
void TestTrendLineError(Func32::TTrendType Type, const TDblPoint *Points, unsigned Size, unsigned N, Func32::TErrorCode ErrorCode)
{
	try
	{
    std::vector<double> W;
    TrendLine(Type, std::vector<TDblPoint>(Points, Points + Size), W, N);

		cerr << "-- Trendline --" << endl;
		cerr << "Expected error code:     " << ErrorCode << endl << endl;
  }
	catch(EFuncError &E)
  {
    if(E.ErrorCode != ErrorCode)
		{
      cerr << "-- Trendline --" << endl;
      cerr << "Error code:     " << E.ErrorCode << endl << endl;
		}
	}
}
//---------------------------------------------------------------------------
void TestDif(const std::wstring &f, const std::wstring &df, TTrigonometry Trig = Radian)
{
	try
	{
		TFunc Func(f, L"x", Trig);
		TFunc Dif = Func.MakeDif();
		TFunc Dif2(df, L"x", Trig);

		if(!CompareFunc(Dif, Dif2))
		{
			wcerr << "f(x)=" << f << std::endl;
			wcerr << "f'(x)=" << Dif.MakeText() << std::endl;
			wcerr << "Expected f'(x)=" << df << std::endl << std::endl;
		}
	}
  catch(EFuncError &E)
  {
    wcerr << "f(x)=" << f << std::endl;
    wcerr << "Expected f'(x)=" << df << std::endl;
		wcerr << "Error code: " << E.ErrorCode << std::endl << std::endl;
  }
}
//---------------------------------------------------------------------------
void TestDif(const std::string &f, const std::string &df, TTrigonometry Trig = Radian)
{
	TestDif(ToWString(f), ToWString(df), Trig);
}
//---------------------------------------------------------------------------
void TestDif(const std::wstring &Str, long double x, long double y, TTrigonometry Trig = Radian)
{
	try
  {
		TFunc Func(Str, L"x", Trig);
		TFunc Dif = Func.MakeDif();
		long double f = Dif(x);

		if(!::IsEqual(f, y))
    {
      wcerr << "f(x)=" << Str << std::endl;
      wcerr << "f'(x)=" << Dif.MakeText() << std::endl;
			wcerr << "f'(" << x << ")=" << f << std::endl;
      wcerr << "Expected f'(" << x << ")=" << y << std::endl << std::endl;
		}
	}
	catch(EFuncError &E)
	{
    wcerr << "f(x)=" << Str << std::endl;
    wcerr << "Expected f'(" << x << ")=" << y << std::endl;
		wcerr << "Error code: " << E.ErrorCode << std::endl << std::endl;
  }
}
//---------------------------------------------------------------------------
void TestDif(const std::string &Str, long double x, long double y, TTrigonometry Trig = Radian)
{
	TestDif(ToWString(Str), x, y, Trig);
}
//---------------------------------------------------------------------------
void TestDif(const std::wstring &Str, TErrorCode ErrorCode)
{
	try
	{
		TFunc Func(Str);
		TFunc Dif = Func.MakeDif();
		wcerr << "f(x)=" << Str << std::endl;
		wcerr << "f'(x)=" << Dif.MakeText() << std::endl;
		wcerr << "Expected error: " << ErrorCode << std::endl << std::endl;
	}
	catch(EFuncError &E)
	{
		if(E.ErrorCode != ErrorCode)
		{
			wcerr << "Function:       " << Str << std::endl;
			wcerr << "Error code:     " << E.ErrorCode << std::endl;
			wcerr << "Expected error: " << ErrorCode << std::endl << std::endl;
		}
	}
}
//---------------------------------------------------------------------------
void TestDif(const std::string &Str, TErrorCode ErrorCode)
{
	TestDif(ToWString(Str), ErrorCode);
}
//---------------------------------------------------------------------------
void TestSimplify(const std::wstring &Str, const std::wstring &Str2)
{
	TFunc Func(Str);
	Func.Simplify();
//	if(Func != TFunc(Str2))
	if(Func.MakeText() != Str2)
	{
		wcerr << "f(x)=" << Str << std::endl;
		wcerr << "Simplified: f(x)=" << Func.MakeText() << std::endl;;
		wcerr << "Expected:   f(x)=" << Str2 << std::endl << std::endl;
	}
}
//---------------------------------------------------------------------------
void TestSimplify(const std::string &Str, const std::string &Str2)
{
	TestSimplify(ToWString(Str), ToWString(Str2));
}
//---------------------------------------------------------------------------
void TestCustom(const std::wstring &Str, const TArgType &Args, const std::vector<TComplex> &Values, const TComplex &Result)
{
	try
  {
    Func32::TCustomFunc Func(Str, Args);
		TComplex FuncResult = Func.Calc(Values);
    if(!IsEqual(Result, FuncResult))
		{
			wcerr << "Function:     " << Str << std::endl;
			for(unsigned I = 0; I < Values.size(); I++)
        wcerr << Args[I] << ":            " << setprecision(10) << Values[I] << std::endl;
			wcerr << "Evaluated to: " << setprecision(10) << FuncResult << std::endl;
			wcerr << "Expected:     " << setprecision(10) << Result << std::endl << std::endl;
    }
	}
	catch(EFuncError &E)
  {
		wcerr << "Function:     " << Str << std::endl;
    for(unsigned I = 0; I < Values.size(); I++)
      wcerr << Args[I] << ":            " << setprecision(10) << Values[I] << std::endl;
    wcerr << "Error code   : " << E.ErrorCode << std::endl;
    wcerr << "Expected:     " << setprecision(10) << Result << std::endl << std::endl;
	}
}
//---------------------------------------------------------------------------
void TestMakeText(const TFunc &Func, const std::wstring &Result)
{
	try
	{
		std::wstring TextResult = Func.MakeText();
    if(TextResult != Result)
    {
      wcerr << "Function:     " << TextResult << std::endl;
      wcerr << "Expected:     " << Result << std::endl << std::endl;
		}
	}
	catch(EFuncError &E)
	{
    wcerr << "Error code   : " << E.ErrorCode << std::endl;
    wcerr << "Expected:     " << Result << std::endl << std::endl;
  }
}
//---------------------------------------------------------------------------
/** Called when BOOST_ASSERT fails.
 */
namespace boost
{
	void assertion_failed(char const * expr, char const * function, char const * file, long line)
	{
		cerr << "Assertion failed: " << expr << ", File: " << file << ", Line " << line;
		std::cin.ignore();
		abort();
	}
}
//---------------------------------------------------------------------------
double ErrorToWeight(double x) {return 1/(x*x);}
//---------------------------------------------------------------------------
class TTestSqr : public TBaseCustomFunc
{
public:
	unsigned ArgumentCount() const {return 1;}
	long double Call(const long double *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const
	{
		return Args[0] * Args[0];
	}
	TComplex Call(const TComplex *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const
	{
		return Args[0] * Args[0];
	}
};
//---------------------------------------------------------------------------
class TTestCube : public TBaseCustomFunc
{
public:
	unsigned ArgumentCount() const {return 3;}
	long double Call(const long double *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const
  {
    return Args[0] * Args[0] * Args[0] + Args[1] * Args[1] + Args[2];
  }
  TComplex Call(const TComplex *Args, TTrigonometry Trig, TErrorCode &ErrorCode, std::wstring &ErrorStr) const
  {
    return Args[0] * Args[0] * Args[0] + Args[1] * Args[1] + Args[2];
	}
};
//---------------------------------------------------------------------------
void TestParamFunc(const std::wstring &xText, const std::wstring &yText)
{
	try
	{
		TParamFunc Func(xText, yText); //Test for known problem with parameter functions
	}
	catch(EFuncError &E)
	{                                             
	}
	try
	{
		TParamFunc Func;
		Func.SetFunc(xText, yText);
	}
	catch(EFuncError &E)
	{
	}
}
//---------------------------------------------------------------------------
void Test()
{
	//Test parsing errors
	TestError("sinx", 0, ecUnknownVar);
	TestError("x2", 0, ecUnknownVar);
	TestError("sin2 x", 0, ecUnknownVar);
	TestError("integrate(x, 5)", 0, ecArgCountError);
	TestError("integrate(x, x, 5, 7, 8)", 0, ecArgCountError);

	//Test redundant space
	Test("x*sin x", PI/2, PI/2);
	Test("  x*sin x", PI/2, PI/2);
	Test("x * sin x", PI/2, PI/2);
	Test("x*sin x   ", PI/2, PI/2);
	Test("x*sin    x", PI/2, PI/2);
	TestError("", 0, ecEmptyString);
	TestError("     ", 0, ecEmptyString);

	//Test case (in)sesitivity
	Test("Sin X+x/pI", 0, 0);
	TestError("4.5e2", 0, ecUnknownVar);
	TestError("4.5I", 0, ecUnknownVar);

	//Test numbers
	Test("45", 0, 45);
	Test("-0.5796", 0, -0.5796);
  Test("-.5796", 0, -0.5796);
	Test("-5.478E-23x", 10, -5.478E-22);
  Test("pi", 0, PI);
  Test("e", 0, EULER);
  TestError("E", 0, ecUnknownVar);
  Test<TComplex>(L"i*i", 0, -1);
	TestError<long double>(L"i*i", 0, ecComplexError);
	Test("1E400*x", 2, StrToDouble("2E400")); //2E400 doesn't work directly with BCC 5.6.4; Fails under Cygwin
	Test("1E4000", 1, StrToDouble("1E4000")); //2E4000 doesn't work directly with BCC 5.6.4; Fails under Cygwin
	TestError("1E5000", 1, ecInvalidNumber); //Number too large
  TestError("1.2.3", 0, ecInvalidNumber);
	TestError("5.", 0, ecInvalidNumber);

  //Test constants
	Test("e", 0, EULER);
  Test("pi", 0, PI);
  TestError<long double>(L"i", 0, ecComplexError);
	Test<TComplex>(L"i", 0, TComplex(0, 1));
  TestError(L"undef", 0, ecNotDefError);

  //Test pi symbol
  Test(L"π", 0, PI);
  Test(L"π(x+2)", 1, 3*PI);
  Test(L"π*(x+2)", 1, 3*PI);
  Test(L"πx", 2, 2*PI);
  Test(L"xπ", 2, 2*PI);
  Test(L"π*x", 2, 2*PI);
  Test(L"2π", 1, 2*PI);
  Test(L"π^2", 1, PI*PI);
  Test(L"sin 2π", 1, 0);

	//Test functions with arguments
  TestError("round x", 1.2345, ecArgCountError);
	TestError("round(x)", 1.2345, ecArgCountError);
	Test("round(x, 0)", 1.2345, 1);
  TestError("sqrt(x", 0, ecNoEndPar);

	//Implied multiplication
  Test("10sin(pi/2)x", 10, 100);
	Test("sin(pi*x)4x", 1.5, -6);
  Test("-3x", 10, -30);

	//Test common operators
  Test("4.5+x", 3.1, 7.6);
  Test("x+4.5", 3.1, 7.6);
  Test("4.5-x", 3.1, 1.4);
  Test("x-4.5", 3.1, -1.4);
	Test("4.5*x", 3.1, 13.95);
	Test("x*4.5", 3.1, 13.95);
	Test("4.5/x", 3.1, 1.4516129);
  Test("x/4.5", 3.1, 0.688888889);
  Test("(x)-3", 1, -2);
  Test("-3*x", 10, -30);
  TestError("x(5.4)", 0, ecParAfterConst);
  TestError("rand(2)", 0, ecParAfterConst);
  Test("x 5.4", 3, 16.2);     //Should this be allowed?
	Test("--x", 5, 5);
	Test("-x", 5, -5);
  Test("-5", NaN, -5);
	Test("++x", 5, 5);
	Test("+x", 5, 5);
	Test("+5", NaN, 5);
	Test("+-x", 5, -5);
	Test("-+x", 5, -5);

	//Test power function
	Test("x^2", -2, 4);
	Test("x^2", 10, 100);
	Test("-x^2", 10, -100);
	Test("(-x)^2", 10, 100);
	Test("x^x", 0, 1);         //Important test
	Test("x^0", 458.789, 1);   //Important test
	Test("0^x", 456.789, 0);   //Important test
	Test("log(x)^2", 100, 4);
	TestError("x/(x^2-4)", -2, ecDivByZero);
	TestError<long double>(L"(-2)^x", 2.2, ecPowCalcError);
	Test<TComplex>(L"(-2)^x", 2.2, TComplex(3.717265962,2.70075181));
	TestError<long double>(L"(-2)^x", 2.3, ecPowCalcError);
	TestError("x^(-1)", 0, ecPowCalcError);
	TestError<TComplex>(L"0^x", TComplex(3,1), ecPowCalcError);
	Test<TComplex>(L"0^x", TComplex(3,0), 0);
	Test("e^(2x)", 2, M_E*M_E*M_E*M_E);
	Test("e^2x", 2, M_E*M_E*M_E*M_E); //Same as the above
	Test("x^2^3", 10, 1E8);
	Test("(x^2)^3", 10, 1E6);
	Test("3^x^2", 4, 43046721);
	TestError<TComplex>(L"0^x", TComplex(-2.8, 1), ecPowCalcError);
	TestError("x^-2.8", 0, ecPowCalcError);
	Test("e^x", -10000, 0);

	//Test power with fraction handling
	Test<long double>(L"x^(1/3)", -8, -2);
	Test<TComplex>(L"x^(1/3)", -8, TComplex(1,1.732050807568877));
	Test<long double>(L"x^(2/6)", -8, -2);
	Test<TComplex>(L"x^(2/6)", -8, TComplex(1,1.732050807568877));
	Test<long double>(L"x^(2/3)", -8, 4);
	Test<TComplex>(L"x^(2/3)", -8, TComplex(-2, 3.464101615137755));
	TestError("x^(2/0)", -8, ecDivByZero);
	Test("x^(2/3)", 0, 0);
	Test("x^(4/2)", -2, 4);

	//Test trigonometry functions
	Test("sin(x)", PI/2, 1);
	Test("sin(x)", 90, 1, Degree);
	Test("cos(x)", PI/2, 0);
	Test("cos(x)", 90, 0, Degree);
	Test("tan(x)", PI/4, 1);
	Test("tan(x)", 45, 1, Degree);
	Test("sec(x)", PI, -1);
	Test("sec(x)", 180, -1, Degree);
  Test("csc(x)", PI/2, 1);
  Test("csc(x)", 90, 1, Degree);
  Test("cot(x)", PI/4, 1);
	Test("cot(x)", 45, 1, Degree);

  //Test inverse trigonometry functions
	Test("asin(x)", 1, PI/2);
	Test("asin(x)", 1, 90, Degree);
	Test("asin(x)", 0.4, 0.41151684606748801938473789761734);
	Test("asin(x)", 0.5, PI/6);
	Test<TComplex>(L"asin(x)", 1.5, TComplex(1.570796327, 0.9624236501)); //sign on imag can be both positive and negative
	TestError<long double>(L"asin(x)", 1.5, ecArcError);
	Test<TComplex>(L"asin(x)", TComplex(2.5, 1), TComplex(1.16462351, 1.658693299));

	Test("acos(x)", 0, PI/2);
	Test("acos(x)", 0, 90, Degree);
	Test("acos(x)", 0.4, 1.1592794807274085998465837940224);
	Test<TComplex>(L"acos(x)", 1.5, TComplex(0, 0.9624236501));
	TestError<long double>(L"acos(x)", 1.5, ecArcError);
	Test<TComplex>(L"acos(x)", TComplex(2.5, 1), TComplex(0.4061728165, -1.658693299));

	Test("atan(x)", 1, PI/4);
	Test("atan(x)", 0, 0);
	Test("atan(x)", 1, 45, Degree);
	Test<TComplex>(L"atan(x)", TComplex(2.5, 1), TComplex(1.233425856, 0.1236740605));

	Test("asec(x)", -1, PI);
	Test("asec(x)", -1, 180, Degree);
	Test("acsc(x)", 1, PI/2);
	Test("acsc(x)", 1, 90, Degree);
	Test("acot(x)", 1, PI/4);
	Test("acot(x)", 1, 45, Degree);
	Test("acot(x)", 0, PI/2);
	Test("acot(x)", 0, 90, Degree);

	//Test logarihms
	Test("log(x)", 10000, 4);
	Test("logb(x,2)", 32, 5);
	Test("ln(x)", EULER*EULER, 2);
	Test("exp(x)", 2, EULER*EULER);
  TestError("log(-x)", 0, ecLogError);

  //Test square and roots
  Test("sqr(x)", 5, 25);
  Test("sqrt(x)", 100, 10);
  Test("root(3,x)", 125, 5);
  Test<long double>(L"root(3,x)", -27, -3);
  Test<TComplex>(L"root(3,x)", -27, TComplex(1.5, 2.598076211));
  TestError<long double>(L"root(3.5, x)", -27, ecPowCalcError);
  Test("root(4,x)", 625, 5);
  TestError<long double>(L"root(4,x)", -625, ecPowCalcError);
	Test<TComplex>(L"root(4,x)", -4, TComplex(1, 1));

	Test("fact(x)", 5, 120);

	Test("sign(x)", 7.98, 1);
  Test("sign(x)", -7.98, -1);
	Test("sign(x)", 0, 0);
  Test<TComplex>(L"sign(x)", TComplex(5, 5), TComplex(SQRT_2, SQRT_2));
	Test<TComplex>(L"sign(x)", TComplex(4, -3), TComplex(4.0/5, -3.0/5));

  Test("u(x)", 7.98, 1);
	Test("u(x)", -7.98, 0);
  Test("u(x)", 0, 1);

  //Test hyperbolic functions
  Test("sinh(x)", 5, 74.20321058);
  Test("cosh(x)", 5, 74.20994852);
	Test("tanh(x)", 5, 0.999909204);

	Test("asinh(x)", 5, 2.31243834);
	Test("asinh(x)", 0, 0);
	Test<TComplex>(L"asinh(x)", TComplex(1.5, 0.7), TComplex(1.256224291, 0.3776375126));

	Test("acosh(x)", 5, 2.29243167);
	Test("acosh(x)", 1, 0);
	Test<TComplex>(L"acosh(x)", -1, TComplex(0, PI));
	Test<TComplex>(L"acosh(x)", TComplex(1.5, 1), TComplex(1.260475188, 0.6644205508));
	TestError<long double>(L"acosh(x)", -1, ecACoshError);

	Test("atanh(x)", 0.5, 0.54930614);
	Test("atanh(x)", 0, 0);
	TestError("atanh(x)", 1, ecATanhError);
	TestError("atanh(x)", -1, ecATanhError);
	Test<TComplex>(L"atanh(x)", TComplex(2.5, 1), TComplex(0.351335639, 1.415944855));

	Test("abs(x)", -4.67, 4.67);
	Test<TComplex>(L"abs(x)", TComplex(3, 4), 5);
	Test<TComplex>(L"arg(x)", TComplex(3, 4), 0.927295218);
	Test<TComplex>(L"arg(x)", TComplex(3, 4), 53.13010235, Degree);
	Test<TComplex>(L"conj(x)", TComplex(3, 4), TComplex(3, -4));
  Test<TComplex>(L"re(x)", TComplex(3, 4), 3);
  Test<TComplex>(L"im(x)", TComplex(3, 4), 4);
  Test("re(x)", 3, 3);
	Test("im(x)", 3, 0);
  TestError<long double>(L"re(e^(i*x))", 1, ecComplexError);
  TestError<long double>(L"im(e^(i*x))", 1, ecComplexError);
  Test<TComplex>(L"re(e^(i*x))", 1, 0.5403023059);
  Test<TComplex>(L"im(e^(i*x))", 1, 0.8414709848);

  //Rounding
	Test("trunc(x)", -4.567, -4);
	Test("fract(x)", -4.567, -0.567);
  Test("ceil(x)", -4.567, -4);
  Test("floor(x)", -4.567, -5);
  Test("Round(x, 3)", 412.4572, 412.457);
  Test("Round(x, 2)", 412.4572, 412.46);
  Test("Round(x, 1)", 412.4572, 412.5);
  Test("Round(x, 0)", 412.4572, 412);
	Test("Round(x, -2)", 412.4572, 400);
  Test("Round(x, 2)", -412.4572, -412.46);

  //Boolean functions
  Test("not(x)", -4.567, 0);
  Test("not(x)", 0, 1);
	Test("x=5", 5, 1);
	Test("x=5", 3, 0);
//	Test("((1-x)^2)^.5+((1+2)^2)^.5 = ((x+2)^2)^.5", 5, 1);
//	Test("((1-x)^2)^.5+((1+2)^2)^.5 = ((x+2)^2)^.5", -5, 0);
	Test("x<5", 4, 1);
	Test("x<5", 5, 0);
	Test("x<5", 6, 0);
  Test("x>5", 4, 0);
	Test("x>5", 5, 0);
  Test("x>5", 6, 1);
  Test("x<=5", 4, 1);
  Test("x<=5", 5, 1);
  Test("x<=5", 6, 0);
  Test("x>=5", 4, 0);
	Test("x>=5", 5, 1);
  Test("x>=5", 6, 1);
  Test("3<x<6", 2, 0);
	Test("3<x<6", 3, 0);
  Test("3<x<6", 4, 1);
  Test("3<x<6", 6, 0);
  Test("3<x<6", 7, 0);

  //Test corner cases for compare
  Test("x = 0.2", 0.2, 1);
  Test("x <> 0.2", 0.2, 0);
  Test("x < 0.2", 0.2, 0);
  Test("x <= 0.2", 0.2, 1);
  Test("x > 0.2", 0.2, 0);
  Test("x <= 0.2", 0.2, 1);

  Test("x = 0.01*20", 0.2, 1);
  Test("x <> 0.01*20", 0.2, 0);
  Test("x < 0.01*20", 0.2, 0);
  Test("x <= 0.01*20", 0.2, 1);
  Test("x > 0.01*20", 0.2, 0);
  Test("x <= 0.01*20", 0.2, 1);


  //Test logical functions
  Test("1 and 1", 0, 1);
  Test("0 and 1", 0, 0);
  Test("1 and 0", 0, 0);
  Test("0 and 0", 0, 0);
  Test("1 or 1", 0, 1);
	Test("0 or 1", 0, 1);
  Test("1 or 0", 0, 1);
  Test("0 or 0", 0, 0);
  Test("1 xor 1", 0, 0);
  Test("0 xor 1", 0, 1);
	Test("1 xor 0", 0, 1);
  Test("0 xor 0", 0, 0);

	//Test picewise functions
	Test("if(x<3, 5, 9)", 2, 5);
  Test("if(x<3, 5, 9)", 3, 9);
  Test("if(x<3, 5, 9)", 4, 9);
  Test("range(2,x,5)", 1, 2);
	Test("range(2,x,5)", 3, 3);
  Test("range(2,x,5)", 6, 5);
  Test("min(8, 2, 5)", 0, 2);
	Test("min(x,2)", 1, 1);
  Test("max(8, 2, 5)", 0, 8);

	Test("gamma(x)", 6, 120);
  Test("mod(x,5)", 12, 2);
  Test("mod(x,5)", 10, 0);
  Test("mod(x,-1)", -2, 0);
  Test("mod(x,-5)", 12, -3);
  Test("mod(x,-5)", -12, -2);
  Test("mod(x, 1.2)", 5.6, 0.8);
  Test("mod(x, 1.2)", -5.6, 0.4);
  Test("dnorm(x, 5, 7)", 3, 0.054712394277745);
  Test("dnorm(x)", 3, 0.00443184841193801);
	Test("dnorm(x, 0, 1)", 3, 0.00443184841193801);

	//Test backward compatibility
  Test("integrate(x^2,2,5)", NaN, 39);
	TestError("sum(x, 3, 7)", NaN, ecArgCountError);
  TestError("product(x, 3, 7)", NaN, ecArgCountError);

	//Test improved integrate
	Test("integrate(x^2,x,2,5)", NaN, 39);
  Test("integrate(dnorm(x,100,60),x,-inf,100)", 0, 0.5);
  Test("integrate(dnorm(x,100,20),x,-inf,100)", 0, 0.5);
  Test("integrate(dnorm(x,100,60),x,100,inf)", 0, 0.5);
  Test("integrate(dnorm(x,100,20),x,100,+inf)", 0, 0.5);
	Test("integrate(e^x,x,-inf,0)", 0, 1);
	Test("integrate(e^x,x,0,-inf)", 0, -1);
	Test("integrate(e^-x,x,inf,0)", 0, -1);
	Test("integrate(e^-x,x,0,inf)", 0, 1);
	Test("integrate(e^-abs(x),x,-inf,inf)", 0, 2);
	Test("integrate(e^-abs(x),x,inf,-inf)", 0, -2);
	Test("integrate(x*t^2, t, 0, 3)", 2, 18);
	Test("sum(x, x, 3, 7)", NaN, 3+4+5+6+7);
	Test("product(x, x, 3, 7)", NaN, 3*4*5*6*7);
	Test("sum(x*t, t, 3, 7)", 2, 2*3+2*4+2*5+2*6+2*7);
	Test("product(x*t, t, 3, 7)", 2, 2*3*2*4*2*5*2*6*2*7);
	Test("sum(min(k,8),k,0,x)", 10, 52);
	Test("sum(min(k,8)*2,k,0,x)", 10, 104);
	Test("sum(min((k),[6+2])*(2)/2,k,0,x)", 10, 52);
	Test("sum(sum(k*l,k,1,10),l,1,x)", 2, 165);
	Test("integrate(1/((1+x)*x^(1/2)),x, 0, inf)", 0, PI);
	Test<long double>(L"integrate(1/x^(2/3),-1,1)", 0, 6);
	TestError<TComplex>(L"integrate(1/x^(2/3),-1,1)", 0, ecNoResult);
  TestError("integrate(t, t, 4test, x)", 0, EFuncError(ecUnknownVar, L"test"));
  TestError("integrate(t, t, x, 4test)", 0, EFuncError(ecUnknownVar, L"test"));
//	TestError("sum(x,k+1,0,10)", 0, ecLiteralExpected); To be fixed in Graph 4.5
//	TestError("sum(x,x+1,0,10)", 0, ecLiteralExpected);
//	TestError("sum(x,1,0,10)", 0, ecLiteralExpected);

  //Test infinity
	Test("inf", 0, INF);
	Test("-inf", 0, -INF);
  Test<long double>(L"2*inf", 0, INF);
  Test<TComplex>(L"2*inf", 0, TComplex(INF, NaN));
	Test("2+inf", 0, INF);
  Test<long double>(L"inf*x", 0, NaN);
  Test<TComplex>(L"inf*x", 0, TComplex(NaN, NaN));
  Test<long double>(L"inf/inf", 0, NaN);
	Test<TComplex>(L"inf/inf", 0, TComplex(NaN, NaN));

	//Combined test cases
	Test("(1.01^x-1)/0.01", 1, 1);

	//Test very long function (requires more stack space)
	Test(
		"-2.333029E-2022*x^700+1.9339476E-2019*x^699+1.6771001E-2018*x^698-7.9028945E-2014*x^697+1.5995975E-2012*x^696+3.3754939E-2008*x^695+4.0897729E-2005*x^694-3.2011901E-2002*x^693-6.3383446E-2000*x^692+4.7620169E-1998*x^691-1.0312604E-1993*x^690+6.6797199E-1991*x^689-1.0506451E-1987*x^688-8.8050916E-1987*x^687+1.2206893E-1981*x^686+1.0950178E-1978*x^685-2.4579766E-1976*x^684-2.060247E-1973*x^683-1.4953807E-1970*x^682+2.3779016E-1967*x^681+8.4802251E-1965*x^680+2.1008883E-1961*x^679-2.6041605E-1958*x^678+3.6992262E-1956*x^677-2.0430767E-1952*x^676+5.7750608E-1950*x^675-3.5456781E-1947*x^674-6.9518406E-1944*x^673+3.0683362E-1941*x^672+3.0615482E-1938*x^671+1.2802912E-1935*x^670-2.8888339E-1933*x^669+1.6124016E-1929*x^668-6.2010894E-1927*x^667-1.5252784E-1924*x^666+3.5173322E-1921*x^665+4.338378E-1918*x^664+7.747134E-1916*x^663-2.4247544E-1912*x^662+8.3325567E-1909*x^661-2.3431892E-1906*x^660-8.7754876E-1904*x^659-1.2458204E-1900*x^658-2.3523902E-1897*x^657-1.0677109E-1894*x^656+6.0423093E-1892*x^655-3.2081108E-1890*x^654-3.4926303E-1886*x^653+6.375864E-1883*x^652-1.2857767E-1880*x^651-1.0115965E-1877*x^650-4.149308E-1875*x^649+9.3399409E-1872*x^648+1.2437709E-1868*x^647-7.0000427E-1866*x^646-1.0980082E-1862*x^645-4.9953439E-1860*x^644-2.3962827E-1857*x^643-2.3166145E-1854*x^642+1.462227E-1852*x^641-2.1956896E-1848*x^640-3.0908161E-1845*x^639+2.2695557E-1842*x^638-2.2263798E-1840*x^637-6.5565916E-1837*x^636+7.6693756E-1834*x^635+3.494297E-1831*x^634+4.6792499E-1828*x^633-2.7503837E-1825*x^632+6.1161529E-1822*x^631+4.2869696E-1819*x^630-2.9225447E-1816*x^629+1.5447288E-1813*x^628+5.6552235E-1811*x^627+9.4749993E-1808*x^626-3.0210998E-1805*x^625-5.7002643E-1802*x^624-4.5351898E-1799*x^623+5.6541133E-1796*x^622+3.7713618E-1793*x^621-3.1900198E-1790*x^620-1.4067803E-1787*x^619-2.1903248E-1785*x^618+1.7847451E-1781*x^617-1.117554E-1779*x^616+8.3900963E-1776*x^615+1.9922743E-1773*x^614+7.3231118E-1770*x^613-6.5806867E-1767*x^612-2.0396134E-1764*x^611-3.8036961E-1761*x^610+1.4621178E-1758*x^609-2.6206651E-1756*x^608+2.7071367E-1752*x^607-1.3790939E-1750*x^606+1.184904E-1746*x^605-1.1773275E-1744*x^604+9.7009584E-1741*x^603-9.8599725E-1738*x^602-4.1956854E-1735*x^601-1.9083434E-1732*x^600+1.902005E-1729*x^599+1.6375401E-1727*x^598-2.0416302E-1724*x^597-1.4292664E-1720*x^596-1.1443412E-1717*x^595+4.4808075E-1715*x^594+1.3637784E-1712*x^593+5.1003053E-1709*x^592+3.5406131E-1707*x^591-2.2438274E-1703*x^590-3.2567925E-1700*x^589+1.9464453E-1697*x^588+7.1023557E-1695*x^587+2.4236887E-1691*x^586-9.2744313E-1689*x^585-6.6932103E-1688*x^584+3.3403326E-1683*x^583+4.9332668E-1680*x^582-1.9330345E-1677*x^581+9.0723794E-1675*x^580+3.0565122E-1671*x^579-1.9906566E-1668*x^578+5.8581743E-1666*x^577-1.4701328E-1662*x^576-8.2564963E-1660*x^575-2.4974293E-1656*x^574-1.421161E-1653*x^573-4.1474532E-1651*x^572+1.7445971E-1648*x^571+6.260792E-1646*x^570+1.03932E-1642*x^569+1.963698E-1639*x^568+3.2498217E-1636*x^567-2.6067149E-1633*x^566-1.0725709E-1630*x^565+8.2228966E-1628*x^564+1.4661556E-1624*x^563-6.2959245E-1622*x^562+1.0842825E-1619*x^561+8.5582999E-1617*x^560-2.6839655E-1613*x^559-6.8228249E-1611*x^558-4.4251005E-1607*x^557+1.1350145E-1605*x^556+2.0911419E-1601*x^555-4.8554406E-1599*x^554-6.918057E-1596*x^553+2.0400083E-1592*x^552-6.8384374E-1591*x^551-6.7370636E-1587*x^550-1.4152862E-1585*x^549+3.1528654E-1582*x^548+3.6646289E-1578*x^547-4.8632552E-1575*x^546+1.2297497E-1573*x^545+9.2302593E-1570*x^544+1.3082041E-1566*x^543-2.394438E-1564*x^542+8.1552627E-1561*x^541-7.0177958E-1558*x^540+3.3116579E-1555*x^539+3.9198856E-1553*x^538+1.9236469E-1549*x^537+1.5617868E-1546*x^536+3.4579668E-1544*x^535-3.4996779E-1540*x^534-3.5696062E-1538*x^533-6.8004029E-1535*x^532+2.3552349E-1532*x^531+5.7949419E-1529*x^530+1.7098119E-1526*x^529+1.6509726E-1523*x^528+5.5359179E-1520*x^527+4.8053538E-1518*x^526-4.8314777E-1515*x^525-1.8540976E-1513*x^524-5.6723661E-1509*x^523+2.7044491E-1505*x^522-3.5557495E-1503*x^521+4.5358345E-1500*x^520+3.9241466E-1497*x^519"
		"-1.6055504E-1494*x^518-2.6545938E-1491*x^517-1.4306903E-1488*x^516-1.583004E-1485*x^515-1.2522246E-1482*x^514+9.6469921E-1480*x^513+5.8032174E-1477*x^512-1.9990274E-1474*x^511-6.8240644E-1471*x^510-6.8840738E-1468*x^509-4.957759E-1465*x^508+5.2136852E-1462*x^507-4.3453196E-1460*x^506-7.0329114E-1457*x^505+1.4252092E-1453*x^504-2.1561287E-1450*x^503-3.1739172E-1448*x^502+4.4463257E-1445*x^501-3.9690213E-1442*x^500+6.456814E-1441*x^499-1.2685605E-1436*x^498+1.4063629E-1433*x^497-2.686993E-1430*x^496-2.3786942E-1427*x^495-7.768752E-1425*x^494+8.4674663E-1422*x^493-1.6426823E-1419*x^492+5.283485E-1416*x^491+4.2072509E-1413*x^490-3.0151202E-1410*x^489-2.9462596E-1407*x^488-1.478471E-1404*x^487-6.8644147E-1401*x^486-5.30711E-1399*x^485+1.0348164E-1395*x^484+1.4361296E-1394*x^483+5.4916059E-1390*x^482+4.1509328E-1387*x^481+5.1855909E-1384*x^480-4.9299061E-1381*x^479+1.9528757E-1378*x^478+1.3569796E-1374*x^477-3.8353049E-1372*x^476-1.3185463E-1369*x^475-1.9865216E-1366*x^474+1.9691513E-1364*x^473-2.1690197E-1361*x^472+1.1821632E-1357*x^471-8.9135434E-1356*x^470+8.1989046E-1353*x^469+7.9534861E-1349*x^468+7.3599714E-1346*x^467+4.4012146E-1343*x^466-3.5629752E-1340*x^465+3.402015E-1337*x^464+1.2145292E-1334*x^463+1.2403373E-1332*x^462-6.9149733E-1329*x^461-1.768924E-1326*x^460+4.0026871E-1323*x^459-1.9146347E-1320*x^458-3.3206039E-1317*x^457-1.9270631E-1314*x^456-1.3358156E-1311*x^455+2.5426038E-1308*x^454-6.7966909E-1306*x^453+7.4115427E-1303*x^452+1.8406256E-1300*x^451-2.1172389E-1297*x^450-2.7119807E-1294*x^449-3.6634808E-1291*x^448+3.677628E-1288*x^447+5.4528726E-1285*x^446+3.332251E-1282*x^445+1.180606E-1279*x^444+1.5668853E-1276*x^443-1.3562362E-1273*x^442-4.7137087E-1271*x^441+1.3812525E-1267*x^440-4.4377119E-1265*x^439-1.5835092E-1262*x^438-1.1440401E-1259*x^437-8.1948952E-1256*x^436-6.3762504E-1254*x^435-4.1516092E-1250*x^434+6.2833467E-1248*x^433-1.2780904E-1244*x^432+1.5520658E-1241*x^431-1.3874217E-1238*x^430+1.3577003E-1236*x^429-1.6964959E-1232*x^428-7.3992801E-1230*x^427-5.4678767E-1227*x^426+4.3627942E-1224*x^425+3.569686E-1221*x^424-3.4970846E-1218*x^423+7.7732575E-1216*x^422-1.6203976E-1212*x^421+5.6436815E-1210*x^420-4.9630286E-1207*x^419-5.4077441E-1204*x^418+3.5019249E-1201*x^417-2.2280065E-1198*x^416+1.096874E-1195*x^415+1.7058036E-1192*x^414+1.7787675E-1189*x^413-2.4901615E-1186*x^412+7.3727133E-1184*x^411+1.5530363E-1180*x^410+8.5694774E-1178*x^409+1.247646E-1175*x^408+2.4543997E-1172*x^407-5.5038235E-1169*x^406-3.1544726E-1166*x^405+3.5190072E-1163*x^404+3.1769751E-1160*x^403+2.1385924E-1157*x^402-2.7105364E-1154*x^401-5.6573636E-1152*x^400+3.8035921E-1149*x^399-1.4810071E-1146*x^398+1.3025091E-1143*x^397+7.8765514E-1140*x^396+4.0103891E-1137*x^395+1.9898508E-1134*x^394-1.012869E-1131*x^393+2.7318811E-1128*x^392+3.5249553E-1126*x^391-1.5696122E-1123*x^390+8.3732641E-1120*x^389+6.7298267E-1117*x^388-4.5559138E-1114*x^387-3.9221493E-1111*x^386-2.4969237E-1108*x^385-4.6116099E-1105*x^384-4.0819466E-1102*x^383-1.0313815E-1099*x^382+4.2182505E-1097*x^381+7.6720053E-1094*x^380+3.3140633E-1091*x^379+1.1875098E-1087*x^378-2.6164272E-1085*x^377+7.8707913E-1083*x^376+4.6767252E-1079*x^375-4.6882023E-1078*x^374+1.7416299E-1073*x^373-8.7755958E-1071*x^372-3.5958882E-1067*x^371+1.4847556E-1064*x^370-1.0822586E-1061*x^369-1.531505E-1058*x^368-2.4523697E-1057*x^367+3.9670473E-1053*x^366+1.1410609E-1050*x^365-3.0375942E-1047*x^364-9.4240784E-1045*x^363-4.6901777E-1041*x^362-1.8403172E-1039*x^361-6.3349242E-1036*x^360-1.618855E-1032*x^359+1.1487118E-1030*x^358-1.9488235E-1028*x^357-1.9317074E-1024*x^356+5.5075513E-1021*x^355+1.1823995E-1017*x^354-7.526061E-1015*x^353+4.065837E-1013*x^352+3.8996829E-1009*x^351-1.2740395E-1006*x^350+3.4319399E-1004*x^349+9.0095735E-1002*x^348+7.3283467E-998*x^347+4.2063791E-995*x^346-1.7808691E-992*x^345-2.115094E-989*x^344+1.2800732E-986*x^343+2.7466361E-983*x^342-1.8950564E-980*x^341-6.0664125E-978*x^340+3.6156414E-975*x^339-3.4326047E-972*x^338+7.6120703E-969*x^337+5.7891755E-966*x^336"
		"+2.5988154E-963*x^335-7.3107244E-960*x^334+2.779674E-957*x^333+6.3494399E-954*x^332-2.2839692E-951*x^331-7.8601804E-949*x^330-7.6293146E-946*x^329+1.6399157E-942*x^328-3.9923695E-940*x^327-1.6333858E-937*x^326+1.3876874E-933*x^325+7.3643182E-931*x^324+1.0294517E-928*x^323-1.7965896E-925*x^322+1.911065E-923*x^321-9.5859006E-920*x^320-1.0505227E-916*x^319-8.3452069E-914*x^318+2.0304239E-911*x^317+4.6178137E-908*x^316+2.1777292E-905*x^315-8.2013516E-903*x^314+5.3961063E-899*x^313-5.2656103E-896*x^312+7.1073808E-895*x^311-3.5839609E-890*x^310-4.5421724E-888*x^309-2.2446115E-885*x^308-1.2926803E-882*x^307-8.3573597E-879*x^306-4.1936551E-876*x^305+5.6240742E-873*x^304-7.7218184E-870*x^303+1.7372158E-867*x^302-6.9060363E-865*x^301-1.4128831E-861*x^300-2.6997971E-860*x^299-1.5136548E-855*x^298+8.8916768E-853*x^297+6.5722605E-850*x^296-6.9549682E-847*x^295+3.2205999E-844*x^294-6.1228793E-841*x^293-5.2837747E-839*x^292-2.563983E-837*x^291-4.6664063E-832*x^290-3.0449242E-829*x^289+2.2438327E-826*x^288+1.4225348E-824*x^287+6.7211982E-821*x^286-7.5557267E-818*x^285+1.9279248E-815*x^284+3.3775274E-812*x^283+2.4639484E-809*x^282+6.0213874E-806*x^281-2.13973E-804*x^280-1.0408845E-800*x^279+1.5767971E-797*x^278+1.6820349E-794*x^277-2.0202221E-791*x^276+2.3715015E-789*x^275+1.3388012E-785*x^274+2.9082903E-783*x^273+5.7935927E-780*x^272-5.2816436E-777*x^271+5.0077895E-774*x^270-6.0525292E-772*x^269-1.4881268E-768*x^268-1.7349582E-765*x^267+1.068935E-762*x^266-5.7623026E-760*x^265-8.1534947E-758*x^264+5.077619E-754*x^263-5.6934882E-751*x^262+7.285403E-749*x^261-1.2320645E-745*x^260-2.3836118E-742*x^259+1.1109699E-739*x^258-2.2903395E-737*x^257-8.5215219E-736*x^256-5.6969009E-731*x^255+1.5707991E-727*x^254+6.3031429E-726*x^253-2.8102361E-722*x^252+7.997187E-720*x^251-3.3577696E-716*x^250-1.9141445E-713*x^249+3.610011E-711*x^248-8.794305E-708*x^247-3.8365516E-705*x^246+2.2466224E-702*x^245+1.3321169E-699*x^244-9.7581424E-697*x^243-1.3789627E-693*x^242+1.0374108E-690*x^241+5.1830478E-687*x^240+1.8775442E-684*x^239+1.8667581E-682*x^238+2.4356979E-678*x^237+5.1395725E-676*x^236+1.5387927E-672*x^235+1.0338951E-669*x^234-7.0785646E-667*x^233-2.2419707E-664*x^232+3.3320228E-661*x^231-2.4416838E-658*x^230-5.6936588E-655*x^229+2.1932976E-652*x^228+6.2791899E-650*x^227-3.7945092E-646*x^226+2.7336537E-644*x^225-1.4941139E-641*x^224-2.3032575E-638*x^223-4.2050513E-635*x^222+5.3127372E-632*x^221-4.8338957E-629*x^220-3.3513366E-626*x^219-8.896321E-625*x^218+2.6635543E-620*x^217-8.1682916E-618*x^216+6.6376054E-615*x^215-1.3155331E-611*x^214+3.8855549E-609*x^213-7.6419946E-606*x^212+2.9389622E-603*x^211-6.576811E-600*x^210+8.9048364E-597*x^209+2.1071151E-594*x^208+1.71931E-591*x^207+6.4198793E-589*x^206+5.7283843E-587*x^205-1.4027933E-582*x^204-1.5153744E-579*x^203+1.7461544E-577*x^202+1.8717018E-574*x^201+6.4343601E-574*x^200+1.8702364E-569*x^199+1.3982264E-565*x^198-4.5952915E-562*x^197+2.6800689E-559*x^196+2.2038894E-557*x^195-3.3275411E-554*x^194-3.2940931E-551*x^193+1.1023099E-547*x^192-3.4049581E-545*x^191+3.2038295E-542*x^190-2.6200783E-539*x^189+4.4235107E-536*x^188+2.5517429E-533*x^187-2.8445131E-530*x^186+6.4650921E-528*x^185-2.2213567E-524*x^184-1.4686913E-522*x^183+9.9907204E-520*x^182+3.465838E-516*x^181+6.3498339E-513*x^180+1.4164772E-510*x^179-8.047211E-508*x^178-1.5432806E-504*x^177+2.5555412E-501*x^176+1.6695199E-498*x^175-2.2209331E-495*x^174+7.6261628E-493*x^173+5.1556454E-490*x^172-6.6216486E-487*x^171+4.9729834E-484*x^170+1.9723508E-481*x^169-2.1642516E-478*x^168-2.3132008E-475*x^167-1.2629233E-472*x^166-2.1418314E-469*x^165-3.7194245E-467*x^164+2.8071745E-464*x^163-1.4869293E-460*x^162-6.5590452E-459*x^161-4.5580989E-455*x^160+1.0880085E-451*x^159+1.1986292E-450*x^158+4.5207443E-446*x^157+1.0943644E-443*x^156-6.7120131E-443*x^155+3.1662171E-437*x^154-1.019977E-434*x^153-5.4201015E-432*x^152-1.1848908E-429*x^151-5.1353572E-427*x^150-6.1432566E-423*x^149-7.4191589E-420*x^148+2.3331131E-418*x^147-2.7339692E-414*x^146+2.255655E-411*x^145"
		"-2.4516608E-409*x^144-1.584963E-406*x^143-1.3159888E-402*x^142+1.3058346E-399*x^141+1.4714176E-397*x^140+3.6361542E-394*x^139+3.0487887E-391*x^138-1.2078999E-388*x^137-1.5682898E-385*x^136-3.5737764E-383*x^135+2.3012269E-379*x^134+7.7729893E-377*x^133+9.4734204E-374*x^132-7.8284555E-371*x^131-8.2060351E-368*x^130+6.7487349E-365*x^129-4.9648748E-362*x^128-2.6068767E-360*x^127-9.3829607E-357*x^126+1.3704313E-353*x^125-6.0139498E-351*x^124+1.1583348E-347*x^123-1.4452501E-344*x^122+1.5340448E-341*x^121-6.5795826E-339*x^120-6.9860728E-337*x^119+5.5738796E-333*x^118+1.0519228E-330*x^117+4.6370705E-327*x^116-4.8983763E-324*x^115+3.0037306E-321*x^114-1.5558942E-318*x^113-1.3975779E-315*x^112-2.7048309E-313*x^111-3.6912911E-310*x^110-1.3880656E-307*x^109+3.018661E-304*x^108+1.6970433E-301*x^107-3.5719388E-298*x^106-9.373685E-296*x^105-2.0290298E-292*x^104+1.9441834E-289*x^103+9.5222945E-287*x^102-4.0165925E-284*x^101+2.8757376E-281*x^100-1.4988478E-277*x^99-5.9257389E-275*x^98+2.5680766E-272*x^97+4.8531499E-270*x^96+4.7523285E-266*x^95+2.110257E-263*x^94+1.1365806E-260*x^93-1.0803042E-258*x^92+7.3555399E-255*x^91-5.3254522E-252*x^90+2.5416275E-250*x^89+4.6205704E-246*x^88+2.8009905E-243*x^87+2.2664122E-240*x^86+1.8759549E-237*x^85-3.2603626E-234*x^84-1.6732628E-231*x^83-7.0231444E-229*x^82-1.4602139E-226*x^81+1.4119154E-222*x^80-1.1175263E-219*x^79-7.7249787E-217*x^78-1.887663E-214*x^77+3.4000017E-211*x^76-9.6278956E-209*x^75-1.4095326E-205*x^74+1.2442781E-202*x^73+2.6819949E-200*x^72-5.9221317E-198*x^71-1.2948103E-193*x^70-2.7261984E-191*x^69+1.0108047E-188*x^68+5.410297E-185*x^67-9.9823924E-183*x^66+6.9124388E-179*x^65-1.0201331E-176*x^64-6.2068878E-174*x^63+8.9018769E-171*x^62+4.2872328E-168*x^61-3.3241971E-165*x^60+1.8297275E-162*x^59-1.8119075E-160*x^58-4.0196731E-156*x^57+2.9162821E-153*x^56+4.3381162E-151*x^55-2.0496271E-147*x^54+1.5757668E-144*x^53-2.8148664E-141*x^52+6.5097677E-139*x^51-3.8699498E-136*x^50-5.7830883E-133*x^49+4.2867382E-130*x^48+3.4285646E-127*x^47+5.84482E-125*x^46-3.7827769E-121*x^45-3.6041902E-118*x^44+4.4321866E-115*x^43+1.2734575E-112*x^42-2.630656E-109*x^41+1.430322E-106*x^40+8.1633646E-104*x^39+2.2720705E-101*x^38-4.4016878E-98*x^37+1.5324697E-95*x^36+3.1456575E-92*x^35-5.4705827E-89*x^34-7.2328603E-87*x^33+2.0202838E-83*x^32-1.4525912E-80*x^31-6.699928E-78*x^30+9.5305317E-75*x^29+2.6919149E-72*x^28-7.9146797E-69*x^27+3.0904641E-66*x^26+3.6403704E-63*x^25-1.2728357E-60*x^24-2.866256E-57*x^23+2.7711599E-54*x^22+5.0553912E-52*x^21-5.1739766E-49*x^20+1.2205605E-46*x^19-4.0638358E-43*x^18-3.4632726E-40*x^17-8.5335414E-38*x^16+3.680713E-34*x^15+1.7344365E-31*x^14-3.3608113E-28*x^13+1.1529432E-25*x^12+6.2634458E-23*x^11-2.6112747E-20*x^10-1.8123534E-17*x^9+2.1412961E-14*x^8-3.6402894E-11*x^7+1.0185453E-09*x^6+3.1344186E-05*x^5-0.010808068*x^4-10.405029*x^3+8693.4986*x^2-2424333.9*x+2.4680223E+08",
		0, 2.4680223E+08);

  //Test undef
	TestError("undef", 0, ecNotDefError);
	Test("if(x<5, x, undef)", 0, 0);
	TestError("if(x<5, x, undef)", 6, ecNotDefError);

  //Test block function
	Test("if(x<-2, x, 2x)", -3, -3);
  Test("if(x<-2, x, 2x)", 1, 2);
  Test("if(x<-2, x, x<3, 2x, 3x)", 1, 2);
  Test("if(x<-2, x, x<3, 2x, 3x)", 4, 12);
  Test("2+if(x<-2, x, x<3, 2x, 3x)", 4, 14);
  Test("if(x<-2, x, x<3, 2x)", 1, 2);
	TestError("if(x<-2, x, x<3, 2x)", 4, ecNotDefError);

  //Test Zeta function
  Test("zeta(x)", 0, -0.5);
  Test("zeta(x)", 10, 1.000994575);
  TestError("zeta(x)", 1, ecNotDefError);
  Test("zeta(x)", -5, -1.0/252);

	//Test Omega function
	TestError<long double>(L"W(x)", -PI/2, ecComplexError);
	Test<TComplex>(L"W(x)", -PI/2, TComplex(0, PI/2));
  Test("W(-1/e)", 0, -1);
	Test("W(x)", 0, 0);
  Test("W(x)", -M_LN2/2, -M_LN2);
  Test("W(x)", 1, 0.5671432904097838729999686622);
	Test("W(x)", M_E, 1);

	//Test handling of other symbols
	Test(L"80 \x2212 2*x", 5, 70); //Test Minus sign (0x2212)
	Test(L"\x2212 5x", 2, -10); //Test Minus sign (0x2212)
	TestError("?2", 0, ecUnknownChar);
	TestError("x+?", 0, ecUnknownChar);
	TestError("x+sin ¤", 0, ecUnknownChar);
	TestError("x * &", 0, ecUnknownChar);

	//Test square brackets
	Test("2*[x+1]", 5, 12);
	Test("[x]+5", 3, 8);
	Test("log[x]+5", 10, 6);
	Test("log[x+20] * 3", 80, 6);
	Test("log[10] x", 10, 10);

	//Test curly brackets
	Test("2*{x+1}", 5, 12);
	Test("{x}+5", 3, 8);
	Test("log{x}+5", 10, 6);
	Test("log{x+20} * 3", 80, 6);
	Test("log{10} x", 10, 10);

	//Test brackets combined
	Test("2+(x*[5+x*{2-x}])", 10, -748);
	Test("log[log(log{x}10)10]", 10, 1);
	TestError("2+(x*[5+x*{2-x})]", 10, ecNoEndPar);
	TestError("log[log(log{x}10]10)", 10, ecNoEndPar);

  //Test relations
  Test("sin(x^2) < cos(x^2)", 0, 1);

	//Test difficult function
	Test("-sin(x)+(-sin(x)*sqrt((-sin(x))^2+cos(x)^2)-cos(x)*0.5/sqrt((-sin(x))^2+cos(x)^2)*(--2*sin(x)*cos(x)-2*cos(x)*sin(x)))/sqrt((-sin(x))^2+cos(x)^2)^2", 0, 0);

	//Test vertical trend lines
	TDblPoint Vertical[] = {TDblPoint(5,1), TDblPoint(5,7)};
	TestTrendLineError(ttPower, Vertical, 2, 0, ecNoResult);

	//Test difficult trend line
	TDblPoint Points1[] = {TDblPoint(1950,1571), TDblPoint(1970,524), TDblPoint(1980, 208), TDblPoint(2003, 29)};
	std::vector<double> Empty;
	std::vector<TDblPoint> P1(Points1, Points1 + 4);
	TestTrendLine(ttPower, P1, Empty, 0, NaN, L"7.23106321804096256E+498*x^(-150.630652337941856)"); //Fails under Cygwin

  //Test sample trendline
  TDblPoint Points2[] = {TDblPoint(0,0.1), TDblPoint(1,0.9), TDblPoint(2,1.9), TDblPoint(3,2.7), TDblPoint(4,4.7)};
	double W1[] = {0.1, 0.13, 0.16, 0.2, 0.25};
  std::vector<TDblPoint> P2(Points2, Points2 + 5);
	std::vector<double> W(5);
  std::transform(W1, W1+5, W.begin(), ErrorToWeight); //Perform x=1/(x*x)

  TestTrendLine(ttLinear, P2, Empty, 0, NaN, L"1.1*x-0.14");
  TestTrendLine(ttLinear, P2, Empty, 0, 1, L"0.72*x+1"); //Force crossing with y-axis
  TestTrendLine(ttLinear, P2, W, 0, NaN, L"1.01084802043554*x+0.00367555924507695");
  TestTrendLine(ttLinear, P2, W, 0, 1, L"0.615568703919657*x+1"); //Force crossing with y-axis

	TestTrendLine(ttPolynomial, P2, Empty, 2, NaN, L"0.157142857142857*x^2+0.471428571428571*x+0.174285714285714");
  TestTrendLine(ttPolynomial, P2, Empty, 2, 1, L"0.290322580645161*x^2-0.247741935483871*x+1"); //Force crossing
  TestTrendLine(ttPolynomial, P2, W, 2, NaN, L"0.123428984493036*x^2+0.596597125830378*x+0.12279529365147");
	TestTrendLine(ttPolynomial, P2, W, 2, 1, L"0.292651922716118*x^2-0.254573787956674*x+1");

  TestTrendLine(ttPower, P2, Empty, 0, NaN, L"0.872147571158689*x^1.14047602443827");
  TestTrendLine(ttPower, P2, W, 0, NaN, L"0.890072586434138*x^1.10326113209501");

  TestTrendLine(ttExponential, P2, Empty, 0, NaN, L"0.200922305275277*2.41063632810424^x");
  TestTrendLine(ttExponential, P2, Empty, 0, 1, L"1.41191238522143^x");
  TestTrendLine(ttExponential, P2, W, 0, NaN, L"0.14747627125184*2.92012743745353^x");
	TestTrendLine(ttExponential, P2, W, 0, 1, L"1.36647807441143^x");

	TestCustomTrendLine(L"$a*x+$b", P2, Empty, L"1.1*x-0.14");
  TestCustomTrendLine(L"$a*x+1", P2, Empty, L"0.72*x+1");
  TestCustomTrendLine(L"$a*x+$b", P2, W, L"1.01085*x+0.00368");
  TestCustomTrendLine(L"$a*x+1", P2, W, L"0.61557*x+1");

  TestCustomTrendLine(L"$a*x^2+$b*x+$c", P2, Empty, L"0.15714*x^2+0.47143*x+0.17429");
	TestCustomTrendLine(L"$a*x^2+$b*x+1", P2, Empty, L"0.29032*x^2-0.24774*x+1");
  TestCustomTrendLine(L"$a*x^2+$b*x+$c", P2, W, L"0.12343*x^2+0.5966*x+0.1228");
	TestCustomTrendLine(L"$a*x^2+$b*x+1", P2, W, L"0.29265*x^2-0.25457*x+1");

  //Test trendlines with point at x=0
  TCoordBase<double> Points3[] = {{0,20.4},{0.00016,14.6},{0.00036,10.8},{0.00052,8.8},{0.00076,6.4},{0.00102,4.8},{0.00128,3.8},{0.00152,3},{0.00178,2.8},{0.00202,2.2},{0.00226,2},{0.00252,1.6},{0.00278,1.6},{0.00302,1.4},{0.00328,1.4},{0.00352,1.2}};
  std::vector<TDblPoint> P3(Points3, Points3 + 16);
  TestTrendLine(ttLinear, P3, Empty, 0, NaN, L"-4035.68471434*x+12.1847719");
  TestTrendLine(ttPower, P3, Empty, 0, NaN, L"0.00946867*x^-0.88071702");
  TestTrendLine(ttExponential, P3, Empty, 0, NaN, L"12.895232337158638*7.886678185588269E-333^x");
  TestTrendLine(ttPolynomial, P3, Empty, 2, NaN, L"2.475683432006625E+06*x^2-1.257161390319145E+04*x+16.4719035463553");


  //Test differentiation of common operators
  TestDif("-0.5796", "0");
  TestDif("-3x", "-3");
  TestDif("-3x^1", "-3");
  TestDif("-3x^2", "-6x");
  TestDif("-3x^3", "-9x^2");
	TestDif("5x+sin(x)", "5+cos(x)");
  TestDif("5x-sin(x)", "5-cos(x)");
  TestDif("5/x", "-5/x^2");
  TestDif("sin(x)/x", "(cos(x)*x-sin(x))/x^2");
  TestDif("2*sin 30*x", "1", Degree);
  TestDif("x-cos x", "1+sin x");
  TestDif("e^-(sqrt(2)*x)",  "-e^-(sqrt(2)*x)*sqrt(2)");

  //Test differentiation of special functions
  TestDif("dnorm(x)", "-exp(-x^2/2)*x*sqrt(2pi)/2pi");
	TestDif("dnorm(x, 0, 1)", "-exp(-x^2/2)*x*sqrt(2pi)/2pi");
	TestDif("mod(sin(x), 5)", "cos(x)");
	TestDif("mod(4, x)", ecNotDifAble);

  //Test differentiation of inverse trigonometric functions
  TestDif("acot(x)", 1, -28.647889756541160438399077407053, Degree);

  //Differentiation of special power handling
  TestDif("x^(1/3)", "1/3*x^(-2/3)");
  TestDif("2^(x/3)", "1/3*ln(2)*2^(x/3)");
  TestDif("2^(1/x)", "-ln(2)*2^(1/x)/x^2");

	//Differentiation of special functions
	TestDif("sum(k*x, k, 1, 10)", "sum(k, k, 1, 10)");
	TestDif("sum(sum(k*l,k,1,10),l,1,x)", "0");
	TestDif("sum(sum(k*l*x,k,1,10),l,1,2)", "sum(sum(k*l,k,1,10),l,1,2)");
	TestDif("product(k*x, k, 1, 10)", ecNotDifAble);
	TestDif("integrate(k*x, k, 1, 10)", "integrate(k,k,1,10)");
	TestDif("integrate(x, x, 1, 10)", "0");
	TestDif("integrate(x, 1, 10)", "0");
	TestDif("integrate(sin(x), x, 0, x)", "sin(x)");
	TestDif("integrate(sin(x), 0, x)", "sin(x)");
	TestDif("fact(x)", ecNotDifAble);
	TestDif("fact(5)", "0");

	//Differentiation of piecewise functions
	TestDif("min(x,2)", "if(x<2,1,0)");

	//Test difficult function
	TestDif("(cos x)+(cos(x))/sqrt((-sin(x))^2+(cos(x))^2)", "-sin(x)+(-sin(x)*sqrt((-sin(x))^2+cos(x)^2)-cos(x)*0.5/sqrt((-sin(x))^2+cos(x)^2)*(-(-2*sin(x)*cos(x))-2*cos(x)*sin(x)))/sqrt((-sin(x))^2+cos(x)^2)^2");

  //Test the simplify code
	TestSimplify("ln(e)", "1");
	TestSimplify("log(10)", "1");
	TestSimplify("1--sin x", "1+sin(x)");
	TestSimplify("--sin x", "sin(x)");
	TestSimplify("sqrt(x^2)", "x");
	TestSimplify("sqrt(x)^2", "x");
	TestSimplify("(-x)^2", "x^2");
	TestSimplify("-sin(x)+(-sin(x)*sqrt((-sin(x))^2+cos(x)^2)-cos(x)*0.5/sqrt((-sin(x))^2+cos(x)^2)*(--2*sin(x)*cos(x)-2*cos(x)*sin(x)))/sqrt((-sin(x))^2+cos(x)^2)^2",
							 "-sin(x)+(-sin(x)*sqrt(sin(x)^2+cos(x)^2)-0.5*cos(x)/sqrt(sin(x)^2+cos(x)^2)*(2*sin(x)*cos(x)-2*cos(x)*sin(x)))/(sin(x)^2+cos(x)^2)");

  //Test custom functions
  TArgType Args;
  Args.push_back(L"x");
  Args.push_back(L"y");
  TestCustom(L"x*y+1=0", Args, std::vector<TComplex>(2, 0), 0);

  //Test symbol lists
  TSymbolList SymbolList;
  SymbolList.Add(L"TestSqr", boost::shared_ptr<TBaseCustomFunc>(new TTestSqr));
  SymbolList.Add(L"TestCube", boost::shared_ptr<TBaseCustomFunc>(new TTestCube));
  SymbolList.Add(L"k", L"42");
  Args.clear();
  Args.push_back(L"a");
  Args.push_back(L"b");
	Args.push_back(L"c");
	SymbolList.Add(L"foo", L"a^3+b^2+c", Args);
  SymbolList.Add(L"foo_bar", 5);
	Test("k*x", 2, 84, Radian, "", SymbolList);
	Test("TestSqr(x)", 10, 100, Radian, "", SymbolList);
	Test("TestCube(5, 2, x)", 1, 130, Radian, "", SymbolList);
	Test("foo(5, 2, x)", 1, 130, Radian, "", SymbolList);
  Test("foo_bar", 0, 5, Radian, "", SymbolList);

	//Test conversion to text
	TestMakeText(TFunc(L"0"), L"0");
  TestMakeText(TFunc(L"-e^-(sqrt(2)*x)*sqrt(2)"), L"-e^(-sqrt(2)*x)*sqrt(2)");

	//Test several known problems
	TestParamFunc(L"t", L"dsafd");
	TestParamFunc(L"dsafd", L"t");
}
//---------------------------------------------------------------------------
std::wstringstream DebugStreamBuf;
int main()
{
#ifdef __BORLANDC__
	_control87(DEFAULT_FPU_CONTROL, FPU_MASK);
#endif
	std::wclog.rdbuf(DebugStreamBuf.rdbuf()); //Write debug messages to stringstream instead of console
	try
	{
		Test();
		cout << "Test finished" << std::endl;
	}
	catch(...)
	{
		std::cerr << "Unexpected exception!" << std::endl;
	}
	std::cin.ignore();
	return 0;
}
//---------------------------------------------------------------------------

