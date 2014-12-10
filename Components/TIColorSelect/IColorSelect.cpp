//---------------------------------------------------------------------------
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "IColorSelect.h"
#include "Consts.hpp" //Declarition of constants in resource
#include "ICompCommon.h"
#include <memory>
#pragma package(smart_init)

#define BOX_WIDTH  14
#define BOX_SPACE   4
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TIColorSelect));
//Table with colors used to initialize ColorList
const int ColorTable[256] =
{
  0x000000,0x000080,0x008000,0x008080,0x800000,0x800080,0x808000,0xC0C0C0,0xC0DCC0,0xF0CAA6,0x330000,0x000033,0x330033,0x333300,0x161616,0x1C1C1C,
  0x222222,0x292929,0x555555,0x4D4D4D,0x424242,0x393939,0x807CFF,0x5050FF,0x9300D6,0xFFECCC,0xC6D6EF,0xD6E7E7,0x90A9AD,0x00FF33,0x000066,0x000099,
  0x0000CC,0x003300,0x003333,0x003366,0x003399,0x0033CC,0x0033FF,0x006600,0x006633,0x006666,0x006699,0x0066CC,0x0066FF,0x009900,0x009933,0x009966,
  0x009999,0x0099CC,0x0099FF,0x00CC00,0x00CC33,0x00CC66,0x00CC99,0x00CCCC,0x00CCFF,0x00FF66,0x00FF99,0x00FFCC,0x33FF00,0xFF0033,0x330066,0x330099,
  0x3300CC,0x3300FF,0xFF3300,0x333333,0x333366,0x333399,0x3333CC,0x3333FF,0x336600,0x336633,0x336666,0x336699,0x3366CC,0x3366FF,0x339900,0x339933,
  0x339966,0x339999,0x3399CC,0x3399FF,0x33CC00,0x33CC33,0x33CC66,0x33CC99,0x33CCCC,0x33CCFF,0x33FF33,0x33FF66,0x33FF99,0x33FFCC,0x33FFFF,0x660000,
  0x660033,0x660066,0x660099,0x6600CC,0x6600FF,0x663300,0x663333,0x663366,0x663399,0x6633CC,0x6633FF,0x666600,0x666633,0x666666,0x666699,0x6666CC,
  0x669900,0x669933,0x669966,0x669999,0x6699CC,0x6699FF,0x66CC00,0x66CC33,0x66CC99,0x66CCCC,0x66CCFF,0x66FF00,0x66FF33,0x66FF99,0x66FFCC,0xCC00FF,
  0xFF00CC,0x999900,0x993399,0x990099,0x9900CC,0x990000,0x993333,0x990066,0x9933CC,0x9900FF,0x996600,0x996633,0x993366,0x996699,0x9966CC,0x9933FF,
  0x999933,0x999966,0x999999,0x9999CC,0x9999FF,0x99CC00,0x99CC33,0x66CC66,0x99CC99,0x99CCCC,0x99CCFF,0x99FF00,0x99FF33,0x99CC66,0x99FF99,0x99FFCC,
  0x99FFFF,0xCC0000,0x990033,0xCC0066,0xCC0099,0xCC00CC,0x993300,0xCC3333,0xCC3366,0xCC3399,0xCC33CC,0xCC33FF,0xCC6600,0xCC6633,0x996666,0xCC6699,
  0xCC66CC,0x9966FF,0xCC9900,0xCC9933,0xCC9966,0xCC9999,0xCC99CC,0xCC99FF,0xCCCC00,0xCCCC33,0xCCCC66,0xCCCC99,0xCCCCCC,0xCCCCFF,0xCCFF00,0xCCFF33,
  0x99FF66,0xCCFF99,0xCCFFCC,0xCCFFFF,0xCC0033,0xFF0066,0xFF0099,0xCC3300,0xFF3333,0xFF3366,0xFF3399,0xFF33CC,0xFF33FF,0xFF6600,0xFF6633,0xCC6666,
  0xFF6699,0xFF66CC,0xCC66FF,0xFF9900,0xFF9933,0xFF9966,0xFF9999,0xFF99CC,0xFF99FF,0xFFCC00,0xFFCC33,0xFFCC66,0xFFCC99,0xFFCCCC,0xFFCCFF,0xFFFF33,
  0xCCFF66,0xFFFF99,0xFFFFCC,0x6666FF,0x66FF66,0x66FFFF,0xFF6666,0xFF66FF,0xFFFF66,0x2100A5,0x5F5F5F,0x777777,0x868686,0x969696,0xCBCBCB,0xB2B2B2,
  0xD7D7D7,0xDDDDDD,0xE3E3E3,0xEAEAEA,0xF1F1F1,0xF8F8F8,0xF0FBFF,0xA4A0A0,0x808080,0x0000FF,0x00FF00,0x00FFFF,0xFF0000,0xFF00FF,0xFFFF00,0xFFFFFF,
};
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TIColorSelect *)
{
        new TIColorSelect(NULL);
}
//---------------------------------------------------------------------------
namespace Icolorselect
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TIColorSelect)};
                 RegisterComponents("IComp", classes, 0);
        }
}
//---------------------------------------------------------------------------
//Constructor initializes variables
__fastcall TIColorSelect::TIColorSelect(TComponent* Owner)
  : TComponent(Owner), FColorList((TColor*)ColorTable, (TColor*)ColorTable+256),
    FColCount(16), FRowCount(16), FColor(clRed), FCaption(TranslateCompText("Select color")),
    FEditColor(false), FShowHelp(false), Form(NULL), FOnShow(NULL), FOnClose(NULL)
{
}
//---------------------------------------------------------------------------
//Draw the small color boxes
void __fastcall TIColorSelect::RefreshColors()
{
  //Set bitmap with and height
  Image->Picture->Bitmap->Width=Image->Width;
  Image->Picture->Bitmap->Height=Image->Height;
  //Draw rectangle around all the boxes
  Image->Canvas->Pen->Color = clBlack;
  Image->Canvas->Brush->Color = clBtnFace;
  Image->Canvas->Rectangle(0,0,Image->Width,Image->Height);
  //Loop through all colors and draw the boxes
  for(int j=0;j<RowCount;j++)
    for(int i=0;i<ColCount;i++)
    {
      int x = BOX_SPACE+i*(BOX_WIDTH+BOX_SPACE);
      int y = BOX_SPACE+j*(BOX_WIDTH+BOX_SPACE);
      Image->Canvas->Brush->Color = ColorList[i][j];
      Image->Canvas->FillRect(TRect(x,y,x+BOX_WIDTH,y+BOX_WIDTH));
      if(FColor == Image->Canvas->Brush->Color)
        SelectBox(i,j);
    }
}
//---------------------------------------------------------------------------
//Show the color select dialog on the screen
bool __fastcall TIColorSelect::Execute()
{
  //Call event if enabled
  if(FOnShow)
    FOnShow(this);

  //Create dialog and set properties
  CreateColorDialog();

  //Show dialog and save exit method
  bool Result = Form->ShowModal()==mrOk;
  delete Form; //Free memory
  Form = NULL;
  //Save selected color
  if(Result)
    Color = ColorList[SelectedBox.x][SelectedBox.y];
  //Call event if enabled
  if(FOnClose)
    FOnClose(this);
  return Result;
}
//---------------------------------------------------------------------------
//Returns a color from the list
//Called through property ColorList
TColor __fastcall TIColorSelect::GetColorList(int i,int j)
{
  if(i<0 || i>=ColCount || j<0 || j>=RowCount)
    throw EArrayIndex("Index in array is out of bounds.");
  return FColorList[i+j*ColCount];
}
//---------------------------------------------------------------------------
//Sets a color in the list
//Called through property ColorList
void __fastcall TIColorSelect::SetColorList(int i,int j,TColor Value)
{
  if(i<0 || i>=ColCount || j<0 || j>=RowCount)
    throw EArrayIndex("Index in array is out of bounds.");
  FColorList[i+j*ColCount] = Value;
}
//---------------------------------------------------------------------------
//Set number of rows in dialog
//Called through property RowCount
void __fastcall TIColorSelect::SetRowCount(int Count)
{
  if(Count < 1)
    Count = 1;
  FRowCount=Count;
  FColorList.resize(FRowCount*FColCount);
}
//---------------------------------------------------------------------------
//Set number of columns in dialog
//Called through property ColCount
void __fastcall TIColorSelect::SetColCount(int Count)
{
  if(Count < 1)
    Count = 1;
  FColCount=Count;
  FColorList.resize(FRowCount*FColCount);
}
//---------------------------------------------------------------------------
//Remove selection rectangle from currently selected box and shows the rectangle
//around box (i,j). SelectedBox is updated
void __fastcall TIColorSelect::SelectBox(int i,int j)
{
  if(i>=ColCount || i<0 || j>=RowCount || j<0)
    return;

  Image->Canvas->Brush->Color = clBtnFace;
  int x = Image->Left + BOX_SPACE/2 + SelectedBox.x * (BOX_WIDTH + BOX_SPACE);
  int y = Image->Top + BOX_SPACE/2 + SelectedBox.y * (BOX_WIDTH + BOX_SPACE);
  Image->Canvas->FrameRect(TRect(x, y, x + BOX_WIDTH + BOX_SPACE, y + BOX_WIDTH + BOX_SPACE));

  Image->Canvas->Brush->Color = clBlack;
  x = Image->Left + BOX_SPACE/2 + i*(BOX_WIDTH+BOX_SPACE);
  y = Image->Top + BOX_SPACE/2 + j*(BOX_WIDTH+BOX_SPACE);
  Image->Canvas->FrameRect(TRect(x, y, x + BOX_WIDTH + BOX_SPACE, y + BOX_WIDTH + BOX_SPACE));

  SelectedBox = TPoint(i, j);
}
//---------------------------------------------------------------------------
//Sets (i,j) to the box at pixel position (x,y) calculated from upper left
//corner of the frame
void __fastcall TIColorSelect::PixelToBox(int x,int y,int &i,int &j)
{
  if(x<0 || y<0)
  {
    i = j = -1;
    return;
  }
  i = (x-BOX_SPACE/2)/(BOX_WIDTH+BOX_SPACE);
  j = (y-BOX_SPACE/2)/(BOX_WIDTH+BOX_SPACE);
}
//---------------------------------------------------------------------------
//Called when the one of the mouse buttons is pressed down
void __fastcall TIColorSelect::ImageMouseDown(TObject *Sender,TMouseButton Button,TShiftState Shift, int X, int Y)
{
  //If left button pressed: selct box at mouse position
  if(Button==mbLeft)
  {
    int i;
    int j;
    PixelToBox(X,Y,i,j);
    SelectBox(i,j);
  }
  Panel->SetFocus();
}
//---------------------------------------------------------------------------
//Called when the panel has focus and a key is pressed
void __fastcall TIColorSelect::PanelKeyDown(TObject *Sender, Word &Key,TShiftState Shift)
{
  switch(Key)
  {
    case VK_UP:
      SelectBox(SelectedBox.x,SelectedBox.y-1);
      break;
    case VK_DOWN:
      SelectBox(SelectedBox.x,SelectedBox.y+1);
      break;
    case VK_LEFT:
      if(SelectedBox.x)
        SelectBox(SelectedBox.x-1,SelectedBox.y);
      else
        SelectBox(ColCount-1,SelectedBox.y-1);
      break;
    case VK_RIGHT:
      if(SelectedBox.x<ColCount-1)
        SelectBox(SelectedBox.x+1,SelectedBox.y);
      else
        SelectBox(0,SelectedBox.y+1);
  }
}
//---------------------------------------------------------------------------
//Called when the user dubbleclicks on the image
void __fastcall TIColorSelect::ImageDblClick(TObject *Sender)
{
  //If editing of colors is enabled then show the color dialog and
  //update the select box with the user chosen color
  if(!EditColor)
    return;
  TColorDialog *ColorDialog=new TColorDialog(this);
  ColorDialog->Color = ColorList[SelectedBox.x][SelectedBox.y];
  ColorDialog->CustomColors->Add(("ColorA=")+IntToHex((int)ColorDialog->Color,8));
  if(ColorDialog->Execute())
    DrawBox(SelectedBox,ColorDialog->Color);
  delete ColorDialog;
}
//---------------------------------------------------------------------------
//This draws the box at position Box with brush set to Color
void __fastcall TIColorSelect::DrawBox(TPoint Box,TColor Color)
{
  int x = Image->Left+BOX_SPACE+Box.x*(BOX_WIDTH+BOX_SPACE);
  int y = Image->Top+BOX_SPACE+Box.y*(BOX_WIDTH+BOX_SPACE);
  Image->Canvas->Brush->Color = Color;
  Image->Canvas->FillRect(TRect(x,y,x+BOX_WIDTH,y+BOX_WIDTH));
  ColorList[Box.x][Box.y] = Color;
}
//---------------------------------------------------------------------------
//Creates a palette from the colors in colorList and returns a handle to the palette.
//Called through property Palette
//The user is resposible for freeing the handle through ::DeleteObject()
HPALETTE __fastcall TIColorSelect::GetPalette()
{
  LOGPALETTE *Pal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE)+FColorList.size()*sizeof(TColor));
  Pal->palVersion = 0x300;
  Pal->palNumEntries = 256;
  for(unsigned int i=0;i<FColorList.size();i++)
    Pal->palPalEntry[i] = *((PALETTEENTRY*)&FColorList[i]);
  HPALETTE HPal = CreatePalette(Pal);
  free(Pal);
  return HPal;
}
//---------------------------------------------------------------------------
//Creates color form and its controls and sets properties
void __fastcall TIColorSelect::CreateColorDialog()
{
  Form = new TForm(Application);
  Form->Caption = Caption;
  Form->HelpContext = HelpContext;
  Form->Position = poMainFormCenter;
  Form->BorderStyle = bsDialog;
  Form->BorderIcons = Form->BorderIcons.Clear();

  Panel = new TFocusPanel(Form);
  Panel->Parent = Form;
  Panel->Top = 8;
  Panel->Left = 8;
  Panel->Width =  321;
  Panel->Height = 305;
  Panel->Width = ColCount*(BOX_WIDTH+BOX_SPACE)+BOX_SPACE;
  Panel->Height = RowCount*(BOX_WIDTH+BOX_SPACE)+BOX_SPACE;
  Panel->OnKeyDown = PanelKeyDown;
  Panel->TabStop = true;
  Panel->DoubleBuffered = true; //Prevent flicker

  Image = new TImage(Form);
  Image->Parent = Panel;
  Image->Width = Panel->Width;
  Image->Height = Panel->Height;
  Image->OnMouseDown = ImageMouseDown;
  Image->OnDblClick = ImageDblClick;

  TButton *Button1 = new TButton(Form);
  Button1->Parent = Form;
  Button1->Top = 16;
  Button1->Left = Panel->Left+Panel->Width+20;
  Button1->Caption = Consts_SOKButton;
  Button1->ModalResult = mrOk;
  Button1->Default = true;

  TButton *Button2 = new TButton(Form);
  Button2->Parent = Form;
  Button2->Top = 56;
  Button2->Left = Panel->Left+Panel->Width+20;
  Button2->Caption = Consts_SCancelButton;
  Button2->ModalResult = mrCancel;
  Button2->Cancel = true;

  TButton *Button3 = new TButton(Form);
  Button3->Parent = Form;
  Button3->Top = 96;
  Button3->Left = Panel->Left+Panel->Width+20;
  Button3->Caption = Consts_SHelpButton;
  Button3->OnClick = Button3Click;
  if(!ShowHelp)
    Button3->Visible = false;

  Form->Width = Image->Width + 120;
  Form->Height = Image->Height + 45;
  if(ShowHelp)
  {
    if(Form->ClientHeight < Button3->Height + Button3->Top+5)
      Form->ClientHeight = Button3->Height + Button3->Top+5;
  }
  else
    if(Form->ClientHeight < Button2->Height + Button2->Top+5)
      Form->ClientHeight = Button2->Height + Button2->Top+5;
  RefreshColors();
}
//---------------------------------------------------------------------------
void __fastcall TIColorSelect::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------

