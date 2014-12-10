/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef GraphElemH
#define GraphElemH
#include "../Func32/Func32.h"
#include <boost/weak_ptr.hpp>
#include <deque>
#include <boost/enable_shared_from_this.hpp>
#include <boost\math\special_functions\fpclassify.hpp>

class TConfigFileSection;
namespace Graph
{
struct EGraphError : public std::exception
{
  std::wstring Message;
  EGraphError(const std::wstring &Str) : Message(Str) {}
};

typedef boost::shared_ptr<class TGraphElem> TGraphElemPtr;
typedef boost::weak_ptr<class TGraphElem> TWeakGraphElemPtr;

struct TGraphElemVisitor
{
  virtual void Visit(class TBaseFuncType &Func) =0;
  virtual void Visit(class TTan &Tan) = 0;
  virtual void Visit(class TPointSeries &Series) =0;
  virtual void Visit(class TShading &Shade) =0;
  virtual void Visit(class TTextLabel &Label) =0;
  virtual void Visit(class TRelation &Relation) =0;
  virtual void Visit(class TAxesView &AxesView) =0;
};

struct TTextValue
{
  std::wstring Text;
  double Value;
  TTextValue() : Value(0) {}
  explicit TTextValue(double AValue);
  TTextValue(double AValue, const std::wstring &AText) : Value(AValue), Text(AText) {}
  bool operator==(const TTextValue &TextValue) const {return Value == TextValue.Value;}
  bool operator!=(const TTextValue &TextValue) const {return !(*this == TextValue);}
  bool operator==(double a) const {return Value == a;}
  bool operator<(double a) const {return Value < a;}
  bool operator>(double a) const {return Value > a;}
  bool operator<=(double a) const {return Value <= a;}
  bool operator>=(double a) const {return Value >= a;}
  void Update(const class TData &Data);
  void Set(const std::wstring AText, const TData &Data, bool IgnoreErrors = false);
  void Set(double AValue);
  bool IsFinite() const {return boost::math::isfinite(Value);}
  bool IsDependent(const TData &Data, const std::wstring &SymbolName) const;
};

std::wostream& operator<<(std::wostream &Stream, const TTextValue &TextValue);

class TData;
class TBaseFuncType;

class TGraphElem : public boost::enable_shared_from_this<TGraphElem>
{
  const TGraphElem& operator=(const TGraphElem&); //Not implemented
  bool Visible;
  bool ShowInLegend;
  std::wstring LegendText;
  std::vector<TGraphElemPtr> ChildList;
  TWeakGraphElemPtr Parent;

  void SetParent(const TGraphElemPtr &AParent) {Parent = AParent;}

protected:
  TGraphElemPtr CloneHelper(TGraphElem *NewElem) const;

public:
  std::map<std::wstring,std::wstring> PluginData;

  TGraphElem() : Visible(true), ShowInLegend(true) {}
  TGraphElem(const TGraphElem &Elem);
  virtual ~TGraphElem() {}
  virtual std::wstring MakeLegendText() const {return LegendText.empty() ? MakeText() : LegendText;}
  virtual std::wstring MakeText() const = 0;
  virtual void WriteToIni(TConfigFileSection &Section) const=0;
  virtual void ReadFromIni(const TConfigFileSection &Section) =0;
  virtual void Accept(TGraphElemVisitor&) =0;
  virtual TGraphElemPtr Clone() const = 0;
  virtual void ClearCache();
  virtual void Update();
  virtual const TData& GetData() const {BOOST_ASSERT(!Parent.expired()); return Parent.lock()->GetData();}

  void InsertChild(const TGraphElemPtr &Elem, int Index = -1);
  void ReplaceChild(unsigned Index, const TGraphElemPtr &Elem);
  unsigned GetChildIndex(const TGraphElemPtr &Elem) const;
  void RemoveChild(unsigned Index);
  TGraphElemPtr GetChild(unsigned Index) const {return ChildList.at(Index);} //Don't return a reference as it might be invalid while in use
  unsigned ChildCount() const {return ChildList.size();}
  TGraphElemPtr GetParent() const {return Parent.lock();}

