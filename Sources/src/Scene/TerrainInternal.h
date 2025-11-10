#ifndef __TERRAININTERNAL_H__
#define __TERRAININTERNAL_H__

#include "Terrain.h"
#include "Builders.h"
#include "../AILogic/AITypes.h"
#include "TerrainWater.h"
#include "TerrainRoad.h"
#include "TerrainBuilder.h"

struct STerrainPatch
{
#ifdef _USE_HWTL
  typedef STerrainLVertex SVertex1;
  typedef SGFXLVertex SVertex2;
#else
  using SVertex1 = STerrainTLVertex;
  using SVertex2 = SGFXTLVertex;
#endif // _USE_HWTL
  using CVertex1List = std::vector<SVertex1>;
  using CVertex2List = std::vector<SVertex2>;
  using CIndexList = std::vector<WORD>;
  // main verticals
  CVertex1List mainverts1;// main verts with noise
  CVertex2List mainverts2;// main verts w/o noise
  CVertex1List basecrossverts;// base cross verticals
  std::vector<CVertex1List> layercrossverts;// layered crosses
  std::vector<CVertex1List> layernoiseverts;// layered noises
  CVertex2List noiseverts;// noise w/o crosses
  CVertex2List warfogverts;// fog of war (line of sight)
  CIndexList warfoginds;// indices for fog of war
  int nX, nY;
  bool bSubPatches[4];//
  //
  void Clear()
  {
    mainverts1.clear();
    mainverts2.clear();
    basecrossverts.clear();
    layercrossverts.clear();
    layernoiseverts.clear();
    noiseverts.clear();
    warfogverts.clear();
  }
};

using SMarkerVertex = SGFXTLVertex;

struct SVisMarker
{
  std::vector<CTPoint<int>> info;
  std::vector<SMarkerVertex> vertices;
  std::vector<WORD> indices;
  //
  void Clear()
  {
    info.clear();
    vertices.clear();
    indices.clear();
  }
};

struct SAIMarker
{
  std::vector<SAIPassabilityInfo> info;
  std::vector<SMarkerVertex> vertices;
  std::vector<WORD> indices;
  //
  void Clear()
  {
    info.clear();
    vertices.clear();
    indices.clear();
  }
};

template<class TVertex>
struct STerraMesh
{
  std::vector<TVertex> vertices;
  std::vector<WORD> indices;
  //
  void Clear()
  {
    vertices.clear();
    indices.clear();
  }

  void Reserve(const int nNumVertices, const int nNumIndices)
  {
    vertices.clear();
    vertices.reserve(nNumVertices);
    indices.clear();
    indices.reserve(nNumIndices);
  }

  const bool IsEmpty() const { return vertices.empty() || indices.empty(); }
};

struct STerrainCurrMeshData
{
  struct SCrossesLayer
  {
    STerraMesh<STerrainPatch::SVertex1> mshCrosses;
    STerraMesh<STerrainPatch::SVertex1> mshNoises;
    //
    const bool IsEmpty() const { return mshCrosses.IsEmpty() || mshNoises.IsEmpty(); }
  };

  //
  STerraMesh<STerrainPatch::SVertex1> mshNoiseTiles;// main tiles with noise
  STerraMesh<STerrainPatch::SVertex2> mshNoNoiseTiles;// main tiles w/o noise
  STerraMesh<STerrainPatch::SVertex1> mshBaseCrosses;// base crosses
  std::vector<SCrossesLayer> mshCrossLayers;// layered crosses
  STerraMesh<STerrainPatch::SVertex2> mshNoises;// noise over crosses
  STerraMesh<STerrainPatch::SVertex2> mshWarFog;// warfog
  //
  bool Draw(IGFX *pGFX, IGFXTexture *pTileset, IGFXTexture *pCrosset, IGFXTexture *pNoise, bool bEnableNoise);
};

__forceinline DWORD GetVisibilityColor(const DWORD dwKey, const std::unordered_map<DWORD, DWORD> &visibilities)
{
  std::unordered_map<DWORD, DWORD>::const_iterator pos = visibilities.find(dwKey);
  return pos != visibilities.end() ? static_cast<DWORD>((112UL - (DWORD(pos->second) << 4)) << 24) : 112UL << 24;
}

