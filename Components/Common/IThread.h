//---------------------------------------------------------------------------
#ifndef IThreadH
#define IThreadH
//---------------------------------------------------------------------------
#include <SyncObjs.hpp>
#include <string>

//Used to name the thread in the debugger;
//Warning: Thread must be running before name can be set; else it will be ignored
inline void SetThreadName(const std::string &Name, DWORD ThreadID = -1)
{
  struct THREADNAME_INFO
  {
    DWORD dwType;  // must be 0x1000
    LPCSTR szName; // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags; // reserved for future use, must be zero
  };

  THREADNAME_INFO Info;
  Info.dwType = 0x1000;
  Info.szName = Name.c_str();
  Info.dwThreadID = ThreadID;
  Info.dwFlags = 0;
  __try
  {
    RaiseException( 0x406D1388, 0, sizeof(Info)/sizeof(DWORD),(DWORD*)&Info );
  }
  __except(EXCEPTION_CONTINUE_EXECUTION)
  {
  }
}

class TIThread : public TThread
{
  bool FFinished;        //Set to true when the thread has finished

  template<typename TClassMethod>
  class TSynchronizeHelper
  {
    TClassMethod Method;
  public:
    TSynchronizeHelper(TClassMethod AMethod) : Method(AMethod){}
    void __fastcall Synchronized() {Method();}
  };

  template<typename T, typename TClassMethod>
  class TSynchronizeHelper1
  {
    TClassMethod Method;
    T &Data;
  public:
    TSynchronizeHelper1(TClassMethod AMethod, T &AData) : Method(AMethod), Data(AData) {}
    void __fastcall Synchronized() {Method(Data);}
  };

  void __fastcall DoTerminate()
  {
    FFinished = true;
    TThread::DoTerminate();
  }

protected:
  TIThread(bool CreateSuspended = false) : TThread(CreateSuspended), FFinished(false) {}
//  using TThread::Synchronize;

  template<typename TClassMethod> void Synchronize(TClassMethod Method)
  {
    TSynchronizeHelper<TClassMethod> Helper(Method);
    TThread::Synchronize(&Helper.Synchronized);
  }

  template<typename T, typename TClassMethod> void Synchronize(TClassMethod Method, const T &Ref)
  {
    TSynchronizeHelper1<const T, TClassMethod> Helper(Method, Ref);
    TThread::Synchronize(&Helper.Synchronized);
  }

  template<typename T, typename TClassMethod> void Synchronize(TClassMethod Method, T &Ref)
  {
    TSynchronizeHelper1<T, TClassMethod> Helper(Method, Ref);
    TThread::Synchronize(&Helper.Synchronized);
  }

  bool GetMessage(TMessage &Message)
  {
    MSG Msg;
    BOOL Result = ::GetMessage(&Msg, NULL, 0, 0);
    Message.Msg = Msg.message;
    Message.WParam = Msg.wParam;
    Message.LParam = Msg.lParam;
    return Result;
  }

  void ClearMessageQueue()
  {
    MSG Msg;
    while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE));
  }

  bool HasMessage()
  {
    MSG Msg;
    return PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE);
  }

public:
  void SetName(const std::string &Name)
  {
    SetThreadName(Name, ThreadID);
  }

  //WARNING: Msg must not be below WM_USER
  bool PostMessage(unsigned Msg, unsigned WParam=0, unsigned LParam=0)
  {
    return PostThreadMessage(ThreadID, Msg, WParam, LParam);
  }

  __property bool Finished={read=FFinished};
};
//---------------------------------------------------------------------------
class TIEvent
{
  HANDLE Handle;
  TIEvent(const TIEvent&);                  //Not defined
  const TIEvent& operator=(const TIEvent&); //Not defined

public:
  TIEvent(bool ManualReset=true, bool InitialState=false)
  {
    SECURITY_ATTRIBUTES Attributes = {sizeof(SECURITY_ATTRIBUTES), NULL, false};
    Handle = CreateEvent(&Attributes, ManualReset, InitialState, NULL);
  }

  ~TIEvent()
  {
    CloseHandle(Handle);
  }

  void SetEvent()
  {
    ::SetEvent(Handle);
  }

  void ResetEvent()
  {
    ::ResetEvent(Handle);
  }

  void PulseEvent()
  {
    ::PulseEvent(Handle);
  }

  bool TestEvent()
  {
    return WaitForSingleObject(Handle, 0) == WAIT_OBJECT_0;
  }

  //Timeout is in milliseconds
  //Note: Timeout is not a real timeout. It may take longer time if Synchronize()
  //messages are handled. When it is the timeout from the last handled message
  TWaitResult WaitFor(unsigned Timeout = INFINITE)
  {
    if(GetCurrentThreadId() != MainThreadID)
      return WaitForSingleObject(Handle, Timeout) == WAIT_TIMEOUT	? wrTimeout : wrSignaled;

    unsigned WaitResult = 0;
    MSG Msg;
    do
    {
      PeekMessage(&Msg, 0, 0, 0, PM_NOREMOVE);
      ::Sleep(0);
      CheckSynchronize();
      WaitResult = MsgWaitForMultipleObjects(1, &Handle, false, Timeout, QS_POSTMESSAGE);
      Win32Check(WaitResult != WAIT_FAILED);
    } while(WaitResult == WAIT_OBJECT_0 + 1);

    return WaitResult == WAIT_TIMEOUT ? wrTimeout : wrSignaled;
  }
};
//---------------------------------------------------------------------------
class TMutex
{
  HANDLE Handle;
  const TMutex& operator=(const TMutex&);
  TMutex(const TMutex&);

public:
  TMutex(const Char *Name, bool InitialOwner = false)
  {
    Handle = CreateMutex(NULL, InitialOwner, Name);
    if(Handle == NULL)
      RaiseLastOSError();
  }

  ~TMutex()
  {
    CloseHandle(Handle);
  }
};
//---------------------------------------------------------------------------
//Creates a new thread and calls Method with Arg as parameter in the context of the new thread
//When finsihed Method2  is called in the context of the main thread
template<typename TMethod, typename TArg, typename TMethod2>
void CallFromThread(TMethod Method, const TArg &Arg, TMethod2 Method2)
{
  class TLocalThread : public TThread
  {
    TMethod Method;
    TArg Arg;
    TMethod2 Method2;

    void __fastcall Execute()
    {
      Method(Arg);
    }

    //This function is called in the context of the main thread
    void __fastcall Finished(TObject*)
    {
      Method2();
    }

  public:
    TLocalThread(TMethod AMethod, const TArg &AArg, TMethod2 AMethod2)
      : TThread(false), Method(AMethod), Arg(AArg), Method2(AMethod2)
    {
      FreeOnTerminate = true;
      if(Method2)
        OnTerminate = Finished;
		}
  };

  new TLocalThread(Method, Arg, Method2);
}
//---------------------------------------------------------------------------
//Creates a new thread and calls Method with Arg as parameter in the context of the new thread
template<typename TMethod, typename TArg>
void CallFromThread(TMethod Method, const TArg &Arg)
{
  CallFromThread(Method, Arg, static_cast<void (*)()>(NULL));
}
//---------------------------------------------------------------------------
#endif
