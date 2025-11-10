#ifndef __FORMATION_H__
#define __FORMATION_H__

#pragma once

#include "CommonUnit.h"
#include "../Misc/BitData.h"

class CFormationCenter : public CCommonUnit
{
  DECLARE_SERIALIZE;

  CVec2 speed;
  CVec2 center;
  CVec2 dir;
  CVec2 vAABBHalfSize;
  float z;

  int nBoundTileRadius;

  CPtr<ISmoothPath> pSmoothPath;
  CPtr<IStaticPath> pStaticPath;

  float maxDiff;
  float fSpeedCoeff;
  SVector lastKnownGoodTile;

protected:
  float maxSpeed;

public:
  CFormationCenter() : dir(0, 0), vAABBHalfSize(0, 0), nBoundTileRadius(0), maxSpeed(1000) {}
  void Init(const CVec2 &center, int z, WORD dir, int dbID);

  const CVec2 &GetCenter() const override { return center; }
  const CVec2 &GetSpeed() const override { return speed; }
  const float GetZ() const override { return z; }
  const float GetRotateSpeed() const override { return 0; }
  const float GetMaxPossibleSpeed() const override;
  const float GetSpeedForFollowing() override;
  bool CanMove() const override { return maxSpeed > 0; }
  bool CanMovePathfinding() const override { return CanMove(); }
  bool CanRotate() const override { return true; }
  const int GetBoundTileRadius() const override { return nBoundTileRadius; }
  const WORD GetDir() const override { return GetDirectionByVector(dir); }
  const WORD GetFrontDir() const override { return GetDirectionByVector(dir); }
  const CVec2 &GetDirVector() const override { return dir; }
  const CVec2 GetAABBHalfSize() const override { return vAABBHalfSize; }
  void SetCoordWOUpdate(const CVec3 &newCenter) override;
  void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true) override;
  IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking) override;

  const SRect GetUnitRectForLock() const override;

  bool TurnToDir(const WORD &newDir, bool bCanBackward = true, bool bForward = true) override;
  void UpdateDirection(const CVec2 &newDir) override;
  void UpdateDirection(WORD newDir) override;
  // stop only center of formation, not units
  void StopFormationCenter();

  //
  void SetMaxSpeed(const float &_maxSpeed) { maxSpeed = _maxSpeed; }
  void SetAABBHalfSize(const CVec2 &_vAABBHalfSize) { vAABBHalfSize = _vAABBHalfSize; }
  void SetBoundTileRadius(const int _nBoundTileRadius) { nBoundTileRadius = _nBoundTileRadius; }

  void Segment() override;
  // returns whether I went or not
  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(IPath *pPath) override;

  interface IStaticPathFinder *GetPathFinder() const override;

  interface ISmoothPath *GetCurPath() const override { return pSmoothPath; }
  interface IStaticPath *GetStaticPath() const { return pStaticPath; }

  void LockTiles(bool bUpdate = true) override {}
  void LockTilesForEditor() override {}
  void UnlockTiles(const bool bUpdate = true) override {}
  void FixUnlocking() override {}
  void UnfixUnlocking() override {}
  bool CanTurnToFrontDir(const WORD wDir) override { return true; }

  const CVec2 GetNearFormationPos() const;
  const CVec2 GetFarFormationPos() const;
  void GetNextTiles(std::list<SVector> *pTiles) const;

  void NotifyDiff(float fDiff);

  float GetSmoothTurnThreshold() const override;
  const int CanGoBackward() const override { return false; }

  virtual const float GetCurSpeedBonus() const = 0;
  virtual const float GetRadius() const = 0;
  virtual const int Size() const = 0;
  bool IsLockingTiles() const override { return false; }

  bool CheckToTurn(const WORD wNewDir) override { return true; }
  bool HasSuspendedPoint() const override { return false; }

  bool IsInOneTrain(interface IBasePathUnit *pUnit) const override;
  bool IsTrain() const override { return false; }

  // number of segments that have passed since the last SecondSegment call
  virtual const float GetPathSegmentsPeriod() const;

  const SVector GetLastKnownGoodTile() const override;

  bool IsDangerousDirExist() const override { return false; }
  const WORD GetDangerousDir() const override { return 0; }
};

