//---------------------------------------------------------------------------
#pragma hdrstop
#include "../Func32.h"
#include <iostream>
#include <string>
//---------------------------------------------------------------------------
int main()
{
  try
  {
    Func32::TFunc Func;
    std::wcout << L"f(x)=";
    std::wcin >> Func;
    std::wcout << "f'(x)=" << Func.MakeDif() << std::endl;
    std::wcout << "x=";
    std::wstring x;
    std::wcin >> x;
    std::wcout << "f(" << x << ")=" << Func(Func32::Eval(x)) << std::endl;
  }
  catch(Func32::EFuncError &E)
  {
    std::wcerr << "Error " << E.ErrorCode << std::endl;
  }
	return 0;
}
//---------------------------------------------------------------------------

