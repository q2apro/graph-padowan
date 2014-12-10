//---------------------------------------------------------------------------
#ifndef Unit22H
#define Unit22H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "IRichEdit.h"
#include <Menus.hpp>
#include <ActnPopup.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
//---------------------------------------------------------------------------
class TForm22 : public TForm
{
__published:	// IDE-managed Components
  TIRichEdit *IRichEdit1;
  TPopupActionBar *PopupMenu1;
  TMenuItem *Clear1;
  void __fastcall FormHide(TObject *Sender);
  void __fastcall IRichEdit1ProtectChange(TObject *Sender, int StartPos,
          int EndPos, bool &AllowChange);
  void __fastcall IRichEdit1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Clear1Click(TObject *Sender);
  void __fastcall IRichEdit1Enter(TObject *Sender);
  void __fastcall IRichEdit1Exit(TObject *Sender);
private:	// User declarations
  int LastIndex;
  int PromptIndex;
  bool FAllowChange;
  std::vector<String> TextCache;
  int CacheIndex;
  String Command;
  int IndentLevel;

  void WritePrompt(const String &Str = ">>> ");
  void KeyboardInterrupt();
  void SetUserString(const String &Str);
  void HandleNewLine();
  void __fastcall AfterFormShown(TObject*);
	void __fastcall WMEnable(TMessage &Message);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_ENABLE, TMessage, WMEnable)
END_MESSAGE_MAP(TForm)

public:		// User declarations
  __fastcall TForm22(TComponent* Owner);
  void WriteText(const String &Str, TColor Color = clBlack);
  void Translate();
  void Clear();
};
//---------------------------------------------------------------------------
extern TForm22 *Form22;
//---------------------------------------------------------------------------
#endif
