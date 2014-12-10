//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include "SerialHandler.h"
#include "SerialThread.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
TSerialThread::TSerialThread(TSerialHandler *ASerialHandler)
  : TIThread(false), SerialHandler(ASerialHandler)
{
}
//---------------------------------------------------------------------------
void __fastcall TSerialThread::Execute()
{
  Overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  SetName("Serial thread");
  SetCommMask(SerialHandler->Handle.Get(), EV_BREAK | EV_RXCHAR);
  std::vector<BYTE> Data;

  while(!Terminated)
  {
    try
    {
      DWORD EvtMask;
      DWORD Dummy;
      if(!WaitCommEvent(SerialHandler->Handle.Get(), &EvtMask, &Overlapped))
        Win32Check(GetOverlappedResult(SerialHandler->Handle.Get(), &Overlapped, &Dummy, TRUE));

      if(EvtMask & EV_BREAK)
      {
        if(SerialHandler->Synchronized)
          Synchronize(&SerialHandler->DoBreak);
        else
          SerialHandler->DoBreak();
      }

      if(EvtMask & EV_RXCHAR)
      {
        //Retrieve number of bytes waiting
        COMSTAT ComStat;
        Win32Check(ClearCommError(SerialHandler->Handle.Get(), &Dummy, &ComStat));

        if(ComStat.cbInQue > 0)
        {
          //Resize vector and read data
          Data.resize(ComStat.cbInQue);
          
          DWORD BytesRead;
          if(!ReadFile(SerialHandler->Handle.Get(), &Data[0], Data.size(), &BytesRead, &Overlapped))
            Win32Check(GetOverlappedResult(SerialHandler->Handle.Get(), &Overlapped, &Dummy, TRUE));

          //Send data to main thread
          if(SerialHandler->Synchronized)
            Synchronize(&SerialHandler->DoDataReceived, Data);
          else
            SerialHandler->DoDataReceived(Data);  
        }
      }

      if(EvtMask & EV_TXEMPTY)
      {
        if(SerialHandler->Synchronized)
          Synchronize(&SerialHandler->DoTransmissionFinished);
        else
          SerialHandler->DoTransmissionFinished();
      }
    }
    catch(Exception &E)
    {
      Application->MessageBox(E.Message.c_str(), L"Exception", MB_ICONSTOP);
    }
    catch(...)
    {
      Application->MessageBox(L"Unknown exception in thread", L"Exception", MB_ICONSTOP);
    }
  }
  CloseHandle(Overlapped.hEvent);
}
//---------------------------------------------------------------------------
void TSerialThread::Terminate()
{
  TThread::Terminate();
  SetEvent(Overlapped.hEvent); //To terminate GetOverlappedResult()
}
//---------------------------------------------------------------------------

