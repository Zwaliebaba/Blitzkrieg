#ifndef __STATIC_OBJECTS_H__
#define __STATIC_OBJECTS_H__

#pragma once

#include "../Misc/AreaMap.h"
#include "../Misc/HashFuncs.h"
#include "../Common/Actions.h"
#include "RectTiles.h"
#include <set>
#include "PathFinder.h"

class CExistingObject;
class CStaticObject;
class CBuildingStorage;

template<bool bOnlyContainers>
class CStObjIter;
using CStoragesList = std::list<CPtr<CBuildingStorage>>;
using CIntHash = std::unordered_set<int>;

// **********************************************************************
// *CStaticObjects*
// **********************************************************************

interface IObstacle;
interface IUpdatableObj;

class CStaticObjects : public IRefCount
{
  OBJECT_NORMAL_METHODS(CStaticObjects);
  DECLARE_SERIALIZE;

  using ObstacleAreaMap = CAreaMap<IObstacle, CPtr<IObstacle>, SVector, int>;
  using ObstacleObjectMap = std::unordered_map<int, CPtr<IObstacle>>;

  ObstacleAreaMap obstacles;
  ObstacleObjectMap obstacleObjects;

  using StaticObjectsAreaMap = CAreaMap<CExistingObject, CObj<CExistingObject>, SVector, int>;
  StaticObjectsAreaMap areaMap;
  StaticObjectsAreaMap containersAreaMap;
  int nObjs;

  std::list<CObj<IRefCount>> entrenchments;

  struct SSegmentObjectsSort
  {
    bool operator()(const CPtr<CStaticObject> &segmObj1, const CPtr<CStaticObject> &segmObj2) const;
  };

  // for iterators
  bool bIterCreated;
  StaticObjectsAreaMap &GetAreaMap() { return areaMap; }
  StaticObjectsAreaMap &GetContainersAreaMap() { return containersAreaMap; }
  void SetIterCreated(bool _bCreated) { bIterCreated = _bCreated; }
  bool IsIterCreated() const { return bIterCreated; }

public:
  // to iterate through all repositories
  interface IEnumStoragesPredicate
  {
    // iterate only connected storages
    virtual bool OnlyConnected() const = 0;
    // true - finish, what you need has already been found
    // path length - in TILES
    virtual bool AddStorage(class CBuildingStorage *pStorage, float fPathLenght) = 0;
  };

  // for storing information about RU warehouses
  class CStoragesContainer
  {
    DECLARE_SERIALIZE;

    using CStorages = std::unordered_map<CObj<CBuildingStorage>, bool, SUniqueIdHash>;
    using CStoragesList = std::list<CObj<CBuildingStorage>>;

    struct CPartyInfo
    {
      DECLARE_SERIALIZE;

    public:
      CStoragesList mains;
      CStoragesList secondary;
    };

    std::vector<CPartyInfo> storageSystem;

    // helpline points
    CStorages storages;// for speed search storages

    WORD updated;
    bool bInitOnSegment;// CRAP FOR SAVES COMPATIBILITY

    void AddStorage(class CBuildingStorage *pNewStorage, int nPlayer);

  public:
    CStoragesContainer();
    void UpdateForParty(int nParty);
    void Segment();
    void EnumStoragesForParty(int nParty, IEnumStoragesPredicate *pPred);

    void Init()
    {
      Clear();
      storageSystem.resize(2);
      bInitOnSegment = false;
    }

    void EnumStoragesInRange(const CVec2 &vCenter,
                             int nParty,
                             float fMaxPathLenght,
                             float fMaxOffset,
                             class CCommonUnit *pUnitToFindPath,
                             IEnumStoragesPredicate *pPred);

    void AddStorage(class CBuildingStorage *pNewStorage);
    void RemoveStorage(class CBuildingStorage *pNewStorage);
    void StorageChangedDiplomacy(class CBuildingStorage *pNewStorage, int nNewPlayer);
    void Clear();
  };

  CStoragesContainer storagesContainer;

  // trenches are not cultivated here!
  using CSegmObjects = std::set<CPtr<CStaticObject>, SSegmentObjectsSort>;
  CSegmObjects segmObjects;

  using CObjectsHashSet = std::unordered_set<CObj<CExistingObject>, SUniqueIdHash>;
  CObjectsHashSet terraObjs;
  CObjectsHashSet deletedObjects;
  std::list<CPtr<CStaticObject>> unregisteredObjects;

  std::unordered_set<int> burningObjects;

  //
  void AddToAreaMap(CExistingObject *pObj);
  void AddObjectToAreaMapTile(CExistingObject *pObj, const SVector &tile);
  void RemoveFromAreaMap(CExistingObject *pObj);
  void RemoveObjectFromAreaMapTile(CExistingObject *pObj, const SVector &tile);

