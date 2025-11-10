#ifndef __FMTTERRAIN_H__
#define __FMTTERRAIN_H__

#pragma once

constexpr float fCellSizeY = 16;
const float fCellSizeX = fCellSizeY * 2.0f;
const float fWorldCellSize = fCellSizeX * FP_SQRT_2;

// from AI to Vis coordinates
static constexpr float fAITileXCoeff = 32.0f * FP_SQRT_2 / 64.0f;
static constexpr float fAITileYCoeff = 32.0f * FP_SQRT_2 / 64.0f;
static constexpr float fAITileZCoeff = 32.0f * FP_SQRT_2 / 64.0f;
// 3D
inline void AI2Vis(CVec3 *pRes, const CVec3 &vPos) { pRes->Set(vPos.x * fAITileXCoeff, vPos.y * fAITileYCoeff, vPos.z * fAITileZCoeff); }
inline void AI2Vis(CVec3 *pRes, float x, float y, float z) { pRes->Set(x * fAITileXCoeff, y * fAITileYCoeff, z * fAITileZCoeff); }

inline void AI2Vis(CVec3 *pPos)
{
  pPos->x *= fAITileXCoeff;
  pPos->y *= fAITileYCoeff;
  pPos->z *= fAITileZCoeff;
}

// 2D
inline void AI2Vis(CVec2 *pRes, const CVec2 &vPos) { pRes->Set(vPos.x * fAITileXCoeff, vPos.y * fAITileYCoeff); }
inline void AI2Vis(CVec2 *pRes, float x, float y) { pRes->Set(x * fAITileXCoeff, y * fAITileYCoeff); }

inline void AI2Vis(CVec2 *pPos)
{
  pPos->x *= fAITileXCoeff;
  pPos->y *= fAITileYCoeff;
}

// 1D
inline const float AI2VisX(const float fVal) { return fVal * fAITileXCoeff; }
inline const float AI2VisY(const float fVal) { return fVal * fAITileYCoeff; }
inline const float AI2VisZ(const float fVal) { return fVal * fAITileZCoeff; }

inline void AI2VisX(float *pfVal) { *pfVal *= fAITileXCoeff; }
inline void AI2VisY(float *pfVal) { *pfVal *= fAITileYCoeff; }
inline void AI2VisZ(float *pfVal) { *pfVal *= fAITileZCoeff; }
// from Vis to AI coords
static const float fAITileXCoeff1 = 1.0f / fAITileXCoeff;
static const float fAITileYCoeff1 = 1.0f / fAITileYCoeff;
static const float fAITileZCoeff1 = 1.0f / fAITileZCoeff;
// 3D
inline void Vis2AIFast(CVec3 *pRes, const CVec3 &vPos) { pRes->Set(vPos.x * fAITileXCoeff1, vPos.y * fAITileYCoeff1, vPos.z * fAITileZCoeff1); }
inline void Vis2AIFast(CVec3 *pRes, float x, float y, float z) { pRes->Set(x * fAITileXCoeff1, y * fAITileYCoeff1, z * fAITileZCoeff1); }

inline void Vis2AIFast(CVec3 *pPos)
{
  pPos->x *= fAITileXCoeff1;
  pPos->y *= fAITileYCoeff1;
  pPos->z *= fAITileZCoeff1;
}

