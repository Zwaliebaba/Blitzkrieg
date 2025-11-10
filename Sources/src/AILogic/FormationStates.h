#ifndef __FORMATION_STATES_H__
#define __FORMATION_STATES_H__

#pragma once

#include "UnitStates.h"
#include "StatesFactory.h"
#include "CommonStates.h"
#include "../Common/Actions.h"
#include "StaticObjects.h"

class CBuilding;
class CEntrenchment;
class CAIUnit;
class CStaticObject;
class CMilitaryCar;
class CAITransportUnit;
class CTank;
class CCommonStaticObject;
class CArtillery;
class CSoldier;
class CFormation;
class CEntrenchmentPart;

interface IEngineerFormationState : IUnitState
{
  virtual void SetHomeTransport(class CAITransportUnit *pTransport) = 0;
};

class CFormationStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CFormationStatesFactory);

  static CPtr<CFormationStatesFactory> pFactory;

public:
  static CFormationStatesFactory *Instance();

  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  // for Saving/Loading of static members
  friend class CStaticMembers;
};

class CFormationRestState : public CCommonRestState
{
  OBJECT_COMPLETE_METHODS(CFormationRestState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;

protected:
  virtual class CCommonUnit *GetUnit() const;

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &guardPoint, WORD wDir);

  CFormationRestState() : pFormation(nullptr) {}
  CFormationRestState(class CFormation *pFormation, const CVec2 &guardPoint, WORD wDir);

  void Segment() override;

  EUnitStateNames GetName() override { return EUSN_REST; }
  bool IsAttackingState() const override { return false; }

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
};

class CFormationMoveToState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationMoveToState);
  DECLARE_SERIALIZE;

  enum { TIME_OF_WAITING = 200 };

  enum EMoveToStates { EMTS_FORMATION_MOVING, EMTS_UNITS_MOVING_TO_FORMATION_POINTS };

  EMoveToStates eMoveToState;

  class CFormation *pFormation;

  NTimer::STime startTime;
  bool bWaiting;

  //
  void FormationMovingState();
  void UnitsMovingToFormationPoints();

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &point);

  CFormationMoveToState() : pFormation(nullptr) {}
  CFormationMoveToState(class CFormation *pFormation, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_MOVE; }
};

class CFormationParaDropState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationParaDropState);
  DECLARE_SERIALIZE;

  enum EParadropState
  {
    EPS_WAIT_FOR_PARADROP_BEGIN,
    EPS_WAIT_FOR_PARADROP_END,
  };

  EParadropState eState;

  class CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation);

  CFormationParaDropState() : pFormation(nullptr) {}
  CFormationParaDropState(class CFormation *pFormation);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
  EUnitStateNames GetName() override { return EUSN_PARTROOP; }
};

class CFormationEnterBuildingState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterBuildingState);
  DECLARE_SERIALIZE;

  enum EEnterBuildingStates { EES_START, EES_RUN_UP, EES_FINISHED, EES_WAIT_FOR_UNLOCK, EES_WAITINIG_TO_ENTER };

  EEnterBuildingStates state;

  class CFormation *pFormation;
  CPtr<CBuilding> pBuilding;
  int nEntrance;

  //
  bool SetPathForRunUp();
  void SendUnitsToBuilding();
  bool IsNotEnoughSpace();

public:
  static IUnitState *Instance(class CFormation *pFormation, class CBuilding *pBuilding);

  CFormationEnterBuildingState() : pFormation(nullptr) {}
  CFormationEnterBuildingState(class CFormation *pFormation, class CBuilding *pBuilding);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_ENTER; }
};

class CFormationEnterEntrenchmentState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterEntrenchmentState);
  DECLARE_SERIALIZE;

  enum EEnterState { EES_START, EES_RUN, EES_WAIT_TO_ENTER, EES_FINISHED };

  EEnterState state;

  class CFormation *pFormation;
  CPtr<CEntrenchment> pEntrenchment;

  //
  bool IsAnyPartCloseToEntrenchment() const;
  bool SetPathForRunIn();
  void EnterToEntrenchment();

public:
  static IUnitState *Instance(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  CFormationEnterEntrenchmentState() : pFormation(nullptr) {}
  CFormationEnterEntrenchmentState(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_ENTER_ENTRENCHMENT; }
};

