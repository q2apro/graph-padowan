//---------------------------------------------------------------------------
#ifndef SymbolFormH
#define SymbolFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "FocusPanel.h"
#include <vector>
//---------------------------------------------------------------------------
typedef __declspec(dllimport) int (FAR WINAPI *TGetUNameFunc)(DWORD,LPVOID);
typedef DWORD (WINAPI *TGetFontUnicodeRanges)(HDC, LPGLYPHSET);

const int ColCount = 20;
const int RowCount = 12;

class TSymbolFrm : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TComboBox *ComboBox1;
  TStatusBar *StatusBar1;
  TImage *Image2;
  TFocusPanel *FocusPanel1;
  TImage *Image1;
  TLabel *Label1;
  TScrollBar *ScrollBar1;
  TLabel *Label2;
  TComboBox *ComboBox2;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Image1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Image1DblClick(TObject *Sender);
  void __fastcall FocusPanel1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall ComboBox1Select(TObject *Sender);
  void __fastcall ScrollBar1Scroll(TObject *Sender, TScrollCode ScrollCode,
          int &ScrollPos);
  void __fastcall ScrollBar1Change(TObject *Sender);
  void __fastcall ComboBox2Change(TObject *Sender);

private:	// User declarations
  HMODULE hUNameDll;
  HMODULE hGdi32Dll;
  TGetUNameFunc pGetUNameFunc;
  TGetFontUnicodeRanges pGetFontUnicodeRanges;
  bool ShowUnicode;
  wchar_t Selected;
  std::vector<BYTE> Data;
  const GLYPHSET *Glyphset;
  double Delta;
  int FontSize;
  int PreviewFontSize;

  void UpdateImage();
  bool SetSelected(unsigned Row, unsigned Col);
  bool SetSelected(wchar_t Symbol);
  unsigned GetRangeIndex(wchar_t Symbol);
  void UpdateSubsetList();
  wchar_t GetSymbol(int Pos);
  unsigned GetSymbolPos(wchar_t Symbol);
  void SetPosition(int Pos);
  wchar_t LastSymbol();
  void SelectSubset(wchar_t Symbol);
  void DrawSymbol(TCanvas *Canvas, wchar_t Symbol, const TRect &Rect, bool UseUnicode);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

public:		// User declarations
  __fastcall TSymbolFrm(TComponent* Owner, bool AShowUnicode, wchar_t Symbol);
  __fastcall ~TSymbolFrm();
  wchar_t GetSelected() {return Selected;}
};
//---------------------------------------------------------------------------
#endif
