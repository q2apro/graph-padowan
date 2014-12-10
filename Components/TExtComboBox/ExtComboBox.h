//---------------------------------------------------------------------------
#ifndef ExtComboBoxH
#define ExtComboBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TExtComboBox : public TComboBox
{
private:
  bool FAutoDroppedWidth;
  int FDroppedWidth;
  bool DroppedWidthFound;

  int FindMaxWidth();
  DYNAMIC void __fastcall DropDown();
  void __fastcall SetDroppedWidth(int AWidth);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

protected:
public:
  __fastcall TExtComboBox(TComponent* Owner);
__published:
  __property bool AutoDroppedWidth = {read=FAutoDroppedWidth, write=FAutoDroppedWidth};
  __property int DroppedWidth = {read=FDroppedWidth, write=SetDroppedWidth, default=0};
};
//---------------------------------------------------------------------------
#endif
