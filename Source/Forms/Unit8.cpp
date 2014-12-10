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
#include "Unit8.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm8::TForm8(TComponent* Owner, TData &AData)
  : TForm(Owner), Data(AData), AllowReplace(false)
{
  ScaleForm(this);
  TranslateProperties(this);
  SetAccelerators(this);
  FlipForm(this);
}
//---------------------------------------------------------------------------
bool TForm8::EditModel(std::wstring &Name)
{
  Caption = LoadRes(503);
  AllowReplace = true;
  Edit1->Text = Name.c_str();
  TUserModel UserModel = Data.UserModels[Name];
  for(unsigned I = 0; I < UserModel.Defaults.size(); I++)
    ValueListEditor1->Values[ToUString(UserModel.Defaults[I].first)] = ToUString(UserModel.Defaults[I].second);

  if(ShowModal() == mrOk)
  {
    for(unsigned I = 0; I < UserModel.Defaults.size(); I++)
      UserModel.Defaults[I].second = ToWString(ValueListEditor1->Values[ToUString(UserModel.Defaults[I].first)]);

    Data.UserModels.erase(Name);
    Name = ToWString(Edit1->Text);
    Data.UserModels[Name] = UserModel;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
bool TForm8::AddModel(const std::wstring &Model, std::wstring &ModelName)
{
  TUserModel UserModel;
  UserModel.Model = Model;
  std::vector<std::wstring> Unknowns = Func32::FindUnknowns(Model);

  if(Unknowns.empty())
  {
    MessageBox(LoadRes(529), LoadRes(RES_ERROR));
    return false;
  }

  Unknowns.insert(Unknowns.begin(), L"x");

  Func32::TCustomFunc TempFunc(Model, Unknowns, Data.CustomFunctions.SymbolList, Data.Axes.Trigonometry);

  for(unsigned I = 1; I < Unknowns.size(); I++)
    ValueListEditor1->Values[Unknowns[I].c_str()] = 1;

  if(ShowModal() == mrOk)
  {
    for(std::vector<std::wstring>::const_iterator Iter = Unknowns.begin() + 1; Iter != Unknowns.end(); ++Iter)
      UserModel.Defaults.push_back(std::make_pair(*Iter, ToWString(ValueListEditor1->Values[ToUString(*Iter)])));

    ModelName = ToWString(Edit1->Text);
    Data.UserModels[ModelName] = UserModel;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TForm8::Button1Click(TObject *Sender)
{
  //Make sure that all numbers are valid
  int I;
  std::wstring Name = ToWString(Edit1->Text);
  //Empty names are not allowed
  if(Name.empty())
  {
    MessageBox(LoadRes(516), LoadRes(RES_ERROR));
    return;
  }

  //Two models with the same name cannot exist
  if(!AllowReplace)
  for(unsigned I = 0; I < Data.UserModels.size(); I++)
    if(Data.UserModels.count(Name) != 0)
    {
      MessageBox(LoadRes(517), LoadRes(RES_ERROR));
      return;
    }
  //We only validate here. The values is calculated again later
  for(I = 1; I < ValueListEditor1->RowCount; I++)
    try
    {
      ToDouble(ValueListEditor1->Cells[1][I]);
    }
    catch(...)
    {
      ValueListEditor1->Row = I;
      ValueListEditor1->SetFocus();
      MessageBox(LoadRes(RES_NOT_VALID_NUMBER, ValueListEditor1->Cells[1][I]), LoadRes(RES_ERROR_IN_VALUE));
      return;
    }
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------

