// there is extra code here
// REMOVE_OBJECTS_FROM_RECT
// linkID update happens here
// UPDATE_LINK_ID

#include "stdafx.h"

#include "MapInfo_Types.h"
#include "LA_Types.h"
#include "VA_Types.h"
#include "Polygons_Types.h"
#include "MiniMap_Types.h"
#include "VSO_Types.h"
#include "Resource_Types.h"
#include "IB_Types.h"

#include "../Formats/fmtTerrain.h"

#include "../AILogic/AIConsts.h"
#include "TerrainBuilder.h"

// #include "RMG_Polygons.h"
// #include "RMG_Image.h"
// #include "VSO_Types.h"
// #include "RMG_Types.h"
// #include "MiniMap_Types.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// just some simple number
const int CMapInfo::RANDOM_SEED = 17;
int CMapInfo::nCurRandomSeed = 0;

bool CMapInfo::PointInMap(const SLoadMapInfo &rLoadMapInfo, float x, float y, bool bAIPoint)
{
  if (bAIPoint)
  {
    return ((x >= 0.0f) &&
            (y >= 0.0f) &&
            (x <= rLoadMapInfo.terrain.tiles.GetSizeX() * 2.0f * SAIConsts::TILE_SIZE) &&
            (y <= rLoadMapInfo.terrain.tiles.GetSizeY() * 2.0f * SAIConsts::TILE_SIZE));
  }
  return ((x >= 0.0f) &&
          (y >= 0.0f) &&
          (x <= rLoadMapInfo.terrain.tiles.GetSizeX() * fWorldCellSize) &&
          (y <= rLoadMapInfo.terrain.tiles.GetSizeY() * fWorldCellSize));
}

bool CMapInfo::GetTileIndicesInternal(const CVec3 &rPoint, int *pnXPosition, int *pnYPosition, const CTPoint<int> &rTerrainSize, float fCellSize, bool isYReverse)
{
  if (rPoint.x >= 0) { *pnXPosition = static_cast<int>(rPoint.x / fCellSize); }
  else { *pnXPosition = static_cast<int>(rPoint.x / fCellSize - 1.0f); }
  if (rPoint.y >= 0) { *pnYPosition = static_cast<int>(rPoint.y / fCellSize); }
  else { *pnYPosition = static_cast<int>(rPoint.y / fCellSize - 1.0f); }

  if (isYReverse) { *pnYPosition = rTerrainSize.y - *pnYPosition - 1; }

  return ((*pnXPosition >= 0) &&
          (*pnXPosition < rTerrainSize.x) &&
          (*pnYPosition >= 0) &&
          (*pnYPosition < rTerrainSize.y));
}


void CMapInfo::PackFrameIndex(IObjectsDB *pGDB, SMapObjectInfo *pInfo)
{
  const SGDBObjectDesc *pDesc = pGDB->GetDesc(pInfo->szName.c_str());
  NI_ASSERT_T(pDesc != 0, NStr::Format( "Can't find descriptor for \"%s\"", pInfo->szName.c_str() ));
  switch (pDesc->eGameType)
  {
    case SGVOGT_FENCE:
    {
      TPackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SFenceRPGStats>(pGDB, pDesc), "fence");
      break;
    }
    case SGVOGT_ENTRENCHMENT:
    {
      TPackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SEntrenchmentRPGStats>(pGDB, pDesc), "entrenchment");
      break;
    }
    case SGVOGT_BRIDGE:
    {
      TPackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SBridgeRPGStats>(pGDB, pDesc), "bridge");
      break;
    }
  }
}


void CMapInfo::UnpackFrameIndex(IObjectsDB *pGDB, SMapObjectInfo *pInfo, int *pRandomSeed)
{
  const SGDBObjectDesc *pDesc = pGDB->GetDesc(pInfo->szName.c_str());
  NI_ASSERT_T(pDesc != 0, NStr::Format( "Can't find descriptor for \"%s\"", pInfo->szName.c_str() ));
  switch (pDesc->eGameType)
  {
    case SGVOGT_FENCE:
    {
      TUnpackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SFenceRPGStats>(pGDB, pDesc), "fence", pRandomSeed);
      break;
    }
    case SGVOGT_ENTRENCHMENT:
    {
      TUnpackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SEntrenchmentRPGStats>(pGDB, pDesc), "entrenchment", pRandomSeed);
      break;
    }
    case SGVOGT_BRIDGE:
    {
      TUnpackFrameIndex(pGDB, pInfo, NGDB::GetRPGStats<SBridgeRPGStats>(pGDB, pDesc), "bridge", pRandomSeed);
      break;
    }
  }
}

bool CMapInfo::GetTerrainTileIndices(const STerrainInfo &rTerrainInfo, const CVec3 &rPoint, CTPoint<int> *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));
  return GetTileIndicesInternal(rPoint, &(pPoint->x), &(pPoint->y), CTPoint<int>(rTerrainInfo.tiles.GetSizeX(), rTerrainInfo.tiles.GetSizeY()), fWorldCellSize, true);
}


bool CMapInfo::GetTileIndices(const STerrainInfo &rTerrainInfo, const CVec3 &rPoint, CTPoint<int> *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));
  return GetTileIndicesInternal(rPoint, &(pPoint->x), &(pPoint->y), CTPoint<int>(rTerrainInfo.tiles.GetSizeX(), rTerrainInfo.tiles.GetSizeY()), fWorldCellSize, false);
}


bool CMapInfo::GetAITileIndices(const STerrainInfo &rTerrainInfo, const CVec3 &rPoint, CTPoint<int> *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));
  return GetTileIndicesInternal(rPoint, &(pPoint->x), &(pPoint->y), CTPoint<int>(rTerrainInfo.tiles.GetSizeX(), rTerrainInfo.tiles.GetSizeY()), fWorldCellSize / 2.0f, false);
}


void CMapInfo::InvertYTile(const STerrainInfo &rTerrainInfo, CTPoint<int> *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));
  pPoint->y = rTerrainInfo.tiles.GetSizeY() - pPoint->y - 1;
}


void CMapInfo::InvertYPosition(const STerrainInfo &rTerrainInfo, CTPoint<float> *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));
  pPoint->y = (rTerrainInfo.tiles.GetSizeY() * fWorldCellSize) - pPoint->y;
}


void CMapInfo::FillDefaultDiplomacies(SLoadMapInfo *pLoadMapInfo)
{
  if (pLoadMapInfo->diplomacies.empty())
  {
    pLoadMapInfo->diplomacies.resize(3);
    pLoadMapInfo->diplomacies[0] = 0;
    pLoadMapInfo->diplomacies[1] = 1;
    pLoadMapInfo->diplomacies[2] = 2;

    pLoadMapInfo->unitCreation.units.resize(2);
    pLoadMapInfo->unitCreation.Validate();
  }

  if (pLoadMapInfo->aiGeneralMapInfo.sidesInfo.empty())
  {
    pLoadMapInfo->aiGeneralMapInfo.sidesInfo.push_back(SAIGeneralSideInfo());
    pLoadMapInfo->aiGeneralMapInfo.sidesInfo.push_back(SAIGeneralSideInfo());
  }

  if (pLoadMapInfo->playersCameraAnchors.empty())
  {
    pLoadMapInfo->playersCameraAnchors.push_back(VNULL3);
    pLoadMapInfo->playersCameraAnchors.push_back(VNULL3);
  }
}


