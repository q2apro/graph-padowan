/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef UndoH
#define UndoH
#include <deque>
#include <boost/shared_ptr.hpp>

class TUndoList;

struct TUndoItemBase
{
  virtual void Undo(TUndoList &UndoList) =0;
  virtual ~TUndoItemBase() {}
};

template<class TFunctor>
struct TUndoItem : TUndoItemBase
{
  TFunctor Functor;
  TUndoItem(const TFunctor &AFunctor) : Functor(AFunctor) {}
  void Undo(TUndoList &UndoList)
  {
    Functor(UndoList);
  }
};

class TUndoList
{
  enum TUndoState {usIdle, usUndoing, usRedoing};
  unsigned MaxUndo;
  unsigned MultiUndo;
  boost::shared_ptr<class TMultiUndoItem> MultiUndoItem;
  std::deque<boost::shared_ptr<TUndoItemBase> > UndoStack;
  std::deque<boost::shared_ptr<TUndoItemBase> > RedoStack;

  TUndoList(const TUndoList&); //Not implemented
  TUndoList& operator=(const TUndoList&); //Not implemented
  void PushItem(boost::shared_ptr<TUndoItemBase> UndoItem, TUndoState UndoState);
  void EndMultiUndo(TUndoState UndoState);

public:
  TUndoList(unsigned Size = 0);
  ~TUndoList();
  void Undo();
  void Redo();
  bool CanUndo() const;
  bool CanRedo() const;
  void Clear();
  void ClearRedo();
  void SetMaxUndo(unsigned Size);
  unsigned GetMaxUndo() const {return MaxUndo;}
  void BeginMultiUndo();
  void EndMultiUndo() {EndMultiUndo(usIdle);}

  template<class TFunctor>
  void Push(const TFunctor &Functor)
  {
    PushItem(boost::shared_ptr<TUndoItemBase>(new TUndoItem<TFunctor>(Functor)), usIdle);
  }
};
//---------------------------------------------------------------------------
#endif
