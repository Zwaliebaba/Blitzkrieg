#ifndef __AI_STATIC_MAP_H__
#define __AI_STATIC_MAP_H__

#pragma once

#include "RectTiles.h"
#include "BetaSpline3D.h"
#include "../Misc/BitData.h"
#include "../Formats/fmtMap.h"
#include <set>

// **********************************************************************
// *CStaticMap*
// **********************************************************************

enum ELockMode { ELM_STATIC = 0, ELM_ALL = 1 };

enum ETerrainTypes { ETT_EARTH_TERRAIN = 0, ETT_RIVER_TERRAIN = 1, ETT_EARTH_SEA_TERRAIN = 2 };

class CStaticMap
{
  DECLARE_SERIALIZE;

  CArray2D1Bit entrenchPossibility;// impossibility of building trenches on the tile
  CArray2D1Bit buf[5];
  CArray2D<BYTE> unitsBuf;
  CArray2D4Bit maxes[2][5];

  CArray2D<BYTE> transparency;
  CArray2D<BYTE> passTypes;
  std::vector<float> passabilities;
  // by terrain type - its ai cross-country ability
  std::vector<DWORD> passClasses;
  // by terrain tile number - its type
  std::vector<BYTE> terrSubTypes;

  std::hash_map<int, SRect> unitsRects;

  // heights at the nodes of the visual grid grid
  CArray2D<float> heights;
  CBetaSpline3D betaSpline3D;
  // heights at the centers of AI tiles
  CArray2D<float> tileHeights;

  CArray2D4Bit terrainTypes;

  int nSizeX, nSizeY;// map size in tiles
  int nCellsSizeX, nCellsSizeY;// --- in cells
  int nBigCellsSizeX, nBigCellsSizeY;// --- in large cells

  // 0 - static 
  ELockMode eMode;
  ELockMode eMemMode;

  static std::vector<SVector> oneWayDirs;
  static std::vector<BYTE> classToIndex;

  int tmpUnlockID;

  struct STmpUnlockUnitsBuf
  {
    SVector tile;
    int nUnitsBuf;
    BYTE bufLocking;
  };

  std::list<STmpUnlockUnitsBuf> tmpUnlockUnitsBuf;

  CArray2D1Bit bridgeTiles;
  // types of soil - is it possible to leave traces, generate dust, etc.
  CArray2D<BYTE> soil;

  //
  void CommonInit();
  void InitMaxes();
  void LoadPassabilities(const struct STerrainInfo &terrainInfo);
  void LoadRivers(const struct STerrainInfo &terrainInfo);
  void Load3DRoads(const STerrainInfo &terrainInfo);
  void LoadNormals(const STerrainInfo::TVertexAltitudeArray2D &initHeights);
  void InitExplosionTerrainTypes();

  void UpdateMaxesForAddedTiles(int downX, int upX, int downY, int upY, BYTE cl);
  void UpdateMaxesForRemovedTiles(int downX, int upX, int downY, int upY, BYTE cl);

  bool UnitLocksTile(const SVector &tile);
  bool UnitUnlocksTile(const SVector &tile);

  // locked only for the index of this class, without taking into account AI_CLASS_ANY
  // if it is checked for AI_CLASS_ANY, then the location of units is also taken into account
  bool IsLocked4Class(const int x, const int y, const BYTE aiClassIndex) const { return x < 0 || y < 0 || x >= nSizeX || y >= nSizeY || buf[aiClassIndex].GetData(x, y) || (unitsBuf[y][x] * eMode * static_cast<BYTE>(aiClassIndex == classToIndex[AI_CLASS_ANY])) != 0; }
  // locked only for this class, without taking into account AI_CLASS_ANY
  bool IsLocked4Class(const SVector &coord, const BYTE aiClass) const { return IsLocked4Class(coord.x, coord.y, aiClass); }

  //
  void AddRiverTiles(const CTilesSet &tiles);
  void AddEarthSeaTiles(const CTilesSet &tiles);

  // for debugging
  bool CanPut(int x, int y, int d, BYTE aiClass);
  void GetPoint4Spline(const CVec2 &vPoint, float *pu, float *pv, float ptCtrls[]) const;

public:
  bool CheckMaxes(int downX, int upX, int downY, int upY, BYTE aiClass);
  CStaticMap() : nSizeX(0), nSizeY(0), eMode(ELM_ALL), eMemMode(ELM_ALL) {}