class CFormationIdleBuildingState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationIdleBuildingState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;
  CPtr<CBuilding> pBuilding;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CBuilding *pBuilding);

  CFormationIdleBuildingState() : pFormation(nullptr) {}
  CFormationIdleBuildingState(class CFormation *pFormation, class CBuilding *pBuilding);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_IN_BUILDING; }

  class CBuilding *GetBuilding() const { return pBuilding; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationIdleEntrenchmentState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationIdleEntrenchmentState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;
  CPtr<CEntrenchment> pEntrenchment;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  CFormationIdleEntrenchmentState() : pFormation(nullptr) {}
  CFormationIdleEntrenchmentState(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_ENTRENCHMENT; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  class CEntrenchment *GetEntrenchment() const { return pEntrenchment; }
};

class CFormationLeaveBuildingState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationLeaveBuildingState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;
  CPtr<CBuilding> pBuilding;
  CVec2 point;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CBuilding *pBuilding, const CVec2 &point);

  CFormationLeaveBuildingState() : pFormation(nullptr) {}
  CFormationLeaveBuildingState(class CFormation *pFormation, class CBuilding *pBuilding, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return point; }

  EUnitStateNames GetName() override { return EUSN_GO_OUT; }
};

class CFormationLeaveEntrenchmentState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationLeaveEntrenchmentState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;
  CPtr<CEntrenchment> pEntrenchment;
  CVec2 point;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CEntrenchment *pEntrenchment, const CVec2 &point);

  CFormationLeaveEntrenchmentState() : pFormation(nullptr) {}
  CFormationLeaveEntrenchmentState(class CFormation *pFormation, class CEntrenchment *pEntrenchment, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return point; }

  EUnitStateNames GetName() override { return EUSN_GO_OUT_ENTRENCHMENT; }
};

class CFormationPlaceMine : public IEngineerFormationState
{
  OBJECT_COMPLETE_METHODS(CFormationPlaceMine);
  DECLARE_SERIALIZE;

  enum EPlaceMineStates
  {
    EPM_WAIT_FOR_HOMETRANSPORT,
    EPM_START,
    EPM_MOVE,
    EPM_WAITING
  };

  EPlaceMineStates eState;
  CPtr<CAITransportUnit> pHomeTransport;

  class CFormation *pFormation;

  CVec2 point;
  int /* enum EMineType */ eType;

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &point, enum SMineRPGStats::EType nType);

  CFormationPlaceMine() : pFormation(nullptr) {}
  CFormationPlaceMine(class CFormation *pFormation, const CVec2 &point, enum SMineRPGStats::EType nType);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return point; }

  void SetHomeTransport(class CAITransportUnit *pTransport) override;
};

class CFormationClearMine : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationClearMine);
  DECLARE_SERIALIZE;

  enum EClearMineStates { EPM_START, EPM_MOVE, EPM_WAIT };

  EClearMineStates eState;

  class CFormation *pFormation;

  CVec2 point;

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &point);

  CFormationClearMine() : pFormation(nullptr) {}
  CFormationClearMine(class CFormation *pFormation, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  EUnitStateNames GetName() override { return EUSN_CLEAR_MINE; }

  const CVec2 GetPurposePoint() const override { return point; }
};

class CFormationAttackUnitState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CFormationAttackUnitState);
  DECLARE_SERIALIZE;

  enum EAttackUnitStates { EPM_MOVING, EPM_WAITING };

  EAttackUnitStates eState;

  class CFormation *pFormation;
  CPtr<CAIUnit> pEnemy;
  bool bSwarmAttack;
  int nEnemyParty;

  //
  void SetToMovingState();
  void SetToWaitingState();

public:
  static IUnitState *Instance(class CFormation *pFormation, class CAIUnit *pEnemy, bool bSwarmAttack);

  CFormationAttackUnitState() {}
  CFormationAttackUnitState(class CFormation *pFormation, class CAIUnit *pEnemy, bool bSwarmAttack);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }

  EUnitStateNames GetName() override;
};

class CFormationAttackCommonStatObjState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CFormationAttackCommonStatObjState);
  DECLARE_SERIALIZE;

  enum EAttackUnitStates { EPM_START, EPM_MOVING, EPM_WAITING };

  EAttackUnitStates eState;

  class CFormation *pFormation;
  CPtr<CStaticObject> pObj;

  //
  void SetToWaitingState();

public:
  static IUnitState *Instance(class CFormation *pFormation, class CStaticObject *pObj);

  CFormationAttackCommonStatObjState() {}
  CFormationAttackCommonStatObjState(class CFormation *pFormation, class CStaticObject *pObj);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return false; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }

  EUnitStateNames GetName() override { return EUSN_ATTACK_STAT_OBJECT; }
};

