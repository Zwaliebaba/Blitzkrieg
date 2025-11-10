#include "stdafx.h"

#include "InBuildingStates.h"
#include "Commands.h"
#include "Soldier.h"
#include "Building.h"
#include "AIStaticMap.h"
#include "Updater.h"
#include "UnitStates.h"
#include "Guns.h"
#include "CommonStates.h"
#include "GroupLogic.h"

extern CStaticMap theStaticMap;
extern CUpdater updater;
extern NTimer::STime curTime;
extern CGroupLogic theGroupLogic;

// **********************************************************************
// *CInBuildingStatesFactory*
// **********************************************************************

CPtr<CInBuildingStatesFactory> CInBuildingStatesFactory::pFactory = nullptr;

IStatesFactory *CInBuildingStatesFactory::Instance()
{
  if (pFactory == nullptr) pFactory = new CInBuildingStatesFactory();

  return pFactory;
}

bool CInBuildingStatesFactory::CanCommandBeExecuted(CAICommand *pCommand)
{
  const EActionCommand &cmdType = pCommand->ToUnitCmd().cmdType;
  return
  (cmdType == ACTION_COMMAND_DIE ||
   cmdType == ACTION_COMMAND_ATTACK_UNIT ||
   cmdType == ACTION_COMMAND_IDLE_BUILDING ||
   cmdType == ACTION_COMMAND_AMBUSH ||
   cmdType == ACTION_COMMAND_DISAPPEAR ||
   cmdType == ACTION_COMMAND_SWARM_ATTACK_UNIT
  );
}

IUnitState *CInBuildingStatesFactory::ProduceState(class CQueueUnit *pUnit, CAICommand *pCommand)
{
  NI_ASSERT_T(dynamic_cast<CSoldier*>( pUnit ) != 0, "Wrong unit type");
  auto pSoldier = static_cast<CSoldier *>(pUnit);

  const SAIUnitCmd &cmd = pCommand->ToUnitCmd();
  IUnitState *pResult = nullptr;

  switch (cmd.cmdType)
  {
    case ACTION_COMMAND_DIE:
      NI_ASSERT_T(false, "Command to die in the queue");

      break;
    case ACTION_COMMAND_SWARM_ATTACK_UNIT:
    case ACTION_COMMAND_ATTACK_UNIT:
    {
      CONVERT_OBJECT_PTR(CAIUnit, pUnit, cmd.pObject, "Wrong unit to attack");
      pResult = CSoldierAttackInBuildingState::Instance(pSoldier, pUnit);
    }

    break;
    case ACTION_COMMAND_IDLE_BUILDING:
      pResult = CSoldierRestInBuildingState::Instance(pSoldier, nullptr);

      break;
    case ACTION_COMMAND_AMBUSH:
      pResult = CCommonAmbushState::Instance(pSoldier);

      break;
    default:
      NI_ASSERT_T(false, "Wrong command");
  }

  return pResult;
}

IUnitState *CInBuildingStatesFactory::ProduceRestState(class CQueueUnit *pUnit)
{
  NI_ASSERT_T(dynamic_cast<CSoldier*>( pUnit ) != 0, "Wrong unit type");
  return CSoldierRestInBuildingState::Instance(static_cast<CSoldier *>(pUnit), nullptr);
}

// **********************************************************************
// *CSoldierRestInBuildingState*
// **********************************************************************

IUnitState *CSoldierRestInBuildingState::Instance(CSoldier *pSoldier, CBuilding *pBuilding)
{
  auto pRest = new CSoldierRestInBuildingState(pSoldier);
  pRest->SendUnitTo(pBuilding);

  return pRest;
}

CSoldierRestInBuildingState::CSoldierRestInBuildingState(CSoldier *_pSoldier)
  : pSoldier(_pSoldier)
{
  startTime = curTime;

  pSoldier->StartCamouflating();
  ResetTime(pSoldier);
}

void CSoldierRestInBuildingState::SendUnitTo(CBuilding *pBuilding)
{
  if (pBuilding != nullptr)
  {
    if (pBuilding->IsValid() && pBuilding->IsAlive() && pBuilding->GetNFreePlaces() != 0) pSoldier->SetInBuilding(pBuilding);
    else pSoldier->SetCommandFinished();
  }
  else
    NI_ASSERT_T(pSoldier->IsInBuilding(), "Wrong unit state");
}

void CSoldierRestInBuildingState::Segment()
{
  if (pSoldier->IsInFollowState() && fabs(pSoldier->GetCenter() - pSoldier->GetFollowedUnit()->GetCenter()) >= SConsts::FOLLOW_GO_RADIUS) theGroupLogic.UnitCommand(SAIUnitCmd(ACTION_COMMAND_FOLLOW_NOW, pSoldier->GetFollowedUnit()), pSoldier, false);
  else if (pSoldier->GetSlot() != -1) pSoldier->AnalyzeTargetScan(nullptr, false, false);
}

