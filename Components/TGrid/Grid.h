//===========================================================================
// Copyright © 2003 Ivan Johansen
// Grid.h
//===========================================================================
#ifndef GridH
#define GridH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <vector>
#include "VclObject.h"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TEditorKeyPressEvent)(TInplaceEdit *InplaceEdit, Char &Key);
typedef void __fastcall (__closure *TGetTextEvent)(System::TObject* Sender, long ACol, long ARow, String &Value);
typedef void __fastcall (__closure *TSetTextEvent)(System::TObject* Sender, long ACol, long ARow, const String &Value);

class TGrid : public TDrawGrid
{
  friend class TMyInplaceEdit;
  void __fastcall InplaceEditMouseMove(TObject *Sender,TShiftState Shift,int X,int Y);
  void __fastcall InplaceEditMouseUp(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y);
  void __fastcall InplaceEditMouseDown(TObject *Sender,TMouseButton Button,TShiftState Shift,int X,int Y);
  void __fastcall InplaceEditEnter(TObject *Sender);
  void __fastcall InplaceEditExit(TObject *Sender);
  void EmptySelection();
  bool FSelectCols;
  bool FSelectRows;
  int FMinColWidth;
  int FMinRowHeight;
  TPopupMenu *FPopupMenu;
  bool FAutoAddRows;
  bool FExportFixedRows;
  TEditorKeyPressEvent FOnEditorKeyPress;
  TGetTextEvent FOnGetText;
  TSetTextEvent FOnSetText;
  TPopupMenu *FEditorPopupMenu;
  int EditUpdate; //>0 indicates update triggered from change in Inplace Editor

  bool Editing;
  TGridCoord SelStart;
  TTimer *Timer;
  TGridScrollDirection GridScrollDirection;
  int State;
  bool LeftButtonPressed;
  int CursorPos;
  std::vector<std::vector<String> > Data;
  TVclObject<TStringList> FTitleCaptions;