void CMapInfo::Clear(SLoadMapInfo *pLoadMapInfo)
{
  NI_ASSERT_T(pLoadMapInfo != 0,
              NStr::Format( "Wrong parameter: %x\n", pLoadMapInfo ));

  pLoadMapInfo->szSeasonFolder.clear();
  pLoadMapInfo->szMODName.clear();
  pLoadMapInfo->szMODVersion.clear();

  pLoadMapInfo->terrain.szTilesetDesc.clear();
  pLoadMapInfo->terrain.szCrossetDesc.clear();
  // pLoadMapInfo->terrain.szRoadsetDesc.clear();
  pLoadMapInfo->terrain.szNoise.clear();

  pLoadMapInfo->terrain.patches.Clear();
  pLoadMapInfo->terrain.tiles.Clear();

  pLoadMapInfo->terrain.altitudes.Clear();

  pLoadMapInfo->terrain.roads3.clear();
  pLoadMapInfo->terrain.rivers.clear();

  // Initialize objects ( std::vector<SMapObjectInfo> )
  pLoadMapInfo->objects.clear();

  // Initialize entrenchments ( std::vector<SEntrenchmentInfo> )
  pLoadMapInfo->entrenchments.clear();

  // Initialize bridges ( std::vector< std::vector<int> > )
  pLoadMapInfo->bridges.clear();

  // Initialize reinforcements ( SReinforcementGroupInfo )
  pLoadMapInfo->reinforcements.groups.clear();

  // Initialize szScriptFile ( std::string )
  pLoadMapInfo->szScriptFile.clear();
  // Initialize scriptAreas ( std::vector<SScriptArea> )
  pLoadMapInfo->scriptAreas.clear();

  // Initialize vCameraAnchor (CVec3)
  pLoadMapInfo->vCameraAnchor = VNULL3;

  pLoadMapInfo->nSeason = REAL_SEASONS[SEASON_SUMMER];

  // Initialize startCommandsList ( TStartCommandsList )
  pLoadMapInfo->startCommandsList.clear();

  // Initialize reservePositionsList ( TReservePositionsList )
  pLoadMapInfo->reservePositionsList.clear();

  pLoadMapInfo->soundsList.clear();
  pLoadMapInfo->szForestCircleSounds.clear();
  pLoadMapInfo->szForestAmbientSounds.clear();

  // Initialize unitCreation (SUnitCreationInfo)
  pLoadMapInfo->unitCreation.units.clear();

  pLoadMapInfo->diplomacies.clear();
  pLoadMapInfo->aiGeneralMapInfo.sidesInfo.clear();
  pLoadMapInfo->playersCameraAnchors.clear();

  pLoadMapInfo->nType = TYPE_SINGLE_PLAYER;

  FillDefaultDiplomacies(pLoadMapInfo);
}


bool CMapInfo::Create(SLoadMapInfo *pLoadMapInfo, const CTPoint<int> &rSize, int _nSeason, const std::string &rszSeasonFolder, int nPlayersCount, int _nType)
{
  NI_ASSERT_TF(pLoadMapInfo != 0,
               NStr::Format( "CMapInfo::Create(): Wrong parameter pLoadMapInfo %x\n", pLoadMapInfo ),
               return false);
  NI_ASSERT_TF(( _nSeason >= 0 ) && ( _nSeason < REAL_SEASONS_COUNT ),
               NStr::Format( "CMapInfo::Create(): Invalid season: %d\n", _nSeason ),
               return false);
  NI_ASSERT_TF(( rSize.x > 0 ) && ( rSize.y > 0 ) && ( rSize.x <= 32 ) && ( rSize.y <= 32 ),
               NStr::Format( "CMapInfo::Create(): Invalid size: [%dx%d]\n", rSize.x, rSize.y ),
               return false);

  Clear(pLoadMapInfo);

  // Initialize nSeason ( int )
  pLoadMapInfo->nSeason = _nSeason;
  pLoadMapInfo->szSeasonFolder = rszSeasonFolder;

  pLoadMapInfo->nType = _nType;

  // Initialize the terrain ( STerrainInfo )
  pLoadMapInfo->terrain.szTilesetDesc = pLoadMapInfo->szSeasonFolder + RMGC_TILESET_FILE_NAME;
  pLoadMapInfo->terrain.szCrossetDesc = pLoadMapInfo->szSeasonFolder + RMGC_CROSSSET_FILE_NAME;
  // pLoadMapInfo->terrain.szRoadsetDesc = pLoadMapInfo->szSeasonFolder + RMGC_ROADSET_FILE_NAME;
  pLoadMapInfo->terrain.szNoise = pLoadMapInfo->szSeasonFolder + RMGC_NOISE_FILE_NAME;

  pLoadMapInfo->terrain.patches.SetSizes(rSize.x, rSize.y);
  for (int nXIndex = 0; nXIndex < rSize.x; ++nXIndex)
  {
    for (int nYIndex = 0; nYIndex < rSize.y; ++nYIndex)
    {
      STerrainPatchInfo terrainPatchInfo;
      terrainPatchInfo.nStartX = nXIndex * STerrainPatchInfo::nSizeX;
      terrainPatchInfo.nStartY = nYIndex * STerrainPatchInfo::nSizeY;
      pLoadMapInfo->terrain.patches[nYIndex][nXIndex] = terrainPatchInfo;
    }
  }

  pLoadMapInfo->terrain.tiles.SetSizes(rSize.x * STerrainPatchInfo::nSizeX, rSize.y * STerrainPatchInfo::nSizeY);
  for (int nXIndex = 0; nXIndex < (rSize.x * STerrainPatchInfo::nSizeX); ++nXIndex)
  {
    for (int nYIndex = 0; nYIndex < (rSize.y * STerrainPatchInfo::nSizeY); ++nYIndex)
    {
      SMainTileInfo mainTileInfo;
      mainTileInfo.tile = 0;
      mainTileInfo.noise = 1;
      pLoadMapInfo->terrain.tiles[nYIndex][nXIndex] = mainTileInfo;
    }
  }

  // 1 more than tiles!
  pLoadMapInfo->terrain.altitudes.SetSizes(rSize.x * STerrainPatchInfo::nSizeX + 1, rSize.y * STerrainPatchInfo::nSizeY + 1);
  pLoadMapInfo->terrain.altitudes.SetZero();

  if (nPlayersCount > 0)
  {
    pLoadMapInfo->diplomacies.resize(nPlayersCount);
    for (int nPlayerIndex = 0; nPlayerIndex < pLoadMapInfo->diplomacies.size(); ++nPlayerIndex) { pLoadMapInfo->diplomacies[nPlayerIndex] = 2; }
  }
  return true;
}


bool CMapInfo::IsValid(const SLoadMapInfo rLoadMapInfo)
{
  if ((rLoadMapInfo.terrain.patches.GetSizeX() <= 0) || (rLoadMapInfo.terrain.patches.GetSizeY() <= 0)) { return false; }
  if ((rLoadMapInfo.terrain.tiles.GetSizeX() <= 0) || (rLoadMapInfo.terrain.tiles.GetSizeY() <= 0)) { return false; }
  if ((rLoadMapInfo.terrain.altitudes.GetSizeX() <= 0) || (rLoadMapInfo.terrain.altitudes.GetSizeY() <= 0)) { return false; }
  if ((rLoadMapInfo.nSeason < 0) || (rLoadMapInfo.nSeason >= REAL_SEASONS_COUNT)) { return false; }
  if (rLoadMapInfo.szSeasonFolder.empty()) { return false; }
  if (rLoadMapInfo.diplomacies.empty()) { return false; }
  if (rLoadMapInfo.unitCreation.units.size() < (rLoadMapInfo.diplomacies.size() - 1)) { return false; }
  return true;
}


int CMapInfo::GetSelectedSeason(int nSeason, const std::string &rszSeasonFolder)
{
  std::string szSeasonFolder = rszSeasonFolder;
  NStr::ToLower(szSeasonFolder);
  int nSelectedSeason = nSeason;
  if (nSelectedSeason == SEASON_SUMMER) { if (szSeasonFolder != SEASON_FOLDERS[nSelectedSeason]) { nSelectedSeason = SEASON_SPRING; } }
  return nSelectedSeason;
}


bool CMapInfo::RemoveObject(SLoadMapInfo *pLoadMapInfo, int nObjectIndex)
{
  NI_ASSERT_TF(pLoadMapInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pLoadMapInfo ),
               return false);
  NI_ASSERT_TF(( nObjectIndex >= 0 ) && ( nObjectIndex < pLoadMapInfo->objects.size() ),
               NStr::Format( "Wrong object index: %d\n", nObjectIndex ),
               return false);

  // delete the object
  pLoadMapInfo->objects.erase(pLoadMapInfo->objects.begin() + nObjectIndex);
  return true;
}


