//===========================================================================
// Copyright © 1998 Thin Air Enterprises and Robert Dunn
//===========================================================================
//---------------------------------------------------------------------------
// class TIRichEditOle - adds basic OLE functionality to TRichEdit.
// Based on code found at http://www.dystopia.fi/~janij/techinfo/richedit.htm
// and presumably written by Jani Järvinen.  Thanks, Jani.
//
// Additional code developed through examination of Borland's VCL library,
// Microsoft's MFC source code, and sample code available on Microsoft's
// developer web site.
//
// Note that this code is very experimental -- the author admits to only a
// vague familiarity with OLE and accepts no criticism of the code.  Many,
// if not most, of the interfaces return failure codes arbitrarily chosen by
// the author with no particular reason to think that the values are correct.
// In particular, no great effort has been expended looking for "memory
// leaks," and these are considered quite probable.  You have been warned.
//---------------------------------------------------------------------------
#ifndef RichEditOLEH
#define RichEditOLEH

#include <comctrls.hpp>
#include <comobj.hpp>
#include <ole2.hpp>
#include <oledlg.hpp>
#include <richole.h>
#include "RichEditOleCallback.h"
//---------------------------------------------------------------------------
class TRichEditOle
{
  static void OleUIMetafilePictIconFree(HGLOBAL MetaPict);
  static bool OleStdSwitchDisplayAspect(LPOLEOBJECT lpOleObj,	LPDWORD lpdwCurAspect,
    DWORD dwNewAspect, HGLOBAL hMetaPict,	BOOL fDeleteOldAspect,
    BOOL fSetupViewAdvise, LPADVISESINK lpAdviseSink,	LPBOOL lpfMustUpdate);

protected:
	TCustomRichEdit* RichEdit;
	IRichEditOle* RichEditOle;
	TRichEditOleCallback RichEditCallback;

	UINT CFObjectDescriptor;
	UINT CFEmbeddedObject;
	UINT CFEmbedSource;
	UINT CFLinkSource;
	UINT CFRtf;
	UINT CFRtfNoObjs;
	UINT CFReTextObjs;

	LONG GetObjectCount();
	LONG GetLinkCount();
	IOleClientSite* GetClientSite();
  
public:
	TRichEditOle(TCustomRichEdit* ARichEdit);
	~TRichEditOle();
	void SetHostNames(const AnsiString &HostApp, const AnsiString &HostDoc);
	bool InsertObject();
	bool PasteSpecial();
	bool CloseActiveObjects(bool SavePrompt);
  bool OpenObject();
  bool ObjectSelected();
  void UpdateAll(); //Update all linked objects

	__property LONG ObjectCount = {read = GetObjectCount};
	__property LONG LinkCount = {read = GetLinkCount};
};
#endif

