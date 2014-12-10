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
#include "StackTrace.h"
#include <fstream>
#include <iomanip>
#undef _ASSERTE //Prevent definition warning
#include <crtdbg.h>       //Declares _ErrorMessage()
#include "VersionInfo.h"
#include <cstdio>
#include <delayimp.h>
#include <except.h>
//---------------------------------------------------------------------------
struct TStackFrame
{
  DWORD CallersEBP;
  DWORD CallerAdr;
};
struct TStackInfo
{
  DWORD CallerAdr;
  DWORD Level;
  DWORD CallersEBP;
  DWORD DumpSize;
  DWORD ParamSize;
  DWORD *ParamPtr;
  TStackFrame *StackFrame;
  DWORD Data[20];
};

typedef Exception* (__fastcall *TGetExceptionObject)(Windows::PExceptionRecord P);

static __thread std::vector<TStackInfo> *GlobalStackInfo = NULL; //Must be a pointer because __thread does not work with classes
static String LogFileName = ChangeFileExt(Application->ExeName, ".err");
static bool LogAllExceptions = false;
static TGetExceptionObject OldExceptObjProc = NULL;
//---------------------------------------------------------------------------
//Called when delay loading a DLL failes
FARPROC WINAPI DllLoadFailure(dliNotification dliNotify, DelayLoadInfo *pdli)
{
  if(dliNotify == dliFailGetProcAddress)
    throw EDllLoadError("Failed to get address of: " + pdli->dlp.fImportByName ? String(pdli->dlp.szProcName) : IntToStr((int)pdli->dlp.dwOrdinal));
  throw EDllLoadError("Failed to load " + String(pdli->szDll));
}
//---------------------------------------------------------------------------
/** Write stack trace from StackInfoList to Stream.
 */
void WriteStackTrace(std::ostream &Stream, const std::vector<TStackInfo> &StackInfoList)
{
  Stream << "StackTrace:" << std::endl;
  for(unsigned I = 0; I < StackInfoList.size(); I++)
  {
    const TStackInfo &Info = StackInfoList[I];
    Stream << std::dec << std::setw(2) << std::setfill('0') << Info.Level << ": ";
    Stream << std::hex << std::uppercase << std::setw(8) << Info.CallerAdr;
    Stream << "    Params: " << std::dec << std::setw(2) << Info.ParamSize << "  [" << std::hex;
    for(unsigned J = 0; J < std::min(static_cast<unsigned>(Info.ParamSize), sizeof(Info.Data)/sizeof(Info.Data[0])); J++)
    {
      if(J > 0)
        Stream << ", ";
      Stream << std::setw(8) << Info.Data[J];
    }
    Stream << "]" << std::endl;
  }
}
//---------------------------------------------------------------------------
void WriteStackTrace(std::ostream &Stream, const std::vector<TStackInfo> *&StackInfoList)
{
  if(StackInfoList == NULL)
    return;
  WriteStackTrace(Stream, *StackInfoList);
  delete StackInfoList;
  StackInfoList = NULL;
}
//---------------------------------------------------------------------------
/** Log an uncaught exception pointed to by Exception to a log file.
 *  This function is supposed to be called from an OnException handler.
 */
void LogUncaughtException(TObject *Sender, Exception *E)
{
  std::ofstream File(ChangeFileExt(Application->ExeName, ".err").c_str(), std::ios_base::app);
  File << "UNCAUGHT EXCEPTION\n";
  File << "Version: " << ToString(TVersionInfo().FileVersion().Text()) << std::endl;
  File << "Date: " << DateTimeToStr(Now()) << std::endl;
  File << "Exception: " << E->ClassName() << std::endl;
  File << "Message: " << E->Message << std::endl;
  File << "ExceptAddr: 0x" << std::hex << std::uppercase << ExceptAddr() << std::endl;
  if(Sender)
    File << "SenderClass: " << Sender->ClassName() << std::endl;
  if(TComponent *Component = dynamic_cast<TComponent*>(Sender))
    File << "SenderName: " << Component->Name << std::endl;
  WriteStackTrace(File, GlobalStackInfo);
  File << "-----------------------------------------" << std::endl << std::endl;

  Application->ShowException(E);
}
//---------------------------------------------------------------------------
/** Log uncaught C++ exception to log file.
 *  This function is supposed to be called from a catch handler surrounding a thread.
 *  This should only be used to log C++ exceptions and not Delphi Exceptions.
 */
