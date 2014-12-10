//===========================================================================
// Copyright © 2003 Ivan Johansen
// Grid.cpp
//===========================================================================
#include "Config.h"
#pragma hdrstop
#define VCL_IOSTREAM
#include <vcl.h>
#include <Clipbrd.hpp>
#include "Grid.h"
#include <fstream>
#include <string>
#include <sstream>
#include "HandleCsv.h"
#include <algorithm>
#include "ICompCommon.h"
#include <cassert>
#pragma package(smart_init)
#pragma resource "cursors.res" //Necesarry to ensure the res file is linked into the exe file and not only the package

const TCursor crColCursor = static_cast<TCursor>(256); //Cursor used to select colums
const TCursor crRowCursor = static_cast<TCursor>(257); //Cursor used to select rows
static TRegisterClass Dummy(__classid(TGrid));
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TGrid *)
{
  new TGrid(NULL);
}
//---------------------------------------------------------------------------
namespace Grid
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TGrid)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TGrid::TGrid(TComponent* Owner)
 : TDrawGrid(Owner), FSelectCols(true), FSelectRows(true), FPopupMenu(NULL),
   LeftButtonPressed(false), FAutoAddRows(false), FOnEditorKeyPress(NULL),
   FEditorPopupMenu(NULL), FOnGetText(NULL), FOnSetText(NULL),
   FExportFixedRows(true)
{
  //Initialize to invalid value
  SelStart.X = -1;
  SelStart.Y = -1;
  HCURSOR ColCursor = LoadCursor(HInstance, L"COL");
  HCURSOR RowCursor = LoadCursor(HInstance, L"ROW");
  Screen->Cursors[crColCursor] = ColCursor;
  Screen->Cursors[crRowCursor] = RowCursor;
	assert(ColCursor);
	assert(RowCursor);
  FTitleCaptions->OnChange = TitleCaptionsChange;
}
//---------------------------------------------------------------------------
//This creates the inplace editor
//TMyInplaceEdit is a decident of TInplaceEdit where the mouse
//events are public
TInplaceEdit* __fastcall TGrid::CreateEditor(void)
{
  TMyInplaceEdit* Editor = new TMyInplaceEdit(this);
  Editor->OnMouseMove = &InplaceEditMouseMove;
  Editor->OnMouseUp = &InplaceEditMouseUp;
  Editor->OnMouseDown = &InplaceEditMouseDown;
  Editor->OnEnter = &InplaceEditEnter;
  Editor->OnExit = &InplaceEditExit;
  Editor->PopupMenu = EditorPopupMenu;
  return Editor;
}
//---------------------------------------------------------------------------
//This is called when the mouse keys are pressed while the mouse is over
//the inplace editor
void __fastcall TGrid::InplaceEditMouseDown(TObject* Sender,TMouseButton Button,TShiftState Shift, int X, int Y)
{
  if(OnMouseDown)
    OnMouseDown(this,Button,Shift,X+InplaceEditor->Left,Y+InplaceEditor->Top);
  TGridRect GridRect;
  if(Button==mbLeft)
  {
    SelStart.X=Col; //Save start position for selection
    SelStart.Y=Row;
    int ACol;
    int ARow;
    MouseToCell(X,Y,ACol,ARow); //Get cell under mouse cursor

    switch(State)
    {
      case crRowCursor:
        GridRect.Left=FixedCols;
        GridRect.Top=ARow;
        GridRect.Right=ColCount-1;
        GridRect.Bottom=ARow;
        Options = Options >> goEditing;
        EditorMode = false;
        Selection = GridRect;
        break;

      case crColCursor:
        GridRect.Left=ACol;
        GridRect.Top=FixedRows;
        GridRect.Right=ACol;
        GridRect.Bottom=RowCount-1;
        Options = Options >> goEditing;
        EditorMode = false;
        Selection = GridRect;
        break;
    }
  }
}
//---------------------------------------------------------------------------
//This is called when the mouse is moved over the inplace editor or the
//inplace editor has captured the mouse
void __fastcall TGrid::InplaceEditMouseMove(TObject* Sender,TShiftState Shift, int X, int Y)
{
  MouseMove(Shift,X+InplaceEditor->Left,Y+InplaceEditor->Top);
  if(Shift.Contains(ssLeft))
    return;
  InplaceEditor->Cursor=Cursor;
  State=0;
  if(X+InplaceEditor->Left<10)
  {
    InplaceEditor->Cursor = crRowCursor;
    State = crRowCursor;
  }
  else                                                   
    if(Y+InplaceEditor->Top<10)
    {
      InplaceEditor->Cursor = crColCursor;
      State = crColCursor;
      EmptySelection();
    }
}
//---------------------------------------------------------------------------
//This is called when the mouse keys are released while the inplace
//editor has captured the mouse
void __fastcall TGrid::InplaceEditMouseUp(TObject* Sender,TMouseButton Button,TShiftState Shift, int X, int Y)
{
  MouseUp(Button,Shift,X+InplaceEditor->Left,Y+InplaceEditor->Top);
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::KeyDown(Word &Key, TShiftState Shift)
{
  TInplaceEdit::KeyDown(Key, Shift);

  //Workaround for Windows bug: TInplaceEdit will not move to the next cell when Right arow key is pressed
  //if the current cell only contains the letter "i", "l", "f", or "t"
  if(Key == VK_RIGHT)
    if(static_cast<TGrid*>(Grid)->Options.Contains(goAlwaysShowEditor) && (Shift.Contains(ssCtrl) || SelStart == Text.Length()))
      static_cast<TGrid*>(Grid)->KeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::KeyDown(Word &Key, Classes::TShiftState Shift)
{
  int OldCol = Col;
  int OldRow = Row;
  int OldSelStart = InplaceEditor ? InplaceEditor->SelStart : 0;

  if(LeftButtonPressed)
  {
    Key = 0;
    return;
  }

  if((Key == VK_DOWN || Key == VK_TAB) && EditorMode)
    AjustRows();

  TDrawGrid::KeyDown(Key, Shift);

  if(Key == VK_DELETE && Editing)
    ClearSelection();

  else if(Key==VK_LEFT || Key==VK_RIGHT || Key==VK_UP || Key==VK_DOWN)
    if(Shift.Contains(ssShift))
    {
      Options = Options >> goEditing;
      EditorMode = false;
    }
    else
      if(Editing)
      {
        Options = Options << goEditing;
        EditorMode = true;
        EmptySelection();
      }

  if(InplaceEditor)
    switch(Key)
    {
      case VK_LEFT:
        if(OldCol==FixedCols)
          InplaceEditor->SelStart=0;
        break;
      case VK_UP:
        if(OldRow==FixedRows)
          InplaceEditor->SelStart=OldSelStart;
        break;
      case VK_RIGHT:
        if(OldCol==ColCount-1)
          InplaceEditor->SelStart = OldSelStart;
        else
          InplaceEditor->SelStart = 0;
        break;
      case VK_DOWN:
        if(OldRow==RowCount-1)
          InplaceEditor->SelStart=OldSelStart;
    }

  //If CTRL-TAB has been pressed
  if(Shift.Contains(ssCtrl) && Key == VK_TAB && Parent)
    PostMessage(Parent->Handle, WM_NEXTDLGCTL, Shift.Contains(ssShift), 0);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::KeyPress(Char &Key)
{
  TDrawGrid::KeyPress(Key);
  if(LeftButtonPressed)
    Key = 0;
  if(Editing && !EditorMode)
  {
    FOnEditorKeyPress(InplaceEditor, Key);
    if(Key)
    {
      Options = Options << goEditing;
      EditorMode = true;
      EmptySelection();
      InplaceEditor->Text = Key;
      InplaceEditor->SelStart = 1;
    }
  }
}
//---------------------------------------------------------------------------
//This is called then the mouse keys are pressed while the mouse is over
//the control; Not called while the mouse is over inplace editor
void __fastcall TGrid::MouseDown(TMouseButton Button,TShiftState Shift,int X,int Y)
{
  int XCell,YCell;

  if(Button == mbLeft)
    LeftButtonPressed = true;

  MouseToCell(X, Y, XCell, YCell); //Calc cell for mouse pos

  //If left button pressed and inside non fixed cell
  if(Button == mbLeft && XCell >= FixedCols && YCell >= FixedRows && !Shift.Contains(ssShift))
  {
    SelStart.X = XCell; //Save start position for selection
    SelStart.Y = YCell;
    EditorMode = true; //Enable editing
  }

  TGridRect GridRect;
  //Check if we are selcting rows or coloums
  if(Button == mbLeft)
    switch(State)
    {
      case crRowCursor:
        //Calling inherited MouseDown() may screw up the selection of columns and rows
        if(OnMouseDown)
          OnMouseDown(this, Button, Shift, X, Y);
        GridRect.Left = FixedCols;
        GridRect.Top = YCell;
        GridRect.Right = ColCount-1;
        GridRect.Bottom = YCell;
        SelStart.X = FixedCols;
        SelStart.Y = YCell;
        Options = Options >> goEditing;
        EditorMode = false;
        Selection = GridRect;
        SetTimer(Handle,1,60,NULL);
        InvalidateRow(YCell);
        break;

      case crColCursor:
        //Calling inherited MouseDown() may screw up the selection of columns and rows
        if(OnMouseDown)
          OnMouseDown(this, Button, Shift, X, Y);
        GridRect.Left=XCell;
        GridRect.Top=FixedRows;
        GridRect.Right=XCell;
        GridRect.Bottom=RowCount-1;
        SelStart.X = XCell;
        SelStart.Y = FixedRows;
        Options = Options >> goEditing;
        EditorMode=false;
        Selection=GridRect;
        SetTimer(Handle,1,60,NULL);
        InvalidateCol(XCell);
        break;

      default:
        TDrawGrid::MouseDown(Button, Shift, X, Y); //Call inherited
        if(!Editing || XCell < FixedCols || YCell < FixedRows)
        {
          SelStart.X = SelStart.Y = -1;
          break;
        }
        Options = Options << goEditing;
        EditorMode = true;
        InplaceEditor->SelectAll();
    }
  else
    TDrawGrid::MouseDown(Button, Shift, X, Y); // Call inherited
}
//---------------------------------------------------------------------------
//This is called then the mouse is moved over the control or the inplace
//editor has captured the mouse
void __fastcall TGrid::MouseMove(TShiftState Shift, int X, int Y)
{
  //Check if left mouse button is held down
  //Check for valid selection; When the dialog is shown while the mouse button is down MouseDown is not received
  if(Shift.Contains(ssLeft) && SelStart.X >= 0 && SelStart.Y >= 0)
  {
    TGridRect GridRect;
    TGridRect Empty;
    GridRect.TopLeft = SelStart; //Set start pos for selection

    //Calc the cell position for the mouse
    MouseToCell(X,Y,GridRect.Right,GridRect.Bottom);

    TRect Cell = CellRect(LeftCol, TopRow);

    switch(State)
    {
      case crRowCursor:
        if(GridRect.Bottom < FixedRows)
          break;
        GridRect.Right = ColCount-1;
        Selection = GridRect;
        //Invalidate grid; InvalidateCol(ColCount-1) doesn't seem to work
        ::InvalidateRect(Handle, &ClientRect, false);
        break;

      case crColCursor:
        if(GridRect.Right < FixedCols)
          break;
        GridRect.Bottom = RowCount-1;
        Selection = GridRect;
        //Invalidate grid; InvalidateRow(RowCount-1) doesn't seem to work
        ::InvalidateRect(Handle, &ClientRect, false);
        break;

      default:
        if(Options.Contains(goRangeSelect) &&
           GridRect.Right>=FixedCols && GridRect.Bottom>=FixedRows &&
           SelStart.X>=FixedCols && SelStart.Y>=FixedRows)
        {
          //Set timer and indicate that selecting has been started.
          //This is done by MouseDown in TCustomGrid if editing wasn't enabled.
          SetTimer(Handle,1,60,NULL);
          FGridState=gsSelecting;

          //If the mouse is not moved over the cell there the selection was started
          if(GridRect.Top!=GridRect.Bottom || GridRect.Left!=GridRect.Right)
          {
            CursorPos = InplaceEditor->SelStart;
            Options = Options >> goEditing; //Diasble editing
            EditorMode=false;
          }
          //If the mouse is over the cell there the selectionwas started and
          //we are not already in editormode and editing is allowed
          else if(!EditorMode && Editing)
          {
            Options = Options << goEditing; //Enable editing
            EditorMode=true; //Show inplace editor
            InplaceEditor->SelStart = CursorPos;
          }
        }
      }
  }
  TDrawGrid::MouseMove(Shift, X, Y); //Call inherited
}
//---------------------------------------------------------------------------
void __fastcall TGrid::MouseUp(TMouseButton Button,TShiftState Shift,int X,int Y)
{
  if(Button == mbLeft)
  {
    LeftButtonPressed = false;
    if(State==crRowCursor || State==crColCursor)
      //Inhirited MouseUp may screw up when selecting rows or columns
      KillTimer(Handle,1);
  }
  TDrawGrid::MouseUp(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
//This function is called when the mouse is moved and it is not captured
void __fastcall TGrid::WMSetCursor(TMessage &Message)
{
  TGridDrawInfo DrawInfo;
  TGridState GridState;
  int Index,Pos,Ofs;
  CalcDrawInfo(DrawInfo);
  CalcSizingState(HitTest.x, HitTest.y, GridState, Index, Pos, Ofs, DrawInfo);

  //Check if we are in design mode or the cursor is at a position indicating that
  //the user wants to change the size of the row or column
  if(ComponentState.Contains(csDesigning) || GridState==gsColSizing || GridState==gsRowSizing)
  {
    TDrawGrid::Dispatch(&Message);
    State=0;
    return;
  }

  int ACol;
  int ARow;
  MouseToCell(HitTest.x,HitTest.y,ACol,ARow);
  //Check if the select rows cursor should be shown
  if(SelectRows && HitTest.x>0 && HitTest.x<10 && ARow>=FixedRows && HitTest.y < ClientHeight)
  {
    ::SetCursor(Screen->Cursors[crRowCursor]);
    State=crRowCursor;
  }
  else
    //Check if the select columns cursor should be shown
    if(SelectCols && HitTest.y<10 && ACol>=FixedCols && HitTest.x < ClientWidth)
    {
      ::SetCursor(Screen->Cursors[crColCursor]);
      State=crColCursor;
    }
    else
    {
      //Call inherited
      TDrawGrid::Dispatch(&Message);
      State=0;
    }
}
//---------------------------------------------------------------------------
//This function selects all cells in the grid
void TGrid::SelectAll()
{
  TGridRect GridRect;
  GridRect.Left = FixedCols;
  GridRect.Top = FixedRows;
  GridRect.Right = ColCount-1;
  GridRect.Bottom = RowCount-1;
  Selection = GridRect;
  EditorMode = false;
  Invalidate();
}
//---------------------------------------------------------------------------
//This function clears all cells in the grid
void TGrid::ClearAll()
{
  if(AutoAddRows)
    RowCount = FixedRows+1;
  for(int ARow = FixedRows; ARow < RowCount; ARow++)
    for(int ACol = FixedCols; ACol < ColCount; ACol++)
      DoSetText(ACol, ARow, "");
}
//---------------------------------------------------------------------------
//This function celars all selected cells in the grid
void TGrid::ClearSelection()
{
  if(EditorMode)
  {
    InplaceEditor->ClearSelection();
    return;
  }
  TGridRect GridRect = Selection;
  for(int ARow = GridRect.Top;ARow <= GridRect.Bottom; ARow++)
    for(int ACol = GridRect.Left;ACol <= GridRect.Right; ACol++)
      DoSetText(ACol, ARow, "");
  AjustRows();
}
//---------------------------------------------------------------------------
void TGrid::CopyTextToClipboard(wchar_t DecimalSeparator)
{
  Clipboard()->AsText = ExportText('\t', DecimalSeparator);
}
//---------------------------------------------------------------------------
String TGrid::ExportText(wchar_t Delimiter, wchar_t DecimalSeparator)
{
  String Str;
  TGridRect GridRect = Selection;
  if(GridRect.Top == GridRect.Bottom && GridRect.Left == GridRect.Right)
    GridRect = GetCompleteGridRect();

  //If complete coloumns are selected, also copy the fixed row
  if(FExportFixedRows)
    if(GridRect.Top == FixedRows && GridRect.Bottom == RowCount-1)
      GridRect.Top = 0;

  //Ignore the last row if it is empty
  bool Empty = true;
  for(int ACol = GridRect.Left; ACol <= GridRect.Right; ACol++)
    if(!DoGetText(ACol, GridRect.Bottom).IsEmpty())
      Empty = false;

  if(Empty)
    GridRect.Bottom--;

  for(int ARow = GridRect.Top; ARow <= GridRect.Bottom; ARow++)
  {
    for(int ACol = GridRect.Left; ACol <= GridRect.Right; ACol++)
      if(ColWidths[ACol] > 0)
      {
        if(ACol != GridRect.Left)
          Str += Delimiter;
        Str += DoGetText(ACol, ARow);
      }
    Str += "\r\n";
  }

  //Replace decimal separator if it is different than '.'
  if(DecimalSeparator != 0 && DecimalSeparator != '.')
    for(int I = 1; I <= Str.Length(); I++)
      if(Str[I] == '.')
        Str[I] = DecimalSeparator;
  return Str;
}
//---------------------------------------------------------------------------
//This function copies all selected cells to the clipboard
void TGrid::CopyToClipboard(wchar_t DecimalSeparator)
{
  if(EditorMode)
  {
    InplaceEditor->CopyToClipboard();
    return;
  }
  Clipboard()->Open();
  CopyTextToClipboard(DecimalSeparator);
  CopyRtfToClipboard(DecimalSeparator);
  Clipboard()->Close();
}
//---------------------------------------------------------------------------
//This function cuts all selected cells to the clipboard
void TGrid::CutToClipboard(wchar_t DecimalSeparator)
{
  CopyToClipboard(DecimalSeparator);
  ClearSelection();
}
//---------------------------------------------------------------------------
//This function pastes text from the clipboard and places it in the cells
//begining with the top left cell of the selection
void TGrid::PasteFromClipboard(wchar_t DecimalSeparator, wchar_t Separator)
{
	ImportText(Clipboard()->AsText, DecimalSeparator, Separator);
}
//---------------------------------------------------------------------------
void TGrid::ImportText(String Str, wchar_t DecimalSeparator, wchar_t Separator)
{
  //If there is only one cell to insert: Paste at cursor position
  if(Str.Pos('\n') == 0 && EditorMode)
  {
    InplaceEditor->SelText = Str;
    return;
  }
  std::istringstream Stream(AnsiString(Str).c_str());
  Import(Stream, DecimalSeparator, Separator);
}
//---------------------------------------------------------------------------
void TGrid::EmptySelection()
{
  TGridRect GridRect;
  GridRect.Left = Col;
  GridRect.Top = Row;
  GridRect.Right = Col;
  GridRect.Bottom = Row;
  Selection = GridRect;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetOptions(TGridOptions GridOptions)
{
  Editing = GridOptions.Contains(goAlwaysShowEditor);
  TDrawGrid::Options = GridOptions;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::WMTimer(TMessage &Message)
{
  TPoint CursorPos;
  GetCursorPos(&CursorPos);
  CursorPos = ScreenToClient(CursorPos);
  TGridRect GridRect;
  GridRect.TopLeft = SelStart;
  TRect Cell = CellRect(LeftCol,TopRow);
  switch(State)
  {
    case crRowCursor:
      GridRect.Right = ColCount;
      if(TopRow < (RowCount - VisibleRowCount) && CursorPos.y > ClientHeight)
      {
        TopRow = TopRow + 1;
        GridRect.Bottom = TopRow + VisibleRowCount - 1;
        Selection = GridRect;
      }
      else
        if(CursorPos.y < Cell.Top)
        {
          if(TopRow > FixedRows)
            TopRow = TopRow - 1;
          GridRect.Bottom = TopRow;
          Selection = GridRect;
        }
      //Invalidate grid; InvalidateCol() doesn't seem to work, and the last marked
      //cell may need to be invalidated too.
      ::InvalidateRect(Handle, &ClientRect, false);
      break;
    case crColCursor:
      GridRect.Bottom = RowCount;
      if(LeftCol > FixedCols && CursorPos.x < Cell.Left)
      {
        LeftCol = LeftCol - 1;
        GridRect.Right = LeftCol;
        Selection = GridRect;
      }
      else
        if(CursorPos.x > ClientWidth && LeftCol < (ColCount - VisibleColCount))
        {
          LeftCol = LeftCol + 1;
          GridRect.Right = LeftCol + VisibleColCount;
          Selection = GridRect;
        }
      //Invalidate grid; InvalidateRow() doesn't seem to work, and the last marked
      //cell may need to be invalidated too.
      ::InvalidateRect(Handle, &ClientRect, false);
      break;
    default:
      TDrawGrid::Dispatch(&Message);
  }
}
//---------------------------------------------------------------------------
__fastcall TMyInplaceEdit::TMyInplaceEdit(TComponent *AOwner)
  : TInplaceEdit(AOwner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::WMSetFocus(TMessage &Message)
{
  if(OnEnter)
    OnEnter(this);
  TInplaceEdit::Dispatch(&Message);
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::WMKillFocus(TMessage &Message)
{
  if(OnExit)
    OnExit(this);
  TInplaceEdit::Dispatch(&Message);
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::WMDeadChar(TMessage &Message)
{
  //I am not sure why this works, but it does
  if(Message.WParam == '^')
    PostMessage(Handle, WM_KEYUP, VK_SPACE, 0);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::InplaceEditEnter(TObject *Sender)
{
  TDrawGrid::PopupMenu = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::InplaceEditExit(TObject *Sender)
{
  TDrawGrid::PopupMenu = FPopupMenu;
  AjustRows();
}
//---------------------------------------------------------------------------
void __fastcall TGrid::DblClick()
{
  TGridDrawInfo DrawInfo;
  TGridState State;
  int Index, Pos, Ofs;
  CalcDrawInfo(DrawInfo);
  CalcSizingState(HitTest.x, HitTest.y, State, Index, Pos, Ofs, DrawInfo);
  if(State == gsColSizing)
    AutoSizeCol(Index);
}
//---------------------------------------------------------------------------
void TGrid::AutoSizeCol(int ColIndex)
{
  if(ColIndex < FixedCols || ColIndex >= ColCount)
    return;
  int ColWidth = MinColWidth;
  for(int ARow = 0; ARow < RowCount; ARow++)
  {
    int CellWidth = Canvas->TextWidth(DoGetText(ColIndex, ARow)) + 5;
    if(CellWidth > ColWidth)
      ColWidth = CellWidth;
  }
  ColWidths[ColIndex] = ColWidth;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetPopupMenu(TPopupMenu *Menu)
{
  FPopupMenu = Menu;
  if(InplaceEditor && InplaceEditor->Focused())
    TDrawGrid::PopupMenu = NULL;
  else
    TDrawGrid::PopupMenu = Menu;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::ColWidthsChanged()
{
  for(int I = FixedCols; I < ColCount; I++)
    if(ColWidths[I] < MinColWidth && ColWidths[I] > 0) //Allow ColWidths=0 to hide coloumn
      ColWidths[I] = MinColWidth;
  TDrawGrid::ColWidthsChanged();
}
//---------------------------------------------------------------------------
void __fastcall TGrid::RowHeightsChanged()
{
  for(int I = FixedRows; I < RowCount; I++)
    if(RowHeights[I] < MinRowHeight)
      RowHeights[I] = MinRowHeight;
  TDrawGrid::RowHeightsChanged();
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetMinRowHeight(int Value)
{
  if(Value >=0)
  {
    FMinRowHeight = Value;
    if(DefaultRowHeight < Value)
    {
      DefaultRowHeight = Value;
      for(int I = FixedRows; I < RowCount; I++)
        if(RowHeights[I] < Value)
          RowHeights[I] = Value;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetMinColWidth(int Value)
{
  if(Value >=0)
  {
    FMinColWidth = Value;
    if(DefaultColWidth < Value)
    {
      DefaultColWidth = Value;
      for(int I = FixedCols; I < ColCount; I++)
        if(ColWidths[I] < Value)
          ColWidths[I] = Value;
    }
  }
}
//---------------------------------------------------------------------------
void TGrid::CopyRtfToClipboard(wchar_t DecimalSeparator)
{
  TGridRect GridRect = Selection;
  //If complete coloumns are selected, also copy the fixed row
  if(GridRect.Top == FixedRows && GridRect.Bottom == RowCount-1)
    GridRect.Top = 0;

  //Position of the leftmost edge of the table with respect to the left edge of its column
  int LeftEdge = 100;

  //Font name and size
  AnsiString FontName = "MS Sans Serif";
  int FontSize = 10;

  //String with rtf informations for grid
  AnsiString Str = "{\\rtf1\n";

  //Create font table
  Str += "{\\fonttbl{\\f0\\fswiss\\fprq2\\fcharset0 " + FontName + ";}}\n";

  //Select font
  Str += "\\f0\\fs" + AnsiString(2*FontSize) + "\n";

  //Start table
  Str += "\\trowd\\trgaph70\\trleft-100\\trbrdrt\\brdrs\\brdrw10\n";

  //Position of right side of coloumn
  int ColPos = -LeftEdge;

  Canvas->Font->Size = FontSize;

  //Create coloumns
  for(int ACol = GridRect.Left; ACol <= GridRect.Right; ACol++)
  {
    //Find the necesarry with of the coloumn
    int ColWidth = 32;
    for(int ARow = GridRect.Top; ARow <= GridRect.Bottom; ARow++)
    {
      int CellWidth = Canvas->TextWidth(DoGetText(ACol, ARow)) + 25;
      if(CellWidth > ColWidth)
        ColWidth = CellWidth;
    }

    //Position of right side of column in twix; 1 twix = 1/12 pixel
    ColPos += ColWidth * 12 + 2 * LeftEdge;
    Str += "\\clbrdrt\\brdrw15\\brdrs\\clbrdrl\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\cellx" + AnsiString(ColPos) + "\n";
  }

  //Fill rows
  for(int ARow = GridRect.Top; ARow <= GridRect.Bottom; ARow++)
  {
    Str += "\\intbl";
    for(int ACol = GridRect.Left; ACol <= GridRect.Right; ACol++)
    {
      Str += " ";
      String CellStr = DoGetText(ACol, ARow);
      //Convert all non-ascii characters to Unicode encoding
      for(int I = 1; I <= CellStr.Length(); I++)
        if(CellStr[I] < 128)
          Str += CellStr[I];
        else
          Str += "{\\u" + IntToStr(CellStr[I]) + "}";
      Str += "\\cell";
    }
    Str += "\\row\n";
  }

  //End rtf text
  Str += "}";

  //Replace decimal separator if it is different than '.'
  if(DecimalSeparator != 0 && DecimalSeparator != '.')
    for(int I = 1; I <= Str.Length(); I++)
      if(Str[I] == '.')
        Str[I] = DecimalSeparator;

  //Copy rtf text to clipboard
  unsigned int RtfFormat = RegisterClipboardFormat(L"Rich Text Format");
  SetClipboardData(RtfFormat, Str.c_str(), Str.Length()+1);
}
//---------------------------------------------------------------------------
void TGrid::SetClipboardData(unsigned int Format, void *Data, unsigned int DataSize)
{
  HGLOBAL DataHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, DataSize);
  if(DataHandle == 0)
    return;
  void *Ptr = GlobalLock(DataHandle);
  memcpy(Ptr, Data, DataSize);
  GlobalUnlock(DataHandle);
  Clipboard()->SetAsHandle(Format, (unsigned int)DataHandle);
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::KeyPress(Char &Key)
{
  //Don't call OnEditorKeyPress() for Ctrl-C, Ctrl-V, etc.
  if(!iscntrl(Key) || Key == '\n' || Key == '\r')
    if(TGrid *MyGrid = dynamic_cast<TGrid*>(Grid))
      if(MyGrid->OnEditorKeyPress)
        MyGrid->OnEditorKeyPress(this, Key);

  switch(Key)
  {
    case VK_ESCAPE:
      //Simulate escape key pressed when grid has focus. This usually activates
      //the button with the Cancel property set to true.
      Grid->Perform(CN_KEYDOWN, 27, 65537);
      break;

    case '\n':
      Key = 0; //Ignore Ctrl-Enter
      break;

    case '\r':
      if(TGrid *MyGrid = dynamic_cast<TGrid*>(Grid))
      {
        MyGrid->AjustRows();
        MyGrid->NextCell();
        Key = 0;
      }
      break;

    case 0xB2: //Superscript 2
      SendMessage(Handle, WM_CHAR, '^', 0);
      Key = '2';
      break;

    default:
      //Call inherited
	  TInplaceEdit::KeyPress(Key);
  }
}
//---------------------------------------------------------------------------
void TGrid::NextCell()
{
  //Move focus to next column. If last column, move to next row
  if(Col < ColCount-1)
    Col = Col + 1;
  else
    if(Row < RowCount - 1)
    {
      Row = Row + 1;
      Col = FixedCols;
    }
}
//---------------------------------------------------------------------------
void TGrid::LastCell()
{
  //Move focus one column to the left. If first column, move one row up
  if(Col > FixedCols)
    Col = Col - 1;
  else
    if(Row > FixedRows)
    {
      Row = Row - 1;
      Col = ColCount-1;
    }
}
//---------------------------------------------------------------------------
void TGrid::AjustRows()
{
  if(FAutoAddRows)
  {
    //Remove all empty rows except one
    int NewRowCount = RowCount + 1;
    for(int ARow = RowCount-1; ARow >= FixedRows; ARow--)
    {
      for(int ACol = 0; ACol < ColCount; ACol++)
        if(!DoGetText(ACol, ARow).IsEmpty())
        {
          RowCount = NewRowCount;
          return;
        }
      NewRowCount--;
    }
    RowCount = FixedRows + 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMyInplaceEdit::WMPaste(TMessage &Message)
{
  if(TGrid *MyGrid = dynamic_cast<TGrid*>(Grid))
    MyGrid->PasteFromClipboard(0);
}
//---------------------------------------------------------------------------
void TGrid::InsertRows(int Index, int Count)
{
  if(Index >= RowCount)
    Index = RowCount-1;
  else if(Index < FixedRows)
    Index = FixedRows;
  if(AutoAddRows && Index == RowCount-1)
    return;
  RowCount = RowCount + Count;

  for(int Row = RowCount-Count-1; Row >= Index+Count-1 ; Row--)
    for(int Col = 0; Col < ColCount; Col++)
      DoSetText(Col, Row+Count, DoGetText(Col, Row));

  for(int Row = Index+Count-1; Row >= Index ; Row--)
    for(int Col = 0; Col < ColCount; Col++)
      DoSetText(Col, Row, String());
}
//---------------------------------------------------------------------------
void TGrid::RemoveRows(int Index, int Count)
{
  if(Index >= RowCount)
    return;
  else if(Index <= FixedRows)
    Index = FixedRows;
  if(Index+Count > RowCount)
    Count = RowCount - Index;
  for(int Row = Index; Row < RowCount-Count; Row++)
    for(int Col = 0; Col < ColCount; Col++)
      DoSetText(Col, Row, DoGetText(Col, Row+Count));

  if(AutoAddRows && RowCount-Count == FixedRows)
  {
    for(int Col = 0; Col < ColCount; Col++)
      DoSetText(Col, FixedRows, String());
    RowCount = FixedRows+1;
  }
  else
    RowCount = RowCount - Count;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetEditorPopupMenu(TPopupMenu *Menu)
{
  FEditorPopupMenu = Menu;
  if(TMyInplaceEdit *Edit = dynamic_cast<TMyInplaceEdit*>(InplaceEditor))
    Edit->PopupMenu = Menu;
}
//---------------------------------------------------------------------------
bool TGrid::CanCopy()
{
  if(EditorMode)
    return InplaceEditor->SelLength;
  return true;
}
//---------------------------------------------------------------------------
bool TGrid::ImportFromFile(const String &FileName, wchar_t DecimalSeparator, wchar_t Separator)
{
	std::ifstream Stream(FileName.c_str());
	if(!Stream)
		return false;

	Import(Stream, DecimalSeparator, Separator);
	return true;
 }
//---------------------------------------------------------------------------
void TGrid::Import(std::istream &Stream, wchar_t DecimalSeparator, wchar_t Separator)
{
  TCsvGrid CsvGrid;
  if(Separator == 0)
  {
    std::string Separators = DefaultSeparators;
    size_t pos = Separators.find(DecimalSeparator);
    if(pos != std::string::npos)
      Separators.erase(pos);
    ImportCsv(Stream, CsvGrid, Separators);
  }
  else
    ImportCsv(Stream, CsvGrid, Separator);
  unsigned ACol = Selection.Left;
  unsigned ARow = Selection.Top;
  if(AutoAddRows && static_cast<unsigned>(RowCount) <= ARow + CsvGrid.size())
    RowCount = ARow + CsvGrid.size();
  bool OldAutoAddRows = AutoAddRows;
  AutoAddRows = false;

  for(unsigned Row = 0; Row < CsvGrid.size(); Row++)
    for(unsigned Col = 0; Col < CsvGrid[Row].size(); Col++)
    {
      std::string &Str = CsvGrid[Row][Col];
      if(DecimalSeparator != L'.')
        std::replace(Str.begin(), Str.end(), (char)DecimalSeparator, '.');
      if((int)Row < RowCount && (int)Col < ColCount)
        DoSetText(Col+ACol, Row+ARow, Str.c_str());
    }

  AutoAddRows = OldAutoAddRows;
  AjustRows();
}
//---------------------------------------------------------------------------
bool TGrid::ExportToFile(const String &FileName, wchar_t Delimiter, wchar_t DecimalSeparator, bool Utf8)
{
  //Save binary; \r\n is already used as line feed
  std::ofstream Stream(FileName.c_str(), std::ios::binary);
  if(!Stream)
    return false;
  if(Utf8)
  {
    Stream << "\xEF\xBB\xBF"; //Start with Byte Order Marker
    Stream << UTF8Encode(ExportText(Delimiter, DecimalSeparator)).c_str();
  }
  else
    Stream << AnsiString(ExportText(Delimiter, DecimalSeparator)).c_str();
  return true;
}
//---------------------------------------------------------------------------
TGridRect TGrid::GetCompleteGridRect()
{
  TGridRect GridRect;
  GridRect.Top = FixedRows;
  GridRect.Left = FixedCols;
  GridRect.Bottom = RowCount - 1;
  GridRect.Right = ColCount - 1;
  return GridRect;
}
//---------------------------------------------------------------------------
String TGrid::DoGetText(unsigned ACol, unsigned ARow)
{
  String Value;
  if(FOnGetText)
    FOnGetText(this, ACol, ARow, Value);
  else if(ACol < Data.size())
    if(ARow < Data[ACol].size())
      return Data[ACol][ARow];

  return Value;
}
//---------------------------------------------------------------------------
void TGrid::DoSetText(unsigned ACol, unsigned ARow, const String &Value)
{
  if(FOnSetText)
    FOnSetText(this, ACol, ARow, Value);
  else if(ACol < (unsigned)ColCount && ARow < (unsigned)RowCount)
  {
    Data.resize(std::max(ColCount, Col+1));
    for(unsigned I = 0; I < Data.size(); I++)
      Data[I].resize(std::max(RowCount, Row+1));
    Data[ACol][ARow] = Value;
  }
  InvalidateCell(ACol, ARow);
  if(ACol == static_cast<unsigned>(Col) && ARow == static_cast<unsigned>(Row) && !EditUpdate)
    InvalidateEditor();
}
//---------------------------------------------------------------------------
//Replaces TStringGrid::DrawCell()
void __fastcall TGrid::DrawCell(int ACol, int ARow, const TRect &ARect, TGridDrawState AState)
{
  String Str = DoGetText(ACol, ARow);
  ExtTextOut(Canvas->Handle, ARect.Left+2, ARect.Top+2, ETO_CLIPPED, &ARect, Str.c_str(), Str.Length(), NULL);
  TDrawGrid::DrawCell(ACol, ARow, ARect, AState);
}
//---------------------------------------------------------------------------
String __fastcall TGrid::GetEditText(int ACol, int ARow)
{
  return DoGetText(ACol, ARow);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetEditText(int ACol, int ARow, const String Value)
{
  EditUpdate++; //Don't invalidate inplace editor
  DoSetText(ACol, ARow, Value);
  TDrawGrid::SetEditText(ACol, ARow, Value);
  EditUpdate--;
}
//---------------------------------------------------------------------------
void TGrid::SetCursorPos(int Pos)
{
  if(InplaceEditor)
    InplaceEditor->SelStart = Pos;
}
//---------------------------------------------------------------------------
void __fastcall TGrid::ChangeScale(int M, int D)
{
  TDrawGrid::ChangeScale(M, D); // Call inherited
  if(M != 1 || D != 1)
  {
    DefaultRowHeight = (DefaultRowHeight * M) / D;
    DefaultColWidth = (DefaultColWidth * M) / D;
  }
}
//---------------------------------------------------------------------------
void __fastcall TGrid::SetTitleCaptions(TStrings *Strings)
{
  for(int I = 0; I < Strings->Count; I++)
    DoSetText(I, 0, Strings->Strings[I]);
  FTitleCaptions->Assign(Strings);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::TitleCaptionsChange(TObject *Sender)
{
  for(int I = 0; I < FTitleCaptions->Count; I++)
    DoSetText(I, 0, FTitleCaptions->Strings[I]);
}
//---------------------------------------------------------------------------
void __fastcall TGrid::CalcSizingState(int X, int Y, TGridState &State, int &Index, int &SizingPos, int &SizingOfs, TGridDrawInfo &FixedInfo)
{
  TDrawGrid::CalcSizingState(X, Y, State, Index, SizingPos, SizingOfs, FixedInfo); //Call inherited

  //If the coloumn is invisible (width=0) resizing should not be possible
  if(State == gsColSizing && ColWidths[Index] == 0)
    State = gsNormal;
}
//---------------------------------------------------------------------------
bool __fastcall TGrid::SelectCell(int ACol, int ARow)
{
  if(ColWidths[ACol] <= 0)
  {
    bool MoveAnchor = (GetKeyState(VK_SHIFT) & 0x80000000) == 0;
    int NewCol;
    if(ACol > Selection.Right)
    {
      for(NewCol = ACol + 1; NewCol < ColCount && ColWidths[NewCol] <= 0; NewCol++);
      if(NewCol < ColCount)
        FocusCell(NewCol, ARow, MoveAnchor);
    }
    else
    {
      for(NewCol = ACol - 1; NewCol > FixedCols && ColWidths[NewCol] <= 0; NewCol--);
      if(NewCol > FixedCols)
        FocusCell(NewCol, ARow, MoveAnchor);
    }
    return false;
  }

  return TDrawGrid::SelectCell(ACol, ARow);
}
//---------------------------------------------------------------------------