  virtual int GetVisible() const {return Visible;}
  virtual void ChangeVisible() {Visible = !Visible;}
  void SetVisible(int AVisible) {Visible = AVisible;}
  bool GetShowInLegend() const {return ShowInLegend;}
  void SetShowInLegend(bool Value) {ShowInLegend = Value;}
  std::wstring GetLegendText() const {return LegendText;}
  void SetLegendText(const std::wstring &Str) {LegendText = Str;}
  virtual bool IsDependent(const std::wstring &SymbolName) const =0;
};

class TTopGraphElem : public TGraphElem
{
  const TData *Data;

public:
  TTopGraphElem(const TData *AData) : Data(AData) {}
  std::wstring MakeText() const {return L"";}
  void WriteToIni(TConfigFileSection &Section) const {};
  void ReadFromIni(const TConfigFileSection &Section) {};
  void Accept(TGraphElemVisitor&) {};
  TGraphElemPtr Clone() const {throw "Not implemented!";;}
  boost::shared_ptr<TTopGraphElem> Clone(const TData *AData) const;
  const TData& GetData() const {return *Data;}
  bool IsDependent(const std::wstring &SymbolName) const {return false;}
};

enum TLabelPlacement
{
  lpUserTopLeft,
  lpAboveX,
  lpBelowX,
  lpLeftOfY,
  lpRightOfY,
  lpUserTopRight,
  lpUserBottomLeft,
  lpUserBottomRight
};

class TTextLabel : public TGraphElem
{
  std::string Text;
  TLabelPlacement LabelPlacement;
  TTextValue xPos; //Only used when TextLabelPos is tlpCustom
  TTextValue yPos; //Only used when TextLabelPos is tlpCustom
  TRect Rect;
  TColor BackgroundColor;
  TVclObject<TMetafile> Metafile;
  unsigned Rotation; //Rotation in degrees
  std::wstring StatusText; //The text shown in the TreeView
  bool ContainsOleLink; //The Text contains an OLE object link that must be updated when loaded

public:
  TTextLabel();
  TTextLabel(const std::string &Str, TLabelPlacement Placement, const TTextValue &AxPos, const TTextValue &AyPos, TColor Color, unsigned ARotation, bool OleLink);
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  std::wstring MakeText() const;
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  int UpdateRect(int X, int Y) {int Width = Rect.Width(); int Height = Rect.Height(); Rect = TRect(X, Y, X + Width, Y + Height); return Width;}
  bool IsInsideRect(int X, int Y) const {return InsideRect(Rect, TPoint(X, Y));}
  const TTextValue& GetXPos() const {return xPos;}
  void SetXPos(const TTextValue &Value) {xPos = Value;}
  const TTextValue& GetYPos() const {return yPos;}
  void SetYPos(const TTextValue &Value) {yPos = Value;}
  const std::string& GetText() const {return Text;}
  void SetText(const std::string &Str) {Text = Str;}
  void Scale(double xSizeMul, double ySizeMul);
  const TRect& GetRect() const {return Rect;}
  TGraphElemPtr Clone() const {return CloneHelper(new TTextLabel(*this));}
  TColor GetBackgroundColor() const {return BackgroundColor;}
  void SetBackgroundColor(TColor Color) {BackgroundColor = Color;}
  TLabelPlacement GetPlacement() const {return LabelPlacement;}
  void SetPlacement(TLabelPlacement Value) {LabelPlacement = Value;}
  unsigned GetRotation() const {return Rotation;}
  void SetRotation(unsigned Value) {Rotation = Value;}
  TMetafile* GetImage() const {return Metafile;}
  void Update();
  bool IsDependent(const std::wstring &SymbolName) const;
  bool GetOleLink() {return ContainsOleLink;}
};

struct TAxes;
enum TDrawType {dtAuto, dtDots, dtLines};

class TBaseFuncType : public TGraphElem
{
protected:
  TBaseFuncType(const TBaseFuncType &F);
  TTextValue Steps; //Number of steps/evaluations. Rounded to an integer

public:
  std::vector<Func32::TCoordSet<> > sList;
  std::vector<TPoint> Points;
  std::vector<unsigned> PointNum;
  TColor Color;
  unsigned Size;
  TPenStyle Style;
  TTextValue From, To;
  unsigned StartPointStyle, EndPointStyle;
  TDrawType DrawType;

