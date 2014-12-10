/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * Func32 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Func32; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include <tchar.h>
#include <atl\atlmod.h>
#include "OleServerImpl.h"
#include "StackTrace.h"
#include "ConfigRegistry.h"
//---------------------------------------------------------------------------
USEFORM("Frames\TStdFuncFrame.cpp", StdFuncFrame); /* TFrame: File Type */
USEFORM("Frames\TTanFrame.cpp", TanFrame); /* TFrame: File Type */
USEFORM("Frames\TPolFuncFrame.cpp", PolFuncFrame); /* TFrame: File Type */
USEFORM("Forms\Unit1.cpp", Form1);
USEFORM("Forms\Unit10.cpp", Form10);
USEFORM("Forms\Unit13.cpp", Form13);
USEFORM("Forms\Unit14.cpp", Form14);
USEFORM("Forms\Unit11.cpp", Form11);
USEFORM("Forms\Unit12.cpp", Form12);
USEFORM("Forms\Unit6.cpp", Form6);
USEFORM("Forms\Unit7.cpp", Form7);
USEFORM("Forms\Unit4.cpp", Form4);
USEFORM("Forms\Unit5.cpp", Form5);
USEFORM("Forms\Unit8.cpp", Form8);
USEFORM("Frames\TEvalFrame.cpp", EvalFrame);
USEFORM("Frames\TParFuncFrame.cpp", ParFuncFrame); /* TFrame: File Type */
USEFORM("Forms\Unit9.cpp", Form9);
USEFORM("Frames\TAreaFrame.cpp", AreaFrame); /* TFrame: File Type */
USEFORM("Forms\Unit3.cpp", Form3);
USEFORM("Forms\Unit17.cpp", Form17);
USEFORM("Forms\Unit18.cpp", Form18);
USEFORM("Forms\Unit15.cpp", Form15);
USEFORM("Forms\Unit16.cpp", Form16);
USEFORM("Forms\Unit19.cpp", Form19);
USEFORM("Forms\Unit21.cpp", Form21);
USEFORM("Forms\Unit22.cpp", Form22);
USEFORM("Forms\Unit2.cpp", Form2);
USEFORM("Forms\Unit20.cpp", Form20);
//---------------------------------------------------------------------------
TComModule _ProjectModule(0 /*InitATLServer*/);
TComModule &_Module = _ProjectModule;
// The ATL Object map holds an array of _ATL_OBJMAP_ENTRY structures that
// described the objects of your OLE server. The MAP is handed to your
// project's CComModule-derived _Module object via the Init method.
//
BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_OleServer, TOleServerImpl)
END_OBJECT_MAP()
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
  try
  {
    //Always initialize to Western settings, so the deafult English interface looks correct.
    SysLocale.MiddleEast = false;
    Application->BiDiMode = bdLeftToRight;

    //Translations are used by UpdateRegistry(), which is called from TApplication::Initialize()
    std::wstring Language = GetRegValue(REGISTRY_KEY, L"Language", HKEY_CURRENT_USER, L"");
    if(Language.empty())
      LoadLanguage(GetRegValue(REGISTRY_KEY, L"Language", HKEY_LOCAL_MACHINE, L"English").c_str());
    else
      LoadLanguage(Language.c_str());

    bool Register = FindCmdLineSwitch(L"REGSERVER") || FindCmdLineSwitch(L"UNREGSERVER");
    //Only show main form if we are not running as OLE server
    Application->ShowMainForm = !FindCmdLineSwitch(L"EMBEDDING") && !Register;
    //Exit has been disabled in atlmod.h:242 and replaced by this to avoid crash at exit
 	  if(Register)
      Application->Terminate();
    Application->Initialize();
    Application->MainFormOnTaskBar = true;
    Application->Title = "Graph";
    Application->CreateForm(__classid(TForm1), &Form1);
    Application->Run();
  }
  catch (Exception &E)
  {
    LogUncaughtException(NULL, &E);
  }
  catch(std::exception &E)
  {
    MessageBox(String("Uncaught exception detected in main().\nType: ") + typeid(E).name() + "\nMessage: " + String(E.what()), L"Application error", MB_ICONSTOP);
  }
  catch (...)
  {
    try
    {
      throw Exception("Unknown exception escaped from WinMain");
    }
     catch (Exception &exception)
     {
       Application->ShowException(&exception);
     }
  }
  return 0;
}
//---------------------------------------------------------------------------
