# ---------------------------------------------------------------------------
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif

# ---------------------------------------------------------------------------
# IDE SECTION
# ---------------------------------------------------------------------------
# The following section of the project makefile is managed by the BCB IDE.
# It is recommended to use the IDE to change any of the values in this
# section.
# ---------------------------------------------------------------------------

VERSION = BCB.06.00
# ---------------------------------------------------------------------------
PROJECT = Graph.exe
OBJFILES = Clipboard.obj ..\Components\Common\ConfigFile.obj Common.obj Context.obj \
    Convert.obj Data.obj Draw.obj DrawThread.obj Files.obj Graph.obj \
    GraphElem.obj GuiHelper.obj GuiUtil.obj IGraphic.obj \
    ..\Components\Common\PngHelper.obj Settings.obj StackTrace.obj \
    VersionInfo.obj ..\Components\Common\Undo.obj Unit1.obj Unit2.obj \
    Unit3.obj Unit4.obj Unit5.obj Unit6.obj Unit7.obj Unit8.obj Unit9.obj \
    Unit10.obj Unit11.obj Unit12.obj Unit13.obj Unit14.obj Unit15.obj \
    Unit16.obj Unit17.obj Unit18.obj ..\Components\Common\Tokenizer.obj \
    TStdFuncFrame.obj TParFuncFrame.obj TPolFuncFrame.obj TTanFrame.obj \
    TAreaFrame.obj OleServerImpl.obj Graph_TLB.obj \
    d:\progra~1\dxgettext\gnugettext.obj ..\PDFlib-Lite\bind\pdflib\cpp\pdflib.obj
RESFILES = Graph.res Graph.tlb Resource.res
MAINSOURCE = Graph.cpp
RESDEPEN = $(RESFILES) Unit1.dfm Unit2.dfm Unit3.dfm Unit4.dfm Unit5.dfm Unit6.dfm \
    Unit7.dfm Unit8.dfm Unit9.dfm Unit10.dfm Unit11.dfm Unit12.dfm Unit13.dfm \
    Unit14.dfm Unit15.dfm Unit16.dfm Unit17.dfm Unit18.dfm TStdFuncFrame.dfm \
    TParFuncFrame.dfm TPolFuncFrame.dfm TTanFrame.dfm TAreaFrame.dfm
LIBFILES = ..\Func32\Func32.lib ..\PDFlib-Lite\PDFlib.lib
IDLFILES = 
IDLGENFILES = 
LIBRARIES = vclnp.lib \   
    vclx.lib Func32.lib indy.lib \
    rtl.lib vcl.lib IComp.lib
PACKAGES = vcl.bpi rtl.bpi IComp.bpi indy.bpi
SPARELIBS = vcl.lib rtl.lib IComp.lib indy.lib \
    Func32.lib vclx.lib