  TBaseFuncType();
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  virtual boost::shared_ptr<TBaseFuncType> MakeDifFunc() =0;
  virtual void GetCurrentRange(double &Min, double &Max, double &ds) const;
  virtual const TTextValue& GetSteps() const {return Steps;}
  void SetSteps(const TTextValue &Value) {Steps = Value;}
  virtual std::wstring GetVariable() const {return L"";}
  virtual const Func32::TBaseFunc& GetFunc() const =0;
  Func32::TBaseFunc& GetFunc() {return const_cast<Func32::TBaseFunc&>(const_cast<const TBaseFuncType*>(this)->GetFunc());}
  void ClearCache();
  void Update();
  Func32::TCoord<long double> Eval(long double t) const;
  virtual long double CalcArea(long double From, long double To) const;
  bool IsDependent(const std::wstring &SymbolName) const;
};

enum TTangentType {ttTangent, ttNormal};
class TTan : public TBaseFuncType
{
  mutable double a, q;  //Calculated at last redraw; a!=INF: y=ax+q, a==INF: x=q
  mutable Func32::TParamFunc TanFunc;
  void UpdateTan(double a1, double q1);
public:
  TTextValue t;
  TTangentType TangentType;

  TTan::TTan();
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  std::wstring MakeLegendText() const;
  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TTan(*this));}
  boost::shared_ptr<TBaseFuncType> MakeDifFunc() {throw Exception("Tangent cannot be differentiated");}
  bool IsValid() const; //Indicates the parent function is valid at t
  void GetCurrentRange(double &Min, double &Max, double &ds) const;
  const TTextValue& GetSteps() const;
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  const Func32::TParamFunc& GetFunc() const {return TanFunc;}
  void Update();
  long double CalcArea(long double From, long double To) const;
  bool CalcTan();
  bool IsDependent(const std::wstring &SymbolName) const;
};

class TStdFunc : public TBaseFuncType
{
  std::wstring Text;
  Func32::TFunc Func;

public:
  TStdFunc() {} //Used with ReadFromIni();
  TStdFunc(const std::wstring &AText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig);
  explicit TStdFunc(const Func32::TFunc &AFunc);

  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TStdFunc(*this));}
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  boost::shared_ptr<TBaseFuncType> MakeDifFunc();
  void GetCurrentRange(double &Min, double &Max, double &ds) const;
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  std::wstring GetVariable() const {return L"x";}
  const Func32::TFunc& GetFunc() const {return Func;}
  const std::wstring& GetText() const {return Text;}
};

class TParFunc : public TBaseFuncType
{
  std::wstring xText,yText;
  Func32::TParamFunc Func;

public:
  TParFunc() {}
  TParFunc(const std::wstring &AxText, const std::wstring &AyText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig);
  TParFunc(const Func32::TParamFunc &AFunc);

  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TParFunc(*this));}
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  boost::shared_ptr<TBaseFuncType> MakeDifFunc();
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  std::wstring GetVariable() const {return L"t";}
  const Func32::TParamFunc& GetFunc() const {return Func;}
  const std::wstring& GetxText() const {return xText;}
  const std::wstring& GetyText() const {return yText;}
};

class TPolFunc : public TBaseFuncType
{
  std::wstring Text;
  Func32::TPolarFunc Func;

public:
  TPolFunc() {}
  TPolFunc(const std::wstring &AText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig);

  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TPolFunc(*this));}
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  boost::shared_ptr<TBaseFuncType> MakeDifFunc();
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  std::wstring GetVariable() const {return L"t";}
  const Func32::TPolarFunc& GetFunc() const {return Func;}
  const std::wstring& GetText() const {return Text;}
};

enum TErrorBarType {ebtNone, ebtFixed, ebtRelative, ebtCustom};
struct TPointSeriesPoint
{
	std::wstring First; //X or angle
  std::wstring Second; //Y or r
  TTextValue xError, yError; //Data for error bars; only used if Uncertainty=utCustom

  TPointSeriesPoint() {}
  TPointSeriesPoint(const std::wstring &AFirst, const std::wstring &ASecond, const TTextValue &XError, const TTextValue &YError)
    : First(AFirst), Second(ASecond), xError(XError), yError(YError) {}
  TPointSeriesPoint(const std::wstring &AFirst, const std::wstring &ASecond)
    : First(AFirst), Second(ASecond) {}
};

