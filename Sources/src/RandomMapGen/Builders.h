#ifndef __BUILDERS_H__
#define __BUILDERS_H__

#include "../Formats/fmtTerrain.h"
#include "../Formats/fmtMap.h"

//
// 0
// /\
// /\
// 2 / \ 1
// \/
// \/
// \/
// 3
//
inline void GetPrimaryMaps(int nX, int nY, bool bFlipY, CVec2 *maps, const float fSizeX, const float fSizeY)
{
  // nX: 0..3 => { 0.5f, 1.0f, 0.0f, 0.5f }. 
  // nY: 0..7 => { 0.0f, 0.5f, 0.5f, 1.0f }. 
  const int nIndex0 = bFlipY ? 3 : 0;
  const int nIndex3 = bFlipY ? 0 : 3;
  constexpr int nIndex1 = 1;// bFlipX? 
  constexpr int nIndex2 = 2;// bFlipX? 
  const float fXCoeff = 64.0f / fSizeX;
  const float fYCoeff = 32.0f / fSizeY;
  //
  maps[nIndex0] = CVec2(0.5f * fXCoeff, 0.0f * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex1] = CVec2(1.0f * fXCoeff, 0.5f * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex2] = CVec2(0.0f * fXCoeff, 0.5f * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex3] = CVec2(0.5f * fXCoeff, 1.0f * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
}

inline void GetSecondaryMaps(int nX, int nY, bool bFlipY, CVec2 *maps, const float fSizeX, const float fSizeY)
{
  // nX: 0..2 => { 0.5f, 1.0f, 0.0f, 0.5f } + { 0.5, 0.5 }. 
  // nY: 0..6 => { 0.0f, 0.5f, 0.5f, 1.0f } + { 0.5, 0.5 }. 
  const int nIndex0 = bFlipY ? 3 : 0;
  const int nIndex3 = bFlipY ? 0 : 3;
  constexpr int nIndex1 = 1;// bFlipX? 
  constexpr int nIndex2 = 2;// bFlipX? 
  const float fXCoeff = 64.0f / fSizeX;
  const float fYCoeff = 32.0f / fSizeY;
  //
  maps[nIndex0] = CVec2((0.5f + 0.5f) * fXCoeff, (0.0f + 0.5f) * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex1] = CVec2((1.0f + 0.5f) * fXCoeff, (0.5f + 0.5f) * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex2] = CVec2((0.0f + 0.5f) * fXCoeff, (0.5f + 0.5f) * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
  maps[nIndex3] = CVec2((0.5f + 0.5f) * fXCoeff, (1.0f + 0.5f) * fYCoeff) + CVec2(nX * fXCoeff, nY * fYCoeff) + CVec2(0.5f / fSizeX, 0.5f / fSizeY);
}

struct STerrainTLVertex
{
  enum { format = GFXFVF_XYZRHW | GFXFVF_DIFFUSE | GFXFVF_TEX2 };

  //
  union
  {
    struct
    {
      float x, y, z, rhw;
      DWORD color;
      float tu, tv;
      float tu1, tv1;
    };

    struct
    {
      CVec4 pos;
      DWORD color;
      CVec2 tex;
      CVec2 tex1;
    };
  };

  //
  void Setup(float _sx, float _sy, float _sz, float _rhw, DWORD _color, float _tu, float _tv, float _tu1, float _tv1)
  {
    x = _sx;
    y = _sy;
    z = _sz;
    rhw = _rhw;
    color = _color;
    tu = _tu;
    tv = _tv;
    tu1 = _tu1;
    tv1 = _tv1;
  }
};

struct STerrainLVertex
{
  enum { format = GFXFVF_XYZ | GFXFVF_DIFFUSE | GFXFVF_TEX2 };

  //
  union
  {
    struct
    {
      float x, y, z;
      DWORD color;
      float tu, tv;
      float tu1, tv1;
    };

    struct
    {
      CVec3 pos;
      DWORD color;
      CVec2 tex;
      CVec2 tex1;
    };
  };

  //
  // parameter 'rhw' are fake to achieve compatibility with 'STerrainTLVertex'
  void Setup(float _sx, float _sy, float _sz, float _rhw, DWORD _color, float _tu, float _tv, float _tu1, float _tv1)
  {
    x = _sx;
    y = _sy;
    z = _sz;
    color = _color;
    tu = _tu;
    tv = _tv;
    tu1 = _tu1;
    tv1 = _tv1;
  }
};

// create main tiles with noise texture
void CreateTiles(float fX, float fY,
                 const STerrainPatchInfo &patch, const STerrainInfo &info, const STilesetDesc &tileset,
                 float fNoiseSizeX, float fNoiseSizeY, struct STerrainPatch *pPatch);
// create crosses and noise tiles for crosses
void CreateCrosses(float fX, float fY, const STerrainPatchInfo &patch, const STerrainInfo &info,
                   const STilesetDesc &tileset, const SCrossetDesc &crosset,
                   float fNoiseSizeX, float fNoiseSizeY, struct STerrainPatch *pPatch);
//
void CreateWarFog(float fX, float fY, int nStartX, int nStartY, const std::unordered_map<DWORD, DWORD> &visibilities,
                  const STerrainInfo &info, struct STerrainPatch *pPatch);
void CreateMarker(float fX, float fY, const std::vector<CTPoint<int>> &marker, const STerrainInfo &info,
                  IGFXVertices *pVertices, IGFXIndices *pIndices);
void CreateAIMarker(float fX, float fY, const STerrainInfo &terraInfo,
                    struct SAIPassabilityInfo *marker, int nMarkerSize, IGFXVertices *pVertices, IGFXIndices *pIndices);
int SliceSpline(const class CAnalyticBSpline2 &spline, std::list<SVectorStripeObjectPoint> *points, float *pfRest, float fStep);
void CreateRiver(const SVectorStripeObject &river, struct STerrainRiver *pRiver);
void CreateRoad(const SVectorStripeObject &road, struct STerrainRoad *pRoad);


#endif // __BUILDERS_H__