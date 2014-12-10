//---------------------------------------------------------------------------
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "Debug.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <oledlg.h>
//---------------------------------------------------------------------------
const TNameValue OleUIResultList[] =
{
  NAME_VALUE_ENTRY(OLEUI_FALSE),
  NAME_VALUE_ENTRY(OLEUI_SUCCESS),
  NAME_VALUE_ENTRY(OLEUI_OK),
  NAME_VALUE_ENTRY(OLEUI_CANCEL),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMIN),
  NAME_VALUE_ENTRY(OLEUI_ERR_OLEMEMALLOC),
  NAME_VALUE_ENTRY(OLEUI_ERR_STRUCTURENULL),
  NAME_VALUE_ENTRY(OLEUI_ERR_STRUCTUREINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_CBSTRUCTINCORRECT),
  NAME_VALUE_ENTRY(OLEUI_ERR_HWNDOWNERINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_LPSZCAPTIONINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_LPFNHOOKINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_HINSTANCEINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_LPSZTEMPLATEINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_HRESOURCEINVALID),
  NAME_VALUE_ENTRY(OLEUI_ERR_FINDTEMPLATEFAILURE),
  NAME_VALUE_ENTRY(OLEUI_ERR_LOADTEMPLATEFAILURE),
  NAME_VALUE_ENTRY(OLEUI_ERR_DIALOGFAILURE),
  NAME_VALUE_ENTRY(OLEUI_ERR_LOCALMEMALLOC),
  NAME_VALUE_ENTRY(OLEUI_ERR_GLOBALMEMALLOC),
  NAME_VALUE_ENTRY(OLEUI_ERR_LOADSTRING),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+0),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+1),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+2),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+3),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+4),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+4),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+6),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+7),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+8),
  NAME_VALUE_ENTRY(OLEUI_ERR_STANDARDMAX+9),
  NAME_VALUE_END
};

const TNameValue HResultList[] =
{
  NAME_VALUE_ENTRY(S_OK),
  NAME_VALUE_ENTRY(DV_E_DVASPECT),
  NAME_VALUE_ENTRY(OLE_S_USEREG),
  NAME_VALUE_ENTRY(E_FAIL),
  NAME_VALUE_ENTRY(E_OUTOFMEMORY),
  NAME_VALUE_ENTRY(DV_E_TYMED),
  NAME_VALUE_ENTRY(STG_E_MEDIUMFULL),
  NAME_VALUE_ENTRY(DV_E_FORMATETC),
  NAME_VALUE_ENTRY(OLEOBJ_S_INVALIDVERB),
  NAME_VALUE_ENTRY(DATA_S_SAMEFORMATETC),
  NAME_VALUE_ENTRY(OLE_E_NOCONNECTION),
  NAME_VALUE_ENTRY(E_UNEXPECTED),
  NAME_VALUE_ENTRY(STG_E_INVALIDFLAG),
  NAME_VALUE_ENTRY(STG_E_FILEALREADYEXISTS),
  NAME_VALUE_ENTRY(STG_E_INVALIDFUNCTION),
  NAME_VALUE_ENTRY(S_FALSE),
  NAME_VALUE_ENTRY(E_NOTIMPL),
  NAME_VALUE_ENTRY(E_INVALIDARG),
  NAME_VALUE_ENTRY(E_NOINTERFACE),
  NAME_VALUE_ENTRY(RPC_E_SERVERFAULT),
  NAME_VALUE_ENTRY(OLE_E_STATIC),
  NAME_VALUE_ENTRY(ERROR_MOD_NOT_FOUND),
  NAME_VALUE_END
};
//---------------------------------------------------------------------------
void AddToDebugLog(const char *Str)
{
  std::ofstream out(ChangeFileExt(Application->ExeName, ".log").c_str(), std::ios_base::app);
  if(out)
    out << Str;
}
//---------------------------------------------------------------------------
void AddToDebugLog(const AnsiString &Str)
{
  std::ofstream out(ChangeFileExt(Application->ExeName, ".log").c_str(), std::ios_base::app);
  if(out)
    out << Str.c_str();
}
//---------------------------------------------------------------------------
HRESULT DebugLogReturn(const TNameValue List[], HRESULT Result, const char *Str)
{
  std::ofstream out(ChangeFileExt(Application->ExeName, ".log").c_str(), std::ios_base::app);
  if(out)
    out << " : " << ValueToStr(List, Result).c_str() << Str;
  return Result;
}
//---------------------------------------------------------------------------
extern void __stdcall OutputDebugStringA(const char* lpOutputString);
extern void __stdcall OutputDebugStringW(const wchar_t* lpOutputString);
void OutputDebugStr(const std::string &Str) {OutputDebugStringA(Str.c_str());}
void OutputDebugStr(const std::wstring &Str) {OutputDebugStringW(Str.c_str());}

template<typename T>
class TDebugStreamBuf : public std::basic_stringbuf<T>
{
  void Update()
  {
    std::basic_string<T> Str = str();
    size_t LastIndex = 0;
    size_t Index;
    while((Index = Str.find('\n', LastIndex)) != std::string::npos)
    {
      std::basic_string<T> SubStr = Str.substr(LastIndex, Index-LastIndex);
      if(!SubStr.empty() && SubStr[0] != '\n')
        OutputDebugStr(SubStr);
      LastIndex = Index+1;
    }
    str(Str.substr(LastIndex));
  }

  int sync()
  {
    Update();
    return std::basic_stringbuf<T>::sync();
  }
};
//---------------------------------------------------------------------------
void InitDebug()
{
#pragma warn -8104
  static TDebugStreamBuf<char> DebugStreamBuf;
  static TDebugStreamBuf<wchar_t> DebugWideStreamBuf;
  std::clog.rdbuf(&DebugStreamBuf);
  std::wclog.rdbuf(&DebugWideStreamBuf);
}
//---------------------------------------------------------------------------
AnsiString ValueToStr(const TNameValue List[], unsigned Value)
{
  for(unsigned I = 0; List[I].Name != NULL; I++)
    if(Value == List[I].Value)
      return List[I].Name;
  return "0x" + IntToHex(static_cast<int>(Value), 8);
}
//---------------------------------------------------------------------------
AnsiString FlagsToStr(const TNameValue List[], unsigned Value)
{
  String Str;
  for(unsigned I = 0; List[I].Name != NULL; I++)
    if(Value & List[I].Value)
      Str += List[I].Name + String('|');
  return Str.IsEmpty() ? String(0) : Str;
}
//---------------------------------------------------------------------------

