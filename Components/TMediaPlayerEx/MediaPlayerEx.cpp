//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MediaPlayerEx.h"
#include <digitalv.h>
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TMediaPlayerEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TMediaPlayerEx *)
{
  new TMediaPlayerEx(NULL);
}
//---------------------------------------------------------------------------
namespace Mediaplayerex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TMediaPlayerEx)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TMediaPlayerEx::TMediaPlayerEx(TComponent* Owner)
  : TComponent(Owner), FOnSignal(NULL), FRepeat(false), FReverse(false), FDisplay(NULL), FOnNotify(NULL),
    FNotify(false), DeviceID(0), Handle(0), OldWindowProc(NULL), FShareable(false), FDeviceType(dtAutoSelect),
    FFrom(-1), FTo(-1), FSpeed(1000)
{
}
//---------------------------------------------------------------------------
__fastcall TMediaPlayerEx::~TMediaPlayerEx()
{
  Close();
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::SetSignal(unsigned At, unsigned Interval)
{
  CheckIfOpen();
  MCI_DGV_SIGNAL_PARMS SignalParm;
  SignalParm.dwCallback = Handle;
  SignalParm.dwPosition = At;
  SignalParm.dwPeriod = Interval;
  SignalParm.dwUserParm = 0;

  DWORD Flags = MCI_WAIT | MCI_DGV_SIGNAL_AT | MCI_DGV_SIGNAL_POSITION;
  if(Interval != 0)
    Flags |= MCI_DGV_SIGNAL_EVERY;
  mciSendCommand(DeviceID, MCI_SIGNAL, Flags, reinterpret_cast<DWORD>(&SignalParm));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::RemoveSignal(unsigned At)
{
  CheckIfOpen();
  MCI_DGV_SIGNAL_PARMS SignalParm;
  SignalParm.dwCallback = Handle;
  SignalParm.dwUserParm = At;

  mciSendCommand(DeviceID, MCI_SIGNAL, MCI_WAIT | MCI_DGV_SIGNAL_CANCEL | MCI_DGV_SIGNAL_USERVAL, reinterpret_cast<DWORD>(&SignalParm));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::RemoveAllSignals()
{
  CheckIfOpen();
  MCI_DGV_SIGNAL_PARMS SignalParm;
  SignalParm.dwCallback = Handle;

  mciSendCommand(DeviceID, MCI_SIGNAL, MCI_WAIT | MCI_DGV_SIGNAL_CANCEL, reinterpret_cast<DWORD>(&SignalParm));
}
//---------------------------------------------------------------------------
void __fastcall TMediaPlayerEx::WndProc(Messages::TMessage &Message)
{
  switch(Message.Msg)
  {
    case MM_MCISIGNAL:
      if(OnSignal)
        OnSignal(this, Message.LParam);
      break;

    case MM_MCINOTIFY:
    {
      TMPNotifyValues NotifyValue = nvFailure;
      switch(Message.WParam)
      {
        case MCI_NOTIFY_SUCCESSFUL: NotifyValue = nvSuccessful; break;
        case MCI_NOTIFY_SUPERSEDED: NotifyValue = nvSuperseded; break;
        case MCI_NOTIFY_ABORTED:    NotifyValue = nvAborted; break;
      }
      if(OnNotify)
        OnNotify(this, NotifyValue);
      break;
    }
    case WM_PAINT:
      if(DeviceID == 0 || Display == NULL)
      {
        if(OldWindowProc)
          OldWindowProc(Message);
      }
      else
      {
        PAINTSTRUCT Paint;
        MCI_DGV_UPDATE_PARMS UpdateParms = {0};
        UpdateParms.hDC = BeginPaint(Display->Handle, &Paint);
        DWORD Flags = MCI_WAIT | MCI_DGV_UPDATE_HDC | MCI_DGV_UPDATE_PAINT;
        mciSendCommand(DeviceID, MCI_UPDATE, Flags, reinterpret_cast<DWORD>(&UpdateParms));
        EndPaint(Display->Handle, &Paint);
        Message.Result = 0;
      }
      break;

    default:
      if(OldWindowProc)
        OldWindowProc(Message);
      break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMediaPlayerEx::SetDisplay(TWinControl *Control)
{
  if(FDisplay)
    FDisplay->WindowProc = OldWindowProc;

  FDisplay = Control;
  Handle = 0;
  OldWindowProc = NULL;

  if(Control)
  {
    OldWindowProc = Control->WindowProc;
    Control->WindowProc = &WndProc;
    Handle = reinterpret_cast<DWORD>(Control->Handle);
    Control->FreeNotification(this);
  }

  if(DeviceID != 0 && FCapabilities.Contains(mpUsesWindow) && !ComponentState.Contains(csDestroying))
  {
    MCI_DGV_WINDOW_PARMS AWindowParm;
    DWORD Flags = MCI_WAIT | MCI_DGV_WINDOW_HWND;
    AWindowParm.hWnd = (Control != NULL) ? Control->Handle : reinterpret_cast<HANDLE>(MCI_DGV_WINDOW_DEFAULT);
    MciCheck(mciSendCommand(DeviceID, MCI_WINDOW, Flags, reinterpret_cast<DWORD>(&AWindowParm)));
  }
}
//---------------------------------------------------------------------------
void __fastcall TMediaPlayerEx::SetTimeFormat(TMPTimeFormats Format)
{
  CheckIfOpen();
  MCI_SET_PARMS SetParm;
  DWORD Flags = MCI_WAIT | MCI_SET_TIME_FORMAT;
  SetParm.dwTimeFormat = Format;
  MciCheck(mciSendCommand(DeviceID, MCI_SET, Flags, reinterpret_cast<DWORD>(&SetParm)));
}
//---------------------------------------------------------------------------
DWORD TMediaPlayerEx::GetStatus(DWORD Item)
{
  MCI_STATUS_PARMS StatusParm;
  CheckIfOpen();
  DWORD Flags = MCI_WAIT | MCI_STATUS_ITEM;
  StatusParm.dwItem = Item;
  MciCheck(mciSendCommand(DeviceID, MCI_STATUS, Flags, reinterpret_cast<DWORD>(&StatusParm)));
  return StatusParm.dwReturn;
}
//---------------------------------------------------------------------------
TMPTimeFormats __fastcall TMediaPlayerEx::GetTimeFormat()
{
  return static_cast<TMPTimeFormats>(GetStatus(MCI_STATUS_TIME_FORMAT));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Seek(unsigned Value, bool Wait, bool Notify)
{
  MCI_SEEK_PARMS SeekParm;
  CheckIfOpen();

  DWORD Flags = Value == 0 ? MCI_SEEK_TO_START : MCI_TO;

  if(Wait)
    Flags |= MCI_WAIT;
  if(Notify)
    Flags |= MCI_NOTIFY;

  SeekParm.dwCallback = Handle;
  SeekParm.dwTo = Value;
  MciCheck(mciSendCommand(DeviceID, MCI_SEEK, Flags, reinterpret_cast<DWORD>(&SeekParm)));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::SetPosition(int Value)
{
  if(Value != GetPosition())
    Seek(Value, true, false);
}
//---------------------------------------------------------------------------
int TMediaPlayerEx::GetPosition()
{
  return GetStatus(MCI_STATUS_POSITION);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::GetDeviceCaps()
{
  MCI_GETDEVCAPS_PARMS DevCapParm;

  WORD Flags = MCI_WAIT | MCI_GETDEVCAPS_ITEM;

  DevCapParm.dwItem = MCI_GETDEVCAPS_CAN_PLAY;
  MciCheck(mciSendCommand(DeviceID, MCI_GETDEVCAPS, Flags, reinterpret_cast<DWORD>(&DevCapParm)));
  if(DevCapParm.dwReturn)
    FCapabilities << mpCanPlay;

  DevCapParm.dwItem = MCI_GETDEVCAPS_CAN_RECORD;
  MciCheck(mciSendCommand(DeviceID, MCI_GETDEVCAPS, Flags, reinterpret_cast<DWORD>(&DevCapParm)));
  if(DevCapParm.dwReturn)
    FCapabilities << mpCanRecord;

  DevCapParm.dwItem = MCI_GETDEVCAPS_CAN_EJECT;
  MciCheck(mciSendCommand(DeviceID, MCI_GETDEVCAPS, Flags, reinterpret_cast<DWORD>(&DevCapParm)));
  if(DevCapParm.dwReturn)
    FCapabilities << mpCanEject;

  DevCapParm.dwItem = MCI_GETDEVCAPS_HAS_VIDEO;
  MciCheck(mciSendCommand(DeviceID, MCI_GETDEVCAPS, Flags, reinterpret_cast<DWORD>(&DevCapParm)));
  if(DevCapParm.dwReturn)
    FCapabilities << mpUsesWindow;

  DevCapParm.dwItem = MCI_GETDEVCAPS_DEVICE_TYPE;
  MciCheck(mciSendCommand(DeviceID, MCI_GETDEVCAPS, Flags, reinterpret_cast<DWORD>(&DevCapParm)));
  if(DevCapParm.dwReturn == MCI_DEVTYPE_ANIMATION ||
     DevCapParm.dwReturn == MCI_DEVTYPE_DIGITAL_VIDEO ||
     DevCapParm.dwReturn == MCI_DEVTYPE_OVERLAY ||
     DevCapParm.dwReturn == MCI_DEVTYPE_VCR)
       FCapabilities << mpCanStep;

  MCI_ANIM_RECT_PARMS RectParms;
  MciCheck(mciSendCommand(DeviceID, MCI_WHERE, MCI_ANIM_WHERE_SOURCE, reinterpret_cast<DWORD>(&RectParms)));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::MciCheck(MCIERROR Error)
{
  if(Error != 0)
  {
	wchar_t Str[128];
	mciGetErrorString(Error, Str, sizeof(Str)/sizeof(Str[0]));
    throw EMCIDeviceError(Str);
  }
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::CheckIfOpen()
{
  if(DeviceID == 0)
    throw EMCIDeviceError("Device not open");
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Play(bool Wait)
{
  CheckIfOpen();
  int End = (To == -1) ? Length-1 : To;
  if(!Reverse && AutoRewind && Position >= End)
    Seek(0, true, false);

  MCI_PLAY_PARMS PlayParm;

  DWORD Flags = 0;

  if(Notify)
    Flags |= MCI_NOTIFY;
  if(Wait)
    Flags != MCI_WAIT;
  if(Repeat)
    Flags |= MCI_DGV_PLAY_REPEAT;
  if(Reverse)
    Flags |= MCI_DGV_PLAY_REVERSE;
  if(FFrom != -1)
    Flags |= MCI_FROM;
  if(FTo != -1)
    Flags |= MCI_TO;

  PlayParm.dwCallback = Handle;
  PlayParm.dwFrom = FFrom;
  PlayParm.dwTo = FTo;
  MciCheck(mciSendCommand(DeviceID, MCI_PLAY, Flags, reinterpret_cast<DWORD>(&PlayParm)));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::StepFrames(unsigned Frames, bool Reverse, bool Wait)
{
  CheckIfOpen();

  if(Capabilities.Contains(mpUsesWindow))
  {
    MCI_DGV_STEP_PARMS StepParm;
    DWORD Flags = MCI_DGV_STEP_FRAMES;
    if(Wait)
      Flags |= MCI_WAIT;
    if(Notify)
      Flags |= MCI_NOTIFY;
    if(Reverse)
      Flags |= MCI_DGV_STEP_REVERSE;
    StepParm.dwFrames = Frames;
    StepParm.dwCallback = Handle;
    MciCheck(mciSendCommand(DeviceID, MCI_STEP, Flags, reinterpret_cast<DWORD>(&StepParm)));
  }
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Close(bool Wait)
{
  if(DeviceID == 0)
    return;
  MciGeneric(MCI_CLOSE, 0, Wait, Notify);
  DeviceID = 0;
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::MciGeneric(DWORD Command, DWORD Flags, bool Wait, bool Notify)
{
  CheckIfOpen();
  MCI_GENERIC_PARMS GenParm;

  if(Wait)
    Flags |= MCI_WAIT;
  if(Notify)
    Flags |= MCI_NOTIFY;

  GenParm.dwCallback = Handle;
  MciCheck(mciSendCommand(DeviceID, Command, Flags, reinterpret_cast<DWORD>(&GenParm)));
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::EjectDoor(bool Wait)
{
  MciGeneric(MCI_SET, MCI_SET_DOOR_OPEN, Wait, Notify);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::CloseDoor(bool Wait)
{
  MciGeneric(MCI_SET, MCI_SET_DOOR_CLOSED, Wait, Notify);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Next(bool Wait)
{
  CheckIfOpen();

}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Open(bool Wait)
{
const wchar_t* DeviceName[] = {L"", L"AVIVideo", L"CDAudio", L"DAT",
	L"DigitalVideo", L"MMMovie", L"Other", L"Overlay", L"Scanner", L"Sequencer",
	L"VCR", L"Videodisc" L"WaveAudio"};

  MCI_OPEN_PARMS OpenParm;
  TRect DisplayR;

  if(DeviceID != 0)
    Close(); //Must close MCI Device first before opening another}

  OpenParm.dwCallback = Handle;
  OpenParm.lpstrDeviceType = DeviceName[FDeviceType];
  OpenParm.lpstrElementName = FFileName.c_str();

  DWORD Flags = 0;

  if(Wait)
    Flags |= MCI_WAIT;

  if(Notify)
    Flags |= MCI_NOTIFY;

  if(FDeviceType != dtAutoSelect)
    Flags |= MCI_OPEN_TYPE;
  else
    Flags |= MCI_OPEN_ELEMENT;

  if(FShareable)
    Flags |= MCI_OPEN_SHAREABLE;

  MciCheck(mciSendCommand(0, MCI_OPEN, Flags, reinterpret_cast<DWORD>(&OpenParm)));

  DeviceID = OpenParm.wDeviceID;
  FFrames = Length / 10;  //Default frames to step = 10% of total frames
  GetDeviceCaps(); //Must first get device capabilities}
  if(FCapabilities.Contains(mpUsesWindow)) //Used for video output positioning
    SetDisplay(FDisplay); //If one was set in design mode

  if(FSpeed != 1000)
    SetSpeed(FSpeed);

  if(FDeviceType == dtCDAudio || FDeviceType == dtVideodisc)
    TimeFormat = tfTMSF; //Set timeformat to use tracks
}
//---------------------------------------------------------------------------
void __fastcall TMediaPlayerEx::Notification(TComponent* AComponent, TOperation Operation)
{
  if(Operation == opRemove && AComponent == FDisplay)
    SetDisplay(NULL);
  TComponent::Notification(AComponent, Operation);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Pause(bool Wait)
{
  MciGeneric(MCI_PAUSE, 0, Wait, Notify);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Resume(bool Wait)
{
  MciGeneric(MCI_RESUME, 0, Wait, Notify);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Previous(bool Wait)
{
  CheckIfOpen();

}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Step(bool Wait)
{
  int End = (To == -1) ? Length-1 : To;
  if(FAutoRewind && Position >= End)
    Seek(0, Wait, true);
  else
    StepFrames(FFrames, false, Wait);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Back(bool Wait)
{
  int StepValue = Frames;
  if(Position == Length)
    StepValue++;
  StepFrames(StepValue, true, Wait);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Stop(bool Wait)
{
  MciGeneric(MCI_STOP, 0, Wait, Notify);
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::Rewind(bool Wait)
{
  CheckIfOpen();
  Seek(0, Wait, Notify);
}
//---------------------------------------------------------------------------
int TMediaPlayerEx::GetLength()
{
  return GetStatus(MCI_STATUS_LENGTH);
}
//---------------------------------------------------------------------------
TMPModes TMediaPlayerEx::GetMode()
{
  return static_cast<TMPModes>(GetStatus(MCI_STATUS_MODE) - 524); //MCI Mode #s are 524+enum
}
//---------------------------------------------------------------------------
void __fastcall TMediaPlayerEx::SetSpeed(int Value)
{
  if(Value <= 0)
    return;

  FSpeed = Value;
  if(DeviceID != 0)
  {
    MCI_DGV_SET_PARMS SetParm;
    DWORD Flags = MCI_WAIT | MCI_DGV_SET_SPEED;
    SetParm.dwSpeed = Value;
    MciCheck(mciSendCommand(DeviceID, MCI_SET, Flags, reinterpret_cast<DWORD>(&SetParm)));
  }
}
//---------------------------------------------------------------------------
TRect TMediaPlayerEx::GetDisplayRect()
{
  MCI_DGV_RECT_PARMS RectParm;
  CheckIfOpen();
  DWORD Flags = MCI_WAIT | MCI_DGV_WHERE_DESTINATION;
  MciCheck(mciSendCommand(DeviceID, MCI_WHERE, Flags, reinterpret_cast<DWORD>(&RectParm)));
  return RectParm.rc;
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::SetDisplayRect(const TRect &Rect)
{
  MCI_DGV_RECT_PARMS RectParm;
  RectParm.rc = Rect;
  CheckIfOpen();
  DWORD Flags = MCI_WAIT | MCI_DGV_RECT | MCI_DGV_PUT_DESTINATION;
  MciCheck(mciSendCommand(DeviceID, MCI_PUT, Flags, reinterpret_cast<DWORD>(&RectParm)));
}
//---------------------------------------------------------------------------
TRect TMediaPlayerEx::GetSourceRect()
{
  MCI_DGV_RECT_PARMS RectParm;
  CheckIfOpen();
  DWORD Flags = MCI_WAIT | MCI_DGV_WHERE_SOURCE;
  MciCheck(mciSendCommand(DeviceID, MCI_WHERE, Flags, reinterpret_cast<DWORD>(&RectParm)));
  return RectParm.rc;
}
//---------------------------------------------------------------------------
void TMediaPlayerEx::GetBitmap(Graphics::TBitmap *Bitmap)
{
  CheckIfOpen();
  TRect Rect = DisplayRect;
  Bitmap->Width = Rect.Width();
  Bitmap->Height = Rect.Height();

  MCI_DGV_UPDATE_PARMS UpdateParms = {0};
  UpdateParms.hDC = Bitmap->Canvas->Handle;
  DWORD Flags = MCI_WAIT | MCI_DGV_UPDATE_HDC;
  MciCheck(mciSendCommand(DeviceID, MCI_UPDATE, Flags, reinterpret_cast<DWORD>(&UpdateParms)));
}
//---------------------------------------------------------------------------


