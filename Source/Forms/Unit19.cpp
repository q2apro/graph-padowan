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
#include "Unit19.h"
#include "Unit20.h"
#include <ComObj.hpp>
#include <iterator>
#include "vfw.h"
#include "Images.h"
#include "PyGraph.h"
#include <cmath>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include "Swig.h"
#include "PythonBind.h"
#include "GuiHelper.h"
using phoenix::arg1;
//---------------------------------------------------------------------------
#pragma link "ProgressForm"
#pragma link "MyEdit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm19::TForm19(TComponent* Owner, TData &AData, int AWidth, int AHeight)
  : TForm(Owner), Data(AData), AnimationInfo(AData.AnimationInfo)
{
  TranslateProperties(this);
  SetAccelerators(this);
  ScaleForm(this);

  int Left = Label7->Left + TMaxWidth(Label2)(Label3)(Label4)(Label5)(Label6)(Label7) + 5;
  ResizeControl(ComboBox1, Left);
  ResizeControl(Edit1, Left);
  ResizeControl(Edit2, Left);
  ResizeControl(Edit3, Left);
  ResizeControl(Edit4, Left);
  ResizeControl(Edit5, Left);
  ResizeControl(Edit6, Left);

  dwICValue = dwICValue; //Avoid stupid warning

  //Add constants to the combo box
  for(TCustomFunctions::TConstIterator Iter = Data.CustomFunctions.Begin(); Iter != Data.CustomFunctions.End(); ++Iter)
    if(Iter->Arguments.empty())
    {
      TAnimationConstant &AnimationConstant = AnimationInfo.ConstantList[Iter->Name];
      if(AnimationConstant.Step.empty())
      {
        double Value = Func32::Eval(Iter->Name, Data.CustomFunctions.SymbolList, Data.Axes.Trigonometry);
        double Min = (Value == 0) ? 10 : Value / 10;
        double Max = (Value == 0) ? 10 : Value * 10;
        AnimationConstant.Min = ToWString(Min);
        AnimationConstant.Max = ToWString(Max);
        AnimationConstant.Step = ToWString(std::abs((Max - Min) / 9));
      }
      ComboBox1->Items->Add(Iter->Name.c_str());
    }

  unsigned ImageWidth = AnimationInfo.Width == 0 ? std::min(AWidth, Screen->Width - 50) : AnimationInfo.Width;
  unsigned ImageHeight = AnimationInfo.Height == 0 ? std::min(AHeight, Screen->Height - 180) : AnimationInfo.Height;

  int Index = ComboBox1->Items->IndexOf(ToUString(AnimationInfo.Constant));
  ComboBox1->ItemIndex = Index == -1 ? 0 : Index;

  if(Index == -1)
    ComboBox1Change(ComboBox1);
  else
  {
    Edit1->Text = ToUString(AnimationInfo.ConstantList[AnimationInfo.Constant].Min);
    Edit2->Text = ToUString(AnimationInfo.ConstantList[AnimationInfo.Constant].Max);
    Edit3->Text = ToUString(AnimationInfo.ConstantList[AnimationInfo.Constant].Step);
  }

  Edit4->Text = ImageWidth;
  Edit5->Text = ImageHeight;
  Edit6->Text = AnimationInfo.FramesPerSecond;
  FlipForm(this);
}
//---------------------------------------------------------------------------
void __fastcall TForm19::Button1Click(TObject *Sender)
{
  if(!CheckLimit(Edit4, LoadRes(RES_GREATER, Label5->Caption, 160), 160))
    return;
  if(!CheckLimit(Edit5, LoadRes(RES_GREATER, Label6->Caption, 160), 160))
    return;

  std::auto_ptr<Graphics::TBitmap> Bitmap(new Graphics::TBitmap);
  unsigned ImageWidth = Edit4->Text.ToInt();
  unsigned ImageHeight = Edit5->Text.ToInt();
  if(Edit4->Modified || Edit5->Modified)
  {
    AnimationInfo.Width = ImageWidth;
    AnimationInfo.Height = ImageHeight;
  }

  Bitmap->Width = ImageWidth;
  Bitmap->Height = ImageHeight;

  AnimationInfo.Constant = ToWString(ComboBox1->Text);
  TDraw Draw(Bitmap->Canvas, &Data, false, "Animate thread");
  Draw.SetArea(TRect(0, 0, ImageWidth, ImageHeight));

  ComboBox1Change(NULL); //Update AnimationInfo

  double Min = MakeFloat(Edit1);
  double Max = MakeFloat(Edit2);
  double Step = std::abs(MakeFloat(Edit3, LoadRes(RES_NOT_EQUAL_ZERO, Label4->Caption), phoenix::arg1 != 0));
  if(Max < Min)
    Step = -Step;
  AnimationInfo.FramesPerSecond = MakeFloat(Edit6, LoadRes(RES_GREATER_ZERO, Label7->Caption), 0.01);

  unsigned StepCount = std::ceil((Max - Min) / Step) + 1;
  ProgressForm1->Max = StepCount;
  ProgressForm1->Position = 0;
  ProgressForm1->Show();
  TCallOnRelease Dummy(&ProgressForm1->Close);

  Data.CustomFunctions.Reset();
  Data.GetTopElem()->Update();
  std::vector<TGraphElemPtr> ElemList;
  GetSymbolDependent(AnimationInfo.Constant, ElemList, Data.GetTopElem());

  TRect Rect(0, 0, ImageWidth, ImageHeight);
  std::vector<char> ImageData;
  ImageData.reserve(Rect.Width() * Rect.Height());

  //Make space for 256 colors in palette
  std::vector<char> BitmapInfoData(256 * sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER));
  BITMAPINFO *BitmapInfo = reinterpret_cast<BITMAPINFO*>(&BitmapInfoData[0]);
  FillBitmapInfoHeader(BitmapInfo->bmiHeader, Bitmap.get(), Rect, 256, 0);
  String TempFile = GetTempFileName("Graph", "avi");
  std::wstring OriginalValue = Data.CustomFunctions.GetValue(AnimationInfo.Constant).Text;

  AVIFileInit();
  TCallOnRelease Dummy2(AVIFileExit);
  TCallOnRelease Dummy3(::DeleteFileW, TempFile.c_str());

  //Inner scope needed to release the avi file when the creation is finished.
  {
    PAVIFILE pFile = NULL;
    OleCheck(AVIFileOpen(&pFile, TempFile.c_str(), OF_WRITE | OF_CREATE, NULL));
    TCallOnRelease Dummy4(AVIFileRelease, pFile);

    PAVISTREAM pStream = NULL;
    AVISTREAMINFOA StreamInfo;

    StreamInfo.fccType = streamtypeVIDEO;
    StreamInfo.fccHandler = 0; //CompressionHandler[ComboBox2->ItemIndex];
    StreamInfo.dwFlags = AVISTREAMINFO_FORMATCHANGES;
    StreamInfo.dwCaps = 0;
    StreamInfo.wPriority = 0;
    StreamInfo.wLanguage = 0;
    StreamInfo.dwScale = 100;
    StreamInfo.dwRate = AnimationInfo.FramesPerSecond * 100;
    StreamInfo.dwStart = 0;
    StreamInfo.dwLength = StepCount; //Number of frames, I think
    StreamInfo.dwInitialFrames = 0;
    StreamInfo.dwSuggestedBufferSize = ImageHeight * ImageWidth * 4; //Should be changed later
    StreamInfo.dwQuality = -1;
    StreamInfo.dwSampleSize = 0;
    StreamInfo.rcFrame = TRect(0, 0, ImageWidth, ImageHeight);
    StreamInfo.dwEditCount = 0;
    StreamInfo.dwFormatChangeCount = 0;
    strcpy(StreamInfo.szName, "Graph animation");

    OleCheck(AVIFileCreateStreamA(pFile, &pStream, &StreamInfo));
    TCallOnRelease Dummy5(AVIStreamRelease, pStream);

    //WARNING: The media player in Graph may still have some problems when the palette
    //changes. Therefore we try to guess the colors beforehand.
    std::set<TColor> GraphColors;
    FindColors(Data, GraphColors);
    std::vector<RGBQUAD> Colors;
    for(std::set<TColor>::iterator Iter = GraphColors.begin(); Iter != GraphColors.end(); ++Iter)
      Colors.push_back(ColorToRGBQUAD(*Iter));

		Python::TPyObjectPtr DataObject(Python::IsPythonInstalled() ? ToPyObject(Data) : NULL, false);
    int LastColorCount = -1; //Use invalid value as default to force writing of colors in the first loop
    for(unsigned I = 0; I < StepCount; I++)
    {
      long double Value = Min + Step * I;
      Bitmap->Canvas->Brush->Style = bsSolid;
      Bitmap->Canvas->Brush->Color = Data.Axes.BackgroundColor;
      Bitmap->Canvas->FillRect(TRect(0, 0, ImageWidth, ImageHeight));
      Data.CustomFunctions.Replace(AnimationInfo.Constant, Value);
      ExecutePluginEvent(Python::peAnimate, DataObject.get(), AnimationInfo.Constant, static_cast<double>(Value));
      Data.CustomFunctions.Update(Data);
      for(unsigned I = 0; I < ElemList.size(); I++)
      {
        ElemList[I]->Update();
        ElemList[I]->ClearCache();
      }
      Draw.DrawAll();
      Draw.Wait();

      //As we don't know what colors will be used in all the frames, the palette is updated for each frame.
      //It seems to give problems when the palette is changed, at least with the MCI used in Graph,
      //so we only add to the palette. It also gives problems when the number of entries is
      //increased. Therefore we always store 256 colors, where the unused have dummy values.
      //Why is this so difficult?
      CompressBitmap(Bitmap.get(), Rect, Colors, ImageData);
      if(static_cast<int>(Colors.size()) > LastColorCount)
      {
        std::copy(Colors.begin(), Colors.end(), BitmapInfo->bmiColors);
        OleCheck(AVIStreamSetFormat(pStream, I, BitmapInfo, BitmapInfoData.size()));
        LastColorCount = Colors.size();
      }

      OleCheck(AVIStreamWrite(pStream, I, 1, &ImageData[0], ImageData.size(), AVIIF_KEYFRAME, NULL, NULL));

/*
      //Save uncrompressed
      int BitmapInfoSize;
      int BitmapSize;
      Bitmap->HandleType = bmDIB;
      Bitmap->PixelFormat = pf8bit;
      InternalGetDIBSizes(Bitmap->Handle, BitmapInfoSize, BitmapSize, Bitmap->PixelFormat);
      std::vector<char> BitmapInfo(BitmapInfoSize);
      std::vector<char> BitmapBits(BitmapSize);
      InternalGetDIB(Bitmap->Handle, 0, &BitmapInfo[0], &BitmapBits[0], Bitmap->PixelFormat);
      if(I == 0)
        OleCheck(AVIStreamSetFormat(pStream, 0, &BitmapInfo[0], BitmapInfoSize));
      AVIStreamWrite(pStream, I, 1, &BitmapBits[0], BitmapSize, AVIIF_KEYFRAME, NULL, NULL);
*/

      ProgressForm1->StepIt();
      if(ProgressForm1->AbortProgress)
      {
        //Restore constant in case we want to create a new animation with another constant
        Data.CustomFunctions.Replace(AnimationInfo.Constant, OriginalValue);
        return;
      }
    }
  }

  //Restore constant in case we want to create a new animation with another constant
  Data.CustomFunctions.Replace(AnimationInfo.Constant, OriginalValue);
	ProgressForm1->Close();
	CreateForm<TForm20>(AnimationInfo.Constant, Min, Step)->ShowAnimation(TempFile);
}
//---------------------------------------------------------------------------
void __fastcall TForm19::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void __fastcall TForm19::TntEditKeyPress(TObject *Sender, char &Key)
{
  if(!isdigit(Key) && Key != '\b')
    Key = 0;
}                                             
//---------------------------------------------------------------------------
void __fastcall TForm19::ComboBox1Change(TObject *Sender)
{
  if(!AnimationInfo.Constant.empty())
  {
    TAnimationConstant &AnimationConstant = AnimationInfo.ConstantList[AnimationInfo.Constant];
    AnimationConstant.Min = ToWString(Edit1->Text);
    AnimationConstant.Max = ToWString(Edit2->Text);
    AnimationConstant.Step = ToWString(Edit3->Text);
  }

  AnimationInfo.Constant = ToWString(ComboBox1->Text);
  TAnimationConstant &AnimationConstant = AnimationInfo.ConstantList[ToWString(ComboBox1->Text)];
  Edit1->Text = ToUString(AnimationConstant.Min);
  Edit2->Text = ToUString(AnimationConstant.Max);
  Edit3->Text = ToUString(AnimationConstant.Step);
}
//---------------------------------------------------------------------------
void TForm19::GetSymbolDependent(const std::wstring &SymbolName, std::vector<TGraphElemPtr> &List, const TGraphElemPtr &Elem) const
{
  if(Elem->GetVisible() && Elem->IsDependent(SymbolName))
    List.push_back(Elem);
  for(unsigned I = 0; I < Elem->ChildCount(); I++)
    GetSymbolDependent(SymbolName, List, Elem->GetChild(I));
}
//---------------------------------------------------------------------------




