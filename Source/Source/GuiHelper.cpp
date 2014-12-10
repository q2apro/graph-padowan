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
#include "GuiHelper.h"
#include "PointSelect.h"
#include "Unit1.h"
#include <float.h>
//---------------------------------------------------------------------------
//////////////
// TAddView //
//////////////
TTreeNode* TAddView::AddNode(TGraphElem &Elem, int ImageIndex)
{
  //Replace Hyphen-Minus with Minus Sign
  std::wstring Str = Elem.MakeLegendText();
  std::replace(Str.begin(), Str.end(), L'-', L'\x2212');

  TTreeNode *Node = Elem.GetParent() ?
    Form1->TreeView->Items->AddChild(Form1->GetNode(Elem.GetParent()), Str.c_str()) :
    Form1->TreeView->Items->Add(NULL, Str.c_str());

  Node->ImageIndex = ImageIndex;
  Node->SelectedIndex = ImageIndex;
  switch(Elem.GetVisible())
  {
    case -1: Node->StateIndex =  iiGrayed; break;
    case 0:  Node->StateIndex =  iiUnChecked; break;
    case 1:  Node->StateIndex =  iiChecked; break;
  }
  return Node;
}
//---------------------------------------------------------------------------
int TAddView::AddFuncImage(TColor Color)
{
  std::map<TColor,int>::iterator Iter = FuncIconMap.find(Color);
  if(Iter == FuncIconMap.end())
    return FuncIconMap[Color] = Form1->AddImage(iiFuncNode, Color);
  return Iter->second;
}
//---------------------------------------------------------------------------
void TAddView::Visit(TBaseFuncType &Func)
{
  AddNode(Func, AddFuncImage(Func.Color));
  for(unsigned I = 0; I < Func.ChildCount(); I++)
    Func.GetChild(I)->Accept(*this);
}
//---------------------------------------------------------------------------
void TAddView::Visit(TTan &Tan)
{
  AddNode(Tan, Tan.TangentType == ttTangent ? iiTanNode : iiNormalNode);
}
//---------------------------------------------------------------------------
void TAddView::Visit(TShading &Shade)
{
  AddNode(Shade, Form1->AddImage(Shade.Color, Shade.BrushStyle));
}
//---------------------------------------------------------------------------
void TAddView::Visit(TPointSeries &Series)
{
  std::auto_ptr<Graphics::TBitmap> Bitmap(new Graphics::TBitmap);
  Bitmap->Width = 16;
  Bitmap->Height = 16;
  Bitmap->Canvas->Brush->Color = clWhite;
  Bitmap->Canvas->FillRect(TRect(0, 0, Bitmap->Width, Bitmap->Height));
  TPointSelect::DrawPoint(
    Bitmap->Canvas,
    TPoint(8,8),
    Series.GetStyle(),
    Series.GetFrameColor(),
    Series.GetFillColor(),
    5);
  AddNode(Series, Form1->ImageList1->AddMasked(Bitmap.get(), clWhite));
}
//---------------------------------------------------------------------------
void TAddView::Visit(TTextLabel &Label)
{
  AddNode(Label, iiLabelNode);
}
//---------------------------------------------------------------------------
void TAddView::Visit(TRelation &Relation)
{
  int ImageIndex;
  if(Relation.GetRelationType() == rtInequality)
    ImageIndex = Form1->AddImage(Relation.GetColor(), Relation.GetBrushStyle());
  else
    ImageIndex = AddFuncImage(Relation.GetColor());
  AddNode(Relation, ImageIndex);
}
//---------------------------------------------------------------------------
void TAddView::Visit(TAxesView &AxesView)
{
  AddNode(AxesView, iiAxesNode)->Text = LoadRes(RES_AXES);
}
//---------------------------------------------------------------------------
//////////////
// TZoomFit //
//////////////
TZoomFit::TZoomFit(const TData &AData, const TDraw &ADraw)
 : Data(AData), Draw(ADraw)
{
  //Always show axes when Axes style is Crossed
  xMin = Data.Axes.AxesStyle == asCrossed ? Data.Axes.yAxis.AxisCross : INF;
  xMax = Data.Axes.AxesStyle == asCrossed ? Data.Axes.yAxis.AxisCross : -INF;
  yMin = Data.Axes.AxesStyle == asCrossed ? Data.Axes.xAxis.AxisCross : INF;
  yMax = Data.Axes.AxesStyle == asCrossed ? Data.Axes.xAxis.AxisCross : -INF;
}
//---------------------------------------------------------------------------
bool TZoomFit::IsChanged() const
{
  return xMin != xMax && yMin != yMax && _finite(xMin) && _finite(xMax) && _finite(yMin) && _finite(yMax);
}
//---------------------------------------------------------------------------
void TZoomFit::Visit(TBaseFuncType &Func)
{
  if(TStdFunc *StdFunc = dynamic_cast<TStdFunc*>(&Func))
  {
    Func32::ECalcError E;
    if(std::_finite(StdFunc->From.Value))
      if(Func.GetFunc().Calc(StdFunc->From.Value, E), E.ErrorCode == Func32::ecNoError) //Note comma operator
        xMin = StdFunc->From.Value;
    if(std::_finite(StdFunc->To.Value))
      if(Func.GetFunc().Calc(StdFunc->To.Value, E), E.ErrorCode == Func32::ecNoError) //Note comma operator
        xMax = StdFunc->To.Value;
  }

  for(std::vector<Func32::TCoordSet<> >::const_iterator Iter = Func.sList.begin(); Iter != Func.sList.end(); ++Iter)
  {
    if(Iter->x < xMin)
      xMin = Iter->x;
    if(Iter->x > xMax)
      xMax = Iter->x;
    if(Iter->y < yMin)
      yMin = Iter->y;
    if(Iter->y > yMax)
      yMax = Iter->y;
  }
}
//---------------------------------------------------------------------------
void TZoomFit::Visit(TPointSeries &Series)
{
  const TPointSeries::TPointList &PointList = Series.GetPointList();
  for(TPointSeries::TPointList::const_iterator Point = PointList.begin(); Point != PointList.end(); ++Point)
    //Check if point is valid
    if((!Data.Axes.xAxis.LogScl || Point->x > 0) && (!Data.Axes.yAxis.LogScl || Point->y > 0))
    {
      if(Point->x < xMin)
        xMin = Point->x;
      if(Point->x > xMax)
        xMax = Point->x;
      if(Point->y < yMin)
        yMin = Point->y;
      if(Point->y > yMax)
        yMax = Point->y;
    }
}
//---------------------------------------------------------------------------
void TZoomFit::Visit(TRelation &Relation)
{
  if(!Relation.Region)
    return;

  TRect Rect = Relation.Region->GetBoundingRect();
  xMin = Draw.xCoord(Rect.Left);
  xMax = Draw.xCoord(Rect.Right);
  yMin = Draw.yCoord(Rect.Bottom);
  yMax = Draw.yCoord(Rect.Top);
}
//---------------------------------------------------------------------------
/////////////////
// TFindColors //
/////////////////
void TFindColors::Visit(TBaseFuncType &Func)
{
  Colors.insert(Func.Color);
}
//---------------------------------------------------------------------------
void TFindColors::Visit(TShading &Shade)
{
  Colors.insert(Shade.Color);
}
//---------------------------------------------------------------------------
void TFindColors::Visit(TPointSeries &Series)
{
  Colors.insert(clBlack);
  Colors.insert(Series.GetFillColor());
  Colors.insert(Series.GetLineColor());
  Colors.insert(Series.GetFrameColor());
}
//---------------------------------------------------------------------------
void TFindColors::Visit(TTextLabel &Label)
{
  Colors.insert(Label.GetBackgroundColor());
}
//---------------------------------------------------------------------------
void TFindColors::Visit(TRelation &Relation)
{
  Colors.insert(Relation.GetColor());
}
//---------------------------------------------------------------------------
void TFindColors::Visit(TAxesView &AxesView)
{
  Colors.insert(AxesView.GetAxes().AxesColor);
  Colors.insert(AxesView.GetAxes().GridColor);
  Colors.insert(AxesView.GetAxes().BackgroundColor);
}
//---------------------------------------------------------------------------
void FindColors(const TData &Data, std::set<TColor> &Colors)
{
  TFindColors ColorFinder(Colors);
  for(unsigned I = 0; I < Data.ElemCount(); I++)
    if(Data.GetElem(I)->GetVisible())
      Data.GetElem(I)->Accept(ColorFinder);
}
//---------------------------------------------------------------------------

