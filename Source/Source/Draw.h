/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef DrawH
#define DrawH
//---------------------------------------------------------------------------
#include "Data.h"
#include "Context.h"
#include <boost/function.hpp>
namespace Graph
{
class TDrawThread;
typedef std::vector<TPoint>::iterator TPointIter;
typedef std::vector<TPoint>::const_iterator TConstPointIter;

typedef boost::function0<void> TOnCompleteEvent;

struct TLabelInfo
{
  std::wstring Label;
  unsigned Width;
  int Pos;
};

class TDraw
{
  friend TDrawThread;
  friend class TDrawLegend;
  TContext Context;
  double xScale, yScale;  //The scale of x-axis and y-axis
  int Width, Height;      //Width and height of image
  TRect AxesRect;         //Rectangle where the coordinate system is shown
  TData *Data;
  TAxes &Axes;
  TDrawThread *Thread;
  TRect LegendRect;
  bool ForceBlack;
  double SizeMul;
  TOnCompleteEvent OnComplete;
  std::vector<TLabelInfo> yLabelInfo;

  //Temp variables
  double xAxisCross, yAxisCross;
  double yTickMin, xTickMin;
  int xPixelCross, yPixelCross;
  int NumberHeight;

  void PreCalcXAxis();
  void PreCalcYAxis();
  void DrawPolyline(TConstPointIter Begin, TConstPointIter End, TPenStyle Style, int LineSize, TColor Color);
  void DrawPolydots(TConstPointIter Begin, TConstPointIter End, int LineSize, TColor Color);
  void DrawLegend();
  static double GetMinValue(double Unit, double Min, double Max, double AxisCross, bool Log);
  static std::wstring MakeNumber(double Number, bool MultiplyByPi);
  unsigned FindLabels();

  TDraw(const TDraw&); //Not implemented
  TDraw& operator=(const TDraw&); //Not implemented

public:
  TDraw(TCanvas *Canvas, TData *pData, bool AForceBlack, const std::string &ThreadName);
  ~TDraw();
  void DrawAll();
  void DrawFunc(const TGraphElem *F);
  int xPoint(long double x) const; //Return pixel coresponding to x value
  int yPoint(long double y) const; //Return pixel coresponding to y value
  TPoint xyPoint(long double x, long double y) const {return TPoint(xPoint(x), yPoint(y));}
  TPoint xyPoint(const Func32::TCoord<double> &Coord) const {return TPoint(xPoint(Coord.x), yPoint(Coord.y));}
  TPoint xyPoint(const Func32::TCoord<long double> &Coord) const {return TPoint(xPoint(Coord.x), yPoint(Coord.y));}
  TPoint xyPoint(const Func32::TCoord<Func32::TComplex> &Coord) const {return TPoint(xPoint(Coord.x.real()), yPoint(Coord.y.real()));}
  double xPointExact(long double x) const; //Same as xPoint(), but returns double and don't check for error
  double yPointExact(long double y) const;
  double xCoord(int x) const;
  double yCoord(int y) const;
  Func32::TDblPoint xyCoord(int x, int y) const {return Func32::TDblPoint(xCoord(x), yCoord(y));}
  Func32::TDblPoint xyCoord(const TPoint &P) const {return xyCoord(P.x, P.y);}
  bool AbortUpdate();
  bool Wait();
  bool Updating();
  void SetSize(int Width, int Height);
  int SizeScale(int I) const {return I*SizeMul+0.5;}
  void SetArea(const TRect &Area);
  void RedrawAxes();
  void DrawAxes();
  void SetClippingRegion();
  double GetScaledYAxis() const;
  void SetSizeMul(double xSizeMul, double ySizeMul);
  bool InsideLegend(int X, int Y) const {return Data->Axes.ShowLegend && InsideRect(LegendRect, TPoint(X, Y));}
  static void DrawPointLabel(TCanvas *Canvas, TPoint Pos, int PointSize, const std::wstring &Label, TLabelPosition LabelPosition);
  void SetOnComplete(TOnCompleteEvent AOnComplete) {OnComplete = AOnComplete;}
  const TRect& GetLegendRect() const {return LegendRect;}
  void SetCanvas(TCanvas *Canvas) {Context.SetCanvas(Canvas);}
  const TRect& GetAxesRect() const {return AxesRect;}
};
} //namespace Graph
#endif