class CFormation : public CFormationCenter
{
  OBJECT_NORMAL_METHODS(CFormation);
  DECLARE_SERIALIZE;

  struct SUnitInfo
  {
    DECLARE_SERIALIZE;

    struct SSquadGeometry
    {
      // rotation from formation direction to unit
      CVec2 vForm2Unit;
      // projection of the unit's displacement relative to the center of the formation onto the direction of the formation
      float fUnitProj;
      // the unit's own direction, from the center of the formation
      WORD dir;

      SSquadGeometry() : vForm2Unit(VNULL2), fUnitProj(0), dir(0) {}
    };

  public:
    class CSoldier *pUnit;
    int nSlotInStats;
    std::vector<SSquadGeometry> geoms;

    SUnitInfo() : pUnit(nullptr), nSlotInStats(-1) {}
  };

  std::vector<SUnitInfo> units;
  int nUnits;

  struct SCommonGeometryInfo
  {
    float fMaxUnitProj;
    // formation radius
    float fRadius;

    SCommonGeometryInfo() : fMaxUnitProj(0), fRadius(0) {}
  };

  std::vector<SCommonGeometryInfo> geomInfo;
  int nCurGeometry;

  float fPass;

  NTimer::STime timeToCamouflage;
  WORD id;

  CArray1Bit availCommands;

  //
  BYTE cPlayer;

  struct SGunInfo
  {
    int nUnit;
    int nUnitGun;

    SGunInfo() : nUnit(-1), nUnitGun(-1) {}
    SGunInfo(const int _nUnit, const int _nUnitGun) : nUnit(_nUnit), nUnitGun(_nUnitGun) {}
  };

  std::vector<SGunInfo> guns;

  bool bWaiting;
  CGDBPtr<SSquadRPGStats> pStats;

  bool bDisabled;

  enum EObjectInsideOf { EOIO_NONE, EOIO_BUILDING, EOIO_TRANSPORT, EOIO_ENTRENCHMENT, EOIO_UNKNOWN };

  EObjectInsideOf eInsideType;
  IRefCount *pObjInside;

  float fMaxFireRange;

  struct SVirtualUnit
  {
    DECLARE_SERIALIZE;

  public:
    SVirtualUnit() : pSoldier(nullptr) {}

    CSoldier *pSoldier;
    int nSlotInStats;
  };

  std::vector<SVirtualUnit> virtualUnits;
  int nVirtualUnits;
  bool bCanBeResupplied;

  // for storing data on a portable mortar
  class CCarryedMortar
  {
    DECLARE_SERIALIZE;
    bool bHasMortar;
    CGDBPtr<SUnitBaseRPGStats> pStats;
    float fHP;
    int nDBID;

  public:
    CCarryedMortar() : bHasMortar(false), nDBID(-1) {}
    bool HasMortar() const { return bHasMortar; }
    int CreateMortar(const class CFormation *pOwner);
    void Init(const class CAIUnit *pArt);
  };

  CCarryedMortar mortar;
  bool bBoredInMoveFormationSent;
  NTimer::STime lastBoredInMoveFormationCheck;

  bool bWithMoraleOfficer;

  //
  void InitGeometries();
  void PrepareToDelete();

  // check for package bored if in movement formation
  void CheckForMoveFormationBored();
  bool IsMemberResting(class CSoldier *pSoldier) const;
  void ProcessLoadCommand(CAICommand *pCommand, bool bPlaceInQueue);

public:
  CFormation() : pObjInside(nullptr), bWithMoraleOfficer(false) {}
  void Init(const SSquadRPGStats *pStats, const CVec2 &center, int z, WORD dir, int dbID);
  // moves the center of the formation to its center of mass and initializes geomInfo
  void MoveGeometries2Center();
  void ChangeGeometry(int nGeometry);
  const int GetNGeometries() const;
  const int GetCurGeometry() const { return nCurGeometry; }

  const WORD GetID() const override { return id; }
  const SSquadRPGStats *GetStats() const { return pStats; }