inline void Vis2AI(CVec3 *pPos)
{
  Vis2AIFast(pPos);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

inline void Vis2AI(CVec3 *pPos, const CVec3 &vPos)
{
  Vis2AIFast(pPos, vPos);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

inline void Vis2AI(CVec3 *pPos, float x, float y, float z)
{
  Vis2AIFast(pPos, x, y, z);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

// 2D
inline void Vis2AIFast(CVec2 *pRes, const CVec2 &vPos) { pRes->Set(vPos.x * fAITileXCoeff1, vPos.y * fAITileYCoeff1); }
inline void Vis2AIFast(CVec2 *pRes, float x, float y) { pRes->Set(x * fAITileXCoeff1, y * fAITileYCoeff1); }

inline void Vis2AIFast(CVec2 *pPos)
{
  pPos->x *= fAITileXCoeff1;
  pPos->y *= fAITileYCoeff1;
}

inline void Vis2AI(CVec2 *pPos)
{
  Vis2AIFast(pPos);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

inline void Vis2AI(CVec2 *pPos, const CVec2 &vPos)
{
  Vis2AIFast(pPos, vPos);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

inline void Vis2AI(CVec2 *pPos, float x, float y)
{
  Vis2AIFast(pPos, x, y);
  pPos->x = static_cast<int>(pPos->x + 0.3f);
  pPos->y = static_cast<int>(pPos->y + 0.3f);
}

// fit to grid
inline void Fit2Grid(CVec3 *pPos, const float fGridCellSize)
{
  *pPos /= fGridCellSize;
  pPos->x = static_cast<int>(pPos->x + 0.5f);
  pPos->y = static_cast<int>(pPos->y + 0.5f);
  *pPos *= fGridCellSize;
}

inline void FitAIOrigin2AIGrid(CVec3 *pPos, const CVec2 &vOrigin)
{
  // move the object to origin
  pPos->x -= vOrigin.x;
  pPos->y -= vOrigin.y;
  //
  Fit2Grid(pPos, 32.0f);
  //
  pPos->x += vOrigin.x;
  pPos->y += vOrigin.y;
}

inline void FitVisOrigin2AIGrid(CVec3 *pPos, const CVec2 &vOrigin)
{
  // vis => AI with rounding to whole AI points
  Vis2AI(pPos);
  // move the object to origin
  FitAIOrigin2AIGrid(pPos, vOrigin);
  // AI => Vis
  AI2Vis(pPos);
}

inline void FitVis2AIGrid(CVec3 *pPos) { FitVisOrigin2AIGrid(pPos, VNULL2); }

// ************************************************************************************************************************ //
// **
// ** tileset/crosset/roadset description
// **
// **
// **
// ************************************************************************************************************************ //

struct STileMapsDesc
{
  CVec2 maps[4];
  //
  STileMapsDesc() {}

  STileMapsDesc(const CVec2 &v1, const CVec2 &v2, const CVec2 &v3, const CVec2 &v4)
  {
    maps[0] = v1;
    maps[1] = v2;
    maps[2] = v3;
    maps[3] = v4;
  }

  //
  int operator&(IDataTree &ss);
};

struct SMainTileDesc
{
  int nIndex;// tile maps index from 'tilemaps' array
  float fProbFrom;// probability range 'from'
  float fProbTo;// probability range 'to'
  //
  int operator&(IDataTree &ss);
};

struct STileTypeDesc
{
  std::vector<SMainTileDesc> tiles;// tile of this terrain type
  short GetMapsIndex() const
  {
    NI_ASSERT_TF(!tiles.empty(), "Tileset is empty", return -1);
    const float fRndVal = static_cast<float>(rand() % 10000) / 100.0f;
    for (auto it = tiles.begin(); it != tiles.end(); ++it)
      if ((fRndVal >= it->fProbFrom) && (fRndVal <= it->fProbTo)) return it->nIndex;
    return tiles[0].nIndex;
  }

  //
  virtual int operator&(IDataTree &ss);
};

struct STerrTypeDesc : STileTypeDesc
{
  std::string szName;// terrain type name
  int nCrosset;// index of the crosset to use with this terrain
  int nPriority;// terrain type priority
  float fPassability;// passability coefficient [0..1]
  DWORD dwAIClasses;// AI classes, which can't pass through this terrain type
  bool bMicroTexture;// if true then terrain has microtexture

  enum ESoilParams
  {
    ESP_TRACE = 0x01,
    ESP_DUST = 0x02
  };

  BYTE cSoilParams;// soil parameters - traces, dust, etc.

  float fSoundVolume;// volume for all sounds from this terrain
  bool bCanEntrench;// can unit dig on this terrain
  //
  std::string szLoopedSound;// terrain ambient sounds
  std::string szSound;// looped terrain ambient sounds
  //
  STerrTypeDesc()
    : nCrosset(-1), nPriority(-1), fPassability(1), dwAIClasses(0), bMicroTexture(true),
      cSoilParams(0), fSoundVolume(1.0f), bCanEntrench(true) {}

  int operator&(IDataTree &ss) override;
};

struct STilesetDesc
{
  std::string szName;// tileset name
  std::vector<STileMapsDesc> tilemaps;// all tile maps
  std::vector<STerrTypeDesc> terrtypes;// terrain types of this tileset
  //
  int operator&(IDataTree &ss);
};

// roadset description
// name = [dir]_[type]_[subtype]
// dir:
// Right
// Left
// Up
// Down
//
// type:
// Center
// T-junction
// Cross (+)
// Bend
// End
//
// subtype:
// Center
// Far border
// Near border
//
/* *
 */

// crosset description
struct SCrossTileTypeDesc : STileTypeDesc
{
  std::string szName;// f', d, c, e, d', b', f, a, c', e', a', b
  //
  int operator&(IDataTree &ss) override;
};

struct SCrossDesc
{
  std::string szName;// cross name
  std::vector<SCrossTileTypeDesc> tiles;// cross tiles
  //
  int operator&(IDataTree &ss);
};

struct SCrossetDesc
{
  std::vector<SCrossDesc> crosses;// all cross types
  std::vector<STileMapsDesc> tilemaps;// maps
  //
  int operator&(IDataTree &ss);
};

#endif // __FMTTERRAIN_H__