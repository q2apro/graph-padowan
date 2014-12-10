//===========================================================================
// Copyright © 2009 Ivan Johansen
// GridPanel.cpp
//===========================================================================
//This is a fixed version of TGridPanel
//---------------------------------------------------------------------------
//Nasty hacks because of purly designed class
#define private public
#define protected public
#include <vcl.h>
#pragma hdrstop
#include <Math.hpp>
#include <vector>
#include "GridPanelEx.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TGridPanelEx));
static inline void ValidCtrCheck(TGridPanelEx *)
{
  new TGridPanelEx(NULL);
}
//---------------------------------------------------------------------------
namespace Gridpanelex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TGridPanelEx)};
     RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TGridPanelEx::TGridPanelEx(TComponent* Owner)
  : TGridPanel(Owner)
{
}
//---------------------------------------------------------------------------
void TGridPanelEx::RecalcCellDimensions(const TRect &Rect)
{
  // Subtract the size of absolute size columns and rows and also calculate the total size of
  // the percentage rows and columns
  // Also subtract the width of the widest control in each autosize column
  // or the tallest control in each autosize row and set the column or row size to that value
  int XSize = Rect.Right - Rect.Left;
  int YSize = Rect.Bottom - Rect.Top;
  double PercentX = 0.0;
  double PercentY = 0.0;
  int PercentXCount = 0;
  std::vector<int> MaxPos(FColumnCollection->Count + 1, 0);
  for(int I = 0; I < FColumnCollection->Count; I++)
  {
    if(FColumnCollection->Items[I]->SizeStyle == ssAbsolute)
      XSize -= FColumnCollection->Items[I]->Value;
    else if(FColumnCollection->Items[I]->SizeStyle == ssPercent)
    {
      PercentX += FColumnCollection->Items[I]->Value;
      PercentXCount++;
    }
    else
    {
      for(int J = 0; J < FRowCollection->Count; J++)
      {
        TControlItem *ControlItem = FControlCollection->ControlItems[I][J];
        if(ControlItem != NULL && ControlItem->Control != NULL &&
           ControlItem->Column == I && ControlItem->Row == J)
        {
          int LSize = ControlItem->Control->Margins->ControlWidth + Padding->Left + Padding->Right;
          int Size = MaxPos[I + ControlItem->ColumnSpan] - MaxPos[I];
          if(LSize > Size)
            MaxPos[I + ControlItem->ColumnSpan] = LSize + MaxPos[I];
        }
      }

      int Size = MaxPos[I+1] - MaxPos[I];
      XSize -= Size;
      FColumnCollection->Items[I]->Size = Size;
    }
  }

  std::vector<int>(FRowCollection->Count + 1, 0).swap(MaxPos);
  int PercentYCount = 0;
  for(int I = 0; I < FRowCollection->Count; I++)
  {
    if(FRowCollection->Items[I]->SizeStyle == ssAbsolute)
      YSize -= FRowCollection->Items[I]->Value;
    else if(FRowCollection->Items[I]->SizeStyle == ssPercent)
    {
      PercentY += FRowCollection->Items[I]->Value;
      PercentYCount++;
    }
    else
    {
      for(int J = 0; J < FColumnCollection->Count; J++)
      {
        TControlItem *ControlItem = FControlCollection->ControlItems[J][I];
        if(ControlItem != NULL && ControlItem->Control != NULL &&
           ControlItem->Column == J && ControlItem->Row == I)
        {
          int LSize = ControlItem->Control->Margins->ControlHeight + Padding->Top + Padding->Bottom;
          int Size = MaxPos[I + ControlItem->RowSpan] - MaxPos[I];
          if(LSize > Size)
            MaxPos[I + ControlItem->RowSpan] = MaxPos[I] + LSize;
        }
      }

      int Size = MaxPos[I+1] - MaxPos[I];
      YSize -= Size;
      FRowCollection->Items[I]->Size = Size;
    }
  }

  // Finally Calculate the size of the percentage-based columns and rows based on the remaining
  // X and Y sizes
  int RemainingX = XSize;
  int RemainingY = YSize;
  for(int I = 0; I < FColumnCollection->Count; I++)
  {
    TCellItem *Item = FColumnCollection->Items[I];
    if(Item->SizeStyle == ssPercent)
    {
      if(PercentX == 0)
        Item->Value = 100.0 / PercentXCount;
      else
        Item->Value = (Item->Value / PercentX) * 100.0;
      Item->Size = XSize * (Item->Value / 100.0);
      RemainingX -= Item->Size;
      if(RemainingX > 0 && I == FColumnCollection->Count - 1)
        Item->Size += RemainingX;
    }
  }

  for(int I = 0; I < FRowCollection->Count; I++)
  {
    TCellItem *Item = FRowCollection->Items[I];
    if(Item->SizeStyle == ssPercent)
    {
      if(PercentY == 0)
        Item->Value = 100.0 / PercentYCount;
      else
        Item->Value = (Item->Value / PercentY) * 100.0;
      Item->Size = YSize * (Item->Value / 100.0);
      RemainingY -= Item->Size;
      if(RemainingY > 0 && I == FRowCollection->Count - 1)
        Item->Size += RemainingY;
    }
  }
  FRecalcCellSizes = false;
}
//---------------------------------------------------------------------------
void __fastcall TGridPanelEx::UpdateControlOriginalParentSize(TControl *AControl, TPoint &AOriginalParentSize)
{
  //Always update grid cell sizes when a control changes its size
  if(/*FRecalcCellSizes &&*/ HandleAllocated())
  {
    TRect Rect = GetClientRect();
    AdjustClientRect(Rect);
    RecalcCellDimensions(Rect);
//    Realign();
  }
  int Index = FControlCollection->IndexOf(AControl);
  if(Index > -1)
  {
    TControlItem *ControlItem = FControlCollection->Items[Index];
    AOriginalParentSize = CellSize[ControlItem->Column][ControlItem->Row];
  }
  else
    TGridPanel::UpdateControlOriginalParentSize(AControl, AOriginalParentSize);
}
//---------------------------------------------------------------------------
void TGridPanelEx::ArrangeControlInCell(TControl *AControl, TRect CellRect, TAlignInfo AlignInfo)
{
  if(AControl->Align != alNone)
    ArrangeControl(AControl, Point(CellRect.Right - CellRect.Left, CellRect.Bottom - CellRect.Top),
      AControl->Align, AlignInfo, CellRect, True);
  else
  {
    TRect NewBounds;
    TAnchors AnchorSubset = AControl->Anchors * (TAnchors() << akLeft << akRight);
    int MaxWidth = Max(AControl->Margins->ControlWidth, AControl->Margins->ExplicitWidth);
    if(AnchorSubset == TAnchors() << akLeft)
      NewBounds.Left = CellRect.Left;
    else if(AnchorSubset == TAnchors() << akRight)
      NewBounds.Left = Max(CellRect.Left, (int)(CellRect.Right - MaxWidth));
    else
      NewBounds.Left = Max(CellRect.Left, (int)(CellRect.Left + ((CellRect.Right - CellRect.Left) - AControl->Margins->ControlWidth) / 2));
    NewBounds.Right = NewBounds.Left + Min(CellRect.Width(), MaxWidth);
    AnchorSubset = AControl->Anchors * (TAnchors() << akTop << akBottom);
    if(AnchorSubset == TAnchors() << akTop)
      NewBounds.Top = CellRect.Top;
    else if(AnchorSubset == TAnchors() << akBottom)
      NewBounds.Top = Max(CellRect.Top, (int)(CellRect.Bottom - AControl->Margins->ExplicitHeight));
    else
      NewBounds.Top = Max(CellRect.Top, (int)(CellRect.Top + ((CellRect.Bottom - CellRect.Top) - AControl->Margins->ControlHeight) / 2));
    NewBounds.Bottom = NewBounds.Top + Min(CellRect.Bottom - CellRect.Top, AControl->Margins->ExplicitHeight);
    AControl->Margins->SetControlBounds(NewBounds, True);
  }
}
//---------------------------------------------------------------------------
void TGridPanelEx::AdjustCellRect(TRect &Rect)
{
  Rect.Left += Padding->Left;
  Rect.Top += Padding->Top;
  Rect.Right -= Padding->Right;
  Rect.Bottom -= Padding->Bottom;
}
//---------------------------------------------------------------------------
void TGridPanelEx::ArrangeControls(TRect &Rect)
{
  TAlignInfo AlignInfo;
  AlignInfo.ControlIndex = 0;
  AlignInfo.AlignList = new TList;
  TRect CellRect;
  CellRect.Top = Rect.Top;
  bool RightToLeft = UseRightToLeftAlignment();
  for(int J = 0; J < FRowCollection->Count; J++)
  {
    if(RightToLeft) //Added by Ivan: Check for Right to Left
      CellRect.Right = Rect.Right;
    else
      CellRect.Left = Rect.Left;
    CellRect.Bottom = CellRect.Top + FRowCollection->Items[J]->Size;
    for(int I = 0; I < FColumnCollection->Count; I++)
    {
      TControlItem *ControlItem = FControlCollection->ControlItems[I][J];
      if(RightToLeft)
        CellRect.Left = CellRect.Right - FColumnCollection->Items[I]->Size;
      else
        CellRect.Right = CellRect.Left + FColumnCollection->Items[I]->Size;
      if(ControlItem != NULL && ControlItem->Control != NULL &&
         ControlItem->Column == I && ControlItem->Row == J)
      {
        AlignInfo.AlignList->Clear();
        AlignInfo.AlignList->Add(ControlItem->Control);
        AlignInfo.Align = ControlItem->Control->Align;
        TRect SpanRect = CellRect;
        if(ControlItem->ColumnSpan > 1)
          for(int K = I + 1; K < Min(I + ControlItem->ColumnSpan, FColumnCollection->Count); K++)
            if(RightToLeft)
              SpanRect.Left -= FColumnCollection->Items[K]->Size;
            else
              SpanRect.Right += FColumnCollection->Items[K]->Size;
        if(ControlItem->RowSpan > 1)
          for(int K = J + 1; K < Min(J + ControlItem->RowSpan, FRowCollection->Count); K++)
            SpanRect.Bottom += FRowCollection->Items[K]->Size;
        AdjustCellRect(SpanRect);
        ArrangeControlInCell(ControlItem->Control, SpanRect, AlignInfo);
      }
      if(RightToLeft)
        CellRect.Right = CellRect.Left;
      else
        CellRect.Left = CellRect.Right;
    }
    CellRect.Top = CellRect.Bottom;
  }
  delete AlignInfo.AlignList;
}
//---------------------------------------------------------------------------
void __fastcall TGridPanelEx::AlignControls(TControl *AControl, TRect &Rect)
{
  AdjustClientRect(Rect);
  if(FRecalcCellSizes)
    RecalcCellDimensions(Rect);
  if(ControlCount > 0)
    ArrangeControls(Rect);
  AdjustSize();
}
//---------------------------------------------------------------------------
bool __fastcall TGridPanelEx::CanAutoSize(int &NewWidth, int &NewHeight)
{
  TRect Rect = ClientRect;
  AdjustClientRect(Rect);
  int TempWidth = Width - Rect.Width();
  for(int I = 0; I < FColumnCollection->Count; I++)
  {
    TempWidth += FColumnCollection->Items[I]->Size;
    if(FColumnCollection->Items[I]->SizeStyle == ssPercent)
    {
      TempWidth = NewWidth;
      break;
    }
  }

  int TempHeight = Height - Rect.Height();
  for(int I = 0; I < FRowCollection->Count; I++)
  {
    TempHeight += FRowCollection->Items[I]->Size;
    if(FRowCollection->Items[I]->SizeStyle == ssPercent)
    {
      TempHeight = NewHeight;
      break;
    }
  }

  NewWidth = TempWidth;
  NewHeight = TempHeight;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TGridPanelEx::ChangeScale(int M, int D)
{
  //Scale all the absolute values and leave the rest alone
  for(int I = 0; I < RowCollection->Count; I++)
    if(RowCollection->Items[I]->SizeStyle == ssAbsolute)
      RowCollection->Items[I]->Value = MulDiv(RowCollection->Items[I]->Value, M, D);

  for(int I = 0; I < ColumnCollection->Count; I++)
    if(ColumnCollection->Items[I]->SizeStyle == ssAbsolute)
      ColumnCollection->Items[I]->Value = MulDiv(ColumnCollection->Items[I]->Value, M, D);

  TGridPanel::ChangeScale(M, D);
}
//---------------------------------------------------------------------------