bool CMapInfo::RemoveObjects(SLoadMapInfo *pLoadMapInfo, const std::list<CVec2> &rClearPolygon)
{
  NI_ASSERT_TF(pLoadMapInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pLoadMapInfo ),
               return false);

  // array for marking tiles included in the polygon
  CArray2D<BYTE> tileMap(pLoadMapInfo->terrain.tiles.GetSizeX() * 2, pLoadMapInfo->terrain.tiles.GetSizeY() * 2);
  tileMap.Set(RMGC_UNLOCKED);
  ModifyTilesFunctional<CArray2D<BYTE>, BYTE> tileMapModifyTiles(RMGC_LOCKED, &tileMap);
  CheckTilesFunctional<CArray2D<BYTE>, BYTE> tileMapCheckTiles(RMGC_LOCKED, &tileMap);
  CTRect<int> tileMapRect(0, 0, tileMap.GetSizeX(), tileMap.GetSizeY());

  // mark the tiles that fall into the pogon
  ApplyTilesInPolygon<ModifyTilesFunctional<CArray2D<BYTE>, BYTE>, std::list<CVec2>, CVec2>(tileMapRect, rClearPolygon, fWorldCellSize / 2.0f, tileMapModifyTiles);

  // We remove objects that, with their passability, fall on locked tiles
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size();)
  {
    tileMapCheckTiles.isPresent = false;
    if (!ApplyTilesInObjectsPassability(tileMapRect, pLoadMapInfo->objects[nObjectIndex], tileMapCheckTiles))
    {
      if (RemoveObject(pLoadMapInfo, nObjectIndex))
      {
        // the index already points to another object, no need to increase it
        continue;
      }
    }
    // index points to the same element, increment it
    ++nObjectIndex;
  }
  return true;
}


bool CMapInfo::UpdateTerrain(struct STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect, const struct STilesetDesc &rTilesetDesc, const struct SCrossetDesc &rCrossetDesc, /* const struct SRoadsetDesc &rRoadsetDesc, */ const struct SGFXLightDirectional &rSunlight)
{
  NI_ASSERT_TF(pTerrainInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
               return false);
  bool result = UpdateTerrainCrosses(pTerrainInfo, rUpdateRect, rTilesetDesc, rCrossetDesc/* , rRoadsetDesc */);
  if (result)
  {
    CTRect<int> riversUpdateRect(rUpdateRect.minx * STerrainPatchInfo::nSizeX,
                                 rUpdateRect.miny * STerrainPatchInfo::nSizeY,
                                 rUpdateRect.maxx * STerrainPatchInfo::nSizeX,
                                 rUpdateRect.maxy * STerrainPatchInfo::nSizeY);
    result = UpdateTerrainRivers(pTerrainInfo, riversUpdateRect);
    if (result)
    {
      CTRect<int> roads3DUpdateRect(rUpdateRect.minx * STerrainPatchInfo::nSizeX,
                                    rUpdateRect.miny * STerrainPatchInfo::nSizeY,
                                    rUpdateRect.maxx * STerrainPatchInfo::nSizeX,
                                    rUpdateRect.maxy * STerrainPatchInfo::nSizeY);
      result = UpdateTerrainRoads3D(pTerrainInfo, roads3DUpdateRect);
      if (result)
      {
        CTRect<int> shadeUpdateRect(rUpdateRect.minx * STerrainPatchInfo::nSizeX,
                                    rUpdateRect.miny * STerrainPatchInfo::nSizeY,
                                    (rUpdateRect.maxx * STerrainPatchInfo::nSizeX) + 1,
                                    (rUpdateRect.maxy * STerrainPatchInfo::nSizeY) + 1);
        result = UpdateTerrainShades(pTerrainInfo, shadeUpdateRect, rSunlight);
      }
    }
  }
  return result;
}


bool CMapInfo::UpdateTerrainCrosses(STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect, const STilesetDesc &rTilesetDesc, const SCrossetDesc &rCrossetDesc/* , const SRoadsetDesc &rRoadsetDesc */)
{
  NI_ASSERT_TF(pTerrainInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
               return false);

  int nTerrainSizeX = pTerrainInfo->patches.GetSizeX() * STerrainPatchInfo::nSizeX;
  int nTerrainSizeY = pTerrainInfo->patches.GetSizeY() * STerrainPatchInfo::nSizeY;
  CTRect<int> rcSuper(0, 0, nTerrainSizeX, nTerrainSizeY);

  CTerrainBuilder builder(rTilesetDesc, rCrossetDesc/* , rRoadsetDesc */);

  {
    CTRect<int> rcPatchesRect;
    CTRect<int> rcTemp;

    CTPoint<int> point = rUpdateRect.GetLeftTop();
    builder.GetPatchRect(point.x, point.y, &rcTemp);
    rcPatchesRect.left = rcTemp.left;
    rcPatchesRect.top = rcTemp.top;

    point = rUpdateRect.GetRightBottom();
    builder.GetPatchRect(point.x - 1, point.y - 1, &rcTemp);
    rcPatchesRect.right = rcTemp.right;
    rcPatchesRect.bottom = rcTemp.bottom;

    builder.PreprocessMapSegment(pTerrainInfo->tiles, rcPatchesRect);
  }

  for (int nPatchXIndex = rUpdateRect.minx; nPatchXIndex < rUpdateRect.maxx; ++nPatchXIndex)
  {
    for (int nPatchYIndex = rUpdateRect.miny; nPatchYIndex < rUpdateRect.maxy; ++nPatchYIndex)
    {
      CTerrainBuilder::SComplexCrosses newCrosses;
      CTRect<int> rcRect;
      builder.GetPatchRect(nPatchXIndex, nPatchYIndex, &rcRect);
      builder.MapSegmentGenerateCrosses(pTerrainInfo->tiles, rcRect, rcSuper, &newCrosses);
      {
        builder.CopyCrosses(&pTerrainInfo->patches[nPatchYIndex][nPatchXIndex], newCrosses);
      }
    }
  }
  return true;
}


bool CMapInfo::UpdateTerrainRivers(STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect)
{
  NI_ASSERT_TF(pTerrainInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
               return false);
  // rewriting river IDs
  for (int nRiverIndex = 0; nRiverIndex < pTerrainInfo->rivers.size(); ++nRiverIndex)
  {
    pTerrainInfo->rivers[nRiverIndex].nID = nRiverIndex;
    CVSOBuilder::UpdateZ(pTerrainInfo->altitudes, &(pTerrainInfo->rivers[nRiverIndex]));
  }
  return true;
}


bool CMapInfo::UpdateTerrainRoads3D(STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect)
{
  NI_ASSERT_TF(pTerrainInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
               return false);
  // rewriting road IDs
  for (int nRoad3DIndex = 0; nRoad3DIndex < pTerrainInfo->roads3.size(); ++nRoad3DIndex)
  {
    pTerrainInfo->roads3[nRoad3DIndex].nID = nRoad3DIndex;
    CVSOBuilder::UpdateZ(pTerrainInfo->altitudes, &(pTerrainInfo->roads3[nRoad3DIndex]));
  }
  return true;
}


bool CMapInfo::UpdateTerrainShades(STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect, const SGFXLightDirectional &rSunlight)
{
  NI_ASSERT_TF(pTerrainInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
               return false);

  return CVertexAltitudeInfo::UpdateShades(&(pTerrainInfo->altitudes), rUpdateRect, rSunlight);
}


