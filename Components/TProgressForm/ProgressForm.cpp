//===========================================================================
// Copyright © 2003 Ivan Johansen
// ProgressForm.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include "ProgressForm.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TProgressForm));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TProgressForm *)
{
  new TProgressForm(NULL);
}
//---------------------------------------------------------------------------
namespace Progressform
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TProgressForm)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner)
 : TComponent(Owner), FMin(0), FMax(100), FStep(1),
   FPosition(0), FDelay(500), FCaption("Working"), FOnShow(NULL), FOnCancel(NULL),
   FAbortProgress(false), FCursor(crAppStart), FColor(clBlue), FButtonCaption("Cancel"),
   Form(NULL), Progress(NULL), Timer(NULL), Button(NULL)
{
  Timer = new TTimer(this);
  Timer->Enabled = false;
  Timer->OnTimer = ShowForm;

  Form = new TForm(this);
  Form->Caption = FCaption;
  Form->Width = 263;
  Form->Height = 152;
  Form->BorderStyle = bsDialog;
  Form->BorderIcons = TBorderIcons();
  Form->Position = poOwnerFormCenter;

  Progress = new TProgress(this);
  Progress->Parent = Form;
  Progress->Min = FMin;
  Progress->Max = FMax;
  Progress->Step = FStep;
  Progress->Position = FPosition;
  Progress->Top = 32;
  Progress->Left = 24;
  Progress->Width = 209;

  Button = new TButton(this);
  Button->Parent = Form;
  Button->Top = 80;
  Button->Left = 88;
  Button->Caption = FButtonCaption;
  Button->Cancel = true;
  Button->OnClick = ButtonCancel;
}
//---------------------------------------------------------------------------
__fastcall TProgressForm::~TProgressForm()
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetCaption(String Str)
{
  FCaption = Str;
  Form->Caption = FCaption;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetMin(int Value)
{
  if(Value > Max)
    return;
  FMin = Value;
  if(Progress)
    Progress->Min = Value;
  if(Step < Min)
    Step = Min;
  if(Step > Max)
    Step = Max;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetMax(int Value)
{
  if(Value < Min)
    return;
  FMax = Value;
  if(Progress)
    Progress->Max = Value;
  if(Step < Min)
    Step = Min;
  if(Step > Max)
    Step = Max;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetStep(int Value)
{
  if(Progress)
    Progress->Step = Value;
  FStep = Value;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::StepIt()
{
  if(Progress && Position < Max)
  {
    Progress->StepIt();
    FPosition += FStep;
    Application->ProcessMessages();
  }
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetPosition(int Value)
{
  if(Value < Min)
    FPosition = Min;
  else if(Value > Max)
    FPosition = Max;
  else
    FPosition = Value;
  if(Progress)
    Progress->Position = Value;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::Show()
{
  if(Form->Visible)
    return;

  FAbortProgress = false;

  if(FDelay)
  {
    Timer->Interval = FDelay;
    Timer->Enabled = true;
  }
  else
    ShowForm(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::Close()
{
  if(TForm *OwnerForm = dynamic_cast<TForm*>(Owner))
    OwnerForm->Enabled = true;
  Timer->Enabled = false;
  Form->Close();
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::ShowForm(TObject *Sender)
{
  Timer->Enabled = false;
  if(OnShow)
    OnShow(this);

  Form->Cursor = FCursor;

  Form->Show();
  if(TForm *OwnerForm = dynamic_cast<TForm*>(Owner))
    OwnerForm->Enabled = false;
  Form->Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::ButtonCancel(TObject *Sender)
{
  FAbortProgress = true;
  if(OnCancel)
    OnCancel(this);
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetColor(TColor AColor)
{
  FColor = AColor;
  Progress->Color = AColor;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::SetButtonCaption(String Str)
{
  FButtonCaption = Str;
  Button->Caption = Str;
}
//---------------------------------------------------------------------------

