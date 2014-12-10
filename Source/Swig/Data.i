%module Data
#define SWIG_NO_EXPORT_ITERATOR_METHODS

%include "stl.i"
%include "std_string.i"
%include "std_wstring.i"
%include "boost_shared_ptr.i"
%include "attribute.i"

%pythonnondynamic;

%shared_ptr(TGraphElem)
%shared_ptr(TTopGraphElem)
%shared_ptr(TBaseFuncType)
%shared_ptr(TStdFunc)
%shared_ptr(TParFunc)
%shared_ptr(TPolFunc)
%shared_ptr(TTan)
%shared_ptr(TPointSeries)
%shared_ptr(TTextLabel)
%shared_ptr(TShading)
%shared_ptr(TRelation)
%shared_ptr(TAxesView)

%include "Types.i"

%template(PointVector) std::vector<TPoint>;
%template(CoordSetVector) std::vector<Func32::TCoordSet<> >;
%template(UnsignedVector) std::vector<unsigned>;
%template(StringMap) std::map<std::wstring,std::wstring>;

%begin %{
#include "Graph.h"
#include "Unit1.h"
#include "Python.hpp"
#define WRAP_PYOBJECTS
#include "PythonBind.h"
#include "PyGraph.h"
#include "PythonBind.h"
#include "PyVclObject.h"
#pragma warn -8060
#pragma warn -8070
%}