int CMapInfo::UpdateObjects(SLoadMapInfo *pLoadMapInfo, const CTRect<int> &rUpdateRect)
{
  NI_ASSERT_TF(pLoadMapInfo != 0,
               NStr::Format( "Wrong parameter: %x\n", pLoadMapInfo ),
               return false);

  // UPDATE_LINK_ID
  // usedIDs[old nLinkID] = new nLinkID;
  std::unordered_map<int, int> usedIDs;

  int nCurrentLinkID = RMGC_INVALID_LINK_ID_VALUE + 1;

  // we are distributing new nLinkIDs
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size(); ++nObjectIndex)
  {
    if (pLoadMapInfo->objects[nObjectIndex].link.nLinkID != RMGC_INVALID_LINK_ID_VALUE) { usedIDs[pLoadMapInfo->objects[nObjectIndex].link.nLinkID] = nCurrentLinkID; }
    pLoadMapInfo->objects[nObjectIndex].link.nLinkID = nCurrentLinkID;
    ++nCurrentLinkID;
  }
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->scenarioObjects.size(); ++nObjectIndex)
  {
    if (pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkID != RMGC_INVALID_LINK_ID_VALUE) { usedIDs[pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkID] = nCurrentLinkID; }
    pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkID = nCurrentLinkID;
    ++nCurrentLinkID;
  }

  // Write new nLinkIDs to SMapObjectInfo.link.nLinkWith
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size(); ++nObjectIndex)
  {
    if ((pLoadMapInfo->objects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) &&
        (usedIDs.find(pLoadMapInfo->objects[nObjectIndex].link.nLinkWith) != usedIDs.end())) { pLoadMapInfo->objects[nObjectIndex].link.nLinkWith = usedIDs[pLoadMapInfo->objects[nObjectIndex].link.nLinkWith]; }
    else
    {
      pLoadMapInfo->objects[nObjectIndex].link.nLinkWith = RMGC_INVALID_LINK_ID_VALUE;
      pLoadMapInfo->objects[nObjectIndex].link.bIntention = true;
    }
  }
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->scenarioObjects.size(); ++nObjectIndex)
  {
    if ((pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) &&
        (usedIDs.find(pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith) != usedIDs.end())) { pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith = usedIDs[pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith]; }
    else
    {
      pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith = RMGC_INVALID_LINK_ID_VALUE;
      pLoadMapInfo->scenarioObjects[nObjectIndex].link.bIntention = true;
    }
  }

  // Write new nLinkIDs to entrenchments
  for (int nEntrenchmentIndex = 0; nEntrenchmentIndex < pLoadMapInfo->entrenchments.size(); ++nEntrenchmentIndex) { for (int nSectionIndex = 0; nSectionIndex < pLoadMapInfo->entrenchments[nEntrenchmentIndex].sections.size(); ++nSectionIndex) { for (int nTSegmentIndex = 0; nTSegmentIndex < pLoadMapInfo->entrenchments[nEntrenchmentIndex].sections[nSectionIndex].size(); ++nTSegmentIndex) { pLoadMapInfo->entrenchments[nEntrenchmentIndex].sections[nSectionIndex][nTSegmentIndex] = usedIDs[pLoadMapInfo->entrenchments[nEntrenchmentIndex].sections[nSectionIndex][nTSegmentIndex]]; } } }

  // Write new nLinkIDs to bridges
  for (int nBrigeIndex = 0; nBrigeIndex < pLoadMapInfo->bridges.size(); ++nBrigeIndex) { for (int nBrigeElementIndex = 0; nBrigeElementIndex < pLoadMapInfo->bridges[nBrigeIndex].size(); ++nBrigeElementIndex) { pLoadMapInfo->bridges[nBrigeIndex][nBrigeElementIndex] = usedIDs[pLoadMapInfo->bridges[nBrigeIndex][nBrigeElementIndex]]; } }

  // Write new linkID to startCommandsList
  for (auto it = pLoadMapInfo->startCommandsList.begin();
       it != pLoadMapInfo->startCommandsList.end();
       ++it)
  {
    for (int nLinkIDIndex = 0; nLinkIDIndex < it->unitLinkIDs.size(); ++nLinkIDIndex) { it->unitLinkIDs[nLinkIDIndex] = usedIDs[it->unitLinkIDs[nLinkIDIndex]]; }
    if (it->linkID != RMGC_INVALID_LINK_ID_VALUE) { it->linkID = usedIDs[it->linkID]; }
  }

  // Write new linkID to reservePositionsList
  for (auto it = pLoadMapInfo->reservePositionsList.begin();
       it != pLoadMapInfo->reservePositionsList.end();
       ++it)
  {
    if (it->nArtilleryLinkID != RMGC_INVALID_LINK_ID_VALUE) { it->nArtilleryLinkID = usedIDs[it->nArtilleryLinkID]; }
    if (it->nTruckLinkID != RMGC_INVALID_LINK_ID_VALUE) { it->nTruckLinkID = usedIDs[it->nTruckLinkID]; }
  }

  return nCurrentLinkID;
}


void CMapInfo::PackFrameIndices(struct SLoadMapInfo *pLoadMapInfo)
{
  CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
  NI_ASSERT_T(pODB != 0,
              NStr::Format( "CMapInfo::PackFrameIndices() GetSingleton<IObjectsDB>() == 0" ));

  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size(); ++nObjectIndex) { PackFrameIndex(pODB, &(pLoadMapInfo->objects[nObjectIndex])); }

  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->scenarioObjects.size(); ++nObjectIndex) { PackFrameIndex(pODB, &(pLoadMapInfo->scenarioObjects[nObjectIndex])); }
}


void CMapInfo::UnpackFrameIndices(struct SLoadMapInfo *pLoadMapInfo)
{
  CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
  NI_ASSERT_T(pODB != 0,
              NStr::Format( "CMapInfo::UnpackFrameIndices() GetSingleton<IObjectsDB>() == 0" ));

  nCurRandomSeed = RANDOM_SEED;
  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size(); ++nObjectIndex)
  {
    UnpackFrameIndex(pODB, &(pLoadMapInfo->objects[nObjectIndex]), &nCurRandomSeed);
    nCurRandomSeed += RANDOM_SEED;
  }

  for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->scenarioObjects.size(); ++nObjectIndex)
  {
    UnpackFrameIndex(pODB, &(pLoadMapInfo->scenarioObjects[nObjectIndex]), &nCurRandomSeed);
    nCurRandomSeed += RANDOM_SEED;
  }
}


class CRemoveNonExistingVSOFunctor
{
  IDataStorage *pDataStorage;
  std::string *pszOutputString;
  //
  std::set<std::string> existingTextures;
  std::set<std::string> existingDescriptions;
  //
  std::set<std::string> notExistingTextures;
  std::set<std::string> notExistingDescriptions;
  //
  bool CheckTexture(const std::string &rszTexture)
  {
    if (existingTextures.find(rszTexture) != existingTextures.end()) { return true; }
    if (notExistingTextures.find(rszTexture) != notExistingTextures.end()) { return false; }
    CPtr<IDataStream> pDDSStream = pDataStorage->OpenStream((rszTexture + GetDDSImageExtention(COMPRESSION_HIGH_QUALITY)).c_str(), STREAM_ACCESS_READ);
    if (pDDSStream)
    {
      existingTextures.insert(rszTexture);
      return true;
    }
    notExistingTextures.insert(rszTexture);
    return false;
  }

  //
  bool CheckDescription(const std::string &rszDescription)
  {
    if (existingDescriptions.find(rszDescription) != existingDescriptions.end()) { return true; }
    if (notExistingDescriptions.find(rszDescription) != notExistingDescriptions.end()) { return false; }
    CPtr<IDataStream> pXMLStream = pDataStorage->OpenStream((rszDescription + ".xml").c_str(), STREAM_ACCESS_READ);
    if (pXMLStream)
    {
      existingDescriptions.insert(rszDescription);
      return true;
    }
    notExistingDescriptions.insert(rszDescription);
    return false;
  }

public:
  std::list<SVectorStripeObject> removedVSOs;
  //
  CRemoveNonExistingVSOFunctor(IDataStorage *_pDataStorage, std::string *_pszOutputString) : pDataStorage(_pDataStorage), pszOutputString(_pszOutputString) {}
  //
  bool operator()(const SVectorStripeObject &rVectorStripeObject)
  {
    bool bExists = true;
    bExists = CheckTexture(rVectorStripeObject.bottom.szTexture);
    if (bExists)
    {
      for (int nLayerIndex = 0; nLayerIndex < rVectorStripeObject.bottomBorders.size(); ++nLayerIndex)
      {
        const SVectorStripeObjectDesc::SLayer &rLayer = rVectorStripeObject.bottomBorders[nLayerIndex];
        bExists = CheckTexture(rLayer.szTexture);
        if (!bExists) { break; }
      }
    }
    if (bExists)
    {
      for (int nLayerIndex = 0; nLayerIndex < rVectorStripeObject.layers.size(); ++nLayerIndex)
      {
        const SVectorStripeObjectDesc::SLayer &rLayer = rVectorStripeObject.layers[nLayerIndex];
        bExists = CheckTexture(rLayer.szTexture);
        if (!bExists) { break; }
      }
    }

    if (!bExists) { bExists = CheckDescription(rVectorStripeObject.szDescName); }
    if (!bExists)
    {
      removedVSOs.push_back(rVectorStripeObject);
      if (!rVectorStripeObject.points.empty())
      {
        if (pszOutputString)
        {
          (*pszOutputString) += NStr::Format("VSO: %s, texture: %s, start point: [%.2f, %.2f]\r\n",
                                             rVectorStripeObject.szDescName.c_str(),
                                             rVectorStripeObject.bottom.szTexture.c_str(),
                                             rVectorStripeObject.points[0].vPos.x / fWorldCellSize,
                                             rVectorStripeObject.points[0].vPos.y / fWorldCellSize);
        }
      }
    }
    return !bExists;
  }
};


