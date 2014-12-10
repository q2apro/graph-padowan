/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef GraphUndoH
#define GraphUndoH
#include "Undo.h"

namespace Graph
{
struct TUndoAdd
{
	TGraphElemPtr GraphElem;

	TUndoAdd(const TGraphElemPtr &AGraphElem)
    : GraphElem(AGraphElem) {}
  void operator()(TUndoList &UndoList) const;
};

struct TUndoChange
{
  TGraphElemPtr OldElem;
	TGraphElemPtr NewElem;

	TUndoChange(const TGraphElemPtr &AOldElem, const TGraphElemPtr &ANewElem)
		: OldElem(AOldElem), NewElem(ANewElem)
	{
		OldElem->ClearCache();
	}

	void operator()(TUndoList &UndoList) const
	{
		UndoList.Push(TUndoChange(NewElem, OldElem));
		TData::Replace(NewElem, OldElem);
	}
};

struct TUndoDel
{
	int Index;
	TGraphElemPtr Elem;
	TGraphElemPtr Parent;

	TUndoDel(const TGraphElemPtr &AElem, const TGraphElemPtr &AParent, int AIndex)
		: Index(AIndex), Elem(AElem), Parent(AParent)  { }
	void operator()(TUndoList &UndoList) const
	{
		UndoList.Push(TUndoAdd(Elem));
		Parent->InsertChild(Elem, Index);
	}
};

inline void TUndoAdd::operator()(TUndoList &UndoList) const
{
  UndoList.Push(TUndoDel(GraphElem, GraphElem->GetParent(), TData::GetIndex(GraphElem)));
  TData::Delete(GraphElem);
}


template<class T>
struct TUndoObject
{
  T &OrgObject;
	T Object;
  TUndoObject(T &AOrgObject, const T &AObject) : OrgObject(AOrgObject), Object(AObject) {}
  void operator()(TUndoList &UndoList)
  {
    UndoList.Push(TUndoObject<T>(OrgObject, OrgObject));
    OrgObject = Object;
  }
};

struct TUndoAxes
{
  TAxes Axes;
  TData &Data;
  TUndoAxes(TData &AData) : Data(AData), Axes(AData.Axes) {}
  void operator()(TUndoList &UndoList)
  {
    UndoList.Push(TUndoAxes(Data));
		Data.Axes = Axes;
		Data.ClearCache();
    Data.Update(); //In case trigonmetry has changed
  }
};

struct TUndoCustomFunctions
{
  boost::shared_ptr<TCustomFunctions> CustomFunctions;
  TData &Data;
  TUndoCustomFunctions(TData &AData) //This will steal the custom functions from AData
    : Data(AData), CustomFunctions(new TCustomFunctions)
  {
    CustomFunctions->Swap(AData.CustomFunctions);
  }
	void operator()(TUndoList &UndoList)
  {
    Data.CustomFunctions.Swap(*CustomFunctions);
    UndoList.Push(*this);
    Data.Update();
    Data.ClearCache();
  }
};

struct TUndoMove
{
  TData &Data;
  TGraphElemPtr Elem;
  unsigned Index;

  TUndoMove(TData &AData, const TGraphElemPtr &AElem, unsigned AIndex)
    : Data(AData), Elem(AElem), Index(AIndex) {}
	void operator()(TUndoList &UndoList)
	{
		const TGraphElemPtr &Parent = Elem->GetParent();
		int OldIndex = Parent->GetChildIndex(Elem);
		UndoList.Push(TUndoMove(Data, Elem, OldIndex));
		Parent->RemoveChild(OldIndex);
		Parent->InsertChild(Elem, Index);
	}
};

struct TUndoAddPoint
{
	TPointSeriesPtr Series;
	TUndoAddPoint(TPointSeriesPtr ASeries) : Series(ASeries) {}
	void operator()(TUndoList &UndoList);
};

struct TUndoDelPoint
{
	TPointSeriesPtr Series;
	TPointSeriesPoint Point;
	TUndoDelPoint(TPointSeriesPtr ASeries, const TPointSeriesPoint &APoint)
		: Series(ASeries), Point(APoint) {}
	void operator()(TUndoList &UndoList)
	{
		Series->InsertPoint(Point);
		UndoList.Push(TUndoAddPoint(Series));
	}
};

inline void TUndoAddPoint::operator()(TUndoList &UndoList)
{
	UndoList.Push(TUndoDelPoint(Series, Series->GetPoint(Series->PointCount()-1)));
	Series->DeletePoint(Series->PointCount()-1);
}

template<class T>
TUndoObject<T> MakeUndoObject(T &OrgObject)
{
  return TUndoObject<T>(OrgObject, OrgObject);
}

extern TUndoList UndoList;
//---------------------------------------------------------------------------
} //namespace Graph
#endif
