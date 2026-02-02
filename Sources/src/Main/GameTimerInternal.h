#ifndef __GAMETIMERINTERNAL_H__
#define __GAMETIMERINTERNAL_H__

#pragma once

#include "GameTimer.h"
#include "../AILogic/AIConsts.h"

static constexpr NTimer::STime INITIAL_TIME = SAIConsts::AI_SEGMENT_DURATION;
// ************************************************************************************************************************ //
// **
// ** time slider
// ** calculates time difference
// **
// **
// ************************************************************************************************************************ //

class CTimeSlider : public ITimeSlider
{
  OBJECT_NORMAL_METHODS(CTimeSlider);
  DECLARE_SERIALIZE;
  //
  CPtr<ISingleTimer> pTimer;// timer, this differentiator attached to
  NTimer::STime timeLastTime;// last query time
public:
  CTimeSlider() : timeLastTime(0) {}
  CTimeSlider(ISingleTimer *_pTimer) : pTimer(_pTimer) { Reset(); }

  NTimer::STime GetDelta() override
  {
    NTimer::STime timeCurrTime = pTimer->Get();
    NTimer::STime timeDiffTime = timeCurrTime - timeLastTime;
    timeLastTime = timeCurrTime;
    return timeDiffTime;
  }

  void Reset() override { timeLastTime = pTimer->Get(); }
};

// ************************************************************************************************************************ //
// **
// **segment timer
// ** keeps track of segments and their time
// **
// **
// ************************************************************************************************************************ //

class CSegmentTimer : public ISegmentTimer
{
  OBJECT_NORMAL_METHODS(CSegmentTimer);
  DECLARE_SERIALIZE;
  //
  NTimer::STime prevTime;// last segment independent time
  NTimer::STime currTime;// current independent time
  NTimer::STime tSegmentTime;// one segment duration
  int nSegment;// current segment
public:
  CSegmentTimer()
    : prevTime(INITIAL_TIME), currTime(INITIAL_TIME), tSegmentTime(SAIConsts::AI_SEGMENT_DURATION), nSegment(0) {}

  //
  NTimer::STime Get() override { return prevTime; }
  void Set(const NTimer::STime &time) override { prevTime = time - (time % tSegmentTime); }
  //
  void SetSegmentTime(const NTimer::STime &time) override { tSegmentTime = time; }
  NTimer::STime GetSegmentTime() override { return tSegmentTime; }

  bool BeginSegments(const NTimer::STime &time) override
  {
    if (prevTime > time) Set(time);
    //
    currTime = time;
    return currTime - prevTime >= tSegmentTime;
  }

  bool NextSegment() override
  {
    NI_ASSERT_TF(currTime >= prevTime, "currrent segment time less then previous!!!", return false);
    NTimer::STime tDiff = currTime - prevTime;
    if (tDiff >= tSegmentTime)
    {
      prevTime += tSegmentTime;
      ++nSegment;
      return true;
    }
    return false;
  }

  int GetSegment() override { return nSegment; }
  void SetSegment(int _nSegment) override { nSegment = _nSegment; }
};

// ************************************************************************************************************************ //
// **
// **single timer
// ** keeps track of one timer
// **
// **
// ************************************************************************************************************************ //

class CSingleTimer : public ISingleTimer
{
  OBJECT_NORMAL_METHODS(CSingleTimer);
  DECLARE_SERIALIZE;
  //
  NTimer::STime prevTime;// current dependent time
  NTimer::STime currTime;// current independent time
  bool bPaused;// is timer paused
  float fTimeScale;// time scaling
  float fTimeError;// time rounding error
  //
  float fGuarantieFPS;// FPS to guarantee (for movie sequence capturing)
  int nGuarantieTimeStep;// 
public:
  CSingleTimer()
    : prevTime(0), currTime(INITIAL_TIME), bPaused(false), fTimeScale(1), fTimeError(0),
      fGuarantieFPS(0), nGuarantieTimeStep(0) {}

  //
  NTimer::STime Get() override { return currTime; }
  void Set(const NTimer::STime &time) override { currTime = time; }

  void Reset() override
  {
    prevTime = 0;
    currTime = INITIAL_TIME;
  }

  void Pause(bool bPause) override { bPaused = bPause; }
  ITimeSlider * CreateSlider() override { return new CTimeSlider(this); }
  void Update(const NTimer::STime &time) override;
  bool IsPaused() const override { return bPaused; }
  // guarantee FPS (for movie sequence capturing)
  void SetGuarantieFPS(float fFPS) override;
  const float GetGuarantieFPS() const override { return fGuarantieFPS; }
  //
  void SetTimeScale(float scale) override { fTimeScale = scale; }
};

// ************************************************************************************************************************ //
// **
// ** game timer
// ** contains several timers and runs them
// **
// **
// ************************************************************************************************************************ //

class CGameTimer : public IGameTimer
{
  OBJECT_NORMAL_METHODS(CGameTimer);
  DECLARE_SERIALIZE;
  // timers
  CPtr<ISingleTimer> pGameTimer;// game timer
  CPtr<ISingleTimer> pSyncTimer;// synchronization timer
  CPtr<ISingleTimer> pAbsTimer;// absolute time timer - don't affected by time coeff
  // segment timers
  CPtr<ISegmentTimer> pGameSegmentTimer;
  CPtr<ISegmentTimer> pSyncSegmentTimer;
  // pauses map
  using CPausesMap = std::unordered_map<int, int>;
  CPausesMap gamepauses;// game pauses map
  CPausesMap syncpauses;// sync pauses map
  int nPauseReason;// highest game pause reason
  // time coefficient
  int nTimeCoeff;// time scaling coeff. 
  // guarantee FPS
  float fGuarantieFPS;// game FPS to guarantee with this time
  int nGuarantieTimeStep;// guarantee game time step
  //
  void SetupTimeScaleForTimers();
  //
  void DoPause(bool bPause, int nType, ISingleTimer *pTimer, CPausesMap &pauses);

public:
  void Init() override;
  // timers
  ISingleTimer * GetGameTimer() override { return pGameTimer; }
  ISingleTimer * GetSyncTimer() override { return pSyncTimer; }
  ISingleTimer * GetAbsTimer() override { return pAbsTimer; }
  // times
  NTimer::STime GetGameTime() override { return pGameTimer->Get(); }
  NTimer::STime GetSyncTime() override { return pSyncTimer->Get(); }
  NTimer::STime GetAbsTime() override { return pAbsTimer->Get(); }
  // segment timers
  ISegmentTimer * GetGameSegmentTimer() override { return pGameSegmentTimer; }
  ISegmentTimer * GetSyncSegmentTimer() override { return pSyncSegmentTimer; }
  // segments
  NTimer::STime GetGameSegmentTime() override { return pGameSegmentTimer->Get(); }
  NTimer::STime GetSyncSegmentTime() override { return pSyncSegmentTimer->Get(); }
  // pause
  void PauseGame(bool bPause, int nType = 0) override;
  void PauseSync(bool bPause, int nType = 0) override;
  int GetPauseReason() const override { return nPauseReason; }
  bool HasPause(int nReason) const override;
  // guarantee FPS
  void SetGuarantieFPS(float fFPS) override;
  //
  void Update(const NTimer::STime &time) override;
  // time speed increase/decrease
  int SetSpeed(int nSpeed) override;
  int GetSpeed() const override { return nTimeCoeff; }
};

#endif // __GAMETIMERINTERNAL_H__