  // add a new unit to the formation, its serial number in the stats is nSlot, the unit’s location is initialized
  void AddNewUnitToSlot(class CSoldier *pUnit, int nSlot, bool bSendToWorld = true);

  // add a new unit to the nPos position in the list of units of the formation object, and the location of the unit is not initialized
  void AddUnit(class CSoldier *pUnit, int nPos);
  void DelUnit(BYTE cPos);
  void DelUnit(class CSoldier *pUnit);
  // for Save/Load
  void SetUnitToPos(BYTE cPos, class CSoldier *pUnit);

  BYTE GetAIClass() const override;

  const CVec2 GetUnitCoord(BYTE cSlot) const;
  const float GetUnitLineShift(BYTE cPos) const;
  const float GetMaxProjection() const { return geomInfo[nCurGeometry].fMaxUnitProj; }
  // the unit's shift relative to the center of the formation when the unit is positioned correctly
  const CVec2 GetUnitShift(BYTE cSlot) const;
  // what should the unit's own direction be?
  const WORD GetUnitDir(BYTE cSlot) const;
  // returns the position in the formation stats for a unit with serial number cSlot in the units array
  const int GetUnitSlotInStats(BYTE cSlot) const;
  const float GetPassability() const override { return fPass; }

  bool IsStopped() const;
  const CVec2 &GetCenter() const override { return CFormationCenter::GetCenter(); }
  const float GetRadius() const override { return geomInfo[nCurGeometry].fRadius; }

  void Segment() override;

  void GetNextPositions(BYTE cPos, std::list<SVector> *pTiles) const;
  const CVec2 GetFarUnitPos(BYTE cPos);
  bool IsIdle() const override;
  bool IsTurning() const override { return false; }
  // are all units in rest state?
  bool IsEveryUnitResting() const;
  bool IsEveryUnitInTransport() const;
  void StopUnit() override;
  void StopTurning() override;
  void ForceGoByRightDir() override {}

  interface IStatesFactory *GetStatesFactory() const override;

  const int Size() const override { return nUnits; }

  class CSoldier *operator[](const int n) const
  {
    NI_ASSERT_T(n < nUnits, "Wrong unit number");
    return units[n].pUnit;
  }

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  bool CanCommandBeExecutedByStats(class CAICommand *pCommand) override;
  bool CanCommandBeExecutedByStats(int nCmd) const override;

  //
  const bool CanShootToPlanes() const override;
  int GetNGuns() const override { return guns.size(); }
  class CBasicGun *GetGun(int n) const override;