enum TInterpolationAlgorithm {iaLinear, iaCubicSpline, iaHalfCosine, iaCubicSpline2};
enum TLabelPosition {lpAbove, lpBelow, lpLeft, lpRight, lpAboveLeft, lpAboveRight, lpBelowLeft, lpBelowRight};
enum TPointType {ptCartesian, ptPolar};

class TPointSeries : public TGraphElem
{
public:
  typedef std::vector<TPointSeriesPoint> TPointData;
  typedef std::vector<Func32::TDblPoint> TPointList;
private:
  TColor FrameColor;
  TColor FillColor;
  TColor LineColor;
  unsigned Size;
  unsigned Style; //Marker type
  unsigned LineSize;
  TPenStyle LineStyle;
  TInterpolationAlgorithm Interpolation;
  bool ShowLabels;
  TLabelPosition LabelPosition;
  TVclObject<TFont> Font;
  TPointData PointData;
  TPointList PointList;
  TErrorBarType xErrorBarType, yErrorBarType;
  double xErrorValue, yErrorValue; //Data for error bars; only used if Uncertainty!=utCustom
  TPointType PointType;

  Func32::TDblPoint ConvertPoint(const TPointSeriesPoint &P) const;
public:
  TPointSeries(TColor AFrameColor=clBlack, TColor AFillColor=clRed, TColor ALineColor=clRed,
    unsigned ASize=1, unsigned ALineSize=1, unsigned AStyle=0, TPenStyle ALineStyle=psSolid,
    TInterpolationAlgorithm AInterpolation=iaLinear, bool AShowLabels=false, TFont *AFont=NULL,
		TLabelPosition ALabelPosition=lpBelow, TPointType APointType=ptCartesian,
    TErrorBarType XErrorBarType=ebtNone, double XErrorValue=0, TErrorBarType YErrorBarType=ebtNone, double YErrorValue=0);
  std::wstring MakeText() const {return GetLegendText();}
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TPointSeries(*this));}
  TPointList::const_iterator FindPoint(double x) const;

	void InsertDblPoint(const Func32::TDblPoint &Point, int Index=-1);
	void InsertPoint(const TPointSeriesPoint &Point, int Index=-1, bool AutoUpdate=true);
  void ReplaceDblPoint(const Func32::TDblPoint &Point, unsigned Index);
  void ReplacePoint(const TPointSeriesPoint &Point, unsigned Index);
  void DeletePoint(unsigned Index, unsigned Count=1);
  const Func32::TDblPoint& GetDblPoint(int Index) const {return PointList.at(Index);}
  const TPointSeriesPoint& GetPoint(unsigned Index) const {return PointData.at(Index);}
  unsigned PointCount() const {return PointData.size();}
  const std::vector<TPointSeriesPoint>& GetPointData() const {return PointData;}
  const TPointList& GetPointList() const {return PointList;}

  double GetXError(unsigned Index) const;
  double GetYError(unsigned Index) const;
  void Assign(const TPointData &APointData) {PointData = APointData;}
  void Swap(TPointData &APointData) {PointData.swap(APointData);}
  void Update();

  TErrorBarType GetxErrorBarType() const {return xErrorBarType;}
  void SetxErrorBarType(TErrorBarType Value) {xErrorBarType = Value;}
  TErrorBarType GetyErrorBarType() const {return yErrorBarType;}
  void SetyErrorBarType(TErrorBarType Value) {yErrorBarType = Value;}
	double GetxErrorValue() const {return xErrorValue;}
  void SetxErrorValue(double Value) {xErrorValue = Value;}
  double GetyErrorValue() const {return yErrorValue;}
  void SetyErrorValue(double Value) {yErrorValue = Value;}
  TColor GetFillColor() const {return FillColor;}
  void SetFillColor(TColor Value) {FillColor = Value;}
  unsigned GetSize() const {return Size;}
  void SetSize(unsigned Value) {Size = Value;}
  unsigned GetStyle() const {return Style;}
  void SetStyle(unsigned Value) {Style = Value;}
  TColor GetLineColor() const {return LineColor;}
  void SetLineColor(TColor Value) {LineColor = Value;}
  TColor GetFrameColor() const {return FrameColor;}
  void SetFrameColor(TColor Value) {FrameColor = Value;}
  unsigned GetLineSize() const {return LineSize;}
  void SetLineSize(unsigned Value) {LineSize = Value;}
  TPenStyle GetLineStyle() const {return LineStyle;}
  void SetLineStyle(TPenStyle Value) {LineStyle = Value;}
  TInterpolationAlgorithm GetInterpolation() const {return Interpolation;}
  void SetInterpolation(TInterpolationAlgorithm Value) {Interpolation = Value;}
  bool GetShowLabels() const {return ShowLabels;}
  void SetShowLabels(bool Value) {ShowLabels = Value;}
  TFont* GetFont() const {return Font;}
  TLabelPosition GetLabelPosition() const {return LabelPosition;}
  void SetLabelPosition(TLabelPosition Value) {LabelPosition = Value;}
  TPointType GetPointType() const {return PointType;}
  void SetPointType(TPointType Value) {PointType = Value;}
  bool IsDependent(const std::wstring &SymbolName) const;
};