class CRemoveNonExistingVSOFunctorProxy
{
  CRemoveNonExistingVSOFunctor *pFunctor;

public:
  CRemoveNonExistingVSOFunctorProxy(CRemoveNonExistingVSOFunctor *_pFunctor)
    : pFunctor(_pFunctor) {}

  bool operator()(const SVectorStripeObject &rVectorStripeObject) { return (*pFunctor)(rVectorStripeObject); }
};


class CRemoveNonExistingObjectsFunctor
{
  IDataStorage *pDataStorage;
  IObjectsDB *pObjectsDB;
  std::string *pszOutputString;

  std::set<std::string> existingObjects;
  std::set<std::string> existingObjectFolders;
  //
  std::set<std::string> notExistingObjects;
  std::set<std::string> notExistingObjectFolders;
  //
  bool CheckObjectFolder(const std::string &rszObjectFolder)
  {
    if (existingObjectFolders.find(rszObjectFolder) != existingObjectFolders.end()) { return true; }
    if (notExistingObjectFolders.find(rszObjectFolder) != notExistingObjectFolders.end()) { return false; }
    CPtr<IDataStream> pXMLStream = pDataStorage->OpenStream((rszObjectFolder + "\\1.xml").c_str(), STREAM_ACCESS_READ);
    if (pXMLStream)
    {
      existingObjectFolders.insert(rszObjectFolder);
      return true;
    }
    notExistingObjectFolders.insert(rszObjectFolder);
    return false;
  }

  //
  bool CheckObject(const std::string &rszObject)
  {
    if (existingObjects.find(rszObject) != existingObjects.end()) { return true; }
    if (existingObjectFolders.find(rszObject) != existingObjectFolders.end()) { return false; }
    const SGDBObjectDesc *pGDBObjectDesc = pObjectsDB->GetDesc(rszObject.c_str());
    if (pGDBObjectDesc)
    {
      if (CheckObjectFolder(pGDBObjectDesc->szPath))
      {
        existingObjects.insert(rszObject);
        return true;
      }
    }
    existingObjectFolders.insert(rszObject);
    return false;
  }

public:
  std::list<SMapObjectInfo> removedObjects;
  std::set<int> removedLinkIDs;
  //
  CRemoveNonExistingObjectsFunctor(IDataStorage *_pDataStorage, IObjectsDB *_pObjectsDB, std::string *_pszOutputString) : pDataStorage(_pDataStorage), pObjectsDB(_pObjectsDB), pszOutputString(_pszOutputString) {}

  bool operator()(const SMapObjectInfo &rMapObjectInfo)
  {
    bool bExists = true;
    bExists = CheckObject(rMapObjectInfo.szName);
    if (!bExists)
    {
      removedObjects.push_back(rMapObjectInfo);
      removedLinkIDs.insert(rMapObjectInfo.link.nLinkID);
      if (pszOutputString)
      {
        (*pszOutputString) += NStr::Format("Object: %s, pos: [%.2f, %.2f], player: %d, scriptID: %d\r\n",
                                           rMapObjectInfo.szName.c_str(),
                                           rMapObjectInfo.vPos.x / (SAIConsts::TILE_SIZE * 2.0f),
                                           rMapObjectInfo.vPos.y / (SAIConsts::TILE_SIZE * 2.0f),
                                           rMapObjectInfo.nPlayer,
                                           rMapObjectInfo.nScriptID);
      }
    }
    return !bExists;
  }
};


class CRemoveNonExistingObjectsFunctorProxy
{
  CRemoveNonExistingObjectsFunctor *pFunctor;

public:
  CRemoveNonExistingObjectsFunctorProxy(CRemoveNonExistingObjectsFunctor *_pFunctor)
    : pFunctor(_pFunctor) {}

  bool operator()(const SMapObjectInfo &rMapObjectInfo) { return (*pFunctor)(rMapObjectInfo); }
};


class CRemoveNonExistingEntrenchmentsFunctor
{
  std::set<int> *pRemovedLinkIDs;

public:
  CRemoveNonExistingEntrenchmentsFunctor(std::set<int> *_pRemovedLinkIDs) : pRemovedLinkIDs(_pRemovedLinkIDs) {}
  //
  bool operator()(const SEntrenchmentInfo &rEntrenchmentInfo)
  {
    for (int nSectionIndex = 0; nSectionIndex < rEntrenchmentInfo.sections.size(); ++nSectionIndex) { for (int nTSegmentIndex = 0; nTSegmentIndex < rEntrenchmentInfo.sections[nSectionIndex].size(); ++nTSegmentIndex) { if (pRemovedLinkIDs->find(rEntrenchmentInfo.sections[nSectionIndex][nTSegmentIndex]) != pRemovedLinkIDs->end()) { return true; } } }
    return false;
  }
};


class CRemoveNonExistingBridgesFunctor
{
  std::set<int> *pRemovedLinkIDs;

public:
  CRemoveNonExistingBridgesFunctor(std::set<int> *_pRemovedLinkIDs) : pRemovedLinkIDs(_pRemovedLinkIDs) {}
  //
  bool operator()(const std::vector<int> &rBridgeInfo)
  {
    for (int nBrigeElementIndex = 0; nBrigeElementIndex < rBridgeInfo.size(); ++nBrigeElementIndex) { if (pRemovedLinkIDs->find(rBridgeInfo[nBrigeElementIndex]) != pRemovedLinkIDs->end()) { return true; } }
    return false;
  }
};


class CRemoveNonExistingStartCommandFunctor
{
  std::set<int> *pRemovedLinkIDs;

public:
  CRemoveNonExistingStartCommandFunctor(std::set<int> *_pRemovedLinkIDs) : pRemovedLinkIDs(_pRemovedLinkIDs) {}
  //
  bool operator()(const SAIStartCommand &rAIStartCommand)
  {
    for (int nLinkIDIndex = 0; nLinkIDIndex < rAIStartCommand.unitLinkIDs.size(); ++nLinkIDIndex) { if (pRemovedLinkIDs->find(rAIStartCommand.unitLinkIDs[nLinkIDIndex]) != pRemovedLinkIDs->end()) { return true; } }
    if (rAIStartCommand.linkID != RMGC_INVALID_LINK_ID_VALUE) { if (pRemovedLinkIDs->find(rAIStartCommand.linkID) != pRemovedLinkIDs->end()) { return true; } }
    return false;
  }
};


class CRemoveNonExistingReservePositionFunctor
{
  std::set<int> *pRemovedLinkIDs;

public:
  CRemoveNonExistingReservePositionFunctor(std::set<int> *_pRemovedLinkIDs) : pRemovedLinkIDs(_pRemovedLinkIDs) {}
  //
  bool operator()(const SBattlePosition &rBattlePosition)
  {
    if (rBattlePosition.nArtilleryLinkID != RMGC_INVALID_LINK_ID_VALUE) { if (pRemovedLinkIDs->find(rBattlePosition.nArtilleryLinkID) != pRemovedLinkIDs->end()) { return true; } }
    if (rBattlePosition.nTruckLinkID != RMGC_INVALID_LINK_ID_VALUE) { if (pRemovedLinkIDs->find(rBattlePosition.nTruckLinkID) != pRemovedLinkIDs->end()) { return true; } }
    return false;
  }
};


