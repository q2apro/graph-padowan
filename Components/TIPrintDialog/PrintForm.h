//===========================================================================
// Copyright © 2004 Ivan Johansen
// PrintForm.h
//===========================================================================
#ifndef PrintFormH
#define PrintFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TPrintFrm : public TForm
{
__published:	// IDE-managed Components
  TShape *Shape1;
  TShape *Shape2;
  TButton *Button1;
  TButton *Button2;
  TGroupBox *GroupBox1;
  TComboBox *ComboBox1;
  TLabel *Label1;
  TRadioGroup *RadioGroup1;
  TRadioGroup *RadioGroup2;
  TGroupBox *GroupBox2;
  TCheckBox *CheckBox1;
  TLabel *Label3;
  TLabel *Label4;
  TEdit *Edit1;
  TEdit *Edit2;
  TGroupBox *GroupBox3;
  TLabel *Label5;
  TEdit *Edit3;
  TUpDown *UpDown3;
  TUpDown *UpDown4;
  TEdit *Edit4;
  TLabel *Label6;
  TImage *Image1;
  TButton *Button3;
  TLabel *Label7;
  TLabel *Label8;
  TLabel *Label9;
  TLabel *Label11;
  TLabel *Label12;
  TLabel *Label13;
  TUpDown *UpDown1;
  TUpDown *UpDown2;
  void __fastcall EditKeyPress(TObject *Sender, char &Key);
  void __fastcall CheckBox1Click(TObject *Sender);
  void __fastcall RadioGroup1Click(TObject *Sender);
  void __fastcall Edit1Change(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ComboBox1Change(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);

private:	// User declarations
  class TIPrintDialog *PrintDialog;

  void PrinterChanged();
  void ResizePaper();
  void DoPaintSample();

public:		// User declarations
  __fastcall TPrintFrm(TComponent* Owner, TIPrintDialog *APrintDialog);
};
//---------------------------------------------------------------------------
#endif