__forceinline DWORD GetVisibilityColor(const int nX, const int nY, const std::unordered_map<DWORD, DWORD> &visibilities) { return GetVisibilityColor((static_cast<DWORD>(nY) << 16) | static_cast<DWORD>(nX), visibilities); }

// terrain objects by layers
//
// * terrain configuration (hills, rocks, etc.)
// * roads
// * explosion craters & corpses (in order of appearing) <= dynamic layer
// * shadows
//
class CTerrain : public ITerrain, public ITerrainEditor
{
  OBJECT_NORMAL_METHODS(CTerrain);
  DECLARE_SERIALIZE;
  // shortcuts
  CPtr<IGFX> pGFX;
  CPtr<ITextureManager> pTM;
  CPtr<IDataStorage> pStorage;
  // 
  std::string szMapName;// terrain map name
  //
  STilesetDesc tilesetDesc;// tileset descriptor
  SCrossetDesc crossetDesc;// crosset descriptor
  // SRoadsetDesc roadsetDesc;							
  CPtr<IGFXTexture> pTileset, pCrosset;// tileset and crosset textures
  CPtr<IGFXTexture> pNoise;// noise texture
  int nTilesetSizeX, nTilesetSizeY;// tileset texture size
  int nCrossetSizeX, nCrossetSizeY;// crosset texture size
  int nNoiseSizeX, nNoiseSizeY;// noise texture size
  // CPtr<IGFXTexture> roadsets[nNumRoadTypes];	
  //
  STerrainInfo terrainInfo;// current terrain packed info
  std::vector<CTerrainWater> rivers;// mesh rivers
  std::vector<CTerrainRoad> roads;// mesh roads
  SVisMarker vismarker;// terrain vis marker
  SAIMarker aimarker;// terrain AI marker
  using CPatchesList = std::list<STerrainPatch>;
  CPatchesList patches;// current active patches (mesh)
  std::vector<DWORD> roadPatches;// patches for vector objects
  bool bGridOn;// turn grid on
  bool bEnableNoise;// enable noise drawing
  CVec3 vOldAnchor;// old camera's anchor for re-positioning
  //
  STerrainCurrMeshData mshCurrent;// current mesh data
  // visibilities
  std::unordered_map<DWORD, DWORD> visibilities;
  // terrain editor fields
  CTerrainBuilder terrabuild;
  // for terrain sound info
  std::vector<SSoundTerrainInfo> collectedInfo;
  //
  void CreatePatch(const STerrainPatchInfo &patch, STerrainPatch *pPatch);
  void MovePatch(int nX, int nY, const STerrainPatchInfo &patch, STerrainPatch *pPatch);
  void MoveWarFog();
  void MoveWarFogPatch(int nX, int nY, STerrainPatch *pPatch);
  bool ExtractVisiblePatches(ICamera *pCamera);
  void MovePatches();
  void ReBuildMeshes();
  void Clear();
  void ReservePatchesData();
  //
  void LoadLocal(const std::string &szName, const STerrainInfo &terrainInfo, bool bMinimizeRoadsets, bool bLoadTextures);
  //
  void DrawMarker();
  bool DrawGrid(const STerrainPatch &patch);
  bool DrawAISurface(const STerrainPatch &patch);
  void FillSoundInfo(std::vector<SSoundTerrainInfo> *collectedInfo, int nX, int nY);
  void DrawPatchBorder(CVec3 &vStartPos, const CVec3 &vFarOffset, const CVec3 &vStepOffset, bool bRightOrder, bool bXIncrement, int nXStart, int nYStart, DWORD dwColor, bool bUseFog);

public:
  CTerrain() : bGridOn(false), bEnableNoise(true), terrabuild(tilesetDesc, crossetDesc/* ,roadsetDesc */) {}
  // initialization
  void STDCALL Init(ISingleton *pSingleton) override;
  void STDCALL ResetPosition() override { vOldAnchor.Set(-1000000, -1000000, -1000000); }
  // sizes
  int STDCALL GetSizeX() const override { return terrainInfo.tiles.GetSizeX(); }
  int STDCALL GetSizeY() const override { return terrainInfo.tiles.GetSizeY(); }
  int STDCALL GetPatchesX() const override { return terrainInfo.patches.GetSizeX(); }
  int STDCALL GetPatchesY() const override { return terrainInfo.patches.GetSizeY(); }
  // height in the point
  float STDCALL GetHeight(const CVec2 &vPos) override;
  // drawing
  bool STDCALL Draw(ICamera *pCamera) override;
  bool STDCALL DrawWarFog() override;
  bool STDCALL DrawVectorObjects() override;
  bool STDCALL DrawMarkers() override;
  void STDCALL DrawBorder(DWORD dwColor, int nTiles, bool bUseFog) override;
  void STDCALL SetWarFog(struct SAIVisInfo *vises, int nNumVises) override;
  // enables
  bool STDCALL EnableGrid(bool _bGridOn) override
  {
    bool bOld = bGridOn;
    bGridOn = _bGridOn;
    return bOld;
  }

