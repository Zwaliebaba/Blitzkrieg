#ifndef __TRANSPORT_STATES__
#define __TRANSPORT_STATES__

#pragma once

#include "UnitStates.h"
#include "StatesFactory.h"
#include "CLockWithUnlockPossibilities.h"

class CFormation;
class CAIUnit;
class CBuildingStorage;
class CArtillery;
interface IStaticPath;

class CTransportStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CTransportStatesFactory);

  static CPtr<CTransportStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();
  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  // for Saving/Loading of static members
  friend class CStaticMembers;
};

class CTransportWaitPassengerState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportWaitPassengerState);
  DECLARE_SERIALIZE;

  class CMilitaryCar *pTransport;
  std::list<CPtr<CFormation>> formationsToWait;

public:
  static IUnitState *Instance(class CMilitaryCar *pTransport, class CFormation *pFormation);

  CTransportWaitPassengerState() : pTransport(nullptr) {}
  CTransportWaitPassengerState(class CMilitaryCar *pTransport, class CFormation *pFormation);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_WAIT_FOR_PASSENGER; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  void AddFormationToWait(class CFormation *pFormation);
};

// state of unloading of soldiers from transport
//
class CTransportLandState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportLandState);
  DECLARE_SERIALIZE;

  enum ELandStates { ELS_STARTING, ELS_MOVING, ELS_LANDING };

  ELandStates state;

  class CMilitaryCar *pTransport;

  CVec2 vLandPoint;

  //
  void LandPassenger(class CSoldier *pLandUnit);
  const SVector GetLandingPoint();

public:
  static IUnitState *Instance(class CMilitaryCar *pTransport, const CVec2 &vLandPoint);

  CTransportLandState() : pTransport(nullptr) {}
  CTransportLandState(class CMilitaryCar *pTransport, const CVec2 &vLandPoint);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_LAND; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CTransportLoadRuState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportLoadRuState);
  DECLARE_SERIALIZE;

  enum ETransportLoadRuState
  {
    ETLRS_SEARCH_FOR_STORAGE,
    ETLRS_APPROACHING_STORAGE,
    ETLRS_START_LOADING_RU,
    ETLRS_LOADING_RU,
    ETLRS_WAIT_FOR_LOADERS,

    ETLRS_SUBSTATE_FINISHED,
  };

  ETransportLoadRuState eState;

  CPtr<CBuildingStorage> pStorage;
  CPtr<CFormation> pLoaderSquad;// crowd of movers
  class CAITransportUnit *pTransport;
  int nEntrance;
  bool bSubState;// is this state a substate

  void CreateSquad();

  CBuildingStorage *FindNearestSource();
  void Interrupt();

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, bool bSubState = false, CBuildingStorage *_pPreferredStorage = nullptr);

  CTransportLoadRuState() : pTransport(nullptr) {}
  CTransportLoadRuState(class CAITransportUnit *pTransport, bool bSubState, CBuildingStorage *_pPreferredStorage);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  bool IsSubStateFinished() const { return eState == ETLRS_SUBSTATE_FINISHED; }
};

class CTransportServeState : public IUnitState
{
  DECLARE_SERIALIZE;

  enum ETransportServeState
  {
    ETRS_WAIT_FOR_UNLOCK,
    ETRS_INIT,

    ETRS_START_APPROACH,
    ETRS_APPROACHING,
    ETRS_CREATE_SQUAD,
    ETRS_FINDING_UNIT_TO_SERVE,
    ETRS_LOADERS_INROUTE,

    ETRS_GOING_TO_STORAGE,

    ETRS_WAIT_FOR_LOADERS,
    ETRS_WAIT_FOR_UNIT_TO_SERVE,
  };

  ETransportServeState eState;
  CVec2 vServePoint;// senter of serving circle
  CPtr<CAIUnit> pResupplyUnit;// unit that is being reloaded
  NTimer::STime timeLastUpdate;// time of last behavior update.

  CPtr<IStaticPath> pStaticPath;
  bool bWaitForPath;
  void CreateSquad();

protected:
  bool bUpdatedActionsBegin;
  CPtr<CFormation> pLoaderSquad;// crowd of movers
  CPtr<CAIUnit> pPreferredUnit;// unit that is served first
  class CAITransportUnit *pTransport;

  virtual bool FindUnitToServe(bool *pIsNotEnoughRU) = 0;

