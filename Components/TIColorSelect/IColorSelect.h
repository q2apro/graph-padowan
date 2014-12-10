//---------------------------------------------------------------------------
#ifndef ColorSelectH
#define ColorSelectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <vector>
#include "FocusPanel.h"
//---------------------------------------------------------------------------
class EArrayIndex : public Exception
{
public:
  __fastcall EArrayIndex(const String &Msg):Exception(Msg){};
};

class TIColorSelect : public TComponent
{
private:
  TForm *Form;
  TImage *Image;
  TFocusPanel *Panel;

  std::vector<TColor> FColorList;
  int FRowCount;
  int FColCount;
  String FCaption;
  int FHelpContext;
  TColor FColor;
  TPoint SelectedBox;
  bool FEditColor;
  bool FShowHelp;
  TNotifyEvent FOnShow;
  TNotifyEvent FOnClose;
  void __fastcall RefreshColors();
  TColor __fastcall GetColorList(int i,int j);
  void __fastcall SetColorList(int i,int j,TColor Value);
  void __fastcall SetRowCount(int Count);
  void __fastcall SetColCount(int Count);
  void __fastcall SelectBox(int i,int j);
  void __fastcall PixelToBox(int x,int y,int &i,int &j);
  void __fastcall ImageMouseDown(TObject *Sender,TMouseButton Button,TShiftState Shift, int X, int Y);
  void __fastcall PanelKeyDown(TObject *Sender,Word &Key,TShiftState Shift);
  void __fastcall ImageDblClick(TObject *Sender);
  void __fastcall DrawBox(TPoint Box,TColor Color);
  HPALETTE __fastcall GetPalette();
  void __fastcall CreateColorDialog();
  void __fastcall Button3Click(TObject *Sender);
protected:
public:
  __fastcall TIColorSelect(TComponent* Owner);
  bool __fastcall Execute();
  __property TColor ColorList[int i][int j] = {read=GetColorList, write=SetColorList};
  __property HPALETTE Palette = {read=GetPalette};
  __property std::vector<TColor> ColorListVector = {read=FColorList};

__published:
  __property String Caption = {read=FCaption, write=FCaption};
  __property int HelpContext = {read=FHelpContext, write=FHelpContext,default=0};
  __property int RowCount = {read=FRowCount, write=SetRowCount,default=16};
  __property int ColCount = {read=FColCount, write=SetColCount,default=16};
  __property TColor Color = {read=FColor, write=FColor,default=clRed};
  __property bool EditColor = {read=FEditColor, write=FEditColor,default=false};
  __property bool ShowHelp = {read=FShowHelp, write=FShowHelp,default=false};
  __property TNotifyEvent OnShow = {read=FOnShow, write=FOnShow};
  __property TNotifyEvent OnClose = {read=FOnClose, write=FOnClose};
};
//---------------------------------------------------------------------------
#endif