void LogUncaughtCppException(const char *Message, const char *Thread)
{
  std::ofstream File(ChangeFileExt(Application->ExeName, ".err").c_str(), std::ios_base::app);
  File << "UNCAUGHT C++ EXCEPTION\n";
  File << "Version: " << ToString(TVersionInfo().FileVersion().Text()) << std::endl;
  File << "Date: " << DateTimeToStr(Now()) << std::endl;
  if(Thread)
    File << "Thread: " << Thread << std::endl;
  File << "Exception name: " << __ThrowExceptionName() << std::endl;
  if(Message)
    File << "Message: " << Message << std::endl;
  File << "File: " << __ThrowFileName() << std::endl;
  File << "Line: " << __ThrowLineNumber() << std::endl;
  File << "-----------------------------------------" << std::endl << std::endl;

//  Application->ShowException(E);
}
//---------------------------------------------------------------------------
/** Function to retrieve stack pointer.
 */
#pragma warn -8070  //Disable warning: Function should return a value
DWORD __fastcall GetStackTop()
{
  asm
  {
        MOV     EAX, FS:[4]
  }
}
#pragma warn .8070
//---------------------------------------------------------------------------
/** Check if address is a valid stack address.
 */
bool ValidStackAddr(DWORD StackAddr, DWORD BaseOfStack)
{
  return BaseOfStack < StackAddr && StackAddr < GetStackTop();
}
//---------------------------------------------------------------------------
/** Find instruction size for instruction fround at address given by CodeAddr.
 */
bool ValidCallSite(DWORD CodeAddr, unsigned &CallInstructionSize)
{
  DWORD CodeDWORD4;
  DWORD CodeDWORD8;
  DWORD *C4P;
  DWORD *C8P;

  // First check that the address is within range of our code segment!
  C8P = (DWORD*)(CodeAddr - 8);
  C4P = (DWORD*)(CodeAddr - 4);
  bool Result = (CodeAddr > 8) /*&& ValidCodeAddr(DWORD(C8P), FModuleInfoList)*/ && !IsBadReadPtr(C8P, 8);

  // Now check to see if the instruction preceding the return address
  // could be a valid CALL instruction
  if(Result)
  {
    try
    {
      CodeDWORD8 = *C8P;
      CodeDWORD4 = *C4P;

      // Check the instruction prior to the potential call site.
      // We consider it a valid call site if we find a CALL instruction there
      // Check the most common CALL variants first
      if((CodeDWORD8 & 0xFF000000) == 0xE8000000)  // 5-byte, CALL [-$1234567]
        CallInstructionSize = 5;
      else
      if((CodeDWORD4 & 0x38FF0000) == 0x10FF0000) // 2 byte, CALL EAX
        CallInstructionSize = 2;
      else
      if((CodeDWORD4 & 0x0038FF00) == 0x0010FF00) // 3 byte, CALL [EBP+0x8]
        CallInstructionSize = 3;
      else
      if((CodeDWORD4 & 0x000038FF) == 0x000010FF) // 4 byte, CALL ??
        CallInstructionSize = 4;
      else
      if((CodeDWORD8 & 0x38FF0000) == 0x10FF0000) // 6-byte, CALL ??
        CallInstructionSize = 6;
      else
      if((CodeDWORD8 & 0x0038FF00) == 0x0010FF00) // 7-byte, CALL [ESP-0x1234567]
        CallInstructionSize = 7;
      else
        Result = false;
      // Because we're not doing a complete disassembly, we will potentially report
      // false positives. If there is odd code that uses the CALL 16:32 format, we
      // can also get false negatives.}
    }
    catch(...)
    {
      Result = false;
    }
  }
  return Result;
}
//---------------------------------------------------------------------------
/** Write stack fram info found at StackFrame into StackInfo and move StackFrame to next frame.
 */
