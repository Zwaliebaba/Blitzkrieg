#include "stdafx.h"

#include "Behaviour.h"
#include "GroupLogic.h"
#include "UnitsIterators2.h"
#include "UnitGuns.h"
#include "Commands.h"
#include "Guns.h"
#include "Diplomacy.h"
#include "Technics.h"
#include "Soldier.h"
#include "Formation.h"
#include "UnitStates.h"
#include "CommonStates.h"
#include "Artillery.h"
#include "StaticObjects.h"
#include "StaticObject.h"
#include "Aviation.h"
#include "HitsStore.h"
#include "Turret.h"

#include "MPLog.h"

extern NTimer::STime curTime;
extern CDiplomacy theDipl;
extern CGroupLogic theGroupLogic;
extern CHitsStore theHitsStore;

// **********************************************************************
// *CShootEstimatorLighAA*
// **********************************************************************

void CShootEstimatorLighAA::Init(CCommonUnit *_pUnit)
{
  pUnit = _pUnit;

  party = pUnit->GetParty();

  pResult = nullptr;
  fWorstDamage = -1;
  bestTime = 100000;
  fMinDistance = 1e10;
  pGun = nullptr;
  bCanShootNow = false;
}

void CShootEstimatorLighAA::Init(class CCommonUnit *pUnit, CBasicGun *_pGun)
{
  Init(pUnit);
  pGun = _pGun;
}

void CShootEstimatorLighAA::AddUnit(CAIUnit *pTarget)
{
  if (IsValidObj(pTarget) && pTarget->GetStats()->IsAviation())
  {
    if (pGun != nullptr && (!pGun->CanShootByHeight(pTarget) || !pGun->CanBreakArmor(pTarget))) return;

    CBasicGun *pChosenGun = pGun;
    // gun is not specified, you need to select it and find the time to kill the enemy
    if (pChosenGun == nullptr)
    {
      int i = 0;
      while (i < pUnit->GetNGuns() && !pUnit->GetGun(i)->CanBreach(pTarget)) ++i;
      if (i >= pUnit->GetNGuns()) return;
      // CRAP { we assume that the anti-aircraft guns have 1 gun
      pChosenGun = pUnit->GetGun(i);
      // CRAP}
    }

    const float fDistance = fabs2(pUnit->GetCenter() - pTarget->GetCenter());
    const float fDamage = pTarget->GetMaxDamage(pUnit);

    // he can be shot through
    if (pChosenGun != nullptr)
    {
      static NTimer::STime timeToShoot = DirsDifference(GetDirectionByVector(pTarget->GetCenter() - pUnit->GetCenter()), pChosenGun->GetGlobalDir());
      const bool bCanShoot = pChosenGun->CanShootToUnit(pTarget);
      if (pResult == nullptr)
      {
        bCanShootNow = bCanShoot;
        pResult = pTarget;
        bestTime = timeToShoot;
        fWorstDamage = fDamage;
        fMinDistance = fDistance;
      }
      else
      {
        // it can cause more damage
        // or can shoot (based on distance)
        // or he will do the same damage, but it’s faster to shoot him
        // or damage and time coincide, but it is closer
        if (fDamage > fWorstDamage ||
            fDamage == fWorstDamage && bCanShoot && !bCanShootNow ||
            fDamage == fWorstDamage && bCanShoot == bCanShootNow && bestTime > timeToShoot ||
            fDamage == fWorstDamage && bestTime == timeToShoot && fDistance < fMinDistance)
        {
          bCanShootNow = bCanShoot;
          pResult = pTarget;
          fWorstDamage = fDamage;
          bestTime = timeToShoot;
          fMinDistance = fDistance;
        }
      }
    }
  }
}

CAIUnit *CShootEstimatorLighAA::GetBestUnit() { return pResult; }

// **********************************************************************
// *CStandartBehaviour*
// **********************************************************************

CAIUnit *CStandartBehaviour::LookForTargetInFireRange(CCommonUnit *pUnit) { return nullptr; }

void CStandartBehaviour::ResetTime(CCommonUnit *pUnit)
{
  underFireAnalyzeTime = 0;
  lastTimeOfRotate = -1;
}

void CStandartBehaviour::UponFire(class CCommonUnit *pUnit, class CAIUnit *pWho, class CAICommand *pCommand)
{
  /* if ( IsValidObj( pWho ) )
     */
}

