//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop
#include <shlguid.h>
#include <shobjidl.h>
#include "OpenPreviewDialog.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TOpenPreviewDialog));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TOpenPreviewDialog *)
{
  new TOpenPreviewDialog(NULL);
}
//---------------------------------------------------------------------------
namespace Openpreviewdialog
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TOpenPreviewDialog)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TOpenPreviewDialog::TOpenPreviewDialog(TComponent* Owner)
  : TOpenDialog(Owner), FOnPreviewFile(NULL), Image(new TImage(this)), Panel(new TPanel(this))
{
  Panel->BevelInner = bvRaised;
  Panel->BevelOuter = bvLowered;

  Image->Parent = Panel;
  Image->Align = alClient;

  //Prevent flicker
  Panel->ControlStyle = Panel->ControlStyle << csOpaque;
  Panel->DoubleBuffered = true;
}
//---------------------------------------------------------------------------
String TOpenPreviewDialog::ResolveIt(const String &FileName)
{
  wchar_t szGotPath[MAX_PATH] = {0};

  // Get a pointer to the IShellLink interface.
  IShellLink* psl;
  HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL,
          CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&psl));
  if(SUCCEEDED(hres))
  {
    IPersistFile* ppf;
    // Get a pointer to the IPersistFile interface.
    hres = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&ppf));
    if(SUCCEEDED(hres))
    {
      // Load the shortcut.
      hres = ppf->Load(FileName.c_str(), STGM_READ);
      if(SUCCEEDED(hres))
      {
        // Resolve the link.
        hres = psl->Resolve(Application->Handle, SLR_ANY_MATCH);
        if(SUCCEEDED(hres))
        {
          WIN32_FIND_DATA wfd;
          // Get the path to the link target.
          hres = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_SHORTPATH);
          if(!SUCCEEDED(hres))
            szGotPath[0] = 0; // Error
        }
      }
      // Release the pointer to the IPersistFile interface.
      ppf->Release();
    }

    // Release the pointer to the IShellLink interface.
    psl->Release();
  }
  return szGotPath[0] ? String(szGotPath) : FileName;
}
//---------------------------------------------------------------------------
void __fastcall TOpenPreviewDialog::DoSelectionChange()
{
  DoPreviewFile();

  TOpenDialog::DoSelectionChange();
}
//---------------------------------------------------------------------------
void __fastcall TOpenPreviewDialog::DoFolderChange()
{
  DoPreviewFile();

  TOpenDialog::DoFolderChange();
}
//---------------------------------------------------------------------------
void TOpenPreviewDialog::DoPreviewFile()
{
  if(FOnPreviewFile)
    FOnPreviewFile(this, ResolveIt(FileName), Image->Canvas, TRect(0, 0, Image->Width, Image->Height));
}
//---------------------------------------------------------------------------
void __fastcall TOpenPreviewDialog::DoShow()
{
  const unsigned Border = 35;

  HWND Hwnd = GetParent(Handle);
  Panel->ParentWindow = Hwnd;

  TRect ClientRect;
  TRect WindowRect;
  if(!GetClientRect(Hwnd, &ClientRect))
    RaiseLastOSError();
  if(!GetWindowRect(Hwnd, &WindowRect))
    RaiseLastOSError();

  Panel->Top = ClientRect.Top + Border;
  Panel->Left = ClientRect.Right + 5;
  Panel->Width = ClientRect.Bottom - 2*Border;
  Panel->Height = ClientRect.Bottom - 2*Border;

  Image->Picture->Bitmap->Width = Image->Width;
  Image->Picture->Bitmap->Height = Image->Height;

  //Resize common dialog window
  if(!SetWindowPos(Hwnd, HWND_TOP, 0, 0, WindowRect.Width() + Panel->Width + 10, WindowRect.Height(), SWP_NOMOVE))
    RaiseLastOSError();

  TOpenDialog::DoShow();
}
//---------------------------------------------------------------------------