class CFormationRotateState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationRotateState);
  DECLARE_SERIALIZE;

  class CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation, WORD wDir);

  CFormationRotateState() {}
  CFormationRotateState(class CFormation *pFormation, WORD wDir);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationEnterTransportState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterTransportState);
  DECLARE_SERIALIZE;

  enum { CHECK_PERIOD = 500 };

  enum EEnterTransportStates { EETS_START, EETS_MOVING, EETS_WAIT_FOR_TURRETS_RETURN, EETS_WAITING, EETS_FINISHED, EETS_WAIT_TO_UNLOCK_TRANSPORT };

  EEnterTransportStates eState;

  CFormation *pFormation;
  CPtr<CMilitaryCar> pTransport;
  NTimer::STime lastCheck;
  CVec2 lastTransportPos;
  WORD lastTransportDir;

  //
  bool SetPathToRunUp();
  void SendUnitsToTransport();
  bool IsAllUnitsInside();
  void SetTransportToWaitState();
  // all transport towers are rotated to default position
  bool IsAllTransportTurretsReturned() const;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CMilitaryCar *pTransport);

  CFormationEnterTransportState() {}
  CFormationEnterTransportState(class CFormation *pFormation, class CMilitaryCar *pTransport);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_ENTER_TRANSPORT; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationIdleTransportState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationIdleTransportState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;
  CPtr<CMilitaryCar> pTransport;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CMilitaryCar *pTransport);

  CFormationIdleTransportState() : pFormation(nullptr) {}
  CFormationIdleTransportState(class CFormation *pFormation, class CMilitaryCar *pTransport);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_ON_BOARD; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationEnterTransportNowState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterTransportNowState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;
  CPtr<CMilitaryCar> pTransport;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CMilitaryCar *pTransport);

  CFormationEnterTransportNowState() : pFormation(nullptr) {}
  CFormationEnterTransportNowState(class CFormation *pFormation, class CMilitaryCar *pTransport);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationEnterTransportByCheatPathState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterTransportByCheatPathState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;
  CPtr<CMilitaryCar> pTransport;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CMilitaryCar *pTransport);

  CFormationEnterTransportByCheatPathState() : pFormation(nullptr) {}
  CFormationEnterTransportByCheatPathState(class CFormation *pFormation, class CMilitaryCar *pTransport);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// general code for Repear, Resupply
class CFormationServeUnitState : public IEngineerFormationState
{
  DECLARE_SERIALIZE;

public:
  //
  class CFindUnitPredicate
  {
  public:
    virtual bool HasUnit() = 0;
    // returns true if we have to finish the search
    virtual bool SetUnit(class CAIUnit *pUnit, float fMissedHP, float fDist) = 0;
    virtual void SetNotEnoughRu() = 0;
    static float CalcWeight(const float fMissedHP, const float fDist) { return fMissedHP * SConsts::HP_BALANCE_COEFF + 1000 / fDist; }
  };

  //
  class CFindFirstUnitPredicate : public CFindUnitPredicate
  {
    CPtr<CAIUnit> pUnit;
    bool bNotEnoughtRu;

  public:
    CFindFirstUnitPredicate() : bNotEnoughtRu(false) {}
    bool HasUnit() override { return pUnit; }

    bool SetUnit(class CAIUnit *_pUnit, const float fMissedHP, const float fDist) override
    {
      pUnit = _pUnit;
      return true;
    }

    void SetNotEnoughRu() override { bNotEnoughtRu = true; }
    bool IsNotEnoughRu() const { return bNotEnoughtRu; }
  };

  //
  class CFindBestUnitPredicate : public CFindUnitPredicate
  {
    CPtr<CAIUnit> pUnit;
    bool bNotEnoughtRu;
    float fCurWeight;

  public:
    CFindBestUnitPredicate() : bNotEnoughtRu(false), fCurWeight(0.0f) {}
    bool HasUnit() override { return pUnit; }

    bool SetUnit(class CAIUnit *_pUnit, const float fMissedHP, const float fDist) override
    {
      const float fWeight = CalcWeight(fMissedHP, fDist);
      if (fCurWeight < fWeight)
      {
        pUnit = _pUnit;
        fCurWeight = fWeight;
      }
      return false;
    }

    void SetNotEnoughRu() override { bNotEnoughtRu = true; }
    bool IsNotEnoughRu() const { return bNotEnoughtRu; }
    CAIUnit *GetUnit() { return pUnit; }
  };

