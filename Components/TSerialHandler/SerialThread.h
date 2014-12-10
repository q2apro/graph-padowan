//---------------------------------------------------------------------------
#ifndef SerialThreadH
#define SerialThreadH
//---------------------------------------------------------------------------
#include "IThread.h"
//---------------------------------------------------------------------------
class TSerialThread : public TIThread
{
  class TSerialHandler *SerialHandler;
  OVERLAPPED Overlapped;

public:
  TSerialThread(TSerialHandler *ASerialHandler);
  void __fastcall Execute();
  void Terminate();
};
#endif
