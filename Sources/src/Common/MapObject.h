#ifndef __MAPOBJECT_H__
#define __MAPOBJECT_H__

#pragma once

#include "../Main/RPGStats.h"
#include "../Scene/PFX.h"
#include "../SFX/SFX.h"
#include "../Scene/Scene.h"
#include "../AILogic/AITypes.h"
#include "../Image/Image.h"

interface IMapObjVisitor
{
  virtual void VisitSprite(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) = 0;
  virtual void VisitMesh(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) = 0;
  virtual void VisitEffect(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) = 0;
};

interface ISelectorVisitor
{
  virtual void VisitMapObject(struct SMapObject *pMO) const = 0;
};

interface ISelector
{
  // select single map object
  virtual bool Select(struct SMapObject *pMO, bool bSelect, bool bSelectSuper) = 0;
  virtual bool IsSelected(const struct SMapObject *pMO) const = 0;
  // done multiple selection operation
  virtual void DoneSelection() = 0;
  // register/unregister/access AI group
  virtual int GetAIGroup() = 0;
  virtual bool IsEmpty() const = 0;
  // visiting objects inside
  virtual void Visit(ISelectorVisitor *pVisitor) const = 0;
};

interface IMapObj : IRefCount
{
  enum EActionsType
  {
    ACTIONS_WITH = 0,
    ACTIONS_BY = 1,
    ACTIONS_ALL = 2
  };

  //
  virtual bool Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) = 0;
  // placement
  virtual void SetPlacement(const CVec3 &vPos, const WORD &wDir) = 0;
  virtual void GetPlacement(CVec3 *pvPos, WORD *pwDir) = 0;
  // stats functions
  virtual const SGDBObjectDesc * GetDesc() const = 0;
  virtual const SHPObjectRPGStats * GetRPG() const = 0;
  // AI object retrieving
  virtual IRefCount * GetAIObj() = 0;
  virtual IRefCount * GetParentAIObj() = 0;
  //
  virtual bool CanSelect() const = 0;
  // get status for mission status bar
  virtual void GetStatus(struct SMissionStatusObject *pStatus) const = 0;
  // get actions, which this object can perform or actions, thi object can be acted with
  virtual void GetActions(CUserActions *pActions, EActionsType eActions) const = 0;
  // common updates
  virtual void AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) = 0;
  virtual bool AIUpdateRPGStats(const struct SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) = 0;
  virtual void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) = 0;
  virtual int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) = 0;
  // visiting
  virtual void Visit(IMapObjVisitor *pVisitor) = 0;
};

struct SMapObject : IMapObj
{
  DECLARE_SERIALIZE;

public:
  CPtr<IRefCount> pAIObj;// AI unit for communication with AI
  CPtr<IVisObj> pVisObj;// visual object - what is drawn
  CPtr<IVisObj> pShadow;// the shadow of this object. 
  CGDBPtr<SGDBObjectDesc> pDesc;// game DB entry - a structure from the game database that describes the object
  CGDBPtr<SHPObjectRPGStats> pRPG;// RPG stats for all objects and units
  BYTE diplomacy;// diplomacy settings for this object
  bool bCanSelect;// can object be selected?
  int nSelectionGroupID;// visual selection group for a fast reset
  float fHP;// current health (%)
  //
  SMapObject() : diplomacy(EDI_NEUTRAL), bCanSelect(false), nSelectionGroupID(-1), fHP(1) {}
  //
  void SetHP(float _fHP) { fHP = _fHP; }
  float GetHP() const { return pRPG != nullptr ? pRPG->fMaxHP * fHP : 0; }
  bool IsAlive() const { return (GetHP() > 0) && (pAIObj != nullptr); }
  float GetMaxHP() const { return pRPG != nullptr ? pRPG->fMaxHP : 0; }
  bool IsHuman() const { return pDesc && (pDesc->eGameType == SGVOGT_UNIT) && (pDesc->eVisType == SGVOT_SPRITE); }
  bool IsTechnics() const { return pDesc && (pDesc->eGameType == SGVOGT_UNIT) && (pDesc->eVisType == SGVOT_MESH); }
  //
  void SetDiplomacy(EDiplomacyInfo eDiplomacy);
  bool IsEnemy() const { return diplomacy == EDI_ENEMY; }
  bool IsFriend() const { return diplomacy == EDI_FRIEND; }
  bool IsNeutral() const { return diplomacy == EDI_NEUTRAL; }
  //
  bool CanSelect() const override { return bCanSelect; }
};

struct SBridgeSpanObject : IMapObj
{
  DECLARE_SERIALIZE;

public:
  CPtr<IRefCount> pAIObj;// span AI object
  // map objects, this span consists of
  CPtr<SMapObject> pSlab;
  CPtr<SMapObject> pBackGirder;
  CPtr<SMapObject> pFrontGirder;
  int nIndex;// span index
  //
  void SetHP(float _fHP) { pSlab->SetHP(_fHP); }
  float GetHP() const { return pSlab->GetHP(); }
  float GetMaxHP() const { return pSlab->GetMaxHP(); }
  //
  void SetOpacity(BYTE op)
  {
    pSlab->pVisObj->SetOpacity(op);
    if (pBackGirder) pBackGirder->pVisObj->SetOpacity(op);
    if (pFrontGirder) pFrontGirder->pVisObj->SetOpacity(op);
  }

