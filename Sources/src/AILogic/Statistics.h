#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#pragma once

interface IScenarioTracker;

class CStatistics
{
  DECLARE_SERIALIZE;

  CPtr<IScenarioTracker> pScenarioTracker;// scenario tracker shortcut
  bool bEnablePlayerExp;// can we add player exp? 
public:
  CStatistics() : bEnablePlayerExp(false) {}

  void Init();

  // player captured oter player's unit
  void UnitCaptured(int nPlayer);
  // player nPlayer destroyed units of player nKilledUnitsPlayer, nUnits - number of killed, fTotalAIPrice - their price
  void UnitKilled(int nPlayer, int nKilledUnitsPlayer, int nUnits, float fTotalAIPrice);
  // unit died
  void UnitDead(class CCommonUnit *pUnit);
  // nPlayer destroyed house
  void ObjectDestroyed(int nPlayer);
  // nPlayer called the air force
  void AviationCalled(int nPlayer);
  // nPlayer used reinforcement
  void ReinforcementUsed(int nPlayer);
  // nPlayer used resources
  void ResourceUsed(int nPlayer, float fResources);
  // unit received level
  void UnitLeveledUp(class CCommonUnit *pUnit);
  // player's experience
  void IncreasePlayerExperience(int nPlayer, float fPrice);

  void SetFlagPoints(int nParty, float fPoints);
  void SetCapturedFlags(int nParty, int nFlags);

  interface IMissionStatistics *GetPlayerStats(int nPlayer);
};

#endif // __STATISTICS_H__