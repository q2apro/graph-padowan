/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef DataH
#define DataH
#include "VclObject.h"
#include "GraphElem.h"
#include "Settings.h"
#include "Convert.h"

class TConfigFile;
namespace Graph
{
struct TUserModel
{
  std::wstring Model;
  std::vector<std::pair<std::wstring, std::wstring> > Defaults;
};

typedef std::map<std::wstring, TUserModel> TUserModels;

typedef bool (__closure *TAbortUpdateEvent)();


class TData
{
  mutable bool Modified;
  mutable std::wstring GrfName;
  TAbortUpdateEvent OnAbortUpdate;
  boost::shared_ptr<TTopGraphElem> TopElem;

  void SaveData(TConfigFile &IniFile) const;
  const TData& operator=(const TData&);             //Not implemented
  static void WriteInfoToIni(TConfigFile &IniFile);
  void SaveImage(TConfigFile &IniFile, TCanvas *Canvas, int Width, int Height) const;
  static void WriteElem(TConfigFile &IniFile, const TGraphElemPtr &Elem, struct TElemCount &Count);
  static void WriteCount(TConfigFile &ConfigFile, const TElemCount &Count);

public:
  TAxes Axes;
  TUserModels UserModels;
  TCustomFunctions CustomFunctions;
  TAnimationInfo AnimationInfo;
  std::map<std::wstring,std::wstring> PluginData;

  TData();
  TData(const TData &OldData);
  void ClearCache();
  void Clear();
  void LoadDefault();
  void LoadFromFile(const std::wstring &FileName);
  void Load(TConfigFile &IniFile);
  void LoadPluginData(const TConfigFileSection &Section);
  bool Save(const std::wstring &FileName, bool Remember) const;
  std::wstring SaveToString(bool ResetModified) const;
  std::wstring SaveToString(const TGraphElemPtr &Elem) const;
  void SaveDefault() const;
  static void CheckIniInfo(const TConfigFile &IniFile);
  void LoadData(const TConfigFile &IniFile);
  void PreprocessGrfFile(TConfigFile &IniFile);
  void Import(const std::wstring &FileName);
  void Import(TConfigFile &ConfigFile);
  bool ImportPointSeries(const std::wstring &FileName, char Separator);
  std::wstring CreatePointSeriesDescription() const;
  boost::shared_ptr<TTextLabel> FindLabel(int X, int Y) const; //NULL indicates not label found
  void DeleteLabel(int Index);
  void ImportUserModels(const std::wstring &Str);
  std::wstring ExportUserModels() const;
  void SetModified();
  bool IsModified() const {return Modified;}
  double FindInterception(const TBaseFuncType *Func, int X, int Y) const;

  static void Delete(const TGraphElemPtr &Elem);
  void Insert(const TGraphElemPtr &Elem, int Index=-1);
  static int GetIndex(const TGraphElemPtr &Elem);
  void Replace(unsigned Index, const TGraphElemPtr &Elem);
  static void Replace(const TGraphElemPtr &OldElem, const TGraphElemPtr &NewElem);
  unsigned ElemCount() const {return TopElem->ChildCount();}
  TGraphElemPtr GetElem(unsigned Index) const;
  boost::shared_ptr<TBaseFuncType> GetFuncFromIndex(unsigned Index) const;
  const boost::shared_ptr<TTopGraphElem>& GetTopElem() const {return TopElem;}
  TGraphElemPtr Back() const {return GetElem(ElemCount()-1);}

  const std::wstring& GetFileName() const {return GrfName;}
  void Update();
  void SetAbortUpdateEvent(TAbortUpdateEvent AOnAbortUpdate) {OnAbortUpdate = AOnAbortUpdate;}
  void AbortUpdate() const {if(OnAbortUpdate) OnAbortUpdate();}

  long double Calc(const std::wstring &Str) const
  {
    return Eval(Str, CustomFunctions.SymbolList, Axes.Trigonometry);
  }

  Func32::TComplex CalcComplex(const std::wstring &Str) const
  {
    return EvalComplex(Str, CustomFunctions.SymbolList, Axes.Trigonometry);
  }

  bool IsDependent(const std::wstring &Expression, const std::wstring &SymbolName) const;
};
//---------------------------------------------------------------------------
//extern TData Data;

enum TTraceType {ttTrace, ttIntersection, ttXAxis, ttYAxis, ttExtremeX, ttExtremeY};
double TraceFunction(const TBaseFuncType *Func, TTraceType TraceType, int X, int Y, const TData &Data, const class TDraw &Draw);
bool ExportPointSeries(const TPointSeries *Series, const wchar_t *FileName, char Delimiter, char DecimalSeparator);
double FindNearestPoint(const TBaseFuncType *Func, int X, int Y);
} //namespace Graph
using namespace Graph;
//---------------------------------------------------------------------------
#endif
