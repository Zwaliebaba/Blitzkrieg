#ifndef __AI_LOGIC_INTERNAL_H__
#define __AI_LOGIC_INTERNAL_H__

#pragma once

#include "AILogic.h"
#include "Scripts/Scripts.h"
#include <zlib.h>
#include "LinkObject.h"

class CCommonUnit;
class CAIUnit;
class CBridgeSpan;

using LinkInfo = std::unordered_map<CLinkObject *, SMapObjectInfo::SLinkInfo, SUniqueIdHash>;

class CAILogic : public IAILogic
{
  OBJECT_NORMAL_METHODS(CAILogic);
  DECLARE_SERIALIZE;

  bool bSuspended;
  bool bFirstTime;

  // bridges
  using Bridges = std::list<std::list<CPtr<CBridgeSpan>>>;
  Bridges bridges;

  //
  std::list<CObj<CCommonUnit>> garbage;

  // scripts
  CScripts scripts;
  CPtr<ISegmentTimer> pGameSegment;

  EActionNotify eTypeOfAreasToShow;
  SLoadMapInfo::TStartCommandsList startCmds;
  SLoadMapInfo::TReservePositionsList reservePositions;

  NTimer::STime nextCheckSumTime;
  NTimer::STime periodToCheckSum;
  uLong checkSum;

  bool bSegment;
  bool bNetGameStarted;

  using CAvailTrucks = std::unordered_set<CGDBPtr<SMechUnitRPGStats>, SDefaultPtrHash>;
  CAvailTrucks availableTrucks;
  // check if object is a truck attached to script artillery
  // if yes, searches for a truck suitable for artillery (in pNewStats) and returns true, if no artillery is found, returns false
  // if this is not such a truck, returns true
  bool CheckForScenarioTruck(const SMapObjectInfo &object, IObjectsDB *pIDB, const SGDBObjectDesc *pDesc, int nDBIndex, LinkInfo *linksInfo, const SMechUnitRPGStats **pNewStats) const;

  // Loading
  // part of the initialization common to the game and the editor
  void CommonInit(const STerrainInfo &terrainInfo);

  void LoadUnits(const struct SLoadMapInfo &mapInfo, LinkInfo *linksInfo);
  void LoadScenarioUnits(const struct SLoadMapInfo &mapInfo, LinkInfo *linksInfo);
  void InitReservePositions();
  void InitStartCommands();
  void LaunchStartCommand(const SAIStartCommand &startCommand, IRefCount **pUnitsBuffer, int nSize);
  // bSend - whether to send checksum
  void UpdateCheckSum(bool bSend);

  void LoadAvailableTrucks();
  bool CanShowVisibilities() const;

public:
  CAILogic();

  int GetScriptID(interface IUpdatableObj *pObj) const { return scripts.GetScriptID(pObj); }

  void ToGarbage(class CCommonUnit *pUnit);

  void Suspend() override;
  void Resume() override;
  bool IsSuspended() const override { return bSuspended; }

  ISegmentTimer *GetGameSegment() const { return pGameSegment; }

  void Init(const struct SLoadMapInfo &mapInfo, IProgressHook *pProgress = nullptr) override;
  virtual void InitEditor(const struct STerrainInfo &terrainInfo);
  void Clear() override;

  // Note: These functions use the temp buffer
  void UpdatePlacements(SAINotifyPlacement **pObjPosBuffer, int *pnLen) override;
  void UpdateActions(SAINotifyAction **pActionsBuffer, int *pnLen) override;
  void UpdateRPGParams(SAINotifyRPGStats **pUnitRPGBuffer, int *pnLen) override;
  void UpdateTurretTurn(struct SAINotifyTurretTurn **pTurretsBuffer, int *pnLen) override;
  void UpdateEntranceStates(struct SAINotifyEntranceState **pUnits, int *pnLen) override;

  void UpdateFeedbacks(struct SAIFeedBack **pFeedBacksBuffer, int *pnLen) override;

  // Note: These functions use the temp buffer
  void UpdateShots(struct SAINotifyMechShot **pShots, int *pnLen) override;
  void UpdateShots(struct SAINotifyInfantryShot **pShots, int *pnLen) override;

  void UpdateHits(struct SAINotifyHitInfo **pHits, int *pnLen) override;
  void GetNewProjectiles(struct SAINotifyNewProjectile **pProjectiles, int *pnLen) override;
  void GetDeadProjectiles(IRefCount ***pProjectilesBuf, int *pnLen) override;

  void UpdateStObjPlacements(struct SAINotifyPlacement **pObjPosBuffer, int *pnLen) override;
  void UpdateDiplomacies(struct SAINotifyDiplomacy **pDiplomaciesBuffer, int *pnLen) override;

  void EndUpdates() override;