  virtual void SendLoaders() = 0;

  virtual void UpdateActionBegin() = 0;

public:
  CTransportServeState() : bUpdatedActionsBegin(false), pTransport(nullptr) {}
  CTransportServeState(class CAITransportUnit *pTransport, const CVec2 &_vServePoint, CAIUnit *_pPreferredUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vServePoint; }
};

class CTransportResupplyState : public CTransportServeState
{
  OBJECT_COMPLETE_METHODS(CTransportResupplyState);
  DECLARE_SERIALIZE;

protected:
  bool FindUnitToServe(bool *pIsNotEnoughRU) override;
  void SendLoaders() override;
  void UpdateActionBegin() override;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const CVec2 &_vServePoint, CAIUnit *_pPreferredUnit);

  CTransportResupplyState() {}
  CTransportResupplyState(class CAITransportUnit *pTransport, const CVec2 &_vServePoint, CAIUnit *_pPreferredUnit);

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_RESUPPLY_UNIT; }
};

class CTransportRepairState : public CTransportServeState
{
  OBJECT_COMPLETE_METHODS(CTransportRepairState);
  DECLARE_SERIALIZE;

protected:
  bool FindUnitToServe(bool *pIsNotEnoughRU) override;
  void SendLoaders() override;
  void UpdateActionBegin() override;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const CVec2 &_vServePoint, CAIUnit *_pPreferredUnit);

  CTransportRepairState() {}
  CTransportRepairState(class CAITransportUnit *pTransport, const CVec2 &_vServePoint, CAIUnit *_pPreferredUnit);

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_REPAIR_UNIT; }
};

class CTransportResupplyHumanResourcesState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportResupplyHumanResourcesState);
  DECLARE_SERIALIZE;

  enum ETransportResupplyHumanResourcesState
  {
    ETSHR_GOTO_STORAGE,

    ETSHR_ESTIMATING,
    ETSHR_APPROACHNIG,
    ETSHR_SERVING,

    ETSHR_START_SERVE_SQUAD,
    ETSHR_SERVING_SQUAD,

    ETSHR_START_SERVE_ARTILLERY,
    ETSHR_SERVING_ARTILLERY,

    ETSHR_WAIT_FOR_UNITS,
  };

  ETransportResupplyHumanResourcesState eState;
  class CAITransportUnit *pTransport;
  CVec2 vServePoint;// senter of serving circle
  std::list<CPtr<CFormation>> notCompleteSquads;
  std::list<CPtr<CArtillery>> emptyArtillery;
  NTimer::STime timeLastUpdate;
  CPtr<CArtillery> pPreferredUnit;

  bool bWaitForPath;

  // return true if artillery is served
  bool ServeArtillery(CArtillery *pArtillery);
  // return true if squad is served
  bool ServeSquad(CFormation *pSquad);

  void FindNotCompleteSquads(std::list<CPtr<CFormation>> *pSquads) const;
  void FindEmptyArtillery(std::list<CPtr<CArtillery>> *pArtillerys, CArtillery *_pPreferredUnit) const;
  bool CheckArtillery(CAIUnit *pU) const;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const CVec2 &vServePoint, CArtillery *_pPreferredUnit);

  CTransportResupplyHumanResourcesState() {}
  CTransportResupplyHumanResourcesState(class CAITransportUnit *pTransport, const CVec2 &vServePoint, CArtillery *_pPreferredUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_HUMAN_RESUPPLY; }
};

// loading cannon
class CTransportHookArtilleryState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportHookArtilleryState);
  DECLARE_SERIALIZE;

  enum ETransportTakeGunState
  {
    TTGS_ESTIMATING,
    TTGS_APPROACHING,
    TTGS_START_UNINSTALL,
    TTGS_START_APPROACH_BY_MOVE_BACK,
    TTGS_APPROACH_BY_MOVE_BACK,
    TTGS_START_APPROACH_BY_CHEAT_PATH,
    TTGS_APPROACH_BY_CHEAT_PATH,
    TTGS_WAIT_FOR_UNINSTALL,
    TTGS_WAIT_FOR_TURN,
    TTGS_WAIT_FOR_CREW,
    TTGS_SEND_CREW_TO_TRANSPORT,
    TTGS_WAIT_FOR_LEAVE_TANKPIT,
  };

  ETransportTakeGunState eState;

  CPtr<CArtillery> pArtillery;
  class CAITransportUnit *pTransport;
  CVec2 vArtilleryPoint;

  NTimer::STime timeLast;

  WORD wDesiredTransportDir;// where will the transport be directed during loading?
  bool bInterrupted;
  bool CanInterrupt();
  void InterruptBecauseOfPath();

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, class CArtillery *pArtillery);

  CTransportHookArtilleryState() {}
  CTransportHookArtilleryState(class CAITransportUnit *pTransport, class CArtillery *pArtillery);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_HOOK_ARTILLERY; }
};


