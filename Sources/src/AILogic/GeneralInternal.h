#ifndef __GENERAL_INTERNAL__
#define __GENERAL_INTERNAL__

#include "General.h"
#include "GeneralInternalInterfaces.h"
#include "Commander.h"
#include "AIHashFuncs.h"
#include "Resistance.h"
#include "../Misc/FreeIDs.h"

#pragma once

class CArtillery;
class CAIUnit;
class CFormation;
class CCommonUnit;
class CGeneralAirForce;
class CGeneralArtillery;
class CGeneralIntendant;

class CGeneral : public CCommander, public IEnemyContainer
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneral);
  CFreeIds requestIDs;
  int nParty;// general is for this player

  using CUnitTimeSeen = std::pair<CPtr<CAIUnit>, NTimer::STime>;
  using CEnemyVisibility = std::unordered_map<int/* unit unique ID */, CUnitTimeSeen>;

  // { do not save these, it is only for IN-Segment use
  CEnemyVisibility::iterator curProcessed;// cannot be saved, so there will be some tricks
  std::list<int> erased;
  // }

  CEnemyVisibility enemys;
  CEnemyVisibility antiAviation;

  CommonUnits infantryInTrenches;// commander strores only unsigned units
  CommonUnits infantryFree;
  CommonUnits tanksFree;
  CommonUnits stationaryTanks;
  CommonUnits transportsFree;

  NTimer::STime timeNextUpdate;// next update of this general
  std::unordered_set<int> mobileReinforcementGroupIDs;

  CPtr<CGeneralAirForce> pAirForce;
  CPtr<CGeneralArtillery> pGeneralArtillery;
  CPtr<CGeneralIntendant> pIntendant;

  using RequestedTasks = std::unordered_map<int/* request ID */, CPtr<IGeneralTask>>;
  RequestedTasks requestedTasks;

  CResistancesContainer resContainer;

  NTimer::STime lastBombardmentCheck;
  // 0 - artillery, 1 - bombers
  BYTE cBombardmentType;
  bool bSendReserves;// send tanks to swarm

  // decision making segment - start art. 
  void BombardmentSegment();
  // give the command to start art. 
  void GiveCommandToBombardment();

  void EraseLastSeen();

public:
  CGeneral()
    : timeNextUpdate(0), lastBombardmentCheck(0) { curProcessed = enemys.end(); }

  CGeneral(const int nParty) : nParty(nParty), timeNextUpdate(0), lastBombardmentCheck(0) {}

  // service functions
  void Init(const struct SAIGeneralSideInfo &mapInfo);
  void Init();
  // new units have appeared
  void GiveNewUnits(const std::list<CCommonUnit *> &pUnits);

  // to manipulate mobile reserves
  bool IsMobileReinforcement(int nGroupID) const;

  // to keep track of visible enemies
  void SetUnitVisible(class CAIUnit *pUnit, bool bVisible);
  void SetAAVisible(class CAIUnit *pUnit, bool bVisible);

  // IEnemyContainer
  void GiveEnemies(IEnemyEnumerator *pEnumerator) override;
  void AddResistance(const CVec2 &vCenter, float fRadius) override;
  void RemoveResistance(const CVec2 &vCenter) override;

  // ICommander
  float GetMeanSeverity() const override { return 0; }
  void EnumWorkers(EForceType eType, IWorkerEnumerator *pEnumerator) override;
  void GiveResistances(IEnemyEnumerator *pEnmumerator) override;

  // upon receipt of reinforcements, it must be given to the general’s control.
  // takes the employee back
  void Give(CCommonUnit *pWorker) override;

  void Segment() override;

  void CancelRequest(int nRequestID, enum EForceType eType) override;
  int /* request ID */CGeneral::RequestForSupport(const CVec2 &vSupportCenter, enum EForceType eType) override;


  // for areas of resistance
  void UpdateEnemyUnitInfo(class CAIUnitInfoForGeneral *pInfo,
                           NTimer::STime lastVisibleTimeDelta, const CVec2 &vLastVisiblePos,
                           NTimer::STime lastAntiArtTimeDelta, const CVec2 &vLastVisibleAntiArtCenter, float fDistToLastVisibleAntiArt);
  void UnitDied(class CAIUnitInfoForGeneral *pInfo);
  void UnitDied(class CCommonUnit *pUnit);
  void UnitChangedParty(CAIUnit *pUnit, int nNewParty);

  // to allow Intendant tracking registered units
  void UnitChangedPosition(class CCommonUnit *pUnit, const CVec2 &vNewPos);
  void UnitAskedForResupply(class CCommonUnit *pUnit, EResupplyType eType, bool bSet);

  void ReserveAviationForTimes(const std::vector<NTimer::STime> &times);

  void SetCellInUse(int nResistanceCellNumber, bool bInUse);
  bool IsInResistanceCircle(const CVec2 &vPoint) const;
};

#endif // __GENERAL_INTERNAL__