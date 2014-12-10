//===========================================================================
// Copyright © 2003 Ivan Johansen
// TRecent.cpp
//===========================================================================
#include "Config.h"
#pragma hdrstop
#include <Registry.hpp>
#include "TRecent.h"
#include <deque>
#include <vector>
#include <memory>
#include <StrUtils.hpp>
#include <Shlwapi.h>
#include "ICompCommon.h"
#pragma package(smart_init)
#pragma link "Shlwapi.lib"

//Header that isn't included correct
#define SHARD_PATH     0x00000002L
SHSTDAPI_(void) SHAddToRecentDocs(UINT uFlags, LPCVOID pv);
static TRegisterClass Dummy(__classid(TRecent));

class TOpenFileAction : public TCustomAction
{
  String FileName;
  TRecent *Recent;

public:
  TOpenFileAction(const String &AFileName, TRecent *ARecent)
    : TCustomAction(NULL), FileName(AFileName), Recent(ARecent) {}
  DYNAMIC bool __fastcall Execute() {if(Recent->OnLoadFile) Recent->OnLoadFile(Recent, FileName); return true;}
};
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TRecent *)
{
	new TRecent(NULL);
}
//---------------------------------------------------------------------------
namespace Trecent
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TRecent)};
		RegisterComponents("IComp", classes, 0);
	}
}
//---------------------------------------------------------------------------
//Constructor initialzes values and allocates memory
__fastcall TRecent::TRecent(TComponent* Owner)
	: TComponent(Owner), FEnabled(true), FMaxFiles(9), FOnLoadFile(NULL),
		FFileMenu(NULL), FAddToRecentDocs(true), Seperator1(NULL), Seperator2(NULL), FMaxPathLen(50)
{
	ReadFromRegistry();//Read file names from registry
}
//---------------------------------------------------------------------------
//Destructor deallocates memory
__fastcall TRecent::~TRecent()
{
  //Nothing to do here
}
//---------------------------------------------------------------------------
void __fastcall TRecent::SetMaxFiles(unsigned Value)
{
  RemoveMenuItems();//Remove shown menu items
  FMaxFiles = Value > MaxRecentFiles ? MaxRecentFiles : Value;
  if(FileList.size() > FMaxFiles)
    FileList.resize(FMaxFiles); //RemoveMenuItems() has already removed the actions
  SaveToRegistry();
  ShowMenuItems();//Show menu items again
}
//---------------------------------------------------------------------------
void __fastcall TRecent::SetRegistryKey(const String &Value)
{
  if(Value != FRegistryKey)
  {
    RemoveMenuItems();//Remove shown menu items
    FRegistryKey = Value;//Change used registry key
    ReadFromRegistry();//Read data from new key in registry
    ShowMenuItems();//Show menu items
  }
}
//---------------------------------------------------------------------------
void __fastcall TRecent::SetFileMenu(TActionClients *Value)
{
  RemoveMenuItems();//Remove shown menu items
  FFileMenu = Value;
  ShowMenuItems();//Show menu items in a new place
}
//---------------------------------------------------------------------------
int TRecent::FileIndex(const String &FileName)
{
  for(unsigned I = 0; I < FileList.size(); I++)
    if(SameFileName(FileList[I].first, FileName))
      return I;
  return -1;
}
//---------------------------------------------------------------------------
int TRecent::ObjectIndex(TObject *Object)
{
  for(unsigned I = 0; I < FileList.size(); I++)
    if(FileList[I].second == Object)
      return I;
  return -1;
}
//---------------------------------------------------------------------------
//Called by user to indicate that file have been used
void __fastcall TRecent::FileUsed(const String &FileName)
{
  if(FMaxFiles)
  {
    RemoveMenuItems();//Remove shown menu items
    ReadFromRegistry();//Read data to get changes made by other version of program
    int Index = FileIndex(FileName);//Get index of FileName in list
    if(Index == -1)//If FileName not in list
    {
      if(FileList.size() == static_cast<unsigned>(FMaxFiles))//If list is full
        FileList.pop_back();//Erase last file name in list
    }
    else
      FileList.erase(FileList.begin() + Index);//Remove FileName from list

    FileList.push_front(std::pair<String, TActionClientItem*>(FileName, NULL));//Add FileName to start of list
    SaveToRegistry();//Write new file list to registry
    ShowMenuItems();//Show new list of menu items
  }
  
  //Add Filename to recent documents list in the start menu
  if(FAddToRecentDocs)
    SHAddToRecentDocs(SHARD_PATH, FileName.c_str());
}
//---------------------------------------------------------------------------
//This function is called then one of the menu items is selected
void __fastcall TRecent::MenuClick(TObject *Sender)
{
  int Index = ObjectIndex(Sender);    //Get index of selected menu item in list
  String FileName = FileList[Index].first; //Get file name coresponding to menu item
  if(FOnLoadFile)                               //If event handler sat
    if(FOnLoadFile(this, FileName))             //Call user selected event handler with FileName as parameter
      FileUsed(FileName);                       //Move FileName to first position in file list
}
//---------------------------------------------------------------------------
static String CompactPath(const String &Path, unsigned MaxLen)
{
  std::vector<wchar_t> Str(MaxLen + 1);
  PathCompactPathEx(&Str[0], Path.c_str(), MaxLen+1, 0);
  return &Str[0];
}
//---------------------------------------------------------------------------
//This function shows file names in the menu
void __fastcall TRecent::ShowMenuItems(void)
{
  if(!FFileMenu || FileList.empty() || !Enabled)//If nowhere to show or nothing to show
    return;

  Seperator1 = FFileMenu->Add();
  Seperator1->Caption = '-';//Make seperator

  for(unsigned I = 0; I < FileList.size(); I++)
  {
    //Loop through all file names
    //Add menu item at the bottom
    TActionClientItem *MenuItem = FFileMenu->Add();
    TOpenFileAction *Action = new TOpenFileAction(FileList[I].first, this);
    MenuItem->Action = Action;
    MenuItem->Caption = "&" + String(I+1) + " " + CompactPath(FileList[I].first, FMaxPathLen);
    //Set hint and substitute %s with the filename
    Action->Hint = ReplaceStr(FHint, "%s", FileList[I].first);
    FileList[I].second = MenuItem;//Add pointer to menu item to FileList
  }
}
//---------------------------------------------------------------------------
//This function removes all menu items added by this component
void __fastcall TRecent::RemoveMenuItems()
{
  if(!FFileMenu || FileList.empty())//If no FileMenu or no file names to show
    return;

  if(Seperator1)
  {
    FFileMenu->Delete(Seperator1->Index);//Remove seperator before menu items
    Seperator1 = NULL;
  }

  for(unsigned I = 0; I < FileList.size(); I++)//Loop through each made menu item
  {
    if(FileList[I].second)
    {
      TActionClientItem *Item = FileList[I].second;
      delete Item->Action; //This will also remove the TActionClientItem
    }
    FileList[I].second = NULL;//File name no more have a menu item attached
  }

  if(Seperator2)
  {
    FFileMenu->Delete(Seperator2->Index);//Remove seperator after menu items
    Seperator2 = NULL;
  }
}
//---------------------------------------------------------------------------
//This function reads file names of recent used files from the registry
void __fastcall TRecent::ReadFromRegistry(void)
{
  if(ComponentState.Contains(csDesigning))//If design time
    return;//don't load registry

  FileList.clear();//Make sure there are no files in list
  if(FRegistryKey.IsEmpty())//If no registry key set
    return;//Don't read data from registry

  std::auto_ptr<TRegistry> Registry(new TRegistry());
  if(Registry->OpenKeyReadOnly(FRegistryKey))
    for(unsigned I = 1; I <= FMaxFiles; I++)//Loop through all values in registry
      if(Registry->ValueExists("Recent" + String(I)))
        FileList.push_back(std::pair<String,TActionClientItem*>(Registry->ReadString("Recent" + String(I)), NULL));//Add file name to list
}
//---------------------------------------------------------------------------
//This function saves file names to the registry
void __fastcall TRecent::SaveToRegistry(void)
{
  if(ComponentState.Contains(csDesigning))//If design time
    return;//don't write to registry

  if(FRegistryKey.IsEmpty())//If no registry key choosen
    return;//Don't save data

  std::auto_ptr<TRegistry> Registry(new TRegistry);
  if(Registry->OpenKey(FRegistryKey, true))
  {
    //Open key;Create key if not found
    for(unsigned I = 0; I < FileList.size(); I++)//Loop through recent files in list
      Registry->WriteString("Recent" + String(I+1), FileList[I].first);//Write file name to registry

    for(unsigned I = FileList.size(); Registry->ValueExists("Recent" + String(I+1)); I++)//Loop through until no more fake values
      Registry->DeleteValue("Recent"+String(I+1));//Delete file name
  }
}
//---------------------------------------------------------------------------
void __fastcall TRecent::SetEnabled(bool AEnabled)
{
  if(FEnabled != AEnabled)
  {
    FEnabled = AEnabled;
    if(FEnabled)
      ShowMenuItems();
    else
      RemoveMenuItems();
  }
}
//---------------------------------------------------------------------------
//WARNING: Do not change the argument to a reference. It will crash the IDE
void __fastcall TRecent::SetHint(String Str)
{
  FHint = Str;
  for(unsigned I = 0; I < FileList.size(); I++)
    if(FileList[I].second)
      static_cast<TCustomAction*>(FileList[I].second->Action)->Hint = ReplaceStr(FHint, "%s", FileList[I].first);
}
//---------------------------------------------------------------------------

