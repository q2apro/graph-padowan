%module Utility

%include "std_wstring.i"
%include "Types.i"

%begin %{
#include "Graph.h"
#include "Unit1.h"
#include "Python.hpp"
#define WRAP_PYOBJECTS
#include "PythonBind.h"
#include "PyGraph.h"
#include "PythonBind.h"
#pragma warn -8060
%}

%inline %{
  void BeginMultiUndo() {UndoList.BeginMultiUndo();}
  void EndMultiUndo() {UndoList.EndMultiUndo();}
  void LoadDefault() { Form1->LoadDefault();}
  bool LoadFromFile(const std::wstring &FileName, bool AddToRecent = true, bool ShowErrorMessages = true)
  {
    return Form1->LoadFromFile(FileName.c_str(), AddToRecent, ShowErrorMessages);
  }

  bool SaveToFile(const std::wstring &FileName, bool Remember = true) {return Form1->Data.Save(FileName, Remember);}
  void ImportPointSeries(const std::wstring &FileName, char Separator = 0) {Form1->Data.ImportPointSeries(FileName, Separator);}
  void Import(const std::wstring &FileName) {Form1->Data.Import(FileName);}
%}