  // Note: This function uses the temp buffer
  void GetNewUnits(SNewUnitInfo **pNewUnitBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void GetNewStaticObjects(struct SNewUnitInfo **pObjects, int *pnLen) override;
  void GetEntrenchments(struct SSegment2Trench **pEntrenchemnts, int *pnLen) override;
  void GetFormations(struct SSoldier2Formation **pFormations, int *pnLen) override;
  void GetNewBridgeSpans(struct SNewUnitInfo **pObjects, int *pnLen) override;
  bool GetNewBridge(IRefCount ***pSpans, int *pnLen) override;

  // Note: This function uses the temp buffer
  void GetDeadUnits(SAINotifyDeadAtAll **pDeadUnitsBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void GetDisappearedUnits(IRefCount ***pUnitsBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void GetDeletedStaticObjects(IRefCount ***pObjBuffer, int *pnLen) override;
  void GetRevealCircles(CCircle **pCircleBuffer, int *pnLen) override;

  void UnitCommand(const SAIUnitCmd *pCommand, WORD wGroupID, int nPlayer) override;
  // Note: This function uses the temp buffer
  void GetVisibilities(const class CVec2 &upLeft, const class CVec2 &downLeft,
                               const class CVec2 &downRight, const class CVec2 &upRight,
                               struct SAIVisInfo **pVisBuffer, int *pnLen) const override;

  const WORD GenerateGroupNumber() override;
  void RegisterGroup(IRefCount **pUnitsBuffer, int nLen, WORD wGroup) override;
  void UnregisterGroup(WORD wGroup) override;
  void GroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) override;

  void CheckDiplomacy(const IRefCount **pUnitsBuffer, BYTE **pResults, int nLen) override;

  void GetGlobalPassability(BYTE **pMapBuffer, int *pnLen) override;
  void GetDisplayPassability(const class CVec2 &upLeft, const class CVec2 &downLeft,
                                     const class CVec2 &downRight, const class CVec2 &upRight,
                                     SAIPassabilityInfo **pPassBuffer, int *pnLen) override;

  void ShowAreas(int nGroup, EActionNotify eType, bool bShow) override;
  void UpdateShootAreas(struct SShootAreas **pShootAreas, int *pnLen) override;

  void GetMiniMapInfo(struct SMiniMapUnitInfo **pUnitsBuffer, int *pnLen) override;
  void GetMiniMapInfo(BYTE **pVisBuffer, int *pnLen) override;

  void CallScriptFunction(const char *pszCommand) override;

  int GetUniqueIDOfObject(IRefCount *pObj) override;
  IRefCount * GetObjByUniqueID(int id) override;

  void Segment() override;

  // CRAP{no need to add gunners in the editor
  IRefCount *AddObject(const SMapObjectInfo &object, IObjectsDB *pIDB, LinkInfo *linksInfo, bool bInitialization, bool IsEditor, const SHPObjectRPGStats *pPassedStats);
  // CRAP}no need to add gunners in the editor
  void InitLinks(LinkInfo &linksInfo);
  void LoadEntrenchments(const std::vector<struct SEntrenchmentInfo> &entrenchments);
  void LoadBridges(const std::vector<std::vector<int>> &bridgesInfo);

  void SetMyInfo(int nParty, int nNumber) override;
  void SetNPlayers(int nPlayers) override;
  void SetNetGame(bool bNetGame) override;

  bool SubstituteUniqueIDs(IRefCount **pUnitsBuffer, int nLen) override;

  void UpdateAcknowledgments(SAIAcknowledgment **pAckBuffer, int *pnLen) override;
  void UpdateAcknowledgments(SAIBoredAcknowledgement **pAckBuffer, int *pnLen) override;

  float GetZ(const CVec2 &vPoint) const override;
  const DWORD GetNormal(const CVec2 &vPoint) const override;
  const bool GetIntersectionWithTerrain(CVec3 *pvResult, const CVec3 &vBegin, const CVec3 &vEnd) const override;

  bool ToggleShow(int nShowType) override;

  bool IsCombatSituation() override;
  void InitStartCommands(const LinkInfo &linksInfo, std::unordered_map<int, int> &old2NewLinks);
  void InitReservePositions(std::unordered_map<int, int> &old2NewLinks);

  bool IsSegment() const { return bSegment; }

  CVec2 LockAvitaionAppearPoint() override;
  void UnlockAviationAppearPoint() override;

  // difficully levels
  void SetDifficultyLevel(int nLevel) override;
  virtual void SetCheatDifficultyLevel(int nCheatLevel);

  const bool IsFirstTime() const { return bFirstTime; }

  void SendAcknowlegdementForced(IRefCount *pObj, EUnitAckType eAck) override;

  // for debug
  int GetUniqueID(IRefCount *pObj) override
  {
    if (auto pLinkObj = dynamic_cast<CLinkObject *>(pObj)) return pLinkObj->GetUniqueId();
    return 0;
  }

  // when playing multiplayer: all players loaded and the game started
  void NetGameStarted() override;
  bool IsNetGameStarted() const override;

  const class CDifficultyLevel * GetDifficultyLevel() const override;

  void NeutralizePlayer(int nPlayer) override;
  void NoWin() override;
  bool IsNoWin() const override;

  IRefCount * GetUnitState(IRefCount *pObj) override;
  bool IsFrozen(IRefCount *pObj) const override;
  bool IsFrozenByState(IRefCount *pObj) const override;

  void GetGridUnitsCoordinates(int nGroup, const CVec2 &vGridCenter, CVec2 **pCoord, int *pnLen) override;
};

#endif // __AI_LOGIC_INTERNAL_H__