#ifndef __MOUNIT_H__
#define __MOUNIT_H__

#pragma once

#include "MapObject.h"
#include "../Scene/Scene.h"
#include "../Anim/Animation.h"
#include "../Main/TextSystem.h"

class CMOUnit : public CTRefCount<IMOUnit>
{
  DECLARE_SERIALIZE;

protected:
  enum { AMMO_TYPE_PRIMARY = 0, AMMO_TYPE_SECONDARY = 1 };

  enum EUnitState
  {
    STATE_IDLE = 0,
    STATE_MOVE = 1,
    STATE_AMBUSH = 2,
    STATE_DIE = 3
  };

private:
  int ammos[2];// primary and secondary ammo counts
  float fAmmoValue;// ammo value (minimum respection curr_ammo/max_ammo)
  float fMorale;// morale
  EUnitState eState;// current unit's state
  CPtr<IMOContainer> pContainer;// AI object, this units contained in
  mutable CPtr<IText> pLocalName;// localized name of this unit (ZB "Medium tank T-34")
  bool bVisible;// is this unit visible by others?
  static int nSeason;// season (one for all)
  static DWORD dwFlashFireColor;// flash color during firing(one for all units)
  static DWORD dwFlashExpColor;// flash color in explosion(one for all units)
  int nScenarioIndex;// scenario unit index
  int nPlayerIndex;// player index
  CPtr<IUnitStateObserver> pObserver;// icon updater for 'who-in-container' interface
  DWORD dwIconFlags;// icons by bits
  //
  virtual const CVec3 GetIconAddValue() const = 0;

protected:
  bool OnCreate();
  //
  void CommonUpdateRPGStats(float fNewHP, const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, bool bUpdateHPBar = true);
  bool CommonUpdateHP(float fNewHP, bool bUpdateHPBar = true);
  const int GetAmmo(const int nIndex) const { return ammos[nIndex]; }
  //
  void SetSeason(const int _nSeason) { nSeason = _nSeason; }
  const int GetSeason() const { return nSeason; }
  //
  void SetScenarioIndex(const int nIndex) { nScenarioIndex = nIndex == -1 ? -2 : nIndex; }
  const int GetScenarioIndex() const { return nScenarioIndex; }
  //
  void SetVisible(const bool _bVisible)
  {
    bVisible = _bVisible;
    MakeVisible(bVisible);
  }

  virtual void MakeVisible(bool bVisible) = 0;
  const bool IsVisibleLocal() const { return bVisible; }
  //
  void GetActionsLocal(EActionsType eActions, CUserActions *pActions) const;
  const bool IsDead() const { return fHP <= 0.0f; }
  DWORD GetFlashFireColor() const { return dwFlashFireColor; }
  DWORD GetFlashExpColor() const { return dwFlashExpColor; }
  //
  EUnitState GetCurrState() const { return eState; }
  void SetCurrState(EUnitState state) { eState = state; }
  //
  const bool CanAddIcon(int nType) const;
  const bool CanShowIcons() const;
  void SetIcon(int nType, IVisObjBuilder *pVOB);
  void RemoveIcon(int nType);
  //
  const SUnitBaseRPGStats::SAnimDesc *GetAnimDesc(const DWORD dwAnim) const
  {
    const int dwAnimType = (dwAnim >> 16) & 0x0fff;
    const std::vector<SUnitBaseRPGStats::SAnimDesc> &animdescs = static_cast_gdb<const SUnitBaseRPGStats *>(pRPG)->animdescs[dwAnimType];
    return animdescs.empty() ? nullptr : &(animdescs[dwAnim & 0xffff]);
  }

  //
  IText *GetLocalNameLocal() const
  {
    if (pLocalName == nullptr) pLocalName = ::GetLocalName(pDesc);
    return pLocalName;
  }

  void ClearLocalName() { pLocalName = nullptr; }
  IUnitStateObserver *GetObserver() { return pObserver; }
  //
  void UpdateLevel(int nLevel);
  void UpdateGunTraces(const CVec3 &vStart, const CVec3 &vEnd, float fSpeed, NTimer::STime nCurrTime, IScene *pScene);
  void SendDeathAcknowledgement(interface IClientAckManager *pAckManager, unsigned int nTimeAfterStart);

public:
  CMOUnit();
  //
  void STDCALL PrepareToRemove() override {}
  // check, is this object selected?
  bool STDCALL IsSelected() const override { return pVisObj->GetSelectionState() == SGVOSS_SELECTED; }
  // placement
  void STDCALL SetPlacement(const CVec3 &vPos, const WORD &wDir) override;
  void STDCALL GetPlacement(CVec3 *pvPos, WORD *pwDir) override;
  // stats functions
  const SGDBObjectDesc * STDCALL GetDesc() const override { return pDesc; }
  const SHPObjectRPGStats * STDCALL GetRPG() const override { return pRPG; }
  // AI object retrieving
  IRefCount * STDCALL GetAIObj() override { return pAIObj; }
  IRefCount * STDCALL GetParentAIObj() override { return pContainer; }
  // get status for mission status bar
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override;
  // is unit visible?
  const bool STDCALL IsVisible() const override { return IsVisibleLocal(); }
  // assign selection group
  void STDCALL AssignSelectionGroup(int nGroupID) override;
  // load unit onboard or unload it
  void STDCALL SetContainer(IMOContainer *_pContainer) override
  {
    pContainer = _pContainer;
    if (pContainer == nullptr) static_cast_ptr<IObjVisObj *>(pVisObj)->SetVisible(IsVisibleLocal());
    else static_cast_ptr<IObjVisObj *>(pVisObj)->SetVisible(false);
  }

  IMOContainer * STDCALL GetContainer() const override { return pContainer; }
  void STDCALL SetSquad(interface IMOSquad *pSquad) override {}
  interface IMOSquad * STDCALL GetSquad() override { return nullptr; }
  // common updates
  void STDCALL AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateDiplomacy(const struct SAINotifyDiplomacy &diplomacy) override;
  // unit's update
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void STDCALL AIUpdateAcknowledgement(EUnitAckType eAck, IClientAckManager *pAckManager, int nSet) override;
  void STDCALL AIUpdateBoredAcknowledgement(const SAIBoredAcknowledgement &ack, IClientAckManager *pAckManager) override;
  void STDCALL SendAcknowledgement(interface IClientAckManager *pAckManager, EUnitAckType eAckType, int nSet) override;
  // general update. 
  bool STDCALL Update(const NTimer::STime &currTime) override { return true; }
  // unit's updates
  void STDCALL AIUpdateAiming(const struct AIUpdateAiming &aiming) override {}
  // CRAP{ for animations testing
  void STDCALL AddAnimation(const SUnitBaseRPGStats::SAnimDesc *pDesc) override {}
  // CRAP}
  void STDCALL RemoveSounds(interface IScene *pScene) override {}
  // retrieve localized name
  interface IText * STDCALL GetLocalName() const override { return GetLocalNameLocal(); }
  // set icon update hook
  void STDCALL SetObserver(IUnitStateObserver *pObserver) override;
  int STDCALL GetPlayerIndex() const override { return nPlayerIndex; };
};

#endif // __MOUNIT_H__