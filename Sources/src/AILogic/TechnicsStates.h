#ifndef __TechnicsStates_h__
#define __TechnicsStates_h__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "UnitStates.h"
#include "CLockWithUnlockPossibilities.h"
// //////////////////////////////////////////////////////////// 
// exit from the tank pit.
class CTankPitLeaveState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CTankPitLeaveState);
  DECLARE_SERIALIZE;

  enum ETankLeaveTankPitState
  {
    TLTPS_ESTIMATING,
    TLTPS_MOVING,
  };

  ETankLeaveTankPitState eState;
  class CAIUnit *pUnit;
  NTimer::STime timeStartLeave;

public:
  static IUnitState *Instance(class CAIUnit *pTank);

  CTankPitLeaveState() {}
  CTankPitLeaveState(class CAIUnit *pTank);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

// //////////////////////////////////////////////////////////// 
// bury yourself in the ground (dig yourself in)
class CSoldierEntrenchSelfState : public IUnitState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CSoldierEntrenchSelfState);

  enum ESoldierHullDownState
  {
    ESHD_ESTIMATE,
    ESHD_START_BUILD,
    ESHD_BUILD_PIT,
  };

  CTilesSet tiles;// locked tiles under tank pit that being built
  CVec2 vHalfSize;// half size of tank pit
  CGDBPtr<SMechUnitRPGStats> pStats;// stats of tank pit
  int nDBIndex;// db index of tank pit

  ESoldierHullDownState eState;
  class CAIUnit *pUnit;
  NTimer::STime timeStartBuild;
  CVec2 vTankPitCenter;

  bool CheckTrenches(const CAIUnit *pUnit, const SRect &rectToTest) const;
  bool CheckInfantry(const CAIUnit *pUnit, const SRect &rect) const;

public:
  static IUnitState *Instance(class CAIUnit *pUnit);

  CSoldierEntrenchSelfState() : pUnit(nullptr) {}
  CSoldierEntrenchSelfState(class CAIUnit *pUnit);


  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

};

// //////////////////////////////////////////////////////////// 
#endif // __TechnicsStates_h__