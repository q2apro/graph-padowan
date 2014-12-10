/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
// OLESERVERIMPL.H : Declaration of the TOleServerImpl

#ifndef OleServerImplH
#define OleServerImplH

#include "OleServer.h"
#include <atl/atlmod.h>

#define ATL_APARTMENT_THREADED

extern class TOleServerImpl *OleServerImpl;
extern "C" const __declspec(selectany) GUID CLSID_OleServer = {0xFD536B77, 0x5DF5, 0x448C,{ 0x90, 0xD1, 0x2C,0x04, 0xB3, 0xC1,0xD1, 0xBD} };

class __declspec(uuid("FD536B77-5DF5-448C-90D1-2C04B3C1D1BD")) ATL_NO_VTABLE TOleServerImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TOleServerImpl, &CLSID_OleServer>,
  public IOleObject,
  public IDataObject,
  public IPersistStorage,
  public IPersistFile
{
  static const CLIPFORMAT cfObjectDescriptor;
  static const CLIPFORMAT cfEmbedSource;
  static const CLIPFORMAT cfEmbeddedObject;
  static const CLIPFORMAT cfPng;

  IOleClientSite *OleClientSite;
  IOleAdviseHolder *OleAdviseHolder;
  IDataAdviseHolder *DataAdviseHolder;
  bool DataIsDirty;
  int Width;
  int Height;

  static String ClipboardFormatToStr(CLIPFORMAT Format);
  static String DeviceToStr(const DVTARGETDEVICE *Device);
  template<typename T> static void ReleaseCom(T *&Unknown);
  static bool CheckRegistration();
  static bool Register(bool AllUsers);
  static void LogException();

  void DrawMetafile(TMetafile *Metafile, unsigned ImageWidth, unsigned ImageHeight);
  HMETAFILE ConvertEnhMetaToMeta(HENHMETAFILE hemf);

public:
  TOleServerImpl();
  ~TOleServerImpl();
  void SetSize(int AWidth, int AHeight) {Width = AWidth; Height = AHeight;}
  int GetWidth()  {return Width;}
  int GetHeight() {return Height;}
  void SendAdvise(TAdviseCode AdviseCode);

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID(L"GraphFile");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister);

BEGIN_COM_MAP(TOleServerImpl)
  COM_INTERFACE_ENTRY(IOleObject)
  COM_INTERFACE_ENTRY(IPersistStorage)
  COM_INTERFACE_ENTRY(IDataObject)
  COM_INTERFACE_ENTRY(IPersistFile)
END_COM_MAP()

public:

