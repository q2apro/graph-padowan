/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#pragma hdrstop
#include "Config.h"
#include "Undo.h"
#include <cassert>
//---------------------------------------------------------------------------
struct TMultiUndoItem : TUndoItemBase
{
  std::deque<boost::shared_ptr<TUndoItemBase> > List;

  void Undo(TUndoList &UndoList)
  {
    while(!List.empty())
    {
      List.back()->Undo(UndoList);
      List.pop_back();
    }
  }
};
//---------------------------------------------------------------------------
TUndoList::TUndoList(unsigned int Size) : MaxUndo(Size), MultiUndo()
{
}
//---------------------------------------------------------------------------
TUndoList::~TUndoList()
{
  Clear();
}
//---------------------------------------------------------------------------
void TUndoList::SetMaxUndo(unsigned Size)
{
  MaxUndo = Size;
  while(UndoStack.size() > MaxUndo)
    UndoStack.pop_front();
}
//---------------------------------------------------------------------------
void TUndoList::PushItem(boost::shared_ptr<TUndoItemBase> UndoItem, TUndoState UndoState)
{
  if(MultiUndo)
    MultiUndoItem->List.push_back(UndoItem);
  else
  {
    if(UndoState == usUndoing)
      RedoStack.push_back(UndoItem);
    else
      UndoStack.push_back(UndoItem);

    if(UndoState == usIdle)
    {
      RedoStack.clear();
      if(UndoStack.size() > MaxUndo)
        UndoStack.pop_front();
    }
  }
}
//---------------------------------------------------------------------------
void TUndoList::Undo()
{
  assert(MultiUndo == 0);
  if(UndoStack.empty())
    return;

  BeginMultiUndo();
  UndoStack.back()->Undo(*this);
  UndoStack.pop_back();
  EndMultiUndo(usUndoing);
}
//---------------------------------------------------------------------------
void TUndoList::Redo()
{
  if(RedoStack.empty())
    return;

  BeginMultiUndo();
  RedoStack.back()->Undo(*this);
  RedoStack.pop_back();
  EndMultiUndo(usRedoing);
}
//---------------------------------------------------------------------------
bool TUndoList::CanUndo() const
{
  return !UndoStack.empty();
}
//---------------------------------------------------------------------------
bool TUndoList::CanRedo() const
{
  return !RedoStack.empty();
}
//---------------------------------------------------------------------------
void TUndoList::Clear()
{
  UndoStack.clear();
  RedoStack.clear();
}
//---------------------------------------------------------------------------
void TUndoList::ClearRedo()
{
  RedoStack.clear();
}
//---------------------------------------------------------------------------
void TUndoList::BeginMultiUndo()
{
  if(MultiUndo++ == 0)
    MultiUndoItem.reset(new TMultiUndoItem);
}
//---------------------------------------------------------------------------
void TUndoList::EndMultiUndo(TUndoState UndoState)
{
  assert(MultiUndo != 0);
  if(--MultiUndo == 0 && !MultiUndoItem->List.empty())
  {
    PushItem(MultiUndoItem, UndoState);
    MultiUndoItem.reset();
  }
}
//---------------------------------------------------------------------------




