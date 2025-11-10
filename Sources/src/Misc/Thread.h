#ifndef __THREAD_H__
#define __THREAD_H__

#pragma once

#include "../Misc/Win32Helper.h"

class CThread
{
  // thread
  HANDLE hThread;
  HANDLE hFinishReport;
  HANDLE hStopCommand;

  NWin32Helper::CCriticalSection criticalSection;

  const int nDelay;
  bool bRun;

protected:
  virtual void Step() = 0;

public:
  explicit CThread(int nDelay);
  ~CThread();

  const int GetDelay() const { return nDelay; }

  void RunThread();
  void StopThread();

  // functions for internal work of the thread
  // thread functions
  static DWORD WINAPI TheThreadProc(LPVOID lpParameter);

  void StartThread();
  bool CanWork();
  void FinishThread();
};

#endif // __THREAD_H__