//IOleObject
  HRESULT STDMETHODCALLTYPE SetClientSite(
      /* [unique][in] */ IOleClientSite *pClientSite);

  HRESULT STDMETHODCALLTYPE GetClientSite(
      /* [out] */ IOleClientSite **ppClientSite);

  HRESULT STDMETHODCALLTYPE SetHostNames(
      /* [in] */ LPCOLESTR szContainerApp,
      /* [unique][in] */ LPCOLESTR szContainerObj);

  HRESULT STDMETHODCALLTYPE Close(
      /* [in] */ DWORD dwSaveOption);

  HRESULT STDMETHODCALLTYPE SetMoniker(
      /* [in] */ DWORD dwWhichMoniker,
      /* [unique][in] */ IMoniker *pmk);

  HRESULT STDMETHODCALLTYPE GetMoniker(
      /* [in] */ DWORD dwAssign,
      /* [in] */ DWORD dwWhichMoniker,
      /* [out] */ IMoniker **ppmk);

  HRESULT STDMETHODCALLTYPE InitFromData(
      /* [unique][in] */ IDataObject *pDataObject,
      /* [in] */ BOOL fCreation,
      /* [in] */ DWORD dwReserved);

  HRESULT STDMETHODCALLTYPE GetClipboardData(
      /* [in] */ DWORD dwReserved,
      /* [out] */ IDataObject **ppDataObject);

  HRESULT STDMETHODCALLTYPE DoVerb(
      /* [in] */ LONG iVerb,
      /* [unique][in] */ LPMSG lpmsg,
      /* [unique][in] */ IOleClientSite *pActiveSite,
      /* [in] */ LONG lindex,
      /* [in] */ HWND hwndParent,
      /* [unique][in] */ LPCRECT lprcPosRect);

  HRESULT STDMETHODCALLTYPE EnumVerbs(
      /* [out] */ IEnumOLEVERB **ppEnumOleVerb);

  HRESULT STDMETHODCALLTYPE Update(void);

  HRESULT STDMETHODCALLTYPE IsUpToDate(void);

  HRESULT STDMETHODCALLTYPE GetUserClassID(
    /* [out] */ CLSID *pClsid);

  HRESULT STDMETHODCALLTYPE GetUserType(
    /* [in] */ DWORD dwFormOfType,
    /* [out] */ LPOLESTR *pszUserType);

  HRESULT STDMETHODCALLTYPE SetExtent(
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ SIZEL *psizel);

  HRESULT STDMETHODCALLTYPE GetExtent(
    /* [in] */ DWORD dwDrawAspect,
    /* [out] */ SIZEL *psizel);

  HRESULT STDMETHODCALLTYPE Advise(
    /* [unique][in] */ IAdviseSink *pAdvSink,
    /* [out] */ DWORD *pdwConnection);

  HRESULT STDMETHODCALLTYPE Unadvise(
    /* [in] */ DWORD dwConnection);

  HRESULT STDMETHODCALLTYPE EnumAdvise(
    /* [out] */ IEnumSTATDATA **ppenumAdvise);

  HRESULT STDMETHODCALLTYPE GetMiscStatus(
    /* [in] */ DWORD dwAspect,
    /* [out] */ DWORD *pdwStatus);

  HRESULT STDMETHODCALLTYPE SetColorScheme(
    /* [in] */ LOGPALETTE *pLogpal);


  //IDataObject
  /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
    /* [unique][in] */ FORMATETC *pformatetcIn,
    /* [out] */ STGMEDIUM *pmedium);

  /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [out][in] */ STGMEDIUM *pmedium);

  HRESULT STDMETHODCALLTYPE QueryGetData(
    /* [unique][in] */ FORMATETC *pformatetc);

  HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
    /* [unique][in] */ FORMATETC *pformatectIn,
    /* [out] */ FORMATETC *pformatetcOut);

  /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [unique][in] */ STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease);

  HRESULT STDMETHODCALLTYPE EnumFormatEtc(
    /* [in] */ DWORD dwDirection,
    /* [out] */ IEnumFORMATETC **ppenumFormatEtc);

  HRESULT STDMETHODCALLTYPE DAdvise(
    /* [in] */ FORMATETC *pformatetc,
    /* [in] */ DWORD advf,
    /* [unique][in] */ IAdviseSink *pAdvSink,
    /* [out] */ DWORD *pdwConnection);

  HRESULT STDMETHODCALLTYPE DUnadvise(
    /* [in] */ DWORD dwConnection);

  HRESULT STDMETHODCALLTYPE EnumDAdvise(
    /* [out] */ IEnumSTATDATA **ppenumAdvise);


// IPersistStorage
  HRESULT STDMETHODCALLTYPE IsDirty(void);

  HRESULT STDMETHODCALLTYPE InitNew(
    /* [unique][in] */ IStorage *pStg);

  HRESULT STDMETHODCALLTYPE Load(
    /* [unique][in] */ IStorage *pStg);

  HRESULT STDMETHODCALLTYPE Save(
    /* [unique][in] */ IStorage *pStgSave,
    /* [in] */ BOOL fSameAsLoad);

  HRESULT STDMETHODCALLTYPE SaveCompleted(
    /* [unique][in] */ IStorage *pStgNew);

  HRESULT STDMETHODCALLTYPE HandsOffStorage(void);

  HRESULT STDMETHODCALLTYPE GetClassID(
    /* [out] */ CLSID *pClassID);

// IPersistFile
//  STDMETHODCALLTYPE IsDirty(void);

  HRESULT STDMETHODCALLTYPE Load(
    /* [in] */ LPCOLESTR pszFileName,
    /* [in] */ DWORD dwMode);

  HRESULT STDMETHODCALLTYPE Save(
    /* [unique][in] */ LPCOLESTR pszFileName,
    /* [in] */ BOOL fRemember);

  HRESULT STDMETHODCALLTYPE SaveCompleted(
    /* [unique][in] */ LPCOLESTR pszFileName);

  HRESULT STDMETHODCALLTYPE GetCurFile(
    /* [out] */ LPOLESTR *ppszFileName);
};

#endif //OleServerImplH