  void Clear();
  void LoadMap(const struct STerrainInfo &terrainInfo, bool bLoadRivers = true);

  void SetMode(const ELockMode _eMode) { eMode = _eMode; }

  void MemMode() { eMemMode = eMode; }
  void RestoreMode() { eMode = eMemMode; }

  // operation with tiles and coordinates
  const int GetSizeX() const { return nSizeX; }
  const int GetSizeY() const { return nSizeY; }
  const int GetCellsSizeX() const { return nCellsSizeX; }
  const int GetCellsSizeY() const { return nCellsSizeY; }
  const int GetBigCellsSizeX() const { return nBigCellsSizeX; }
  const int GetBigCellsSizeY() const { return nBigCellsSizeY; }

  // cell num by the cells coordinates
  const int GetCellNum(int x, int y) const
  {
    NI_ASSERT_T(x >= 0 && y >= 0, "Wrong coordinates in GetCellNum");
    return (y * nCellsSizeX + x);
  }

  // big cell num by the cells coordinates
  const int GetBigCellNum(int x, int y) const
  {
    NI_ASSERT_T(x >= 0 && y >= 0, "Wrong coordinates in GetCellNum");
    return (y * nCellsSizeX + x);
  }

  bool IsTileInside(const int x, const int y) const { return x >= 0 && y >= 0 && x < nSizeX && y < nSizeY; }
  bool IsTileInside(const SVector &tile) const { return IsTileInside(tile.x, tile.y); }

  // lock static 
  void LockTile(const int x, const int y, const BYTE aiClasses)
  {
    if (aiClasses == AI_CLASS_ANY) buf[classToIndex[aiClasses]].SetData(x, y);
    else { for (int i = 1; i < 16; i *= 2) { if (i & aiClasses) buf[classToIndex[i]].SetData(x, y); } }
  }

  // unlock from static 
  void UnlockTile(const int x, const int y, const BYTE aiClasses)
  {
    if (aiClasses == AI_CLASS_ANY) buf[classToIndex[aiClasses]].RemoveData(x, y);
    else { for (int i = 1; i < 16; i *= 2) { if (i & aiClasses) buf[classToIndex[i]].RemoveData(x, y); } }
  }

  bool IsLockedForResupply(const int x, const int y) const { return IsLocked(x, y, AI_CLASS_HUMAN); }
  // locked taking into account eMode
  bool IsLocked(const int x, const int y, const BYTE aiClass) const { return x < 0 || y < 0 || x >= nSizeX || y >= nSizeY || buf[classToIndex[aiClass]].GetData(x, y) || buf[classToIndex[AI_CLASS_ANY]].GetData(x, y) || (unitsBuf[y][x] * eMode) != 0; }
  void LockTile(const SVector &coord, const BYTE aiClasses) { LockTile(coord.x, coord.y, aiClasses); }
  void UnlockTile(const SVector &coord, const BYTE aiClasses) { UnlockTile(coord.x, coord.y, aiClasses); }
  bool IsLocked(const SVector &coord, const BYTE aiClass) const { return IsLocked(coord.x, coord.y, aiClass); }

  BYTE GetTileLockInfo(const int x, const int y) const
  {
    return
        IsStaticLocked4Class(x, y, AI_CLASS_ANY) * AI_CLASS_ANY |
        IsStaticLocked4Class(x, y, AI_CLASS_WHEEL) * AI_CLASS_WHEEL | IsStaticLocked4Class(x, y, AI_CLASS_HUMAN) * AI_CLASS_HUMAN |
        IsStaticLocked4Class(x, y, AI_CLASS_HALFTRACK) * AI_CLASS_HALFTRACK | IsStaticLocked4Class(x, y, AI_CLASS_TRACK) * AI_CLASS_TRACK;
  }

  BYTE GetTileLockInfo(const SVector &tile) const { return GetTileLockInfo(tile.x, tile.y); }

  // locked specifically for this class by a static object
  bool IsStaticLocked4Class(const int x, const int y, const BYTE aiClass) const { return x < 0 || y < 0 || x >= nSizeX || y >= nSizeY || buf[classToIndex[aiClass]].GetData(x, y); }
  bool IsStaticLocked4Class(const SVector &coord, const BYTE aiClass) const { return IsStaticLocked4Class(coord.x, coord.y, aiClass); }