  //
protected:
  enum EFormationServiceUnitState
  {
    EFRUS_WAIT_FOR_HOME_TRANSPORT,
    EFRUS_FIND_UNIT_TO_SERVE,
    EFRUS_START_APPROACH,
    EFRUS_APPROACHING,
    EFRUS_START_SERVICE,
    EFRUS_SERVICING,
    EFRUS_WAIT_FOR_UNIT_TO_SERVE,
  };

  EFormationServiceUnitState eState;
  CPtr<CAITransportUnit> pHomeTransport;// transport from which resources are taken for repairs
  float fWorkAccumulator;// accumulation of work in segments
  float fWorkLeft;// the soldiers took so many resources with them
  CPtr<CAIUnit> pPreferredUnit;

public:
  CFormationServeUnitState(CAIUnit *_pPreferredUnit)
    : eState(EFRUS_WAIT_FOR_HOME_TRANSPORT),
      fWorkAccumulator(0),
      fWorkLeft(0),
      pPreferredUnit(_pPreferredUnit) {}

  void SetHomeTransport(class CAITransportUnit *pTransport) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

// reaches the unit and launches this command to each member of the squad
class CFormationRepairUnitState : public CFormationServeUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationRepairUnitState);
  DECLARE_SERIALIZE;

  CFormation *pUnit;
  CPtr<CAIUnit> pUnitInQuiestion;// unit to be serviced
  CPtr<CTank> pTank;
  CVec2 vPointInQuestion;// where is the unit

  NTimer::STime lastRepearTime;
  float fRepCost;
  bool bNearTruck;

  void Interrupt();

  static bool CheckUnit(CAIUnit *pU, CFindUnitPredicate *pPred, float fResurs, const CVec2 &vCenter);

public:
  // the first storage facility we came across for repairs
  class CFindFirstStorageToRepearPredicate : public CStaticObjects::IEnumStoragesPredicate
  {
    bool bHasStor;
    bool bNotEnoughRu;
    const float fMaxRu;// such a reserve of resources
  public:
    CFindFirstStorageToRepearPredicate(const float fMaxRu) : bHasStor(false), bNotEnoughRu(false), fMaxRu(fMaxRu) {}
    bool OnlyConnected() const override { return false; }
    bool AddStorage(class CBuildingStorage *pStorage, float fPathLenght) override;
    bool HasStorage() const { return bHasStor; }
    bool IsNotEnoughRU() const { return bNotEnoughRu; }
  };

  static void FindUnitToServe(const CVec2 &vCenter,
                              int nPlayer,
                              float fResurs,
                              CCommonUnit *pLoaderSquad,
                              CFindUnitPredicate *pPred,
                              CAIUnit *_pPreferredUnit);

  static IUnitState *Instance(class CFormation *pUnit, CAIUnit *_pPreferredUnit);

  CFormationRepairUnitState() : CFormationServeUnitState(nullptr), pUnit(nullptr) {}
  CFormationRepairUnitState(class CFormation *pUnit, CAIUnit *_pPreferredUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_REPAIR_UNIT; }
};

class CFormationResupplyUnitState : public CFormationServeUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationResupplyUnitState);
  DECLARE_SERIALIZE;
  CFormation *pUnit;
  CPtr<CAIUnit> pUnitInQuiestion;// unit to be serviced
  CVec2 vPointInQuestion;// where is the unit
  NTimer::STime lastResupplyTime;

  CPtr<CFormation> pSquadInQuestion;// if the unit that needs to be serviced is a formation, then this is it
  int iCurUnitInFormation;// We are currently serving this soldier
  bool bSayAck;// unit being resupplied must say ack when being resupplied
  bool bNearTruck;

  void Interrupt();
  static bool CheckUnit(CAIUnit *pU, CFindUnitPredicate *pPred, float fResurs, const CVec2 &vCenter);

public:
  static IUnitState *Instance(class CFormation *pUnit, CAIUnit *pPreferredUnit);

  CFormationResupplyUnitState() : CFormationServeUnitState(nullptr), pUnit(nullptr) {}
  CFormationResupplyUnitState(class CFormation *pUnit, CAIUnit *pPreferredUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  static void FindUnitToServe(const CVec2 &vCenter, int nPlayer, float fResurs,
                              CCommonUnit *pLoaderSquad, CFindUnitPredicate *pPred, CAIUnit *_pPreferredUnit);

  EUnitStateNames GetName() override { return EUSN_RESUPPLY_UNIT; }
};

