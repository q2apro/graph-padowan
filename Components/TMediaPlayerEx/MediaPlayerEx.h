//---------------------------------------------------------------------------
#ifndef MediaPlayerExH
#define MediaPlayerExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <MPlayer.hpp>
//---------------------------------------------------------------------------
class TMediaPlayerEx;
typedef void __fastcall (__closure *TSignalEvent)(TMediaPlayerEx* Sender, unsigned Position);
typedef void __fastcall (__closure *TNotifyEvent)(TMediaPlayerEx* Sender, TMPNotifyValues NotifyValue);

class PACKAGE TMediaPlayerEx : public TComponent
{
private:
  TSignalEvent FOnSignal;
  ::TNotifyEvent FOnNotify;
  bool FRepeat;
  bool FReverse;
  bool FNotify;
  bool FShareable;
  bool FAutoRewind;
  int FFrom;
  int FTo;
  int FFrames;
  int FSpeed;
  TWinControl *FDisplay;
  String FFileName;
  TMPDevCapsSet FCapabilities;
  TMPDeviceTypes FDeviceType;

  unsigned DeviceID;
  DWORD Handle;
  TWndMethod OldWindowProc;
  TRect WorkR;

  void __fastcall WndProc(Messages::TMessage &Message);
  void GetDeviceCaps();
  void MciCheck(MCIERROR Error);
  void CheckIfOpen();
  void StepFrames(unsigned Frames, bool Reverse, bool Wait);
  DWORD GetStatus(DWORD Item);
  void Seek(unsigned Value, bool Wait, bool Notify);
  void MciGeneric(DWORD Command, DWORD Flags, bool Wait, bool Notify);

  void __fastcall SetDisplay(TWinControl *Control);
  void __fastcall SetTimeFormat(TMPTimeFormats Format);
  TMPTimeFormats __fastcall GetTimeFormat();
  void SetPosition(int Value);
  int GetPosition();
  int GetLength();
  TMPModes GetMode();
  void __fastcall SetSpeed(int Value);
  TRect GetDisplayRect();
  void SetDisplayRect(const TRect &Rect);
  TRect GetSourceRect();

  void __fastcall Notification(TComponent* AComponent, TOperation Operation);

protected:
public:
  __fastcall TMediaPlayerEx(TComponent* Owner);
  __fastcall ~TMediaPlayerEx();
  void SetSignal(unsigned At, unsigned Interval = 0);
  void RemoveSignal(unsigned At);
  void RemoveAllSignals();
  void Play(bool Wait=false);
  void Back(bool Wait=true);
  void Close(bool Wait=true);
  void EjectDoor(bool Wait=true);
  void CloseDoor(bool Wait=true);
  void Next(bool Wait=true);
  void Open(bool Wait=true);
  void Pause(bool Wait=true);
  void Resume(bool Wait=false);
  void Previous(bool Wait=true);
  void Step(bool Wait=true);
  void Stop(bool Wait=true);
  void Rewind(bool Wait=true);
  void GetBitmap(Graphics::TBitmap *Bitmap);

  __property int Position = {read=GetPosition, write=SetPosition};
  __property int Frames = {read=FFrames, write=FFrames};
  __property int Length = {read=GetLength};
  __property int From = {read=FFrom, write=FFrom, default=-1};
  __property int To = {read=FTo, write=FTo, default=-1};
  __property TMPDevCapsSet Capabilities = {read=FCapabilities};
  __property TMPTimeFormats TimeFormat = {read=GetTimeFormat, write=SetTimeFormat};
  __property TMPModes Mode = {read=GetMode};
  __property TRect DisplayRect = {read=GetDisplayRect, write=SetDisplayRect};
  __property TRect SourceRect = {read=GetSourceRect};

__published:
  __property TWinControl* Display = {read=FDisplay, write=SetDisplay, default=NULL};
  __property String FileName = {read=FFileName, write=FFileName};
  __property TMPDeviceTypes DeviceType = {read=FDeviceType, write=FDeviceType, default=dtAutoSelect};
  __property bool Repeat = {read=FRepeat, write=FRepeat, default=false};
  __property bool Reverse = {read=FReverse, write=FReverse, default=false};
  __property bool Notify = {read=FNotify, write=FNotify, default=false};
  __property bool Shareable = {read=FShareable, write=FShareable, default=false};
  __property bool AutoRewind = {read=FAutoRewind, write=FAutoRewind, default=false};
  __property int Speed = {read=FSpeed, write=SetSpeed, default=1000};

  __property TSignalEvent OnSignal = {read=FOnSignal, write=FOnSignal, default=NULL};
  __property ::TNotifyEvent OnNotify = {read=FOnNotify, write=FOnNotify, default=NULL};
};
//---------------------------------------------------------------------------
#endif
