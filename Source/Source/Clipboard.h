/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef ClipbardH
#define ClipbardH
//---------------------------------------------------------------------------
class TGraphClipboard
{
  unsigned ClipboardFormat;
  unsigned OldClipboardFormat;
  unsigned PngFormat;

  void SetClipboardData(unsigned Format, const void *Data, unsigned DataSize);
  bool GetClipboardData(unsigned Format, void *Data, unsigned DataSize);
  int GetClipboardDataSize(unsigned Format);

public:
  TGraphClipboard();
  void Copy(const TData &Data, const TGraphElemPtr &Elem);
  void Paste(TData &Data);
  bool HasData();
  void CopyPngData(Graphics::TBitmap *Bitmap);
};

extern TGraphClipboard GraphClipboard;

#endif
