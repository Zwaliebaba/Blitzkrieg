#ifndef __IMISSIONINTERNAL_H__
#define __IMISSIONINTERNAL_H__

#pragma once

#include "../GFX/GFX.h"
#include "../Scene/Scene.h"
#include "../Input/Input.h"
#include "../Input/InputHelper.h"
#include "../Common/World.h"
#include "../AILogic/AILogic.h"
#include "../Formats/fmtMap.h"
#include "../Common/MapObject.h"
#include "../Common/InterfaceScreenBase.h"
#include "../UI/UI.h"
#include "iMission.h"
#include "../Main/TextSystem.h"
#include "ListControlWrapper.h"

using CPickVisObj = std::pair<IVisObj *, CVec2>;
using CPickVisObjList = std::list<CPickVisObj>;// these objects don't live long, so they don't need ref counting
using CVisObjList = std::vector<IVisObj *>;

class CInterfaceMission : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfaceMission);
  DECLARE_SERIALIZE;
  //
public:
  class CMultiplayerScoresSmall
  {
    DECLARE_SERIALIZE;

    enum EScoresStateName
    {
      ESSN_NONE,
      ESSN_MP_REPLAY,
      ESSN_MP_GAME,
    };

  public:
    //
    class CScoresState : public IRefCount
    {
      DECLARE_SERIALIZE;
      int timeBeforeCapture;
      int nSecondsBeforeCapture;
      int nGameTime;
      bool bVisible;// is whole interface visible

    protected:
      bool bInitted;
      CPtr<IUIDialog> pDialog;
      CPtr<IGameTimer> pTimer;

      virtual void Show(IUIScreen *pUIScreen) = 0;
      virtual void OnFrags(int nFrags, int nParty) = 0;
      virtual void OnFlags(int nFlags, int nParty) = 0;
      virtual void OnTimeBeforeCapture(int nTime) = 0;
      virtual void OnTime(int nTime) = 0;
      virtual void Init(IUIScreen *pUIScreen) = 0;

    public:
      CScoresState() : timeBeforeCapture(-1), nSecondsBeforeCapture(-1), nGameTime(-1), bVisible(false), bInitted(false) {}

      void ProcessMessage(const SGameMessage &msg, IUIScreen *pUIScreen);
      void StepLocal(IUIScreen *pUIScreen);
    };

    //
    class CGameScoresState : public CScoresState
    {
      DECLARE_SERIALIZE;
      OBJECT_COMPLETE_METHODS(CGameScoresState);

      void Show(IUIScreen *pUIScreen) override;
      void OnFrags(int nFrags, int nPlayer) override;
      void OnFlags(int nFlags, int nPlayer) override;
      void OnTimeBeforeCapture(int nTime) override;
      void OnTime(int nTime) override;
      void Init(IUIScreen *pUIScreen) override;

    };

    //
    class CReplayScoresState : public CScoresState
    {
      DECLARE_SERIALIZE;
      OBJECT_COMPLETE_METHODS(CReplayScoresState);

      void Show(IUIScreen *pUIScreen) override;
      void OnFrags(int nFrags, int nPlayer) override;
      void OnFlags(int nFlags, int nPlayer) override;
      void OnTimeBeforeCapture(int nTime) override;
      void OnTime(int nTime) override;
      void Init(IUIScreen *pUIScreen) override;

    };

  private:
    CPtr<CScoresState> pState;

  public:
    CMultiplayerScoresSmall() {}
    void ProcessMessage(const SGameMessage &msg, IUIScreen *pUIScreen);
    void StepLocal(IUIScreen *pUIScreen);
    void Init();
    void Done();
  };

private:
  CMultiplayerScoresSmall multiplayerScoresSmall;
  // for counting of lagged players
public:
  class CPlayerLaggedDialog
  {
    DECLARE_SERIALIZE;

  public:
    enum EDialogStateName
    {
      EWM_LAG,
      EWM_LOADING,
    };

    // for easy tracking of states
    class CDialogState : public IRefCount
    {
      DECLARE_SERIALIZE;

    protected:
      bool bShowedWindow;

    public:
      CDialogState() : bShowedWindow(false) {}
      virtual void Hide(IUIScreen *pUIScreen) = 0;
      virtual void AddPlayer(int nParam, IUIScreen *pUIScreen) = 0;
      virtual void RemovePlayer(int nPlayer, IUIScreen *pUIScreen) = 0;

      virtual EDialogStateName GetName() const = 0;
      virtual bool IsActive() const { return bShowedWindow; }
    };

    // lagged state
    class CDialogStateLagged : public CDialogState
    {
      DECLARE_SERIALIZE;
      OBJECT_COMPLETE_METHODS(CDialogStateLagged);

      struct SPlayerLaggedInfo : IRefCount
      {
        OBJECT_COMPLETE_METHODS(SPlayerLaggedInfo);

      public:
        int nPlayer;
        int nLaggedSeconds;
        SPlayerLaggedInfo() {}
        SPlayerLaggedInfo(const int _nPlayer, const int _nLaggedSeconds) : nPlayer(_nPlayer), nLaggedSeconds(_nLaggedSeconds) {}
        int GetID() const { return nPlayer; }
      };

      // this doesn't serialize, MP only
      CListControlWrapper<SPlayerLaggedInfo, int /* nPlayer */> players;

      void Show(IUIScreen *pUIScreen);

    public:
      void Hide(IUIScreen *pUIScreen) override;
      void AddPlayer(int nParam, IUIScreen *pUIScreen) override;
      void RemovePlayer(int nPlayer, IUIScreen *pUIScreen) override;

      EDialogStateName GetName() const override { return EWM_LAG; }
    };

    // 
    class CDialogStateLoading : public CDialogState
    {
      DECLARE_SERIALIZE;
      OBJECT_COMPLETE_METHODS(CDialogStateLoading);

      struct SPlayerLoadingInfo : IRefCount
      {
        OBJECT_COMPLETE_METHODS(SPlayerLoadingInfo);

      public:
        int nPlayer;
        SPlayerLoadingInfo() {}
        SPlayerLoadingInfo(const int _nPlayer) : nPlayer(_nPlayer) {}
        int GetID() const { return nPlayer; }
      };

      // this doesn't serialize, MP only
      CListControlWrapper<SPlayerLoadingInfo, int /* nPlayer */> players;

      void Show(IUIScreen *pUIScreen);

    public:
      void Hide(IUIScreen *pUIScreen) override;
      void AddPlayer(int nParam, IUIScreen *pUIScreen) override;
      void RemovePlayer(int nPlayer, IUIScreen *pUIScreen) override;

      EDialogStateName GetName() const override { return EWM_LOADING; }
    };

  private:
    CPtr<CDialogState> pState;

  public:
    void ProcessMessage(const SGameMessage &msg, IUIScreen *pUIScreen);
    void StepLocal(IUIScreen *pUIScreen);
  };

