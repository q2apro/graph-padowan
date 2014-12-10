/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2005 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef ConfigRegistryH
#define ConfigRegistryH
//---------------------------------------------------------------------------
#include <string>

struct ERegistryError : std::exception
{
  unsigned ErrorCode;
  std::string ErrorStr;
  const char* what() const throw() {return ErrorStr.c_str();}
  ERegistryError(unsigned AErrorCode, const std::string &AErrorStr)
    : ErrorCode(AErrorCode), ErrorStr(AErrorStr) {}
};

class TConfigRegistry
{
  HKEY Handle;

  static std::wstring GetKey(const std::wstring &Key);
  unsigned GetValueSize(const std::wstring &Name, unsigned ValueType = REG_NONE) const;
  unsigned GetValueSize(const std::string &Name, unsigned ValueType = REG_NONE) const;

public:
  TConfigRegistry() : Handle(NULL) {}
  TConfigRegistry(const std::wstring &Key, HKEY RootKey = HKEY_CURRENT_USER, bool Create = false);
  ~TConfigRegistry() {CloseKey();}

  HKEY CurrentKey() const {return Handle;}
  bool CreateKey(const std::wstring &Key, HKEY RootKey = HKEY_CURRENT_USER);
  bool OpenKey(const std::wstring &Key, HKEY RootKey = HKEY_CURRENT_USER, bool ReadOnly = true);
  void CloseKey();
  bool IsOpen() const {return Handle;}

  void Write(const std::string &Name, const std::string &Value);
  void Write(const std::wstring &Name, const std::wstring &Value);
  void Write(const std::wstring &Name, int Value);
  void Write(const std::string &Name, int Value);

  std::wstring Read(const std::wstring &Name, const std::wstring &Default) const;
  std::wstring Read(const std::wstring &Name, const wchar_t *Default) const {return Read(Name, std::wstring(Default));}
  int Read(const std::wstring &Name) const;
  int Read(const std::wstring &Name, int Default) const;
  bool Read(const std::wstring &Name, bool Default) const;
  unsigned Read(const std::wstring &Name, unsigned Default) const;

  int Read(const std::string &Name) const;
  int Read(const std::string &Name, int Default) const;
  bool Read(const std::string &Name, bool Default) const {return Read(Name, static_cast<int>(Default));}
  unsigned Read(const std::string &Name, unsigned Default) const {return Read(Name, static_cast<int>(Default));}

  template<typename T> //T must be an enum
  T Read(const std::string &Name, const T &Default) const {return static_cast<T>(Read(Name, static_cast<int>(Default)));}
  bool ValueExists(const std::wstring &Name) const {return GetValueSize(Name) != 0;}

  static bool KeyExists(const std::wstring &Key, HKEY RootKey = HKEY_CURRENT_USER);
  void DeleteKey(const std::wstring &Key);
  unsigned NumSubKeys() const;
  std::wstring SubKey(unsigned Index);
};

void RemoveRegistryKey(const std::wstring &Key, HKEY RootKey);
std::wstring GetRegValue(const std::wstring &Key, const std::wstring &ValueName, HKEY RootKey, const std::wstring &Default);
unsigned GetRegValue(const std::wstring &Key, const std::wstring &ValueName, HKEY RootKey, unsigned Default);
bool RegKeyExists(const std::wstring &Key, HKEY RootKey);
void SetRegValue(const std::wstring &Key, const std::wstring &ValueName, HKEY RootKey, const std::wstring &Value);
void SetRegValue(const std::wstring &Key, const std::wstring &ValueName, HKEY RootKey, unsigned Value);
#endif
