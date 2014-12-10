//===========================================================================
// Copyright © 2003 Ivan Johansen
// MySplitter.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "MySplitter.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TMySplitter));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TMySplitter *)
{
  new TMySplitter(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMySplitter::TMySplitter(TComponent* Owner)
        : TSplitter(Owner), DblClicked(false)
{
}
//---------------------------------------------------------------------------
namespace Mysplitter
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TMySplitter)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMySplitter::DblClick(void)
{
  TSplitter::DblClick(); //Call inherited
  DblClicked = true;     //Set double click flag
}
//---------------------------------------------------------------------------
void __fastcall TMySplitter::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
  //Prevent MouseDown() in TSplitter from being called immediately after a double click.
  //This prevents the user from moveing the splitter aftera double click
  if(!DblClicked)
    TSplitter::MouseDown(Button, Shift, X, Y);  //Call inherited
  DblClicked = false;                           //Clear double click flag
}
//---------------------------------------------------------------------------