// loading the truck with resources
// approach the warehouse, do Use, and resources are supplied to the truck
class CBuildingStorage;

class CFormationLoadRuState : public CFormationServeUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationLoadRuState);
  DECLARE_SERIALIZE;

  CFormation *pUnit;
  CPtr<CBuildingStorage> pStorage;// we take resources from this storage
  NTimer::STime lastResupplyTime;
  int nEntrance;
  void Interrupt();

public:
  static IUnitState *Instance(class CFormation *pUnit, class CBuildingStorage *pStorage);

  CFormationLoadRuState() : CFormationServeUnitState(nullptr), pUnit(nullptr) {}
  CFormationLoadRuState(class CFormation *pUnit, class CBuildingStorage *pStorage);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

// they run after the transport and sit down as they go.
class CFormationCatchTransportState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationCatchTransportState);
  DECLARE_SERIALIZE;

  enum ECatchState
  {
    E_SENDING,
    E_CHECKING,
  };

  CFormation *pUnit;
  CPtr<CAITransportUnit> pTransportToCatch;// we'll jump in here

  std::list<CPtr<CSoldier>> deleted;// This is not serialized, it is filled and cleared on 1 segment.

  NTimer::STime timeLastUpdate;
  CVec2 vEnterPoint;
  float fResursPerSoldier;// soldiers running into vehicles can bring resources
  ECatchState eState;

  void UpdatePath(CSoldier *pSold, bool bForce = false);
  void Interrupt();

public:
  static IUnitState *Instance(class CFormation *pUnit, class CAITransportUnit *pTransport, float fResursPerSoldier);

  CFormationCatchTransportState() : pUnit(nullptr), fResursPerSoldier(0) {}
  CFormationCatchTransportState(class CFormation *pUnit, class CAITransportUnit *pTransport, float fResursPerSoldier);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// put an anti-tank hedgehog
class CFormationPlaceAntitankState : public IEngineerFormationState
{
  OBJECT_COMPLETE_METHODS(CFormationPlaceAntitankState);
  DECLARE_SERIALIZE;

  enum EFormationPlaceAntitankState
  {
    FPAS_WAIT_FOR_HOMETRANSPORT,
    FPAS_ESITMATING,
    FPAS_APPROACHING,
    FPAS_APPROACHING_2,
    FPAS_START_BUILD,
    FPAS_START_BUILD_2,
    FPAS_BUILDING,
  };

  EFormationPlaceAntitankState eState;

  CPtr<CCommonStaticObject> pAntitank;
  CPtr<CAITransportUnit> pHomeTransport;
  CVec2 vDesiredPoint;// here antitank is going to be built
  float fWorkAccumulator;
  CFormation *pUnit;
  NTimer::STime timeBuild;

public:
  static IUnitState *Instance(class CFormation *pUnit, const CVec2 &vDesiredPoint);

  CFormationPlaceAntitankState() : pUnit(nullptr) {}
  CFormationPlaceAntitankState(class CFormation *pUnit, const CVec2 &vDesiredPoint);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vDesiredPoint; }
  void SetHomeTransport(class CAITransportUnit *pTransport) override;
};

class CLongObjectCreation;

class CFormationBuildLongObjectState : public IEngineerFormationState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CFormationBuildLongObjectState);

  enum EFormationBuildEntrenchState
  {
    ETBS_WAITING_FOR_HOMETRANSPORT,


    FBFS_READY_TO_START,
    FBFS_APPROACHING_STARTPOINT,
    FBFS_BUILD_SEGMENT,

    FBFS_START_APPROACH_SEGMENT,
    FBFS_NEXT_SEGMENT,
    FBFS_CANNOT_BUILD_ANYMORE,
    FBFS_CHECK_FOR_UNITS_PREVENTING,
    FBFS_WAIT_FOR_UNITS,
    FBFS_APPROACH_SEGMENT,
  };

  EFormationBuildEntrenchState eState;
  CFormation *pUnit;

  NTimer::STime lastTime;
  std::list<CPtr<CAIUnit>> unitsPreventing;
  float fWorkLeft;
  CPtr<CAITransportUnit> pHomeTransport;

  CPtr<CLongObjectCreation> pCreation;
  float fCompletion;// degree of readiness of this segment
  void SendUnitsAway(std::list<CPtr<CAIUnit>> *pUnitsPreventing);

