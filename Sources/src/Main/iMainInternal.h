#ifndef __IMAININTERNAL_H__
#define __IMAININTERNAL_H__

#pragma once

#include "iMain.h"
#include "../Input/Input.h"
#include "../Input/InputHelper.h"
#include "../GFX/GFX.h"
#include "../Scene/Scene.h"
#include "../AILogic/AILogic.h"
#include "../Misc/HPTimer.h"
#include "TextSystem.h"
#include "ScenarioTracker.h"

class CMainLoop : public IMainLoop
{
  OBJECT_MINIMAL_METHODS(CMainLoop);
  //
  using STime = DWORD;
  using CInterfaceCommandsList = std::list<CPtr<IInterfaceCommand>>;
  using CInterfacesList = std::list<CPtr<IInterfaceBase>>;
  using CManagersList = std::vector<CPtr<ISharedManager>>;
  //
  CPtr<IGFX> pGFX;
  CPtr<IInput> pInput;
  CPtr<IScene> pScene;
  CPtr<ICamera> pCamera;
  CPtr<ICursor> pCursor;
  CPtr<IAILogic> pAILogic;
  //
  CInterfaceCommandsList cmds;// interface (inter-frame) commands
  CInterfacesList interfaces;// interfaces
  CManagersList managers;// data managers (for save/load)
  bool bAppIsActive;// is app active
  bool bWireFrame;// wireframe mode
  bool bTextMode;// text input mode
  bool bPaused;// is app paused
  bool bDisableMessageProcessing;// disable message processing
  std::string szBaseDir;
  //
  int nAutoSavePeriod;// auto save period (in msec)
  NTimer::STime timeLastAutoSave;// last autosave time
  int nGuaranteeFPS;// 
  int nGuaranteeFPSTime;//
  // network configuration
  int nNetAppID;// app id to achieve different apps on one port
  int nNetPort;// network socket port
  //
  // movie sequence
  CPtr<IImage> pScreenShotImage;
  //
  CPtr<IScenarioTracker> pStoredScenarioTracker;
  //
  NInput::CCommandRegistrator standardMsgs;
  //
  void ProcessStandardMsgs(const SGameMessage &msg);
  void OnMultiplayerStateCommand(const SGameMessage &msg);
  // void ProcessTimeoutMsg( const SGameMessage &msg );
  virtual ~CMainLoop();

public:
  CMainLoop();
  //
  const char * GetBaseDir() const override { return szBaseDir.c_str(); }
  //
  void ConfigureNet(const int nAppID, const int nPort) override
  {
    nNetAppID = nAppID;
    nNetPort = nPort;
  }

  //
  bool StepApp(bool bActive) override;
  void Command(IInterfaceCommand *pCommand) override;
  void Command(int nCommandID, const char *pszConfiguration) override;
  // work with interfaces
  void ResetStack() override;
  void SetInterface(IInterfaceBase *pNewInterface) override;
  void PushInterface(IInterfaceBase *pNewInterface) override;
  void PopInterface() override;
  IInterfaceBase * GetInterface() const override;
  //
  void Pause(bool _bPause, int _nPauseReason) override;
  bool IsPaused() const override { return bPaused; }
  void EnableMessageProcessing(bool bEnable) override;
  // clear all unreferenced resources
  void ClearResources(bool bClearAll) override;
  //
  void StoreScenarioTracker() override;
  void RestoreScenarioTracker() override;
  void SerializeConfig(bool bRead, DWORD dwSerialize) override;
  void Serialize(IStructureSaver *pSS, interface IProgressHook *pHook) override;
};

struct SProgressMovieInfo
{
  std::string szMovieName;
  std::string szTextSource;
  CVec2 vTextTop;
  CVec2 vTextBottom;
  int nTextAlign;
  DWORD dwTextColor;
  int nFontSize;

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;
    saver.Add("MovieName", &szMovieName);
    saver.Add("TextSource", &szTextSource);
    saver.Add("TextTop", &vTextTop);
    saver.Add("TextBottom", &vTextBottom);
    saver.Add("TextColor", &dwTextColor);
    saver.Add("FontSize", &nFontSize);
    saver.Add("TextAlign", &nTextAlign);
    return 0;
  }
};

class CProgressScreen : public IMovieProgressHook
{
  OBJECT_NORMAL_METHODS(CProgressScreen);
  int nNumSteps;// total number of steps
  int nCurrentStep;// current step
  int nNumFrames;// total number of frames
  int nCurrFrame;// current frame
  int nMaxFrame;// max frame to play
  CPtr<IGFX> pGFX;// where do we draw?
  CPtr<IVideoPlayer> pVP;// what we draw with
  CPtr<IGFXText> pGFXText;// text to draw on top of the cartoon
  DWORD dwTextColor;// text color
  CTRect<float> wndRect;
  int nTextAlign;
  int nFontSize;
  //
  void Draw();
  void SetText(const SProgressMovieInfo *pInfo);

public:
  CProgressScreen()
    : nNumSteps(0), nCurrentStep(0), nMaxFrame(0), nFontSize(1) {}

  void Init(EProgressType nType) override;
  void Init(const std::string &szMovieName) override;
  void Stop() override;
  // set total num steps
  void SetNumSteps(int nRange, float fPercentage = 1.0f) override;
  // do one step
  void Step() override;
  // recover during loading
  void Recover() override;
  // get/set current position
  void SetCurrPos(int nPos) override;
  int GetCurrPos() const override;
};

#endif // __IMAININTERNAL_H__