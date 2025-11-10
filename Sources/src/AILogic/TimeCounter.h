#ifndef __TIME_COUNTER_H__
#define __TIME_COUNTER_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "../Misc/HPTimer.h"

// //////////////////////////////////////////////////////////// 
class CTimeCounter
{
  std::vector<double> counters;
  std::vector<NHPTimer::STime> startTimes;
  std::vector<std::string> names;

  std::hash_map<std::string, double> szCounters;
  std::hash_map<std::string, NHPTimer::STime> szStartTimes;

  NTimer::STime printTime;
  int nMaxIndex;

  std::vector<float> variables;
  int nMaxVar;

public:
  CTimeCounter();

  // bStart true - start counter, false - finish
  void Count(int nName, bool bStart);
  // slow and inaccurate, bStart true - start counter, false - finish
  void Count(const std::string &szName, bool bStart);

  void PrintCounters();

  void RegisterCounter(int nName, const std::string &szName);

  void ChangeVar(int nIndex, float fChange);
  void SetVar(int nIndex, float fValue);
};

// //////////////////////////////////////////////////////////// 
#endif // __TIME_COUNTER_H__