//===========================================================================
// Copyright © 2009 Ivan Johansen
// UpDownEx.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "UpDownEx.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TUpDownEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TUpDownEx *)
{
  new TUpDownEx(NULL);
}
//---------------------------------------------------------------------------
namespace Updownex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TUpDownEx)};
     RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TUpDownEx::TUpDownEx(TComponent* Owner)
  : TCustomUpDown(Owner), FOnUpDown(NULL), Associate(NULL)
{
  Min = -32768;
  Max = 32767;
}
//---------------------------------------------------------------------------
void __fastcall TUpDownEx::SetAssociate(TWinControl *Value)
{
/*
  TCustomEdit *Edit = dynamic_cast<TCustomEdit*>(Value);
  String Str = Edit ? Edit->Text : String();
  TCustomUpDown::Associate = Value;
  if(Edit)
    Edit->Text = Str;
    */
/*  if(Value != NULL)
    for(int I = 0; I < Parent->ControlCount; I++) // is control already associated
      if(TCustomUpDown *UpDown = dynamic_cast<TCustomUpDown*>(Parent->Controls[I]))
        if(UpDown != this)
          if(UpDown->Associate == Value)*/
            ;//throw Exception(SUDAssociated,            [Value.Name, Parent.Controls[I].Name]);

  if(FAssociate != NULL) //undo the current associate control
  {
    if(HandleAllocated())
      SendMessage(Handle, UDM_SETBUDDY, 0, 0);
    FAssociate = NULL;
  }

  if(Value != NULL && Value->Parent == Parent &&
    !dynamic_cast<TCustomUpDown*>(Value) &&
    !dynamic_cast<TCustomTreeView*>(Value) &&
    !dynamic_cast<TCustomListView*>(Value))
  {
    if(HandleAllocated())
    {
      UndoAutoResizing(Value);
      SendMessage(Handle, UDM_SETBUDDY, (int)Value->Handle, 0);
    }
    FAssociate = Value;
  }
}
//---------------------------------------------------------------------------
void TUpDownEx::UndoAutoResizing(TWinControl *Value)
{
  //undo Window's auto-resizing
  int OrigWidth = Value->Width;
  int OrigLeft = Value->Left;
  SendMessage(Handle, UDM_SETBUDDY, (int)Value->Handle, 0);
  int NewWidth = Value->Width;
  int NewLeft = Value->Left;
  int DeltaWidth = OrigWidth - NewWidth;
  int DeltaLeft = NewLeft - OrigLeft;
  Value->Width = OrigWidth + DeltaWidth;
  Value->Left = OrigLeft - DeltaLeft;
}
//---------------------------------------------------------------------------
void TUpDownEx::CNNotify(TWMNotifyUD &Message)
{
  if(Message.NMUpDown->hdr.code == UDN_DELTAPOS)
  {
    if(FOnUpDown)
      FOnUpDown(this, Message.NMUpDown->iDelta < 0 ? btPrev : btNext);
    Message.Result = 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TUpDownEx::CreateWnd()
{
  int OrigWidth = Width;  //control resizes width - disallowing user to set width
  TWinControl::CreateWnd();
  if(FAssociate != NULL)
  {
    UndoAutoResizing(FAssociate);
    SendMessage(Handle, UDM_SETBUDDY, (int)FAssociate->Handle, 0);
    SendMessage(Handle, UDM_SETRANGE, 0, MAKELONG(100, 0));
    TUDAccel Accel;
    SendMessage(Handle, UDM_GETACCEL, 1, (int)&Accel);
    Accel.nInc = 1;
    SendMessage(Handle, UDM_SETACCEL, 1, (int)&Accel);
  }
  Width = OrigWidth;
}
//---------------------------------------------------------------------------
