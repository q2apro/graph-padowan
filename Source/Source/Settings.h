/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
class TConfigRegistry;

namespace Graph
{
enum TAxesStyle {asNone, asCrossed, asBoxed};
enum TLegendPlacement {lpCustom, lpTopRight, lpBottomRight, lpTopLeft, lpBottomLeft};
enum TNumberPlacement{npCenter, npBefore};

struct TAxis
{
  double Min;
  double Max;
  bool LogScl, MultipleOfPi;
  bool ShowLabel, ShowNumbers, ShowTicks, ShowGrid;
  bool AutoTick, AutoGrid;
  std::wstring Label;
  double AxisCross;
  double TickUnit;
  double GridUnit;
  bool Visible;
  bool ShowPositiveArrow;
  bool ShowNegativeArrow;
  TNumberPlacement NumberPlacement;

  TAxis();
  void WriteToIni(TConfigFileSection &IniFile, const std::wstring &Prefix) const;
  void ReadFromIni(const TConfigFileSection &IniFile, const std::wstring &Prefix);
};

enum TGridStyle
{
  gsLines, gsDots
};

struct TAxes
{
  TAxis xAxis;
  TAxis yAxis;
  TColor AxesColor;
  TColor GridColor;
  TColor BackgroundColor;
  TVclObject<TFont> NumberFont;
  TVclObject<TFont> LabelFont;
  TVclObject<TFont> LegendFont;
  std::wstring Title;
  TVclObject<TFont> TitleFont;
  bool ShowLegend;
  Func32::TTrigonometry Trigonometry;
  TAxesStyle AxesStyle;
  TLegendPlacement LegendPlacement;
  Func32::TDblPoint LegendPos; //Only valid when LegendPlacement is lpCustom
  bool CalcComplex;//When true: Complex numbers are used for graphing
  TGridStyle GridStyle;

  TAxes();
  void WriteToIni(TConfigFileSection &IniFile) const;
  void ReadFromIni(const TConfigFileSection &IniFile);
  void HandleZoomSquare(double xyScale);
};

enum TComplexFormat {cfReal,cfRectangular,cfPolar};
enum TFunctionType {ftStdFunc, ftParFunc, ftPolFunc};

struct TDefaultData
{
  unsigned Style;
  TColor Color;
  unsigned Size;
  std::wstring ToString() const;
  void FromString(const std::wstring &Str, unsigned AStyle, TColor AColor, unsigned ASize);
  void Set(unsigned AStyle, TColor AColor, unsigned ASize);
};

struct TProperty
{
  int RoundTo; //decimals to round to
  bool SavePos;//True is the windows should have the same size at start as at last exit
  TComplexFormat ComplexFormat;//Format then complex numbers are shown
  bool CheckForUpdate;
  TDefaultData DefaultFunction, DefaultPoint, DefaultPointLine, DefaultShade, DefaultTrendline, DefaultRelation, DefaultTangent, DefaultDif;
  TVclObject<TFont> DefaultPointLabelFont, DefaultLabelFont;
  bool ShowTipsAtStartup;
  unsigned NextTip;
  std::wstring Language;
  int FontScale; //Window scale in percent
  bool CustomDecimalSeparator; //Default system decimal separator is used when false
  wchar_t DecimalSeparator;    //Decimal separator used when exporting files

  TProperty();
  void Read(const TConfigRegistry &Registry);
  void Write(class TConfigRegistry &Registry);
};

enum TCustomFunctionErrorCode
{
  cfeEmptyName,           //!< Empty name string
  cfeInvalidName,         //!< Invalid character in name: ErrorPos is the index in the string
  cfeEmptyArg,            //!< Empty argument name
  cfeInvalidArg,          //!< Invalid character in argument: ErrorPos is the index in the string
  cfeEndParMissing,       //!< No end parenthesis found: ErrorPos is the index in the name string
  cfeCharAfterEndPar,     //!< Character found after end parenthesis: ErrorPos is the index in the name string
  cfeDoubleDefinedSymbol, //!< Symbol has been defined more than once
  cfeParseError,          //!< Error parsing String: ErrorPos is the index; ParseError contains actual error information
  cfeSymbolUndefined      //!< The requested symbol was not found
};

struct ECustomFunctionError : public std::exception
{
  TCustomFunctionErrorCode ErrorCode;
  int ErrorPos;
  std::wstring Text;
  Func32::EParseError ParseError;

