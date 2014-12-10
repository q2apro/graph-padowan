//---------------------------------------------------------------------------
#ifndef ICompCommonH
#define ICompCommonH
#include <string>
//---------------------------------------------------------------------------
typedef String __fastcall (*TTranslateFunc)(String);

class TRegisterClass
{
public:
	TRegisterClass(TClass Class) {RegisterClass(Class);}
};

String TranslateCompText(const String &Str);
void SetCompTranslateFunc(TTranslateFunc Func);
std::string Trim(const std::string &Str);
std::wstring Trim(const std::wstring &Str);
#endif