public:
  static IUnitState *Instance(class CFormation *pUnit, class CLongObjectCreation *pCreation);

  CFormationBuildLongObjectState() : pUnit(nullptr) {}
  CFormationBuildLongObjectState(class CFormation *pUnit, class CLongObjectCreation *pCreation);

  void Segment() override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1, -1); }

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  void SetHomeTransport(class CAITransportUnit *pTransport) override;
  EUnitStateNames GetName() override { return EUSN_BUILD_LONGOBJECT; }

};

// give yourself as a service team
class CFormationCaptureArtilleryState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationCaptureArtilleryState);
  DECLARE_SERIALIZE;

  enum EFormationCaptureArtilleryState
  {
    FCAS_ESTIMATING,
    FCAS_APPROACHING,
  };

  EFormationCaptureArtilleryState eState;

  class CFormation *pUnit;
  CPtr<CArtillery> pArtillery;
  std::vector<CPtr<CAIUnit>> usedSoldiers;

public:
  static IUnitState *Instance(class CFormation *_pUnit, CArtillery *pArtillery, bool bUseFormationPart);

  CFormationCaptureArtilleryState() {}
  CFormationCaptureArtilleryState(class CFormation *_pUnit, CArtillery *pArtillery, bool bUseFormationPart);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_GUN_CAPTURE; }
};

// maintenance of guns by artillerymen. 
class CFormationGunCrewState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationGunCrewState);
  DECLARE_SERIALIZE;

  struct SCrewAnimation
  {
    EActionNotify eAction;
    WORD wDirection;
    //
    SCrewAnimation() : eAction(ACTION_NOTIFY_NONE), wDirection(0) {}

    SCrewAnimation(EActionNotify eAction, WORD wDirection)
      : eAction(eAction), wDirection(wDirection) {}
  };

  struct SUnit
  {
    DECLARE_SERIALIZE;
    EActionNotify eAction;
    EActionNotify eNewAction;
    bool bForce;
    WORD wDirection;
    NTimer::STime timeNextUpdate;

  public:
    CPtr<CSoldier> pUnit;
    CVec2 vServePoint;

    void UpdateAction();
    void SetAction(const struct SCrewAnimation &rNewAnim, bool force = false);
    void ResetAnimation();
    inline bool IsAlive() const;
    SUnit();
    SUnit(class CSoldier *pUnit, const CVec2 &vServePoint, EActionNotify eAction = ACTION_NOTIFY_IDLE);
  };

  struct SCrewMember : SUnit
  {
    DECLARE_SERIALIZE;

  public:
    bool bOnPlace;

    SCrewMember();
    SCrewMember(const CVec2 &vServePoint, class CSoldier *pUnit = nullptr, EActionNotify eAction = ACTION_NOTIFY_IDLE);
  };

  enum EGunServeState
  {
    EGSS_OPERATE = 0,
    EGSS_ROTATE = 1,
    EGSS_MOVE = 2,
  };

  enum EGunOperateSubState
  {
    EGOSS_AIM,
    EGOSS_AIM_VERTICAL,
    EGOSS_RELAX,
    EGOSS_RELOAD,
  };

  int nReloadPhaze;// reboot is divided into several phases
  bool b360DegreesRotate;// gun has no horizontal constraints

  // gun state (as well as injection into the vGunners array of gun stats)
  EGunServeState eGunState;

  // gun substates in Operate mode
  EGunOperateSubState eGunOperateSubState;

  std::vector<SCrewMember> crew;// Seats with a lower number have higher priority

  CFormation *pUnit;
  CPtr<CArtillery> pArtillery;
  CGDBPtr<SMechUnitRPGStats> pStats;

  using CFreeUnits = std::list<SUnit>;
  CFreeUnits freeUnits;
  NTimer::STime startTime;
  NTimer::STime timeLastUpdate;

  bool bReloadInProgress;

  float fReloadPrice;// price of one recharge
  float fReloadProgress;// current recharge state
  bool bSegmPriorMove;

  WORD wGunTurretDir;
  WORD wGunBaseDir;
  WORD wTurretHorDir;// previous direction of the barrel
  WORD wTurretVerDir;// previous direction of the barrel
  int nFormationSize;
  CVec2 vGunPos;

  // resetting all distributions - to arrange the artillerymen in a new way
  // return true - end the state
  bool ClearState();

  // for each EGunServeState and unit number it produces the necessary animation
  SCrewAnimation CalcNeededAnimation(int iUnitNumber) const;
  SCrewAnimation CalcAniamtionForMG(int iUnitNumber) const;

  WORD CalcDirToAmmoBox(int nCrewNumber) const;
  WORD CalcDirFromTo(int nCrewNumberFrom, int nCrewNumberTo) const;

  void SetAllAnimation(EActionNotify action, bool force = false);
  void RecountPoints(const CVec2 &gunDir, const CVec2 &vTurretDir);
  void SendThatAreNotOnPlace(bool bNoAnimation);
  int CheckThatAreOnPlace();
  void RefillCrew();
  void UpdateAnimations();
  void Interrupt();
  bool CanInterrupt();
  bool IsGunAttacking() const;

