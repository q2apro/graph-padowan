//---------------------------------------------------------------------------
#ifndef IFontBoxH
#define IFontBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class TIFontBox : public TCustomComboBox
{
private:
	typedef Stdctrls::TCustomComboBox inherited;

  String FSample;
  int FSamplePos;
  int OldItemIndex;

  String __fastcall GetFontName();
  void __fastcall SetFontName(const String &Str);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

protected:
  void __fastcall CreateParams(TCreateParams &Params);
  void __fastcall DrawItem(int Index, const Types::TRect &Rect, TOwnerDrawState State);
  void __fastcall MeasureItem(int Index, int &H);
  DYNAMIC void __fastcall DoExit();
  DYNAMIC void __fastcall Select();

public:
  __fastcall TIFontBox(TComponent* Owner);
  __property SelText;
  __property DroppedDown;
  __property SelStart;
  __property SelLength;

__published:
  __property String FontName = {read=GetFontName, write=SetFontName};
  __property String Sample = {read=FSample, write=FSample};
  __property int SamplePos = {read=FSamplePos, write=FSamplePos};
  __property AutoComplete;
  __property AutoDropDown;
  __property DropDownCount;
  __property TabOrder;
  __property TabStop;
  __property Font;
  __property Visible;
  __property ParentShowHint;
  __property ShowHint;
  __property ItemHeight;

  __property OnChange;
	__property OnCloseUp;
	__property OnDropDown;
  __property OnSelect;
  __property OnEnter;
  __property OnExit;
  __property OnKeyDown;
  __property OnKeyPress;
  __property OnKeyUp;
};
//---------------------------------------------------------------------------
#endif