  const float GetVisZ(float x, float y) const;
  const float GetZ(const SVector &tile) const { return 0; }
  const DWORD GetNormal(const CVec2 &vPoint) const { return GetNormal(vPoint.x, vPoint.y); }
  const DWORD GetNormal(float x, float y) const;
  const float GetTileHeight(const SVector &tile) const;

  // units related functions
  bool CanUnitGo(const int nBoundTileRadius, const SVector &tile, const SVector &dir, const BYTE aiClass) const
  {
    const SVector tileToGo(tile + dir);
    return
        tileToGo.x >= 0 && tileToGo.y >= 0 && tileToGo.x < GetSizeX() && tileToGo.y < GetSizeY() &&
        maxes[eMode][classToIndex[aiClass]].GetData(tileToGo.x, tileToGo.y) >= nBoundTileRadius + 1 &&
        maxes[eMode][classToIndex[AI_CLASS_ANY]].GetData(tileToGo.x, tileToGo.y) >= nBoundTileRadius + 1;
  }

  bool CanUnitComplexGo(const int nBoundTileRadius, const SVector &tile, const SVector &dir, const BYTE aiClass) const
  {
    if (dir.x == 0 || dir.y == 0) return CanUnitGo(nBoundTileRadius, tile, dir, aiClass);
    return CanUnitGo(nBoundTileRadius, tile, SVector(dir.x, 0), aiClass);
  }

  bool CanUnitGo(const int nBoundTileRadius, const SVector &tile, const BYTE aiClass) const
  {
    return
        tile.x >= 0 && tile.y >= 0 && tile.x < GetSizeX() && tile.y < GetSizeY() &&
        maxes[eMode][classToIndex[aiClass]].GetData(tile.x, tile.y) >= nBoundTileRadius + 1 && maxes[eMode][classToIndex[AI_CLASS_ANY]].GetData(tile.x, tile.y) >= nBoundTileRadius + 1;
  }

  bool CanUnitGoToPoint(const int nBoundTileRadius, const CVec2 &point, const BYTE aiClass)
  {
    const SVector tileToGo(AICellsTiles::GetTile(point));
    return point.x >= 0 && point.y >= 0 && CanUnitGo(nBoundTileRadius, tileToGo, aiClass);
  }

  bool IsBroken(const int nBoundTileRadius, const SVector &p0, const SVector &p1, const SVector &p2, const BYTE aiClass)
  {
    const SVector v1 = p2 - p1;
    const SVector v2 = p1 - p0;

    if (v1 == v2) return false;
    return !CanUnitGo(nBoundTileRadius, p0 + v1, aiClass);
  }

  void AddLockedUnitTiles(const SRect &rect, int id, bool bAddToRects, bool bUpdate = true);
  void RemoveLockedUnitTiles(int id, bool bUpdate);

  void UpdateMaxesByTiles(const CTilesSet &tiles, BYTE aiClasses, bool bAdd);
  void CalcMaxesBoundsByTiles(const CTilesSet &tiles, SVector *vMin, SVector *vMax);

  void UpdateMaxesForAddedStObject(int downX, int upX, int downY, int upY, BYTE aiClasses);
  void UpdateMaxesForRemovedStObject(int downX, int upX, int downY, int upY, BYTE aiClasses);
  void UpdateMaxesForAddedRect(int nDownX, int nDownY, int nUpX, int nUpY);
  void UpdateMaxesForRemovedRect(int nDownX, int nDownY, int nUpX, int nUpY);

  bool IsRectInside(const SRect &rect) const
  {
    return IsPointInside(rect.v1) &&
           IsPointInside(rect.v2) &&
           IsPointInside(rect.v3) &&
           IsPointInside(rect.v4);
  }

  bool IsBigCellInside(const SVector &bigCell) const { return (bigCell.x >= 0 && bigCell.y >= 0 && bigCell.x < GetBigCellsSizeX() && bigCell.y < GetBigCellsSizeY()); }
  bool IsPointInside(const float x, const float y) const { return x >= 0 && y >= 0 && x < GetSizeX() * SAIConsts::TILE_SIZE && y < GetSizeY() * SAIConsts::TILE_SIZE; }
  bool IsPointInside(const CVec2 &point) const { return IsPointInside(point.x, point.y); }

  //
  void SetTransparency(const int x, const int y, const BYTE cTransp)
  {
    transparency[y][x] &= ~7;
    transparency[y][x] |= cTransp;
  }

