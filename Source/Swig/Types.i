%typemap(in) TPoint  {
  if(!PyArg_ParseTuple($input, "ii", &($1.x), &($1.y)))
    SWIG_fail;
}

%typemap(out) TPoint {
  $result = Py_BuildValue("ii", $1.x, $1.y);
}

%typemap(in) const TPoint& {
  if(!PyArg_ParseTuple($input, "ii", &($1->x), &($1->y)))
    SWIG_fail;
}

%typemap(out) const TPoint& {
  $result = Py_BuildValue("ii", $1->x, $1->y);
}

%typemap(out) const TRect& {
  $result = Py_BuildValue("iiii", $1->Left, $1->Top, $1->Right, $1->Bottom);
}

%typemap(out) TRect {
  $result = Py_BuildValue("iiii", $1.Left, $1.Top, $1.Right, $1.Bottom);
}

%typemap(in) TTextValue  {
  $1 = TTextValue(PyFloat_AsDouble($input));
  if(PyErr_Occurred() != NULL)
    SWIG_fail;
}

%typemap(out) TTextValue {
  $result = PyFloat_FromDouble($1.Value);
}

%typemap(out) TFont* {
  $result = Python::VclObject_Create($1, false);
}

%typemap(out) boost::shared_ptr<TGraphElem>
{
  $result = DownCastSharedPtr($1);
}

%typemap(out) const boost::shared_ptr<TGraphElem>&
{
  $result = DownCastSharedPtr(*$1);
}
//%typemap(out) boost::shared_ptr<TBaseFuncType> = boost::shared_ptr<TGraphElem>;
%typemap(out) const boost::shared_ptr<TBaseFuncType>& = const boost::shared_ptr<TGraphElem>&;

%typemap(out) Func32::TCoord<long double> {
  $result = Py_BuildValue("dd", (double)$1.x, (double)$1.y);
}

%typemap(out) const Func32::TCoordSet& {
  $result = Py_BuildValue("ddd", (double)$1->t, (double)$1->x, (double)$1->y);
}

%typemap(out) std::pair<double,double>* {
  $result = Py_BuildValue("dd", $1->first, $1->second);
}

%define TUPLE(StructName, V...)
%typemap(out) StructName
{
  $1_type *p = &$1;
  $result = Python::CreateTuple(V);
}

%typemap(out) const StructName&
{
  $*1_ltype *p = $1;
  $result = Python::CreateTuple(V);
}

%typemap(in) StructName
{
  $1_type *p = &$1;
  if(!Python::FromTuple($input, V))
      SWIG_fail;
}

%typemap(in) const StructName&
{
  $*1_ltype *p = $1;
  if(!Python::FromTuple($input, V))
      SWIG_fail;
}
%enddef

%typemap(throws) Func32::EFuncError %{
  PyErr_SetString(Python::PyEFuncError, ToString(GetErrorMsg($1)).c_str());
  SWIG_fail;
%}

%define HANDLE_FPU(function)
%exception function
%{
  _clear87(); //Clear FPU status flags
  _control87(DEFAULT_FPU_CONTROL, FPU_MASK);   //Reset FPU exception state to the previous
  $action
  _control87(PYTHON_FPU_CONTROL, FPU_MASK); //Set the FPU Control Word to what Python expects
%}
%enddef

TUPLE(TPointSeriesPoint, p->First, p->Second, p->xError.Text, p->yError.Text)
TUPLE(Func32::TDblPoint, p->x, p->y)
TUPLE(TDefaultData, p->Style, p->Color, p->Size)

%apply double {long double};
typedef unsigned TColor;
typedef unsigned TBrushStyle;

