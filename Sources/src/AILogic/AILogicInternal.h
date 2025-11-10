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

using LinkInfo = std::hash_map<CLinkObject *, SMapObjectInfo::SLinkInfo, SUniqueIdHash>;

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

  using CAvailTrucks = std::hash_set<CGDBPtr<SMechUnitRPGStats>, SDefaultPtrHash>;
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

  void STDCALL Suspend() override;
  void STDCALL Resume() override;
  bool STDCALL IsSuspended() const override { return bSuspended; }

  ISegmentTimer *GetGameSegment() const { return pGameSegment; }

  void STDCALL Init(const struct SLoadMapInfo &mapInfo, IProgressHook *pProgress = nullptr) override;
  virtual void STDCALL InitEditor(const struct STerrainInfo &terrainInfo);
  void STDCALL Clear() override;

  // Note: These functions use the temp buffer
  void STDCALL UpdatePlacements(SAINotifyPlacement **pObjPosBuffer, int *pnLen) override;
  void STDCALL UpdateActions(SAINotifyAction **pActionsBuffer, int *pnLen) override;
  void STDCALL UpdateRPGParams(SAINotifyRPGStats **pUnitRPGBuffer, int *pnLen) override;
  void STDCALL UpdateTurretTurn(struct SAINotifyTurretTurn **pTurretsBuffer, int *pnLen) override;
  void STDCALL UpdateEntranceStates(struct SAINotifyEntranceState **pUnits, int *pnLen) override;

  void STDCALL UpdateFeedbacks(struct SAIFeedBack **pFeedBacksBuffer, int *pnLen) override;

  // Note: These functions use the temp buffer
  void STDCALL UpdateShots(struct SAINotifyMechShot **pShots, int *pnLen) override;
  void STDCALL UpdateShots(struct SAINotifyInfantryShot **pShots, int *pnLen) override;

  void STDCALL UpdateHits(struct SAINotifyHitInfo **pHits, int *pnLen) override;
  void STDCALL GetNewProjectiles(struct SAINotifyNewProjectile **pProjectiles, int *pnLen) override;
  void STDCALL GetDeadProjectiles(IRefCount ***pProjectilesBuf, int *pnLen) override;

  void STDCALL UpdateStObjPlacements(struct SAINotifyPlacement **pObjPosBuffer, int *pnLen) override;
  void STDCALL UpdateDiplomacies(struct SAINotifyDiplomacy **pDiplomaciesBuffer, int *pnLen) override;

  void STDCALL EndUpdates() override;

  // Note: This function uses the temp buffer
  void STDCALL GetNewUnits(SNewUnitInfo **pNewUnitBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void STDCALL GetNewStaticObjects(struct SNewUnitInfo **pObjects, int *pnLen) override;
  void STDCALL GetEntrenchments(struct SSegment2Trench **pEntrenchemnts, int *pnLen) override;
  void STDCALL GetFormations(struct SSoldier2Formation **pFormations, int *pnLen) override;
  void STDCALL GetNewBridgeSpans(struct SNewUnitInfo **pObjects, int *pnLen) override;
  bool STDCALL GetNewBridge(IRefCount ***pSpans, int *pnLen) override;

  // Note: This function uses the temp buffer
  void STDCALL GetDeadUnits(SAINotifyDeadAtAll **pDeadUnitsBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void STDCALL GetDisappearedUnits(IRefCount ***pUnitsBuffer, int *pnLen) override;
  // Note: This function uses the temp buffer
  void STDCALL GetDeletedStaticObjects(IRefCount ***pObjBuffer, int *pnLen) override;
  void STDCALL GetRevealCircles(CCircle **pCircleBuffer, int *pnLen) override;

  void STDCALL UnitCommand(const SAIUnitCmd *pCommand, WORD wGroupID, int nPlayer) override;
  // Note: This function uses the temp buffer
  void STDCALL GetVisibilities(const class CVec2 &upLeft, const class CVec2 &downLeft,
                               const class CVec2 &downRight, const class CVec2 &upRight,
                               struct SAIVisInfo **pVisBuffer, int *pnLen) const override;

  const WORD STDCALL GenerateGroupNumber() override;
  void STDCALL RegisterGroup(IRefCount **pUnitsBuffer, int nLen, WORD wGroup) override;
  void STDCALL UnregisterGroup(WORD wGroup) override;
  void STDCALL GroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) override;

  void STDCALL CheckDiplomacy(const IRefCount **pUnitsBuffer, BYTE **pResults, int nLen) override;

  void STDCALL GetGlobalPassability(BYTE **pMapBuffer, int *pnLen) override;
  void STDCALL GetDisplayPassability(const class CVec2 &upLeft, const class CVec2 &downLeft,
                                     const class CVec2 &downRight, const class CVec2 &upRight,
                                     SAIPassabilityInfo **pPassBuffer, int *pnLen) override;

  void STDCALL ShowAreas(int nGroup, EActionNotify eType, bool bShow) override;
  void STDCALL UpdateShootAreas(struct SShootAreas **pShootAreas, int *pnLen) override;

  void STDCALL GetMiniMapInfo(struct SMiniMapUnitInfo **pUnitsBuffer, int *pnLen) override;
  void STDCALL GetMiniMapInfo(BYTE **pVisBuffer, int *pnLen) override;

  void STDCALL CallScriptFunction(const char *pszCommand) override;

  int STDCALL GetUniqueIDOfObject(IRefCount *pObj) override;
  IRefCount * STDCALL GetObjByUniqueID(int id) override;

  void STDCALL Segment() override;

  // CRAP{no need to add gunners in the editor
  IRefCount *AddObject(const SMapObjectInfo &object, IObjectsDB *pIDB, LinkInfo *linksInfo, bool bInitialization, bool IsEditor, const SHPObjectRPGStats *pPassedStats);
  // CRAP}no need to add gunners in the editor
  void InitLinks(LinkInfo &linksInfo);
  void LoadEntrenchments(const std::vector<struct SEntrenchmentInfo> &entrenchments);
  void LoadBridges(const std::vector<std::vector<int>> &bridgesInfo);

  void STDCALL SetMyInfo(int nParty, int nNumber) override;
  void STDCALL SetNPlayers(int nPlayers) override;
  void STDCALL SetNetGame(bool bNetGame) override;

  bool SubstituteUniqueIDs(IRefCount **pUnitsBuffer, int nLen) override;

  void STDCALL UpdateAcknowledgments(SAIAcknowledgment **pAckBuffer, int *pnLen) override;
  void STDCALL UpdateAcknowledgments(SAIBoredAcknowledgement **pAckBuffer, int *pnLen) override;

  float STDCALL GetZ(const CVec2 &vPoint) const override;
  const DWORD STDCALL GetNormal(const CVec2 &vPoint) const override;
  const bool STDCALL GetIntersectionWithTerrain(CVec3 *pvResult, const CVec3 &vBegin, const CVec3 &vEnd) const override;

  bool STDCALL ToggleShow(int nShowType) override;

  bool STDCALL IsCombatSituation() override;
  void InitStartCommands(const LinkInfo &linksInfo, std::hash_map<int, int> &old2NewLinks);
  void InitReservePositions(std::hash_map<int, int> &old2NewLinks);

  bool IsSegment() const { return bSegment; }

  CVec2 STDCALL LockAvitaionAppearPoint() override;
  void STDCALL UnlockAviationAppearPoint() override;

  // difficully levels
  void STDCALL SetDifficultyLevel(int nLevel) override;
  virtual void STDCALL SetCheatDifficultyLevel(int nCheatLevel);

  const bool IsFirstTime() const { return bFirstTime; }

  void STDCALL SendAcknowlegdementForced(IRefCount *pObj, EUnitAckType eAck) override;

  // for debug
  int STDCALL GetUniqueID(IRefCount *pObj) override
  {
    if (auto pLinkObj = dynamic_cast<CLinkObject *>(pObj)) return pLinkObj->GetUniqueId();
    return 0;
  }

  // when playing multiplayer: all players loaded and the game started
  void STDCALL NetGameStarted() override;
  bool STDCALL IsNetGameStarted() const override;

  const class CDifficultyLevel * STDCALL GetDifficultyLevel() const override;

  void STDCALL NeutralizePlayer(int nPlayer) override;
  void STDCALL NoWin() override;
  bool STDCALL IsNoWin() const override;

  IRefCount * STDCALL GetUnitState(IRefCount *pObj) override;
  bool STDCALL IsFrozen(IRefCount *pObj) const override;
  bool STDCALL IsFrozenByState(IRefCount *pObj) const override;

  void STDCALL GetGridUnitsCoordinates(int nGroup, const CVec2 &vGridCenter, CVec2 **pCoord, int *pnLen) override;
};

#endif // __AI_LOGIC_INTERNAL_H__