Func32::TDblPoint FindCoord(TPointSeries::TPointList::const_iterator Iter, double x);
typedef boost::shared_ptr<TPointSeries> TPointSeriesPtr;

enum TShadeStyle {ssAbove, ssBelow, ssXAxis, ssYAxis, ssBetween, ssInside};

class TShading : public TGraphElem
{
public:
  boost::shared_ptr<class TRegion> Region;
  TShadeStyle ShadeStyle;
  TBrushStyle BrushStyle;
  TColor Color;
  boost::shared_ptr<TBaseFuncType> Func2;
  TTextValue sMin;
  TTextValue sMax;
  TTextValue sMin2;
  TTextValue sMax2;
  bool ExtendMinToIntercept;
  bool ExtendMaxToIntercept;
  bool ExtendMin2ToIntercept;
  bool ExtendMax2ToIntercept;
  bool MarkBorder;

  TShading();
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  TGraphElemPtr Clone() const;
  void Update();
  void ClearCache();
  bool IsDependent(const std::wstring &SymbolName) const;
};
typedef boost::shared_ptr<TShading> TShadingPtr;

enum TRelationType {rtEquation, rtInequality};
class TRelation : public TGraphElem
{
  std::wstring Text;
  std::wstring ConstraintsText;
  Func32::TCustomFunc Func;
  Func32::TCustomFunc Constraints;
  TColor Color;
  unsigned Size;
  TBrushStyle BrushStyle;
  TRelationType RelationType;

public:
  boost::shared_ptr<class TRegion> Region;
  boost::shared_ptr<class TRegion> BoundingRegion; //Used to draw the frame around Region for inequalities

  TRelation();
  TRelation(const std::wstring &AText, const std::wstring &AConstraints, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig);
  TRelation(const TRelation &Relation);
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TRelation(*this));}
  void SetConstraints(const std::wstring &AConstraintsText, const Func32::TSymbolList &SymbolList);
  void Update();

  TColor GetColor() const {return Color;}
  void SetColor(TColor Value) {Color = Value;}
  TBrushStyle GetBrushStyle() const {return BrushStyle;}
  void SetBrushStyle(TBrushStyle Value) {BrushStyle = Value;}
  const std::wstring& GetText() const {return Text;}
  const std::wstring& GetConstraints() const {return ConstraintsText;}
  long double Eval(const std::vector<long double> &Args, Func32::ECalcError &E);
  long double Eval(long double x, long double y);
  void ClearCache();
  TRelationType GetRelationType() const {return RelationType;}
  unsigned GetSize() const {return Size;}
  void SetSize(unsigned Value) {Size = Value;}
  bool IsDependent(const std::wstring &SymbolName) const;
};

class TAxesView : public TGraphElem
{
public:
  TAxesView() {SetShowInLegend(false);}
  std::wstring MakeText() const;
  void WriteToIni(TConfigFileSection &Section) const;
  void ReadFromIni(const TConfigFileSection &Section);
  void Accept(TGraphElemVisitor &v) {v.Visit(*this);}
  boost::shared_ptr<TGraphElem> Clone() const {return CloneHelper(new TAxesView(*this));}
  int GetVisible() const;
  void ChangeVisible();
  const TAxes& GetAxes() const;
  bool IsDependent(const std::wstring &SymbolName) const;
};
} //namespace Graph
using namespace Graph;
//---------------------------------------------------------------------------
#endif
