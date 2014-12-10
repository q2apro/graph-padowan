/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
// OLESERVERIMPL : Implementation of TOleServerImpl (CoClass: OleServer, Interface: IOleServer)
#include "Graph.h"
#pragma hdrstop
#include <PngImage.hpp>
#include "OleServerImpl.h"
#include "VersionInfo.h"
#include <fstream>
#include <iomanip>
#include "ConfigFile.h"
#include "Debug.h"
#include "Unit1.h"
#include "ConfigRegistry.h"
#include "PyGraph.h"

TOleServerImpl *OleServerImpl;

const CLIPFORMAT TOleServerImpl::cfObjectDescriptor = RegisterClipboardFormat(L"Object Descriptor");
const CLIPFORMAT TOleServerImpl::cfEmbedSource = RegisterClipboardFormat(L"Embed Source");
const CLIPFORMAT TOleServerImpl::cfEmbeddedObject = RegisterClipboardFormat(L"Embedded Object");
const CLIPFORMAT TOleServerImpl::cfPng = RegisterClipboardFormat(L"PNG");

TNameValue AspectList[] =
{
  NAME_VALUE_ENTRY(DVASPECT_CONTENT),
	NAME_VALUE_ENTRY(DVASPECT_THUMBNAIL),
	NAME_VALUE_ENTRY(DVASPECT_ICON),
	NAME_VALUE_ENTRY(DVASPECT_DOCPRINT),
  NAME_VALUE_END
};

TNameValue TymedList[] =
{
  NAME_VALUE_ENTRY(TYMED_HGLOBAL),
  NAME_VALUE_ENTRY(TYMED_FILE),
  NAME_VALUE_ENTRY(TYMED_ISTREAM),
  NAME_VALUE_ENTRY(TYMED_ISTORAGE),
  NAME_VALUE_ENTRY(TYMED_GDI),
  NAME_VALUE_ENTRY(TYMED_MFPICT),
  NAME_VALUE_ENTRY(TYMED_ENHMF),
  NAME_VALUE_END
};

TNameValue AdvfList[] =
{
  NAME_VALUE_ENTRY(ADVF_NODATA),
  NAME_VALUE_ENTRY(ADVF_PRIMEFIRST),
  NAME_VALUE_ENTRY(ADVF_ONLYONCE),
  NAME_VALUE_ENTRY(ADVF_DATAONSTOP),
  NAME_VALUE_ENTRY(ADVFCACHE_NOHANDLER),
  NAME_VALUE_ENTRY(ADVFCACHE_FORCEBUILTIN),
  NAME_VALUE_ENTRY(ADVFCACHE_ONSAVE),
  NAME_VALUE_END
};

