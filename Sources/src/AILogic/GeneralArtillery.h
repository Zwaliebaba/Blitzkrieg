#ifndef __GENERAL_ARTILLERY_H__
#define __GENERAL_ARTILLERY_H__

#pragma once

#include "GeneralInternalInterfaces.h"

class CAntiArtillery;
class CAIUnit;

class CGeneralArtilleryGoToPosition : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CGeneralArtilleryGoToPosition);
  DECLARE_SERIALIZE;

  enum EBombardmentState { EBS_START, EBS_MOVING, EBS_WAIT_FOR_TRUCK, EBS_MOVING_WITH_TRUCK, EBS_FINISHING };

  EBombardmentState eState;

  CAIUnit *pUnit;
  CVec2 vPos;
  bool bToReservePosition;
  bool bFinished;
  NTimer::STime timeOfFinish;
  NTimer::STime startTime;

  // 
  void StartState();
  void WaitForTruck();
  void MovingWithTruck();
  void Finishing();

public:
  CGeneralArtilleryGoToPosition() {}
  CGeneralArtilleryGoToPosition(CAIUnit *pUnit, const CVec2 &vPos, bool bToReservePosition);

  void Segment();
  bool IsFinished() { return bFinished; }

  bool DoesGoToReservePosition() const { return bToReservePosition; }
};

class CGeneralArtilleryTask
{
  DECLARE_SERIALIZE;

  bool bBombardmentFinished;

  CVec2 vBombardmentCenter;
  float fBombardmentRadius;
  NTimer::STime timeToFinishBombardment;
  NTimer::STime timeToSendAntiArtilleryAck;
  CVec2 vAntiArtilleryAckCenter;
  bool bIsAntiArtilleryFight;
  NTimer::STime startRotatingTime;

  enum EBombardmentState { EBS_START, EBS_ROTATING, EBS_GOING_TO_BATTLE, EBS_FIRING, EBS_ESCAPING };

  EBombardmentState eState;

  struct SBombardmentUnitState
  {
    DECLARE_SERIALIZE;

  public:
    CVec2 vReservePosition;
    CVec2 vAttackPos;

    CPtr<CAIUnit> pUnit;
    CPtr<CGeneralArtilleryGoToPosition> pGoToPosition;

    SBombardmentUnitState() {}
    explicit SBombardmentUnitState(CAIUnit *pUnit);
  };

  std::list<SBombardmentUnitState> bombardmentUnits;

  class CGeneralArtillery *pOwner;

  int nCellNumber;
  int nParty;

  //
  // start of shelling - give commands to hook up to the trucks and drive
  void StartBombardment();
  // as soon as we arrive, give the command to turn towards the enemy
  void GoingToBattle();
  // as soon as you turn around, give the command to shoot
  void Rotating();
  // when they have fired, give the command to leave for reserve positions
  void Firing();
  // when we arrived at the reserve positions, finish the shelling
  void Escaping();
  // if some of the units for some reason were unable to carry out artillery shelling, trace,
  // so that they return to reserve positions
  void CheckEscapingUnits();

  void CalculateTimeToSendAntiArtilleryAck();

  void SetBombardmentFinished();

public:
  CGeneralArtilleryTask() : bBombardmentFinished(true), bIsAntiArtilleryFight(false) {}
  CGeneralArtilleryTask(CGeneralArtillery *pOwner, std::list<CAIUnit *> &givenUnits, bool bAntiArtilleryFight, const CVec2 &vCenter, float fRadius, int nCellNumber);

  void Segment();

  bool IsTaskFinished() const { return bBombardmentFinished; }
};

class CGeneralArtillery : public IRefCount, public IEnemyEnumerator
{
  OBJECT_COMPLETE_METHODS(CGeneralArtillery);
  DECLARE_SERIALIZE;

  class CGeneral *pGeneralOwner;
  int nParty;

  using CUnitsList = std::list<CPtr<CAIUnit>>;
  CUnitsList freeUnits;
  CUnitsList trucks;

  std::list<CGeneralArtilleryTask> tasks;

public:
  CGeneralArtillery() {}
  CGeneralArtillery(int nParty, CGeneral *pGeneralOwner);

  // give the general a unit to use as artillery
  void TakeArtillery(CAIUnit *pUnit);
  // the same for trucks
  void TakeTruck(CAIUnit *pUnit);

  void Segment();
  bool CanBombardRegion(const CVec2 &vRegionCenter);

  int RequestForSupport(const CVec2 &vCenter, float fRadius, bool bIsAntiArtilleryFight, int nCellNumber);
  void CancelRequest(int nRequestID, enum EForceType eType);

  void SetEnemyContainer(IEnemyContainer *pEnemyContainer);

  // IEnemyEnumerator
  bool EnumEnemy(class CAIUnit *pEnemy) override;

  const int GetParty() const { return nParty; }
  const int GetNFreeUnits() const { return freeUnits.size(); }

  void SetCellInUse(int nResistanceCell, bool bInUse);
};

#endif // __GENERAL_ARTILLERY_H__