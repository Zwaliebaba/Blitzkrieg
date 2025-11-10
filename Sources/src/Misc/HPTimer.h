#ifndef __HPTIMER_H_
#define __HPTIMER_H_

// /////////////////////////
namespace NHPTimer
{
  using STime = int64;
  double GetSeconds(const STime &a);
  // get current time
  void GetTime(STime *pTime);
  // get the time elapsed since the moment recorded in *pTime, while the current time will be written in *pTime
  double GetTimePassed(STime *pTime);
  // get processor frequency
  double GetClockRate();
};

// /////////////////////////
#endif