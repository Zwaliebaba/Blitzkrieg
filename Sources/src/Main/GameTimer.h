#ifndef __GAMETIMER_H__
#define __GAMETIMER_H__

#pragma once

enum
{
  TIMER_BASE_VALUE = 0x10050000,
  TIMER_SINGLE_TIMER = TIMER_BASE_VALUE + 1,
  TIMER_GAME_TIMER = TIMER_BASE_VALUE + 2
};

namespace NTimer
{
  using STime = DWORD;
  //
  inline float GetCoeffFromSpeed(const int nSpeed) { return nSpeed >= 0 ? nSpeed + 1 : 1.0f / fabsf(static_cast<float>(nSpeed - 1)); }
};

class CScaleTimer
{
  NTimer::STime prevTime;// current dependent time
  NTimer::STime currTime;// current independent time
  float fScale;// time scaling
  float fError;// time rounding error (for scaling)
public:
  CScaleTimer() : prevTime(0), currTime(0), fScale(1), fError(0) {}
  // set scale
  void SetScale(float _fScale) { fScale = _fScale; }
  // get current time value
  const NTimer::STime &Get() const { return currTime; }
  // reset timer to this time
  void Reset(const NTimer::STime &time)
  {
    prevTime = time;
    currTime = 0;
    fError = 0;
  }

  // update timer with new time
  void Update(const NTimer::STime &time)
  {
    NTimer::STime dT = prevTime == 0 ? 0 : time - prevTime;
    prevTime = time;
    const float fdt = static_cast<float>(dT * fScale) + fError;
    dT = static_cast<NTimer::STime>(MINT(fdt));
    fError = fdt - static_cast<float>(dT);
    currTime += dT;
  }

  // serialize
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &prevTime);
    saver.Add(2, &currTime);
    saver.Add(3, &fScale);
    saver.Add(4, &fError);
    return 0;
  }
};

interface ITimeSlider : IRefCount
{
  virtual NTimer::STime GetDelta() = 0;
  virtual void Reset() = 0;
};

interface IBaseTimer : IRefCount
{
  virtual NTimer::STime Get() = 0;
  virtual void Set(const NTimer::STime &time) = 0;
};

interface ISingleTimer : IBaseTimer
{
  virtual void Reset() = 0;
  virtual void Pause(bool bPause) = 0;
  virtual ITimeSlider * CreateSlider() = 0;
  virtual void Update(const NTimer::STime &time) = 0;
  virtual bool IsPaused() const = 0;
  // guarantee FPS (for movie sequence capturing)
  virtual void SetGuarantieFPS(float fFPS) = 0;
  virtual const float GetGuarantieFPS() const = 0;
  //
  virtual void SetTimeScale(float scale) = 0;
};

interface ISegmentTimer : IBaseTimer
{
  // segment time kduration
  virtual void SetSegmentTime(const NTimer::STime &time) = 0;
  virtual NTimer::STime GetSegmentTime() = 0;
  // segments
  virtual bool BeginSegments(const NTimer::STime &time) = 0;
  virtual bool NextSegment() = 0;
  virtual int GetSegment() = 0;
  virtual void SetSegment(int nSegment) = 0;
};

interface IGameTimer : IRefCount
{
  // type ID
  enum { tidTypeID = TIMER_GAME_TIMER };

  //
  virtual void Init() = 0;
  // timers
  virtual ISingleTimer * GetGameTimer() = 0;
  virtual ISingleTimer * GetSyncTimer() = 0;
  virtual ISingleTimer * GetAbsTimer() = 0;
  // times
  virtual NTimer::STime GetGameTime() = 0;
  virtual NTimer::STime GetSyncTime() = 0;
  virtual NTimer::STime GetAbsTime() = 0;
  // segment timers
  virtual ISegmentTimer * GetGameSegmentTimer() = 0;
  virtual ISegmentTimer * GetSyncSegmentTimer() = 0;
  // segments
  virtual NTimer::STime GetGameSegmentTime() = 0;
  virtual NTimer::STime GetSyncSegmentTime() = 0;
  // pause
  virtual void PauseGame(bool bPause, int nType = 0) = 0;
  virtual void PauseSync(bool bPause, int nType = 0) = 0;
  virtual int GetPauseReason() const = 0;
  virtual bool HasPause(int nReason) const = 0;
  // guarantee FPS (for movie sequence capturing)
  virtual void SetGuarantieFPS(float fFPS) = 0;
  // update
  virtual void Update(const NTimer::STime &time) = 0;
  // time speed increase/decrease
  virtual int SetSpeed(int nSpeed) = 0;
  virtual int GetSpeed() const = 0;
};

#endif // __GAMETIMER_H__