  CStaticObjects()
    : obstacles(SConsts::STATIC_OBJ_CELL), areaMap(SConsts::STATIC_OBJ_CELL),
      containersAreaMap(SConsts::STATIC_CONTAINER_OBJ_CELL), bIterCreated(false) {}

  void Init(int nMapTileSizeX, int nMapTileSizeY);

  void Clear()
  {
    // storagesContainer2.Clear();
    DestroyContents();
  }

  // for editor
  void RecalcPassabilityForPlayer(CArray2D<BYTE> *array, int nParty);

  void AddObstacle(interface IObstacle *pObstacle);
  void RemoveObstacle(interface IObstacle *pObstacle);

  class CStaticObject *AddNewFenceObject(const SFenceRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, int nDiplomacy, bool bInitialization = false, bool IsEditor = false);
  void AddStaticObject(class CCommonStaticObject *pObj, bool bAlreadyLocked, bool bInitialization = false);
  class CStaticObject *AddNewStaticObject(const SObjectBaseRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, bool bInitialization = false);
  class CStaticObject *AddNewTerraObj(const SObjectBaseRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, bool bInitialization = false);
  class CStaticObject *AddNewTerraMeshObj(const SObjectBaseRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, WORD wDir, int nFrameIndex, bool bInitialization = false);
  class CStaticObject *AddNewBuilding(const SBuildingRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, bool bInitialization = false);
  class CStaticObject *AddNewStorage(const SBuildingRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, int player, bool bInitialization = false);
  class CStaticObject *AddNewFlag(const SStaticObjectRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, int player, bool bInitialization = false);
  void AddStorage(class CBuildingStorage *pObj, bool bInitialization = false);
  class CStaticObject *AddNewEntrencmentPart(const SEntrenchmentRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, WORD dir, int nFrameIndex, bool bInitialization = false);
  void AddEntrencmentPart(class CEntrenchmentPart *pObj, bool bLockedAlready, bool bInitialization = false);
  class CStaticObject *AddNewEntrencment(IRefCount **segments, int nLen, class CFullEntrenchment *pFullEntrenchment, bool bInitialization = false);
  class CStaticObject *AddNewBridgeSpan(const SBridgeRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, WORD ir, int nFrameIndex, bool bInitialization = false);
  class CStaticObject *AddNewSmokeScreen(const CVec2 &vCenter, float fR, int nTransparency, int nTime);

  class CStaticObject *AddNewMine(const SMineRPGStats *pStats, float fHPFactor, int dbID, const CVec2 &center, int nFrameIndex, int player, bool bInitialization = false);
  class CExistingObject *AddNewTankPit(const SMechUnitRPGStats *pStats, const CVec2 &center, WORD dir, int nFrameIndex, int dbID, const class CVec2 &vHalfSize, const CTilesSet &tilesToLock, class CAIUnit *pOwner, bool bInitialization = false);
  void GetNewStaticObjects(struct SNewUnitInfo **pObjects, int *pnLen);
  void GetDeletedStaticObjects(IRefCount ***pObjects, int *pnLen);

  void RegisterSegment(class CStaticObject *pObj);
  void UnregisterSegment(class CStaticObject *pObj);

  // void UpdateRUStorageAreas( const EActionNotify eAction, const int nDipl );

  void Segment();

  // for RU storages
  // void UpdateStoragesForParty( const int nParty, const bool bNewStorage, const bool bIncreasePassibility );
  // void UpdateAllPartiesStorages( const bool bNewStorage, const bool bIncreasePassibility );

  // are called only by the objects being deleted themselves
  void DeleteInternalObjectInfo(class CExistingObject *pObj);
  void DeleteInternalObjectInfoForEditor(class CExistingObject *pObj);
  void DeleteInternalEntrenchmentInfo(class CEntrenchment *pEntrench);

  void StartBurning(class CExistingObject *pObj);
  void EndBurning(class CExistingObject *pObj);

  // bool IsPointUnderSupply( const int nPlayer, const CVec2 &vCenter ) const;
  void StorageChangedDiplomacy(class CBuildingStorage *pNewStorage, int nNewPlayer);

  // for the editor
  void UpdateAllObjectsPos();

  void EnumObstaclesInRange(const CVec2 &vCenter, float fRadius, interface IObstacleEnumerator *f);
  void EnumStoragesForParty(int nParty, interface IEnumStoragesPredicate *pPred);
  void EnumStoragesInRange(const CVec2 &vCenter, int nParty, float fMaxPathLength, float fMaxOffset,
                           class CCommonUnit *pUnitToFindPath, interface IEnumStoragesPredicate *pPred);


  friend class CStObjIter<false>;
  friend class CStObjIter<true>;
};

#endif // __STATIC_OBJECTS_H__