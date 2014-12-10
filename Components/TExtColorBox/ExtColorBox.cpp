//===========================================================================
// Copyright © 2005 Ivan Johansen
// ExtColorBox.cpp
//===========================================================================
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include "ExtColorBox.h"
#include "IColorSelect.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TExtColorBox));
static TColor ColorList[] =
{
  clBlack,
  clMaroon,
  clGreen,
  clOlive,
  clNavy,
  clPurple,
  clTeal,
  clGray,
  clSilver,
  clRed,
  clLime,
  clYellow,
  clBlue,
  clFuchsia,
  clAqua,
  clWhite,
  clMoneyGreen,
  clSkyBlue,
  clMedGray,
};

// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TExtColorBox *)
{
  new TExtColorBox(NULL);
}
//---------------------------------------------------------------------------
namespace Extcolorbox
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TExtColorBox)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TExtColorBox::TExtColorBox(TComponent* Owner)
  : TCustomComboBox(Owner), FOnPickColor(NULL), FColorDialogType(cdtColorSelect), FDroppedWidth(0),
    FAutoDroppedWidth(true), DroppedWidthFound(false), FShowDefault(false), FShowCustom(true),
		FDefaultName("Default"), FCustomName("Custom..."), FSelected(clBlack)
{
	Style = csOwnerDrawFixed;
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::Loaded()
{
	PopulateList();
	Selected = FSelected;
	TCustomComboBox::Loaded();
}
//---------------------------------------------------------------------------
bool __fastcall TExtColorBox::PickCustomColor()
{
  if(OnPickColor)
  {
    TColor Color = Selected;
    OnPickColor(this, Color);
    Selected = Color;
    Invalidate();
    return true;
  }

  if(ColorDialogType == cdtColorSelect)
  {
    std::auto_ptr<TIColorSelect> ColorSelect(new TIColorSelect(NULL));
    ColorSelect->Color = Selected;
    if(ColorSelect->Execute())
    {
      Selected = ColorSelect->Color;
      Invalidate();
      return true;
    }
    return false;
  }

  std::auto_ptr<TColorDialog> ColorDialog(new TColorDialog(NULL));
  ColorDialog->Color = Selected;
  if(ColorDialog->Execute())
  {
    Selected = ColorDialog->Color;
    Invalidate();
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::SetDroppedWidth(int AWidth)
{
  SendMessage(Handle, CB_SETDROPPEDWIDTH, AWidth, 0);
  FDroppedWidth = AWidth;
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::SetExtendedUI(bool Value)
{
  SendMessage(Handle, CB_SETEXTENDEDUI, Value, 0);
}
//---------------------------------------------------------------------------
bool __fastcall TExtColorBox::GetExtendedUI()
{
  return SendMessage(Handle, CB_GETEXTENDEDUI, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::SetSelected(TColor Value)
{
  FSelected = Value;
  if(ComponentState.Contains(csLoading) || Parent == NULL)
    return;

  int Index = -1;
  for(int I = 0; I < Items->Count; I++)
    if(reinterpret_cast<TColor>(Items->Objects[I]) == Value)
    {
      Index = I;
      break;
    }

  if(Index == -1 && ShowCustom)
  {
    ItemIndex = 0;
    Items->Objects[0] = reinterpret_cast<TObject*>(Value);
  }
  else
    ItemIndex = Index;
}
//---------------------------------------------------------------------------
TColor __fastcall TExtColorBox::GetSelected()
{
  return ItemIndex == -1 ? NoColorSelected : reinterpret_cast<TColor>(Items->Objects[ItemIndex]);
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::DrawItem(int Index, const Types::TRect &Rect, TOwnerDrawState State)
{
  Canvas->FillRect(Rect);
  TColor BackgroundColor = Canvas->Brush->Color;
  TRect LRect = Rect;
  if(Index < ShowCustom + ShowDefault)
    LRect.Right = LRect.Height() + LRect.Left;

  InflateRect(&LRect, -1, -1);
  TColor Color = reinterpret_cast<TColor>(Items->Objects[Index]);
  if(Color == clDefault)
    Color = clWhite;
  Canvas->Brush->Color = Color;
  Canvas->Pen->Color = clBlack;
  Canvas->Rectangle(LRect);
  Canvas->Brush->Color = BackgroundColor;

  if(Index < ShowCustom + ShowDefault)
  {
    LRect = TRect(LRect.Right + 5, Rect.Top, Rect.Right, Rect.Bottom);
    String Str = ShowCustom && Index == 0 ? CustomName : DefaultName;
    Canvas->TextRect(LRect, LRect.Left,
      (LRect.Bottom + LRect.Top - Canvas->TextHeight(Str)) / 2, Str);
  }
}
//---------------------------------------------------------------------------
void TExtColorBox::PopulateList()
{
  Clear();
  if(ShowCustom)
    AddItem("", reinterpret_cast<TObject*>(0x669900));
  if(ShowDefault)
    AddItem("", reinterpret_cast<TObject*>(clDefault));

  for(unsigned I = 0; I < sizeof(ColorList)/sizeof(ColorList[0]); I++)
    AddItem("", reinterpret_cast<TObject*>(ColorList[I]));
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::KeyPress(Char &Key)
{
  inherited::KeyPress(Key);
  if(Key == VK_RETURN && ShowCustom && ItemIndex == 0)
  {
    PickCustomColor();
    Key = 0;
  }
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::Select()
{
  if(ListSelected)
  {
    ListSelected = false;
    if(ShowCustom && ItemIndex == 0 && !PickCustomColor())
      return;
  }
  inherited::Select();
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::CloseUp()
{
  inherited::CloseUp();
  ListSelected = true;
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::KeyDown(unsigned short &Key, TShiftState Shift)
{
  ListSelected = False;
  inherited::KeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::DropDown()
{
  if(FAutoDroppedWidth && !DroppedWidthFound)
  {
    int MaxWidth = 0;
    if(ShowCustom)
      MaxWidth = std::max(MaxWidth, Canvas->TextWidth(CustomName));
    if(ShowDefault)
      MaxWidth = std::max(MaxWidth, Canvas->TextWidth(DefaultName));
    DroppedWidthFound = true;
    DroppedWidth = MaxWidth + Height + GetSystemMetrics(SM_CXVSCROLL) + 10;
  }
  inherited::DropDown();
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::ChangeScale(int M, int D)
{
  TCustomComboBox::ChangeScale(M, D); // Call inherited
  ItemHeight = (ItemHeight * M) / D;
}
//---------------------------------------------------------------------------
void __fastcall TExtColorBox::CreateWnd()
{
  TCustomComboBox::CreateWnd();
  PopulateList();
  Selected = FSelected;
}
//---------------------------------------------------------------------------