ETryStateInterruptResult CSoldierRestInBuildingState::TryInterruptState(class CAICommand *pCommand)
{
  pSoldier->SetCommandFinished();
  return TSIR_YES_IMMIDIATELY;
}

const CVec2 CSoldierRestInBuildingState::GetPurposePoint() const
{
  if (pSoldier && pSoldier->IsValid() && pSoldier->IsAlive()) return pSoldier->GetCenter();
  return CVec2(-1.0f, -1.0f);
}

// **********************************************************************
// *CSoldierAttackInBuildingState*
// **********************************************************************

IUnitState *CSoldierAttackInBuildingState::Instance(CSoldier *pSoldier, CAIUnit *pEnemy) { return new CSoldierAttackInBuildingState(pSoldier, pEnemy); }

CSoldierAttackInBuildingState::CSoldierAttackInBuildingState(class CSoldier *_pSoldier, CAIUnit *_pEnemy)
  : pSoldier(_pSoldier), pEnemy(_pEnemy), bFinish(false), bAim(true), nEnemyParty(_pEnemy->GetParty())
{
  pSoldier->GetBuilding()->Alarm();

  if (!pEnemy->IsAlive()) pSoldier->SendAcknowledgement(ACK_INVALID_TARGET, true);
}

void CSoldierAttackInBuildingState::AnalyzeCurrentState()
{
  // you can shoot
  if (pGun->CanShootToUnitWOMove(pEnemy))
  {
    // shoot
    pSoldier->RegisterAsBored(ACK_BORED_ATTACK);
    pGun->StartEnemyBurst(pEnemy, bAim);
    bAim = false;
  }
  else
  {
    pSoldier->SendAcknowledgement(pGun->GetRejectReason());
    pGun->StopFire();

    pSoldier->UnRegisterAsBored(ACK_BORED_ATTACK);
    damageToEnemyUpdater.UnsetDamageFromEnemy(pEnemy);
    pSoldier->SetCommandFinished();
  }
}

void CSoldierAttackInBuildingState::Segment()
{
  if (!pSoldier->GetBuilding()->IsAnyAttackers() || bFinish)
  {
    if (bFinish && (!pGun.IsValid() || !pGun->IsFiring())) pSoldier->SetCommandFinished();
    else if (!pGun.IsValid() && !bFinish)
    {
      if (IsValidObj(pEnemy))
      {
        pSoldier->ResetShootEstimator(pEnemy, false);
        pGun = pSoldier->GetBestShootEstimatedGun();
        if (pGun == nullptr) pSoldier->SendAcknowledgement(pSoldier->GetGunsRejectReason());
      }

      if (pGun == nullptr)
      {
        pSoldier->UnRegisterAsBored(ACK_BORED_ATTACK);
        damageToEnemyUpdater.UnsetDamageFromEnemy(pEnemy);
        pSoldier->SetCommandFinished();
      }
    }
    // not the moment of shooting
    else if (!pGun->IsFiring())
    {
      damageToEnemyUpdater.SetDamageToEnemy(pSoldier, pEnemy, pGun);

      // if the enemy is dead or is not visible or we shoot on our own or it’s time to stop shooting
      if (!IsValidObj(pEnemy) || pEnemy.GetPtr() == pSoldier ||
          !pEnemy->IsNoticableByUnit(pSoldier, pGun->GetFireRange(0)) || bFinish ||
          nEnemyParty != pEnemy->GetParty())
      {
        if (IsValidObj(pEnemy) && !pEnemy->IsNoticableByUnit(pSoldier, pGun->GetFireRange(0))) pSoldier->SendAcknowledgement(ACK_DONT_SEE_THE_ENEMY);

        pSoldier->UnRegisterAsBored(ACK_BORED_ATTACK);
        damageToEnemyUpdater.UnsetDamageFromEnemy(pEnemy);
        pSoldier->SetCommandFinished();
        pGun->StopFire();
      }
      else AnalyzeCurrentState();
    }
  }
}

ETryStateInterruptResult CSoldierAttackInBuildingState::TryInterruptState(class CAICommand *pCommand)
{
  if (!pCommand)
  {
    pSoldier->UnRegisterAsBored(ACK_BORED_ATTACK);
    damageToEnemyUpdater.UnsetDamageFromEnemy(pEnemy);
    pSoldier->SetCommandFinished();
    return TSIR_YES_IMMIDIATELY;
  }
  const SAIUnitCmd &cmd = pCommand->ToUnitCmd();
  if (cmd.cmdType != ACTION_COMMAND_ATTACK_UNIT || cmd.pObject.GetPtr() != pEnemy)
  {
    if (pGun.IsValid()) pGun->StopFire();

    bFinish = true;
    return TSIR_YES_WAIT;
  }

  return TSIR_NO_COMMAND_INCOMPATIBLE;
}

const CVec2 CSoldierAttackInBuildingState::GetPurposePoint() const
{
  if (IsValidObj(pEnemy)) return pEnemy->GetCenter();
  return CVec2(-1.0f, -1.0f);
}

CAIUnit *CSoldierAttackInBuildingState::GetTargetUnit() const { return pEnemy; }