// artillery uncoupling
class CTransportUnhookArtilleryState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTransportUnhookArtilleryState);
  DECLARE_SERIALIZE;

  enum ETransportUnhookGunState
  {
    TUAS_START_APPROACH,
    TUAS_APPROACHING,
    TUAS_ESTIMATING,
    TUAS_ADVANCE_A_LITTLE,
    TUAS_MOVE_A_LITTLE,
    TUAS_MOVE_ARTILLERY_TO_THIS_POINT,
    TUAS_START_UNHOOK,
  };

  ETransportUnhookGunState eState;
  class CAITransportUnit *pTransport;
  CVec2 vDestPoint;
  int nAttempt;// number of attempts to place artillery
  bool bInterrupted;
  bool bNow;// unhook gun right at the current place

  bool CanPlaceUnit(const class CAIUnit *pUnit) const;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint, bool bNow);

  CTransportUnhookArtilleryState() {}
  CTransportUnhookArtilleryState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint, bool bNow);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vDestPoint; }
};

class CTransportBuildState : public IUnitState
{
  DECLARE_SERIALIZE;

  enum ETransportBuildState
  {
    ETBS_ESTIMATE,

    ETBS_WAIT_FOR_ENDPOINT,
    ETBS_END_POINT_READY,

    ETBS_APROACHING_BUILDPOINT,

    ETBS_CREATE_SQUAD,
    ETBS_WAIT_FINISH_BUILD,
    ETBS_START_APPROACH,

    ETBS_LOADING_RESOURCES,

    ETBS_WAIT_FOR_LOADERS,
  };

  ETransportBuildState eState;
  CPtr<CTransportLoadRuState> pLoadRuSubState;

protected:
  CAITransportUnit *pUnit;
  CVec2 vStartPoint;
  CVec2 vEndPoint;
  CPtr<CFormation> pEngineers;

  virtual void SendTransportToBuildPoint() = 0;
  virtual bool HaveToSendEngeneersNow() = 0;
  virtual void SendEngineers() = 0;
  virtual bool IsEndPointNeeded() const = 0;
  // do engineers have enough resources to build
  virtual bool IsEnoughResources() const = 0;
  // did the engineers build everything?
  virtual bool IsWorkDone() const = 0;
  virtual bool MustSayNegative() const { return true; }
  virtual void NotifyGoToStorage() {}

  CTransportBuildState() {}
  CTransportBuildState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint);

public:
  void Segment() override;

  void SetStartPoint(const CVec2 &_vStartPoint) { vStartPoint = _vStartPoint; }

  virtual void SetEndPoint(const CVec2 &_vEndPoint);

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vStartPoint; }
};

class CLongObjectCreation;

class CTransportBuildLongObjectState : public CTransportBuildState
{
  DECLARE_SERIALIZE;

protected:
  CPtr<CLongObjectCreation> pCreation;

  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;

  bool IsEndPointNeeded() const override { return true; }

  CTransportBuildLongObjectState() {}

  CTransportBuildLongObjectState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint, class CLongObjectCreation *pCreation)
    : CTransportBuildState(pTransport, vDestPoint), pCreation(pCreation) {}

public:
  void SetEndPoint(const CVec2 &_vEndPoint) override;

};

class CTransportBuildFenceState : public CTransportBuildLongObjectState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportBuildFenceState);

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  CTransportBuildFenceState() {}
  CTransportBuildFenceState(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  EUnitStateNames GetName() override { return EUSN_BUILD_FENCE; }
};

class CTransportBuildEntrenchmentState : public CTransportBuildLongObjectState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportBuildEntrenchmentState);

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  CTransportBuildEntrenchmentState() {}
  CTransportBuildEntrenchmentState(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  EUnitStateNames GetName() override { return EUSN_BUILD_ENTRENCHMENT; }
};

