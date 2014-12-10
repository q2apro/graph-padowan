/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit9H
#define Unit9H
#include "TAreaFrame.h"
#include "TParFuncFrame.h"
#include "TPolFuncFrame.h"
#include "TStdFuncFrame.h"
#include "TTanFrame.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "TAreaFrame.h"
#include "TParFuncFrame.h"
#include "TPolFuncFrame.h"
#include "TStdFuncFrame.h"
#include "TTanFrame.h"
#include "TEvalFrame.h"
//---------------------------------------------------------------------------
enum TEvalType {etEval, etArea, etArc};
class TForm9 : public TForm
{
__published:	// IDE-managed Components
  TStdFuncFrame *StdFuncFrame1;
  TParFuncFrame *ParFuncFrame1;
  TLabel *Label1;
  TPolFuncFrame *PolFuncFrame1;
  TAreaFrame *AreaFrame1;
  TTanFrame *TanFrame1;
  void __fastcall FormHide(TObject *Sender);
  void __fastcall Edit1Change(TObject *Sender);
  void __fastcall FormShortCut(TWMKey &Msg, bool &Handled);
  void __fastcall FormStartDock(TObject *Sender,
  TDragDockObject *&DragObject);
  void __fastcall FormEndDock(TObject *Sender, TObject *Target, int X, int Y);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall UpDownExUpDown(TObject *Sender, TUDBtnType Button);


private:	// User declarations
    boost::shared_ptr<TGraphElem> Elem;
    TEvalType EvalType;
    int SizeDif; //Difference between Height and ClientHeight
    TVclObject<TStringList> StdFuncStrings, ParFuncStrings, PolFuncStrings;
    TEvalFrame *VisibleFrame;

    void __fastcall Loaded() {VisibleFrame=NULL; TForm::Loaded();} //Prevent crash when resources are reloaded
    void ShowFrame(TEvalFrame *Frame);
    TFrame* GetFrame() {return VisibleFrame->GetFrame();} //Workaround for nasty compiler bug

public:		// User declarations
    __fastcall TForm9(TComponent* Owner);
    void FuncChanged(const boost::shared_ptr<TGraphElem> &AElem);
    void StartValueChanged(int X, int Y);
    void EndValueChanged(int X, int Y);
    void Clear();
    void SetEvalType(TEvalType AEvalType);
    void Translate();
    int GetFrameHeight() {return VisibleFrame ? GetFrame()->Height : 0;}
};
//---------------------------------------------------------------------------
#endif
