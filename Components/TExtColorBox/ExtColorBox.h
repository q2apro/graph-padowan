//---------------------------------------------------------------------------
#ifndef ExtColorBoxH
#define ExtColorBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class TExtColorBox;
typedef void __fastcall (__closure *TPickColorEvent)(TExtColorBox* Sender, TColor &Color);
enum TColorDialogType {cdtColorDialog, cdtColorSelect};

class PACKAGE TExtColorBox : public TCustomComboBox
{
private:
  TColorDialogType FColorDialogType;
  TPickColorEvent FOnPickColor;
  int FDroppedWidth;
  bool FAutoDroppedWidth;
  bool DroppedWidthFound;
  bool FShowDefault;
  bool FShowCustom;
  String FDefaultName;
  String FCustomName;
  TColor FSelected; //Used until we have finished loading
  bool ListSelected;

  void __fastcall SetDroppedWidth(int AWidth);
  void __fastcall SetExtendedUI(bool Value);
  bool __fastcall GetExtendedUI();
  void __fastcall SetSelected(TColor Value);
  TColor __fastcall GetSelected();
  bool __fastcall PickCustomColor(void);
  void PopulateList();

protected:
  DYNAMIC void __fastcall ChangeScale(int M, int D);
  void __fastcall DrawItem(int Index, const Types::TRect &Rect, TOwnerDrawState State);
  DYNAMIC void __fastcall KeyPress(Char &Key);
  DYNAMIC void __fastcall Select();
  void __fastcall CreateWnd();
  void __fastcall Loaded();
  DYNAMIC void __fastcall CloseUp();
  DYNAMIC void __fastcall KeyDown(unsigned short &Key, TShiftState Shift);
  DYNAMIC void __fastcall DropDown();

public:
  __fastcall TExtColorBox(TComponent* Owner);

__published:
  __property Align;
  __property Anchors;
  __property BevelEdges;
  __property BevelInner;
  __property BevelKind;
  __property BevelOuter;
  __property BiDiMode;
  __property Color;
  __property Constraints;
  __property Ctl3D;
  __property DropDownCount;
  __property Enabled;
  __property Font;
  __property ItemHeight;
  __property ParentBiDiMode;
  __property ParentColor;
  __property ParentCtl3D;
  __property ParentFont;
  __property ParentShowHint;
  __property PopupMenu;
  __property ShowHint;
  __property TabOrder;
  __property TabStop;
  __property Visible;
  __property OnChange;
  __property OnCloseUp;
  __property OnClick;
  __property OnContextPopup;
  __property OnDblClick;
  __property OnDragDrop;
  __property OnDragOver;
  __property OnDropDown;
  __property OnEndDock;
  __property OnEndDrag;
  __property OnEnter;
  __property OnExit;
  __property OnKeyDown;
  __property OnKeyPress;
  __property OnKeyUp;
  __property OnSelect;
  __property OnStartDock;
  __property OnStartDrag;

  __property TColorDialogType ColorDialogType = {read=FColorDialogType, write=FColorDialogType, default=cdtColorSelect};
  __property TPickColorEvent OnPickColor = {read=FOnPickColor, write=FOnPickColor, default=NULL};
  __property int DroppedWidth = {read=FDroppedWidth, write=SetDroppedWidth, default=0};
  __property bool ExtendedUI = {read=GetExtendedUI, write=SetExtendedUI};
  __property bool AutoDroppedWidth = {read=FAutoDroppedWidth, write=FAutoDroppedWidth};
  __property bool ShowCustom = {read=FShowCustom, write=FShowCustom, default=true};
  __property bool ShowDefault = {read=FShowDefault, write=FShowDefault, default=false};
  __property TColor Selected = {read=GetSelected, write=SetSelected};
  __property String DefaultName = {read=FDefaultName, write=FDefaultName};
  __property String CustomName = {read=FCustomName, write=FCustomName};
};
//---------------------------------------------------------------------------
#endif
