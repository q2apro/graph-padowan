//---------------------------------------------------------------------------
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "IRichEdit.h"
#include <memory>
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TIRichEdit));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TIRichEdit *)
{
  new TIRichEdit(NULL);
}
//---------------------------------------------------------------------------
namespace Irichedit
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TIRichEdit)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TIRichEdit::TIRichEdit(TComponent* Owner)
  : TCustomRichEdit(Owner), FTransparent(false), TextFormat(this, false),
    GlobalTextFormat(this, true), FOnOleError(NULL), FBackgroundColor(clDefault),
    FParagraph(new ::TParaFormat(this)), FOnLink(NULL), FProtectedChange(false),
    FOnActivateObject(NULL), RichEditOle(NULL), FEnableOLE(false), FWrapType(wtWord),
    OldEditWordBreakProc(NULL)
{
  ControlStyle = ControlStyle >> csSetCaption;
}
//---------------------------------------------------------------------------
__fastcall TIRichEdit::~TIRichEdit()
{
  delete FParagraph;
}
//---------------------------------------------------------------------------
//The following ensures that the new version of richedit (v2.0) is loaded;
// Note: Do not call TRichEdit::CreateParams -- it will attempt to
// load the older version of the rich edit DLL -- not a Good Thing -- too bad
// if anything important goes on there
void __fastcall TIRichEdit::CreateParams(Controls::TCreateParams &Params)
{
	// modified from TCustomRichEdit
	FLibHandle = LoadLibrary(L"RICHED20.DLL");

	TCustomMemo::CreateParams(Params); //Don't call TCustomRichEdit::CreateParams

	CreateSubClass(Params, L"RichEdit20W" /*RICHEDIT_CLASS*/);
	Params.Style = Params.Style |
		(HideScrollBars ? 0 : ES_DISABLENOSCROLL) |
		(HideSelection ? 0 : ES_NOHIDESEL);
//	Params.WindowClass.style &= ~(CS_HREDRAW | CS_VREDRAW);

  Params.ExStyle |= (FTransparent ? WS_EX_TRANSPARENT : 0);
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::CreateWnd()
{
  TCustomRichEdit::CreateWnd();
  if(EnableOLE && RichEditOle == NULL)
    RichEditOle = new TRichEditOle(this);

  if(OnActivateObject)
    SetEventMask(ENM_MOUSEEVENTS, ENM_MOUSEEVENTS);

  OldEditWordBreakProc = (EDITWORDBREAKPROC)SendMessage(Handle, EM_GETWORDBREAKPROC, 0, 0);
  if(FWrapType == wtChar)
    SendMessage(Handle, EM_SETWORDBREAKPROC, 0, (LONG)&EditWordBreakProc);
}
//---------------------------------------------------------------------------
//Free the library on destroy (actually, it will not unload until last
// TRIichEdit instance is destroyed)
void __fastcall TIRichEdit::DestroyWnd(void)
{
  if(RichEditOle)
    delete RichEditOle;
  RichEditOle = NULL;
	TCustomRichEdit::DestroyWnd();
	if(FLibHandle)
    FreeLibrary(FLibHandle);
  FLibHandle = NULL;
  OldEditWordBreakProc = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetTransparent(bool Value)
{
  FTransparent = Value;

  //We cannot change style if there is no handle
  if(HandleAllocated())
  {
    LONG exStyle = GetWindowLong(Handle, GWL_EXSTYLE);
    if(FTransparent)
    {
      exStyle |= WS_EX_TRANSPARENT;
      Brush->Style = bsClear;
    }
    else
    {
      exStyle &= ~(WS_EX_TRANSPARENT);
      Brush->Style = bsSolid;
    }
    SetWindowLong(Handle, GWL_EXSTYLE, exStyle);
  }
}
//---------------------------------------------------------------------------
void TIRichEdit::SetRichText(const AnsiString &Str)
{
  //Using Lines->LoadFromStream seams to cause some trouble with OLE objects in some cases.
  SETTEXTEX SetTextEx = {ST_DEFAULT, CP_ACP};
  SendMessage(Handle, EM_SETTEXTEX, (WPARAM)&SetTextEx, (LPARAM)Str.c_str());
}
//---------------------------------------------------------------------------
AnsiString TIRichEdit::GetRichText()
{
  std::auto_ptr<TStringStream> Stream(new TStringStream(String()));
  Lines->SaveToStream(Stream.get());

  //The string will have a '\0' embedded as the last character
  //I don't know where this comes from, but it gives problems when saved to file
  //It looks like there may be more than one
  AnsiString Str = Stream->DataString;
  while(Str[Str.Length()] == 0)
    Str.Delete(Str.Length(), 1);
  return Str;
}
//---------------------------------------------------------------------------
void TIRichEdit::Render(TCanvas *Canvas, const TPoint &Pos, int Width)
{
  FORMATRANGE Format;
  Format.hdc = Canvas->Handle;

  Format.hdcTarget = Canvas->Handle;

  //Convert coordinates to twips
  int PixelsX = GetDeviceCaps(Canvas->Handle, LOGPIXELSX);
  int PixelsY = GetDeviceCaps(Canvas->Handle, LOGPIXELSY);
  if(PixelsX == 0 || PixelsY == 0)
    return;

  double xPixels = 1440.0 / PixelsX;
  double yPixels = 1440.0 / PixelsY;
                                                  
  Format.rc.left = Pos.x * xPixels;
  Format.rc.top = Pos.y * yPixels;
  Format.rc.right = (Pos.x + Width) * xPixels;
  Format.rc.bottom = MAXSHORT;

  Format.rcPage = Format.rc;
  Format.chrg.cpMin = 0;
  Format.chrg.cpMax = -1;

  SendMessage(Handle, EM_FORMATRANGE, 1, reinterpret_cast<long>(&Format));
  //Free cached memory
  SendMessage(Handle, EM_FORMATRANGE, 1, NULL);
}                             
//---------------------------------------------------------------------------
//Returns the size of the text
TPoint TIRichEdit::GetTextSize()
{
  TPoint Point;
  int Lines = LineCount();
  TPoint Size(0, 0);
  for(int I = 0; I < Lines; I++)
  {
    int Index = I ? LineIndex(I) : MaxInt;
    SendMessage(Handle, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&Point), Index - 1);
    if(Point.x > Size.x)
      Size.x = Point.x;
    if(Point.y > Size.y)
      Size.y = Point.y;
  }

  return Size;
}
//---------------------------------------------------------------------------
bool TIRichEdit::IsEmpty() const
{
  return !Lines->Count;
}
//---------------------------------------------------------------------------
TTextFormat::TTextFormat(TIRichEdit *ARichEdit, bool AGlobal) : RichEdit(ARichEdit), Global(AGlobal)
{
}
//---------------------------------------------------------------------------
void TTextFormat::SetFormat(::CHARFORMAT2 Format, unsigned Mask)
{
  Format.cbSize = sizeof(Format);
  Format.dwMask = Mask;
  Format.dwReserved = 0;
  if(!SendMessage(RichEdit->Handle,
                  EM_SETCHARFORMAT,
                  Global ? SCF_ALL : SCF_SELECTION,
                  reinterpret_cast<long>(&Format)))
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
void TTextFormat::SetFormat(unsigned Mask, unsigned Effects)
{
  ::CHARFORMAT2 Format;
  Format.dwEffects = Effects;
  SetFormat(Format, Mask);
}
//---------------------------------------------------------------------------
::CHARFORMAT2 TTextFormat::GetFormat() const
{
  ::CHARFORMAT2 Format;
  Format.cbSize = sizeof(Format);

  SendMessage(RichEdit->Handle,
              EM_GETCHARFORMAT,
              Global ? SCF_ALL : SCF_SELECTION,
              reinterpret_cast<long>(&Format));
  return Format;
}
//---------------------------------------------------------------------------
void TTextFormat::SetSuperscript(bool Value)
{
  SetFormat(CFM_SUPERSCRIPT, Value ? CFE_SUPERSCRIPT : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetSubscript(bool Value)
{
  SetFormat(CFM_SUBSCRIPT, Value ? CFE_SUBSCRIPT : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetBold(bool Value)
{
  SetFormat(CFM_BOLD, Value ? CFE_BOLD : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetItalic(bool Value)
{
  SetFormat(CFM_ITALIC, Value ? CFE_ITALIC : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetUnderline(bool Value)
{
  SetFormat(CFM_UNDERLINE, Value ? CFE_UNDERLINE : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetStrikeOut(bool Value)
{
  SetFormat(CFM_STRIKEOUT, Value ? CFE_STRIKEOUT : 0);
}
//---------------------------------------------------------------------------
void TTextFormat::SetSize(unsigned Value)
{
  ::CHARFORMAT2 Format;
  Format.yHeight = Value * 20;
  SetFormat(Format, CFM_SIZE);
}
//---------------------------------------------------------------------------
unsigned GetCharset(const AnsiString &FontName)
{
  //We need the charset to make sure that the correct font is used. Especially "script"
  //Doesn't work else. This way is a little overkill, but it works.
  //It I first set the font to "ABSALOM" and when to "Script", the font will be
  //shown as wingdings instead. This seems to be a rich edit bug, as it also happens in Wordpad
  std::auto_ptr<TCanvas> Canvas(new TCanvas);
  Canvas->Handle = GetDC(NULL);
  Canvas->Font->Name = FontName;
  unsigned Charset = GetTextCharset(Canvas->Handle);
  ReleaseDC(NULL, Canvas->Handle);
  return Charset;
}
//---------------------------------------------------------------------------
void TTextFormat::SetName(const String &Str)
{
  ::CHARFORMAT2 Format;
  wcsncpy(Format.szFaceName, Str.c_str(), sizeof(Format.szFaceName));
  Format.szFaceName[sizeof(Format.szFaceName) - 1] = 0;
  Format.bCharSet = GetCharset(Str);
  SetFormat(Format, CFM_FACE);    //We must set the font name first and the charset afterwards
  SetFormat(Format, CFM_CHARSET);
}
//---------------------------------------------------------------------------
void TTextFormat::SetColor(TColor Value)
{
  ::CHARFORMAT2 Format;
  Format.crTextColor = ColorToRGB(Value);
  Format.dwEffects = 0;
  SetFormat(Format, CFM_COLOR);
}
//---------------------------------------------------------------------------
//Set background color for text; Use clNone for transparent
void TTextFormat::SetBackgroundColor(TColor Value)
{
  ::CHARFORMAT2 Format;
  Format.crBackColor = ColorToRGB(Value);
  Format.dwEffects = Value == clNone ? CFE_AUTOBACKCOLOR : 0;
  SetFormat(Format, CFM_BACKCOLOR);
}
//---------------------------------------------------------------------------
bool TTextFormat::GetSuperscript() const
{
  return GetFormat().dwEffects & CFE_SUPERSCRIPT;
}
//---------------------------------------------------------------------------
bool TTextFormat::GetSubscript() const
{
  return GetFormat().dwEffects & CFE_SUBSCRIPT;
}
//---------------------------------------------------------------------------
bool TTextFormat::GetBold() const
{
  return GetFormat().dwEffects & CFE_BOLD;
}
//---------------------------------------------------------------------------
bool TTextFormat::GetItalic() const
{
  return GetFormat().dwEffects & CFE_ITALIC;
}
//---------------------------------------------------------------------------
bool TTextFormat::GetUnderline() const
{
  return GetFormat().dwEffects & CFE_UNDERLINE;
}
//---------------------------------------------------------------------------
bool TTextFormat::GetStrikeOut() const
{
  return GetFormat().dwEffects & CFE_STRIKEOUT;
}
//---------------------------------------------------------------------------
unsigned TTextFormat::GetSize() const
{
  return GetFormat().yHeight / 20;
}
//---------------------------------------------------------------------------
String TTextFormat::GetName() const
{
  return GetFormat().szFaceName;
}
//---------------------------------------------------------------------------
TColor TTextFormat::GetColor() const
{
  return static_cast<TColor>(GetFormat().crTextColor);
}
//---------------------------------------------------------------------------
TColor TTextFormat::GetBackgroundColor() const
{
  ::CHARFORMAT2 Format = GetFormat();
  return Format.dwEffects & CFE_AUTOBACKCOLOR ? clDefault : static_cast<TColor>(Format.crBackColor);
}
//---------------------------------------------------------------------------
void TIRichEdit::Redo()
{
  SendMessage(Handle, EM_REDO, 0, 0); 
}
//---------------------------------------------------------------------------
bool TIRichEdit::CanRedo()
{
  return SendMessage(Handle, EM_CANREDO, 0, 0);
}
//---------------------------------------------------------------------------
bool TIRichEdit::CanPaste() 
{
  return SendMessage(Handle, EM_CANPASTE, 0, 0);
}
//---------------------------------------------------------------------------
void TIRichEdit::SetUndoLimit(unsigned Limit)
{
  SendMessage(Handle, EM_SETUNDOLIMIT, Limit, 0);
}
//---------------------------------------------------------------------------
//Returns the character index for the given line number.
//Line=-1 return index for the current line
//Returns -1 if the line does not exist
int TIRichEdit::LineIndex(int Line)
{
  return SendMessage(Handle, EM_LINEINDEX, Line, 0);
}
//---------------------------------------------------------------------------
//Returns the number of lines in the rich edit
int TIRichEdit::LineCount()
{
  return SendMessage(Handle, EM_GETLINECOUNT, 0, 0);
}
//---------------------------------------------------------------------------
//Returns the length in characters of line index by character (and not indexed by line)
int TIRichEdit::LineLength(int Index)
{
  return SendMessage(Handle, EM_LINELENGTH, Index, 0);
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::WMNotify(TMessage &Message)
{
  NMHDR *Nmhdr = reinterpret_cast<NMHDR*>(Message.LParam);
  switch(Nmhdr->code)
  {
    case EN_OLEOPFAILED:
    {
      ENOLEOPFAILED *OleOpFailed = reinterpret_cast<ENOLEOPFAILED*>(Message.LParam);
      if(FOnOleError)
        FOnOleError(this, OleOpFailed->lOper, OleOpFailed->hr);
      break;
    }
    case EN_LINK:
    {
      ENLINK *Link = reinterpret_cast<ENLINK*>(Message.LParam);
      Message.Result = DoLink(Link->msg, Link->chrg.cpMin, Link->chrg.cpMax);
      break;
    }
    case EN_PROTECTED:
    {
      ENPROTECTED *Protected = reinterpret_cast<ENPROTECTED*>(Message.LParam);
      bool AllowChange = ProtectedChange;
      switch(Protected->msg)
      {
        case WM_COPY:
          AllowChange = true;
          break;

        default:
          if(OnProtectChange)
            OnProtectChange(this, Protected->chrg.cpMin, Protected->chrg.cpMax, AllowChange);
      }
      Message.Result = !AllowChange;
      break;
    }

    case EN_MSGFILTER:
    {
      MSGFILTER *Filter = reinterpret_cast<MSGFILTER*>(Message.LParam);
      //If the event is a double click on an OLE object, call OnActivateObject.
      //If the event returns false, don't process the message further.
      if(Filter->msg == WM_LBUTTONDBLCLK && ObjectSelected() && FOnActivateObject && !FOnActivateObject(this))
        Message.Result = true;
      break;
    }

    default:
      TCustomRichEdit::Dispatch(&Message);
  }
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetBackgroundColor(TColor Color)
{
  FBackgroundColor = Color;
  SendMessage(Handle, EM_SETBKGNDCOLOR, Color == clDefault, ColorToRGB(Color));
}
//---------------------------------------------------------------------------
void TIRichEdit::SetSelText(wchar_t Ch, const String &FontName, unsigned Size)
{
  SETTEXTEX SetTextEx = {ST_KEEPUNDO | ST_SELECTION, CP_ACP};
  AnsiString Temp;
  AnsiString TempFontName = FontName;
  unsigned Char = Ch;
  Temp.sprintf("{\\rtf1{\\fonttbl{\\f0\\fcharset%u %s;}}\\f0 \\fs%u \\u%u?}", GetCharset(FontName), TempFontName.c_str(), Size*2, Char);

  SendMessage(Handle, EM_SETTEXTEX, reinterpret_cast<long>(&SetTextEx), reinterpret_cast<long>(Temp.c_str()));
}
//---------------------------------------------------------------------------
void TIRichEdit::SetSelText(char Ch, const String &FontName, unsigned Size)
{
  SETTEXTEX SetTextEx = {ST_KEEPUNDO | ST_SELECTION, CP_ACP};
  AnsiString Temp;
  AnsiString TempFontName = FontName;
  unsigned char Char = Ch;
  Temp.sprintf("{\\rtf1{\\fonttbl{\\f0\\fcharset%u %s;}}\\f0 \\fs%u \\'%02X}", GetCharset(FontName), TempFontName.c_str(), Size*2, Char);

  SendMessage(Handle, EM_SETTEXTEX, reinterpret_cast<long>(&SetTextEx), reinterpret_cast<long>(Temp.c_str()));
}
//---------------------------------------------------------------------------
TParaFormat::TParaFormat(TIRichEdit *ARichEdit)
  : RichEdit(ARichEdit), TParaAttributes(ARichEdit)
{
}
//---------------------------------------------------------------------------
::PARAFORMAT2 TParaFormat::GetFormat() const
{
  ::PARAFORMAT2 Format;
  Format.cbSize = sizeof(Format);

  SendMessage(RichEdit->Handle, EM_GETPARAFORMAT, 0, reinterpret_cast<long>(&Format));
  return Format;
}
//---------------------------------------------------------------------------
TParaFormatAlignment TParaFormat::GetAlignment()
{
  return static_cast<TParaFormatAlignment>(GetFormat().wAlignment);
}
//---------------------------------------------------------------------------
void TParaFormat::SetAlignment(TParaFormatAlignment Value)
{
  ::PARAFORMAT2 Format;
  Format.cbSize = sizeof(Format);
  Format.wAlignment = Value;
  Format.dwMask = PFM_ALIGNMENT;
  SendMessage(RichEdit->Handle, EM_SETPARAFORMAT, 0, reinterpret_cast<long>(&Format));
}
//---------------------------------------------------------------------------
void TTextFormat::SetLink(bool Value)
{
  SetFormat(CFM_LINK, Value ? CFE_LINK : 0);
}
//---------------------------------------------------------------------------
bool TTextFormat::GetLink() const
{
  return GetFormat().dwEffects & CFE_LINK;
}
//---------------------------------------------------------------------------
void TTextFormat::SetProtected(bool Value)
{
  SetFormat(CFM_PROTECTED, Value ? CFE_PROTECTED : 0);
}
//---------------------------------------------------------------------------
bool TTextFormat::GetProtected() const
{
  return GetFormat().dwEffects & CFE_PROTECTED;
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetAutoUrlDetect(bool Value)
{
  SendMessage(Handle, EM_AUTOURLDETECT, Value, 0);
}
//---------------------------------------------------------------------------
bool __fastcall TIRichEdit::GetAutoUrlDetect()
{
  return SendMessage(Handle, EM_GETAUTOURLDETECT, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetOnLink(TLinkEvent Value)
{
  FOnLink = Value;
  SetEventMask(ENM_LINK, Value ? ENM_LINK : 0);
}
//---------------------------------------------------------------------------
void TIRichEdit::SetEventMask(DWORD Mask, DWORD Value)
{
  DWORD Events = SendMessage(Handle, EM_GETEVENTMASK, 0, 0);
  Events &= ~Mask;
  Events |= (Mask & Value);
  SendMessage(Handle, EM_SETEVENTMASK, 0, Events);
}
//---------------------------------------------------------------------------
bool TIRichEdit::DoLink(UINT Msg, unsigned Min, unsigned Max)
{
  switch(Msg)
  {
    case WM_LBUTTONDOWN:
      if(OnLink)
        OnLink(this, Min, Max);
      return true;

    case WM_SETCURSOR:
      ::SetCursor(Screen->Cursors[crHandPoint]);
      return true;

    default:
      return false;
  }
}
//---------------------------------------------------------------------------
String TIRichEdit::GetText(int Min, int Max)
{
  int Length = GetWindowTextLengthW(Handle);
  if(Max > Length)
    Max = Length;

  String Str;
  Str.SetLength(Length);
  ::TEXTRANGEW Range = {{Min, Max}, Str.c_str()};
  int Length2 = SendMessage(Handle, EM_GETTEXTRANGE, 0, reinterpret_cast<LONG>(&Range));
  Str.SetLength(Length2);
  return Str;
}
//---------------------------------------------------------------------------
int TIRichEdit::GetLine(int Index)
{
  return SendMessage(Handle, EM_LINEFROMCHAR, Index, 0);
}
//---------------------------------------------------------------------------
int TIRichEdit::TextSize()
{
  return GetWindowTextLengthW(Handle);
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetOnActivateObject(TActivateObjectEvent Value)
{
  FOnActivateObject = Value;
  SetEventMask(ENM_MOUSEEVENTS, Value ? ENM_MOUSEEVENTS : 0);
}
//---------------------------------------------------------------------------
void TIRichEdit::SetHostNames(const AnsiString &HostApp, const AnsiString &HostDoc)
{
  if(RichEditOle)
    RichEditOle->SetHostNames(HostApp, HostDoc);
}
//---------------------------------------------------------------------------
bool TIRichEdit::ObjectSelected()
{
  return RichEditOle ? RichEditOle->ObjectSelected() : false;
}
//---------------------------------------------------------------------------
bool TIRichEdit::OpenObject()
{
  return RichEditOle ? RichEditOle->OpenObject() : false;
}
//---------------------------------------------------------------------------
bool TIRichEdit::PasteSpecial()
{
  return RichEditOle ? RichEditOle->PasteSpecial() : false;
}
//---------------------------------------------------------------------------
bool TIRichEdit::InsertObject()
{
  return RichEditOle ? RichEditOle->InsertObject() : false;
}
//---------------------------------------------------------------------------
int CALLBACK TIRichEdit::EditWordBreakProc(LPTSTR lpch, int ichCurrent, int cch, int code)
{
  switch(code)
  {
    case WB_MOVEWORDLEFT:
      --ichCurrent;
      while(--ichCurrent)
        if(iswspace(lpch[ichCurrent]))
          return ichCurrent + 1;
      return 0;

    case WB_MOVEWORDRIGHT:
      for(;ichCurrent < cch; ichCurrent++)
        if(iswspace(lpch[ichCurrent]))
          return ichCurrent + 1;
      return cch;

    default:
      //Always wrap
      return 0;
  }
}
//---------------------------------------------------------------------------
void __fastcall TIRichEdit::SetWrapType(TWrapType Value)
{
  if(Value != FWrapType)
  {
    FWrapType = Value;
    if(!ComponentState.Contains(csDesigning))
    {
      TCustomRichEdit::WordWrap = Value != wtNone;
      if(HandleAllocated())
        SendMessage(Handle, EM_SETWORDBREAKPROC, 0, Value == wtChar ? (LONG)&EditWordBreakProc : (LONG)OldEditWordBreakProc);
    }
  }
}
//---------------------------------------------------------------------------
unsigned TIRichEdit::FindWordBreak(TWordBreak WordBreak, unsigned Pos)
{
  return SendMessage(Handle, EM_FINDWORDBREAK, WordBreak, Pos);
}
//---------------------------------------------------------------------------
bool TIRichEdit::HasOleLink()
{
  return RichEditOle->LinkCount > 0;
}
//---------------------------------------------------------------------------


