//---------------------------------------------------------------------------
// class TRichEditOle - adds basic OLE functionality to TCustomRichEdit.
// Based on code found at borland.public.cppbuilder.* posted by Robert Dunn
// A lot of information about RichEdit can be found at his web site:
// http://home.att.net/~robertdunn/Yacs.html
//---------------------------------------------------------------------------
#include "Config.h"
#pragma hdrstop
#include "RichEditOle.h"
#include "Debug.h"
#ifdef _DEBUG
#undef _ASSERTE
#endif
#include <Atl/atlbase.h>
#pragma package(smart_init)
#pragma link "Debug"
//---------------------------------------------------------------------------
TRichEditOle::TRichEditOle(TCustomRichEdit* ARichEdit)
  : RichEditOle(NULL), RichEdit(ARichEdit), RichEditCallback(ARichEdit)
{
	//Do we need to call OleInitialize(NULL)? seems to work without it...
	OleInitialize(NULL);

	SendMessage(RichEdit->Handle, EM_SETOLECALLBACK, 0, reinterpret_cast<LPARAM>(&RichEditCallback));
	if(!SendMessage(RichEdit->Handle, EM_GETOLEINTERFACE, 0, reinterpret_cast<LPARAM>(&RichEditOle)))
		RichEditOle = NULL;

	//Register clipboard formats used in ole (so we'll have the clipboard
	//format IDs for ole clipboard operations (like OleUIPasteSpecial)
	CFObjectDescriptor = RegisterClipboardFormat(L"Object Descriptor");
	CFEmbeddedObject = RegisterClipboardFormat(L"Embedded Object");
	CFEmbedSource = RegisterClipboardFormat(L"Embed Source");
	CFLinkSource = RegisterClipboardFormat(L"Link Source");
	CFRtf = RegisterClipboardFormat(CF_RTF);
	CFRtfNoObjs = RegisterClipboardFormat(CF_RTFNOOBJS);
	CFReTextObjs = RegisterClipboardFormat(CF_RETEXTOBJ);
}
//---------------------------------------------------------------------------
TRichEditOle::~TRichEditOle()
{
	//Close any active objects unconditionally
	if(RichEditOle)
    CloseActiveObjects(false);

	//Set the callback to null
	SendMessage(RichEdit->Handle, EM_SETOLECALLBACK, 0, 0);

	// release the interface
	if(RichEditOle)
		RichEditOle = NULL;

	// balance calls to OleInitialize()
	OleUninitialize();
}
//---------------------------------------------------------------------------
void TRichEditOle::OleUIMetafilePictIconFree(HGLOBAL MetaPict)
{
	if(!MetaPict)
    return;

	LPMETAFILEPICT MF = static_cast<LPMETAFILEPICT>(GlobalLock(MetaPict));

	if(MF && MF->hMF)
    DeleteMetaFile(MF->hMF);
	GlobalUnlock(MetaPict);
	GlobalFree(MetaPict);
}
//---------------------------------------------------------------------------
bool TRichEditOle::OleStdSwitchDisplayAspect(LPOLEOBJECT OleObj,
  LPDWORD CurAspect, DWORD NewAspect, HGLOBAL MetaPict, BOOL DeleteOldAspect,
  BOOL SetupViewAdvise, LPADVISESINK AdviseSink, LPBOOL MustUpdate)
{
	LPOLECACHE      OleCache = NULL;
	LPVIEWOBJECT    ViewObj = NULL;
	LPENUMSTATDATA  EnumStatData = NULL;
	STATDATA        StatData;
	FORMATETC       FmtEtc;
	STGMEDIUM       Medium;
	DWORD           Advf;
	DWORD           NewConnection;
	DWORD           OldAspect = *CurAspect;
	HRESULT         Err;

	if(MustUpdate)
    *MustUpdate = FALSE;

	LOG_FUNCTION_CALL(OleObj->QueryInterface(IID_IOleCache, reinterpret_cast<LPVOID*>(&OleCache)));

	//If IOleCache* is NOT available, do nothing
	if(!OleCache)
    return false;

	//Setup new cache with the new aspect
	FmtEtc.cfFormat = 0;     //Whatever is needed to draw
	FmtEtc.ptd      = NULL;
	FmtEtc.dwAspect = NewAspect;
	FmtEtc.lindex   = -1;
	FmtEtc.tymed    = TYMED_NULL;

	/* OLE2NOTE: if we are setting up Icon aspect with a custom icon
	**    then we do not want DataAdvise notifications to ever change
	**    the contents of the data cache. thus we set up a NODATA
	**    advise connection. otherwise we set up a standard DataAdvise
	**    connection.
	*/
	if(NewAspect == DVASPECT_ICON && MetaPict)
    Advf = ADVF_NODATA;
	else
    Advf = ADVF_PRIMEFIRST;

	Err = LOG_FUNCTION_CALL(OleCache->Cache(&FmtEtc, Advf, &NewConnection));
	if(!SUCCEEDED(Err))
  {
		OleCache->Release();
		return false;
	}

	*CurAspect = NewAspect;

	/* OLE2NOTE: if we are setting up Icon aspect with a custom icon,
	**    then stuff the icon into the cache. otherwise the cache must
	**    be forced to be updated. set the *lpfMustUpdate flag to tell
	**    caller to force the object to Run so that the cache will be
	**    updated.
	*/
	if(NewAspect == DVASPECT_ICON && MetaPict)
  {
		FmtEtc.cfFormat = CF_METAFILEPICT;
		FmtEtc.ptd      = NULL;
		FmtEtc.dwAspect = DVASPECT_ICON;
		FmtEtc.lindex   = -1;
		FmtEtc.tymed    = TYMED_MFPICT;

		Medium.tymed          = TYMED_MFPICT;
		Medium.hGlobal        = MetaPict;
		Medium.pUnkForRelease = NULL;

		Err = LOG_FUNCTION_CALL(OleCache->SetData(&FmtEtc, &Medium, FALSE /* fRelease */));
    if(!SUCCEEDED(Err))
    {
      OleCache->Release();
      return false;
    }
	}
	else
    if(MustUpdate)
      *MustUpdate = TRUE;

	if(SetupViewAdvise && AdviseSink)
  {
		/* OLE2NOTE: re-establish the ViewAdvise connection */
		LOG_FUNCTION_CALL(OleObj->QueryInterface(IID_IViewObject, reinterpret_cast<LPVOID*>(&ViewObj)));

		if(ViewObj)
    {
			LOG_FUNCTION_CALL(ViewObj->SetAdvise(NewAspect, 0, AdviseSink));
      ViewObj->Release();
    }
  }

	/* OLE2NOTE: remove any existing caches that are set up for the old
	**    display aspect. It WOULD be possible to retain the caches set
	**    up for the old aspect, but this would increase the storage
	**    space required for the object and possibly require additional
	**    overhead to maintain the unused cachaes. For these reasons the
	**    strategy to delete the previous caches is prefered. If it is a
	**    requirement to quickly switch between Icon and Content
	**    display, then it would be better to keep both aspect caches.
	*/

	if(DeleteOldAspect)
  {
		Err = OleCache->EnumCache(&EnumStatData);

		while(Err == NOERROR)
    {
			Err = LOG_FUNCTION_CALL(EnumStatData->Next(1, &StatData, NULL));
			if(Err != NOERROR)
        break;		// DONE! no more caches.

			if(StatData.formatetc.dwAspect == OldAspect)
				// Remove previous cache with old aspect
				LOG_FUNCTION_CALL(OleCache->Uncache(StatData.dwConnection));
		}

		if(EnumStatData)
			if(EnumStatData->Release())
      {
				throw EOleError("OleStdSwitchDisplayAspect: Cache enumerator NOT released");
      }
	}

	if(OleCache)
    OleCache->Release();

	return true;
}
//---------------------------------------------------------------------------
void TRichEditOle::SetHostNames(const AnsiString &HostApp, const AnsiString &HostDoc)
{
	if(RichEditOle)
  	LOG_FUNCTION_CALL(RichEditOle->SetHostNames(HostApp.c_str(), HostDoc.c_str()));
}
//---------------------------------------------------------------------------
IOleClientSite* TRichEditOle::GetClientSite()
{
	IOleClientSite *ClientSite;
	if(LOG_FUNCTION_CALL(RichEditOle->GetClientSite(&ClientSite)) != S_OK)
    ClientSite = NULL;
	return ClientSite;
}
//---------------------------------------------------------------------------
LONG TRichEditOle::GetObjectCount()
{
	if(!RichEditOle)
    throw EOleError("IRichEditOle interface is not valid.");
	return RichEditOle->GetObjectCount();
}
//---------------------------------------------------------------------------
LONG TRichEditOle::GetLinkCount()
{
	if(!RichEditOle)
    throw EOleError("IRichEditOle interface is not valid.");
	return RichEditOle->GetLinkCount();
}
//---------------------------------------------------------------------------
bool TRichEditOle::InsertObject()
{
	LPOLEOBJECT OleObject = 0;

	//Better be a valid richedit
	if(!RichEdit)
    return false;

	//Make sure client site is valid
	CComPtr<IOleClientSite>  ClientSite;
  ClientSite.Attach(GetClientSite());
	if(!ClientSite)
    throw EOleError("IOleClientSite interface is not valid.");

	//Get substorage
	IStorage *Stg;
	OleCheck(LOG_FUNCTION_CALL(RichEditCallback.GetNewStorage(&Stg)));

	//Display the InsertObject dialog
	TCHAR Buf[MAX_PATH] = {0};
	OLEUIINSERTOBJECT io;

	memset(&io, 0, sizeof(io));
	io.cbStruct = sizeof(io);
	io.dwFlags = IOF_CREATENEWOBJECT | IOF_CREATEFILEOBJECT |
		IOF_SELECTCREATENEW | IOF_CREATELINKOBJECT;
	io.hWndOwner = RichEdit->Handle;
	io.lpszFile = Buf;
	io.cchFile = MAX_PATH;
	io.iid = IID_IOleObject;
	io.oleRender = OLERENDER_DRAW;
	io.lpIOleClientSite = ClientSite;
	io.lpIStorage = Stg;
	io.ppvObj = reinterpret_cast<void**>(&OleObject);
	io.clsid = CLSID_NULL;

	DWORD RetVal = LOG_OLEUI_CALL(OleUIInsertObject(&io));
  LOG_DATA("io.sc=" + ValueToStr(HResultList, io.sc));
	if(RetVal != OLEUI_SUCCESS)
  {
		Stg->Release();
		if(io.hMetaPict)
      OleUIMetafilePictIconFree(io.hMetaPict);
		if(OleObject)
      OleObject->Release();
		if(RetVal == OLEUI_CANCEL)
      return false;
		OleError(io.sc);
	}

	//Got the object
	OleObject = reinterpret_cast<LPOLEOBJECT>(*io.ppvObj);

	REOBJECT Obj;
	memset(&Obj, 0, sizeof(Obj));
	Obj.cbStruct = sizeof(Obj);
	Obj.cp = REO_CP_SELECTION;
	Obj.clsid = io.clsid;
	Obj.poleobj = OleObject;
	Obj.pstg = Stg;
	Obj.polesite = ClientSite;
	Obj.dvaspect = DVASPECT_CONTENT;
	Obj.dwFlags = REO_RESIZABLE;

	if(io.dwFlags & IOF_SELECTCREATENEW)
    Obj.dwFlags |= REO_BLANK;

	//Try to get a valid clsid
	if(IsEqualCLSID(Obj.clsid, CLSID_NULL))
  {
#ifdef UNICODE
		GetClassFile(Buf, &Obj.clsid);
#else
		WCHAR BufFile[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, Buf, -1, BufFile, sizeof(BufFile));
		GetClassFile(BufFile, &Obj.clsid);
#endif
	}

	//Display as icon?
	if(io.dwFlags & IOF_CHECKDISPLAYASICON)
  {
		int Update;
		if(!OleStdSwitchDisplayAspect(OleObject, &Obj.dvaspect, DVASPECT_ICON, io.hMetaPict, true, false, 0, &Update))
			Application->MessageBox(L"Cannot display object as icon.", L"Insert Object", MB_OK | MB_ICONWARNING);
	}

	//Insert the object into the richedit
	if(LOG_FUNCTION_CALL(RichEditOle->InsertObject(&Obj)) != S_OK)
  {
		Stg->Release();
		if(io.hMetaPict)
      OleUIMetafilePictIconFree(io.hMetaPict);
		if(OleObject)
      OleObject->Release();
		throw EOleError("RichEdit refused to insert object.");
	}

	//If new object, do the show verb
	if(io.dwFlags & IOF_SELECTCREATENEW)
  {
		//Get object position
		POINT Pos;
		//Hey! where is REO_IOB_SELECTION documented!
		SendMessage(RichEdit->Handle, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&Pos), REO_IOB_SELECTION);
		RECT Rect = { 0, 0, 100, 100 };
		OffsetRect(&Rect, Pos.x, Pos.y);
    //OLEIVERB_SHOW should be used in DoVerb() here, but that doesn't work with Open Office objects
		LOG_FUNCTION_CALL(OleObject->DoVerb(OLEIVERB_OPEN, NULL, ClientSite, 0, RichEdit->Handle, &Rect));
	}

	Stg->Release();
	if(io.hMetaPict)
    OleUIMetafilePictIconFree(io.hMetaPict);
	if(OleObject)
    OleObject->Release();

	return true;
}
//---------------------------------------------------------------------------
bool TRichEditOle::PasteSpecial()
{
  OLEUIPASTESPECIAL Data;
	::OLEUIPASTEENTRY Formats[8];

	memset(&Data, 0, sizeof(Data));
	memset(&Formats, 0, sizeof(Formats));

	Data.cbStruct = sizeof(Data);
	Data.hWndOwner = RichEdit->Handle;
	Data.arrPasteEntries = Formats;
	Data.cPasteEntries = sizeof(Formats) / sizeof(Formats[0]);
	Data.arrLinkTypes = &CFLinkSource;
	Data.cLinkTypes = 1;

	//The following entries were devined from MS MFC code and appear to
	//be fairly standard for rich edit controls; this is basically a static
	//table and could be moved, but the overhead here is small...
	Formats[0].fmtetc.cfFormat = CFEmbedSource;
	Formats[0].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[0].fmtetc.lindex = -1;
	Formats[0].fmtetc.tymed = TYMED_ISTREAM;//TYMED_ISTORAGE;
	Formats[0].lpstrFormatName = L"%s";
	Formats[0].lpstrResultText = L"%s";
	Formats[0].dwFlags = OLEUIPASTE_PASTE | OLEUIPASTE_ENABLEICON;

	Formats[1].fmtetc.cfFormat = CFLinkSource;
	Formats[1].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[1].fmtetc.lindex = -1;
	Formats[1].fmtetc.tymed = TYMED_ISTREAM;
	Formats[1].lpstrFormatName = L"%s";
	Formats[1].lpstrResultText = L"%s";
	Formats[1].dwFlags = OLEUIPASTE_LINKTYPE1 | OLEUIPASTE_ENABLEICON;

	Formats[2].fmtetc.cfFormat = CF_TEXT;
	Formats[2].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[2].fmtetc.lindex = -1;
	Formats[2].fmtetc.tymed = TYMED_HGLOBAL;
	Formats[2].lpstrFormatName = L"Unformatted Text";
	Formats[2].lpstrResultText = L"text without any formatting";
	Formats[2].dwFlags = OLEUIPASTE_PASTEONLY;

	Formats[3].fmtetc.cfFormat = CFRtf;
	Formats[3].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[3].fmtetc.lindex = -1;
	Formats[3].fmtetc.tymed = TYMED_HGLOBAL;
	Formats[3].lpstrFormatName = L"Formatted Text (RTF)";
	Formats[3].lpstrResultText = L"text with font and paragraph formatting";
	Formats[3].dwFlags = OLEUIPASTE_PASTEONLY;

	Formats[4].fmtetc.cfFormat = CF_ENHMETAFILE;
	Formats[4].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[4].fmtetc.lindex = -1;
	Formats[4].fmtetc.tymed = TYMED_ENHMF;
	Formats[4].lpstrFormatName = L"Picture (Enhanced Metafile)";
	Formats[4].lpstrResultText = L"a picture";
	Formats[4].dwFlags = OLEUIPASTE_PASTEONLY;

	Formats[5].fmtetc.cfFormat = CF_METAFILEPICT;
	Formats[5].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[5].fmtetc.lindex = -1;
	Formats[5].fmtetc.tymed = TYMED_MFPICT;
	Formats[5].lpstrFormatName = L"Picture (Metafile)";
	Formats[5].lpstrResultText = L"a picture";
	Formats[5].dwFlags = OLEUIPASTE_PASTEONLY;

	Formats[6].fmtetc.cfFormat = CF_DIB;
	Formats[6].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[6].fmtetc.lindex = -1;
	Formats[6].fmtetc.tymed = TYMED_MFPICT;
	Formats[6].lpstrFormatName = L"Device Independent Bitmap";
	Formats[6].lpstrResultText = L"a device independent bitmap";
	Formats[6].dwFlags = OLEUIPASTE_PASTEONLY;

	Formats[7].fmtetc.cfFormat = CF_BITMAP;
	Formats[7].fmtetc.dwAspect = DVASPECT_CONTENT;
	Formats[7].fmtetc.lindex = -1;
	Formats[7].fmtetc.tymed = TYMED_GDI;
	Formats[7].lpstrFormatName = L"Bitmap";
	Formats[7].lpstrResultText = L"a bitmap";
	Formats[7].dwFlags = OLEUIPASTE_PASTEONLY;

	DWORD RetVal = LOG_OLEUI_CALL(OleUIPasteSpecial(&Data));
	if(RetVal == OLEUI_OK)
	{
		//Apparently, richedit handles linking for us; unfortunately, some
		//objects do not embed (MS Word/Office 97 simply fails to embed,
		//although linking works...
		HRESULT Result = LOG_FUNCTION_CALL(RichEditOle->ImportDataObject(Data.lpSrcDataObj,
			Formats[Data.nSelectedIndex].fmtetc.cfFormat,
			(Data.dwFlags & PSF_CHECKDISPLAYASICON) ? Data.hMetaPict : 0));

    //Sometimes it only works with CFEmbeddedObject and sometimes with CFEmbedSource
    //I don't know why
    if(Result != S_OK && Data.nSelectedIndex == 0)
      Result = LOG_FUNCTION_CALL(RichEditOle->ImportDataObject(Data.lpSrcDataObj, CFEmbeddedObject,
	      (Data.dwFlags & PSF_CHECKDISPLAYASICON) ? Data.hMetaPict : 0));

    OleCheck(Result);
	}

	if(Data.hMetaPict)
    OleUIMetafilePictIconFree(Data.hMetaPict);
	if(Data.lpSrcDataObj)
    Data.lpSrcDataObj->Release();
	return RetVal == OLEUI_OK;
}
//---------------------------------------------------------------------------
// close any active ole objects (else servers left hanging); return false on
// cancel unconditionally if savePrompt != true (changes are lost)
bool TRichEditOle::CloseActiveObjects(bool savePrompt)
{
	//If no interface, yell
	if(!RichEditOle)
    throw EOleError("RichEdit OLE interface is not valid.");

	//Get the total number of objects
	int objCnt = RichEditOle->GetObjectCount();

	//Check each object and, if active, deactivate and, if open, close
	for(int i = 0; i < objCnt; i++)
  {
		REOBJECT Obj;
		memset(&Obj, 0, sizeof(Obj));
		Obj.cbStruct = sizeof(Obj);

		//Get object data
		if(LOG_FUNCTION_CALL(RichEditOle->GetObject(i, &Obj, REO_GETOBJ_POLEOBJ)) == S_OK)
    {
			//If active, kill it
			if(Obj.dwFlags & REO_INPLACEACTIVE)
				RichEditOle->InPlaceDeactivate();
			//If open, close it (prompting if requested)
			HRESULT hr = S_OK;
			if(Obj.dwFlags & REO_OPEN)
				hr = Obj.poleobj->Close(savePrompt ?
					OLECLOSE_PROMPTSAVE : OLECLOSE_NOSAVE);
			//Release the interface
			Obj.poleobj->Release();
			//If cancelled, return false
			if(hr == OLE_E_PROMPTSAVECANCELLED)
        return false;
		}
  }

	return true;
}
//---------------------------------------------------------------------------
//Opens selected OLE object; returns false on failure
bool TRichEditOle::OpenObject()
{
	REOBJECT Obj;
	memset(&Obj, 0, sizeof(Obj));
  Obj.cbStruct = sizeof(Obj);
  Obj.cp = RichEdit->SelStart;
  if(LOG_FUNCTION_CALL(RichEditOle->GetObject(REO_IOB_USE_CP, &Obj, REO_GETOBJ_POLEOBJ)) == S_OK)
  {
    RECT Rect = RichEdit->ClientRect;
    HRESULT Result = LOG_FUNCTION_CALL(Obj.poleobj->DoVerb(OLEIVERB_OPEN, NULL, GetClientSite(), 0, RichEdit->Handle, &Rect));
    Obj.poleobj->Release();
    return Result == S_OK;
  }
  return false;
}
//---------------------------------------------------------------------------
bool TRichEditOle::ObjectSelected()
{
	REOBJECT Obj;
	memset(&Obj, 0, sizeof(Obj));
  Obj.cbStruct = sizeof(Obj);
  Obj.cp = RichEdit->SelStart;
  //GetObject() returns E_INVALIDARG if there is no object at the given position
  return LOG_FUNCTION_CALL(RichEditOle->GetObject(REO_IOB_USE_CP, &Obj, REO_GETOBJ_NO_INTERFACES)) == S_OK;
}
//---------------------------------------------------------------------------
void TRichEditOle::UpdateAll()
{
  unsigned Count = GetLinkCount();
  for(unsigned I = 0; I < Count; I++)
  {
  	REOBJECT Obj = {0};
    Obj.cbStruct = sizeof(Obj);
    if(LOG_FUNCTION_CALL(RichEditOle->GetObject(I, &Obj, REO_GETOBJ_POLEOBJ)) == S_OK)
    {
      if(Obj.dwFlags & REO_LINK)
        Obj.poleobj->Update();
      Obj.poleobj->Release();
    }
  }
}
//---------------------------------------------------------------------------



