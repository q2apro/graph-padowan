/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "ComCtrls.hpp"
#include "IGraphic.h"
#include "IRichEdit.h"
#include "RichEditOle.h"
#include <fstream>
#include <algorithm>
//---------------------------------------------------------------------------
void ReplaceExpression(TIRichEdit *RichEdit, const TData &Data)
{
  std::wstring Str = ToWString(RichEdit->Text);
  for(int I = Str.size()-1; I >=0; I--)
    if(Str[I] == '\r')
      Str.erase(I, 1);

  size_t Pos = std::wstring::npos;
  while((Pos = Str.rfind(L"%(", Pos-1)) != std::wstring::npos)
  {
    size_t Pos2 = FindEndPar(Str, Pos);
    if(Pos2 == std::string::npos)
      return;
    unsigned Length = Pos2 - Pos + 1;

    try
		{
      std::wstring Expression = Str.substr(Pos+2, Pos2-Pos-2);
      bool UseReal = Property.ComplexFormat == cfReal;
      String Value = ComplexToString(UseReal ? Func32::TComplex(Data.Calc(Expression)) : Data.CalcComplex(Expression));

      //If %() is preceded by a '+' and the value is negative, the '+' will be removed to avoid such as "2x+-3"
      if(Value[1] == '-' && Pos > 0 && Str[Pos - 1] == '+')
      {
        Pos--;
        Length++;
      }

      RichEdit->SelStart = Pos;
      RichEdit->SelLength = Length;
      RichEdit->SelText = Value;                 
    }
    catch(Func32::EFuncError &Error)
    {
      String ErrorMessage = GetErrorMsg(Error);
      RichEdit->SelStart = Pos;
      RichEdit->SelLength = Length;
      RichEdit->SelText = ErrorMessage;

      RichEdit->SelStart = Pos;
      RichEdit->SelLength = ErrorMessage.Length();
      RichEdit->TextFormat.SetBold(true);
      RichEdit->TextFormat.SetColor(clRed);
    }

		if(Pos == 0)
      break; //Special case
  }
}
//---------------------------------------------------------------------------
void RenderRichText(const String &Str, TCanvas *Canvas, const TPoint &Pos, int Width, TColor BackgroundColor, const TData *Data)
{
  //We need a parent window; just use main form
  std::auto_ptr<TIRichEdit> RichEdit(new TIRichEdit(Application->MainForm));
  RichEdit->Visible = false;
  RichEdit->Transparent = BackgroundColor == clNone;
  RichEdit->Parent = Application->MainForm;
  RichEdit->WrapType = wtNone;
  RichEdit->BackgroundColor = BackgroundColor;
  TRichEditOle RichEditOle(RichEdit.get());

  RichEdit->SetRichText(Str);
  if(Data)
    ReplaceExpression(RichEdit.get(), *Data);
  RichEdit->Render(Canvas, Pos, Width);
}
//---------------------------------------------------------------------------
String UpdateRichText(const String &Str)
{
  //We need a parent window; just use main form
  std::auto_ptr<TIRichEdit> RichEdit(new TIRichEdit(Application->MainForm));
  RichEdit->Visible = false;
  RichEdit->Parent = Application->MainForm;
  TRichEditOle RichEditOle(RichEdit.get());
  RichEdit->SetRichText(Str);
  RichEditOle.UpdateAll();
  return RichEdit->GetRichText();
}
//---------------------------------------------------------------------------
TPoint RichTextSize(const std::string &Str, const TData *Data)
{
  //We need a parent window; just use main form
  std::auto_ptr<TIRichEdit> RichEdit(new TIRichEdit(Application->MainForm));
  RichEdit->Visible = false;
  RichEdit->Transparent = true;
  RichEdit->WrapType = wtNone; //Must be false for LineIndex() to find the correct line
	RichEdit->ProtectedChange = true;

  RichEdit->Parent = Application->MainForm;
  TRichEditOle RichEditOle(RichEdit.get());
  RichEdit->SetRichText(Str.c_str());
  if(Data)
		ReplaceExpression(RichEdit.get(), *Data);

	//We have to disable Beep while setting SelText below;
	//Else the system beeps if the Rich Edit cannot be changed , for example because
	//the Rich Edit contains a table
	BOOL BeepOn;
	SystemParametersInfo(SPI_GETBEEP, 0, &BeepOn, 0);
	SystemParametersInfo(SPI_SETBEEP, false, NULL, 0);

	//We need to add a space to the end of each lines. Else the width will be too
	//small if the line ends with an italic character
	int LineCount = RichEdit->LineCount();
	for(int I = 0; I < LineCount; I++)
	{
		int Start = RichEdit->LineIndex(I);
		RichEdit->SelStart = Start + RichEdit->LineLength(Start);
		RichEdit->SelText = L" ";
	}

  //Set Beep back to the previous value
	SystemParametersInfo(SPI_SETBEEP, BeepOn, NULL, 0);

	//We need to add an empty line to the end, because GetTextSize doesn't count
  //the last line in the height
	RichEdit->SelText = "\n";

	//We need to left align the text because there is no specific right side
	RichEdit->SelectAll();
	RichEdit->Paragraph->Alignment = pfaLeft;

	return RichEdit->GetTextSize();
}
//---------------------------------------------------------------------------
/*struct TSplineParam
{
	TPoint Param[4];
};

TSplineParam CalcSingleSpline(TPoint p1, TPoint p2, TPoint p3, TPoint p4)
{
	TSplineParam Spline;
	Spline.Param[0].x = -p1.x + 3*p2.x -3*p3.x + p4.x;
	Spline.Param[1].x = 2*p1.x -5*p2.x + 4*p3.x - p4.x;
	Spline.Param[2].x = -p1.x+p3.x;
	Spline.Param[3].x = 2*p2.x;
	Spline.Param[0].y = -p1.y + 3*p2.y -3*p3.y + p4.y;
	Spline.Param[1].y = 2*p1.y -5*p2.y + 4*p3.y - p4.y;
	Spline.Param[2].y = -p1.y+p3.y;
	Spline.Param[3].y = 2*p2.y;
	return Spline;
}
//---------------------------------------------------------------------------
//Calculates points used to draw a cubic spline
//The spline will go from p2 to p3; p1 and p4 are used as control points
void CreateSingleSpline(std::vector<TPoint> &Points, TPoint p1, TPoint p2, TPoint p3, TPoint p4)
{
	//If the two points are identical there is nothing to draw, and we prevent
	//a division by zero in calculation of dt.
	if(p2.x == p3.x && p2.y == p3.y)
		return;

	TPoint q;
	//Find step point from the horizontal or vertical differense in the points
	double dt = 2.0/std::max(abs(p2.x-p3.x), abs(p2.y-p3.y));

	TSplineParam Spline = CalcSingleSpline(p1, p2, p3, p4);

	//Calculate points on spline
	for(double t = 0; t < 1; t += dt)
	{
		//Add 0.5 to take care of rounding error. We assume the coordinates are always positive.
		q.x = 0.5 * (Spline.Param[0].x * t*t*t
							 + Spline.Param[1].x * t*t
							 + Spline.Param[2].x * t
							 + Spline.Param[3].x) + 0.5;
		q.y = 0.5 * (Spline.Param[0].y * t*t*t
							 + Spline.Param[1].y * t*t
							 + Spline.Param[2].y * t
							 + Spline.Param[3].y) + 0.5;

		//No need to add point if it is equal to the last one
		if(Points.empty() || q != Points.back())
			Points.push_back(q);
	}
}
//---------------------------------------------------------------------------
//Calculate points used to draw a series of cubic splines between points in P
void Create2DCubicSplines(std::vector<TPoint> &Points, const std::vector<TPoint> &P)
{
	if(P.size() == 2)
	{
		//Draw a straight line if only two points exists
		Points.push_back(P.front());
		Points.push_back(P.back());
	}

	if(P.size() <= 2)
		return;

	std::vector<TPoint>::const_iterator p1 = P.begin();
	std::vector<TPoint>::const_iterator p2 = P.begin();
	std::vector<TPoint>::const_iterator p3 = p2 + 1;
	std::vector<TPoint>::const_iterator p4 = p3 + 1;

	//Clear points, calculate the number of points needed to draw the splines and
	//allocate memory
	Points.clear();
	int PointNum = 0;
	for(unsigned I = 1; I < P.size(); I++)
		PointNum += std::max(abs(P[I-1].x-P[I].x), abs(P[I-1].y-P[I].y));
	Points.reserve(PointNum);

	//Draw the first spline from p2 to p3. The first point is also used as the first control point
	if(P.front() == P.back()) //WARNING: Don't use operator ?:  because of code generation bug in Bcc 5.6.4
		p1 = P.end()-2;
	CreateSingleSpline(Points, *p1, *p2++, *p3++, *p4++);
	p1 = P.begin();

	//Draw splines from p2 to p3. The point before and the point after are used as control points
	for(unsigned n = 0; n < P.size() - 3; n++)
		CreateSingleSpline(Points, *p1++, *p2++, *p3++, *p4++);

	//Draw the last spline. The last point is also used as the last control point
	if(P.front() == P.back()) //WARNING: Don't use operator ?: beacuse of code generation bug in Bcc 5.6.4
		p4 = P.begin()+1;
	else
		p4 = p3;
	CreateSingleSpline(Points, *p1, *p2, *p3, *p4);
}*/
//---------------------------------------------------------------------------
std::vector<double> CalcSecondDerivatives(const std::vector<TPoint> &P)
{
	int n = P.size();
	std::vector<double> y2(n), u(n-1);

	for(int i = 1; i < n-1; i++)
    if(P[i-1].x != P[i].X && P[i].x != P[i+1].x)
    {
      double sig = static_cast<double>(P[i].x - P[i-1].x) / (P[i+1].x - P[i-1].x);
      double p = sig * y2[i-1] + 2;
      y2[i] = (sig - 1) / p;
      u[i] = static_cast<double>(P[i+1].y - P[i].y) / (P[i+1].x - P[i].x) -
             static_cast<double>(P[i].y - P[i-1].y) / (P[i].x - P[i-1].x);
      u[i] = (6.0 * u[i] / (P[i+1].x - P[i-1].x) - sig * u[i-1]) / p;
    }
    else
      y2[i] = 0, u[i] = 0; //Avoid division by zero

	y2[n-1] = 0;
	for(int k = n-2; k >= 0; k--)
		y2[k] = y2[k] * y2[k+1] + u[k];
	return y2;
}
//---------------------------------------------------------------------------
double EvalCubicSpline(const std::vector<TPoint> &P, const std::vector<double> &y2, unsigned i, double x)
{
	double h = P[i+1].x - P[i].x;
  if(h == 0)
    return P[i].x; //Avoid division by zero
	double a = (P[i+1].x - x) / h;
	double b = (x - P[i].x) / h;
	double y = a * P[i].y + b *P[i+1].y + ((a*a*a-a) * y2[i] + (b*b*b-b)*y2[i+1]) * h*h/6.0;
	return y;
}
//---------------------------------------------------------------------------
void Create2DCubicSplines(std::vector<TPoint> &Points, const std::vector<TPoint> &P)
{
	unsigned n = P.size();
	std::vector<TPoint> Px, Py;
	Px.reserve(n);
	Py.reserve(n);
	for(unsigned I = 0; I < n; I++)
	{
		Px.push_back(TPoint(I, P[I].x));
		Py.push_back(TPoint(I, P[I].y));
	}

	std::vector<double> x2 = CalcSecondDerivatives(Px);
	std::vector<double> y2 = CalcSecondDerivatives(Py);
	for(unsigned I = 0; I < n-1; I++)
	{
    int ds = std::max(abs(P[I].x-P[I+1].x), abs(P[I].y-P[I+1].y));
		double dt = ds == 0 ? 1 : 1.0/ds;
		for(double t = I; t < I+1; t += dt)
		{
			double x = EvalCubicSpline(Px, x2, I, t);
			double y = EvalCubicSpline(Py, y2, I, t);
			Points.push_back(TPoint(x + 0.5, y + 0.5));
		}
	}
	Points.push_back(P.back());
}
//---------------------------------------------------------------------------
struct TCompPoint
{
	bool operator()(const TPoint &P1, const TPoint &P2) const {return P1.x < P2.X;}
};
//---------------------------------------------------------------------------
void CreateCubicSplines(std::vector<TPoint> &Points, const std::vector<TPoint> &P)
{
	std::vector<TPoint> P2 = P;
	std::sort(P2.begin(), P2.end(), TCompPoint());
	for(unsigned I = P2.size()-1; I > 1; I--)
		if(P2[I-1].X == P2[I].X) //Two points with same x-coordinate is not allowed
			P2.erase(P2.begin() + I);

	std::vector<double> y2 = CalcSecondDerivatives(P2);
	unsigned n = P2.size();
	for(unsigned i = 0; i < n-1; i++)
	{
		for(int x = P2[i].x; x < P2[i+1].x; x++)
		{
			double y = EvalCubicSpline(P2, y2, i, x);
			Points.push_back(TPoint(x, y + 0.5));
		}
	}
	Points.push_back(P2.back());
}
//---------------------------------------------------------------------------
// vt = (v1 + v2)/2 + (v1 - v2)/2 * cos(pi * (t - t1)/(t2-t1))
// where, vt is the value you're looking for (at time t), v1 and v2 are the start and end values,
// t1 and t2 are the start and end times.
// half-cosine interpolation
enum TCurveDirection {cdHorizontal, cdVertical};
TCurveDirection CreateSingleHalfCosine(std::vector<TPoint> &Points, TCurveDirection Direction, TPoint p2, TPoint p3, TPoint p4)
{
	//Continue horizontal if the points don't start to move backwards
	if((Direction == cdHorizontal && p2.x < p3.x && p3.x <= p4.x) || (Direction == cdHorizontal && p2.x > p3.x && p3.x >= p4.x))
	{
		bool Sign = p2.x < p3.x;
		for(double t = p2.x; (t <= p3.x) == Sign; Sign ? t++ : t--)
			Points.push_back(TPoint(t, (p2.y + p3.y)/2 + (p2.y - p3.y)/2 * std::cos(M_PI * (t - p2.x)/(p3.x - p2.x))));
		return cdHorizontal;
	}
	//Continue vertical if the points don't change up/down direction
	else if((Direction == cdVertical && p2.y < p3.y && p3.y <= p4.y) || (Direction == cdVertical && p2.y > p3.y && p3.y >= p4.y))
	{
		bool Sign = p2.y < p3.y;
		for(double t = p2.y; (t <= p3.y) == Sign; Sign ? t++ : t--)
			Points.push_back(TPoint((p2.x + p3.x)/2 + (p2.x - p3.x)/2 * std::cos(M_PI * (t - p2.y)/(p3.y - p2.y)), t));
		return cdVertical;
  }
  else
  {
    //Draw an quarter ellipsis if we need to change horizontal/vertical direction
		double dt = M_PI/std::max(abs(p2.x-p3.x), abs(p2.y-p3.y));
    for(double t = 0; t <= M_PI/2; t += dt)
    {
      TPoint q;
      if(Direction == cdHorizontal)
      {
        q.x = p2.x + (p3.x-p2.x) * std::sin(t);
        q.y = p3.y - (p3.y-p2.y) * std::cos(t);
			}
      else
      {
				q.x = p3.x - (p3.x-p2.x) * std::cos(t);
        q.y = p2.y + (p3.y-p2.y) * std::sin(t);
      }
      if(Points.empty() || Points.back() != q)
        Points.push_back(q);
    }
    return Direction == cdHorizontal ? cdVertical : cdHorizontal;
  }
}
//---------------------------------------------------------------------------
void CreateHalfCosineInterpolation(std::vector<TPoint> &Points, const std::vector<TPoint> &P)
{
	if(P.size() < 2)
	  return;
	//Start the curve horizontally; This might not always be the best but works in most cases
	TCurveDirection Direction = cdHorizontal;
	if(P.size()>2)
		Direction = CreateSingleHalfCosine(Points, Direction, P[0], P[1], P[2]);
  for(unsigned n = 1; n < P.size() - 2; n++)
    Direction = CreateSingleHalfCosine(Points, Direction, P[n], P[n+1], P[n+2]);
	if(P.size()>=2)
    Direction = CreateSingleHalfCosine(Points, Direction, *(P.end()-2), P.back(), P.back());
}
//---------------------------------------------------------------------------
//Calculate points used to draw a series of cubic splines between points in P
void Interpolate(std::vector<TPoint> &Points, const std::vector<TPoint> &P, TInterpolationAlgorithm Algorithm)
{
  switch(Algorithm)
  {
		case iaCubicSpline:
			Create2DCubicSplines(Points, P);
			break;

    case iaCubicSpline2:
      CreateCubicSplines(Points, P);
      break;

		case iaHalfCosine:
      CreateHalfCosineInterpolation(Points, P);
      break;

    case iaLinear:
    default:
      Points.insert(Points.end(), P.begin(), P.end());
  }
}
//---------------------------------------------------------------------------
//Returns the angle (slope) of the spline at p3. The spline goes from p2 to p3. p1 and p4 are control points
double InterpolationAngle(Func32::TDblPoint p1, Func32::TDblPoint p2, Func32::TDblPoint p3, Func32::TDblPoint p4, TInterpolationAlgorithm Algorithm)
{
  switch(Algorithm)
  {
    case iaCubicSpline:
    {
      double dx = 1.5 * (-p1.x + 3*p2.x -3*p3.x + p4.x) +
                        (2*p1.x -5*p2.x + 4*p3.x - p4.x) +
                  0.5 * (-p1.x+p3.x);

      double dy = 1.5 * (-p1.y + 3*p2.y -3*p3.y + p4.y) +
                        (2*p1.y -5*p2.y + 4*p3.y - p4.y) +
                  0.5 * (-p1.y+p3.y);
      return dx == 0 && dy == 0 ? 0 : std::atan2(dy, dx);
    }
    case iaHalfCosine:
      return 0;

    case iaLinear:
    default:
    {
      double dy = p3.y - p2.y;
      double dx = p3.x - p2.x;
      return dx == 0 && dy == 0 ? 0 : std::atan2(dy, dx);
    }
  }
}
//---------------------------------------------------------------------------
TRect Rotate(TMetafile *Metafile, int Degrees)
{
  if(Degrees == 0)
    return TRect(0, 0, Metafile->Width, Metafile->Height);

  unsigned Width = Metafile->Width;
  unsigned Height = Metafile->Height;
  double Angle = Degrees * M_PI / 180;

  unsigned NewWidth = std::abs(Width * std::cos(Angle)) + std::abs(Height * std::sin(Angle));
  unsigned NewHeight = std::abs(Width * std::sin(Angle)) + std::abs(Height * std::cos(Angle));

  std::auto_ptr<TMetafile> TempMetafile(new TMetafile);
  TempMetafile->Width = NewWidth;
  TempMetafile->Height = NewHeight;
  std::auto_ptr<TMetafileCanvas> Canvas(new TMetafileCanvas(TempMetafile.get(), 0));

  int dx = 0;
  int dy = 0;
  if(std::sin(Angle) > 0)
    dy += Width * std::sin(Angle);
  else
    dx -= Height * std::sin(Angle);
  if(std::cos(Angle) < 0)
  {
    dy -= Height * std::cos(Angle);
    dx -= Width * std::cos(Angle);
  }

  XFORM Xform = {std::cos(Angle), -std::sin(Angle), std::sin(Angle), std::cos(Angle), dx, dy};
  Win32Check(SetGraphicsMode(Canvas->Handle, GM_ADVANCED));
  Win32Check(SetWorldTransform(Canvas->Handle, &Xform));
  Canvas->Draw(0, 0, Metafile);
  Canvas.reset();
  Metafile->Assign(TempMetafile.get());
  return TRect(0, 0, NewWidth, NewHeight);
}
//---------------------------------------------------------------------------
unsigned AlignBit(unsigned Bits, unsigned BitsPerPixel, unsigned Alignment)
{
  Alignment--;
  return (((Bits * BitsPerPixel) + Alignment) & ~Alignment) >> 3;
}
//---------------------------------------------------------------------------
void InitializeBitmapInfoHeader(HBITMAP Bitmap, BITMAPINFOHEADER &Info, TPixelFormat PixelFormat)
{
  DIBSECTION DIB;
  DIB.dsBmih.biSize = 0;
  unsigned Bytes = GetObject(Bitmap, sizeof(DIB), &DIB);
  if(Bytes == 0)
    throw Exception("Invalid bitmap");

  if(Bytes >= (sizeof(DIB.dsBm) + sizeof(DIB.dsBmih)) &&
    DIB.dsBmih.biSize >= sizeof(DIB.dsBmih))
    Info = DIB.dsBmih;
  else
  {
    memset(&Info, 0, sizeof(Info));
    Info.biSize = sizeof(Info);
    Info.biWidth = DIB.dsBm.bmWidth;
    Info.biHeight = DIB.dsBm.bmHeight;
  }
  switch(PixelFormat)
  {
    case pf1bit: Info.biBitCount = 1;   break;
    case pf4bit: Info.biBitCount = 4;   break;
    case pf8bit: Info.biBitCount = 8;   break;
    case pf24bit: Info.biBitCount = 24; break;
    case pf32bit: Info.biBitCount = 32; break;
    default:
      throw Exception("Invalid pixel foramt");
  }
  Info.biPlanes = 1;
  Info.biCompression = BI_RGB; // Always return data in RGB format
  Info.biSizeImage = AlignBit(Info.biWidth, Info.biBitCount, 32) * abs(Info.biHeight);
}
//---------------------------------------------------------------------------
bool InternalGetDIB(HBITMAP Bitmap, HPALETTE Palette, void *BitmapInfo, void *Bits, TPixelFormat PixelFormat)
{
  HPALETTE OldPal = 0;
  BITMAPINFOHEADER &Info = *static_cast<BITMAPINFOHEADER*>(BitmapInfo);
  InitializeBitmapInfoHeader(Bitmap, Info, PixelFormat);
  HDC DC = CreateCompatibleDC(0);
  if(Palette != 0)
  {
    OldPal = SelectPalette(DC, Palette, False);
    RealizePalette(DC);
  }
  bool Result = GetDIBits(DC, Bitmap, 0, abs(Info.biHeight), Bits,
    static_cast<BITMAPINFO*>(BitmapInfo), DIB_RGB_COLORS);
  if(OldPal != 0)
    SelectPalette(DC, OldPal, false);
  DeleteDC(DC);
  return Result;
}
//---------------------------------------------------------------------------
void InternalGetDIBSizes(HBITMAP Bitmap, int &InfoHeaderSize, int &ImageSize, TPixelFormat PixelFormat)
{
  BITMAPINFOHEADER Info;
  InitializeBitmapInfoHeader(Bitmap, Info, PixelFormat);
  // Check for palette device format
  if(Info.biBitCount > 8)
  {
    // Header but no palette
    InfoHeaderSize = sizeof(Info);
    if((Info.biCompression & BI_BITFIELDS) != 0)
      InfoHeaderSize += 12;
  }
  else
    // Header and palette
    InfoHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(TRGBQuad) * (1 << Info.biBitCount);
  ImageSize = Info.biSizeImage;
}
//---------------------------------------------------------------------------

