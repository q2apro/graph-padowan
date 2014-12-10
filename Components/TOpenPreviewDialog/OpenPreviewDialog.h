//---------------------------------------------------------------------------
#ifndef OpenPreviewDialogH
#define OpenPreviewDialogH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TOpenPreviewDialog;

typedef bool __fastcall (__closure *TPreviewFileEvent)(TOpenPreviewDialog *Dialog, const String &FileName, TCanvas *Canvas, const TRect &Rect);

class TOpenPreviewDialog : public TOpenDialog
{
private:
  TPreviewFileEvent FOnPreviewFile;
  TImage *Image;
  TPanel *Panel;

  String ResolveIt(const String &FileName);

protected:
  DYNAMIC void __fastcall DoSelectionChange();
  DYNAMIC void __fastcall DoFolderChange();
  virtual void DoPreviewFile();

public:
  __fastcall TOpenPreviewDialog(TComponent* Owner);
  DYNAMIC void __fastcall DoShow();

__published:
  __property TPreviewFileEvent OnPreviewFile = {read=FOnPreviewFile, write=FOnPreviewFile, default=NULL};
};
//---------------------------------------------------------------------------
#endif