  bool STDCALL EnableNoise(bool bEnable) override
  {
    bool bOld = bEnableNoise;
    bEnableNoise = bEnable;
    return bOld;
  }

  //
  bool STDCALL Load(const char *pszName, const struct STerrainInfo &terrainInfo) override;
  // markers
  void STDCALL SetAIMarker(SAIPassabilityInfo *infos, int nNumInfos) override;
  // import/export to image
  bool STDCALL Import(interface IImage *pImage) override;
  interface IImage * STDCALL Export() override;
  // editor part
  bool STDCALL GetTileIndex(const CVec3 &point, int *pnX, int *pnY, bool isExact = false) override;
  bool STDCALL GetAITileIndex(const CVec3 &point, int *pnX, int *pnY, bool isExact = false) override;
  void STDCALL SetTile(int x, int y, BYTE tile) override;
  BYTE STDCALL GetTile(int x, int y) override;

  void STDCALL SetShade(int x, int y, BYTE shade) override;
  BYTE STDCALL GetShade(int x, int y) override;

  void STDCALL Update(const CTRect<int> &rcPatches) override;
  void STDCALL SetMarker(const CTPoint<int> *pPoints, int nNumPoints) override;
  // virtual void STDCALL SetRoads( const SRoadItem *pItems, int nNumItems );
  // rivers & roads
  void STDCALL SampleCurve(const CVec3 *plots, int nNumPlots, float fStep,
                           SVectorStripeObjectPoint **ppSamples, int *pnNumSamples) override;
  void STDCALL SmoothCurveWidth(SVectorStripeObjectPoint *points, int nNumPoints) override;
  int STDCALL AddRiver(const SVectorStripeObject &river) override;
  bool STDCALL UpdateRiver(int nID) override;
  bool STDCALL RemoveRiver(int nID) override;
  int STDCALL AddRoad(const struct SVectorStripeObject &road) override;
  bool STDCALL UpdateRoad(int nID) override;
  bool STDCALL RemoveRoad(int nID) override;
  // get the internal terrain descriptor for the editor
  const struct STerrainInfo & STDCALL GetTerrainInfo() const override { return terrainInfo; }
  // set descriptors
  const struct STilesetDesc & STDCALL GetTilesetDesc() const override { return tilesetDesc; }
  const struct SCrossetDesc & STDCALL GetCrossetDesc() const override { return crossetDesc; }
  // virtual const struct SRoadsetDesc& STDCALL GetRoadsetDesc() const { return roadsetDesc; 

  const char * STDCALL GetTerrainSound(int nTerrainType) override;
  const char * STDCALL GetTerrainCycleSound(int nTerrainType) override;
  void STDCALL GetTerrainMassData(SSoundTerrainInfo **ppData, int *pnSize) override;
  float STDCALL GetSoundVolume(int nTerrainType) const override;
};

#endif // __TERRAININTERNAL_H__