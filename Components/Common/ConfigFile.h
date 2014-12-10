/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef ConfigFileH
#define ConfigFileH
//---------------------------------------------------------------------------
#include <string>
#include <deque>
#include <iosfwd>
#include <iomanip>
#include <algorithm>

namespace ConfigFileImpl
{
  template<typename T>
  std::wistream& operator>>(std::wistream &Stream, T &Value)
  {
    int TempValue;
    if(Stream >> TempValue)
      Value = static_cast<T>(TempValue);
    return Stream;
  }
}

class TConfigFile;
class TConfigFileSection
{
private:
  struct TCmpString
  {
    const std::wstring &Str;
    TCmpString(const std::wstring &AStr) : Str(AStr) {}
    bool operator()(const std::pair<std::wstring, std::wstring> &Pair) const {return Str == Pair.first;}
    bool operator()(const TConfigFileSection &Section) const {return Str == Section.Name;}
  };

  friend TConfigFile;
  friend TCmpString;
  typedef std::deque<std::pair<std::wstring, std::wstring> > TSection;
  std::wstring Name;
  TSection Section;

  TConfigFileSection() {}
  TConfigFileSection(const std::wstring &AName) : Name(AName) {}
  const std::wstring& Get(const std::wstring &Key) const;

  void Write(const std::wstring&, const std::string&); //Not implemented; Compiler error
  void Write(const std::wstring&, const std::string&, const std::string&); //Not implemented; Compiler error
  void Read(const std::wstring&, const std::string&) const; //Not implemented; Compiler error

public:
  typedef TSection::const_iterator TIterator;

  TIterator Begin() const {return Section.begin();}
  TIterator End() const {return Section.end();}
  const std::wstring GetName() const {return Name;}
  bool KeyExists(const std::wstring &Key) const;
  void DeleteKey(const std::wstring &Key);
  bool Empty() const {return Section.empty();}

  void Write(const std::wstring &Key, const std::wstring &Value);
  void Write(const std::wstring &Key, const std::wstring &Value, const std::wstring &Default);

  template<class T>
  void Write(const std::wstring &Key, const T &Value)
  {
    std::wostringstream Stream;
    Stream << std::setprecision(15) << std::uppercase << Value;
    Write(Key, Stream.str());
  }

  template<class T>
  void Write(const std::wstring &Key, const T &Value, const T &Default)
  {
    if(Value == Default)
      DeleteKey(Key);
    else
      Write(Key, Value);
  }

  template<class T>
  T Read(const std::wstring &Key, const T &Default) const
  {
    try
    {
      using namespace ConfigFileImpl;
      const std::wstring &Str = Get(Key);
      if(!Str.empty())
      {
        std::wistringstream Stream(Str);
        T Value;
        if(Stream >> Value)
          return Value;
      }
    }
    //Just return default if any exceptions occur
    catch(...)
    {
    }
    return Default;
  }

  const std::wstring& Read(const std::wstring &Key, const std::wstring &Default) const;
  std::wstring Read(const std::wstring &Key, const wchar_t* Default) const;
};

class TConfigFile
{
  typedef std::deque<TConfigFileSection> TConfigData;
  typedef TConfigFileSection::TCmpString TCmpString;

  TConfigData ConfigData;
  std::wstring Comment;

  static std::wstring TrimString(const std::wstring &Str);

public:
  TConfigFile() {}
  TConfigFile(const std::wstring &Str);
  bool LoadFromAnsiFile(const std::wstring &FileName);
  bool LoadFromUtf8File(const std::wstring &FileName);
  bool SaveToUtf8File(const std::wstring &FileName) const;
  void LoadFromStream(std::wistream &Stream);
  void SaveToStream(std::wostream &Stream) const;
  void LoadFromString(const std::wstring &Str);
  void LoadFromAnsiString(const std::string &Str);
  std::wstring GetAsString() const;
  void SetComment(const std::wstring &Str);
  void Clear() {ConfigData.clear();}
  void DeleteSection(const std::wstring &Section);
  bool SectionExists(const std::wstring &Section) const;

  unsigned SectionCount() const {return ConfigData.size();}
  const TConfigFileSection& Section(unsigned Index) const {return ConfigData.at(Index);}
  TConfigFileSection& Section(const std::wstring &ASection);
  const TConfigFileSection& Section(const std::wstring &ASection) const;
};


#endif