  ECustomFunctionError(TCustomFunctionErrorCode AErrorCode, int AErrorPos = 0)
    : ErrorCode(AErrorCode), ErrorPos(AErrorPos) {}
  ECustomFunctionError(TCustomFunctionErrorCode AErrorCode, int AErrorPos, const std::wstring &AText)
    : ErrorCode(AErrorCode), ErrorPos(AErrorPos), Text(AText) {}
  ECustomFunctionError(TCustomFunctionErrorCode AErrorCode, int AErrorPos, const Func32::EParseError &AParseError)
    : ErrorCode(AErrorCode), ErrorPos(AErrorPos), ParseError(AParseError) {}
};

struct TCustomFunction
{
  std::wstring Name;
  std::wstring Text;
  std::vector<std::wstring> Arguments;
  Func32::TCustomFuncPtr Func;
  Func32::TCustomFuncPtr InternalFunc;
  TCustomFunction(const std::wstring &Str, const std::wstring &AText);
  TCustomFunction(const std::wstring &AName, const std::vector<std::wstring> &AArguments, const std::wstring &AText)
    : Name(AName), Arguments(AArguments), Text(AText) {}
  std::wstring GetName() const;
  static std::wstring CheckAndTrimName(const std::wstring &Str, unsigned Offset);
};

class TCustomFunctions
{
  std::vector<TCustomFunction> Functions;
  typedef std::vector<TCustomFunction>::iterator TIterator;
  Func32::TSymbolList InternalSymbolList; //Unoptimized

public:
  typedef std::vector<TCustomFunction>::const_iterator TConstIterator;
  Func32::TSymbolList SymbolList;
  static Func32::TSymbolList GlobalSymbolList;

  TCustomFunctions();
  TCustomFunctions(const TCustomFunctions &Other);
  void Add(const std::wstring &Str, const std::wstring &Value);
  void Add(const std::wstring &Name, const Func32::TArgType &Args, const std::wstring &Text);
  void Replace(const std::wstring &Name, const std::wstring &Value);
  void Replace(const std::wstring &Name, long double Value);
  void Delete(const std::wstring &Name);
  const TCustomFunction& GetValue(const std::wstring &Name) const;
  void Update(const TData &Data);
  void UpdateAll(bool IgnoreErrors=false);
  void Clear();
  void Swap(TCustomFunctions &Other);
  void WriteToIni(TConfigFileSection &IniFile) const;
  void ReadFromIni(const TConfigFileSection &IniFile);
  TConstIterator Begin() const {return Functions.begin();}
  TConstIterator End() const {return Functions.end();}
  void Reset();
};

struct TAnimationConstant
{
  std::wstring Min;
  std::wstring Max;
  std::wstring Step;
};

struct TAnimationInfo
{
  std::wstring Constant;
  std::map<std::wstring, TAnimationConstant> ConstantList;
  unsigned Width;
  unsigned Height;
  double FramesPerSecond;

  TAnimationInfo() : Width(0), Height(0), FramesPerSecond(1) {}
  void WriteToIni(TConfigFileSection &IniFile) const;
  void ReadFromIni(const TConfigFileSection &IniFile);
  void Clear();
};

struct TGuiFormatSettings
{
  std::wstring CartesianPointFormat;
  std::wstring DegreePointFormat;
  std::wstring RadianPointFormat;
  TGuiFormatSettings();
};

struct TPlotSettings
{
  int AxisWidth;
  int GridWidth;
  int xNumberDist;
  int yNumberDist;
  int TickWidth;
  int TickLength;
  TPlotSettings();
};

struct TGuiSettings
{
  double MajorZoomIn;
  double MinorZoomIn;
  double MajorZoomOut;
  double MinorZoomOut;
  double MajorStepSize;
  double MinorStepSize;
  double MouseZoomIn;
  double MouseZoomOut;
  TGuiSettings();
};

extern TProperty Property;
extern TGuiFormatSettings GuiFormatSettings;
extern TPlotSettings PlotSettings;
extern TGuiSettings GuiSettings;
} //namespace Graph;
using namespace Graph;
#endif
