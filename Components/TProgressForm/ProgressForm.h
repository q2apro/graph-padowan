//===========================================================================
// Copyright © 2003 Ivan Johansen
// ProgressForm.h
//===========================================================================
#ifndef ProgressFormH
#define ProgressFormH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include "Progress.h"
//---------------------------------------------------------------------------
class TProgressForm : public TComponent
{
private:
  String FCaption;
  int FMin;
  int FMax;
  int FStep;
  int FPosition;
  int FDelay;
  bool FAbortProgress;
  TColor FColor;
  TCursor FCursor;
  String FButtonCaption;

  TNotifyEvent FOnShow;
  TNotifyEvent FOnCancel;

  TForm *Form;
  TTimer *Timer;
  TProgress *Progress;
  TButton *Button;

  void __fastcall SetCaption(String Str);
  void __fastcall SetButtonCaption(String Str);
  void __fastcall SetMin(int Value);
  void __fastcall SetMax(int Value);
  void __fastcall SetStep(int Value);
  void __fastcall SetPosition(int Value);
  void __fastcall SetColor(TColor AColor);
  void __fastcall ShowForm(TObject *Sender);
  void __fastcall ButtonCancel(TObject *Sender);

protected:
public:
  __fastcall TProgressForm(TComponent* Owner);
  __fastcall ~TProgressForm();
  void __fastcall StepIt();
  void __fastcall Show();
  void __fastcall Close();

  __property bool AbortProgress = {read=FAbortProgress};

__published:
  __property String Caption = {read=FCaption, write=SetCaption};
  __property int Min = {read=FMin, write=SetMin, default=0};
  __property int Max = {read=FMax, write=SetMax, default=100};
  __property int Step = {read=FStep, write=SetStep, default=1};
  __property int Position = {read=FPosition, write=SetPosition, default=0};
  __property int Delay = {read=FDelay, write=FDelay, default=500};
  __property TCursor Cursor = {read=FCursor, write=FCursor, default=crAppStart};
  __property TColor Color = {read=FColor, write=SetColor, default=clBlue};
  __property String ButtonCaption = {read=FButtonCaption, write=SetButtonCaption};

  __property TNotifyEvent OnShow = {read=FOnShow, write=FOnShow, default=NULL};
  __property TNotifyEvent OnCancel = {read=FOnCancel, write=FOnCancel, default=NULL};
};
//---------------------------------------------------------------------------
#endif
 