public:
  static IUnitState *Instance(class CFormation *pUnit, CArtillery *pArtillery);

  CFormationGunCrewState() : pUnit(nullptr) {}
  CFormationGunCrewState(class CFormation *pUnit, CArtillery *pArtillery);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_GUN_CREW_STATE; }

  CArtillery *GetArtillery() const { return pArtillery; }
};

// 
class CFormationInstallMortarState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationInstallMortarState);
  DECLARE_SERIALIZE;

  CFormation *pUnit;
  NTimer::STime timeInstall;
  CPtr<CArtillery> pArt;
  int nStage;

public:
  static IUnitState *Instance(class CFormation *pUnit);
  CFormationInstallMortarState() : pUnit(nullptr) {}
  CFormationInstallMortarState(class CFormation *pUnit);
  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1, -1); }
};

class CFormationUseSpyglassState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationUseSpyglassState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &point);

  CFormationUseSpyglassState() : pFormation(nullptr) {}
  CFormationUseSpyglassState(class CFormation *pFormation, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_USE_SPYGLASS; }
};

// to attack a platoon with a platoon
class CFormationAttackFormationState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CFormationAttackFormationState);
  DECLARE_SERIALIZE;

  CFormation *pUnit;
  CPtr<CFormation> pTarget;
  bool bSwarmAttack;
  int nEnemyParty;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CFormation *pTarget, bool bSwarmAttack);

  CFormationAttackFormationState() : pUnit(nullptr) {}
  CFormationAttackFormationState(class CFormation *pFormation, class CFormation *pTarget, bool bSwarmAttack);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
};

class CFormationParadeState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationParadeState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;
  NTimer::STime startTime;

public:
  static IUnitState *Instance(class CFormation *pFormation, int nType);

  CFormationParadeState() : pFormation(nullptr) {}
  CFormationParadeState(class CFormation *pFormation, int nType);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_PARADE; }
};

class CFormationDisbandState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationDisbandState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation);

  CFormationDisbandState() : pFormation(nullptr) {}
  CFormationDisbandState(class CFormation *pFormation);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationFormState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationFormState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation);

  CFormationFormState() : pFormation(nullptr) {}
  CFormationFormState(class CFormation *pFormation);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CFormationWaitToFormState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationWaitToFormState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;
  CPtr<CFormation> pFormFormation;
  CPtr<CSoldier> pMainSoldier;
  bool bMain;
  NTimer::STime startTime;

  //
  void FinishState();
  void FormFormation();

public:
  static IUnitState *Instance(class CFormation *pFormation, float fMain, class CSoldier *pMainSoldier);

  CFormationWaitToFormState() : pFormation(nullptr) {}
  CFormationWaitToFormState(class CFormation *pFormation, float fMain, class CSoldier *pMainSoldier);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_WAIT_TO_FORM; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CCatchFormationState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CCatchFormationState);
  DECLARE_SERIALIZE;

  enum ECatchFormationState { ECFS_NONE, ECFS_FREE, ECFS_IN_BUILDING, ECFS_IN_ENTRENCHMENT, ECFS_IN_TRANSPORT };

  ECatchFormationState eState;

  CVec2 lastFormationPos;
  CPtr<IRefCount> pLastFormationObject;

  // formation that catches
  class CFormation *pCatchingFormation;
  // formation being caught
  CPtr<CFormation> pFormation;
  // time for periodic checks of formation condition
  NTimer::STime lastUpdateTime;

  //
  void LeaveCurStaticObject();
  void MoveSoldierToFormation();
  void JoinToFormation();

  void SetToDisbandedState();
  void AnalyzeFreeFormation();
  void AnalyzeFormationInTransport(class CMilitaryCar *pCar);
  void AnalyzeFormationInEntrenchment(class CEntrenchment *pEntrenchment);
  void AnalyzeFormationInBuilding(class CBuilding *pBuilding);

