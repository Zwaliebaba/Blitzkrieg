#ifndef __RECTTILES_H__
#define __RECTTILES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "AIHashFuncs.h"
#include "AICellsTiles.h"
// //////////////////////////////////////////////////////////// 
using CTilesSet = std::list<SVector>;
// //////////////////////////////////////////////////////////// 
// for GetTilesCoveredByRectSides
class CTilesCollector
{
public:
  CTilesSet *pTiles;
  CTilesCollector(CTilesSet *pTiles) : pTiles(pTiles) {}
  bool operator()(float x, float y);
};

// //////////////////////////////////////////////////////////// 
bool IsRectOnLockedTiles(const SRect &rect, BYTE aiClass);
// returns the tiles that the given quad covers
void GetTilesCoveredByQuadrangle(const CVec2 &v1, const CVec2 &v2, const CVec2 &v3, const CVec2 &v4, CTilesSet *pTiles);
// returns the tiles that this rect covers
void GetTilesCoveredByRect(const SRect &rect, CTilesSet *pTiles);
// returns tiles that intersect with the sides of the given rect
void GetTilesCoveredByRectSides(const SRect &rect, CTilesSet *pTiles);
// the same except for the side that lies towards dir from the center
void GetTilesCoveredByRectSides(const SRect &rect, CTilesSet *pTiles, WORD dir);
// tiles under the side that lies towards dir from the center
void GetTilesCoveredBySide(const SRect &rect, CTilesSet *pTiles, WORD dir);
// whether the map under rect is free (there are no locked tiles and does not intersect with units), the pUnit unit is not taken into account
bool IsMapFullyFree(const SRect &rect, interface IBasePathUnit *pUnit);
void GetTilesNextToRect(const SRect &rect, CTilesSet *pTiles);
// same as above, excludes tiles near given direction from center.
void GetTilesNextToRect(const SRect &rect, CTilesSet *pTiles, WORD wDirExclude);
// //////////////////////////////////////////////////////////// 
inline bool operator <(const SVector &cell1, const SVector &cell2);
// //////////////////////////////////////////////////////////// 
#endif // __RECTTILES_H__