class CTransportClearMineState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportClearMineState);

  NTimer::STime timeCheckPeriod, timeLastCheck;
  bool bWorkDone;

protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;
  bool MustSayNegative() const override { return false; }
  bool IsEndPointNeeded() const override;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  CTransportClearMineState() {}
  CTransportClearMineState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint);

  EUnitStateNames GetName() override { return EUSN_CLEAR_MINE; }
};

class CTransportPlaceMineState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportPlaceMineState);

  int nNumber;
  bool bWorkDone;
  bool bTransportSent;

protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;

  bool IsEndPointNeeded() const override;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint, float fNumer);

  CTransportPlaceMineState() {}

  CTransportPlaceMineState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint, const float fNumber)
    : CTransportBuildState(pTransport, vDestPoint), nNumber(fNumber), bWorkDone(false), bTransportSent(false) {}

  EUnitStateNames GetName() override { return EUSN_PLACE_MINE; }
};

class CTransportPlaceAntitankState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportPlaceAntitankState);

  bool bWorkFinished;
  bool bSent;

protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;
  bool IsEndPointNeeded() const override;

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, const class CVec2 &vStartPoint);

  CTransportPlaceAntitankState() {}
  CTransportPlaceAntitankState(class CAITransportUnit *pTransport, const class CVec2 &vDestPoint);

  EUnitStateNames GetName() override { return EUSN_PLACE_ANTITANK; }
};

class CFullBridge;

class CTransportRepairBridgeState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportRepairBridgeState);

  CPtr<CFullBridge> pBridgeToRepair;
  bool bSentToBuildPoint;

protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;
  bool IsEndPointNeeded() const override;
  void NotifyGoToStorage() override { bSentToBuildPoint = false; }

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, class CFullBridge *pFullBridge);

  CTransportRepairBridgeState() {}
  CTransportRepairBridgeState(class CAITransportUnit *pTransport, class CFullBridge *pFullBridge);

  EUnitStateNames GetName() override { return EUSN_REPAIR_BRIDGE; }
};

class CBridgeCreation;

class CTransportBuildBridgeState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportBuildBridgeState);

  CPtr<CFullBridge> pFullBridge;
  CPtr<CBridgeCreation> pCreation;
  bool bTransportSent;// transport saw sent to build point
protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;
  bool IsEndPointNeeded() const override;
  void NotifyGoToStorage() override { bTransportSent = false; }

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, class CFullBridge *pFullBridge);

  CTransportBuildBridgeState() {}
  CTransportBuildBridgeState(class CAITransportUnit *pTransport, class CFullBridge *pFullBridge);

  EUnitStateNames GetName() override { return EUSN_BUILD_BRIDGE; }
};

class CBuilding;

class CTransportRepairBuildingState : public CTransportBuildState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CTransportRepairBuildingState);

  CPtr<CBuilding> pBuilding;
  bool bSentToBuildPoint;

protected:
  void SendTransportToBuildPoint() override;
  bool HaveToSendEngeneersNow() override;
  bool IsEnoughResources() const override;
  bool IsWorkDone() const override;
  void SendEngineers() override;
  bool IsEndPointNeeded() const override;
  void NotifyGoToStorage() override { bSentToBuildPoint = false; }

public:
  static IUnitState *Instance(class CAITransportUnit *pTransport, class CBuilding *pBuilding);

  CTransportRepairBuildingState() {}
  CTransportRepairBuildingState(class CAITransportUnit *pTransport, class CBuilding *pBuilding);

  EUnitStateNames GetName() override { return EUSN_REPAIR_BUILDING; }
};

class CMoveToPointNotPresize : public IUnitState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CMoveToPointNotPresize);

  CAIUnit *pTransport;
  CVec2 vPurposePoint;
  float fRadius;

  void SendToPurposePoint();

public:
  static IUnitState *Instance(class CAIUnit *pTransport, const CVec2 &vGeneralCell, float fRadius);

  CMoveToPointNotPresize() {}
  CMoveToPointNotPresize(class CAIUnit *pTransport, const CVec2 &vGeneralCell, float fRadius);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vPurposePoint; }
  EUnitStateNames GetName() override { return EUSN_MOVE_TO_RESUPPLY_CELL; }
};

#endif // __TRANSPORT_STATES__