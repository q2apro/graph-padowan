//===========================================================================
// Copyright © 2011 Ivan Johansen
// ExtComboBox.cpp
//===========================================================================
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "ExtComboBox.h"
#include <algorithm>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TExtComboBox *)
{
  new TExtComboBox(NULL);
}
//---------------------------------------------------------------------------
namespace Extcombobox
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TExtComboBox)};
    RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TExtComboBox::TExtComboBox(TComponent* Owner)
  : TComboBox(Owner), FAutoDroppedWidth(true), FDroppedWidth(0), DroppedWidthFound(false)
{
}
//---------------------------------------------------------------------------
void __fastcall TExtComboBox::SetDroppedWidth(int AWidth)
{
  SendMessage(Handle, CB_SETDROPPEDWIDTH, AWidth, 0);
  FDroppedWidth = AWidth;
}
//---------------------------------------------------------------------------
void __fastcall TExtComboBox::DropDown()
{
  if(FAutoDroppedWidth && !DroppedWidthFound)
  {
    DroppedWidthFound = true;
    DroppedWidth = FindMaxWidth();
  }
  inherited::DropDown();
}
//---------------------------------------------------------------------------
int TExtComboBox::FindMaxWidth()
{
  int Count = Items->Count;
  int MaxWidth = 0;
  for(int I = 0; I < Count; I++)
    MaxWidth = std::max(MaxWidth, Canvas->TextWidth(Items->Strings[I]));
  return MaxWidth + 10;
}
//---------------------------------------------------------------------------
void __fastcall TExtComboBox::ChangeScale(int M, int D)
{
  TComboBox::ChangeScale(M, D); // Call inherited
  ItemHeight = (ItemHeight * M) / D;
}
//---------------------------------------------------------------------------

