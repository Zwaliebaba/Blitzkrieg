#ifndef __EMERGENCYSAVE_H__
#define __EMERGENCYSAVE_H__

#pragma once

#include "iMain.h"

class CEmergencySave : public IBaseCommand
{
  OBJECT_MINIMAL_METHODS(CEmergencySave);
  //
  CPtr<IMainLoop> pMainLoop;// main loop to call save from
  CPtr<IStructureSaver> pSS;// emergency save structure saver
public:
  CEmergencySave(IMainLoop *_pMainLoop, IStructureSaver *_pSS)
    : pMainLoop(_pMainLoop), pSS(_pSS) {}

  // execute command
  void STDCALL Do() override
  {
    try
    {
      pMainLoop->operator&(*pSS);
      pSS = nullptr;
    }
    catch (...) { ::MessageBox(nullptr, "Unable to execute 'emergency save' command", "ERROR", MB_OK | MB_ICONEXCLAMATION); }
  }

  // un-execute command
  void STDCALL UnDo() override {}
  // can this command be un-executed
  bool STDCALL CanUnDo() override { return false; }
};

#endif // __EMERGENCYSAVE_H__