bool CMapInfo::RemoveNonExistingObjects(struct SLoadMapInfo *pLoadMapInfo, IDataStorage *pDataStorage, IObjectsDB *pObjectsDB, std::string *pszOutputString)
{
  NI_ASSERT_TF(( pDataStorage != nullptr ) && ( pObjectsDB != nullptr ),
               NStr::Format( "Wrong parameters: pDataStorage %x, pObjectsDB %x\n", pDataStorage, pObjectsDB ),
               return false);

  bool bSomeRemoved = false;
  {
    CRemoveNonExistingVSOFunctor vsoFunctor(pDataStorage, pszOutputString);

    if (pszOutputString) { (*pszOutputString) += std::string("Roads deleted:\r\n"); }
    // roads
    {
      auto vsoIterator = std::remove_if(pLoadMapInfo->terrain.roads3.begin(), pLoadMapInfo->terrain.roads3.end(), CRemoveNonExistingVSOFunctorProxy(&vsoFunctor));
      if (vsoIterator != pLoadMapInfo->terrain.roads3.end())
      {
        pLoadMapInfo->terrain.roads3.erase(vsoIterator, pLoadMapInfo->terrain.roads3.end());
        bSomeRemoved = true;
      }
    }
    if (pszOutputString)
    {
      (*pszOutputString) += std::string("\r\n");
      (*pszOutputString) += std::string("Rivers deleted:\r\n");
    }
    // rivers
    {
      auto vsoIterator = std::remove_if(pLoadMapInfo->terrain.rivers.begin(), pLoadMapInfo->terrain.rivers.end(), CRemoveNonExistingVSOFunctorProxy(&vsoFunctor));
      if (vsoIterator != pLoadMapInfo->terrain.rivers.end())
      {
        pLoadMapInfo->terrain.rivers.erase(vsoIterator, pLoadMapInfo->terrain.rivers.end());
        bSomeRemoved = true;
      }
    }
    if (pszOutputString) { (*pszOutputString) += std::string("\r\n"); }
  }

  // objects & scenarioObjects
  {
    if (pszOutputString) { (*pszOutputString) += std::string("Objects deleted:\r\n"); }
    CRemoveNonExistingObjectsFunctor objectsFunctor(pDataStorage, pObjectsDB, pszOutputString);
    {
      auto objectIterator = std::remove_if(pLoadMapInfo->objects.begin(), pLoadMapInfo->objects.end(), CRemoveNonExistingObjectsFunctorProxy(&objectsFunctor));
      if (objectIterator != pLoadMapInfo->objects.end())
      {
        pLoadMapInfo->objects.erase(objectIterator, pLoadMapInfo->objects.end());
        bSomeRemoved = true;
      }
    }
    {
      auto objectIterator = std::remove_if(pLoadMapInfo->scenarioObjects.begin(), pLoadMapInfo->scenarioObjects.end(), CRemoveNonExistingObjectsFunctorProxy(&objectsFunctor));
      if (objectIterator != pLoadMapInfo->scenarioObjects.end())
      {
        pLoadMapInfo->scenarioObjects.erase(objectIterator, pLoadMapInfo->scenarioObjects.end());
        bSomeRemoved = true;
      }
    }

    if (!objectsFunctor.removedLinkIDs.empty())
    {
      bSomeRemoved = true;
      // remove nLinkID from SMapObjectInfo.link.nLinkWith
      for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->objects.size(); ++nObjectIndex)
      {
        if ((pLoadMapInfo->objects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) &&
            (objectsFunctor.removedLinkIDs.find(pLoadMapInfo->objects[nObjectIndex].link.nLinkWith) != objectsFunctor.removedLinkIDs.end())) { pLoadMapInfo->objects[nObjectIndex].link.nLinkWith = RMGC_INVALID_LINK_ID_VALUE; }
      }
      for (int nObjectIndex = 0; nObjectIndex < pLoadMapInfo->scenarioObjects.size(); ++nObjectIndex)
      {
        if ((pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) &&
            (objectsFunctor.removedLinkIDs.find(pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith) != objectsFunctor.removedLinkIDs.end())) { pLoadMapInfo->scenarioObjects[nObjectIndex].link.nLinkWith = RMGC_INVALID_LINK_ID_VALUE; }
      }

      // remove entrenchments
      {
        CRemoveNonExistingEntrenchmentsFunctor entrenchmentsFunctor(&(objectsFunctor.removedLinkIDs));
        auto entrenchmentIterator = std::remove_if(pLoadMapInfo->entrenchments.begin(), pLoadMapInfo->entrenchments.end(), entrenchmentsFunctor);
        if (entrenchmentIterator != pLoadMapInfo->entrenchments.end()) { pLoadMapInfo->entrenchments.erase(entrenchmentIterator, pLoadMapInfo->entrenchments.end()); }
      }

      // remove bridges
      {
        CRemoveNonExistingBridgesFunctor bridgeFunctor(&(objectsFunctor.removedLinkIDs));
        auto bridgeIterator = std::remove_if(pLoadMapInfo->bridges.begin(), pLoadMapInfo->bridges.end(), bridgeFunctor);
        if (bridgeIterator != pLoadMapInfo->bridges.end()) { pLoadMapInfo->bridges.erase(bridgeIterator, pLoadMapInfo->bridges.end()); }
      }

      // remove startCommandsList
      {
        CRemoveNonExistingStartCommandFunctor startCommandFunctor(&(objectsFunctor.removedLinkIDs));
        auto startCommandIterator = std::remove_if(pLoadMapInfo->startCommandsList.begin(), pLoadMapInfo->startCommandsList.end(), startCommandFunctor);
        if (startCommandIterator != pLoadMapInfo->startCommandsList.end()) { pLoadMapInfo->startCommandsList.erase(startCommandIterator, pLoadMapInfo->startCommandsList.end()); }
      }

      // remove reservePositionsList
      {
        CRemoveNonExistingReservePositionFunctor reservePositionFunctor(&(objectsFunctor.removedLinkIDs));
        auto reservePositionIterator = std::remove_if(pLoadMapInfo->reservePositionsList.begin(), pLoadMapInfo->reservePositionsList.end(), reservePositionFunctor);
        if (reservePositionIterator != pLoadMapInfo->reservePositionsList.end()) { pLoadMapInfo->reservePositionsList.erase(reservePositionIterator, pLoadMapInfo->reservePositionsList.end()); }
      }
    }
  }
  return bSomeRemoved;
}


bool CMapInfo::TerrainHitTest(const STerrainInfo &rTerrainInfo, const CVec3 &rPoint, TERRAIN_HIT_TEST_TYPE type, std::vector<int> *pTerrainObjects)
{
  NI_ASSERT_TF(pTerrainObjects != 0,
               NStr::Format( "Wrong parameter: %x\n", pTerrainObjects ),
               return false);

  pTerrainObjects->clear();
  switch (type)
  {
    case THT_ROADS3D:
    {
      for (int nRoad3DIndex = 0; nRoad3DIndex < rTerrainInfo.roads3.size(); ++nRoad3DIndex)
      {
        std::vector<CVec3> boundingPolygon;
        GetBoundingPolygon(rTerrainInfo.roads3[nRoad3DIndex], &boundingPolygon);
        if (ClassifyPolygon(boundingPolygon, rPoint) != CP_OUTSIDE) { pTerrainObjects->push_back(nRoad3DIndex); }
      }
      break;
    }
    case THT_RIVERS:
    {
      for (int nRiverIndex = 0; nRiverIndex < rTerrainInfo.rivers.size(); ++nRiverIndex)
      {
        std::vector<CVec3> boundingPolygon;
        GetBoundingPolygon(rTerrainInfo.rivers[nRiverIndex], &boundingPolygon);
        if (ClassifyPolygon(boundingPolygon, rPoint) != CP_OUTSIDE) { pTerrainObjects->push_back(nRiverIndex); }
      }
      break;
    }
    default: { break; }
  }
  return true;
}

const SVectorStripeObject *CMapInfo::GetRiver(const STerrainInfo &rTerrainInfo, int nID)
{
  for (int nRiverIndex = 0; nRiverIndex < rTerrainInfo.rivers.size(); ++nRiverIndex) { if (rTerrainInfo.rivers[nRiverIndex].nID == nID) { return &(rTerrainInfo.rivers[nRiverIndex]); } }
  return nullptr;
}

const SVectorStripeObject *CMapInfo::GetRoad3D(const STerrainInfo &rTerrainInfo, int nID)
{
  for (int nRoad3DIndex = 0; nRoad3DIndex < rTerrainInfo.roads3.size(); ++nRoad3DIndex) { if (rTerrainInfo.roads3[nRoad3DIndex].nID == nID) { return &(rTerrainInfo.roads3[nRoad3DIndex]); } }
  return nullptr;
}


bool CMapInfo::GetScenarioObjects(const std::string &rszMapInfoFileName, std::vector<SMapObjectInfo> *pMapObjects)
{
  CPtr<IDataStorage> pDataStorage = GetSingleton<IDataStorage>();
  if (!pDataStorage) { return false; }
  NI_ASSERT_TF(pMapObjects != 0,
               NStr::Format( "GetScenarioObjects, Wrong parameter pMapObjects: %x\n", pMapObjects ),
               return false);

  const std::string szMapInfoFileName = rszMapInfoFileName.substr(0, rszMapInfoFileName.rfind('.'));
  // load map info
  CMapInfo mapinfo;
  SStorageElementStats statsXML, statsBZM;
  {
    // get stats from XML and BZM files
    Zero(statsXML);
    const bool bHasXML = pDataStorage->GetStreamStats((szMapInfoFileName + ".xml").c_str(), &statsXML);
    Zero(statsBZM);
    const bool bHasBZM = pDataStorage->GetStreamStats((szMapInfoFileName + ".bzm").c_str(), &statsBZM);
    NI_ASSERT_TF(bHasXML || bHasBZM, NStr::Format("Can't load neither XML nor BZM map \"%s\" to get scenario objects - check mission stats resources", szMapInfoFileName.c_str()), return false);
  }
  // load newest one
  if (statsXML.mtime > statsBZM.mtime) { LoadDataResource(szMapInfoFileName, ".bzm", false, 1, "ScenarioObjects", (*pMapObjects)); }
  else
  {
    CMapInfo mapInfo;
    if (!LoadTypedSuperDataResource(szMapInfoFileName, ".bzm", true, 1, mapInfo)) { return false; }
    (*pMapObjects) = mapInfo.scenarioObjects;
  }
  return true;
}