bool NextStackFrame(TStackFrame *&StackFrame, TStackInfo &StackInfo, DWORD BaseOfStack)
{
  unsigned CallInstructionSize;

  // Only report this stack frame into the StockInfo structure
  // if the StackFrame pointer, EBP on the stack and return
  // address on the stack are valid addresses
  while(ValidStackAddr(reinterpret_cast<DWORD>(StackFrame), BaseOfStack))
  {
    // CallerAdr within current process space, code segment etc.
    // CallersEBP within current thread stack. Added Mar 12 2002 per Hallvard's suggestion
    if(/*ValidCodeAddr(StackFrame->CallerAdr, FModuleInfoList) &&*/ ValidStackAddr(StackFrame->CallersEBP, BaseOfStack))
    {
      StackInfo.Level++;
      StackInfo.StackFrame = StackFrame;
      StackInfo.ParamPtr = reinterpret_cast<DWORD*>(StackFrame + 1);
      StackInfo.CallersEBP = StackFrame->CallersEBP;
      // Calculate the address of caller by subtracting the CALL instruction size (if possible)
      if(ValidCallSite(StackFrame->CallerAdr, CallInstructionSize))
        StackInfo.CallerAdr = StackFrame->CallerAdr - CallInstructionSize;
      else
        StackInfo.CallerAdr = StackFrame->CallerAdr;
      StackInfo.DumpSize = StackFrame->CallersEBP - DWORD(StackFrame);
      StackInfo.ParamSize = (StackInfo.DumpSize - sizeof(TStackFrame)) / 4;
      // Step to the next stack frame by following the EBP pointer
      StackFrame = (TStackFrame*)StackFrame->CallersEBP;
      memcpy(StackInfo.Data, StackInfo.ParamPtr, sizeof(StackInfo.Data));
      return true;
    }
    // Step to the next stack frame by following the EBP pointer
    StackFrame = (TStackFrame*)StackFrame->CallersEBP;
  }
  return false;
}
//---------------------------------------------------------------------------
/** Write trace of current stack into StackInfoList.
 */
void TraceStackFrames(std::vector<TStackInfo> &StackInfoList, unsigned IgnoreFrames, unsigned Count = 200)
{
  TStackFrame *StackFrame;
  TStackInfo StackInfo;
  // Start at level 0
  StackInfo.Level = 0;
  // Get the current stack frame from the EBP register
  StackFrame = reinterpret_cast<TStackFrame*>(_EBP);
  // We define the bottom of the valid stack to be the current EBP Pointer
  // There is a TIB field called pvStackUserBase, but this includes more of the
  // stack than what would define valid stack frames.
  DWORD BaseOfStack = DWORD(StackFrame) - 1;
  StackInfoList.clear();

  //Ignore the first stack frames
  for(; IgnoreFrames > 0; IgnoreFrames--)
    if(!NextStackFrame(StackFrame, StackInfo, BaseOfStack))
      return;

  // Loop over and report all valid stackframes
  while(Count-- > 0 && NextStackFrame(StackFrame, StackInfo, BaseOfStack))
    StackInfoList.push_back(StackInfo);
}
//---------------------------------------------------------------------------
/** Write trace of current stack to Stream.
 */
void WriteStackFrameToStream(std::ostream &Stream, unsigned IgnoreFrames, unsigned Count = 200)
{
  std::vector<TStackInfo> StackInfoList;
  TraceStackFrames(StackInfoList, IgnoreFrames, Count);
  WriteStackTrace(Stream, StackInfoList);
}
//---------------------------------------------------------------------------
/** Log an OS exception (for example Division by zero) to a log file.
 */
void LogOsException(EExternal *E, unsigned IgnoreFrames)
{
  System::TExceptionRecord *Record = E->ExceptionRecord;
	std::ofstream File(LogFileName.c_str(), std::ios_base::app);
  File << "OS EXCEPTION" << std::endl;
  File << "Version: " << ToString(TVersionInfo().FileVersion().Text()) << std::endl;
  File << "Date: " << DateTimeToStr(Now()) << std::endl;
  File << "Exception: " << E->ClassName() << std::endl;
  File << "Message: " << E->Message << std::endl;
  File << "Exception code: " << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << Record->ExceptionCode << std::endl;
  File << "Exception flags: " << Record->ExceptionFlags << std::endl;
  File << "ExceptionAddress: " << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << Record->ExceptionAddress << std::endl;
  WriteStackFrameToStream(File, IgnoreFrames);
  File << "-----------------------------------------" << std::endl << std::endl;
}
//---------------------------------------------------------------------------
/** Called to raise a Delphi exception.
 */
