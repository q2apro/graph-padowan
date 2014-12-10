//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include "SerialHandler.h"
#include "SerialThread.h"
#include "ICompCommon.h"
#include <Registry.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TSerialHandler));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TSerialHandler *)
{
  new TSerialHandler(NULL);
}
//---------------------------------------------------------------------------
namespace Serialhandler
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TSerialHandler)};
     RegisterComponents("IComp", classes, 0);   
  }
}
//---------------------------------------------------------------------------
__fastcall TSerialHandler::TSerialHandler(TComponent* Owner)
  : TComponent(Owner), FPort("COM1"), FSpeed(19200), FByteSize(8), FParity(pbNoParity), FStopBits(sbOneStopBit),
    FSynchronized(true), FSerialPorts(new TStringList),
    FOnBreak(NULL), FOnDataReceived(NULL), FOnTransmissionFinished(NULL)
{
}
//---------------------------------------------------------------------------
__fastcall TSerialHandler::~TSerialHandler()
{
  Disconnect();
}
//---------------------------------------------------------------------------
void TSerialHandler::Connect()
{
  if(Handle.Get())   //If already open, don't bother
      return;

  THandle TempHandle(CreateFile(Port.c_str(),
                      GENERIC_READ | GENERIC_WRITE,
                      0,    /* comm devices must be opened w/exclusive-access */
                      NULL, /* no security attrs */
                      OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
                      FILE_FLAG_OVERLAPPED,    /* Overlapped I/O */
                      NULL  /* hTemplate must be NULL for comm devices */
                      ));

  //If CreateFile fails, throw an exception. CreateFile will fail if the
  //port is already open, or if the com port does not exist.
  if(TempHandle.Get() == INVALID_HANDLE_VALUE)
    RaiseLastOSError();

  //Now get the DCB properties of the port we just opened
  DCB dcb;
  Win32Check(GetCommState(TempHandle.Get(), &dcb));

  //dcb contains the actual port properties.  Now copy our settings into this dcb
  dcb.BaudRate  = Speed;
  dcb.ByteSize  = ByteSize;
  dcb.Parity    = Parity;
  dcb.StopBits  = StopBits;

  //Now we can set the properties of the port with our settings.
  Win32Check(SetCommState(TempHandle.Get(), &dcb));

  //Set the intial size of the transmit and receive queues. These are
  //not exposed to outside clients of the class either. Perhaps they should be?
  //I set the receive buffer to 32k, and the transmit buffer to 9k (a default).
  Win32Check(SetupComm(TempHandle.Get(), 1024*32, 1024*9));

  // These values are just default values that I determined empirically.
  // Adjust as necessary. I don't expose these to the outside because
  // most people aren't sure how they work (uhhh, like me included).
  COMMTIMEOUTS TimeOuts;
  TimeOuts.ReadIntervalTimeout         = 15;
  TimeOuts.ReadTotalTimeoutMultiplier  = 1;
  TimeOuts.ReadTotalTimeoutConstant    = 250;
  TimeOuts.WriteTotalTimeoutMultiplier = 1;
  TimeOuts.WriteTotalTimeoutConstant   = 250;
  Win32Check(SetCommTimeouts(TempHandle.Get(), &TimeOuts));

  Handle.Swap(TempHandle);
  Thread = new TSerialThread(this);

  //Create event for use when writing. Needed for overlapped I/O
  Overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}
//---------------------------------------------------------------------------
void TSerialHandler::Disconnect()
{
  if(Thread)
  {
    Thread->Terminate();
    Thread->WaitFor();
    delete Thread;
    Thread = NULL;
    CloseHandle(Overlapped.hEvent);
  }
  Handle.Close();
}
//---------------------------------------------------------------------------
bool TSerialHandler::GetConnected()
{
  return Handle.Get();
}
//---------------------------------------------------------------------------
void TSerialHandler::SetBreak()
{
  Win32Check(SetCommBreak(GetHandle()));
}
//---------------------------------------------------------------------------
void TSerialHandler::ClearBreak()
{
  Win32Check(ClearCommBreak(GetHandle()));
}
//---------------------------------------------------------------------------
void TSerialHandler::DoBreak()
{
  if(FOnBreak)
    FOnBreak(this);
}
//---------------------------------------------------------------------------
HANDLE TSerialHandler::GetHandle()
{
  if(!Handle)
    throw Exception("Serial port is not open");
  return Handle.Get();  
}
//---------------------------------------------------------------------------
void TSerialHandler::WriteBuffer(const BYTE *Buffer, unsigned ByteCount, bool Wait)
{
  DWORD Dummy;
  if(ByteCount == 0 || Buffer == NULL)
    return;

  WriteFile(GetHandle(), Buffer, ByteCount, &Dummy, &Overlapped);
  if(Wait)
    Win32Check(GetOverlappedResult(GetHandle(), &Overlapped, &Dummy, TRUE));
}
//---------------------------------------------------------------------------
void TSerialHandler::WriteString(const AnsiString &Str, bool Wait)
{
  WriteBuffer(Str.c_str(), Str.Length(), Wait);
}
//---------------------------------------------------------------------------
void TSerialHandler::DoDataReceived(const std::vector<BYTE> &Data)
{
  if(FOnDataReceived)
    FOnDataReceived(this, &Data[0], Data.size());
}
//---------------------------------------------------------------------------
void TSerialHandler::Purge()
{
  Win32Check(PurgeComm(GetHandle(), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR));
}
//---------------------------------------------------------------------------
void TSerialHandler::DoTransmissionFinished()
{
  if(FOnTransmissionFinished)
    FOnTransmissionFinished(this);
}
//---------------------------------------------------------------------------
void TSerialHandler::ClearDTR()
{
  Win32Check(EscapeCommFunction(GetHandle(), CLRDTR));
}
//---------------------------------------------------------------------------
void TSerialHandler::ClearRTS()
{
  Win32Check(EscapeCommFunction(GetHandle(), CLRRTS));
}
//---------------------------------------------------------------------------
void TSerialHandler::SetDTR()
{
  Win32Check(EscapeCommFunction(GetHandle(), SETDTR));
}
//---------------------------------------------------------------------------
void TSerialHandler::SetRTS()
{
  Win32Check(EscapeCommFunction(GetHandle(), SETRTS));
}
//---------------------------------------------------------------------------
TStrings* TSerialHandler::GetSerialPorts()
{
  if(FSerialPorts->Count == 0)
  {
    std::auto_ptr<TRegistry> Registry(new TRegistry);
    Registry->RootKey = HKEY_LOCAL_MACHINE;
    if(Registry->OpenKeyReadOnly("Hardware\\DeviceMap\\SerialComm"))
    {
      std::auto_ptr<TStrings> Strings(new TStringList);
      Registry->GetValueNames(Strings.get());
      for(int I = 0; I < Strings->Count; I++)
        FSerialPorts->Add(Registry->ReadString(Strings->Strings[I]));
    }
  }
  return FSerialPorts;
}
//---------------------------------------------------------------------------

