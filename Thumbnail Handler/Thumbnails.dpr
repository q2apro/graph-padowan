library Thumbnails;

{$include ..\Include\EasyIncludeHandler.inc}

uses
  Windows,
  Classes,
  ActiveX,
  EasyIDEComponents,
  EasyCommonObjects,
  EasyThumbnailHandler,
  Unit1 in 'Unit1.pas' {DataModule1: TDataModule};

{$R Winxp_DLL.res}
{$R *.res}

var
  Initialized: Boolean = False;

// Ensures that the library is initialized (core objects are created) and is
// initialized only once
procedure Initialize;
begin
  if not Initialized then
  begin
    RegisterClass(TEasyThumbnailHandler);
    RegisterClass(TEasyFactoryManager);
    OleInitialize(nil);
    InitializeLibrary;
    Initialized := True
  end
end;

// Ensures that the library is finalized (core objects are destroyed)
procedure Finalize;
begin
  if Initialized then
  begin
    OLEUnInitialize;
    FinalizeLibrary;
    Initialized := False
  end
end;

function DllGetClassObject(const CLSID, IID: TGUID; var Obj: Pointer): HResult; stdcall;
begin
  // Validate Obj Address
  if @Obj = nil then
  begin
    Result := E_POINTER;
    Exit
  end;

  Obj := nil;
  try
    Result := CLASS_E_CLASSNOTAVAILABLE;
    // Make sure the objects are initialized
    Initialize;
    if IsEqualIID(IID, IClassFactory) then
    begin
      if Assigned(FactoryManager) then
      begin
        IUnknown(Obj) := FactoryManager.CreateInstance(CLSID);
        if Assigned(Obj) then
          Result := S_OK;
      end
    end
  except
    Obj := nil;
    Result := CLASS_E_CLASSNOTAVAILABLE;
  end
end;

function DllCanUnloadNow: HResult; stdcall;
begin
  try
    if ObjectCount = 0 then
      Result := S_OK
    else
      Result := S_False
  except
    Result := S_FALSE;
  end
end;

function DllRegisterServer: HResult; stdcall;
begin
  try
    // Make sure the objects are initialized
    Initialize;
    if RegisterLibrary and Assigned(FactoryManager) then
    begin
      FactoryManager.RegisterHandlers;
      Result := S_OK;
    end else
      Result := S_FALSE;
  except
    Result := S_FALSE;
  end
end;

function DllUnregisterServer: HResult; stdcall;
begin
  try
    // Make sure the objects are initialized
    Initialize;
    if UnRegisterLibrary and Assigned(FactoryManager) then
    begin
      FactoryManager.UnRegisterHandlers;
      Result := S_OK;
    end else
      Result := S_FALSE;
  except
    Result := S_FALSE;
  end
end;

exports
  DllGetClassObject,
  DllCanUnloadNow,
  DllRegisterServer,
  DllUnregisterServer;

// There has *always* been a stability problem with the Win32 Delphi RTL.
// I finally got a demo to constantly crash and Mathias Rauen FINALLY tracked
// it down!  I now feel it is feasable to write Global Hooks and DLLs that
// run in other processes address space, like a NSE.

// Patch by Mathias Rauen
// Until Borland fixed the RTL the fix is very clean to implement outside of the
// RTL.

{   "I have found the bug in the RTL and I have a clean workaround. Let me
    explain the situation:

    DLL_PROCESS_ATTACH -> SysInit.InitProcessTLS -> SysInit.InitThreadTLS
    DLL_THREAD_ATTACH -> SysInit.InitThreadTLS
    DLL_THREAD_DETACH -> SysInit.ExitThreadTLS
    DLL_PROCESS_DETACH -> SysInit.ExitProcessTLS -> SysInit.ExitThreadTLS

    As you can see, DLL_XXX_ATTACH always ends up in "InitThreadTLS", while
    DLL_XXX_DETACH always ends up in "ExitThreadTLS". We can forget about
    "Init/ExitProcessTLS". Now let's go through some situations. For all of
    the following cases please note that the events are meant to be for the
    very same thread (that's important!!).

    (1)
    InitThreadTLS
    ExitThreadTLS
    -> perfectly fine (standard case)

    (2)
    ExitThreadTLS
    -> strange situation, but no problems

    (3)
    InitThreadTLS
    -> memory leak (but I think this situation will never occur)

    (4)
    InitThreadTLS
    InitThreadTLS
    ExitThreadTLS
    -> memory leak (but I think this situation will never occur)

    (5)
    InitThreadTLS
    ExitThreadTLS
    ExitThreadTLS
    -> LocalAlloc gets called twice for the same pointer

    Now what happens when doing CBT hooking in win98 is situation (5). The
    very same thread gets a DLL_PROCESS_ATTACH + DLL_THREAD_DETACH + 
    DLL_PROCESS_DETACH event. And the end result is the Explorer crash. If 
    you ask me, the Borland programmers didn't believe, that case (5) can
    happen - but it does. Now let's look at my patched "ExitThreadTLS"
    function. I just added one line: "
}

procedure madPatch_ExitThreadTLS;
var
  p: Pointer;
begin
  if @TlsLast = nil then
    Exit;
  if TlsIndex <> -1 then
  begin
    p := TlsGetValue(TlsIndex);
    if p <> nil then
    begin

      // In D5 the RTL is different.  It calls the ExitThreadTLS in SysInit
      // BEFORE it calls DLLPROC (where this is called).  In these compilers we
      // only need to nil the TLS!

      {$IFDEF COMPILER_6_UP}
      LocalFree(Cardinal(p));
      {$ENDIF COMPILER_6_UP}
      TlsSetValue(TlsIndex, nil);   // <- this fixes case (5)
    end;
  end;
end;


procedure DLLEntryProc(EntryCode: integer);
begin
  case EntryCode of
    DLL_PROCESS_DETACH:
      begin
        // The process is dumping us so clean up
        Finalize;
      end;
    DLL_PROCESS_ATTACH:
      begin
      end;
    DLL_THREAD_ATTACH:
      begin
      end;
    DLL_THREAD_DETACH:
      begin
        // We are freeing the TLS AND setting it to nil before
        // the RTL can get to it to fix the Delphi global DLL stablility
        // issue.
        madPatch_ExitThreadTLS;
      end;
  end;
end;

begin
  IsMultiThread := True;
  DLLProc := @DLLEntryProc;
  DLLEntryProc(DLL_PROCESS_ATTACH);
end.