public:
  static IUnitState *Instance(class CFormation *pCatchingFormation, class CFormation *pFormation);

  CCatchFormationState() : pCatchingFormation(nullptr) {}
  CCatchFormationState(class CFormation *pCatchingFormation, class CFormation *pFormation);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

class CFormationSwarmState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CFormationSwarmState);
  DECLARE_SERIALIZE;

  enum { TIME_OF_WAITING = 200 };

  enum EFormationSwarmStates { EFSS_START, EFSS_WAIT, EFSS_MOVING };

  EFormationSwarmStates state;

  class CFormation *pFormation;

  CVec2 point;
  NTimer::STime startTime;
  bool bContinue;

  //
  void AnalyzeTargetScan();

public:
  static IUnitState *Instance(class CFormation *pFormation, const CVec2 &point, float fContinue);

  CFormationSwarmState() : pFormation(nullptr) {}
  CFormationSwarmState(class CFormation *pFormation, const CVec2 &point, float fContinue);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_SWARM; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override { return point; }

  bool IsAttacksUnit() const override { return false; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
};

class CFullBridge;
class CBridgeSpan;

class CFormationRepairBridgeState : public IEngineerFormationState
{
  OBJECT_COMPLETE_METHODS(CFormationRepairBridgeState);
  DECLARE_SERIALIZE;

  enum EFormationRepearBridgeState
  {
    FRBS_WAIT_FOR_HOMETRANSPORT,

    FRBS_START_APPROACH,
    FRBS_APPROACH,
    FRBS_REPEAR,
  };

  EFormationRepearBridgeState eState;
  class CFormation *pUnit;


  CPtr<CFullBridge> pBridgeToRepair;
  CPtr<CAITransportUnit> pHomeTransport;
  std::vector<CObj<CBridgeSpan>> bridgeSpans;

  float fWorkLeft;// EN that engineers have with them
  float fWorkDone;
  NTimer::STime timeLastCheck;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CFullBridge *pBridge);

  CFormationRepairBridgeState() : pUnit(nullptr) {}
  CFormationRepairBridgeState(class CFormation *pFormation, class CFullBridge *pBridge);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1, -1); }

  void SetHomeTransport(class CAITransportUnit *pTransport) override;
  EUnitStateNames GetName() override { return EUSN_REPAIR_BRIDGE; }

};

class CFormationRepairBuildingState : public IEngineerFormationState
{
  OBJECT_COMPLETE_METHODS(CFormationRepairBuildingState);
  DECLARE_SERIALIZE;

  enum EFormationRepairBuildingState
  {
    EFRBS_WAIT_FOR_HOME_TRANSPORT,
    EFRBS_START_APPROACH,
    EFRBS_APPROACHING,
    EFRBS_START_SERVICE,
    EFRBS_SERVICING,
  };

  EFormationRepairBuildingState eState;

  class CFormation *pUnit;
  CPtr<CAITransportUnit> pHomeTransport;
  CPtr<CBuilding> pBuilding;

  float fWorkAccumulator;
  float fWorkLeft;
  NTimer::STime lastRepearTime;
  void Interrupt();

public:
  static IUnitState *Instance(class CFormation *pFormation, class CBuilding *pBuilding);

  CFormationRepairBuildingState() : pUnit(nullptr) {}
  CFormationRepairBuildingState(class CFormation *pFormation, class CBuilding *pBuilding);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1, -1); }

  void SetHomeTransport(class CAITransportUnit *pTransport) override;

  // returns nearest entrance
  static int SendToNearestEntrance(CCommonUnit *pTransport, CBuilding *pStorage);

  EUnitStateNames GetName() override { return EUSN_REPAIR_BUILDING; }
};

class CFormationEnterBuildingNowState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterBuildingNowState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CBuilding *pBuilding);

  CFormationEnterBuildingNowState() : pFormation(nullptr) {}
  CFormationEnterBuildingNowState(class CFormation *pFormation, class CBuilding *pBuilding);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

class CFormationEnterEntrenchmentNowState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CFormationEnterEntrenchmentNowState);
  DECLARE_SERIALIZE;

  CFormation *pFormation;

public:
  static IUnitState *Instance(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  CFormationEnterEntrenchmentNowState() : pFormation(nullptr) {}
  CFormationEnterEntrenchmentNowState(class CFormation *pFormation, class CEntrenchment *pEntrenchment);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

#endif // __FORMATION_STATES_H__