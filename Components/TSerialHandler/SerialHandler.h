//---------------------------------------------------------------------------
#ifndef SerialHandlerH
#define SerialHandlerH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <vector>
//---------------------------------------------------------------------------

enum TParityBit {pbNoParity=NOPARITY, pbOddParity=ODDPARITY, pbEvenParity=EVENPARITY, pbMarkParity=MARKPARITY, pbSpaceParity=SPACEPARITY};
enum TStopBits   {sbOneStopBit=ONESTOPBIT, sbOne5StopBits=ONE5STOPBITS, sbTwoStopBits=TWOSTOPBITS};

class TSerialHandler;
class TSerialThread;
typedef void __fastcall (__closure *TDataReceivedEvent)(TSerialHandler *Sender, const BYTE *Data, unsigned Size);

class PACKAGE TSerialHandler : public TComponent
{
private:
  class THandle
  {
    HANDLE FHandle;
    THandle& operator=(const THandle&);
    THandle(const THandle&);
  public:
    THandle() : FHandle(NULL) {}
    THandle(HANDLE AHandle) : FHandle(AHandle) {}
    ~THandle() {Close();}
    void Swap(THandle &Handle) {HANDLE Temp = FHandle; FHandle = Handle.FHandle; Handle.FHandle = Temp;}
    HANDLE Get() const {return FHandle;}
    void Close() {if(FHandle) CloseHandle(FHandle); FHandle = NULL;}
    bool operator!() const {return !FHandle;}
  };

  friend TSerialThread;

  THandle Handle;
  TSerialThread *Thread;
  OVERLAPPED Overlapped;

  String FPort;
  unsigned FSpeed;
  unsigned FByteSize;
  TParityBit FParity;
  TStopBits FStopBits;
  bool FSynchronized;
  TStringList *FSerialPorts;

  TNotifyEvent FOnBreak;
  TDataReceivedEvent FOnDataReceived;
  TNotifyEvent FOnTransmissionFinished;

  bool GetConnected();
  TStrings* GetSerialPorts();

protected:
  void DoBreak();
  void DoDataReceived(const std::vector<BYTE> &Data);
  void DoTransmissionFinished();
  HANDLE GetHandle();

public:
  __fastcall TSerialHandler(TComponent* Owner);
  __fastcall ~TSerialHandler();
  void Connect();
  void Disconnect();
  void SetBreak();
  void ClearBreak();
  void ClearDTR(); //Clear Data Terminal Ready
  void ClearRTS(); //Clear Request To Send
  void SetDTR(); //Set Data Terminal Ready
  void SetRTS(); //Set Request To Send
  void WriteBuffer(const BYTE *Buffer, unsigned ByteCount, bool Wait=false);
  void WriteString(const AnsiString &Str, bool Wait=false);
  void Purge();

  __property bool Connected = {read=GetConnected};
  __property TStrings* SerialPorts = {read=GetSerialPorts};

__published:
  __property String Port = {read=FPort, write=FPort};
  __property unsigned Speed = {read=FSpeed, write=FSpeed, default=19200};
  __property unsigned ByteSize = {read=FByteSize, write=FByteSize, default=8};
  __property TParityBit Parity = {read=FParity, write=FParity, default=pbNoParity};
  __property TStopBits StopBits = {read=FStopBits, write=FStopBits, default=sbOneStopBit};
  __property bool Synchronized = {read=FSynchronized, write=FSynchronized, default=true};

  __property TNotifyEvent OnBreak = {read=FOnBreak, write=FOnBreak, default=NULL};
  __property TDataReceivedEvent OnDataReceived = {read=FOnDataReceived, write=FOnDataReceived, default=NULL};
  __property TNotifyEvent OnTransmissionFinished = {read=FOnTransmissionFinished, write=FOnTransmissionFinished, default=NULL};
};
//---------------------------------------------------------------------------
#endif