private:
  CPlayerLaggedDialog laggedDialog;

  // for displaying timeout messages
  class CTimeoutDialog
  {
    DECLARE_SERIALIZE;

  public:
    CTimeoutDialog() {}

    void ProcessMessage(const SGameMessage &msg, IUIScreen *pUIScreen);
    void StepLocal(IUIScreen *pUIScreen);
  };

  CTimeoutDialog timeoutDialog;
  // shortcut
  CPtr<IClientAckManager> pAckManager;// acknowledgments manager
  CPtr<IAILogic> pAILogic;// singleton AI logic shortcut
  // external data
  CPtr<IFrameSelection> pFrameSelection;// frame selection - shortcut from scene
  // internal data
  CObj<IWorldClient> pWorld;// world - all game data
  // substituted data
  CVec3 vCameraStartPos;// camera start position
  //
  std::string szCurrMapName;// map name
  bool bCycledLaunch;// cycled launch or normal
  // pre-selection temporary structures
  CVisObjList preselectedObjects;
  // selection structures (for editor mode only!!!)
  CPickVisObjList selectedObjects;// currently selected objects
  CPickVisObjList::iterator itCurrSelected;
  CPickVisObj *pSelectedObject;
  // text shortcuts
  CPtr<IGFXText> pTextPause;// pause
  //
  bool bForceRotation;
  // start pause
  int nStartPauseCounter;// mission start pause counter
  // input
  NInput::CCommandRegistrator missionMsgs;
  //
  bool bEditMode;// edit mode (obsolete)
  bool bEnableStatistics;// enable statistics drawing
  //
  int nDirection;// rotation direction
  CVec3 vLastAnchor;// last camera anchor for sync
  int nFPSAveragePeriod;
  //
  bool OpenCurtains() override;
  //
  void BeginSelection(const CVec2 &vPos);
  void EndSelection(const CVec2 &vPos);
  bool PickObjects(CPickVisObjList *pPickedObjects, const CVec2 &pos, EObjGameType type, bool bVisible = false);
  bool PickObjects(CPickVisObjList *pPickedObjects, const CTRect<float> &rect, EObjGameType type, bool bVisible = false);
  void ResetSelection(IVisObj *pObj = nullptr);
  void ResetSelectionLocal();
  void ResetPreSelection();
  bool PreSelectObjects(const CVec2 &pos, EObjGameType type);
  bool PreSelectObjects(const CTRect<float> &rect, EObjGameType type);
  void PreSelectObjects(const CPickVisObjList &picked);
  void AddPreSelectedObjects();

  bool SelectFirstObject(const CVec2 &pos);
  void SelectNextObject();
  bool DropObject(const CVec2 &pos);
  // unit commands
  void DoAction(const SGameMessage &msg);
  void BeginAction(const SGameMessage &msg);
  //
  void GetPos3(CVec3 *pPos, const CVec2 &pos);
  void GetPos3(CVec3 *pPos, float x, float y);
  //
  void VisualizeFeedback(int nFeedBack, int nParam);
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  virtual bool STDCALL ProcessMessageLocal(const SGameMessage &msg);
  bool STDCALL StepLocal(bool bAppActive) override;
  void STDCALL DrawAdd() override;
  bool STDCALL OnCursorMove(const CVec2 &vPos) override;
  void SetMissionStatusObject(bool bStatus);
  //
  bool MakeMapShot();
  // disable explicit destruction
  ~CInterfaceMission() override;

public:
  CInterfaceMission();
  //
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void STDCALL OnGetFocus(bool bFocus) override;
  bool NewMission(const std::string &_szMapName, bool _bCycledLaunch);
  void CheckResolution();
  void ConfigureInterfacePreferences();
};

class CICMission : public CInterfaceCommandBase<CInterfaceMission, MISSION_INTERFACE_MISSION>
{
  OBJECT_NORMAL_METHODS(CICMission);
  DECLARE_SERIALIZE;
  //
  std::string szMapName;
  bool bCycledLaunch;
  //
  void PreCreate(IMainLoop *pML) override;
  void PostCreate(IMainLoop *pML, CInterfaceMission *pInterface) override;
  //
  CICMission() : bCycledLaunch(false) {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif // __IMISSIONINTERNAL_H__