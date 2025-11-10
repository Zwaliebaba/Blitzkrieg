#include "stdafx.h"

#include "Thread.h"

using namespace NWin32Helper;

DWORD WINAPI CThread::TheThreadProc(LPVOID lpParameter)
{
  // run function
  auto pThread = reinterpret_cast<CThread *>(lpParameter);
  pThread->StartThread();

  const int nDelay = pThread->GetDelay();
  while (pThread->CanWork())
  {
    Sleep(nDelay);
    pThread->Step();
  }

  pThread->FinishThread();

  return 0;
}

void CThread::StartThread() { ResetEvent(hFinishReport); }

bool CThread::CanWork() { return WaitForSingleObject(hStopCommand, 0) != WAIT_OBJECT_0; }

void CThread::FinishThread() { SetEvent(hFinishReport); }

CThread::CThread(const int _nDelay)
  : nDelay(_nDelay), bRun(false)
{
  hThread = nullptr;
  hFinishReport = CreateEvent(nullptr, true, false, nullptr);
  hStopCommand = CreateEvent(nullptr, true, false, nullptr);
}

void CThread::StopThread()
{
  CCriticalSectionLock criticalSectionLock(criticalSection);

  if (bRun)
  {
    if (hThread)
    {
      // the thread itself did not terminate
      if (WaitForSingleObject(hFinishReport, 0) != WAIT_OBJECT_0)
      {
        SetEvent(hStopCommand);
        criticalSectionLock.Leave();
        WaitForSingleObject(hFinishReport, INFINITE);
        criticalSectionLock.Enter();
      }

      CloseHandle(hThread);
      hThread = nullptr;
    }

    ResetEvent(hStopCommand);
    CloseHandle(hStopCommand);
    ResetEvent(hFinishReport);
    CloseHandle(hFinishReport);

    bRun = false;
  }
}

CThread::~CThread()
{
  CCriticalSectionLock criticalSectionLock(criticalSection);
  StopThread();
}

void CThread::RunThread()
{
  if (!bRun)
  {
    DWORD dwThreadId;
    ResetEvent(hStopCommand);
    ResetEvent(hFinishReport);
    bRun = true;
    hThread = CreateThread(nullptr, 1024 * 1024, TheThreadProc, this, 0, &dwThreadId);
  }
}