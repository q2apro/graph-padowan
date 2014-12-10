//===========================================================================
// Copyright © 2003 Ivan Johansen
// TRecent.h
//===========================================================================
#ifndef TRecentH
#define TRecentH
#include <ActnMan.hpp>
#include <deque>
//---------------------------------------------------------------------------
#define MaxRecentFiles 9 //Max number of recent files that can be shown
//---------------------------------------------------------------------------
class TRecent;
//Don't use reference. The String may be used afterwards
typedef bool __fastcall(__closure *LoadFileEvent)(TRecent *Sender, String FileName);

class TRecent : public TComponent
{
private:
    unsigned FMaxFiles;			//Number of recent files to show
    bool FEnabled;
    String FRegistryKey;//Registry key to store data
    LoadFileEvent FOnLoadFile;//Event to call then a recent file menu is pressed
    TActionClients *FFileMenu;
    TActionClientItem *Seperator1, *Seperator2;//Seperator before and after recent files in menu
    String FHint;
    bool FAddToRecentDocs;
    int FMaxPathLen;
    std::deque<std::pair<String,TActionClientItem*> > FileList;

    void __fastcall SetMaxFiles(unsigned Value);
    void __fastcall SetRegistryKey(const String &Value);
    void __fastcall SetFileMenu(TActionClients *Value);
    void __fastcall MenuClick(TObject *Sender);
    void __fastcall ShowMenuItems();
    void __fastcall RemoveMenuItems();
    void __fastcall ReadFromRegistry();
    void __fastcall SaveToRegistry();
    void __fastcall SetEnabled(bool AEnabled);
    void __fastcall SetHint(String Str);
    int FileIndex(const String &FileName);
    int ObjectIndex(TObject *Object);

protected:
public:
    __fastcall TRecent(TComponent* Owner);
    __fastcall ~TRecent();
    void __fastcall FileUsed(const String &FileName);
__published:
    __property unsigned MaxFiles={read=FMaxFiles,write=SetMaxFiles,default=4};
    __property String RegistryKey={read=FRegistryKey,write=SetRegistryKey};
    __property LoadFileEvent OnLoadFile={read=FOnLoadFile,write=FOnLoadFile};
    __property TActionClients *FileMenu = {read=FFileMenu, write=SetFileMenu};
//The hint property in the recent menu items is set to this hint property
//%s in the string is substituted by the file name
    __property String Hint = {read=FHint, write=SetHint};
    __property bool AddToRecentDocs={read=FAddToRecentDocs,write=FAddToRecentDocs,default=true};
    __property bool Enabled = {read=FEnabled, write=SetEnabled, default=true};
    __property int MaxPathLen = {read=FMaxPathLen, write=FMaxPathLen};
};
//---------------------------------------------------------------------------
#endif