  void SetTransparency(const SVector &tile, const BYTE cTransp) { SetTransparency(tile.x, tile.y, cTransp); }
  void RemoveTransparency(const int x, const int y) { transparency[y][x] &= ~7; }
  void RemoveTransparency(const SVector &tile) { RemoveTransparency(tile.x, tile.y); }

  const BYTE GetDissipation(const SVector &tile) const
  {
    if (tile.x < 0 || tile.y < 0 || tile.x >= nSizeX || tile.y >= nSizeY) return SAIConsts::VIS_POWER;
    return transparency[tile.y][tile.x] & 7;
  }

  //
  void SetOneWayTransp(const int x, const int y, const BYTE cDir)
  {
    transparency[y][x] &= 7;
    transparency[y][x] |= ((cDir << 1) | 1) << 3;
  }

  void RemoveOneWayTransp(const int x, const int y) { transparency[y][x] &= 7; }

  bool IsOneWayTransp(const int x, const int y) const { return transparency[y][x] & 8; }
  bool IsOneWayTransp(const SVector &tile) const { return IsOneWayTransp(tile.x, tile.y); }

  const SVector &GetOneWayTransp(const int x, const int y) const
  {
    NI_ASSERT_T(IsOneWayTransp( x, y ), "Effort to call GetOneWayTransp for a cell without one way transparency");
    return oneWayDirs[transparency[y][x] >> 4];
  }

  const SVector &GetOneWayTransp(const SVector &tile) const { return GetOneWayTransp(tile.x, tile.y); }

  const bool GetIntersectionWithTerrain(CVec3 *pvResult, const CVec3 &vBegin, const CVec3 &vEnd) const;

  void ApplyPattern(const struct SVAPattern &rPattern);
  void SetHeightForPatternApplying(int nX, int nY, float fHeight);
  void UpdateAllHeights();

  const ETerrainTypes GetTerrainType(int nX, int nY) const;
  const ETerrainTypes GetTerrainType(const SVector &tile) const { return GetTerrainType(tile.x, tile.y); }

  // true - if unlock was performed when calling, false - if unlock was already done before
  bool TemporaryUnlockUnitRect(int id);
  void RemoveTemporaryUnlocking();
  // if unit id temporarily locates tiles, then delete
  void RemoveTemporaryUnlockingByUnit(int id);

  // terrain passability
  const float GetPass(const CVec2 &point) const { return GetPass(point.x, point.y); }

  const float GetPass(const int nX, const int nY) const
  {
    if (!IsPointInside(nX, nY)) return 1;
    SVector visTile = AICellsTiles::GetTile(nX, nY);
    visTile.x >>= 1;
    visTile.y >>= 1;

    return passabilities[passTypes[visTile.y][visTile.x]];
  }

  const int GetTerrainPassabilityType(int nX, int nY) const;
  // unlocks the tile according to the terrain's passability
  void RemoveTerrainPassability(int nX, int nY);
  // in the rectangle either removes the terrain or puts the old one
  void UpdateTerrainPassabilityRect(int nMinX, int nMinY, int nMaxX, int nMaxY, bool bRemove);
  // locates the tile and sets the cross-country ability in accordance with the terrain nTerrainType,
  void SetTerrainPassability(int nX, int nY, int nTerrainType);
  // by tile number shows the terrain type
  const int GetTerrainPassTypeByTileNum(const int nTile) { return terrSubTypes[nTile]; }
  const bool CanDigEntrenchment(int x, int y) const;
  const void AddUndigableTiles(const CTilesSet &tiles);

  // is the tile on a bridge?
  const bool IsBridge(const SVector &tile) const;
  void AddBridgeTile(const SVector &tile);
  void RemoveBridgeTile(const SVector &tile);

  BYTE GetSoilType(const SVector &tile) const
  {
    NI_ASSERT_T(IsTileInside( tile ), NStr::Format( "Wrong tile (%d, %d) passed", tile.x, tile.y ));
    return soil[tile.y][tile.x];
  }

  void UpdateRiverPassability(const SVectorStripeObject &river, bool bAdd, bool bUpdate);
};

class CTemporaryUnitRectUnlocker
{
  bool bLocking;

public:
  explicit CTemporaryUnitRectUnlocker(int nUnitID);
  ~CTemporaryUnitRectUnlocker();
};

#endif // __AI_STATIC_MAP_H__