/////////////////////////////////////////////////////////////////////////////
// TOleServerImpl
TOleServerImpl::TOleServerImpl()
  : OleClientSite(NULL), OleAdviseHolder(NULL), DataAdviseHolder(NULL), DataIsDirty(false),
    Width(Form1->Image1->Width), Height(Form1->Image1->Height)
{
#ifdef _DEBUG
  std::ofstream out(ChangeFileExt(Application->ExeName, ".log").c_str(), std::ios_base::app);
  if(out)
  {
    out << std::endl << std::endl;
//    out << "PID: 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << GetCurrentProcessId() << std::endl;
//    out << "Date: " << DateTimeToStr(Now()) << std::endl;
    out << __FUNC__ << "(CmdLine=" << ::ToString(CmdLine) << ")";
  }
#endif
  OleServerImpl = this;
}
//---------------------------------------------------------------------------
TOleServerImpl::~TOleServerImpl()
{
  DEBUG_CALL();
  OleServerImpl = NULL;

  //Release saved pointers
  ReleaseCom(OleAdviseHolder);
  ReleaseCom(DataAdviseHolder);
}
//---------------------------------------------------------------------------
template<typename T> void TOleServerImpl::ReleaseCom(T *&Unknown)
{
  if(Unknown)
    Unknown->Release();
  Unknown = NULL;
}
//---------------------------------------------------------------------------
String TOleServerImpl::ClipboardFormatToStr(CLIPFORMAT Format)
{
  char Str[100];
  switch(Format)
  {
    case 0:               return "0";
    case CF_TEXT:         return "CF_TEXT";
    case CF_BITMAP:       return "CF_BITMAP";
    case CF_METAFILEPICT: return "CF_METAFILEPICT";
    case CF_SYLK:         return "CF_SYLK";
    case CF_DIF:          return "CF_DIF";
    case CF_TIFF:         return "CF_TIFF";
    case CF_OEMTEXT:      return "CF_OEMTEXT";
    case CF_DIB:          return "CF_DIB";
    case CF_PALETTE:      return "CF_PALETTE";
    case CF_PENDATA:      return "CF_PENDATA";
    case CF_RIFF:         return "CF_RIFF";
    case CF_WAVE:         return "CF_WAVE";
    case CF_UNICODETEXT:  return "CF_UNICODETEXT";
    case CF_ENHMETAFILE:  return "CF_ENHMETAFILE";
    case CF_HDROP:        return "CF_HDROP";
    case CF_LOCALE:       return "CF_LOCALE";
    case CF_DIBV5:        return "CF_DIBV5";
    default:
      GetClipboardFormatNameA(Format, Str, sizeof(Str));
      return "\"" + String(Str) + "\"";
  }
}
//---------------------------------------------------------------------------
String TOleServerImpl::DeviceToStr(const DVTARGETDEVICE *Device)
{
  if(Device == NULL)
    return "NULL";
  String Str = "[";
  if(Device->tdDriverNameOffset)
    Str += "DriverName=" + String((wchar_t*)((char*)Device + Device->tdDriverNameOffset));
  if(Device->tdDeviceNameOffset)
    Str += ",DeviceName=" + String((wchar_t*)((char*)Device + Device->tdDeviceNameOffset));
  return Str + "]";
}
//---------------------------------------------------------------------------
void TOleServerImpl::LogException()
{
  std::ofstream out(ChangeFileExt(Application->ExeName, ".log").c_str(), std::ios_base::app);
  if(out)
    try
    {
      throw;
    }
    catch(Exception &E)
    {
      out << "\nCaught exception with message '" << E.Message << "'" << std::endl;
    }
    catch(...)
    {
      out << "\nCaught unknown exception" << std::endl;
    }
}
//---------------------------------------------------------------------------
bool TOleServerImpl::CheckRegistration()
{
  try
  {
    std::wstring ClassKey = L"CLSID\\" + ToWString(Comobj::GUIDToString(CLSID_OleServer));
    std::wstring ProgID = GetProgID();

    if(GetRegValue(ClassKey + L"\\LocalServer32", L"", HKEY_CLASSES_ROOT, L"")
        == ToWString(Application->ExeName) + L" /automation" &&
        GetRegValue(ProgID + L"\\shell\\open\\command", L"", HKEY_CLASSES_ROOT, L"")
        == L"\"" + ToWString(Application->ExeName) + L"\" \"%1\"")
      return true;
  }
  catch(...)
  {
  }
  return false;
}
//---------------------------------------------------------------------------
bool TOleServerImpl::Register(bool AllUsers)
{
  try
  {
    String Clsid = Comobj::GUIDToString(CLSID_OleServer);
    String ClassKey = "Software\\Classes\\CLSID\\" + Comobj::GUIDToString(CLSID_OleServer);
    String ProgID = "Software\\Classes\\" + String(GetProgID());
    DWORD RootKey = reinterpret_cast<DWORD>(AllUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);

    CreateRegKey(ClassKey, "", LoadRes(RES_OLE_GRAPH_SYSTEM), RootKey);
    CreateRegKey(ClassKey + "\\InprocHandler32", "", "OLE32.DLL", RootKey);
    CreateRegKey(ClassKey + "\\Insertable", "", "", RootKey);

    //The /automation switch is needed to prevent an extra instance that doesn't close when inserting from file in Word; Don't know why
    //Don't use Quotes; Quotes prevents arguments from being passed under Windows 9x
    CreateRegKey(ClassKey + "\\LocalServer32", "", Application->ExeName + " /automation", RootKey);
    CreateRegKey(ClassKey + "\\Verb\\0", "", "&" + LoadRes(RES_OLE_OPEN) + L",0,2", RootKey);
    CreateRegKey(ClassKey + "\\DefaultIcon", "", "\"" + Application->ExeName + L"\",1", RootKey);
    CreateRegKey(ClassKey + "\\AuxUserType\\2", "", LoadRes(RES_OLE_GRAPH_SYSTEM), RootKey);
    CreateRegKey(ClassKey + "\\AuxUserType\\3", "", "Graph", RootKey);
    CreateRegKey(ClassKey + "\\MiscStatus", "", OLEMISC_RENDERINGISDEVICEINDEPENDENT, RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\DefaultFile", "", "Embed Souce", RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\GetSet\\0", "", String("Embed Source,") + DVASPECT_CONTENT + "," + TYMED_ISTORAGE + "," + DATADIR_GET, RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\GetSet\\1", "", String(CF_ENHMETAFILE) + "," + DVASPECT_CONTENT + "," + TYMED_ENHMF + "," + DATADIR_GET, RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\GetSet\\2", "", String(CF_METAFILEPICT) + "," + DVASPECT_CONTENT + "," + TYMED_MFPICT + "," + DATADIR_GET, RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\GetSet\\3", "", String(CF_BITMAP) + "," + DVASPECT_CONTENT + "," + TYMED_GDI + "," + DATADIR_GET, RootKey);
    CreateRegKey(ClassKey + "\\DataFormats\\GetSet\\4", "", String("PNG") + "," + DVASPECT_CONTENT + "," + TYMED_ISTREAM + "," + DATADIR_GET, RootKey);
    CreateRegKey(ClassKey + "\\ProgID", "", GetProgID(), RootKey);

    CreateRegKey(ProgID, "", LoadRes(RES_OLE_GRAPH_SYSTEM), RootKey);
    CreateRegKey(ProgID + "\\DefaultIcon", "", Application->ExeName + ",1", RootKey);
    CreateRegKey(ProgID + "\\shell\\open\\command", "", "\"" + Application->ExeName + "\" \"%1\"", RootKey);
    CreateRegKey(ProgID + "\\Insertable", "", "", RootKey);
    CreateRegKey(ProgID + "\\Clsid", "", Clsid, RootKey);

    //Remove local registration to prevent it from overwriting the global one
    if(AllUsers)
    {
      RemoveRegistryKey(ToWString(ClassKey), HKEY_CURRENT_USER);
      RemoveRegistryKey(ToWString(ProgID), HKEY_CURRENT_USER);
    }
    return true;
  }
  catch(...)
  {
    return false;
  }
}
//---------------------------------------------------------------------------
void UpgradeSettings()
{
  //Old default font (Times New Roman) is changed to Tahoma to support angle symbol
  std::wstring VersionStr = GetRegValue(REGISTRY_KEY, L"Version", HKEY_CURRENT_USER, L"");
  if(TVersion(VersionStr) < TVersion(4,4,0,456))
    SetRegValue(REGISTRY_KEY "\\Property", L"DefaultPointLabelFont", HKEY_CURRENT_USER, DEFAULT_POINT_FONT);
}
//---------------------------------------------------------------------------
HRESULT WINAPI TOleServerImpl::UpdateRegistry(BOOL bRegister)
{
  try
  {
    String ClassKey = "CLSID\\" + Comobj::GUIDToString(CLSID_OleServer);
    String ProgID = GetProgID();

    //Set "DontRegisterOLE" to 1 in the registry to disable writing to registry when Graph is started
    //This is for debuging purpose
    if(GetRegValue(REGISTRY_KEY, L"DontRegisterOLE", HKEY_CURRENT_USER, 0))
      return S_OK;

    if(bRegister)
    {
      UpgradeSettings();
      //Update version info to last registered version for current user
      CreateRegKey(REGISTRY_KEY, L"Version", TVersionInfo().ProductVersion().Text().c_str(), (unsigned)HKEY_CURRENT_USER);

      //If "Install for all users" was selected under installation,
      //first try to register for all users. If that fails, register for current user only.
      //Only install if the path is different from the current installation.
      //This prevent a registration for each user if there is a machine wide registration available.
      //As far as I know it is not possible to register for current user only under Windows 9x
      if(FindCmdLineSwitch("REGSERVER") || !CheckRegistration())
        if(GetRegValue(REGISTRY_KEY, L"InstallAllUsers", HKEY_CURRENT_USER, 0))
        {
          if(!Register(true))
            Register(false);
        }
        else
          Register(false);
    }
    else
    {
      //SHDeleteKey() deletes a key and all subkeys
      RemoveRegistryKey(ClassKey.c_str(), HKEY_CLASSES_ROOT);
      RemoveRegistryKey(ProgID.c_str(), HKEY_CLASSES_ROOT);
      RemoveRegistryKey(L".grf", HKEY_CLASSES_ROOT);
    }

    return S_OK;
  }
  catch(...)
  {
    return E_FAIL;
  }
}
//---------------------------------------------------------------------------
void TOleServerImpl::DrawMetafile(TMetafile *Metafile, unsigned ImageWidth, unsigned ImageHeight)
{
  Metafile->Width = ImageWidth;
  Metafile->Height = ImageHeight;

  std::auto_ptr<TMetafileCanvas> MetaCanvas(new TMetafileCanvas(Metafile, 0));

  Form1->Draw.Wait(); //Don't copy data while it might be in use
  TData MetaData(Form1->Data);
  TDraw DrawMeta(MetaCanvas.get(), &MetaData, false, "OLE Metafile DrawThread");

  //Set width and height
  DrawMeta.SetSize(ImageWidth, ImageHeight);
  DrawMeta.DrawAll();
  DrawMeta.Wait();
}
//---------------------------------------------------------------------------
HMETAFILE TOleServerImpl::ConvertEnhMetaToMeta(HENHMETAFILE hemf)
{
  HMETAFILE hMF = 0;
  HDC dc = GetDC(0);
  int Size = GetWinMetaFileBits(hemf, 0, NULL, MM_ANISOTROPIC, dc);
  if(Size)
  {
    std::vector<unsigned char> Buffer(Size);
    GetWinMetaFileBits(hemf, Buffer.size(), &Buffer[0], MM_ANISOTROPIC, dc);
    hMF = SetMetaFileBitsEx(Buffer.size(), &Buffer[0]);
  }
  ReleaseDC(0, dc);
  return hMF;
}
//---------------------------------------------------------------------------
void TOleServerImpl::SendAdvise(TAdviseCode AdviseCode)
{
  switch(AdviseCode)
  {
    case acDataChanged:
      if(DataAdviseHolder != NULL)
        LOG_FUNCTION_CALL(DataAdviseHolder->SendOnDataChange(this, 0, 0));
      DataIsDirty = true;
      break;

    case acDataSaved:
      if(OleAdviseHolder != NULL)
        LOG_FUNCTION_CALL(OleAdviseHolder->SendOnSave());
      DataIsDirty = false;
      break;

    case acClose:
      if(DataAdviseHolder != NULL)
        LOG_FUNCTION_CALL(DataAdviseHolder->SendOnDataChange(this, 0, ADVF_DATAONSTOP));

      if(OleAdviseHolder != NULL)
        LOG_FUNCTION_CALL(OleAdviseHolder->SendOnClose());
      break;

    case acRenamed:
      break;

    case acSaveObject:
      if(OleClientSite != NULL)
        LOG_FUNCTION_CALL(OleClientSite->SaveObject());
      break;

    case acShowWindow:
      if(OleClientSite != NULL)
        LOG_FUNCTION_CALL(OleClientSite->OnShowWindow(true));
      break;

    case acHideWindow:
      if(OleClientSite != NULL)
        LOG_FUNCTION_CALL(OleClientSite->OnShowWindow(false));
      break;

    case acShowObject:
      if(OleClientSite != NULL)
        LOG_FUNCTION_CALL(OleClientSite->ShowObject());
      break;
  }
}
//---------------------------------------------------------------------------
void SendOleAdvise(TAdviseCode AdviseCode)
{
  if(OleServerImpl != NULL)
    OleServerImpl->SendAdvise(AdviseCode);
}
//---------------------------------------------------------------------------
bool OleServerRunning()
{
  return OleServerImpl != NULL;
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// IOleObject
HRESULT STDMETHODCALLTYPE TOleServerImpl::SetClientSite(
    /* [unique][in] */ IOleClientSite *pClientSite)
{
  DEBUG_CALL();

  ReleaseCom(OleClientSite);

  OleClientSite = pClientSite;
  if(OleClientSite)
    OleClientSite->AddRef();
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetClientSite(
    /* [out] */ IOleClientSite **ppClientSite)
{
  DEBUG_CALL();
  *ppClientSite = OleClientSite;
  OleClientSite->AddRef(); //Apparently we need to call AddRef(). See Inside OLE
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SetHostNames(
    /* [in] */ LPCOLESTR szContainerApp,
    /* [unique][in] */ LPCOLESTR szContainerObj)
{
  DEBUG_CALL();
  LOG_ARG("App=" + szContainerApp + ", Obj=" + szContainerObj);
  Application->MainForm->Caption = "Graph - " + String(szContainerApp) + " (" + szContainerObj + ")";
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Close(
    /* [in] */ DWORD dwSaveOption)
{
  DEBUG_CALL();
  LOG_ARG("dwSaveOption=" + dwSaveOption);

  if(OleClientSite)
  {
    if(dwSaveOption != OLECLOSE_NOSAVE && DataIsDirty)
    {
      SendAdvise(acSaveObject);
      SendAdvise(acDataSaved);
    }

    SendAdvise(acHideWindow);
    SendAdvise(acShowObject);
  }
  else
    //If no OleClientSite then this must be a linked object and we have to save it ourself
    if(DataIsDirty && !Form1->Data.GetFileName().empty())
      if(dwSaveOption != OLECLOSE_NOSAVE)
        Form1->Data.Save(Form1->Data.GetFileName(), true);
      else
      { //If file should not be saved; Reload data and update image
        Form1->LoadFromFile(Form1->Data.GetFileName().c_str(), false, false);
        SendAdvise(acDataChanged);
      }

  SendAdvise(acClose);

  ReleaseCom(OleClientSite);

  //Cut off all clients
  LOG_FUNCTION_CALL(CoDisconnectObject(static_cast<IOleObject*>(this), 0));
  LOG_FUNCTION_CALL(CoDisconnectObject(static_cast<IDataObject*>(this), 0));
  LOG_FUNCTION_CALL(CoDisconnectObject(static_cast<IPersistStorage*>(this), 0));
  LOG_FUNCTION_CALL(CoDisconnectObject(static_cast<IPersistFile*>(this), 0));

  LOG_FUNCTION_CALL(CoLockObjectExternal(static_cast<IOleObject*>(this), false, true));

  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SetMoniker(
    /* [in] */ DWORD dwWhichMoniker,
    /* [unique][in] */ IMoniker *pmk)
{
  DEBUG_CALL();
  LOG_ARG("dwWhichMoniker=" + dwWhichMoniker);

  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetMoniker(
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ IMoniker **ppmk)
{
  DEBUG_CALL();
  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::InitFromData(
    /* [unique][in] */ IDataObject *pDataObject,
    /* [in] */ BOOL fCreation,
    /* [in] */ DWORD dwReserved)
{
  DEBUG_CALL();
  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetClipboardData(
    /* [in] */ DWORD dwReserved,
    /* [out] */ IDataObject **ppDataObject)
{
  DEBUG_CALL();
  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::DoVerb(
    /* [in] */ LONG iVerb,
    /* [unique][in] */ LPMSG lpmsg,
    /* [unique][in] */ IOleClientSite *pActiveSite,
    /* [in] */ LONG lindex,
    /* [in] */ HWND hwndParent,
    /* [unique][in] */ LPCRECT lprcPosRect)
{
  DEBUG_CALL();
  LOG_ARG("iVerb=" + iVerb);

  try
  {
    switch(iVerb)
    {
      case OLEIVERB_SHOW:
      case OLEIVERB_PROPERTIES:
      case OLEIVERB_PRIMARY:
      case OLEIVERB_OPEN:
      case 1:
      {
        //Only call OnShowWindow() on embedded objects and not on linked objects
        SendAdvise(acShowWindow);
        Form1->ActivateOleUserInterface();

        int NewWidth = GetWidth();
        int NewHeight = GetHeight();
        Application->MainForm->Show(); //Warning The different control sizes are not updated until the form is shown

        NewWidth += Form1->Width - Form1->Image1->ClientWidth;
        NewHeight += Form1->Height - Form1->Image1->ClientHeight;
        Form1->SetBounds(Form1->Left, Form1->Top, NewWidth, NewHeight);

        SendAdvise(acShowObject);

        //This locks the user interface until either the user or the client closes the user interface
        LOG_FUNCTION_CALL(CoLockObjectExternal(static_cast<IOleObject*>(this), true, true));

        SetForegroundWindow(Form1->Handle);
        break;
      }

      case OLEIVERB_HIDE:
        Form1->Hide();
        SendAdvise(acHideWindow);
        break;

      case OLEIVERB_UIACTIVATE:
      case OLEIVERB_INPLACEACTIVATE:
        return LOG_RESULT(E_NOTIMPL);

      default:
        return LOG_RESULT(OLEOBJ_S_INVALIDVERB);
    }
    return LOG_RESULT(S_OK);
  }
  catch(...)
  {
    LogException();
    return LOG_RESULT(E_FAIL);
  }
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::EnumVerbs(
    /* [out] */ IEnumOLEVERB **ppEnumOleVerb)
{
  DEBUG_CALL();
  return LOG_RESULT(OLE_S_USEREG);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Update(void)
{
  DEBUG_CALL();
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::IsUpToDate(void)
{
  DEBUG_CALL();
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetUserClassID(
  /* [out] */ CLSID *pClsid)
{
  DEBUG_CALL();
  *pClsid = CLSID_OleServer;
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetUserType(
  /* [in] */ DWORD dwFormOfType,
  /* [out] */ LPOLESTR *pszUserType)
{
  DEBUG_CALL();
  return LOG_RESULT(OLE_S_USEREG);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SetExtent(
  /* [in] */ DWORD dwDrawAspect,
  /* [in] */ SIZEL *pSizel)
{
  DEBUG_CALL();

  SIZEL Size;
  AtlHiMetricToPixel(pSizel, &Size);

  LOG_ARG("DrawAspect=" + FlagsToStr(AspectList, dwDrawAspect) + ", cx=" + Size.cx + ", cy=" + Size.cy);

  if(dwDrawAspect != DVASPECT_CONTENT)
    return LOG_RESULT(DV_E_DVASPECT);

  if(Size.cx != Width || Size.cy != Height)
  {
    Form1->Draw.AbortUpdate();
    SetSize(Size.cx, Size.cy);
    Form1->Data.ClearCache();
    Form1->Redraw();
    SendAdvise(acDataChanged);
  }

  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetExtent(
  /* [in] */ DWORD dwDrawAspect,
  /* [out] */ SIZEL *pSizel)
{
  DEBUG_CALL();
  if(dwDrawAspect != DVASPECT_CONTENT)
    return LOG_RESULT(DV_E_DVASPECT);

  SIZEL Size = {GetWidth(), GetHeight()};
  LOG_DATA("cx=" + Size.cx);
  LOG_DATA("cy=" + Size.cy);
  AtlPixelToHiMetric(&Size, pSizel);

  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Advise(
  /* [unique][in] */ IAdviseSink *pAdvSink,
  /* [out] */ DWORD *pdwConnection)
{
  DEBUG_CALL();
  if(OleAdviseHolder == NULL)
    if(FAILED(LOG_FUNCTION_CALL(CreateOleAdviseHolder(&OleAdviseHolder))))
      return LOG_RESULT(E_FAIL);
  return LOG_RESULT(LOG_FUNCTION_CALL(OleAdviseHolder->Advise(pAdvSink, pdwConnection)));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Unadvise(
  /* [in] */ DWORD dwConnection)
{
  DEBUG_CALL();
  if(OleAdviseHolder == NULL)
    return LOG_RESULT(E_FAIL);
  return LOG_RESULT(LOG_FUNCTION_CALL(OleAdviseHolder->Unadvise(dwConnection)));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::EnumAdvise(
  /* [out] */ IEnumSTATDATA **ppenumAdvise)
{
  DEBUG_CALL();
  if(OleAdviseHolder == NULL)
    return LOG_RESULT(E_FAIL);
  return LOG_RESULT(LOG_FUNCTION_CALL(OleAdviseHolder->EnumAdvise(ppenumAdvise)));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetMiscStatus(
  /* [in] */ DWORD dwAspect,
  /* [out] */ DWORD *pdwStatus)
{
  DEBUG_CALL();
  return LOG_RESULT(OLE_S_USEREG);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SetColorScheme(
  /* [in] */ LOGPALETTE *pLogpal)
{
  DEBUG_CALL();
  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// IDataObject
/* [local] */ HRESULT STDMETHODCALLTYPE TOleServerImpl::GetData(
  /* [unique][in] */ FORMATETC *pFormatetcIn,
  /* [out] */ STGMEDIUM *pmedium)
{
  DEBUG_CALL();
  LOG_ARG("cfFormat=" + ClipboardFormatToStr(pFormatetcIn->cfFormat) + ", tymed=" + FlagsToStr(TymedList, pFormatetcIn->tymed)
    + ", dwAspect=" + ValueToStr(AspectList, pFormatetcIn->dwAspect) + ", ptd=" + DeviceToStr(pFormatetcIn->ptd));

  pmedium->pUnkForRelease = NULL;
  if(pFormatetcIn->dwAspect != DVASPECT_CONTENT && pFormatetcIn->dwAspect != DVASPECT_THUMBNAIL)
    return LOG_RESULT(DV_E_FORMATETC);

  int ImageWidth = pFormatetcIn->dwAspect == DVASPECT_THUMBNAIL ? 120 : GetWidth();
  int ImageHeight = pFormatetcIn->dwAspect == DVASPECT_THUMBNAIL ? 120 : GetHeight();

  try
  {
    if(pFormatetcIn->cfFormat == CF_ENHMETAFILE && (pFormatetcIn->tymed & TYMED_ENHMF))
    {
      LOG_DATA("Width=" + ImageWidth + ", Height=" + ImageHeight);
      std::auto_ptr<TMetafile> Metafile(new TMetafile);
      DrawMetafile(Metafile.get(), ImageWidth, ImageHeight);

      pmedium->tymed = TYMED_ENHMF;
      pmedium->hEnhMetaFile = reinterpret_cast<HENHMETAFILE>(Metafile->ReleaseHandle());
    }
    else if(pFormatetcIn->cfFormat == CF_METAFILEPICT && (pFormatetcIn->tymed & TYMED_MFPICT))
    {
      LOG_DATA("Width=" + ImageWidth + ", Height=" + ImageHeight);
      std::auto_ptr<TMetafile> Metafile(new TMetafile);
      DrawMetafile(Metafile.get(), ImageWidth, ImageHeight);

      // Get memory handle
      HGLOBAL hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(METAFILEPICT));
      if(!hMem)
        return STG_E_MEDIUMFULL;

      METAFILEPICT *MetaPict = static_cast<METAFILEPICT*>(GlobalLock(hMem));
      if(!MetaPict)
        return STG_E_MEDIUMFULL;

      MetaPict->hMF  = ConvertEnhMetaToMeta(reinterpret_cast<HENHMETAFILE>(Metafile->Handle));
      MetaPict->mm   = MM_ANISOTROPIC;
      MetaPict->xExt = Metafile->MMWidth;
      MetaPict->yExt = Metafile->MMHeight;
      GlobalUnlock(hMem);

      pmedium->tymed = TYMED_MFPICT;
      pmedium->hGlobal = hMem;
    }
    else if(pFormatetcIn->cfFormat == CF_BITMAP && (pFormatetcIn->tymed & TYMED_GDI))
    {
      LOG_DATA("Width=" + ImageWidth + ", Height=" + ImageHeight + ", ImageWidth=" + Form1->Image1->Width + ", ImageHeight=" + Form1->Image1->Height);
      Form1->Draw.Wait();
      std::auto_ptr<Graphics::TBitmap> Bitmap(new Graphics::TBitmap);
      Bitmap->Width = ImageWidth;
      Bitmap->Height = ImageHeight;
      Bitmap->Canvas->CopyRect(TRect(0, 0, ImageWidth, ImageHeight), Form1->Image1->Canvas, Form1->Image1->ClientRect);
      pmedium->tymed = TYMED_GDI;
      pmedium->hBitmap = Bitmap->ReleaseHandle();
    }
    else if(pFormatetcIn->cfFormat == cfObjectDescriptor && (pFormatetcIn->tymed & TYMED_HGLOBAL))
    {
      String UserTypeName = LoadRes(RES_OLE_GRAPH_SYSTEM);
      String SrcOfCopy = Form1->Data.GetFileName().c_str();

      //Get memory handle; Remember memory for text and zero terminations (zero termination is 2*2 bytes)
      HGLOBAL hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(OBJECTDESCRIPTOR) + 2*UserTypeName.Length() + 2*SrcOfCopy.Length() + 4);
      if(!hMem)
        return STG_E_MEDIUMFULL;

      OBJECTDESCRIPTOR *ObjectDescriptor = static_cast<OBJECTDESCRIPTOR*>(GlobalLock(hMem));
      if(!ObjectDescriptor)
        return STG_E_MEDIUMFULL;

      memset(ObjectDescriptor, 0, sizeof(OBJECTDESCRIPTOR));
      ObjectDescriptor->cbSize = sizeof(OBJECTDESCRIPTOR);
      ObjectDescriptor->clsid = CLSID_OleServer;
      ObjectDescriptor->dwDrawAspect = DVASPECT_CONTENT;
      ObjectDescriptor->sizel.cx = 0;
      ObjectDescriptor->sizel.cy = 0;
      ObjectDescriptor->pointl.x = 0;
      ObjectDescriptor->pointl.y = 0;
      ObjectDescriptor->dwStatus = OLEMISC_RENDERINGISDEVICEINDEPENDENT;
      ObjectDescriptor->dwFullUserTypeName = sizeof(OBJECTDESCRIPTOR); //Index to User Type Name
      ObjectDescriptor->dwSrcOfCopy = sizeof(OBJECTDESCRIPTOR) + 2*UserTypeName.Length() + 2; //Index to Src Of Copy
      wcscpy(reinterpret_cast<wchar_t*>(ObjectDescriptor) + ObjectDescriptor->dwFullUserTypeName/2, UserTypeName.c_str());
      wcscpy(reinterpret_cast<wchar_t*>(ObjectDescriptor) + ObjectDescriptor->dwSrcOfCopy/2, SrcOfCopy.IsEmpty() ? L"" : SrcOfCopy.c_str());

      GlobalUnlock(hMem);

      pmedium->tymed = TYMED_HGLOBAL;
      pmedium->hGlobal = hMem;
    }
    else if(pFormatetcIn->cfFormat == cfEmbedSource && (pFormatetcIn->tymed & TYMED_ISTORAGE))
    {
      pmedium->tymed = TYMED_ISTORAGE;
      ILockBytes *LockBytes = NULL;
      HRESULT Result = LOG_FUNCTION_CALL(CreateILockBytesOnHGlobal(NULL, TRUE, &LockBytes));
      if(FAILED(Result))
        return LOG_RESULT(Result);

      Result = LOG_FUNCTION_CALL(StgCreateDocfileOnILockBytes(LockBytes, STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, &pmedium->pstg));
      if(FAILED(Result))
        return LOG_RESULT(Result);

      return LOG_RESULT(OleSave(this, pmedium->pstg, FALSE));
    }
    else
      return LOG_RESULT(DV_E_FORMATETC);
    return LOG_RESULT(S_OK);
  }
  catch(...)
  {
    return LOG_RESULT(E_FAIL);
  }
}
//---------------------------------------------------------------------------
/* [local] */ HRESULT STDMETHODCALLTYPE TOleServerImpl::GetDataHere(
  /* [unique][in] */ FORMATETC *pformatetc,
  /* [out][in] */ STGMEDIUM *pmedium)
{
  DEBUG_CALL();
  LOG_ARG("cfFormat=" + ClipboardFormatToStr(pformatetc->cfFormat) + ", tymed=" + FlagsToStr(TymedList, pformatetc->tymed)
    + ", dwAspect=" + ValueToStr(AspectList, pformatetc->dwAspect) + ", ptd=" + DeviceToStr(pformatetc->ptd));

  int ImageWidth = pformatetc->dwAspect == DVASPECT_THUMBNAIL ? 120 : GetWidth();
  int ImageHeight = pformatetc->dwAspect == DVASPECT_THUMBNAIL ? 120 : GetHeight();
  pmedium->pUnkForRelease = NULL;

  if(pformatetc->cfFormat == cfEmbedSource && (pformatetc->tymed & TYMED_ISTORAGE))
    return LOG_RESULT(OleSave(this, pmedium->pstg, FALSE));
  else if(pformatetc->cfFormat == cfPng && (pformatetc->tymed & TYMED_ISTREAM))
  {
    LOG_DATA("Width=" + ImageWidth + ", Height=" + ImageHeight);
    Form1->Draw.Wait();
    std::auto_ptr<TPngImage> Image(new TPngImage(COLOR_RGB, 8, ImageWidth, ImageHeight));
    Image->Canvas->CopyRect(TRect(0, 0, ImageWidth, ImageHeight), Form1->Image1->Canvas, Form1->Image1->ClientRect);
    std::auto_ptr<TMemoryStream> Stream(new TMemoryStream);
    Image->SaveToStream(Stream.get());
    return LOG_RESULT(pmedium->pstm->Write(Stream->Memory, Stream->Size, NULL));
  }

  return LOG_RESULT(DV_E_FORMATETC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::QueryGetData(
  /* [unique][in] */ FORMATETC *pFormatetc)
{
  DEBUG_CALL();
  LOG_ARG("cfFormat=" + ClipboardFormatToStr(pFormatetc->cfFormat) + ", tymed=" + FlagsToStr(TymedList, pFormatetc->tymed)
    + ", dwAspect=" + ValueToStr(AspectList, pFormatetc->dwAspect) + ", ptd=" + DeviceToStr(pFormatetc->ptd));

  if(pFormatetc->tymed & (TYMED_MFPICT | TYMED_ENHMF))
    return LOG_RESULT(S_OK);
  else if(pFormatetc->cfFormat == cfObjectDescriptor)
    return LOG_RESULT(S_OK);
  else if(pFormatetc->cfFormat == cfEmbedSource)
    return LOG_RESULT(S_OK);

  return LOG_RESULT(DV_E_FORMATETC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetCanonicalFormatEtc(
  /* [unique][in] */ FORMATETC *pformatectIn,
  /* [out] */ FORMATETC *pformatetcOut)
{
  DEBUG_CALL();
  LOG_ARG("cfFormat=" + ClipboardFormatToStr(pformatectIn->cfFormat) + ", dwAspect=" + ValueToStr(AspectList, pformatectIn->dwAspect)
    + ", ptdIn=" + DeviceToStr(pformatectIn->ptd) + ", ptdOut=" + DeviceToStr(pformatetcOut->ptd));

  //From the API documentation:
  //For data objects that never provide device-specific renderings,
  //the simplest implementation of this method is to copy the input FORMATETC to the
  //output FORMATETC, store a NULL in the ptd member of the output FORMATETC,
  //and return DATA_S_SAMEFORMATETC.
  *pformatetcOut = *pformatectIn;
  pformatetcOut->ptd = NULL;
  return LOG_RESULT(DATA_S_SAMEFORMATETC);
}
//---------------------------------------------------------------------------
/* [local] */ HRESULT STDMETHODCALLTYPE TOleServerImpl::SetData(
  /* [unique][in] */ FORMATETC *pformatetc,
  /* [unique][in] */ STGMEDIUM *pmedium,
  /* [in] */ BOOL fRelease)
{
  DEBUG_CALL();
  return LOG_RESULT(E_NOTIMPL);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::EnumFormatEtc(
  /* [in] */ DWORD dwDirection,
  /* [out] */ IEnumFORMATETC **ppenumFormatEtc)
{
  DEBUG_CALL();
  HRESULT Result = OleRegEnumFormatEtc(CLSID_OleServer, dwDirection, ppenumFormatEtc);
  return LOG_RESULT(Result);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::DAdvise(
  /* [in] */ FORMATETC *pFormatetc,
  /* [in] */ DWORD advf,
  /* [unique][in] */ IAdviseSink *pAdvSink,
  /* [out] */ DWORD *pdwConnection)
{
  DEBUG_CALL();
  LOG_ARG("cfFormat=" + ClipboardFormatToStr(pFormatetc->cfFormat) + ", tymed=" + FlagsToStr(TymedList, pFormatetc->tymed) + ", advf=" + FlagsToStr(AdvfList, advf));

  if(pFormatetc->cfFormat != 0 && //Wildcard, not very well documented
     pFormatetc->cfFormat != CF_METAFILEPICT &&
     pFormatetc->cfFormat != CF_ENHMETAFILE &&
     pFormatetc->cfFormat != CF_BITMAP &&
     pFormatetc->cfFormat != cfObjectDescriptor &&
     pFormatetc->cfFormat != cfEmbedSource &&
     pFormatetc->cfFormat != cfPng)
      return LOG_RESULT(DV_E_FORMATETC);

  if(!DataAdviseHolder)
    if(FAILED(LOG_FUNCTION_CALL(CreateDataAdviseHolder(&DataAdviseHolder))))
      return LOG_RESULT(E_OUTOFMEMORY);

  return LOG_RESULT(LOG_FUNCTION_CALL(DataAdviseHolder->Advise(this, pFormatetc, advf, pAdvSink, pdwConnection)));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::DUnadvise(
  /* [in] */ DWORD dwConnection)
{
  DEBUG_CALL();
  if(!DataAdviseHolder)
    return LOG_RESULT(E_FAIL);
  return LOG_RESULT(LOG_FUNCTION_CALL(DataAdviseHolder->Unadvise(dwConnection)));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::EnumDAdvise(
  /* [out] */ IEnumSTATDATA **ppenumAdvise)
{
  DEBUG_CALL();
  if(!DataAdviseHolder)
    return LOG_RESULT(E_FAIL);
  return LOG_RESULT(LOG_FUNCTION_CALL(DataAdviseHolder->EnumAdvise(ppenumAdvise)));
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// IPersistStorage
HRESULT STDMETHODCALLTYPE TOleServerImpl::IsDirty(void)
{
  DEBUG_CALL();
  return LOG_RESULT(DataIsDirty ? S_OK : S_FALSE);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::InitNew(
  /* [unique][in] */ IStorage *pStg)
{
  DEBUG_CALL();
  Form1->LoadDefault();
  Form1->Redraw();
  DataIsDirty = true; //We always want to save a new object; Even if nothing was done
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Load(
  /* [unique][in] */ IStorage *pStg)
{
  DEBUG_CALL();
  IStream *Stream;
  DWORD GrfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
  if(FAILED(LOG_FUNCTION_CALL(pStg->OpenStream(L"Graph", 0, GrfMode, 0, &Stream))))
    return LOG_RESULT(E_FAIL);

  std::vector<char> Buffer(10000);
  RawByteString Str;
  ULONG BytesRead;
  do
  {
    LOG_FUNCTION_CALL(Stream->Read(&Buffer[0], Buffer.size(), &BytesRead));
    Str += RawByteString(&Buffer[0], BytesRead);
  }
  while(BytesRead == Buffer.size());

  TConfigFile ConfigFile;
  ConfigFile.LoadFromString(ToWString(UTF8ToString(Str)));
  std::wstring SavedVersion = ConfigFile.Section(L"Graph").Read(L"Version", L"NA");
  if(SavedVersion < TVersion(L"4.4.0.414"))
    ConfigFile.LoadFromString(String(Str.c_str()).c_str());

  HRESULT Result = S_OK;
  try
  {
    Form1->Data.Load(ConfigFile);
  }
  catch(...)
  {
    Result = E_FAIL;
  }

  const TConfigFileSection &Section = ConfigFile.Section(L"Image");
  if(Section.KeyExists(L"Width"))
  {
    SetSize(Section.Read(L"Width", 500), Section.Read(L"Height", 500));
    LOG_DATA("Width=" + GetWidth());
    LOG_DATA("Height=" + GetHeight());
  }

  UndoList.Clear();
  Form1->UpdateTreeView();
  Form1->UpdateMenu();
  Form1->Redraw();

  Stream->Release();
  return LOG_RESULT(Result);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Save(
  /* [unique][in] */ IStorage *pStgSave,
  /* [in] */ BOOL fSameAsLoad)
{
  DEBUG_CALL();
  LOG_ARG("fSameAsLoad=" + fSameAsLoad);
  IStream *Stream = NULL;
  DWORD GrfMode = STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE;
  if(FAILED(LOG_FUNCTION_CALL(pStgSave->CreateStream(L"Graph", GrfMode, 0, 0, &Stream))))
    return LOG_RESULT(E_FAIL);

  std::wstring Str = Form1->Data.SaveToString(fSameAsLoad);
  LOG_DATA("Width=" + GetWidth());
  LOG_DATA("Height=" + GetHeight());
  TConfigFile ConfigFile;
  ConfigFile.LoadFromString(Str);
  ConfigFile.Section(L"Image").Write(L"Width", GetWidth());
  ConfigFile.Section(L"Image").Write(L"Height", GetHeight());
  Str = ConfigFile.GetAsString();
  RawByteString Utf8Str = UTF8Encode(ToUString(Str));

  HRESULT Result = S_OK;
  if(FAILED(LOG_FUNCTION_CALL(Stream->Write(Utf8Str.c_str(), Utf8Str.Length(), NULL))))
    Result = E_FAIL;

  Stream->Release();
  return LOG_RESULT(Result);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SaveCompleted(
  /* [unique][in] */ IStorage *pStgNew)
{
  DEBUG_CALL();
  SendAdvise(acDataSaved);
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::HandsOffStorage(void)
{
  DEBUG_CALL();
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetClassID(
  /* [out] */ CLSID *pClassID)
{
  DEBUG_CALL();
  *pClassID = CLSID_OleServer;
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// IPersistFile
HRESULT STDMETHODCALLTYPE TOleServerImpl::Load(
  /* [in] */ LPCOLESTR pszFileName,
  /* [in] */ DWORD dwMode)
{
  DEBUG_CALL();
  LOG_ARG("FileName=" + String(pszFileName));
  HRESULT Result = Form1->LoadFromFile(pszFileName, false, false) ? S_OK : E_FAIL;
  Form1->Redraw();
  return LOG_RESULT(Result);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::Save(
  /* [unique][in] */ LPCOLESTR pszFileName,
  /* [in] */ BOOL fRemember)
{
  DEBUG_CALL();
  LOG_ARG("FileName=" + pszFileName + ", fRemember=" + fRemember);
  return LOG_RESULT(Form1->Data.Save(pszFileName ? std::wstring(pszFileName) : Form1->Data.GetFileName(), fRemember));
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::SaveCompleted(
  /* [unique][in] */ LPCOLESTR pszFileName)
{
  DEBUG_CALL();
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TOleServerImpl::GetCurFile(
  /* [out] */ LPOLESTR *ppszFileName)
{
  DEBUG_CALL();

  if(Form1->Data.GetFileName().empty())
  {
    const wchar_t DefaultName[] = L"*.grf";
    if((*ppszFileName = static_cast<wchar_t*>(CoTaskMemAlloc(sizeof(DefaultName)))) == NULL)
      return LOG_RESULT(E_OUTOFMEMORY);
    wcscpy(*ppszFileName, DefaultName);
    return LOG_RESULT(S_FALSE);
  }

  String FileName = Form1->Data.GetFileName().c_str();
  if((*ppszFileName = static_cast<wchar_t*>(CoTaskMemAlloc(FileName.Length() * 2 + 2))) == NULL)
    return LOG_RESULT(E_OUTOFMEMORY);
  wcscpy(*ppszFileName, FileName.c_str());
  return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------










