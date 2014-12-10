//---------------------------------------------------------------------------
// class TRichEditOle - adds basic OLE functionality to TCustomRichEdit.
// Based on code found at borland.public.cppbuilder.* posted by Robert Dunn
// A lot of information about RichEdit can be found at his web site:
// http://home.att.net/~robertdunn/Yacs.html
//---------------------------------------------------------------------------
#ifndef RichEditOleCallbackH
#define RichEditOleCallbackH

#include <comctrls.hpp>
#include <comobj.hpp>
#include <\ole2.hpp>
#include <oledlg.hpp>
#include <richole.h>
//---------------------------------------------------------------------------
class TRichEditOleCallback : public IRichEditOleCallback
{
private:
	TCustomRichEdit* RichEdit;
	ULONG FRefCnt;
	bool FAcceptDrop;

public:
	// *** IUnknown methods ***
  HRESULT STDMETHODCALLTYPE QueryInterface(
      /* [in] */ REFIID riid,
      /* [out] */ void** lplpObj);

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	// *** IRichEditOleCallback methods ***
  HRESULT STDMETHODCALLTYPE GetNewStorage(
      /* [out] */ IStorage** lplpstg);

  HRESULT STDMETHODCALLTYPE GetInPlaceContext(
      /* [out] */ IOleInPlaceFrame **lplpFrame,
      /* [out] */ IOleInPlaceUIWindow** lplpDoc,
      /* [out] */ OLEINPLACEFRAMEINFO* lpFrameInfo);

  HRESULT STDMETHODCALLTYPE ShowContainerUI(
      /* [in] */ BOOL fShow);

  HRESULT STDMETHODCALLTYPE QueryInsertObject(
      /* [in] */ CLSID *lpclsid,
      /* [in] */ IStorage *lpstg,
      /* [in] */ LONG cp);

  HRESULT STDMETHODCALLTYPE DeleteObject(
      /* [in] */ IOleObject *lpoleobj);

  HRESULT STDMETHODCALLTYPE QueryAcceptData(
      /* [in] */ IDataObject *lpdataobj,
      /* [in] */ CLIPFORMAT *lpcfFormat,
      /* [in] */ DWORD reco,
      /* [in] */ BOOL fReally,
      /* [in] */ HGLOBAL hMetaPict);

  HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
      /* [in] */ BOOL fEnterMode);

  HRESULT STDMETHODCALLTYPE GetClipboardData(
      /* [in] */ CHARRANGE* lpchrg,
      /* [in] */ DWORD reco,
      /* [out] */ IDataObject **lplpdataobj);

  HRESULT STDMETHODCALLTYPE TRichEditOleCallback::GetDragDropEffect(
      /* [in] */ BOOL fDrag,
      /* [in] */ DWORD grfKeyState,
      /* [out] */ DWORD *pdwEffect);

  HRESULT STDMETHODCALLTYPE GetContextMenu(
      /* [in] */ WORD seltype,
      /* [in] */ IOleObject *lpoleobj,
      /* [in] */ CHARRANGE *lpchrg,
      /* [out] */ HMENU *lphmenu);

public:
	TRichEditOleCallback(TCustomRichEdit* ARichEdit);
	__property bool AcceptDrop = {read = FAcceptDrop, write = FAcceptDrop};
};
//---------------------------------------------------------------------------
#endif