bool CStandartBehaviour::TryToTraceEnemy(CAIUnit *pUnit)
{
  const int nParty = pUnit->GetParty();
  if (curTime - fleeTraceEnemyTime >= 3000 + Random(0, SConsts::AI_SEGMENT_DURATION * 10) && pUnit->GetNGuns() > 0)
  {
    fleeTraceEnemyTime = curTime;

    int nBestPiercing = -1;
    CBasicGun *pBestGun = nullptr;
    for (int i = 0; i < pUnit->GetNGuns(); ++i)
    {
      CBasicGun *pGun = pUnit->GetGun(i);
      const int nPiercing = pGun->GetPiercing();
      if (pGun->GetNAmmo() != 0 && pGun->GetTurret() != nullptr && (nPiercing > nBestPiercing || pBestGun == nullptr))
      {
        nBestPiercing = nPiercing;
        pBestGun = pUnit->GetGun(i);
      }
    }

    if (pBestGun == nullptr) return false;

    const float r = pUnit->GetSightRadius();
    const float r2 = sqr(r);
    int nBestSides = -1;
    CAIUnit *pBestUnit = nullptr;
    const CVec2 vCenter = pUnit->GetCenter();

    for (CUnitsIter<1, 3> iter(pUnit->GetParty(), EDI_ENEMY, pUnit->GetCenter(), r); !iter.IsFinished() && nBestSides < 3; iter.Iterate())
    {
      CAIUnit *pEnemy = *iter;
      if (!pEnemy->GetStats()->IsAviation() && pEnemy->IsVisible(nParty) && fabs2(pEnemy->GetCenter() - vCenter) < r2)
      {
        int nSides = 0;
        for (int i = 0; i < 4; ++i) { if (pBestGun->CanBreach(pEnemy, i)) ++nSides; }

        if (nSides > nBestSides || pBestUnit == nullptr)
        {
          nBestSides = nSides;
          pBestUnit = pEnemy;
        }
      }
    }

    if (pBestUnit)
    {
      pBestGun->GetTurret()->TraceAim(pBestUnit, pBestGun);
      return true;
    }
    pBestGun->GetTurret()->StopTracing();
    return false;
  }
  for (int i = 0; i < pUnit->GetNTurrets(); ++i)
  {
    CAIUnit *pTracedUnit = pUnit->GetTurret(i)->GetTracedUnit();
    if (pTracedUnit && !pTracedUnit->IsVisible(nParty)) pUnit->GetTurret(i)->StopTracing();
  }

  return false;
}

void CStandartBehaviour::AnalyzeUnderFire(CAIUnit *pUnit)
{
  if (curTime >= underFireAnalyzeTime)
  {
    if (pUnit->IsOperable() && pUnit->GetNTurrets() != 0)
    {
      if (TryToTraceEnemy(pUnit)) lastTimeOfRotate = curTime;
      else if (pUnit->GetTurret(0)->IsFinished())
      {
        if ((theHitsStore.WasHit(pUnit->GetCenter(), 2 * SConsts::RADIUS_OF_HIT_NOTIFY, CHitsStore::EHT_ANY) ||
             lastTimeOfRotate != static_cast<NTimer::STime>(-1) && curTime - lastTimeOfRotate < 10000 + Random(0, 10 * SConsts::AI_SEGMENT_DURATION)))
        {
          // if a tank with a broken caterpillar or anti-aircraft gun
          if (pUnit->GetTurret(0)->GetHorTurnConstraint() != 0 &&
              (!pUnit->CanMove() && pUnit->GetStats()->IsArmor() || pUnit->GetStats()->type == RPG_TYPE_ART_AAGUN)
          )
          {
            if (Random(0.0f, 1.0f) > 0.7f)
            {
              CTurret *pTurret = pUnit->GetTurret(0);
              const WORD wTurretAngle = pTurret->GetHorCurAngle();

              WORD wRotateAngle;
              if (pUnit->GetStats()->IsArmor()) wRotateAngle = Min(static_cast<WORD>(15000), pTurret->GetHorTurnConstraint());
              else wRotateAngle = Min(static_cast<WORD>(32768), pTurret->GetHorTurnConstraint());

              const WORD wMinRotateAngle = wRotateAngle / 4;
              const WORD wMaxRotateAngle = Max(DirsDifference(wTurretAngle, wRotateAngle), DirsDifference(wTurretAngle, -wRotateAngle));

              if (wMaxRotateAngle > 0 && static_cast<int>(wMaxRotateAngle) - static_cast<int>(wMinRotateAngle) + 1 != 0)
              {
                WORD wRotate = Random(wMinRotateAngle, wMaxRotateAngle);

                bool bClockWise = false;
                bool bCounterClockWise = false;

                if (DirsDifference(wTurretAngle + wRotate, 0) <= wRotateAngle) bClockWise = true;
                if (DirsDifference(wTurretAngle - wRotate, 0) <= wRotateAngle) bCounterClockWise = true;

                int nSign;
                if (bClockWise && !bCounterClockWise) nSign = 1;
                if (!bClockWise && bCounterClockWise) nSign = -1;
                if (bClockWise && bCounterClockWise)
                {
                  if (nLastSign == 1) nSign = (Random(0.0f, 1.0f) >= 0.6f) ? 1 : -1;
                  else nSign = (Random(0.0f, 1.0f) >= 0.4f) ? 1 : -1;
                }
                if (!bClockWise && !bCounterClockWise)
                {
                  wRotate = wTurretAngle;
                  nSign = -1;
                }
                nLastSign = nSign;

                const WORD wResAngle = wTurretAngle + nSign * wRotate;
                pTurret->TurnHor(wResAngle);
              }
            }

            if (theHitsStore.WasHit(pUnit->GetCenter(), 2 * SConsts::RADIUS_OF_HIT_NOTIFY, CHitsStore::EHT_ANY)) lastTimeOfRotate = curTime;
          }
        }
        else pUnit->GetTurret(0)->SetCanReturn();
      }
    }

    underFireAnalyzeTime = curTime + SConsts::TIME_OF_HIT_NOTIFY + Random(0, 1000);
  }
}