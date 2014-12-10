/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef VersionInfoH
#define VersionInfoH
#include <string>
//---------------------------------------------------------------------------
class TVersion
{
  void Init(const wchar_t *Str);

public:
  union
  {
    struct
    {
      unsigned short Minor;
      unsigned short Major;
      unsigned short Build;
      unsigned short Release;
    };
    struct
    {
      unsigned VersionMS;
      unsigned VersionLS;
    };
  };

  TVersion(const wchar_t *Str) {Init(Str);}
  TVersion(const std::wstring &Str) {Init(Str.c_str());}
  TVersion(unsigned short AMajor=0, unsigned short AMinor=0, unsigned short ARelease=0, unsigned short ABuild=0)
   : Major(AMajor), Minor(AMinor), Release(ARelease), Build(ABuild) {}
  unsigned PackedVersion() const;
  std::wstring Text() const;
};

bool operator<(const TVersion &Ver1, const TVersion &Ver2);
bool operator<=(const TVersion &Ver1, const TVersion &Ver2);
bool operator==(const TVersion &Ver1, const TVersion &Ver2);
bool operator!=(const TVersion &Ver1, const TVersion &Ver2);
bool operator>(const TVersion &Ver1, const TVersion &Ver2);
bool operator>=(const TVersion &Ver1, const TVersion &Ver2);
std::wostream& operator<<(std::wostream &Stream, const TVersion &Ver);
std::wistream& operator>>(std::wistream &Stream, TVersion &Ver);

enum TFileFlags {ffNone=0, ffDebug=1, ffPatched=2, ffPrerelease=4, ffPrivateBuild=8, ffSpecialBuild=16};

class TVersionInfo
{
private:
  std::wstring FFileName;
  TVersion FFileVersion;
  TVersion FProductVersion;
  TFileFlags FFileFlags;
  unsigned int FFileType;
  unsigned int FFileSubtype;
  bool FInfoAvailable;

  void LoadVersionInfo();

protected:
public:
  TVersionInfo();
  TVersionInfo(const std::wstring &FileName);
  std::wstring StringValue(const std::wstring &Ident) const;
  void SetFileName(const std::wstring &FileName);
  TVersion FileVersion() const {return FFileVersion;}
  TVersion ProductVersion() const {return FProductVersion;}
  TFileFlags FileFlags() const {return FFileFlags;}
  unsigned FileType() const {return FFileType;}
  unsigned FileSubtype() const {return FFileSubtype;}
  bool InfoAvailable() const {return FInfoAvailable;}
  int GetLanguage() const;
  std::wstring GetLanguageName() const;
};

std::wstring GetWindowsVersion();
//---------------------------------------------------------------------------
#endif