  TInplaceEdit* __fastcall CreateEditor(void);
  DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
  DYNAMIC void __fastcall KeyPress(Char &Key);
  DYNAMIC void __fastcall MouseMove(TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall MouseDown(TMouseButton Button,TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button,TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall DblClick();
	DYNAMIC void __fastcall ColWidthsChanged();
	DYNAMIC void __fastcall RowHeightsChanged();
	DYNAMIC String __fastcall GetEditText(int ACol, int ARow);
	DYNAMIC void __fastcall SetEditText(int ACol, int ARow, String Value);

	void __fastcall DrawCell(int ACol, int ARow, const TRect &ARect, TGridDrawState AState);
	void __fastcall SetOptions(TGridOptions GridOptions);
	void __fastcall SetPopupMenu(TPopupMenu *Menu);
	void __fastcall SetMinRowHeight(int Value);
	void __fastcall SetMinColWidth(int Value);
	void __fastcall CalcSizingState(int X, int Y, TGridState &State, int &Index, int &SizingPos, int &SizingOfs, TGridDrawInfo &FixedInfo);
	bool __fastcall SelectCell(int ACol, int ARow);

	TGridOptions __fastcall GetOptions(){return TDrawGrid::Options;};
	void AjustRows();
	void ImportText(String Str, wchar_t DecimalSeparator, wchar_t Separator);
  String ExportText(wchar_t Delimiter, wchar_t DecimalSeparator);
  void __fastcall SetEditorPopupMenu(TPopupMenu *Menu);
  TGridRect GetCompleteGridRect();
	String DoGetText(unsigned ACol, unsigned ARow);
  void DoSetText(unsigned ACol, unsigned ARow, const String &Value);
  DYNAMIC void __fastcall ChangeScale(int M, int D);
  TStrings* __fastcall GetTitleCaptions() {return FTitleCaptions;}
  void __fastcall SetTitleCaptions(TStrings *Strings);
  void __fastcall TitleCaptionsChange(TObject *Sender);

	//Declare function to handle WM_SetCursor
  void __fastcall WMSetCursor(TMessage &Message);
  void __fastcall WMTimer(TMessage &Message);
  void CopyTextToClipboard(wchar_t DecimalSeparator);
  void CopyRtfToClipboard(wchar_t DecimalSeparator);
  void SetClipboardData(unsigned int Format, void *Data, unsigned int DataSize);
BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(WM_SETCURSOR, TMessage, WMSetCursor)
  VCL_MESSAGE_HANDLER(WM_TIMER, TMessage, WMTimer)
END_MESSAGE_MAP(TDrawGrid)

public:
  __fastcall TGrid(TComponent* Owner);
  void SelectAll();
  void ClearAll();
  void ClearSelection();
  void CopyToClipboard(wchar_t DecimalSeparator);
  void CutToClipboard(wchar_t DecimalSeparator);
  void PasteFromClipboard(wchar_t DecimalSeparator, wchar_t Separator=0);
  bool CanCopy();
	bool ImportFromFile(const String &FileName, wchar_t DecimalSeparator, wchar_t Separator=0);
	void Import(std::istream &Stream, wchar_t DecimalSeparator, wchar_t Separator=0);
  bool ExportToFile(const String &FileName, wchar_t Delimiter, wchar_t DecimalSeparator, bool Utf8=false);
  void AutoSizeCol(int ColIndex);
  void LastCell();
  void NextCell();
  void InsertRows(int Index, int Count);
  void RemoveRows(int Index, int Count);
  void SetCursorPos(int Pos);

  __property String Cells[unsigned ACol][unsigned ARow] = {read=DoGetText, write=DoSetText};

__published:
  __property bool SelectCols = {read=FSelectCols, write=FSelectCols, default=true};
  __property bool SelectRows = {read=FSelectRows, write=FSelectRows ,default=true};
  __property TPopupMenu* PopupMenu = {read=FPopupMenu, write=SetPopupMenu};
  __property TGridOptions Options = {read=GetOptions, write=SetOptions};
  __property int MinColWidth = {read=FMinColWidth, write=SetMinColWidth};
  __property int MinRowHeight = {read=FMinRowHeight, write=SetMinRowHeight};
  __property bool AutoAddRows = {read=FAutoAddRows, write=FAutoAddRows, default=false};
  __property bool ExportFixedRows = {read=FExportFixedRows, write=FExportFixedRows, default=true};
  __property TPopupMenu* EditorPopupMenu = {read=FEditorPopupMenu, write=SetEditorPopupMenu, default=NULL};
  __property TStrings* TitleCaptions = {read=GetTitleCaptions, write=SetTitleCaptions};

  __property TEditorKeyPressEvent OnEditorKeyPress = {read=FOnEditorKeyPress, write=FOnEditorKeyPress, default=NULL};
  __property TGetTextEvent OnGetText = {read=FOnGetText, write=FOnGetText, default=NULL};
  __property TSetTextEvent OnSetText = {read=FOnSetText, write=FOnSetText, default=NULL};
};

class TMyInplaceEdit : public TInplaceEdit
{
  void __fastcall WMDeadChar(TMessage &Message);
  void __fastcall WMSetFocus(TMessage &Message);
  void __fastcall WMKillFocus(TMessage &Message);
  void __fastcall WMPaste(TMessage &Message);
BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(WM_DEADCHAR, TMessage, WMDeadChar)
  VCL_MESSAGE_HANDLER(WM_SETFOCUS, TMessage, WMSetFocus)
  VCL_MESSAGE_HANDLER(WM_KILLFOCUS, TMessage, WMKillFocus)
  VCL_MESSAGE_HANDLER(WM_PASTE, TMessage, WMPaste);
END_MESSAGE_MAP(TInplaceEdit)

  DYNAMIC void __fastcall KeyPress(Char &Key);
  DYNAMIC void __fastcall KeyDown(Word &Key, TShiftState Shift);

public:
  __fastcall TMyInplaceEdit(TComponent *AOwner);
  __property OnMouseMove;
  __property OnMouseUp;
  __property OnMouseDown;
  __property OnEnter;
  __property OnExit;
  __property PopupMenu;
};
//---------------------------------------------------------------------------
#endif
