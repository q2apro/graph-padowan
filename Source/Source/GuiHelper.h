/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef GuiHelperH
#define GuiHelperH
#include <set>
//---------------------------------------------------------------------------
class TAddView : public TGraphElemVisitor
{
  std::map<TColor, int> FuncIconMap;
  TTreeNode* AddNode(TGraphElem &Elem, int ImageIndex);
  int AddFuncImage(TColor Color);
public:
  void Visit(TBaseFuncType &Func);
  void Visit(TTan &Tan);
  void Visit(TShading &Shade);
  void Visit(TPointSeries &Series);
  void Visit(TTextLabel &Label);
  void Visit(TRelation &Relation);
  void Visit(TAxesView &AxesView);
};

struct TZoomFit : public TGraphElemVisitor
{
  const TData &Data;
  const TDraw &Draw;
  double xMin, xMax, yMin, yMax;
  bool IsChanged() const;

  TZoomFit(const TData &AData, const TDraw &ADraw);
  void Visit(TBaseFuncType &Func);
  void Visit(TTan &Tan) {Visit(static_cast<TBaseFuncType&>(Tan));} //Forward
  void Visit(TShading &Shade) {} //Not used
  void Visit(TPointSeries &Series);
  void Visit(TTextLabel &Label) {}  //Not used
  void Visit(TRelation &Relation);
  void Visit(TAxesView &AxesView) {} //Not used
};

struct TFindColors : public TGraphElemVisitor
{
  std::set<TColor> &Colors;
  TFindColors(std::set<TColor> &AColors) : Colors(AColors) {}
  void Visit(TBaseFuncType &Func);
  void Visit(TTan &Tan) {Visit(static_cast<TBaseFuncType&>(Tan));} //Forward
  void Visit(TShading &Shade);
  void Visit(TPointSeries &Series);
  void Visit(TTextLabel &Label);
  void Visit(TRelation &Relation);
  void Visit(TAxesView &AxesView);
};

void FindColors(const TData &Data, std::set<TColor> &Colors);
#endif
