/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef CommonH
#define CommonH
#include "Func32.h"
#include <complex>
#include <Values.h>
#include <comctrls.hpp>
//---------------------------------------------------------------------------
void CenterForm(TForm *Form);

void AssociateExt(std::wstring Ext, std::wstring ProgramName, std::wstring Ident, std::wstring Description, std::wstring Icon, bool AllUsers);
void RemoveAsociation(const std::wstring &Ext, const std::wstring &Ident, bool RemoveIdent=true);
bool CheckAssocation(const std::wstring &Ext, const std::wstring &Ident);

String GetErrorMsg(const Func32::EFuncError &Error);
String GetErrorMsg(Func32::TErrorCode ErrorCode);
void InvertBitmap(Graphics::TBitmap *Bitmap);
double AdjustUnit(double Number);
void SetGlobalFocus(TWinControl *Control);
std::string EncodeEscapeSequence(const std::string &Str);
std::string DecodeEscapeSequence(const std::string &Str);
void GetLanguageList(TStrings *List);

String GetTempFileName(const String &Prefix, const String &Ext);
String GetTempPath();

inline bool InsideRect(const TRect &Rect, const TPoint &P) {return P.x >= Rect.Left && P.x <= Rect.Right && P.y >= Rect.Top && P.y <= Rect.Bottom;}
std::wstring ReduceString(const std::wstring &Str, unsigned MaxLength);
void LoadLanguage(const String &Lang);
void StartTimer(unsigned Time, TNotifyEvent CallBack);

//Allow comparasion between shared_ptr<> and pointer
template<class T1, class T2>
bool operator==(const boost::shared_ptr<T1> &SharedPtr, const T2 *Ptr)
{
  const T1 *Ptr2 = SharedPtr.get();
  return Ptr2 == Ptr;
}

template<class T1, class T2>
int IndexOf(const std::vector<T1> &Vector, const T2 &Value)
{
  std::vector<T1>::const_iterator iter = std::find(Vector.begin(), Vector.end(), Value);
  return iter == Vector.end() ? -1 : iter - Vector.begin();
}

template<typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
inline InputIterator1 FindInRanges(InputIterator1 First1, InputIterator2 Last1,
                                   InputIterator2 First2, InputIterator2 Last2,
                                   BinaryPredicate Comp = std::equal_to())
{
  while(First1 != Last1 && First2 != Last2)
  {
    if(Comp(*First1, *Last1))
      return First1;
    ++First1;
    ++First2;
  }
  return Last1;
}

//Calculate the perp dot product
template<typename T>
inline double Perp(const T &u, const T &v)
{
  return u.x * v.y - u.y * v.x;
}
//---------------------------------------------------------------------------
//Searches for the place where the range [First;Last[ crosses [p1;p1+1[
//Notice p1+1 must be valid
//Returns an iterator in the range [First;Last[ where crossing occurs
//Returns Last if no crossing found
//Implemented from http://geometryalgorithms.com/Archive/algorithm_0104/algorithm_0104B.htm
template<typename TIterator1, typename TIterator2>
TIterator2 FindCrossing(TIterator1 p1, TIterator2 First, TIterator2 Last)
{
  if(First == Last)
    return Last;
  TIterator1 p2 = p1;
  ++p2;
  Func32::TDblPoint u(p2->x - p1->x, p2->y - p1->y);  //Vector from p1 to p2

  TIterator2 p4 = First;
  ++p4;
  for(TIterator2 p3 = First; p4 != Last; ++p3, ++p4)
  {
    Func32::TDblPoint v(p4->x - p3->x, p4->y - p3->y); //Vector from p3 to p4
    Func32::TDblPoint w(p1->x - p3->x, p1->y - p3->y); //vector from p3 to p1

    double D = Perp(u, v);

    if(std::abs(D) < 0.00000001)
      continue; //The lines are parallel

    double sI = Perp(v, w) / D;
    double tI = Perp(u, w) / D;

    if(sI >= 0 && sI <= 1 && tI >= 0 && tI <= 1)
      return p3;
  }
  return Last;
}
//---------------------------------------------------------------------------
//Search for the place there the two ranges of points [First1;Last1[ and [First2;Last2[
//crosses. Returns Last1 if no cross is found.
//Else the cross is between the returned iterator and the following iterator.
template<typename TIterator1, typename TIterator2>
TIterator1 FindCrossing(TIterator1 First1, TIterator1 Last1, TIterator2 First2, TIterator2 Last2)
{
  if(First1 == Last1 || First2 == Last2)
    return Last1;

  TIterator1 p1; //Allways one behind p2
  TIterator1 p2 = First1;

  while(p1 = p2, ++p2 != Last1)
  {
    if(FindCrossing(p1, First2, Last2) != Last2)
      return p1;
  }
  return Last1;
}
/*
//Used as compare with std::lower_bound() when searching for a t-value
inline bool CompCoordSet1(const Func32::TCoordSet &Set, long double Value)
{
  return Set.t < Value;
}

//Used as compare with std::upper_bound() when searching for a t-value
inline bool CompCoordSet2(long double Value, const Func32::TCoordSet &Set)
{
  return Value < Set.t;
}
*/

class TCompCoordSet
{
public:
  bool operator()(const Func32::TCoordSet<> &Set, long double Value)
  {
    return Set.t < Value;
  }
  bool operator()(long double Value, const Func32::TCoordSet<> &Set)
  {
    return Value < Set.t;
  }
};

