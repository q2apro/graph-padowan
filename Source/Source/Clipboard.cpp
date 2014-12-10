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
#include <PngImage.hpp>
#include <Clipbrd.hpp>
#include "Clipboard.h"
#include "VersionInfo.h"
#include "ConfigFile.h"
//---------------------------------------------------------------------------
TGraphClipboard GraphClipboard;
//---------------------------------------------------------------------------
TGraphClipboard::TGraphClipboard()
{
  ClipboardFormat = RegisterClipboardFormat(CLIPBOARDFORMAT);
  OldClipboardFormat = RegisterClipboardFormat(L"Graph Data");
  PngFormat = RegisterClipboardFormat(L"PNG");
}
//---------------------------------------------------------------------------
bool TGraphClipboard::HasData()
{
  return Clipboard()->HasFormat(ClipboardFormat) || Clipboard()->HasFormat(OldClipboardFormat);
}
//---------------------------------------------------------------------------
void TGraphClipboard::Copy(const TData &Data, const TGraphElemPtr &Elem)
{
  std::wstring Str = Data.SaveToString(Elem);
  SetClipboardData(ClipboardFormat, Str.c_str(), Str.size()*sizeof(wchar_t));
}
//---------------------------------------------------------------------------
void TGraphClipboard::Paste(TData &Data)
{
  if(!HasData())
    return;
  try
  {
    TConfigFile IniFile;
    if(Clipboard()->HasFormat(ClipboardFormat))
    {
      int DataSize = GetClipboardDataSize(ClipboardFormat);
      std::wstring Str(DataSize/sizeof(wchar_t), 0);
      GetClipboardData(ClipboardFormat, &Str[0], DataSize);
      IniFile.LoadFromString(Str);
    }
    else
    {
      int DataSize = GetClipboardDataSize(OldClipboardFormat);
      std::string Str(DataSize, 0);
      GetClipboardData(OldClipboardFormat, &Str[0], DataSize);
      IniFile.LoadFromAnsiString(Str);
    }

    unsigned ElemNo = Data.ElemCount();
    try
    {
      Data.Import(IniFile);
    }
    catch(std::exception &E)
    {
      ShowErrorMsg(E);
      return;
    }

    UndoList.BeginMultiUndo();
    for(unsigned I = ElemNo; I < Data.ElemCount(); I++)
      UndoList.Push(TUndoAdd(Data.GetElem(I)));
    UndoList.EndMultiUndo();
  }
  catch(Func32::EFuncError &Error)
  {
    ShowErrorMsg(Error);
  }
}
//---------------------------------------------------------------------------
void TGraphClipboard::SetClipboardData(unsigned Format, const void *Data, unsigned DataSize)
{
  HGLOBAL DataHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, DataSize);
  if(DataHandle == 0)
    return;
  void *Ptr = GlobalLock(DataHandle);
  memcpy(Ptr, Data, DataSize);
  GlobalUnlock(DataHandle);
  Clipboard()->SetAsHandle(Format, (unsigned)DataHandle);
}
//---------------------------------------------------------------------------
bool TGraphClipboard::GetClipboardData(unsigned Format, void *Data, unsigned DataSize)
{
  Clipboard()->Open();
  HGLOBAL DataHandle = (void*)Clipboard()->GetAsHandle(Format);
  if(DataHandle == NULL || GlobalSize(DataHandle) > DataSize)
  {
    Clipboard()->Close();
    return false;
  }
  void *Ptr = GlobalLock(DataHandle);
  memcpy(Data, Ptr, DataSize);
  GlobalUnlock(DataHandle);
  Clipboard()->Close();
  return true;
}
//---------------------------------------------------------------------------
int TGraphClipboard::GetClipboardDataSize(unsigned Format)
{
  Clipboard()->Open();
  HGLOBAL DataHandle = (void*)Clipboard()->GetAsHandle(Format);
  int Size = GlobalSize(DataHandle);
  Clipboard()->Close();
  return Size;
}
//---------------------------------------------------------------------------
void TGraphClipboard::CopyPngData(Graphics::TBitmap *Bitmap)
{
  std::auto_ptr<TMemoryStream> Stream(new TMemoryStream);
  Bitmap->PixelFormat = pf8bit; //Change bitmap to 8 bit
  std::auto_ptr<TPngImage> PngImage(new TPngImage);
  PngImage->Assign(Bitmap);
  PngImage->SaveToStream(Stream.get());
  SetClipboardData(PngFormat, Stream->Memory, Stream->Size);
}
//---------------------------------------------------------------------------


