/*WINBASEAPI*/ VOID WINAPI MyRaiseException(
    DWORD dwExceptionCode,	// exception code
    DWORD dwExceptionFlags,	// continuable exception flag
    DWORD nNumberOfArguments,	// number of arguments in array
    CONST DWORD *lpArguments 	// address of array of arguments
   )
{
  switch(dwExceptionCode)
  {
    case 0x0EEDFADE: //cDelphiException
      if(LogAllExceptions && dwExceptionFlags == EXCEPTION_NONCONTINUABLE && nNumberOfArguments == 7 &&
        (DWORD)lpArguments == (DWORD)&lpArguments + 4)
      {
				if(GlobalStackInfo == NULL)
          GlobalStackInfo = new std::vector<TStackInfo>;
        TraceStackFrames(*GlobalStackInfo, 0);
      }
      break;

    case 0x0EEDFAE4:  //cNonDelphiException
      if(nNumberOfArguments == 2)
        if(EExternal *E = dynamic_cast<EExternal*>(reinterpret_cast<Exception*>(lpArguments[1])))
          LogOsException(E, 6);
      break;

    case 0x0EEDFADF:  //cDelphiReRaise
    case 0x0EEDFAE0:  //cDelphiExcept
    case 0x0EEDFAE1:  //cDelphiFinally
    case 0x0EEDFAE2:  //cDelphiTerminate
    case 0x0EEDFAE3:  //cDelphiUnhandled
    case 0x0EEDFAE5:  //cDelphiExitFinally
    case 0x0EEFFACE:  //cCppException
    default:
      break;
  }
  RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}
//---------------------------------------------------------------------------
/** Called when BOOST_ASSERT fails.
 */
#ifdef BOOST_ENABLE_ASSERT_HANDLER
void boost::assertion_failed(char const * expr, char const * function, char const * file, long line)
{
  std::ofstream File(LogFileName.c_str(), std::ios_base::app);
  File << "ASSERTION FAILED" << std::endl;
  File << "Version: " << ToString(TVersionInfo().FileVersion().Text()) << std::endl;
  File << "Date: " << DateTimeToStr(Now()) << std::endl;
	File << "Expression: " << expr << std::endl;
	if(function)
	  File << "Function: " << function << std::endl;
  File << "File: " << file << std::endl;
  File << "Line: " << line << std::endl;
  WriteStackFrameToStream(File, 0);
  File << "-----------------------------------------" << std::endl << std::endl;

  AnsiString Message = "Assertion failed: " + String(expr) + ", File: " + file + ", Line " + line;
  Message += "\nLogfile " + ExtractFileName(LogFileName) + " created.";
  _ErrorMessage(Message.c_str());
  abort();
}
#endif
//---------------------------------------------------------------------------
void _RTLENTRY _EXPFUNC std::_assert(char * cond, char * file, int line)
{
#ifdef BOOST_ENABLE_ASSERT_HANDLER
	boost::assertion_failed(cond, NULL, file, line);
#endif
}
//---------------------------------------------------------------------------
Exception* __fastcall MyGetExceptionObject(Windows::PExceptionRecord P)
{
  //If a C++ exception was thrown
  if(P->ExceptionCode == 0x0EEFFACE && LogAllExceptions)
  {
    if(GlobalStackInfo == NULL)
      GlobalStackInfo = new std::vector<TStackInfo>;
    TraceStackFrames(*GlobalStackInfo, 6);
  }

  Exception *Result = OldExceptObjProc(P);

  //Stack overflow cannot be handled from MyRaiseException() if running outside the debugger
  if(P->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
    if(EExternal *E = dynamic_cast<EExternal*>(Result))
      LogOsException(E, 3);

	return Result;
}
//---------------------------------------------------------------------------
/** Call at startup to setup handling of exceptions.
 */
void SetApplicationExceptionHandler(bool ALogAllExceptions)
{
//  RaiseExceptionProc = MyRaiseException;
//  OldExceptObjProc = reinterpret_cast<TGetExceptionObject>(ExceptObjProc);
//  ExceptObjProc = MyGetExceptionObject;
	LogAllExceptions = ALogAllExceptions;
	__pfnDliFailureHook = DllLoadFailure;
}
//---------------------------------------------------------------------------


