//---------------------------------------------------------------------------
// class TRichEditOle - adds basic OLE functionality to TCustomRichEdit.
// Based on code found at borland.public.cppbuilder.* posted by Robert Dunn
// A lot of information about RichEdit can be found at his web site:
// http://home.att.net/~robertdunn/Yacs.html
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RichEditOleCallback.h"
#include "Debug.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
TRichEditOleCallback::TRichEditOleCallback(TCustomRichEdit* ARichEdit)
  : FRefCnt(0), RichEdit(ARichEdit), FAcceptDrop(false)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::QueryInterface(
    /* [in] */ REFIID riid,
    /* [out] */ void** lplpObj)
{
  DEBUG_CALL();
	// return S_OK if the interface is supported, E_NOINTERFACE if not.
  if(lplpObj == NULL)
    return LOG_RESULT(E_POINTER);
	if (riid == IID_IUnknown)
		*lplpObj = static_cast<IUnknown*>(this);
	else if (riid == IID_IRichEditOleCallback)
		*lplpObj = static_cast<IRichEditOleCallback*>(this);
	else
  {
		*lplpObj = NULL;
		return LOG_RESULT(E_NOINTERFACE);
	}

	AddRef();
	return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE TRichEditOleCallback::AddRef()
{
	// need to add real reference counting if any caller really holds
	// on to interface pointers
	return ++FRefCnt;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE TRichEditOleCallback::Release()
{
	// need to add real reference counting if any caller really holds
	// on to interface pointers
	--FRefCnt;
	return FRefCnt;
}
//---------------------------------------------------------------------------
// *** IRichEditOleCallback methods ***
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetNewStorage(
    /* [out] */ IStorage** lplpstg)
{
  DEBUG_CALL();
	if(lplpstg == NULL)
    return LOG_RESULT(E_INVALIDARG);
	*lplpstg = NULL;

	//
	// We need to create a new storage for an object to occupy.  We're going
	// to do this the easy way and just create a storage on an HGLOBAL and let
	// OLE do the management.  When it comes to saving things we'll just let
	// the RichEdit control do the work.  Keep in mind this is not efficient.
	//
	LPLOCKBYTES pLockBytes;
	HRESULT hr = LOG_FUNCTION_CALL(::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes));
	if(FAILED(hr))
    return LOG_RESULT(hr);

	hr = LOG_FUNCTION_CALL(::StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, lplpstg));
	pLockBytes->Release();
	return LOG_RESULT(hr);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetInPlaceContext(
    /* [out] */ IOleInPlaceFrame **lplpFrame,
	  /* [out] */ IOleInPlaceUIWindow** lplpDoc,
    /* [out] */ OLEINPLACEFRAMEINFO* lpFrameInfo)
{
  DEBUG_CALL();
	return LOG_RESULT(E_NOTIMPL);	// what to return???
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::ShowContainerUI(
    /* [in] */ BOOL fShow)
{
  DEBUG_CALL();
  LOG_ARG("fShow=" + fShow);
	return LOG_RESULT(E_NOTIMPL);	// what to return???
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::QueryInsertObject(
    /* [in] */ CLSID *lpclsid,
	  /* [in] */ IStorage *lpstg,
    /* [in] */ LONG cp)
{
  DEBUG_CALL();
  LOG_ARG("cp=" + cp);
	// let richedit insert any and all objects
	return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::DeleteObject(
    /* [in] */ IOleObject *lpoleobj)
{
  DEBUG_CALL();
	// per doc, no return value, i.e., this is simply a notification
	return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::QueryAcceptData(
    /* [in] */ IDataObject *lpdataobj,
	  /* [in/out] */ CLIPFORMAT *lpcfFormat,
    /* [in] */ DWORD reco,
    /* [in] */ BOOL fReally,
    /* [in] */ HGLOBAL hMetaPict)
{
  DEBUG_CALL();
  LOG_ARG("*lpcfFormat=" + *lpcfFormat + ", reco=" + reco + ",fReally=" + fReally);
	// allow insertion on dropped file?
	if (reco == RECO_DROP && !FAcceptDrop)
    return LOG_RESULT(E_NOTIMPL);
	return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::ContextSensitiveHelp(
    /* [in] */ BOOL fEnterMode)
{
  DEBUG_CALL();
  LOG_ARG("fEnterMode=" + fEnterMode);
	return LOG_RESULT(E_NOTIMPL);	// what to return???
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetClipboardData(
    /* [in] */ CHARRANGE* lpchrg,
	  /* [in] */ DWORD reco,
    /* [out] */ IDataObject **lplpdataobj)
{
  DEBUG_CALL();
  LOG_ARG("lpchrg->cpMin=" + lpchrg->cpMin + ", lpchrg->cpMax=" + lpchrg->cpMax + ", reco=" + reco);
	return LOG_RESULT(E_NOTIMPL);	// what to return???
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetDragDropEffect(
    /* [in] */ BOOL fDrag,
  	/* [in] */ DWORD grfKeyState,
    /* [out] */ DWORD *pdwEffect)
{
  DEBUG_CALL();
  LOG_ARG("fDrag=" + fDrag + ", grfKeyState=" + grfKeyState);
	// per doc, no return value, i.e., for notification only
	*pdwEffect = DROPEFFECT_NONE;
	return LOG_RESULT(S_OK);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetContextMenu(
    /* [in] */ WORD seltype,
	  /* [in] */ IOleObject *lpoleobj,
    /* [in] */ CHARRANGE *lpchrg,
    /* [out] */ HMENU *lphmenu)
{
  DEBUG_CALL();
  LOG_ARG("seltype=" + seltype + "lpchrg->cpMin=" + lpchrg->cpMin + ", lpchrg->cpMax=" + lpchrg->cpMax);
	return LOG_RESULT(E_NOTIMPL);	// what to return???
}
//---------------------------------------------------------------------------

