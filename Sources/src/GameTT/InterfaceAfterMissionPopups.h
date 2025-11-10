#ifndef __INTERFACEAFTERMISSIONPOPUPS_H__
#define __INTERFACEAFTERMISSIONPOPUPS_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CAfterMissionPopups : public IRefCount
{
  OBJECT_NORMAL_METHODS(CAfterMissionPopups);
  DECLARE_SERIALIZE;
  // input
  NInput::CCommandRegistrator commandMsgs;

  bool bTutorialWindow;
  bool bCheckReplay;
  int nMedalIterator;// current medal index
  bool bMainScreenShown;
  bool bUpgradesShown;
  bool bNewUnitsShown;
  bool bNextChapterShown;
  bool bPlayerRankShown;
  bool bLastFullScreen;// variable for the operation of curtains, if true then the last time the full screen interface was displayed
  bool bUnitsPerformanceShown;

  bool bNeedFinish;
  int nCommandID;
  std::string szCommandParams;

  bool bShowBlack;// black bottom screen

  // sets bNeedFinish
  void FinishInterface(int _nCommandID, const char *pszParam);
  //
public:
  CAfterMissionPopups() : bCheckReplay(false), nMedalIterator(0),
                          bMainScreenShown(false), bUpgradesShown(false), bNewUnitsShown(false), bNextChapterShown(false),
                          bPlayerRankShown(false),
                          bLastFullScreen(false), bUnitsPerformanceShown(false), bNeedFinish(false), nCommandID(0), bShowBlack(false) {}

  bool IsNeedFinish() const { return bNeedFinish; }
  bool IsNeedBlack() const { return bShowBlack; }
  int GetFinishCommandID() const { return nCommandID; }
  const char *GetFinishCommandParams() const { return (szCommandParams.empty() ? nullptr : szCommandParams.c_str()); }

  virtual void STDCALL OnGetFocus(bool bFocus);
};


#endif // __INTERFACEAFTERMISSIONPOPUPS_H__