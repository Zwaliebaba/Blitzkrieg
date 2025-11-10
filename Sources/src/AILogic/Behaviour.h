#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#pragma once
// //////////////////////////////////////////////////////////// 
class CCommonUnit;
class CAIUnit;
class CBasicGun;
// //////////////////////////////////////////////////////////// 
// selection of the best air unit (for light anti-aircraft guns) - for shooting resistance
// driver's light
class CShootEstimatorLighAA
{
  BYTE party;

  CPtr<CCommonUnit> pUnit;
  CPtr<CAIUnit> pResult;
  CPtr<CBasicGun> pGun;
  float fWorstDamage;
  NTimer::STime bestTime;
  float fMinDistance;
  bool bCanShootNow;

public:
  CShootEstimatorLighAA()
    : fWorstDamage(-1),
      bestTime(100000),
      fMinDistance(1e10),
      bCanShootNow(false) {}

  void Init(class CCommonUnit *pUnit);
  // choose the optimal unit for a given gun, estimate the time to shoot,
  // only the time it takes for the gun to turn to this unit is taken into account
  void Init(class CCommonUnit *pUnit, CBasicGun *pGun);
  void AddUnit(class CAIUnit *pTarget);
  class CAIUnit *GetBestUnit();
};

// //////////////////////////////////////////////////////////// 
class CStandartBehaviour
{
  DECLARE_SERIALIZE;
  //
  class CAIUnit *LookForTargetInFireRange(class CCommonUnit *pUnit);

  NTimer::STime camouflateTime;

  NTimer::STime underFireAnalyzeTime;
  NTimer::STime lastTimeOfRotate;
  NTimer::STime fleeTraceEnemyTime;
  int nLastSign;

  // track the enemy if there is no one to shoot at and cannot move, and there is a turret
  bool TryToTraceEnemy(class CAIUnit *pUnit);

public:
  CStandartBehaviour() : camouflateTime(0), underFireAnalyzeTime(0), lastTimeOfRotate(-1), fleeTraceEnemyTime(0), nLastSign(1) {}

  void ResetTime(class CCommonUnit *pUnit);
  void UponFire(class CCommonUnit *pUnit, class CAIUnit *pWho, class CAICommand *pCommand);

  void AnalyzeUnderFire(class CAIUnit *pUnit);

  // void StartCamouflating();
  // void AnalyzeCamouflage( class CAIUnit *pUnit );
};

// //////////////////////////////////////////////////////////// 
#endif // __BEHAVIOUR_H__