DEFFILE = 
OTHERFILES = 
# ---------------------------------------------------------------------------
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
USERDEFINES = 
SYSDEFINES = NO_STRICT;USING_ATLVCL;_ATL_NO_SECURITY;INC_ATL_HEADERS;_ATL_SINGLEUSE_INSTANCING;_ATL_MULTIUSE_INSTANCING;_ATL_APARTMENT_THREADED
INCLUDEPATH = ..\PDFlib-Lite\bind\pdflib\cpp;..\PDFlib-Lite\libs\pdflib;..\Components\TSymbolDialog;..\Components\TExtColorBox;"D:\Program Files\Borland\CBuilder6\Source\Rtl\Source\except";..\Components\TSaveDialogEx;..\Components\TIPrintDialog;d:\progra~1\dxgettext;..\Components\TOpenPreviewDialog;..\Func32;..\Components\TIFontBox;..\components\tipolygon;..\Components\TIPageSetup;..\Components\TIColorSelect;$(BCB)\include;$(BCB)\include\vcl;..\components\ticolorbox;..\Components\TCross;..\Components\TFocusPanel;..\Components\TGrid;..\Components\TLineSelect;..\Components\TMyEdit;..\Components\TMyRadioButton;..\Components\TMySplitter;..\Components\TPointSelect;..\Components\TProgress;..\Components\TProgressForm;..\Components\TRecent;..\Components\TShadeSelect;..\BMGLib\BMGLib_DLL;..\Components\Common;..\Components\TIRichEdit;$(BCB)\include\atl;..\boost;..\BMGLib\lpng125;..\BMGLib\zlib114;$(BCB)\source\rtl\rtlinc
LIBPATH = ..\PDFlib-Lite;..\Components\TSaveDialogEx;d:\progra~1\dxgettext;..\Components\TOpenPreviewDialog;..\Components\TIRichEdit;..\Components\TIPrintDialog;..\Components\Common;..\Components\TSymbolDialog;..\Components\TIFontBox;..\components\tipolygon;..\Components\TIPageSetup;..\Components\TIColorSelect;..\Components\TMyEdit;..\components\ticolorbox;$(BCB)\Projects\Lib;$(BCB)\lib\obj;$(BCB)\lib;..\Func32;..\Components\TGrid;..\Components\TCross;..\Components\TFocusPanel;..\Components\TMySplitter;..\Components\TLineSelect;..\Components\TMyRadioButton;"D:\Program Files\Borland\CBuilder6\Lib\Psdk"
WARNINGS= -wprc -wuse -wucp -w-par -w-8027 -wdef -wbbf -wasm -wamp -wamb -w-8026 -w-8058 -w-8084
PATHCPP = .;..\Components\Common;..\PDFlib-Lite\bind\pdflib\cpp
PATHASM = .;
PATHPAS = .;d:\progra~1\dxgettext
PATHRC = .;
PATHOBJ = .;$(LIBPATH)
# ---------------------------------------------------------------------------
CFLAG1 = -O2 -H=b:\vcl60.csm -Hc -Vx -Ve -xp -X- -a8 -6 -b- -d -k- -vi -c -tW -tWM
IDLCFLAGS = -src_suffix cpp -IDAN -I..\components\ticolorbox -I$(BCB)\include \
    -I$(BCB)\include\vcl -I..\Components\TColorSelect -I..\Components\TCross \
    -I..\Components\TFocusPanel -I..\Components\TGrid \
    -I..\Components\TLineSelect -I..\Components\TMyEdit \
    -I..\Components\TMyRadioButton -I..\Components\TMySplitter \
    -I..\Components\TPageSetup -I..\Components\TPointSelect \
    -I..\Components\TProgress -I..\Components\TProgressForm \
    -I..\Components\TRecent -I..\Components\TShadeSelect -boa
PFLAGS = -$Y- -$L- -$D- -$C- -$A8 -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zn
LFLAGS = -aa -Tpe -s -C
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mt.lib
# ---------------------------------------------------------------------------
!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0

[Version Info Keys]
CompanyName=
FileDescription=
FileVersion=1.0.0.0
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[Debugging]
DebugSourceDirs=$(BCB)\source\vcl

!endif





# ---------------------------------------------------------------------------
# MAKE SECTION
# ---------------------------------------------------------------------------
# This section of the project file is not used by the BCB IDE.  It is for
# the benefit of building from the command-line using the MAKE utility.
# ---------------------------------------------------------------------------

.autodepend
# ---------------------------------------------------------------------------
!if "$(USERDEFINES)" != ""
AUSERDEFINES = -d$(USERDEFINES:;= -d)
!else
AUSERDEFINES =
!endif

!if !$d(BCC32)
BCC32 = bcc32
!endif

!if !$d(CPP32)
CPP32 = cpp32
!endif

!if !$d(DCC32)
DCC32 = dcc32
!endif

!if !$d(TASM32)
TASM32 = tasm32
!endif

!if !$d(LINKER)
LINKER = ilink32
!endif

!if !$d(BRCC32)
BRCC32 = brcc32
!endif


# ---------------------------------------------------------------------------
!if $d(PATHCPP)
.PATH.CPP = $(PATHCPP)
.PATH.C   = $(PATHCPP)
!endif

!if $d(PATHPAS)
.PATH.PAS = $(PATHPAS)
!endif

!if $d(PATHASM)
.PATH.ASM = $(PATHASM)
!endif

!if $d(PATHRC)
.PATH.RC  = $(PATHRC)
!endif

!if $d(PATHOBJ)
.PATH.OBJ  = $(PATHOBJ)
!endif
# ---------------------------------------------------------------------------
$(PROJECT): $(OTHERFILES) $(IDLGENFILES) $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) -L$(LIBPATH) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!
# ---------------------------------------------------------------------------
.pas.hpp:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.pas.obj:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.cpp.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.cpp.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.asm.obj:
    $(BCB)\BIN\$(TASM32) $(AFLAGS) -i$(INCLUDEPATH:;= -i) $(AUSERDEFINES) -d$(SYSDEFINES:;= -d) $<, $@

.rc.res:
    $(BCB)\BIN\$(BRCC32) $(RFLAGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -fo$@ $<



# ---------------------------------------------------------------------------




