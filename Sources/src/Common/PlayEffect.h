#ifndef __PLAY_EFFECT_H__
#define __PLAY_EFFECT_H__

#pragma once

#include "Season.h"

inline const std::string *GetHitEffect(const SAINotifyHitInfo &hit, const SWeaponRPGStats::SShell &shell)
{
  switch (hit.eHitType)
  {
    case SAINotifyHitInfo::EHT_HIT: // direct hit
      return &shell.szEffectHitDirect;
    case SAINotifyHitInfo::EHT_MISS: // visual hit, but RPG miss
      return &shell.szEffectHitMiss;
    case SAINotifyHitInfo::EHT_REFLECT: // visual hit, RPG hit, but absorbed
      return &shell.szEffectHitReflect;
  }
  return nullptr;
}

inline IEffectVisObj *PlayEffect(const std::string &szEffect, const CVec3 &vPos,
                                 const NTimer::STime &currTime, bool bOutbound, IScene *pScene, IVisObjBuilder *pVOB,
                                 const NTimer::STime &timeAfterStart = 0, ESoundMixType eMixType = SFX_MIX_IF_TIME_EQUALS,
                                 ESoundAddMode eAddType = SAM_ADD_N_FORGET, ESoundCombatType eCombatType = ESCT_ASK_RPG)
{
  if (szEffect.empty()) return nullptr;
  if (IVisObj *pObj = pVOB->BuildObject(("effects\\effects\\" + szEffect).c_str(), nullptr, SGVOT_EFFECT))
  {
    static_cast<IEffectVisObj *>(pObj)->SetStartTime(currTime);
    pObj->SetPlacement(vPos, 0);
    // add sound
    const std::string &szSoundEffect = static_cast<IEffectVisObj *>(pObj)->GetSoundEffect();
    if (!szSoundEffect.empty()) pScene->AddSound(szSoundEffect.c_str(), vPos, eMixType, eAddType, eCombatType, 1, 100, timeAfterStart);
    // add object to scene
    if (bOutbound) { if (pScene->AddOutboundObject(pObj, SGVOGT_EFFECT) == false) return nullptr; }
    else if (pScene->AddObject(pObj, SGVOGT_EFFECT, nullptr) == false) return nullptr;
    //
    return static_cast<IEffectVisObj *>(pObj);
  }
  return nullptr;
}

inline void SetCraterEffect(const std::string &szCreater, int nSeason, const CVec3 &vPos, int nPriority, IScene *pScene, IVisObjBuilder *pVOB)
{
  const std::string szName = szCreater + GetSeasonApp2(nSeason);
  IVisObj *pObj = pVOB->BuildObject(szName.c_str(), nullptr, SGVOT_SPRITE);
  if (pObj == nullptr) pObj = pVOB->BuildObject(szCreater.c_str(), nullptr, SGVOT_SPRITE);
  NI_ASSERT_SLOW_T(pObj != 0, NStr::Format("Can't create crater picture \"%s\"", szCreater.c_str()));
  pObj->SetPlacement(vPos, 0);
  checked_cast<IObjVisObj *>(pObj)->SetPriority(nPriority);
  pScene->AddCraterObject(pObj, SGVOGT_TERRAOBJ);
}

inline void SetFlashEffect(const SFlashEffect &flash, const NTimer::STime &currTime, const CVec3 &vPos,
                           const DWORD dwColor, IScene *pScene, IVisObjBuilder *pVOB)
{
  NI_ASSERT_SLOW_T(flash.HasFlash(), "Empty flash passed");
  auto pszName = "effects\\sprites\\flash\\1";
  IVisObj *pObj = pVOB->BuildObject(pszName, nullptr, SGVOT_FLASH);
  NI_ASSERT_SLOW_T(pObj != 0, NStr::Format("Can't create flash \"%s\"", pszName));
  pObj->SetPosition(vPos);
  static_cast<IFlashVisObj *>(pObj)->Setup(currTime, flash.nDuration, flash.nPower, dwColor);
  pScene->AddObject(pObj, SGVOGT_FLASH, nullptr);
}

inline void MakeMatrixFromDirection(SHMatrix *pMatrix, const float fHDirection, const float fVDirection = 0)
{
  auto quat = CQuat(-(FP_PI2 - fVDirection), V3_AXIS_X);
  quat *= CQuat(fHDirection, V3_AXIS_Z);
  pMatrix->Set(quat);
}

#endif // __PLAY_EFFECT_H__