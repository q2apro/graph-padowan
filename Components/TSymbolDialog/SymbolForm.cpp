//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include "SymbolForm.h"
#include "ICompCommon.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FocusPanel"
#pragma resource "*.dfm"

struct TSubset
{
  int First;
  int Last;
  const char *Name;
};

//List from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/intl/unicode_63ub.asp
const TSubset Subsets[] = {
{0x0020, 0x007A, "Basic Latin"},
{0x00A0, 0x00FF, "Latin-1"},
{0x0100, 0x017F, "Latin Extended-A"},
{0x0180, 0x024F, "Latin Extended-B"},
{0x0250, 0x02AF, "IPA Extensions"},
{0x02B0, 0x02FF, "Spacing Modifier Letters"},
{0x0300, 0x036F, "Combining Diacritical Marks"},
{0x0370, 0x03FF, "Greek and Coptic"},
{0x0000, 0x0000, ""},
{0x0400, 0x052F, "Cyrillic"},
{0x0530, 0x058F, "Armenian"},
{0x0590, 0x05FF, "Basic Hebrew"},
{0x0000, 0x0000, ""},
{0x0600, 0x06FF, "Basic Arabic"},
{0x0000, 0x0000, ""},
{0x0900, 0x097F, "Devanagari"},
{0x0980, 0x09FF, "Bengali"},
{0x0A00, 0x0A7F, "Gurmukhi"},
{0x0A80, 0x0AFF, "Gujarati"},
{0x0B00, 0x0B7F, "Oriya"},
{0x0B80, 0x0BFF, "Tamil"},
{0x0C00, 0x0C7F, "Telugu"},
{0x0C80, 0x0CFF, "Kannada"},
{0x0D00, 0x0D7F, "Malayalam"},
{0x0E00, 0x0E7F, "Thai"},
{0x0E80, 0x0EFF, "Lao"},
{0x10A0, 0x10FF, "Georgian"},
{0x0000, 0x0000, ""},
{0x1100, 0x11FF, "Hangul Jamo"},
{0x1E00, 0x1EFF, "Latin Extended Additional"},
{0x1F00, 0x1FFF, "Greek Extended"},
{0x2000, 0x206F, "General Punctuation"},
{0x2070, 0x209F, "Subscripts and Superscripts"},
{0x20A0, 0x20CF, "Currency Symbols"},
{0x20D0, 0x20FF, "Combining Diacritical Marks for Symbols"},
{0x2100, 0x214F, "Letter-like Symbols"},
{0x2150, 0x218F, "Number Forms"},
{0x2190, 0x297F, "Arrows"},
{0x2200, 0x29FF, "Mathematical Operators"},
{0x2300, 0x23FF, "Miscellaneous Technical"},
{0x2400, 0x243F, "Control Pictures"},
{0x2440, 0x245F, "Optical Character Recognition"},
{0x2460, 0x24FF, "Enclosed Alphanumerics"},
{0x2500, 0x257F, "Box Drawing"},
{0x2580, 0x259F, "Block Elements"},
{0x25A0, 0x25FF, "Geometric Shapes"},
{0x2600, 0x26FF, "Miscellaneous Symbols"},
{0x2700, 0x27BF, "Dingbats"},
{0x3000, 0x303F, "Chinese, Japanese, and Korean (CJK) Symbols and Punctuation"},
{0x3040, 0x309F, "Hiragana"},
{0x30A0, 0x31FF, "Katakana"},
{0x3100, 0x318F, "Bopomofo"},
{0x3130, 0x318F, "Hangul Compatibility Jamo"},
{0x0000, 0x0000, ""},
{0x3200, 0x32FF, "Enclosed CJK Letters and Months"},
{0x3300, 0x33FF, "CJK Compatibility"},
{0xAC00, 0xD7A3, "Hangul"},
{0xD800, 0xDFFF, "Surrogates"},
{0x0000, 0x0000, ""},
{0x0000, 0x0000, ""}, //Bit 59: This is actually several ranges
{0xE000, 0xF8FF, "Private Use Area"},
{0xF900, 0xFAFF, "CJK Compatibility Ideographs"},
{0xFB00, 0xFB4F, "Alphabetic Presentation Forms"},
{0xFB50, 0xFDFF, "Arabic Presentation Forms-A"},
{0xFE20, 0xFE2F, "Combining Half Marks"},
{0xFE30, 0xFE4F, "CJK Compatibility Forms"},
{0xFE50, 0xFE6F, "Small Form Variants"},
{0xFE70, 0xFEFE, "Arabic Presentation Forms-B"},
{0xFF00, 0xFFEF, "Halfwidth and Fullwidth Forms"},
{0xFFF0, 0xFFFF, "Specials"},
{0x0F00, 0x0FFF, "Tibetan"},
{0x0700, 0x074F, "Syriac"},
{0x0780, 0x07BF, "Thaana"},
{0x0D80, 0x0DFF, "Sinhala"},
{0x1000, 0x109F, "Myanmar"},
{0x1200, 0x12BF, "Ethiopic"},
{0x13A0, 0x13FF, "Cherokee"},
{0x1400, 0x167F, "Canadian Aboriginal Syllabics"},
{0x1680, 0x169F, "Ogham"},
{0x16A0, 0x16FF, "Runic"},
{0x1780, 0x17FF, "Khmer"}, //Khmer Symbols are missing
{0x1800, 0x18AF, "Mongolian"},
{0x2800, 0x28FF, "Braille"},
{0xA000, 0xA4CF, "Yi"},
{0x1700, 0x177F, "Tagalog, Hanunoo, Buhid, Tagbanwa"},
{0x10300, 0x1032F, "Old Italic"},
{0x10330, 0x1034F, "Gothic"},
{0x10440, 0x1044F, "Deseret"},
{0x1D000, 0x1D1FF, "Musical Symbols"},
{0x1D400, 0x1D7FF, "Mathematical Alphanumeric Symbols"},
};
//---------------------------------------------------------------------------
__fastcall TSymbolFrm::TSymbolFrm(TComponent* Owner, bool AShowUnicode, wchar_t Symbol)
  : TForm(Owner), Selected(Symbol), hUNameDll(NULL), pGetUNameFunc(NULL), ShowUnicode(AShowUnicode),
    hGdi32Dll(NULL), pGetFontUnicodeRanges(NULL), Glyphset(NULL),
    FontSize(12), PreviewFontSize(30), Delta(20)
{
  Caption = TranslateCompText(L"Insert symbol");
  Label1->Caption = L"&" + TranslateCompText(L"Font name:");
  Label2->Caption = L"&" + TranslateCompText(L"Subset:");
  Button1->Caption = TranslateCompText(L"Close");
  Button2->Caption = TranslateCompText(L"Insert");
  ComboBox1->Left = Label1->Left + Label1->Width + 5;

  hUNameDll = LoadLibrary(L"GETUNAME.DLL");
  if(hUNameDll)
    pGetUNameFunc = reinterpret_cast<TGetUNameFunc>(GetProcAddress(hUNameDll, "GetUName"));

  hGdi32Dll = LoadLibrary(L"GDI32.DLL");
  if(hGdi32Dll)
    pGetFontUnicodeRanges = reinterpret_cast<TGetFontUnicodeRanges>(GetProcAddress(hGdi32Dll, "GetFontUnicodeRanges"));

  //Add list of fonts; Take care of duplicates, that for some strange reason has happened
  for(int I = 0; I < Screen->Fonts->Count; I++)
    if(ComboBox1->Items->IndexOf(Screen->Fonts->Strings[I]) == -1)
      ComboBox1->Items->Add(Screen->Fonts->Strings[I]);

  if(!ShowUnicode)
  {
    Label2->Visible = false;
    ComboBox2->Visible = false;
    ScrollBar1->Visible = false;
  }

  if(PixelsPerInch != 96)
    StatusBar1->Height = (StatusBar1->Height * PixelsPerInch) / 96;
  FocusPanel1->DoubleBuffered = true;
}
//---------------------------------------------------------------------------
__fastcall TSymbolFrm::~TSymbolFrm()
{
  if(hUNameDll)
    FreeLibrary(hUNameDll);
  if(hGdi32Dll)
    FreeLibrary(hGdi32Dll);
}
//---------------------------------------------------------------------------
void TSymbolFrm::DrawSymbol(TCanvas *Canvas, wchar_t Symbol, const TRect &Rect, bool UseUnicode)
{
  if(UseUnicode)
    DrawTextW(Canvas->Handle, &Symbol, 1, const_cast<TRect*>(&Rect), DT_CENTER | DT_VCENTER);
  else
  {
    char Ch = Symbol;
    DrawTextA(Canvas->Handle, &Ch, 1, const_cast<TRect*>(&Rect), DT_CENTER | DT_VCENTER);
  }
}
//---------------------------------------------------------------------------
void TSymbolFrm::UpdateImage()
{
  TCanvas *Canvas = Image1->Canvas;
  int Width = Image1->Width;
  int Height = Image1->Height;

  Canvas->Brush->Color = clWhite;
  Canvas->Pen->Color = clBlack;
  Canvas->Font->Name = ComboBox1->Text;
  Canvas->Font->Size = FontSize;
  Canvas->Font->Color = clBlack;
  Canvas->Font->Charset = DEFAULT_CHARSET;

  int Charset = GetTextCharset(Canvas->Handle);
  
  //Clear image
  Canvas->FillRect(Image1->ClientRect);

  //Draw symbols
  wchar_t Symbol = GetSymbol(ScrollBar1->Position * ColCount);
  unsigned Index = GetRangeIndex(Symbol);
  for(int Row = 0; Row < RowCount; Row++)
    for(int Col = 0; Col < ColCount; Col++)
    {
      if(Symbol >= Glyphset->ranges[Index].wcLow + Glyphset->ranges[Index].cGlyphs)
      {
        Index++;
        if(Index >= Glyphset->cRanges)
          break;
        Symbol = Glyphset->ranges[Index].wcLow;
      }

      TRect Rect(Col * Delta, Row * Delta, (Col+1) * Delta, (Row+1) * Delta);
      DrawSymbol(Image1->Canvas, Symbol, Rect, ShowUnicode && Charset != SYMBOL_CHARSET);
      Symbol++;
    }

  //Draw horizontal and vertical lines
  for(int Col = 0; Col <= ColCount; Col++)
  {
    Canvas->MoveTo(Col * Delta, 0);
    Canvas->LineTo(Col * Delta, Height);
  }

  for(int Row = 0; Row <= RowCount; Row++)
  {
    Canvas->MoveTo(0, Row * Delta);
    Canvas->LineTo(Width, Row*Delta);
  }

  //Draw selected symbol
  Canvas->Brush->Color = (TColor)0x006B2408; //A dark blue color
  unsigned Pos = GetSymbolPos(Selected);
  int Row = Pos / ColCount - ScrollBar1->Position;
  unsigned Col = Pos % ColCount;

  TRect SelectedRect(Col * Delta + 1, Row * Delta + 1, (Col+1) * Delta, (Row+1) * Delta);
  Canvas->FillRect(SelectedRect);
  Canvas->Font->Color = clWhite;
  DrawSymbol(Image1->Canvas, Selected, SelectedRect, ShowUnicode && Charset != SYMBOL_CHARSET);
  if(FocusPanel1->Focused())
    Canvas->DrawFocusRect(SelectedRect);

  //Show large preview
  Image2->Canvas->Brush->Color = clBtnFace;
  Image2->Canvas->FillRect(Image2->ClientRect);
  Image2->Canvas->Font->Name = ComboBox1->Text;
  Image2->Canvas->Font->Size = PreviewFontSize;
  DrawSymbol(Image2->Canvas, Selected, Image2->ClientRect, ShowUnicode && Charset != SYMBOL_CHARSET);

  //Show character information in status bar
  if(pGetUNameFunc && Charset != SYMBOL_CHARSET)
  {
    wchar_t CharInfo[200];
    pGetUNameFunc(Selected, CharInfo);
    StatusBar1->Panels->Items[0]->Text = CharInfo;
    StatusBar1->Panels->Items[1]->Text = "U+" + IntToHex(Selected, 4) + " (" + (int)Selected + ")";
  }
  else
  {
    StatusBar1->Panels->Items[0]->Text = "";
    StatusBar1->Panels->Items[1]->Text = "0x" + IntToHex(Selected, 2) + " (" + (int)Selected + ")";
  }

  Image1->Repaint();
  Image2->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::FormShow(TObject *Sender)
{
  Delta = static_cast<double>(Image1->Width - 1) / ColCount;
  ComboBox1Select(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::Image1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(Button == mbLeft)
    if(SetSelected(X / Delta, Y / Delta))
    {
      UpdateImage();
      if(Shift.Contains(ssDouble))
        Button2->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::Image1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if(Shift.Contains(ssLeft))
    if(SetSelected(X / Delta, Y / Delta))
      UpdateImage();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::Button1Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::Image1DblClick(TObject *Sender)
{
  Button2->Click();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::FocusPanel1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  unsigned Pos = GetSymbolPos(Selected);
  unsigned Row = Pos / ColCount - ScrollBar1->Position;
  unsigned Col = Pos % ColCount;
  switch(Key)
  {
    case VK_LEFT:
      if(Col > 0)
        Col--;
      else if(Row > 0)
        Row--, Col = ColCount-1;
      else if(ScrollBar1->Position > ScrollBar1->Min)
        Col = ColCount-1, SetPosition(ScrollBar1->Position - ScrollBar1->SmallChange);
      break;

    case VK_RIGHT:
      if(Col < ColCount-1)
        Col++;
      else if(Row < RowCount-1)
        Row++, Col = 0;
      else
        Col = 0, SetPosition(ScrollBar1->Position + ScrollBar1->SmallChange);
      break;

    case VK_UP:
      if(Row > 0)
        Row--;
      else
        SetPosition(ScrollBar1->Position - ScrollBar1->SmallChange);
      break;

    case VK_DOWN:
      if(Row <  RowCount-1)
        Row++;
      else
        SetPosition(ScrollBar1->Position + ScrollBar1->SmallChange);
      break;

    case VK_PRIOR:
      SetPosition(ScrollBar1->Position - ScrollBar1->LargeChange);
      break;

    case VK_NEXT:
      SetPosition(ScrollBar1->Position + ScrollBar1->LargeChange);
      break;

    case VK_HOME:
      SetPosition(ScrollBar1->Min);
      Col = 0;
      Row = 0;
      break;

    case VK_END:
      SetPosition(ScrollBar1->Max);
      if(SetSelected(LastSymbol()))
        UpdateImage();
      return;
  }

  if(SetSelected(Col, Row))
    UpdateImage();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::ComboBox1Select(TObject *Sender)
{
  Image1->Canvas->Font->Name = ComboBox1->Text;

  //Symbol charset only has characters up to '\xFF'
  int Charset = GetTextCharset(Image1->Canvas->Handle);
  if(Charset == SYMBOL_CHARSET || !ShowUnicode)
  {
    SetPosition(0);
    ScrollBar1->Enabled = false;
    ComboBox2->Enabled = false;
    ComboBox2->ItemIndex = -1;
    static const GLYPHSET SymbolGlyphset = {0, 0, 224, 1, {0x20, 224}};
    Glyphset = &SymbolGlyphset;
  }
  else if(pGetFontUnicodeRanges)
  {
    ComboBox2->Enabled = true;
    Data.resize(pGetFontUnicodeRanges(Image1->Canvas->Handle, NULL));
    Glyphset = reinterpret_cast<GLYPHSET*>(&Data[0]);
    pGetFontUnicodeRanges(Image1->Canvas->Handle, const_cast<GLYPHSET*>(Glyphset));
    int Max = Glyphset->cGlyphsSupported / ColCount;
    if(Max > RowCount)
      ScrollBar1->Max = Max;
    else
      SetPosition(0);
    ScrollBar1->Enabled = Max > RowCount;
    UpdateSubsetList();
  }

  unsigned Pos = GetSymbolPos(Selected);
  SetPosition(Pos >= ColCount ? Pos / ColCount - 1 : 0);
  SetSelected(GetSymbol(Pos));
  SelectSubset(Selected);
  UpdateImage();
}
//---------------------------------------------------------------------------
wchar_t TSymbolFrm::GetSymbol(int Count)
{
  unsigned Index;
  for(Index = 0; Count >= Glyphset->ranges[Index].cGlyphs && Index < Glyphset->cRanges; Index++)
    Count -= Glyphset->ranges[Index].cGlyphs;

  if(Index == Glyphset->cRanges)
    return 0; /*Glyphset->ranges[Glyphset->cRanges - 1].wcLow + Glyphset->ranges[Glyphset->cRanges - 1].cGlyphs - 1;*/

  return Glyphset->ranges[Index].wcLow + Count;
}
//---------------------------------------------------------------------------
bool TSymbolFrm::SetSelected(unsigned Col, unsigned Row)
{
  wchar_t NewSelected = GetSymbol((ScrollBar1->Position + Row) * ColCount + Col);
  return SetSelected(NewSelected);
}
//---------------------------------------------------------------------------
bool TSymbolFrm::SetSelected(wchar_t Symbol)
{
  if(Symbol != Selected && Symbol != 0)
  {
    Selected = Symbol;
    SelectSubset(Symbol);
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
unsigned TSymbolFrm::GetRangeIndex(wchar_t Symbol)
{
  unsigned Index;
  for(Index = 0; Index < Glyphset->cRanges; Index++)
    if(Symbol < Glyphset->ranges[Index].wcLow)
      return Index == 0 ? 0 : Index - 1;
  return 0;
}
//---------------------------------------------------------------------------
void TSymbolFrm::UpdateSubsetList()
{
  ComboBox2->Items->Clear();
  unsigned Index = 0;
  for(unsigned I = 0; I < sizeof(Subsets)/sizeof(Subsets[0]); I++)
  {
    while(Glyphset->ranges[Index].wcLow + Glyphset->ranges[Index].cGlyphs - 1 < Subsets[I].First)
      if(++Index >= Glyphset->cRanges)
        break;
    if(Index < Glyphset->cRanges && Glyphset->ranges[Index].wcLow <= Subsets[I].Last)
      ComboBox2->Items->Add(Subsets[I].Name);
  }
}
//---------------------------------------------------------------------------
unsigned TSymbolFrm::GetSymbolPos(wchar_t Symbol)
{
  unsigned Pos = 0;
  unsigned Index;
  for(Index = 0; Index < Glyphset->cRanges-1 && Symbol >= Glyphset->ranges[Index+1].wcLow; Index++)
    Pos += Glyphset->ranges[Index].cGlyphs;

  if(Symbol < Glyphset->ranges[Index].wcLow + Glyphset->ranges[Index].cGlyphs)
    Pos += Symbol - Glyphset->ranges[Index].wcLow;
  else
    Pos += Glyphset->ranges[Index].cGlyphs;

  return (Pos >= Glyphset->cGlyphsSupported) ? (Glyphset->cGlyphsSupported - 1) : Pos;
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::ScrollBar1Scroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos)
{
  if(ScrollPos > ScrollBar1->Max - RowCount + 1)
    ScrollPos = ScrollBar1->Max - RowCount + 1;
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::ScrollBar1Change(TObject *Sender)
{
  unsigned Pos = GetSymbolPos(Selected);
  int Row = Pos / ColCount - ScrollBar1->Position;
  unsigned Col = Pos % ColCount;

  if(Row < 0)
    Row = 0, SetSelected(Col, Row);
  else if(Row >= RowCount)
    Row = RowCount - 1, SetSelected(Col, Row);

  UpdateImage();
}
//---------------------------------------------------------------------------
void TSymbolFrm::SetPosition(int Pos)
{
  if(!ScrollBar1->Enabled)
    return;

  ScrollBar1->OnChange = NULL;
  if(Pos < ScrollBar1->Max - RowCount + 1)
    ScrollBar1->Position = Pos;
  else
  ScrollBar1->Position = ScrollBar1->Max - RowCount + 1;
  ScrollBar1->OnChange = ScrollBar1Change;
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::ComboBox2Change(TObject *Sender)
{
  for(unsigned I = 0; I < sizeof(Subsets)/sizeof(Subsets[0]); I++)
    if(ComboBox2->Text == Subsets[I].Name)
    {
      unsigned Pos = GetSymbolPos(Subsets[I].First);
      SetSelected(GetSymbol(Pos));
      SetPosition(Pos >= ColCount ? Pos / ColCount - 1 : 0);
      UpdateImage();
      break;
    }
}
//---------------------------------------------------------------------------
wchar_t TSymbolFrm::LastSymbol()
{
  int Index = Glyphset->cRanges - 1;
  return Glyphset->ranges[Index].wcLow + Glyphset->ranges[Index].cGlyphs - 1;
}
//---------------------------------------------------------------------------
void TSymbolFrm::SelectSubset(wchar_t Symbol)
{
  //Check if this is a symbolic font
  if(Glyphset->cbThis == 0)
    return;

  for(unsigned I = 0; I < sizeof(Subsets)/sizeof(Subsets[0]); I++)
    if(Symbol >= Subsets[I].First && Symbol <= Subsets[I].Last)
      ComboBox2->ItemIndex = ComboBox2->Items->IndexOf(Subsets[I].Name);
}
//---------------------------------------------------------------------------
void __fastcall TSymbolFrm::ChangeScale(int M, int D)
{
  FontSize = (FontSize * M) / D;
  PreviewFontSize = (PreviewFontSize * M) / D;
  TForm::ChangeScale(M, D); // Call inherited
}
//---------------------------------------------------------------------------