  class CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) override;

  const BYTE GetPlayer() const override { return cPlayer; }
  void SetPlayerForEditor(int nPlayer) override;
  void ChangePlayer(BYTE cPlayer) override;

  void SetSelectable(bool bSelectable) override;

  const float GetSightRadius() const override;

  //
  void SetToWaitingState() { bWaiting = true; }
  void UnsetFromWaitingState() { bWaiting = false; }
  const bool IsInWaitingState() const { return bWaiting; }

  const bool IsVisible(BYTE party) const override;

  //
  void WasHitNearUnit();

  void Fired(const float fGunRadius, const int nGun) override {}

  void SetAmbush() override;
  void RemoveAmbush() override;

  const NTimer::STime GetTimeToCamouflage() const override;
  void SetCamoulfage() override;
  void RemoveCamouflage(ECamouflageRemoveReason eReason) override;

  void UpdateArea(EActionNotify eAction) override;

  class CTurret *GetTurret(const int nTurret) const override { return nullptr; }
  const int GetNTurrets() const override { return 0; }
  bool IsMech() const override { return false; }

  void Disappear() override;
  void Die(bool fromExplosion, float fDamage) override;

  bool IsAlive() const override;
  // returns whether I went or not
  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(IPath *pPath) override;

  const float GetCurSpeedBonus() const override;
  virtual const float GetDispersionBonus() const;
  virtual const float GetRelaxTimeBonus() const;
  virtual const float GetFireRateBonus() const;
  virtual const float GetCoverBonus() const;
  const float GetSightMultiplier() const;

  bool IsAllowedLieDown() const;
  bool IsAllowedStandUp() const;

  void Disable();
  void Enable();
  bool IsDisabled() const { return bDisabled; }

  void UnitCommand(CAICommand *pCommand, bool bPlaceInQueue, bool bOnlyThisUnitCommand) override;

  bool IsFree() const override { return eInsideType == EOIO_NONE; }
  bool IsInBuilding() const { return eInsideType == EOIO_BUILDING; }
  bool IsInEntrenchment() const { return eInsideType == EOIO_ENTRENCHMENT; }
  bool IsInTransport() const { return eInsideType == EOIO_TRANSPORT; }

  void SetFree();
  void SetInBuilding(class CBuilding *pBuilding);
  void SetInTransport(class CMilitaryCar *pUnit);
  void SetInEntrenchment(class CEntrenchment *pEntrenchment);

  class CBuilding *GetBuilding() const;
  class CEntrenchment *GetEntrenchment() const;
  class CMilitaryCar *GetTransportUnit() const;

  void GetNewUnitInfo(struct SNewUnitInfo *pNewUnitInfo) override;

  bool IsFormation() const override { return true; }

  void SendAcknowledgement(EUnitAckType ack, bool bForce = false) override;
  void SendAcknowledgement(CAICommand *pCommand, EUnitAckType ack, bool bForce = false) override;
  // set the center of the formation to the units' center of gravity
  void BalanceCenter();

  const int GetMinArmor() const override { return 0; }
  const int GetMaxArmor() const override { return 0; }
  const int GetMinPossibleArmor(const int nSide) const override { return 0; }
  const int GetMaxPossibleArmor(const int nSide) const override { return 0; }
  const int GetArmor(const int nSide) const override { return 0; }
  const int GetRandomArmor(const int nSide) const override { return 0; }

  float GetMaxFireRange() const override { return fMaxFireRange; }
  void AddAvailCmd(const EActionCommand &eCmd) { availCommands.SetData(eCmd); }

  EUnitAckType GetGunsRejectReason() const override;

  // used only for deferred updates
  virtual const bool IsVisible(const int nParty) const { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }

  const int VirtualUnitsSize() const { return nVirtualUnits; }
  const int GetVirtualUnitSlotInStats(int nVirtualUnit) const;
  void AddVirtualUnit(class CSoldier *pSoldier, int nSlotInStats);
  void MakeVirtualUnitReal(class CSoldier *pSoldier);
  void DelVirtualUnit(class CSoldier *pSoldier);

  // for bored condition
  void UnRegisterAsBored(enum EUnitAckType eBoredType) override;
  void RegisterAsBored(enum EUnitAckType eBoredType) override;

  // sets soldier bonuses from the formation
  void SetGeometryPropertiesToSoldier(class CSoldier *pSoldier, bool bChangeWarFog);

  // for carrying a mortar
  void SetCarryedMortar(class CAIUnit *pMortar);
  bool HasMortar() const;// true when formation carries a mortar
  // returns ID of installed artillery
  int InstallCarryedMortar();
  // CRAP}

  void ResetTargetScan() override;
  // scan if it's time; 
  BYTE AnalyzeTargetScan(CAIUnit *pCurTarget, bool bDamageUpdated, bool bScanForObstacles, IRefCount *pCheckBuilding) override;
  void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun) override;

  bool CanMoveForGuard() const override { return CanMove(); }
  float GetPriceMax() const override;
  const NTimer::STime &GetBehUpdateDuration() const override { return SConsts::BEH_UPDATE_DURATION; }
  // to protect from human resupply formation in some states
  virtual bool IsResupplyable() const { return bCanBeResupplied; }
  virtual void SetResupplyable(const bool _bCanBeResupplied) { bCanBeResupplied = _bCanBeResupplied; }

  virtual const IsWithMoraleOfficer (
  )
  const
 { return bWithMoraleOfficer; }

  void FreezeByState(bool bFreeze) override;

  // slow working
  const float GetTargetScanRadius() override;

  bool CanMoveAfterUserCommand() const override { return CanMove(); }
};

#endif // __FORMATION_H__