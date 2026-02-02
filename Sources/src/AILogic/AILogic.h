#ifndef __AI_LOGIC_H__
#define __AI_LOGIC_H__

#include "../Formats/fmtMap.h"
#include "../AILogic/AIGeometry.h"
#include "AIClassesID.h"
#include "../StreamIO/ProgressHook.h"

#define AI_INIT_PROGRESS_STEPS 5

interface IAIEditor : IRefCount
{
  // type ID
  enum { tidTypeID = AI_EDITOR };

  //
  virtual void Init(const struct STerrainInfo &terrainInfo) = 0;
  virtual void Clear() = 0;

  virtual bool IsFormation(IRefCount *pObject) const = 0;
  virtual void GetUnitsInFormation(IRefCount *pObject, IRefCount ***pUnits, int *pnLen) = 0;
  virtual IRefCount * GetFormationOfUnit(IRefCount *pObject) = 0;

  virtual bool AddNewObject(const struct SMapObjectInfo &object, IRefCount **pObject) = 0;
  virtual bool AddNewEntrencment(IRefCount **segments, int nLen, IRefCount **pObject) = 0;
  virtual void LoadEntrenchments(const std::vector<SEntrenchmentInfo> &entrenchments) = 0;
  virtual bool MoveObject(IRefCount *pObject, short x, short y) = 0;
  virtual void DeleteObject(IRefCount *pObject) = 0;
  virtual void DamageObject(IRefCount *pObject, float fHP) = 0;

  virtual bool TurnObject(IRefCount *pObject, WORD wDir) = 0;

  virtual float GetObjectHP(IRefCount *pObject) = 0;
  virtual int GetObjectScriptID(IRefCount *pObject) = 0;

  virtual void HandOutLinks() = 0;
  virtual IRefCount * LinkToAI(int ID) = 0;
  virtual int AIToLink(IRefCount *pObj) = 0;

  virtual const CVec2 & GetCenter(IRefCount *pObj) const = 0;
  virtual const WORD GetDir(IRefCount *pObj) const = 0;

  virtual const int GetUnitDBID(IRefCount *pObj) const = 0;

  // checks whether the object is inside the map
  virtual bool IsObjectInsideOfMap(const struct SMapObjectInfo &object) const = 0;
  // checks whether the object is inside the map and whether it is placed on locked tiles
  virtual bool CanAddObject(const struct SMapObjectInfo &object) const = 0;

  virtual void ApplyPattern(const struct SVAPattern &rPattern) = 0;
  virtual void UpdateAllHeights() = 0;

  // enable - disable something, returns: true - enabled, false - disabled
  virtual bool ToggleShow(int nShowType) = 0;

  virtual void UpdateTerrain(const CTRect<int> &rect, const struct STerrainInfo &terrainInfo) = 0;

  // for visualization of storage system
  virtual void RecalcPassabilityForPlayer(CArray2D<BYTE> *array, int nPlayer) = 0;

  virtual void SetPlayer(IRefCount *pObj, int nPlayer) = 0;
  virtual void SetDiplomacies(const std::vector<BYTE> &playerParty) = 0;

  virtual void DeleteRiver(const SVectorStripeObject &river) = 0;
  virtual void AddRiver(const SVectorStripeObject &river) = 0;
};

interface IAILogic : IRefCount
{
  // type ID
  enum { tidTypeID = AI_LOGIC };

  virtual void Suspend() = 0;
  virtual void Resume() = 0;
  virtual bool IsSuspended() const = 0;
  // Note: This function uses the temp buffer
  virtual void Init(const struct SLoadMapInfo &mapInfo, IProgressHook *pProgress = nullptr) = 0;
  virtual void Clear() = 0;

  // Note: These functions use the temp buffer
  virtual void UpdatePlacements(struct SAINotifyPlacement **pUnitPosBuffer, int *pnLen) = 0;
  virtual void UpdateActions(struct SAINotifyAction **pActionsBuffer, int *pnLen) = 0;
  virtual void UpdateRPGParams(struct SAINotifyRPGStats **pUnitRPGBuffer, int *pnLen) = 0;
  virtual void UpdateTurretTurn(struct SAINotifyTurretTurn **pTurretsBuffer, int *pnLen) = 0;
  virtual void UpdateEntranceStates(struct SAINotifyEntranceState **pUnits, int *pnLen) = 0;

  virtual void UpdateFeedbacks(struct SAIFeedBack **pFeedBacksBuffer, int *pnLen) = 0;

  virtual void UpdateStObjPlacements(struct SAINotifyPlacement **pObjPosBuffer, int *pnLen) = 0;
  virtual void UpdateDiplomacies(struct SAINotifyDiplomacy **pDiplomaciesBuffer, int *pnLen) = 0;

  virtual void EndUpdates() = 0;

  // Note: These functions use the temp buffer
  virtual void UpdateShots(struct SAINotifyMechShot **pShots, int *pnLen) = 0;
  virtual void UpdateShots(struct SAINotifyInfantryShot **pShots, int *pnLen) = 0;
  virtual void UpdateHits(struct SAINotifyHitInfo **pHits, int *pnLen) = 0;
  virtual void GetNewProjectiles(struct SAINotifyNewProjectile **pProjectiles, int *pnLen) = 0;
  virtual void GetDeadProjectiles(IRefCount ***pProjectilesBuf, int *pnLen) = 0;