  void SetSpecular(SColor color)
  {
    if (pSlab) { pSlab->pVisObj->SetSpecular(color); }
    if (pBackGirder) { pBackGirder->pVisObj->SetSpecular(color); }
    if (pFrontGirder) { pFrontGirder->pVisObj->SetSpecular(color); }
  }

  std::string GetObjectName() { return pSlab->pDesc->szKey; };
};

using CBridgeSpanObjectsList = std::list<CPtr<SBridgeSpanObject>>;

interface IMOEffect : SMapObject
{
  virtual bool Create(IRefCount *pAIObj, const char *pszName, interface IVisObjBuilder *pVOB) = 0;
};

interface IMOSelectable : SMapObject
{
  // check, is this object selected?
  virtual bool IsSelected() const = 0;
  //
  virtual void Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) = 0;
  //
  virtual bool AIUpdateDiplomacy(const struct SAINotifyDiplomacy &diplomacy) = 0;
  // sends selection acknowledgment
  virtual void SendAcknowledgement(interface IClientAckManager *pAckManager, const EUnitAckType eAckType, const int nSet) {}
};

interface IMOContainer : IMOSelectable
{
  // load unit onboard or unload it
  virtual bool Load(interface IMOUnit *pMO, bool bEnter) = 0;
  // show icons of the passangers
  virtual void UpdatePassangers() = 0;
  // get all passangers from container. 
  virtual int GetPassangers(IMOUnit **pBuffer, bool bCanSelectOnly = false) const = 0;
  // get free places
  virtual int GetFreePlaces() const = 0;
  // firing... (from container of by himself)
  virtual void AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) = 0;
};

interface IUnitStateObserver : IRefCount
{
  virtual void AddIcon(int nType, interface ISceneIcon *pIcon) = 0;
  virtual void RemoveIcon(int nType) = 0;
  virtual void UpdateHP(float fValue) = 0;
  virtual void RemoveUnit() = 0;
  virtual IMOUnit * GetMOUnit() = 0;
};

interface IMOUnit : IMOContainer
{
  virtual void PrepareToRemove() = 0;
  //
  virtual const bool IsVisible() const = 0;
  //
  virtual void AssignSelectionGroup(int nGroupID) = 0;
  //
  virtual void SetContainer(IMOContainer *pContainer) = 0;
  virtual IMOContainer * GetContainer() const = 0;
  virtual void SetSquad(interface IMOSquad *pSquad) = 0;
  virtual interface IMOSquad * GetSquad() = 0;
  // general update. 
  virtual bool Update(const NTimer::STime &currTime) = 0;
  // unit's updates
  virtual void AIUpdateAiming(const struct AIUpdateAiming &aiming) = 0;
  //
  virtual IMapObj * AIUpdateFireWithProjectile(const struct SAINotifyNewProjectile &projectile, const NTimer::STime &currTime, interface IVisObjBuilder *pVOB) = 0;
  // CRAP{ for animations testing
  virtual void AddAnimation(const SUnitBaseRPGStats::SAnimDesc *pDesc) = 0;
  // CRAP}
  // for asci
  virtual void AIUpdateAcknowledgement(EUnitAckType eAck, interface IClientAckManager *pAckManager, int nSet) = 0;
  virtual void AIUpdateBoredAcknowledgement(const struct SAIBoredAcknowledgement &ack, interface IClientAckManager *pAckManager) = 0;
  // remove all sounds that attached to this unit
  virtual void RemoveSounds(interface IScene *pScene) = 0;
  // retrieve localized name
  virtual interface IText * GetLocalName() const = 0;
  // set icon update hook
  virtual void SetObserver(IUnitStateObserver *pObserver) = 0;
  virtual int GetPlayerIndex() const = 0;
  // change look with blood settings
  virtual bool ChangeWithBlood(IVisObjBuilder *pVOB) = 0;
};

interface IMOSquad : IMOContainer
{
  // notify about RPG stats changing fot the single squad member
  virtual void NotifyStatsChanged(IMOUnit *pUnit, float fHP, float fAmmo1, float fAmmo2) = 0;
  // get selection ID
  virtual const int GetSelectionGroupID() const = 0;
};

using CMapObjectsList = std::list<CPtr<SMapObject>>;
using CMapObjectsPtrList = std::list<SMapObject *>;
using CMapObjectsMap = std::unordered_map<IRefCount *, CObj<SMapObject>, SDefaultPtrHash>;
using CMapObjectsSet = std::unordered_set<SMapObject *, SDefaultPtrHash>;
//
using CBridgeSpanObjectsMap = std::unordered_map<IRefCount *, CPtr<SBridgeSpanObject>, SDefaultPtrHash>;

interface IText *GetLocalName(const SGDBObjectDesc *pDesc);

#endif // __MAPOBJECT_H__