bool CMapInfo::GetUsedLinkIDs(const SLoadMapInfo &rLoadMapInfo, CUsedLinkIDs *pUsedLinkIDs)
{
  NI_ASSERT_TF(pUsedLinkIDs != 0,
               NStr::Format( "GetUsedLinkIDs, Wrong parameter pUsedLinkIDs: %x\n", pUsedLinkIDs ),
               return false);

  for (int nObjectIndex = 0; nObjectIndex < rLoadMapInfo.objects.size(); ++nObjectIndex) { if (rLoadMapInfo.objects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(rLoadMapInfo.objects[nObjectIndex].link.nLinkWith); } }
  for (int nObjectIndex = 0; nObjectIndex < rLoadMapInfo.scenarioObjects.size(); ++nObjectIndex) { if (rLoadMapInfo.scenarioObjects[nObjectIndex].link.nLinkWith != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(rLoadMapInfo.scenarioObjects[nObjectIndex].link.nLinkWith); } }

  for (int nEntrenchmentIndex = 0; nEntrenchmentIndex < rLoadMapInfo.entrenchments.size(); ++nEntrenchmentIndex) { for (int nSectionIndex = 0; nSectionIndex < rLoadMapInfo.entrenchments[nEntrenchmentIndex].sections.size(); ++nSectionIndex) { for (int nTSegmentIndex = 0; nTSegmentIndex < rLoadMapInfo.entrenchments[nEntrenchmentIndex].sections[nSectionIndex].size(); ++nTSegmentIndex) { if (rLoadMapInfo.entrenchments[nEntrenchmentIndex].sections[nSectionIndex][nTSegmentIndex] != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(rLoadMapInfo.entrenchments[nEntrenchmentIndex].sections[nSectionIndex][nTSegmentIndex]); } } } }

  for (int nBrigeIndex = 0; nBrigeIndex < rLoadMapInfo.bridges.size(); ++nBrigeIndex) { for (int nBrigeElementIndex = 0; nBrigeElementIndex < rLoadMapInfo.bridges[nBrigeIndex].size(); ++nBrigeElementIndex) { if (rLoadMapInfo.bridges[nBrigeIndex][nBrigeElementIndex] != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(rLoadMapInfo.bridges[nBrigeIndex][nBrigeElementIndex]); } } }

  for (auto it = rLoadMapInfo.startCommandsList.begin();
       it != rLoadMapInfo.startCommandsList.end();
       ++it)
  {
    for (int nLinkIDIndex = 0; nLinkIDIndex < it->unitLinkIDs.size(); ++nLinkIDIndex) { if (it->unitLinkIDs[nLinkIDIndex] != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(it->unitLinkIDs[nLinkIDIndex]); } }
    if (it->linkID != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(it->linkID); }
  }

  for (auto it = rLoadMapInfo.reservePositionsList.begin();
       it != rLoadMapInfo.reservePositionsList.end();
       ++it)
  {
    if (it->nArtilleryLinkID != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(it->nArtilleryLinkID); }
    if (it->nTruckLinkID != RMGC_INVALID_LINK_ID_VALUE) { pUsedLinkIDs->insert(it->nTruckLinkID); }
  }

  return true;
}


bool CMapInfo::GetUsedScriptIDs(const SLoadMapInfo &rLoadMapInfo, CUsedScriptIDs *pUsedScriptIDs)
{
  NI_ASSERT_TF(pUsedScriptIDs != 0,
               NStr::Format( "GetUsedScriptIDs, Wrong parameter pUsedScriptIDs: %x\n", pUsedScriptIDs ),
               return false);

  for (int nObjectIndex = 0; nObjectIndex < rLoadMapInfo.objects.size(); ++nObjectIndex) { if (rLoadMapInfo.objects[nObjectIndex].nScriptID != RMGC_INVALID_SCRIPT_ID_VALUE) { pUsedScriptIDs->insert(rLoadMapInfo.objects[nObjectIndex].nScriptID); } }
  for (int nObjectIndex = 0; nObjectIndex < rLoadMapInfo.scenarioObjects.size(); ++nObjectIndex) { if (rLoadMapInfo.scenarioObjects[nObjectIndex].nScriptID != RMGC_INVALID_SCRIPT_ID_VALUE) { pUsedScriptIDs->insert(rLoadMapInfo.scenarioObjects[nObjectIndex].nScriptID); } }
  return true;
}


bool CMapInfo::GetUsedScriptAreas(const SLoadMapInfo &rLoadMapInfo, CUsedScriptAreas *pUsedScriptAreas)
{
  NI_ASSERT_TF(pUsedScriptAreas != 0,
               NStr::Format( "GetUsedScriptAreas, Wrong parameter pUsedScripAreas: %x\n", pUsedScriptAreas ),
               return false);

  for (int nScriptAreaIndex = 0; nScriptAreaIndex < rLoadMapInfo.scriptAreas.size(); ++nScriptAreaIndex) { pUsedScriptAreas->insert(rLoadMapInfo.scriptAreas[nScriptAreaIndex].szName); }
  return true;
}


// obsolete storage