%define CHECK_GRAPH_ELEM(TType)
  if(dynamic_cast<TType*>(Ptr))
    return SWIG_NewPointerObj(new boost::shared_ptr<TType>(boost::static_pointer_cast<TType>(Elem)), SWIGTYPE_p_boost__shared_ptrT_##TType##_t, SWIG_POINTER_OWN |  0 );
%enddef

%header {
PyObject* DownCastSharedPtr(const boost::shared_ptr<TGraphElem> &Elem)
{
  TGraphElem *Ptr = Elem.get();
  if(Ptr == NULL)
    return SWIG_Py_Void();
  CHECK_GRAPH_ELEM(TStdFunc)
  CHECK_GRAPH_ELEM(TParFunc)
  CHECK_GRAPH_ELEM(TPolFunc)
  CHECK_GRAPH_ELEM(TTan)
  CHECK_GRAPH_ELEM(TPointSeries)
  CHECK_GRAPH_ELEM(TTextLabel)
  CHECK_GRAPH_ELEM(TShading)
  CHECK_GRAPH_ELEM(TRelation)
  CHECK_GRAPH_ELEM(TAxesView)
  CHECK_GRAPH_ELEM(TTopGraphElem)
  return SWIG_NewPointerObj(new boost::shared_ptr< TGraphElem >(Elem), SWIGTYPE_p_boost__shared_ptrT_TGraphElem_t, SWIG_POINTER_OWN |  0 );
}
}

HANDLE_FPU(Redraw)
HANDLE_FPU(InsertDblPoint)
HANDLE_FPU(ReplaceDblPoint)
HANDLE_FPU(TStdFunc::TStdFunc)
HANDLE_FPU(TParFunc::TParFunc)
HANDLE_FPU(TPolFunc::TPolFunc)

typedef boost::shared_ptr<class TGraphElem> TGraphElemPtr;

%inline %{
static void AbortUpdate() {Form1->Data.AbortUpdate();}
static void Redraw() {Form1->Redraw();}
static TGraphElemPtr GetSelected() {return Form1->GetSelected();}
static void SetSelected(const TGraphElemPtr &Elem) {Form1->SetSelected(Elem);}

static unsigned ChildCount(const TGraphElemPtr &Elem) {return Elem->ChildCount();}
static TGraphElemPtr GetChild(const TGraphElemPtr &Elem, unsigned Index) {return Elem->GetChild(Index);}
static void RemoveChild(const TGraphElemPtr &Elem, unsigned Index)
{
  UndoList.Push(TUndoDel(Elem->GetChild(Index), Elem, Index));
  Elem->RemoveChild(Index);
  Form1->UpdateTreeView();
}

static void InsertChild(const TGraphElemPtr &Elem, const TGraphElemPtr &Child, int Index)
{
  UndoList.BeginMultiUndo();
  TGraphElemPtr Parent = Child->GetParent();
  if(Parent)
    UndoList.Push(TUndoDel(Child, Parent, Parent->GetChildIndex(Child)));
  Elem->InsertChild(Child, Index);
  UndoList.Push(TUndoAdd(Child));
  UndoList.EndMultiUndo();
  Form1->UpdateTreeView();
}

static void ReplaceChild(const TGraphElemPtr &Elem, unsigned Index, const TGraphElemPtr &Child)
{
  UndoList.Push(TUndoChange(Elem->GetChild(Index), Child));
  Elem->ReplaceChild(Index, Child);
  Form1->UpdateTreeView();
}

static bool CompareElem(const TGraphElemPtr &E1, const TGraphElemPtr &E2) {return E1.get() == E2.get();}
static std::map<std::wstring,std::wstring>& GetPluginData() {return Form1->Data.PluginData;}
static const boost::shared_ptr<TTopGraphElem>& GetTopElem() {return Form1->Data.GetTopElem();}
%}

//This must be placed after the %shared_ptr macro
%typemap(out) boost::shared_ptr<TBaseFuncType>
{
  $result = DownCastSharedPtr($1);
}

%nodefaultctor TGraphElem;
%attribute(TGraphElem, int, Visible, GetVisible, SetVisible);
%attribute(TGraphElem, bool, ShowInLegend, GetShowInLegend, SetShowInLegend);
%attributestring(TGraphElem, std::wstring, LegendText, GetLegendText, SetLegendText);
%attributestring(TGraphElem, TGraphElemPtr, Parent, GetParent);
%rename(_PluginData) PluginData;
class TGraphElem
{
public:
  std::map<std::wstring,std::wstring> PluginData;
  std::wstring MakeLegendText() const;
  std::wstring MakeText() const;
  TGraphElemPtr Clone();
};

%extend TGraphElem
{
  int ThisPtr() const {return reinterpret_cast<int>(self);}
  %pythoncode %{
    def __eq__(self, rhs):
      if not isinstance(rhs, TGraphElem): return False
      return self.ThisPtr() == rhs.ThisPtr()
  %}
}


enum TDrawType {dtAuto, dtDots, dtLines};
typedef unsigned TPenStyle;

%nodefaultctor TBaseFuncType;
//%attributeval(TBaseFuncType, %arg(std::pair<double,double>), CurrentRange, GetCurrentRange);
%attributestring(TBaseFuncType, TTextValue, Steps, GetSteps, SetSteps);
%attributestring(TBaseFuncType, std::wstring, Variable, GetVariable);
class TBaseFuncType : public TGraphElem
{
public:
  const std::vector<Func32::TCoordSet<> > sList;
  const std::vector<TPoint> Points;
  const std::vector<unsigned> PointNum;
  TColor Color;
  unsigned Size;
  TPenStyle Style;
  TTextValue From, To;
  unsigned StartPointStyle, EndPointStyle;
  TDrawType DrawType;

  boost::shared_ptr<TBaseFuncType> MakeDifFunc();
  Func32::TCoord<long double> Eval(long double t) const throw(Func32::EFuncError);
  long double CalcArea(long double From, long double To) const throw(Func32::EFuncError);
};

%nodefaultctor TTopGraphElem;
class TTopGraphElem : public TGraphElem
{
};

%attributestring(TStdFunc, std::wstring, Text, GetText);
class TStdFunc : public TBaseFuncType
{
};

%extend TStdFunc {
  TStdFunc(const std::wstring &Str) throw(Func32::EFuncError)
  {
    return new TStdFunc(Str, Form1->Data.CustomFunctions.SymbolList, Form1->Data.Axes.Trigonometry);
  }
}

%nodefaultctor TParFunc;
%attributestring(TParFunc, std::wstring, xText, GetxText);
%attributestring(TParFunc, std::wstring, yText, GetyText);
class TParFunc : public TBaseFuncType
{
};

%extend TParFunc {
  TParFunc(const std::wstring &xStr, const std::wstring &yStr) throw(Func32::EFuncError)
  {
    return new TParFunc(xStr, yStr, Form1->Data.CustomFunctions.SymbolList, Form1->Data.Axes.Trigonometry);
  }
}

%nodefaultctor TPolFunc;
%attributestring(TPolFunc, std::wstring, Text, GetText);
class TPolFunc : public TBaseFuncType
{
};

%extend TPolFunc {
  TPolFunc(const std::wstring &Str) throw(Func32::EFuncError)
  {
    return new TPolFunc(Str, Form1->Data.CustomFunctions.SymbolList, Form1->Data.Axes.Trigonometry);
  }
}

enum TTangentType {ttTangent, ttNormal};
%attribute(TTan, bool, Valid, IsValid);
class TTan : public TBaseFuncType
{
public:
  TTextValue t;
  TTangentType TangentType;
};

enum TErrorBarType {ebtNone, ebtFixed, ebtRelative, ebtCustom};
enum TInterpolationAlgorithm {iaLinear, iaCubicSpline, iaHalfCosine, iaCubicSpline2};
enum TPointType {ptCartesian, ptPolar};
namespace Graph
{
enum TLabelPosition {lpAbove, lpBelow, lpLeft, lpRight, lpAboveLeft, lpAboveRight, lpBelowLeft, lpBelowRight};
}

HANDLE_FPU(TPointSeries::GetDblPoint)

%attribute(TPointSeries, TErrorBarType, xErrorBarType, GetxErrorBarType, SetxErrorBarType);
%attribute(TPointSeries, TErrorBarType, yErrorBarType, GetyErrorBarType, SetyErrorBarType);
%attribute(TPointSeries, double, xErrorValue, GetxErrorValue, SetxErrorValue);
%attribute(TPointSeries, double, yErrorValue, GetyErrorValue, SetyErrorValue);
%attribute(TPointSeries, TColor, FillColor, GetFillColor, SetFillColor);
%attribute(TPointSeries, TColor, LineColor, GetLineColor, SetLineColor);
%attribute(TPointSeries, TColor, FrameColor, GetFrameColor, SetFrameColor);
%attribute(TPointSeries, unsigned, Size, GetSize, SetSize);
%attribute(TPointSeries, unsigned, Style, GetStyle, SetStyle);
%attribute(TPointSeries, unsigned, LineSize, GetLineSize, SetLineSize);
%attribute(TPointSeries, TPenStyle, LineStyle, GetLineStyle, SetLineStyle);
%attribute(TPointSeries, TInterpolationAlgorithm, Interpolation, GetInterpolation, SetInterpolation);
%attribute(TPointSeries, bool, ShowLabels, GetShowLabels, SetShowLabels);
%attribute(TPointSeries, TFont*, Font, GetFont);
%attribute(TPointSeries, Graph::TLabelPosition, LabelPosition, GetLabelPosition, SetLabelPosition);
%attribute(TPointSeries, TPointType, PointType, GetPointType, SetPointType);
class TPointSeries : public TGraphElem
{
public:
  void InsertDblPoint(Func32::TDblPoint Point, int Index) throw(std::out_of_range);
  void InsertPoint(TPointSeriesPoint Point, int Index) throw(std::out_of_range);
  void ReplaceDblPoint(Func32::TDblPoint Point, unsigned Index) throw(std::out_of_range);
  void ReplacePoint(TPointSeriesPoint Point, unsigned Index) throw(std::out_of_range);
  void DeletePoint(unsigned Index, unsigned Count=1) throw(std::out_of_range);
  const Func32::TDblPoint& GetDblPoint(int Index) const throw(std::out_of_range);
  const TPointSeriesPoint& GetPoint(unsigned Index) const throw(std::out_of_range);
  unsigned PointCount() const;
};

enum TLabelPlacement {lpUserTopLeft, lpAboveX, lpBelowX, lpLeftOfY, lpRightOfY, lpUserTopRight, lpUserBottomLeft, lpUserBottomRight};

%attribute(TTextLabel, TRect, Rect, GetRect);
%attribute(TTextLabel, TTextValue, xPos, GetXPos, SetXPos);
%attribute(TTextLabel, TTextValue, yPos, GetYPos, SetYPos);
%attributestring(TTextLabel, std::string, Text, GetText, SetText);
%attribute(TTextLabel, TColor, BackgroundColor, GetBackgroundColor, SetBackgroundColor);
%attribute(TTextLabel, TLabelPlacement, Placement, GetPlacement, SetPlacement);
%attribute(TTextLabel, unsigned, Rotation, GetRotation, SetRotation);
class TTextLabel : public TGraphElem
{
public:
  void Scale(double xSizeMul, double ySizeMul);
  TMetafile* GetImage() const {return Metafile;}
};

enum TShadeStyle {ssAbove, ssBelow, ssXAxis, ssYAxis, ssBetween, ssInside};
class TShading : public TGraphElem
{
public:
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
};

enum TRelationType {rtEquation, rtInequality};

%attribute(TRelation, TColor, Color, GetColor, SetColor);
%attribute(TRelation, TBrushStyle, BrushStyle, GetBrushStyle, SetBrushStyle);
%attribute(TRelation, TRelationType, RelationType, GetRelationType);
%attributestring(TRelation, std::wstring, Text, GetText);
%attributestring(TRelation, std::wstring, Constraints, GetConstraints);
%attribute(TRelation, unsigned, Size, GetSize, SetSize);
class TRelation : public TGraphElem
{
public:
  long double Eval(long double x, long double y) throw(Func32::EFuncError);
};

%extend TRelation {
  TRelation(const std::wstring &Str, const std::wstring &ConstraintsStr) throw(Func32::EFuncError)
  {
    return new TRelation(Str, ConstraintsStr, Form1->Data.CustomFunctions.SymbolList, Form1->Data.Axes.Trigonometry);
  }
  TRelation(const std::wstring &Str) throw(Func32::EFuncError)
  {
    return new TRelation(Str, L"", Form1->Data.CustomFunctions.SymbolList, Form1->Data.Axes.Trigonometry);
  }
}

%nodefaultctor TAxesView;
class TAxesView : public TGraphElem
{
};

%nodefaultctor TData;
%nodefaultdtor TData;
struct TData
{
  const TAxes Axes;
};

%{
  PyObject* ToPyObject(TData &Data)
  {
    Python::TLockGIL Dummy;
    return Python::IsPythonInstalled() ? SWIG_NewPointerObj(SWIG_as_voidptr(&Data), SWIGTYPE_p_TData, 0) : NULL;
  }
%}

%pythoncode
{
  import vcl
  def GraphElemRepr(self):
    return '%s("%s")' % (self.__class__.__name__, self.MakeText())
  TStdFunc.__repr__ = GraphElemRepr
  TParFunc.__repr__ = GraphElemRepr
  TPolFunc.__repr__ = GraphElemRepr
  TTan.__repr__ = GraphElemRepr
  TPointSeries.__repr__ = GraphElemRepr
  TRelation.__repr__ = GraphElemRepr
  TTextLabel.__repr__ = GraphElemRepr
  TShading.__repr__ = GraphElemRepr
  TAxesView.__repr__ = GraphElemRepr
  TTopGraphElem.__repr__ = GraphElemRepr
  def SetPoints(self, L):
    while len(self.Points) > 0: del self.Points[0]
    for n in L: self.Points.append(n)
  TPointSeries.__swig_setmethods__["Points"] = SetPoints
  TPointSeries.__swig_getmethods__["Points"] = lambda self: TPointDataList(self)
  TPointSeries.__swig_getmethods__["PointData"] = lambda self: TPointList(self)

  import collections
  class TPointList(collections.MutableSequence):
    def __init__(self, PointSeries):
      self.PointSeries = PointSeries
    def __getitem__(self, key):
      return self.PointSeries.GetPoint(key)
    def __len__(self):
      return self.PointSeries.PointCount()
    def insert(self, key, value):
      self.PointSeries.InsertPoint(value, key)
    def __setitem__(self, key, value):
      self.PointSeries.ReplacePoint(value, key)
    def append(self, value):
      self.PointSeries.InsertPoint(value, -1)
    def __delitem__(self, key):
      self.PointSeries.DeletePoint(key)
    def __repr__(self):
      return repr(list(self))

  class TPointDataList(collections.MutableSequence):
    def UnpackSlice(s):
      count = self.PointSeries.PointCount()
      start = 0 if key.start is None else (max(count+key.start, 0) if key.start < 0 else key.start)
      stop = count if key.stop is None else (max(count+key.stop, 0) if key.stop < 0 else min(key.stop, count))

    def __init__(self, PointSeries):
      self.PointSeries = PointSeries
    def __getitem__(self, key):
      if isinstance(key, slice):
        count = self.PointSeries.PointCount()
        start = 0 if key.start is None else (max(count+key.start,0) if key.start < 0 else key.start)
        stop = count if key.stop is None else (max(count+key.stop,0) if key.stop < 0 else min(key.stop, count))
        step = 1 if key.step is None else key.step
        return [self.PointSeries.GetDblPoint(i) for i in range(start, stop, step)]
      return self.PointSeries.GetDblPoint(key)
    def __len__(self):
      return self.PointSeries.PointCount()
    def insert(self, key, value):
      self.PointSeries.InsertDblPoint(value, key)
    def __setitem__(self, key, value):
      if isinstance(key, slice):
        count = self.PointSeries.PointCount()
        start = 0 if key.start is None else (count+key.start if key.start < 0 else key.start)
        stop = count if key.stop is None else (max(count+key.stop, 0) if key.stop < 0 else min(key.stop, count))
        if key.step is None or key.step == 1:
          self.PointSeries.DeletePoint(start, stop - start)
          for x in zip(value, range(start, start + len(value))):
            self.PointSeries.InsertDblPoint(x[0], x[1])
        elif len(range(start, stop, key.step)) == len(value):
          for x in zip(value, range(start, stop, key.step)):
            self.PointSeries.ReplaceDblPoint(x[0], x[1])
        else:
          raise ValueError("attempt to assign sequence of size %d to extended slice of size %d" % (len(value), len(range(start, stop, key.step))))
      else:
        self.PointSeries.ReplaceDblPoint(value, key)
    def append(self, value):
      self.PointSeries.InsertDblPoint(value, -1)
    def __delitem__(self, key):
      if isinstance(key, slice):
        count = self.PointSeries.PointCount()
        start = 0 if key.start is None else (count+key.start if key.start < 0 else key.start)
        stop = count if key.stop is None else (count+key.stop if key.stop < 0 else key.stop)
        if key.step is None or key.step == 1:
          self.PointSeries.DeletePoint(key.start, key.stop - key.start)
        else:
          for i in reversed(range(start, stop, key.step)):
            self.PointSeries.DeletePoint(i)
      else:
        self.PointSeries.DeletePoint(key)
    def __repr__(self):
      return repr(list(self))
    def __eq__(self, other):
      if self is other:
        return True
      if len(other) !=  self.PointSeries.PointCount():
        return False
      for i in range(len(other)):
        if other[i] != self.PointSeries.GetDblPoint(i):
          return False
      return True
    def __ne__(self, other):
      return not self == other
}