inline unsigned MaxDist(const TPoint &P1, const TPoint &P2)
{
  return std::max(abs(P1.x - P2.x), abs(P1.y - P2.y));
}

inline unsigned Sqr(int x)
{
  return x*x;
}

inline unsigned Dist(const TPoint &P1, const TPoint &P2)
{
  return Sqr(P1.x - P2.x) + Sqr(P1.y - P2.y);
}

template<class T>
inline int Sign(T Val){return Val>=0 ? Val ? 1 : 0 : -1;};

template<typename T>
inline T Range(T Min, T Value, T Max) { return std::max(Min, std::min(Value, Max)); }

template<typename T>
inline T IsInRange(const T &Value, const T &Min, const T &Max)
{
  return Value >= Min && Value <= Max;
}

class TWaitCursor
{
  TCursor OldCursor;
public:
  TWaitCursor(TCursor Cursor = crHourGlass) : OldCursor(Screen->Cursor) {Screen->Cursor = Cursor;}
  ~TWaitCursor() {Screen->Cursor = OldCursor;}
};

class TCallOnRelease
{
  struct TCallOnReleaseBase
  {
    virtual ~TCallOnReleaseBase() {}
  };

  template<typename F>
  struct TCallOnReleaseImpl : public TCallOnReleaseBase
  {
    F Func;
    TCallOnReleaseImpl(F AFunc) : Func(AFunc) {}
    ~TCallOnReleaseImpl() {Func();}
  };

  template<typename F, typename T>
  struct TCallOnReleaseImpl1 : public TCallOnReleaseBase
  {
    F Func;
    T Data;
    TCallOnReleaseImpl1(F AFunc, const T &AData) : Func(AFunc), Data(AData) {}
    ~TCallOnReleaseImpl1() {Func(Data);}
  };

  TCallOnReleaseBase *Impl;

public:
  template<typename F>
  TCallOnRelease(F Func)
    : Impl(new TCallOnReleaseImpl<F>(Func)) {}
  template<typename T, typename F>
  TCallOnRelease(F Func, const T &Data)
    : Impl(new TCallOnReleaseImpl1<F, T>(Func, Data)) {}
  ~TCallOnRelease() {delete Impl;}
};

//Function used to create a for and call a function on it before it is destroyed, e.g.:
//CreateForm<TForm5>()->EditFunc();
template<typename T> inline std::auto_ptr<T> CreateForm() {return std::auto_ptr<T>(new T(Application));}
template<typename T, typename TArg> inline std::auto_ptr<T> CreateForm(TArg &Arg) {return std::auto_ptr<T>(new T(Application, Arg));}
template<typename T, typename TArg1, typename TArg2> inline std::auto_ptr<T> CreateForm(TArg1 &Arg1, TArg2 &Arg2) {return std::auto_ptr<T>(new T(Application, Arg1, Arg2));}
template<typename T, typename TArg1, typename TArg2, typename TArg3> inline std::auto_ptr<T> CreateForm(TArg1 &Arg1, TArg2 &Arg2, TArg3 &Arg3) {return std::auto_ptr<T>(new T(Application, Arg1, Arg2, Arg3));}

//Used to show a modal form, e.g.: if(ShowForm<TForm2>() == mrOk)
template<typename T> inline TModalResult ShowForm() {return CreateForm<T>()->ShowModal();}
template<typename T, typename TArg> inline TModalResult ShowForm(TArg &Arg) {return CreateForm<T>(Arg)->ShowModal();}

//Function that should be in gnugettext.pas
inline void TranslateProperties(TObject *Object)
{
  DefaultInstance->TranslateProperties(Object);
}

//The operator<<() in dstring.h doesn't seem to compile
inline std::wostream& operator<<(std::wostream &Stream, const String &Str)
{
  return Stream << Str.c_str();
}

inline std::ostream& operator<<(std::ostream &Stream, const String &Str)
{
  return Stream << AnsiString(Str).c_str();
}

inline std::wostream& operator<<(std::wostream &Stream, TColor Color)
{
  return Stream << ColorToString(Color);
}

std::wistream& operator>>(std::wistream &Stream, TColor &Color);

class TMaxWidth
{
  int Value;
public:
  TMaxWidth(int AValue) : Value(AValue) {}
  TMaxWidth(TControl *Control);
  TMaxWidth operator() (TMaxWidth MaxWidth) const {return TMaxWidth(MaxWidth.Value > Value ? MaxWidth.Value : Value);}
  operator int() const {return Value;}
};

//Same as std::transform except that no result is added to Result if Op throws an exception
template <class TInputIterator, class TOutputIterator, class TUnaryOperation>
TOutputIterator Transform(TInputIterator First, TInputIterator Last,
                          TOutputIterator Result, TUnaryOperation Op)
{
  while(First != Last)
    try
    {
      *Result = Op(*First++);
      ++Result;
    }
    catch(...)
    {
    }
    return Result;
}

//Check if a is very close to zero
//From an article "The G3D Graphics Engine" by Morgan McGuire in CUJ Dec. 2004
inline bool IsZero(long double a)
{
  return a == 0 || std::abs(a) <= 0.0000001L * (std::abs(a) + 1);
}

extern const double NAN;
extern const double INF;
#endif
