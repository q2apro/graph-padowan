/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "Unit1.h"
#include "DrawThread.h"
#include <complex>
#include <cfloat>
#include <values.h>
#include <algorithm>
#include <ctime>
#include <float.h>
#include "PointSelect.h"
#include "IGraphic.h"
#include "StackTrace.h"
#pragma warn -8072 //Disable warning: Suspicous pointer arithmetic
#include <boost/tr1/complex.hpp>

namespace Graph
{
const unsigned MaxPixelDist = 10;
const unsigned MaxExtraPoints = 100;
//---------------------------------------------------------------------------
using std::real;
using std::imag;
//---------------------------------------------------------------------------
template<typename T> inline bool TDrawThread::IsError(Func32::TErrorCode ErrorCode, const Func32::TCoord<T> &Coord)
{
  return ErrorCode != Func32::ecNoError || std::abs(imag(Coord.x)) > MIN_ZERO || std::abs(imag(Coord.y)) > MIN_ZERO ||
    (Data->Axes.xAxis.LogScl && real(Coord.x) <= 0) || (Data->Axes.yAxis.LogScl && real(Coord.y) <= 0);
}
//---------------------------------------------------------------------------
inline bool TDrawThread::InsideImage(const TPoint &P)
{
  return P.x >= AxesRect.Left && P.y >= AxesRect.Top && P.x <= AxesRect.Right && P.y <= AxesRect.Bottom;
}
//---------------------------------------------------------------------------
__fastcall TDrawThread::TDrawThread(TDraw *ADraw)
 : TIThread(false),
   Data(ADraw->Data), AxesRect(ADraw->AxesRect), Aborted(false),
   ForceBlack(ADraw->ForceBlack), SizeMul(ADraw->SizeMul),
   Draw(ADraw), Context(ADraw->Context), Axes(ADraw->Axes)
{
}
//---------------------------------------------------------------------------
void __fastcall TDrawThread::Execute()
{
	randomize();
	SetThreadName("DrawThread");

  while(!Terminated)
  {
    try
    {
      if(Aborted)
        ClearMessageQueue();
      if(!HasMessage())
        IdleEvent.SetEvent();
      Aborted = false;
      TMessage Message;
      GetMessage(Message);
      IdleEvent.ResetEvent();

      //No need to prepare drawings if we are just terminating
      if(Message.Msg == dmTerminate)
        return;

      AxesRect = Draw->AxesRect;
      SizeMul = Draw->SizeMul;

      Synchronize(&BeginUpdate);

      switch(Message.Msg)
      {
        case dmDrawAll:
          DrawAll();
          break;

        case dmDrawFunc:
          reinterpret_cast<TGraphElem*>(Message.WParam)->Accept(*this);
          break;
      }
    }
    catch(Exception &E)
    {
      LogUncaughtException(this, &E);
      Synchronize(&Form1->ShowStatusError, String("Internal error. Log file Graph.err created."));
    }
    catch(std::exception &E)
    {
      LogUncaughtCppException(E.what(), "DrawThread");
      Synchronize(&Form1->ShowStatusError, String("Internal error. Log file Graph.err created."));
    }
    catch(...)
    {
      LogUncaughtCppException(NULL, "DrawThread");
      Synchronize(&Form1->ShowStatusError, String("Internal error. Log file Graph.err created."));
    }
    Synchronize(&EndUpdate);
  }
}
//---------------------------------------------------------------------------
void TDrawThread::DrawAll()
{
  for(unsigned I = 0; I < Data->ElemCount() && !Aborted; I++)
  {
    const boost::shared_ptr<TGraphElem> &Elem = Data->GetElem(I);
    unsigned Count = Elem->ChildCount();
    for(unsigned N = 0; N < Count && !Aborted; N++)
    {
      const TGraphElemPtr &Child = Elem->GetChild(N);
      if(Child->GetVisible())
        Child->Accept(*this);
    }
    if(Elem->GetVisible() && !Aborted)
      Elem->Accept(*this);
  }
}
//---------------------------------------------------------------------------
void TDrawThread::PrepareFunction(TBaseFuncType *F)
{
  if(F->Points.empty())
  {
    //Use axes range for standard functions if the axes range is smaller than the function range
    double Min, Max, ds;
    F->GetCurrentRange(Min, Max, ds);
    if(Min > Max)
      return; //No range

    bool LogScl = Axes.xAxis.LogScl && ds == 0;
    if(ds == 0)
      ds = LogScl ? std::exp(1/Draw->xScale) : 1/Draw->xScale;

    if(ds <= 0)
      ds = 1; //ds must always be a positive number

    //Avoid calculations on an empty range  
    if(!IsZero(Max - Min))
    {
      if(Data->Axes.CalcComplex)
        CalcFunc<Func32::TComplex>(*F, Min, Max, ds, LogScl);
      else
        CalcFunc<long double>(*F, Min, Max, ds, LogScl);
    }
  }
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TBaseFuncType &Func)
{
  PrepareFunction(&Func);

  if(!Aborted)
    Synchronize(&DrawFunction, Func);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawFunction(const TBaseFuncType &Func)
{
//  TContextLock ContextLock(Context);
  Draw->SetClippingRegion();

  if(Func.DrawType == dtDots)
    Draw->DrawPolydots(Func.Points.begin(), Func.Points.end(), Func.Size, Func.Color);
  else
  {
    std::vector<TPoint>::const_iterator iter = Func.Points.begin();
    for(unsigned I = 0; I < Func.PointNum.size(); I++)
    {
      Draw->DrawPolyline(iter, iter + Func.PointNum[I], Func.Style, Func.Size, Func.Color);
      iter += Func.PointNum[I];
    }
  }
  
  DrawEndPoints(Func);
}
//---------------------------------------------------------------------------
//This function draws a mathematical function on the canvas
//Func is the function to draw; sMin and sMax are the range
//ds is the increment between the calculations
template<typename T>
void TDrawThread::CalcFunc(TBaseFuncType &F, double sMin, double sMax, double ds, bool LogScl)
{
  try
  {
    Func32::ECalcError CalcError;
    const Func32::TBaseFunc &Func = F.GetFunc();
    Func32::TCoord<T> Coord = Func.Calc(T(sMin), CalcError); //Calc first x- and y-coordinate
    TPoint Pos;

    //Calculate position in pixel coordinates
    TPoint LastPos;
    if(!CalcError.ErrorCode)
      LastPos = TPoint(xPoint(real(Coord.x)), yPoint(real(Coord.y)));
    unsigned OldSize = 0;
    int Err = 4; //0=no error; 1=calc error; 2=croped; 3=crossing image boundary; 4=start (first point)

    bool Finished = false;
    double sLast = sMin;
    //WARNING: Because of bug in Bcc 5.6.4 we need to put sLast = s inside the ?: operator instead of before
    for(double s = sMin; !Finished && !Aborted; LogScl ? (sLast = s, s*=ds) : (sLast = s, s+=ds))
    {
      //Last point will always be exactly at sMax
      if(s > sMax)
      {
        s = sMax;
        Finished = true;
      }

      try
      {
        Coord = Func.Calc(T(s), CalcError);    //Calc x- and y-coordinate

        Pos.x = xPoint(real(Coord.x));
        Pos.y = yPoint(real(Coord.y));

        //If an error occurred or x or y have an imaginary part
        if(IsError(CalcError.ErrorCode, Coord))
        {
          //If we just went outside defined range
          //Use binary search to find the place between the last defined point and
          //the undefined point where the pixel position no longer changes.
          if(!Err && F.DrawType == dtAuto)
          {
            double s2 = sLast;   //The last defined point
            Pos = LastPos;       //Current pixel position

            do
            {
              double sMiddle = LogScl ? std::sqrt(s*s2) : (s + s2) / 2; // Average between s and s2
              Coord = Func.Calc(T(sMiddle), CalcError);
              Err = IsError(CalcError.ErrorCode, Coord);
              if(Err)
                s = sMiddle;
              else
              {
                s2 = sMiddle;
                LastPos = Pos;
                Pos.x = xPoint(real(Coord.x));
                Pos.y = yPoint(real(Coord.y));
             }
            }
            while((Err || (LastPos != Pos && InsideImage(Pos))) && std::abs(s-s2) > 1E-12);

            //If we have a new position, add it. We do not check for error here.
            //Even if the last calculation was an error, we may have a better position.
            if(Pos != F.Points.back())
            {
              F.Points.push_back(Pos);
              F.sList.push_back(Func32::TCoordSet<>(s2, real(Coord.x), real(Coord.y)));
            }
          }
          Err = true;
          continue;
        }

        //If no error occured: Check if Pos and LastPos are not outside the image on the same side
        if(!Err && F.DrawType == dtAuto)
          if(!((Pos.x < 0 && LastPos.x < 0) || (Pos.x > AxesRect.Right && LastPos.x > AxesRect.Right) || (Pos.y < 0 && LastPos.y < 0) || (Pos.y > AxesRect.Bottom && LastPos.y > AxesRect.Bottom)))
          {
            unsigned Count = MaxExtraPoints; //Use a max loop count to prevent the algorithm from running wild
            //If there is more than 10 pixels between the two points, calculate a point in the middle.
            //We use virtical and horizontal dist to avoid overflow in square calculation
            while(--Count && MaxDist(Pos, LastPos) > MaxPixelDist)
            {
              double sMiddle = LogScl ? std::sqrt(s*sLast) : (s + sLast) / 2; // Average between s and sLast
              Coord = Func.Calc(T(sMiddle), CalcError);
              Err = IsError(CalcError.ErrorCode, Coord);
              if(Err)
                break;

              //Cosider the new point to be part of the line it is closest to
              TPoint P = Draw->xyPoint(Coord);
              if(MaxDist(P, LastPos) < MaxDist(P, Pos))
              {
                if(P == LastPos)
                {
                  Err = 3; //Simulate error at LastPos to make Pos part of a new line
                  break;
                }
                sLast = sMiddle;
                LastPos = P;
                F.Points.push_back(LastPos);
                F.sList.push_back(Func32::TCoordSet<>(sLast, real(Coord.x), real(Coord.y)));
              }
              else
              {
                if(P == Pos)
                {
                  Err = 3; //Simulate error at LastPos to make Pos part of a new line
                  break;
                }
                s = sMiddle;
                Pos = P;
                Finished = false;
              }

              //If both points are outside the image, simulate an error at LastPos to break the line between LastPos and Pos
              if(!InsideRect(AxesRect, LastPos) && !InsideRect(AxesRect, Pos))
              {
                Err = 3; //Simulate error at LastPos to make Pos part of a new line
                break;
              }
            }
          }

        if(Err)             //If an error happened last but not now
        {
          //If points has been added since last calculation
          if(F.Points.size() > OldSize)
            //Add number of points in piece of line
            F.PointNum.push_back(F.Points.size() - OldSize);

          OldSize = F.Points.size();

          //If line was split because of we were out of image area
          if(Err == 2)
          {
             //Add last point
             F.Points.push_back(LastPos);
             Func32::TCoord<T> Coord = Func.Calc(T(s-ds), CalcError);
             F.sList.push_back(Func32::TCoordSet<>(s - ds, real(Coord.x), real(Coord.y)));
          }
          else if(Err == 1 && F.DrawType == dtAuto)   //If calculation error
          {
            //If we just went inside defined range
            //Use binary tree to find the place between the last undefined point and
            //the defined point where the pixel position no longer changes.
            double s1 = sLast;  //The last undefined point
            double s2 = s;      //The first defined point
            TPoint Pos2 = Pos;  //Pixel position at s2
            LastPos = Pos2;
            Func32::TCoord<T> Coord2;

            do
            {
              double sMiddle = LogScl ? std::sqrt(s1 * s2) : (s1 + s2) / 2; //Average between s1 and s2
              Coord2 = Func.Calc(T(sMiddle), CalcError);

              Err = IsError(CalcError.ErrorCode, Coord2);
              if(Err)
                s1 = sMiddle;
              else
              {
                s2 = sMiddle;
                LastPos = Pos2;
                Pos2.x = xPoint(real(Coord2.x));
                Pos2.y = yPoint(real(Coord2.y));
              }
            }
            while((Err || (LastPos != Pos2 && InsideImage(LastPos))) && std::abs(s1-s2) > 1E-12);

            //Add point if we have a new one. Don't check Err. Even if the last calculation
            //was an error, Pos2 might be better than Pos
            if(Pos2 != Pos)
            {
              F.Points.push_back(Pos2);
              F.sList.push_back(Func32::TCoordSet<>(s2, real(Coord2.x), real(Coord2.y)));
            }
          }

          //Add point to vector
          F.Points.push_back(Pos);
          F.sList.push_back(Func32::TCoordSet<>(s, real(Coord.x), real(Coord.y)));
          Err = false;            //Indicate no error
        }
        else //No errors
        {
          //Add point to vector
          if(Pos != LastPos)
          {
            F.Points.push_back(Pos);
            F.sList.push_back(Func32::TCoordSet<>(s, real(Coord.x), real(Coord.y)));
          }
        }
        LastPos = Pos; //Save point
      }
      catch(EOverflow&)         //"e^1000" results in EOverflow when casting from long double to double
      {                         //Note: The exception is not thrown until xPoint(x)
        Err = true;
        continue;
      }
    }
    if(F.Points.size() > OldSize)
      F.PointNum.push_back(F.Points.size() - OldSize);

    //Clear points if aborted
    if(Aborted)
      F.ClearCache();
  }
  catch(Exception &E)
  {
    Synchronize(&Form1->ShowStatusError, "Internal error. Unexpected exception: " + E.Message);
  }
  catch(std::exception &E)
  {
    Synchronize(&Form1->ShowStatusError, "Internal error. Unexpected exception: " + String(E.what()));
  }
  catch(...)
  {
    Synchronize(&Form1->ShowStatusError, String("Internal error. Unknown exception"));
  }
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TShading &Shade)
{
  if(Shade.Region.get() == NULL)
    CreateShade(Shade);
  if(!Aborted)
    Synchronize(&DrawShade, Shade);
}
//---------------------------------------------------------------------------
int ComparePoint(const TPoint &P1, const TPoint &P2, TShadeStyle Style)
{
  switch(Style)
  {
    case ssAbove:
    case ssBelow:
    case ssXAxis:
      return P2.x == P1.x ? 0 : (P2.x > P1.x ? 1 : -1);

    case ssYAxis:
      return P2.y == P1.y ? 0 : (P2.y > P1.y ? 1 : -1);

    default:
      return 0;
  }
}
//---------------------------------------------------------------------------
TPoint TDrawThread::GetFixedPoint(const TShading &Shade, const TPoint &P)
{
  switch(Shade.ShadeStyle)
  {
    case ssAbove:
      return TPoint(P.x, AxesRect.Top - 1);
    case ssBelow:
      return TPoint(P.x, AxesRect.Bottom + 1);
    case ssXAxis:
      return TPoint(P.x, Draw->yPixelCross);
    case ssYAxis:
      return TPoint(Draw->xPixelCross, P.y);
    default:
      return P;
  }
}
//---------------------------------------------------------------------------
void TDrawThread::CreateShade(TShading &Shade)
{
  TBaseFuncType *F = dynamic_cast<TBaseFuncType*>(Shade.GetParent().get());

  PrepareFunction(F);
  if(Shade.Func2)
    PrepareFunction(Shade.Func2.get());

	if(F->sList.empty() || (Shade.Func2.get() && Shade.Func2->sList.empty()))
    return;

  //The intervals can go both ways. Make sure sMin is always less than sMax
  double sMin = std::min(Shade.sMin.Value, Shade.sMax.Value);
  double sMax = std::max(Shade.sMin.Value, Shade.sMax.Value);
  double Min, Max;

  //If shade is outside the function area, nothing to draw
  if(sMax < F->sList.front().t || sMin > F->sList.back().t)
    return;

  if(dynamic_cast<const TStdFunc*>(F))
  {
    //If Min or Max is outside coordinate system: Make sure they are just one pixel outside
    Min = Range(std::max(Draw->xCoord(-1), F->From.Value), sMin, Draw->xCoord(AxesRect.Right+1));
    Max = Range(Draw->xCoord(-1), sMax, std::min(Draw->xCoord(AxesRect.Right+1), F->To.Value));
  }
  else
  {
    Min = std::_finite(sMin) ? sMin : F->From.Value;
    Max = std::_finite(sMax) ? sMax : F->To.Value;
  }

  //Find starting and ending points in point vector
  unsigned N1 = std::lower_bound(F->sList.begin(), F->sList.end() - 1, Min, TCompCoordSet()) - F->sList.begin();
  unsigned N2 = std::upper_bound(F->sList.begin() + N1, F->sList.end(), Max, TCompCoordSet()) - F->sList.begin() - 1;

	if(N2 <= N1)
		return; //This may happen because we subtracted 1 when we calculated N2

  //Position of x-axis on the y-axis
  int yAxisPixel = Range<int>(AxesRect.Top, yPoint(Axes.xAxis.AxisCross), AxesRect.Bottom);

  //Position of y-axis on the x-axis
  int xAxisPixel = Range<int>(AxesRect.Left, xPoint(Axes.yAxis.AxisCross), AxesRect.Right);

  if(Shade.ExtendMinToIntercept)
  {
    switch(Shade.ShadeStyle)
    {
      case ssAbove:
      case ssBelow:
      case ssXAxis:
        int CrossLine;
        if(Shade.ShadeStyle == ssAbove)
          CrossLine = 0;
        else if(Shade.ShadeStyle == ssBelow)
          CrossLine = AxesRect.Bottom;
        else
          CrossLine = yAxisPixel;

        //Decrease N1 until F->Points[N1] intercepts with the x-axis, top or bottom of image
        bool Above;
        Above = F->Points[N1].y < CrossLine;
        while(N1 && (Above ? F->Points[N1].y < CrossLine : F->Points[N1].y > CrossLine))
          --N1;
        break;
      case ssYAxis:
        //Decrease N1 until F->Points[N1] intercepts with the y-axis
        bool Left;
        Left = F->Points[N1].x < xAxisPixel;
        while(N1 && (Left ? F->Points[N1].x < xAxisPixel : F->Points[N1].x > xAxisPixel))
          --N1;
        break;
      case ssBetween:
        N1 = F->Points.rend() - FindCrossing(F->Points.rend() - (N1+1), F->Points.rend(), Shade.Func2->Points.begin(), Shade.Func2->Points.end());
        if(N1)
          N1--;
        break;
      case ssInside:
        N1 = F->Points.rend() - FindCrossing(F->Points.rend() - (N1+1), F->Points.rend(), F->Points.begin() + N2, F->Points.end());
        if(N1)
          N1--;
    }
    Min = F->sList[N1].t;
  }

  if(Shade.ExtendMaxToIntercept)
  {
    switch(Shade.ShadeStyle)
    {
      case ssAbove:
      case ssBelow:
      case ssXAxis:
        int CrossLine;
        if(Shade.ShadeStyle == ssAbove)
          CrossLine = 0;
        else if(Shade.ShadeStyle == ssBelow)
          CrossLine = AxesRect.Bottom;
        else
          CrossLine = yAxisPixel;

        //Increase N2 until F->Points[N2] intercepts with the x-axis, top or bottom of image
        bool Above;
        Above = F->Points[N2].y < CrossLine;
        while(N2 < F->Points.size() && (Above ? F->Points[N2].y < CrossLine : F->Points[N2].y > CrossLine))
          ++N2;
        break;

      case ssYAxis:
        //Increase N2 until F->Points[N2] intercepts with the y-axis
        bool Left;
        Left = F->Points[N2].x < xAxisPixel;
        while(N2 < F->Points.size() && (Left ? F->Points[N2].x < xAxisPixel : F->Points[N2].x > xAxisPixel))
          ++N2;
        break;

      case ssBetween:
        N2 = FindCrossing(F->Points.begin() + N2, F->Points.end(), Shade.Func2->Points.begin(), Shade.Func2->Points.end()) - F->Points.begin();
        break;

      case ssInside:
        N2 = FindCrossing(F->Points.begin() + N2, F->Points.end(), F->Points.rend() - N1, F->Points.rend()) - F->Points.begin();
    }
    if(N2)
      N2--;
    Max = F->sList[N2].t;
  }

  //Find start and end points of function; May not have been drawn, and may not be valid
  int x1, y1, x2, y2;
  try
  {
    x1 = xPoint(F->GetFunc().CalcX(Min));
    y1 = yPoint(F->GetFunc().CalcY(Min));
  }
  catch(...)
  {
    x1 = F->Points[N1].x;
    y1 = F->Points[N1].y;
  }

  try
  {
    x2 = xPoint(F->GetFunc().CalcX(Max));
    y2 = yPoint(F->GetFunc().CalcY(Max));
  }
  catch(...)
  {
    x2 = F->Points[N2].x;
    y2 = F->Points[N2].y;
  }

  if(x1 == AxesRect.Left)
    x1--;
  if(x2 == AxesRect.Right)
    x2++;
  if(y1 == AxesRect.Bottom)
    y1++;
  if(y2 == AxesRect.Top)
    y2--;

  boost::shared_ptr<TRegion> Region(new TRegion(TRect(0,0,0,0)));
  unsigned CountIndex = 0;
  unsigned Sum = 0;
  while(Sum <= N1)
    Sum += F->PointNum[CountIndex++];

  switch(Shade.ShadeStyle)
  {
    case ssAbove:
    case ssBelow:
    case ssXAxis:
    case ssYAxis:
    {
      std::vector<TPoint> Points;
      //Set start point
      Points.push_back(GetFixedPoint(Shade, TPoint(x1, y1)));
      Points.push_back(TPoint(x1, y1));
      int y = F->Points[N1].y;
      unsigned I = N1+1;
      bool Pos = ComparePoint(F->Points[N1], F->Points[I], Shade.ShadeStyle) >= 0;
      while(I < N2+1)
      {
        while(I < N2+1 && I < Sum)
        {
          int Pos2 = ComparePoint(F->Points[I-1], F->Points[I], Shade.ShadeStyle);
          if(Pos ? Pos2 < 0 : Pos2 > 0)
            break;
          I++;
        }
        if(I == Sum)
        {
          if(CountIndex < F->PointNum.size())
            Sum += F->PointNum[CountIndex++];
        }
        else
          Pos = !Pos;
        Points.insert(Points.end(), F->Points.begin()+N1, F->Points.begin()+I);
        if(I < N2+1)
        {
          Points.push_back(GetFixedPoint(Shade, Points.back()));
          *Region |= TRegion(Points);
          Points.clear();
          N1 = I;
          Points.push_back(GetFixedPoint(Shade, F->Points[I]));
        }
      }
      Points.push_back(TPoint(x2, y2));
      Points.push_back(GetFixedPoint(Shade, TPoint(x2, y2)));
      *Region |= TRegion(Points);
      Region->Widen(0, 0, 1, 1);
      Points.clear();
      break;
    }

    case ssInside:
      if(N1 != N2)
        *Region |= TRegion(&F->Points[N1], N2-N1);
      break;

    case ssBetween:
    {
      std::vector<TPoint> Points;
      Points.push_back(TPoint(x1, y1));
      Points.insert(Points.end(), F->Points.begin()+N1, F->Points.begin()+N2+1);
      Points.push_back(TPoint(x2, y2));

      //The intervals can go both ways. Make sure sMin is always less than sMax
      double sMin2 = std::min(Shade.sMin2.Value, Shade.sMax2.Value);
      double sMax2 = std::max(Shade.sMin2.Value, Shade.sMax2.Value);

      double Min2, Max2;

      if(dynamic_cast<const TStdFunc*>(Shade.Func2.get()))
      {
        Min2 = Range(Draw->xCoord(-1), sMin2, Draw->xCoord(AxesRect.Right+1));
        Max2 = Range(Draw->xCoord(-1), sMax2, Draw->xCoord(AxesRect.Right+1));
      }
      else
      {
        Min2 = std::_finite(sMin2) ? sMin2 : Shade.Func2->From.Value;
        Max2 = std::_finite(sMax2) ? sMax2 : Shade.Func2->To.Value;
      }

      //Find starting and ending points in point vector
      //If list is empty: M1=-1, M2=-1
      int M1 = std::lower_bound(Shade.Func2->sList.begin(), Shade.Func2->sList.end() - 1, Min2, TCompCoordSet()) - Shade.Func2->sList.begin();
      int M2 = std::upper_bound(Shade.Func2->sList.begin()+M1, Shade.Func2->sList.end(), Max2, TCompCoordSet()) - Shade.Func2->sList.begin() - 1;

      //Find start and end points of function; May not have been drawn, and may not be valid
      int X1, Y1, X2, Y2;
      try
      {
        X1 = xPoint(Shade.Func2->GetFunc().CalcX(Min2));
        Y1 = yPoint(Shade.Func2->GetFunc().CalcY(Min2));
      }
      catch(...)
      {
        X1 = Shade.Func2->Points[M1].x;
        Y1 = Shade.Func2->Points[M1].y;
      }

      try
      {
        X2 = xPoint(Shade.Func2->GetFunc().CalcX(Max2));
        Y2 = yPoint(Shade.Func2->GetFunc().CalcY(Max2));
      }
      catch(...)
      {
        X2 = Shade.Func2->Points[M2].x;
        Y2 = Shade.Func2->Points[M2].y;
      }

      //Check if Max is actually less than min for one of the functions
      bool SwapMinMax = (Shade.sMin.Value < Shade.sMax.Value) != (Shade.sMin2.Value < Shade.sMax2.Value);

      if(X1 == AxesRect.Left)
        X1--;
      if(X2 == AxesRect.Right)
        X2++;
      if(Y1 == AxesRect.Bottom)
        Y1++;
      if(Y2 == AxesRect.Top)
        Y2--;

      //Add start point to the polygon points list
      if(!Shade.ExtendMax2ToIntercept)
        Points.push_back(SwapMinMax ? TPoint(X1, Y1) : TPoint(X2, Y2));

      if(Shade.ExtendMin2ToIntercept)
      {
        M1 = Shade.Func2->Points.rend() -
          FindCrossing(Shade.Func2->Points.rend() - (M1+1), Shade.Func2->Points.rend(), F->Points.begin(), F->Points.end());
        if(M1)
          M1--;
      }

      if(Shade.ExtendMax2ToIntercept)
      {
        M2 = FindCrossing(Shade.Func2->Points.begin() + M2, Shade.Func2->Points.end(), F->Points.begin(), F->Points.end()) - Shade.Func2->Points.begin();
        if(M2)
          M2--;
      }

      //Add points on the second function to the polygon points list
      if(M2 > M1)
      {
        if(SwapMinMax)
          Points.insert(Points.end(), Shade.Func2->Points.begin()+M1, Shade.Func2->Points.begin()+M2);
        else
          Points.insert(Points.end(), Shade.Func2->Points.rend()-M2-1, Shade.Func2->Points.rend()-M1);
      }

      //Add end point to the polygon points list
      //Add start point to the polygon points list
      if(!Shade.ExtendMin2ToIntercept)
        Points.push_back(SwapMinMax ? TPoint(X2, Y2) : TPoint(X1, Y1));
      *Region |= TRegion(Points);
      break;
    }
  }

  Shade.Region = Region;
}
//---------------------------------------------------------------------------
void TDrawThread::DrawShade(const TShading &Shade)
{
	if(!Shade.Region)
	  return;
  Draw->SetClippingRegion();

  Context.SetBrush(Shade.BrushStyle, ForceBlack ? clBlack : Shade.Color);

  //Draw marked area, and clip at AxesRect
  Context.DrawRegion(*Shade.Region);

  Context.SetBrush(bsSolid, ForceBlack ? clBlack : Shade.Color);
  if(Shade.MarkBorder)
    Context.DrawFrameRegion(*Shade.Region, 1);
}
//---------------------------------------------------------------------------
void __fastcall TDrawThread::BeginUpdate()
{
  Form1->BeginUpdate();
  Form1->ShowStatusMessage(LoadRes(RES_UPDATE));
}
//---------------------------------------------------------------------------
//Must be synchronized
void __fastcall TDrawThread::EndUpdate()
{
  try
  {
    Form1->EndUpdate();
    Form1->ShowStatusMessage("");
    Form1->UpdateEval();
    if(Draw->OnComplete)
      Draw->OnComplete();
  }
  catch(Exception &E)
  {
    Form1->ShowStatusError("Internal error. Unexpected exception: " + E.Message);
  }
}
//---------------------------------------------------------------------------
void TDrawThread::DrawEndPoints(const TBaseFuncType &Func)
{
  if(Func.StartPointStyle != 0)
  {
    long double t = Func.From.Value;
    if(_finitel(t))
      DrawEndPoint(Func, t, Func.StartPointStyle, true);
    else
    {
      //If a start value has not been specified, search for the first point inside the visible area
      //that is close to the border, and show the end point here.
      for(unsigned I = 0; I < Func.sList.size(); I++)
        if(InsideRect(AxesRect, Func.Points[I]))
        {
          if(!IsInRange(Func.Points[I].x, AxesRect.Left + 5, AxesRect.Right - 5) ||
             !IsInRange(Func.Points[I].y, AxesRect.Top + 5, AxesRect.Bottom - 5))
          {
            t = Func.sList[I].t;
            TPoint Pos = Func.Points[I];
            if(I > 0)
              //This only works because we know the function is a standard function
              Pos = TContext::ClipLine(Func.Points[I-1], Func.Points[I], AxesRect);
            DrawEndPoint(Func, t, Pos, Func.StartPointStyle, true);
          }
          break;
        }
    }
  }

  if(Func.EndPointStyle != 0)
  {
    long double t = Func.To.Value;
    if(_finitel(t))
      DrawEndPoint(Func, t, Func.EndPointStyle, false);
    else if(!Func.sList.empty())
    {
      //If a start value has not been specified, search for the first point inside the visible area
      //that is close to the border, and show the end point here.
      for(unsigned I = Func.sList.size()-1; I > 0; I--)
        if(InsideRect(AxesRect, Func.Points[I]))
        {
          if(!IsInRange(Func.Points[I].x, AxesRect.Left + 5, AxesRect.Right - 5) ||
             !IsInRange(Func.Points[I].y, AxesRect.Top + 5, AxesRect.Bottom - 5))
          {
            t = Func.sList[I].t;
            TPoint Pos = Func.Points[I];
            if(I < Func.sList.size() - 1)
              //This only works because we know the function is a standard function
              Pos = TContext::ClipLine(Func.Points[I+1], Func.Points[I], AxesRect);
            DrawEndPoint(Func, t, Pos, Func.EndPointStyle, false);
          }
          break;
        }
    }
  }
}
//---------------------------------------------------------------------------
void TDrawThread::DrawEndPoint(const TBaseFuncType &Func, long double t, const TPoint &Pos, unsigned Style, bool InvertArrow)
{
  if(_finitel(t))
    try
    {
      Func32::TCoord<long double> Coord = Func.GetFunc().Calc(t);
      if(IsError(Func32::ecNoError, Coord))
        return; //Invalid coordinate because of log axes

      switch(Style)
      {
        case 1:
          TPointSelect::DrawPoint(Context.GetCanvas(), Pos, 0, Func.Color, Axes.BackgroundColor, SizeScale(Func.Size + 3));
          break;

        case 2:
          TPointSelect::DrawPoint(Context.GetCanvas(), Pos, 0, Func.Color, Func.Color, SizeScale(Func.Size + 3));
          break;

        case 3:
          if(InvertArrow)
						DrawArrow(Pos, Func.GetFunc().CalcAngleSlope(t) + M_PI, Func.Color, Func.Size);
					else
            DrawArrow(Pos, Func.GetFunc().CalcAngleSlope(t), Func.Color, Func.Size);
          break;

        case 4:
          DrawHalfCircle(Pos, Func.GetFunc().CalcAngleSlope(t), Func.Color, Func.Size);
          break;

        case 5:
          DrawHalfCircle(Pos, Func.GetFunc().CalcAngleSlope(t) + M_PI, Func.Color, Func.Size);
          break;

        case 6:
          DrawSquareBracket(Pos, Func.GetFunc().CalcAngleSlope(t), Func.Color, Func.Size);
          break;

        case 7:
          DrawSquareBracket(Pos, Func.GetFunc().CalcAngleSlope(t) + M_PI, Func.Color, Func.Size);
          break;
      }
    }
    catch(Func32::EFuncError&)
    {
    }
}
//---------------------------------------------------------------------------
void TDrawThread::DrawEndPoint(const TBaseFuncType &Func, long double t, unsigned Style, bool InvertArrow)
{
  if(_finitel(t))
    try
    {
      Func32::TCoord<long double> Coord = Func.GetFunc().Calc(t);
      if(IsError(Func32::ecNoError, Coord))
        return; //Invalid coordinate because of log axes

      DrawEndPoint(Func, t, Draw->xyPoint(Coord), Style, InvertArrow);
    }
    catch(Func32::EFuncError&)
    {
    }
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TTextLabel &Label)
{
  Synchronize(&DrawLabel, Label);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawLabel(TTextLabel &Label)
{
  if(_isnan(Label.GetXPos().Value) || _isnan(Label.GetYPos().Value))
    return;

  TPoint Pos;
  switch(Label.GetPlacement())
  {
    case lpAboveX:
      Pos = TPoint(AxesRect.Right - Label.GetRect().Width() + 2, Draw->yPixelCross - Label.GetRect().Height() - 4);
      break;

    case lpBelowX:
      Pos = TPoint(AxesRect.Right - Label.GetRect().Width() + 2, Draw->yPixelCross);
      break;

    case lpRightOfY:
      Pos = TPoint(Draw->xPixelCross + 12, AxesRect.Top + 1);
      break;

    case lpLeftOfY:
      Pos = TPoint(Draw->xPixelCross - Label.GetRect().Width(), AxesRect.Top + 1);
      break;

    case lpUserTopLeft:
    default:
      Pos = Draw->xyPoint(Label.GetXPos().Value, Label.GetYPos().Value);
      break;

    case lpUserTopRight:
      Pos = Draw->xyPoint(Label.GetXPos().Value, Label.GetYPos().Value);
      Pos.x -= Label.GetRect().Width();
      break;

    case lpUserBottomLeft:
      Pos = Draw->xyPoint(Label.GetXPos().Value, Label.GetYPos().Value);
      Pos.y -= Label.GetRect().Height();
      break;

    case lpUserBottomRight:
      Pos = Draw->xyPoint(Label.GetXPos().Value, Label.GetYPos().Value);
      Pos.x -= Label.GetRect().Width();
      Pos.y -= Label.GetRect().Height();
      break;
  }

  Label.UpdateRect(Pos.x, Pos.y);

  //Check for valid coordinate; The label may have a negative position on a log axis
  if(Pos.x != MAXINT && Pos.y != MAXINT)
  {
    Draw->SetClippingRegion();
    Context.GetCanvas()->Draw(Pos.x, Pos.y, Label.GetImage());
  }
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TPointSeries &PointSeries)
{
  Synchronize(&DrawPointSeries, PointSeries);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawPointSeries(const TPointSeries &PointSeries)
{
  Draw->SetClippingRegion();

  std::vector<TPoint> PointArray;
  const std::vector<TPointSeriesPoint> &PointData = PointSeries.GetPointData();
  const TPointSeries::TPointList PointList = PointSeries.GetPointList();
  TPointSeries::TPointList::const_iterator End = PointList.end();
	for(TPointSeries::TPointList::const_iterator Iter = PointList.begin(); Iter != End; ++Iter)
		//Check that point is valid (NAN is used to indicate invalid value)
		if((!Axes.xAxis.LogScl || Iter->x > 0) && (!Axes.yAxis.LogScl || Iter->y > 0) && _finite(Iter->x) && _finite(Iter->y))
		{
			TPoint P = xyPoint(*Iter);
			if(PointArray.empty() || P != PointArray.back())
				PointArray.push_back(P);
		}

  if(PointSeries.GetLineStyle() != psClear && PointSeries.GetLineSize() > 0)
  {
    std::vector<TPoint> SplinePoints;
    Interpolate(SplinePoints, PointArray, PointSeries.GetInterpolation());
    Draw->DrawPolyline(SplinePoints.begin(), SplinePoints.end(), PointSeries.GetLineStyle(), PointSeries.GetLineSize(), PointSeries.GetLineColor());
  }

  if(PointSeries.GetShowLabels())
  {
    Context.SetFont(PointSeries.GetFont());
    if(ForceBlack)
      Context.SetFontColor(clBlack);
  }

  //Only draw for point sizes >0
  if(PointSeries.GetSize() == 0)
    return;

  std::wstring FormatStr;
  if(PointSeries.GetPointType() == ptPolar)
    FormatStr = Axes.Trigonometry == Func32::Degree ? GuiFormatSettings.DegreePointFormat : GuiFormatSettings.RadianPointFormat;
  else
    FormatStr = GuiFormatSettings.CartesianPointFormat;
  boost::wformat Format(FormatStr);

  //First draw all error bars and labels
  if(PointSeries.GetxErrorBarType() != ebtNone || PointSeries.GetyErrorBarType() != ebtNone ||  PointSeries.GetShowLabels())
    for(TPointSeries::TPointList::const_iterator Iter = PointList.begin(); Iter != End; ++Iter)
    {
      double x = Iter->x;
      double y = Iter->y;
      if(!_finite(x) || !_finite(y))
        continue;

      //Check that point is valid (NAN is used to indicate invalid value)
      if((!Axes.xAxis.LogScl || x > 0) && (!Axes.yAxis.LogScl || y > 0) )
      {
        TPoint Pos = xyPoint(*Iter);
        if(!InsideRect(AxesRect, Pos)) //We might get rounding errors if the point is too much outside the visible area
          continue;

        //Draw x error bar
        Context.SetPen(psSolid, clBlack, SizeScale(1));
        unsigned I = Iter - PointList.begin();
        double Delta = PointSeries.GetXError(I);
        if(Delta)
        {
          int X1 = xPoint(x - Delta);
          int X2 = xPoint(x + Delta);
          Context.DrawLine(X1, Pos.y, X2, Pos.y);
          Context.DrawLine(X1, Pos.y - 4, X1, Pos.y +5);
          Context.DrawLine(X2, Pos.y - 4, X2, Pos.y +5);
        }

        //Draw y error bar
        Delta = PointSeries.GetYError(I);
        if(Delta)
        {
          int Y1 = yPoint(y - Delta);
          int Y2 = yPoint(y + Delta);
          Context.DrawLine(Pos.x, Y1, Pos.x, Y2);
          Context.DrawLine(Pos.x - 4, Y1, Pos.x +5, Y1);
          Context.DrawLine(Pos.x - 4, Y2, Pos.x +5, Y2);
        }

        if(PointSeries.GetShowLabels())
        {
          std::wstring Str = str(Format % PointData[I].First % PointData[I].Second);
          int PointSize = SizeScale(PointSeries.GetSize());
          TDraw::DrawPointLabel(Context.GetCanvas(), Pos, PointSize, Str, PointSeries.GetLabelPosition());
        }
      }
    }

  //Remove clipping region and draw all markers.
  //This is done so you either is all the marker or none of it. InsideRect checks if it should be shown.
  Context.DestroyClipRect();
  for(TPointSeries::TPointList::const_iterator Iter = PointList.begin(); Iter != End; ++Iter)
  {
    //Check that point is valid (NAN is used to indicate invalid value)
    if((!Axes.xAxis.LogScl || Iter->x > 0) && (!Axes.yAxis.LogScl || Iter->y > 0) && _finite(Iter->x) && _finite(Iter->y))
    {
      TPoint Pos = xyPoint(*Iter);
      if(!InsideRect(AxesRect, Pos)) //We might get rounding errors if the point is too much outside the visible area
        continue;

      //Check if we are inside the legend
      if(Axes.ShowLegend && InsideRect(Draw->LegendRect, Pos))
        continue;

      int PointSize = SizeScale(PointSeries.GetSize());
      TColor FillColor = ForceBlack ? clWhite : PointSeries.GetFillColor();
      TColor FrameColor = ForceBlack ? clBlack : PointSeries.GetFrameColor();

      if(PointSeries.GetStyle() != 7)
        TPointSelect::DrawPoint(Context.GetCanvas(), Pos, PointSeries.GetStyle(), FrameColor, FillColor, PointSize); //Draw normalt markers
      else if(Iter != PointList.begin()) //Draw arrow
      {
        Context.SetPen(PointSeries.GetLineStyle() == psClear ? psSolid : PointSeries.GetLineStyle(), PointSeries.GetFillColor(), PointSize);
        unsigned I = Iter - PointList.begin();
        double Angle = InterpolationAngle(PointList[std::max((int)I-2, 0)], PointList[I-1],
          PointList[I], PointList[std::min(I+1, PointList.size()-1)], PointSeries.GetInterpolation());

        DrawArrow(Pos, Angle, PointSeries.GetFillColor(), PointSeries.GetSize());
      }
    }
  }
}
//---------------------------------------------------------------------------
void TDrawThread::DrawArrow(const TPoint &P, long double Angle, TColor Color, unsigned Size)
{
  Context.SetPen(psSolid, Color, SizeScale(Size/2+1), psjMiter);
  Context.SetBrush(bsSolid, Color);

  long double Angle2 = RealToVisualAngle(Angle);
  int Length = SizeScale(13 + 2*Size);
  int dX1 = Length * std::cos(Angle2 + 2.8);
  int dY1 = Length * std::sin(Angle2 + 2.8);
  int dX2 = Length * std::cos(Angle2 - 2.8);
  int dY2 = Length * std::sin(Angle2 - 2.8);
  TPoint Arrow[] = {TPoint(P.x + dX1, P.y - dY1), P, TPoint(P.x + dX2, P.y - dY2)};
  Context.DrawPolygon(Arrow, 3);
}
//---------------------------------------------------------------------------
void TDrawThread::LineToAngle(int X, int Y, double Angle, double Length)
{
  int dX = Length * std::cos(Angle);
  int dY = Length * std::sin(Angle);

  Context.DrawLine(X, Y, X + dX, Y - dY);
}
//---------------------------------------------------------------------------
long double TDrawThread::RealToVisualAngle(long double Angle)
{
  //Adjust angle for visual coordinate system instead of real system
  long double Sin = std::sin(Angle) * Draw->yScale;
  long double Cos = std::cos(Angle) * Draw->xScale;
  return std::atan2(Sin, Cos);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawHalfCircle(const TPoint &Point, long double Angle, TColor Color, unsigned Size)
{
  Context.SetPen(psSolid, Color, SizeScale(Size));
  long double Angle2 = RealToVisualAngle(Angle);

  int r = SizeScale(3 + Size);
  int a = r * std::sin(Angle2);
  int b = r * std::cos(Angle2);
  TPoint C(Point.x - b, Point.y + a);

  Context.DrawArc(C.x - r, C.y - r, C.x + r, C.y + r, C.x + a, C.y + b, C.x - a, C.y - b);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawSquareBracket(const TPoint &P, long double Angle, TColor Color, unsigned Size)
{
  Context.SetPen(psSolid, Color, SizeScale(Size), psjMiter);
  long double Angle2 = RealToVisualAngle(Angle);
  double Length = SizeScale(3 + Size);
  double a = Length * std::cos(Angle2);
  double b = Length * std::sin(Angle2);
  TPoint Data[] = {TPoint(P.x - b - a + 0.5, P.y - a + b + 0.5), TPoint(P.x - b + 0.5, P.y - a + 0.5),
                   TPoint(P.x + b + 0.5, P.y + a + 0.5), TPoint(P.x + b - a + 0.5, P.y + a + b + 0.5)};
  Context.DrawPolyline(Data, 4);
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TAxesView &AxesView)
{
  Synchronize(&Draw->DrawAxes);
}
//---------------------------------------------------------------------------
void TDrawThread::CreateInequality(TRelation &Relation)
{
  bool xLogScl = Axes.xAxis.LogScl;
  bool yLogScl = Axes.yAxis.LogScl;
  int dX = (Draw->Width > 1200) ? (Draw->Width > 2400 ? Draw->Width / 120 : 2 ) : 1;
  std::exp(1.0); //Workaround for stupid bug in bcc 5.6.4
  double dx = xLogScl ? std::exp(dX/Draw->xScale) : dX/Draw->xScale;
  double dx2 = xLogScl ? std::exp(1/Draw->xScale) : 1/Draw->xScale;
  double dy = yLogScl ? std::exp(-1/Draw->yScale) : -1/Draw->yScale;
  std::exp(1.0); //Workaround for stupid bug in bcc 5.6.4

  std::vector<TRect> Points;
  Points.reserve(500);
  int XStart;
  std::vector<long double> Args(2);
  Func32::ECalcError CalcError;

  double y = Axes.yAxis.Max;
  for(int Y = AxesRect.Top; Y < AxesRect.Bottom + 1; Y++)
  {
    Args[1] = y;
    bool LastResult = false;
    double x;
    x = xLogScl ? Axes.xAxis.Min / std::sqrt(dx2) : Axes.xAxis.Min - dx2/2;
    for(int X = AxesRect.Left - 1; X < AxesRect.Right + dX; X += dX)
    {
      Args[0] = x;
      long double Temp = Relation.Eval(Args, CalcError);
      bool Result = !_isnanl(Temp) && Temp != 0;
      if(Result != LastResult)
      {
        double x2 = xLogScl ? x / dx * dx2 : x - dx + dx2;
        for(int X2 = X - dX + 1; X2 <= X; X2++)
        {
          Args[0] = x2;
          Temp = Relation.Eval(Args, CalcError);
          Result = !_isnanl(Temp) && Temp != 0;
          if(Result != LastResult)
          {
            if(Result)
              XStart = X2;
            else
              Points.push_back(TRect(
                XStart <= AxesRect.Left ? -100 : XStart + 1,
                Y <= AxesRect.Top ? -100 : Y,
                X2,
                Y >= AxesRect.Bottom ? AxesRect.Bottom + 100 : Y + 1));
            break;
          }
          xLogScl ? x2 *= dx2 : x2 += dx2; //Don't place inside for() because of bug in CB2009
        }
      }
      LastResult = Result;
      xLogScl ? x *= dx : x += dx;
    }

    if(Aborted)
      return;
    if(LastResult)
      Points.push_back(TRect(
        XStart <= AxesRect.Left ? -100 : XStart + 1,
        Y <= AxesRect.Top ? -100 : Y,
        AxesRect.Right + 100,
        Y >= AxesRect.Bottom ? AxesRect.Bottom + 100 : Y + 1));
    yLogScl ? y *= dy : y += dy;
  }

  Relation.Region.reset(new TRegion(Points));

  if(Relation.GetBrushStyle() != bsSolid && Relation.GetSize() > 0)
  {
    //Create bounding region as a region Delta pixels large than the real region, and subtract the real region
    //to get the bounding region
    const std::vector<TRect>::iterator End = Points.end();
    for(std::vector<TRect>::iterator Iter = Points.begin(); Iter != End; ++Iter)
    {
      unsigned Delta = SizeScale(Relation.GetSize());
      Iter->Left -= Delta;
      Iter->Right += Delta;
      Iter->Top -= Delta;
      Iter->Bottom += Delta;
    }

    Relation.BoundingRegion.reset(new TRegion(Points));
    *Relation.BoundingRegion -= *Relation.Region;
  }
  else
    Relation.BoundingRegion.reset(); //Don't draw frame when brush style is bsSolid
}
//---------------------------------------------------------------------------
//Check if there is a possibility for a zero point in f(x,y).
//It may not be certain
bool TDrawThread::CheckResult1(const long double Result[3])
{
  //If the sign has changed, at least one of the current and last result is valid, or
  //the increase/decrease state has changed, which might indicate an asymptote.
  return (Result[1] < 0) == (Result[2] > 0) && !(_isnanl(Result[1]) && _isnanl(Result[2])) ||
    (Result[0] < Result[1]) != (Result[1] < Result[2]);
  if((Result[0] < Result[1]) != (Result[1] < Result[2]))
    return true;
  return false;
}
//---------------------------------------------------------------------------
//Check if there actually is a zero point in f(x,y) for certain.
bool TDrawThread::CheckResult2(const long double Result[3])
{
  //If the sign has changed, both last and current result is valid, and both last and current
  //value is increasing/decreasing (to avoid false vertical line at y=1/x)
  return (Result[1] < 0) == (Result[2] > 0) && !_isnanl(Result[1]) && !_isnanl(Result[2]) &&
    (Result[0] < Result[1]) == (Result[1] < Result[2]);
}
//---------------------------------------------------------------------------
void TDrawThread::EquationLoop(TRelation &Relation, std::vector<TRect> &Points, bool Loop)
{
  std::vector<long double> Args(2);
  Func32::ECalcError CalcError;
  double ds1 = Loop ? 1/Draw->xScale : -1/Draw->yScale;
  double ds2 = Loop ? -1/Draw->yScale : 1/Draw->xScale;
  bool LogScl1 = Loop ? Axes.xAxis.LogScl : Axes.yAxis.LogScl;
  bool LogScl2 = Loop ? Axes.yAxis.LogScl : Axes.xAxis.LogScl;

  int M1 = SizeScale(Relation.GetSize()) / 2;
  int M2 = std::max((SizeScale(Relation.GetSize()) + 1) / 2, 1);

  double s1Min = Loop ? Axes.xAxis.Min : Axes.yAxis.Max;
  double s2Min = Loop ? Axes.yAxis.Max : Axes.xAxis.Min;

  int S1Min = Loop ? AxesRect.Left - 1 : AxesRect.Top - 1;
  int S2Min = Loop ? AxesRect.Top : AxesRect.Left;

  int S1Max = Loop ? AxesRect.Right + 1 : AxesRect.Bottom + 1;
  int S2Max = Loop ? AxesRect.Bottom + 1 : AxesRect.Right + 1;

  int dS1 = Draw->Width > 1200 ? Draw->Width / 120 : 5;
  int dS2 = M2;

  LogScl1 ? s1Min /= std::exp(ds1/2) : s1Min -= ds1/2;

  double ds3 = LogScl1 ? std::exp(ds1) : ds1;
  ds1 *= dS1;
  ds2 *= dS2;
  if(LogScl1)
    ds1 = std::exp(ds1);
  if(LogScl2)
    ds2 = std::exp(ds2);

  double s2 = s2Min;
  for(int S2 = S2Min; S2 < S2Max && !Aborted; S2 += dS2)
  {
    Args[Loop] = s2;
    long double Result[3] = {NAN, NAN}; //Don't use plain double as it might cause overflow with large numbers
    double s1 = s1Min;
    for(int S1 = S1Min; S1 < S1Max; S1 += dS1)
    {
      Args[!Loop] = s1;
      Result[2] = Relation.Eval(Args, CalcError);
      if(CheckResult1(Result))
      {
        double s3 = LogScl1 ? s1 / ds1 * ds3 : s1 - ds1 + ds3;
        for(int S3 = S1 - dS1 + 1; S3 <= S1; S3++)
        {
          Args[!Loop] = s3;
          Result[2] = Relation.Eval(Args, CalcError);
          if(CheckResult2(Result))
            if(Loop)
              Points.push_back(TRect(S3 - M1, S2 - M1, S3 + M2, S2 + M2));
            else
              Points.push_back(TRect(S2 - M1, S3 - M1, S2 + M2, S3 + M2));
          Result[0] = Result[1];
          Result[1] = Result[2];
          if(LogScl1)
            s3 *= ds3;
          else
            s3 += ds3;
        }
      }
      else
        Result[0] = Result[1], Result[1] = Result[2];

      if(LogScl1)
        s1 *= ds1;
      else
        s1 += ds1;
    }
    if(LogScl2)
      s2 *= ds2;  //Don't integrate in for loop. It will trigger a bug in CB2009
    else
      s2 += ds2;
  }
}
//---------------------------------------------------------------------------
void TDrawThread::CreateEquation(TRelation &Relation)
{

  std::vector<TRect> Points;
  Points.reserve(500);

  EquationLoop(Relation, Points, 1);
  EquationLoop(Relation, Points, 0);

  if(!Aborted)
    Relation.Region.reset(new TRegion(Points));
}
//---------------------------------------------------------------------------
void TDrawThread::Visit(TRelation &Relation)
{
  if(!Relation.Region)
  {
    if(Relation.GetRelationType() == rtEquation)
      CreateEquation(Relation);
    else
      CreateInequality(Relation);
  }

  if(!Aborted)
    Synchronize(&DrawRelation, Relation);
}
//---------------------------------------------------------------------------
void TDrawThread::DrawRelation(const TRelation &Relation)
{
  Draw->SetClippingRegion();
  TColor Color = ForceBlack ? clBlack : Relation.GetColor();
  Context.SetBrush(Relation.GetRelationType() == rtInequality ? Relation.GetBrushStyle() : bsSolid, Color);
  Context.DrawRegion(*Relation.Region);

  //Draw a frame around inequalities using BoundingRegion
  //Warning: Do not try to use DrawFrameRegion(), which calls FrameRgn(), which
  //may be extremely slow for complex regions. Actually DrawRegion() can be slow too.
  if(Relation.BoundingRegion)
  {
    Context.SetBrush(bsSolid, Color);
    Context.DrawRegion(*Relation.BoundingRegion);
  }
}
//---------------------------------------------------------------------------
} //namespace Graph

