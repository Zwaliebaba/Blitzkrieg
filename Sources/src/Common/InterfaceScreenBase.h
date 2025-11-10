#ifndef __INTERFACESCREENBASE_H__
#define __INTERFACESCREENBASE_H__

#include "../Main/iMain.h"
#include "../GFX/GFX.h"
#include "../SFX/SFX.h"
#include "../Input/Input.h"
#include "../Scene/Scene.h"
#include "../UI/UI.h"
#include "../Misc/HPTimer.h"
#include "../Main/TextSystem.h"

template<class TInterface, int NInterfaceTypeID>
class CInterfaceCommandBase : public IInterfaceCommand
{
  NTimer::STime timeDelayed;

protected:
  CInterfaceCommandBase() : timeDelayed(0) {}
  virtual ~CInterfaceCommandBase() {}
  //
  virtual void PreCreate(IMainLoop *pML) {}
  virtual void PostCreate(IMainLoop *pML, TInterface *pInterface) {}

public:
  void STDCALL Exec(IMainLoop *pML) override
  {
    PreCreate(pML);
    //
    TInterface *pInterface = CreateObject<TInterface>(NInterfaceTypeID);
    pInterface->Init();
    pML->ClearResources();
    pInterface->StartInterface();
    //
    PostCreate(pML, pInterface);
  }

  //
  void STDCALL SetDelayedTime(const NTimer::STime &timeToExecute) override { timeDelayed = timeToExecute; }
  NTimer::STime STDCALL GetDelayedTime() const override { return timeDelayed; }
};

class CInterfaceScreenBase : public IInterfaceBase
{
  DECLARE_SERIALIZE;

  int nHelpContextNumber;// for intermission intefaces help

  // cursor movement tracking
  CVec2 vLastCursorPos;// last cursor movement position
  bool bLastCursorScreenMoveRes;// 
  // statistics
  bool bEnableStatistics;// enable to show stats
  NHPTimer::STime time;// stats collecting precision time
  float fTotalTime;// total time since start
  int nFrameCounter;// total frame counter since start
  int nTriCounter;// triangle counter
  float fAveFPS;// average FPS (for a last second)
  float fAveTPS;// average TPS -~-
  int nCPUFreq;// main CPU frequency
  //
  std::list<SGameMessage> messages;// messages to top
  // 
  std::string szBindSection;// this interface bind section
  const std::string szInterfaceType;// interface type - "InterMission", "Mission"
  // last tooltip
  CPtr<IText> pLastToolTip;// last setup tooltip
  NTimer::STime timeToolTip;// time, last tooltip appeared
  NTimer::STime timeToolTipShowTime;// time to show tooltip in the case of the cursor inactivity
  NTimer::STime timeToolTipHideTime;// time to hide tooltip in the case of the cursor inactivity

  bool bInterfaceClosed;
  //
  CPtr<IUIScreen> pStoredScreen;// stored UI screen (from under-interface)
  //
  int PlayOverInterface(const char *pszName, DWORD dwAddFlags, bool bFadeIn);
  void EnableMessageProcessingDelayed(bool bEnable, const NTimer::STime &timeToPerform);
  //
  void RestoreScreen();

protected:
  // singleton shortcuts
  CPtr<IGFX> pGFX;
  CPtr<ISFX> pSFX;
  CPtr<IInput> pInput;
  CPtr<IScene> pScene;
  CPtr<ICamera> pCamera;
  CPtr<ICursor> pCursor;
  CPtr<IGameTimer> pTimer;
  // UI screen
  CObj<IUIScreen> pUIScreen;
  //
  bool ToggleShowStats()
  {
    bEnableStatistics = !bEnableStatistics;
    return bEnableStatistics;
  }

  void AddStatistics();
  //
  bool ProcessAndAdd(const SGameMessage &msg)
  {
    if (!ProcessMessage(msg)) AddMessage(msg);
    return messages.empty();
  }

  void AddMessage(const SGameMessage &msg);
  // 
  void SetBindSection(const char *pszBindSection) { szBindSection = pszBindSection; }
  //
  bool ChangeResolution();
  void AddDelayedCommand(IInterfaceCommand *pCmd, const NTimer::STime &timeToPerform);
  void RemoveTransition() { if (pScene) pScene->RemoveSceneObject(nullptr); }
  virtual bool OpenCurtains();
  void OpenCurtainsForced();
  //
  void StoreScreen();
  //
  virtual void SuspendAILogic(bool bSuspend);
  //
  virtual bool STDCALL StepLocal(bool bAppActive) { return bAppActive; }
  virtual void STDCALL DrawAdd() {}
  virtual bool STDCALL ProcessMessage(const SGameMessage &msg) = 0;
  virtual bool STDCALL OnCursorMove(const CVec2 &vPos);
  // tutorial
  void SetTutorialNumber(const int _nHelpContext) { nHelpContextNumber = _nHelpContext; }
  void ShowTutorialIfNotShown();
  bool ShowTutorial();
  void CloseInterface(bool bCurtains = false);
  // disable explicit destruction
  virtual ~CInterfaceScreenBase() {}

public:
  CInterfaceScreenBase(const std::string &_szInterfaceType);
  //
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void STDCALL Step(bool bAppActive) override;
  bool STDCALL ProcessUIMessage(const SGameMessage &msg) override;
  bool STDCALL ProcessTextMessage(const STextMessage &msg) override;
  bool STDCALL GetMessage(SGameMessage *pMsg) override;
  void STDCALL OnGetFocus(bool bFocus) override;
  void STDCALL StartInterface() override;
  // finish interface with transition to the next one
  virtual int STDCALL FinishInterface(IInterfaceCommand *pCmdNextInterface);
  virtual int STDCALL FinishInterface(int nInterfaceCommandTypeID, const char *pszCommandConfig);
  virtual void STDCALL SetWindowText(int nElementID, IText *pText);
  virtual void STDCALL SetWindowText(int nElementID, const WORD *pszText);
};

#endif // __INTERFACESCREENBASE_H__