/* *
bool CMapInfo::UpdateTerrainRoads( STerrainInfo *pTerrainInfo, const CTRect<int> &rUpdateRect, const SRoadsetDesc &rRoadsetDesc )
{
	NI_ASSERT_TF( pTerrainInfo != 0,
							  NStr::Format( "Wrong parameter: %x\n", pTerrainInfo ),
							  return false );
	// сотрем все дороги по всем патчам
	for ( int nPatchXIndex = 0; nPatchXIndex < pTerrainInfo->patches.GetSizeX(); ++nPatchXIndex )
	{
		for ( int nPatchYIndex = 0; nPatchYIndex < pTerrainInfo->patches.GetSizeY() ; ++nPatchYIndex )
		{
			for ( int index = 0; index < nNumRoadTypes; ++index )
			{
				pTerrainInfo->patches[nPatchYIndex][nPatchXIndex].roads[index].clear();
			}
		}
	}

	if ( pTerrainInfo->roads.size() < 1 ) return true;
	//установим правильную ориетацию дорог
	for ( int index = 0; index < pTerrainInfo->roads.size(); ++index )
	{	
		pTerrainInfo->roads[index].rect.Normalize();
	}
	
	//склеивание дорог
	bool isChanged = true;
	while( isChanged )
	{
		isChanged = false;
		for ( int index = 0; index < ( pTerrainInfo->roads.size() - 1 ); ++index )
		{	
			for ( int innerIndex = ( index + 1 ); innerIndex < pTerrainInfo->roads.size(); )
			{	
				if ( //направления совпадают
						 ( pTerrainInfo->roads[index].nDir == pTerrainInfo->roads[innerIndex].nDir ) &&
						 //типы совпадают
						 ( pTerrainInfo->roads[index].nType == pTerrainInfo->roads[innerIndex].nType ) &&
						 //пересекаются
						 pTerrainInfo->roads[index].rect.IsIntersectEdges( pTerrainInfo->roads[innerIndex].rect ) &&
					   //одна дорога налазит краем на другую и при этом не имеет сдвига относительно этого края
						 ( ( ( pTerrainInfo->roads[index].rect.Width() == pTerrainInfo->roads[innerIndex].rect.Width() ) &&
						 		 ( pTerrainInfo->roads[index].rect.minx == pTerrainInfo->roads[innerIndex].rect.minx ) ) ||
							 ( ( pTerrainInfo->roads[index].rect.Height() == pTerrainInfo->roads[innerIndex].rect.Height() ) &&
								 ( pTerrainInfo->roads[index].rect.miny == pTerrainInfo->roads[innerIndex].rect.miny ) ) ) )
				
				{
					pTerrainInfo->roads[index].rect.Union( pTerrainInfo->roads[innerIndex].rect );
					pTerrainInfo->roads.erase( pTerrainInfo->roads.begin() + innerIndex );
					//std::vector<SRoadItem>::iterator pos = pTerrainInfo->roads.begin();
					//std::advance( pos, innerIndex );
					//pTerrainInfo->roads.erase( pos );
					isChanged = true;
				}
				else
				{
					++innerIndex;
				}
			}
		}
	}

	//создание битовой маски дорог
	CArray2D<DWORD> roadBitsArray[nNumRoadTypes];
	for ( int index = 0; index < nNumRoadTypes; ++index )
	{
		roadBitsArray[index].SetSizes( pTerrainInfo->tiles.GetSizeX(), pTerrainInfo->tiles.GetSizeY() );
		roadBitsArray[index].SetZero();
	}
	for ( int index = 0; index < pTerrainInfo->roads.size(); ++index )
	{
		if ( pTerrainInfo->roads[index].nDir == SRoadItem::HORIZONTAL )
		{
			if ( pTerrainInfo->roads[index].rect.Width() < 1 )
			{
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[7];
				for ( int nYIndex = ( pTerrainInfo->roads[index].rect.top + 1 ); nYIndex < pTerrainInfo->roads[index].rect.bottom; ++nYIndex )
				{
					roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[8];	
				}
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[3];
			}
			else
			{
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[0];
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[6];
				for ( int nYIndex = ( pTerrainInfo->roads[index].rect.top + 1 ); nYIndex < pTerrainInfo->roads[index].rect.bottom; ++nYIndex )
				{
					roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[1];	
					roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[5];	
				}
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[2];
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[4];
				if ( pTerrainInfo->roads[index].rect.Width() > 1 )
				{
					for ( int nXIndex = ( pTerrainInfo->roads[index].rect.left + 1 ); nXIndex < pTerrainInfo->roads[index].rect.right; ++nXIndex )
					{
						roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][nXIndex] |= ROAD_BITS[7];
						for ( int nYIndex = ( pTerrainInfo->roads[index].rect.top + 1 ); nYIndex < pTerrainInfo->roads[index].rect.bottom; ++nYIndex )
						{
							roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][nXIndex] |= ROAD_BITS[8];	
						}
						roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][nXIndex] |= ROAD_BITS[3];
					}
				}
			}
		}
		else if ( pTerrainInfo->roads[index].nDir == SRoadItem::VERTICAL )
		{
			if ( pTerrainInfo->roads[index].rect.Height() < 1 )
			{
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[10];
				for ( int nXIndex = ( pTerrainInfo->roads[index].rect.left + 1 ); nXIndex < pTerrainInfo->roads[index].rect.right; ++nXIndex )
				{
					roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][nXIndex] |= ROAD_BITS[17];	
				}
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[14];
			}
			else
			{
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[9];
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[11];
				for ( int nXIndex = ( pTerrainInfo->roads[index].rect.left + 1 ); nXIndex < pTerrainInfo->roads[index].rect.right; ++nXIndex )
				{
					roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][nXIndex] |= ROAD_BITS[16];
					roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][nXIndex] |= ROAD_BITS[12];
				}
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.top][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[15];
				roadBitsArray[pTerrainInfo->roads[index].nType][pTerrainInfo->roads[index].rect.bottom][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[13];
				if ( pTerrainInfo->roads[index].rect.Height() > 1 )
				{
					for ( int nYIndex = ( pTerrainInfo->roads[index].rect.top + 1 ); nYIndex < pTerrainInfo->roads[index].rect.bottom; ++nYIndex )
					{
						roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][pTerrainInfo->roads[index].rect.left] |= ROAD_BITS[10];
						for ( int nXIndex = ( pTerrainInfo->roads[index].rect.left + 1 ); nXIndex < pTerrainInfo->roads[index].rect.right; ++nXIndex )
						{
							roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][nXIndex] |= ROAD_BITS[17];	
						}
						roadBitsArray[pTerrainInfo->roads[index].nType][nYIndex][pTerrainInfo->roads[index].rect.right] |= ROAD_BITS[14];
					}
				}
			}
		}
	}
	//обновление информации по каждому патчу исходя из его положения
	//пока только одним типом дорог, рисуем - третьим
	for ( int nRoadIndex = 0; nRoadIndex < nNumRoadTypes; ++nRoadIndex )
	{
		for ( int nPatchXIndex = 0; nPatchXIndex < pTerrainInfo->patches.GetSizeX(); ++nPatchXIndex )
		{
			for ( int nPatchYIndex = 0; nPatchYIndex < pTerrainInfo->patches.GetSizeY() ; ++nPatchYIndex )
			{
				for ( int nXIndex = nPatchXIndex * STerrainPatchInfo::nSizeX; nXIndex < ( nPatchXIndex + 1 ) * STerrainPatchInfo::nSizeX; ++nXIndex )
				{
					for ( int nYIndex = nPatchYIndex * STerrainPatchInfo::nSizeY; nYIndex < ( nPatchYIndex + 1 ) * STerrainPatchInfo::nSizeY; ++nYIndex )
					{
						if ( roadBitsArray[nRoadIndex][nYIndex][nXIndex] > 0 )
						{
							int nValidBitMask = -1;
							for ( int nBitMaskIndex = 0; nBitMaskIndex < ROAD_CROSS_BITS_DIMENSION; ++nBitMaskIndex )
							{
								for ( int nBitMaskCaseIndex = 0; nBitMaskCaseIndex < ROAD_CROSS_BITS_CASES; ++nBitMaskCaseIndex )
								{
									if ( ROAD_CROSS_BITS[nBitMaskIndex][nBitMaskCaseIndex] == ROAD_BITS_NULL_VALUE ) break;
									if ( ( roadBitsArray[nRoadIndex][nYIndex][nXIndex] & ROAD_CROSS_BITS[nBitMaskIndex][nBitMaskCaseIndex] ) == ROAD_CROSS_BITS[nBitMaskIndex][nBitMaskCaseIndex] )
									{
										nValidBitMask = nBitMaskIndex;
										break;
									}
								}
								if ( nValidBitMask >= 0 )
								{
									break;
								}
							}
							if ( nValidBitMask >= 0 )
							{
								//Если это тайл завершения дороги на крае карты, то вставляем серединный тайл:
								int index = -1;
								if ( ( nXIndex == 0 ) || 
										 ( nXIndex == ( pTerrainInfo->tiles.GetSizeX() - 1 ) ) ||
										 ( nYIndex == 0 ) || 
										 ( nYIndex == ( pTerrainInfo->tiles.GetSizeY() - 1 ) ) ) 
								{
									index = rRoadsetDesc.roads[nRoadIndex].tiles[ ROAD_EDGE_MAP_CROSS_TILE_INDICES[nValidBitMask] ].GetMapsIndex();
								}
								else
								{
									index = rRoadsetDesc.roads[nRoadIndex].tiles[ ROAD_CROSS_TILE_INDICES[nValidBitMask] ].GetMapsIndex();
								}
								pTerrainInfo->patches[nPatchYIndex][nPatchXIndex].roads[nRoadIndex].push_back( SRoadTileInfo( nXIndex - ( nPatchXIndex * STerrainPatchInfo::nSizeX ), nYIndex - ( nPatchYIndex * STerrainPatchInfo::nSizeY ), index ) );
							}
						}
					}
				}
			}
		}
	}
	return true;
}
/* */
/* *

 */
/* *
 */
/* *
	 */
/* *

 */

// basement storage

/* *
 */


/* *

	 */
// DWORD dwTimer = GetTickCount();
// dwTimer = GetTickCount() - dwTimer;
// NStr::DebugTrace("CMapInfo::CreateMiniMapImage: create image: %d\n", dwTimer );
// dwTimer = GetTickCount();
/* *
 */


/* *
	 */
/* *
	 */

/* *
 */

/* *
 */
/* *

	 */
// collecting all road exits from the patch
/* *
 */
/* *
 */
// parameter for building roads, rectangles of patches are written here
// SRoadMakeParameter roadMakeParameter;
// roadMakeParameter.nMinMiddleDistance = 1;

// exit points for roads from patches (array by patch)
// std::vector<std::vector<SRoadPoint> > patchRoadPoints;

/* *
		 */