  // Note: This function uses the temp buffer
  virtual void GetNewUnits(struct SNewUnitInfo **pNewUnitBuffer, int *pnNumNewUnits) = 0;
  // Note: This function uses the temp buffer
  virtual void GetNewStaticObjects(struct SNewUnitInfo **pObjects, int *pnLen) = 0;
  virtual void GetEntrenchments(struct SSegment2Trench **pEntrenchemnts, int *pnLen) = 0;
  virtual void GetFormations(struct SSoldier2Formation **pFormations, int *pnLen) = 0;
  virtual void GetNewBridgeSpans(struct SNewUnitInfo **pObjects, int *pnLen) = 0;
  virtual bool GetNewBridge(IRefCount ***pSpans, int *pnLen) = 0;

  // Note: This function uses the temp buffer
  virtual void GetDeadUnits(struct SAINotifyDeadAtAll **pDeadUnitsBuffer, int *pnLen) = 0;
  // Note: This function uses the temp buffer
  virtual void GetDisappearedUnits(IRefCount ***pUnitsBuffer, int *pnLen) = 0;
  // Note: This function uses the temp buffer
  virtual void GetDeletedStaticObjects(IRefCount ***pObjBuffer, int *pnLen) = 0;
  virtual void GetRevealCircles(CCircle **pCircleBuffer, int *pnLen) = 0;

  virtual void UnitCommand(const struct SAIUnitCmd *pCommand, WORD wGroupID, int nPlayer) = 0;

  // Note: This function uses the temp buffer
  virtual void GetVisibilities(const class CVec2 &upLeft, const class CVec2 &downLeft,
                                       const class CVec2 &downRight, const class CVec2 &upRight,
                                       struct SAIVisInfo **pVisBuffer, int *pnLen) const = 0;

  // replaces pointers in pUnitsBuffer with unique unit ids
  virtual const WORD GenerateGroupNumber() = 0;
  virtual void RegisterGroup(IRefCount **pUnitsBuffer, int nLen, WORD wGroup) = 0;
  virtual void UnregisterGroup(WORD wGroup) = 0;
  virtual void GroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) = 0;

  virtual void CheckDiplomacy(const IRefCount **pUnitsBuffer, BYTE **pResults, int nLen) = 0;

  // 1 - passable, 0 - not
  virtual void GetGlobalPassability(BYTE **pMapBuffer, int *pnLen) = 0;
  virtual void GetDisplayPassability(const class CVec2 &upLeft, const class CVec2 &downLeft,
                                             const class CVec2 &downRight, const class CVec2 &upRight,
                                             struct SAIPassabilityInfo **pPassBuffer, int *pnLen) = 0;

  virtual void ShowAreas(int nGroup, EActionNotify eType, bool bShow) = 0;

  virtual void UpdateShootAreas(struct SShootAreas **pShootAreas, int *pnLen) = 0;

  virtual void GetMiniMapInfo(struct SMiniMapUnitInfo **pUnitsBuffer, int *pnLen) = 0;
  virtual void GetMiniMapInfo(BYTE **pVisBuffer, int *pnLen) = 0;

  virtual void CallScriptFunction(const char *pszCommand) = 0;

  virtual int GetUniqueIDOfObject(IRefCount *pObj) = 0;
  virtual IRefCount * GetObjByUniqueID(int id) = 0;

  virtual void Segment() = 0;

  // to build a fence. 
  virtual void SetMyInfo(int nParty, int nNumber) = 0;
  virtual void SetNPlayers(int nPlayers) = 0;
  virtual void SetNetGame(bool bNetGame) = 0;

  // returns whether the substitution completed successfully (all passed objects are correct)
  virtual bool SubstituteUniqueIDs(IRefCount **pUnitsBuffer, int nLen) = 0;

  virtual void UpdateAcknowledgments(SAIAcknowledgment **pAckBuffer, int *pnLen) = 0;
  virtual void UpdateAcknowledgments(SAIBoredAcknowledgement **pAckBuffer, int *pnLen) = 0;

  virtual float GetZ(const CVec2 &vPoint) const = 0;
  virtual const DWORD GetNormal(const CVec2 &vPoint) const = 0;
  virtual const bool GetIntersectionWithTerrain(CVec3 *pvResult, const CVec3 &vBegin, const CVec3 &vEnd) const = 0;

  // enable - disable something, returns: true - enabled, false - disabled
  virtual bool ToggleShow(int nShowType) = 0;

  // What does AILogic think about the situation - combat or not?
  virtual bool IsCombatSituation() = 0;

  // to visualize the aircraft spawn point
  virtual CVec2 LockAvitaionAppearPoint() = 0;
  virtual void UnlockAviationAppearPoint() = 0;

  // difficully levels
  virtual void SetDifficultyLevel(int nLevel) = 0;

  // for debug
  virtual int GetUniqueID(IRefCount *pObj) = 0;
  virtual IRefCount * GetUnitState(IRefCount *pObj) = 0;
  virtual bool IsFrozen(IRefCount *pObj) const = 0;
  virtual bool IsFrozenByState(IRefCount *pObj) const = 0;

  // for sending acknowledgement of selection
  virtual void SendAcknowlegdementForced(IRefCount *pObj, EUnitAckType eAck) = 0;

  // when playing multiplayer: all players loaded and the game started
  virtual void NetGameStarted() = 0;
  virtual bool IsNetGameStarted() const = 0;

  virtual const class CDifficultyLevel * GetDifficultyLevel() const = 0;

  virtual void NeutralizePlayer(int nPlayer) = 0;
  virtual void NoWin() = 0;
  virtual bool IsNoWin() const = 0;

  virtual void GetGridUnitsCoordinates(int nGroup, const CVec2 &vGridCenter, CVec2 **pCoord, int *pnLen) = 0;
};

#endif // __AI_LOGIC_H__