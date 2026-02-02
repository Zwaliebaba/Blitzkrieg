#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#pragma once

enum
{
  TERRAIN_BASE_VALUE = 0x10080000,
  TERRAIN_TERRAIN = TERRAIN_BASE_VALUE + 1
};

struct SSoundTerrainInfo
{
  // for sorting
  class PrSoundsMassSort
  {
  public:
    bool operator()(const SSoundTerrainInfo &s1, const SSoundTerrainInfo &s2) { return s1.fWeight > s2.fWeight; }
  };

  class PrTerrainTypeSort
  {
  public:
    bool operator()(const SSoundTerrainInfo &s1, const SSoundTerrainInfo &s2) { return s1.nTerrainType < s2.nTerrainType; }
  };

  // for finding objects with zero mass
  class PrZeroMass
  {
  public:
    bool operator()(const SSoundTerrainInfo &s1) { return s1.fWeight == 0.0f; }
  };

  CVec2 vPos;// position of mass center of terrain
  float fWeight;// weight of this terrain on screen
  int nTerrainType;// type of this terrain
  SSoundTerrainInfo() : vPos(VNULL2), fWeight(0.0f), nTerrainType(-1) {}
};

interface ITerrain : IRefCount
{
  // initialization
  virtual void Init(interface ISingleton *pSingleton) = 0;
  virtual void ResetPosition() = 0;
  // sizes
  virtual int GetSizeX() const = 0;
  virtual int GetSizeY() const = 0;
  virtual int GetPatchesX() const = 0;
  virtual int GetPatchesY() const = 0;
  // height in the point
  virtual float GetHeight(const CVec2 &vPos) = 0;
  // drawing
  virtual bool Draw(interface ICamera *pCamera) = 0;
  virtual bool DrawVectorObjects() = 0;
  virtual bool DrawMarkers() = 0;
  virtual void DrawBorder(DWORD dwColor, int nTiles, bool bUseFog) = 0;
  // draw fog of war. 
  virtual bool DrawWarFog() = 0;
  virtual void SetWarFog(struct SAIVisInfo *vises, int nNumVises) = 0;
  // enables
  virtual bool EnableGrid(bool bGridOn) = 0;
  virtual bool EnableNoise(bool bEnable) = 0;
  //
  virtual bool Load(const char *pszName, const struct STerrainInfo &terrainInfo) = 0;
  // markers
  virtual void SetAIMarker(struct SAIPassabilityInfo *infos, int nNumInfos) = 0;


  // for sound
  // returns random sound for this terrain or 0
  virtual const char * GetTerrainSound(int nTerrainType) = 0;

  // virtual int GetNTerrainCycleSounds( int nTerrainType ) = 0;
  virtual const char * GetTerrainCycleSound(int nTerrainType) = 0;
  // returns *pnSize or less terrains that can make sounds. 
  // *pnSize
  // scans through visible terrain and returns data about it
  // returned terrain is sorted by number of terrain (the smaller numbers are at start)
  virtual void GetTerrainMassData(SSoundTerrainInfo **ppData, int *pnSize) = 0;
  // returns relative volume of all sounds for specific terrain
  virtual float GetSoundVolume(int nTerrainType) const = 0;
};

// special terrain interface for working in the editor.
// NOTE: this interface does NOT support refcounting
interface ITerrainEditor
{
  // import/export to image
  virtual bool Import(interface IImage *pImage) = 0;
  virtual interface IImage * Export() = 0;
  // editor part
  virtual bool GetTileIndex(const CVec3 &point, int *pnX, int *pnY, bool isExact = false) = 0;
  virtual bool GetAITileIndex(const CVec3 &point, int *pnX, int *pnY, bool isExact = false) = 0;
  virtual void SetTile(int x, int y, BYTE tile) = 0;
  virtual BYTE GetTile(int x, int y) = 0;
  virtual void SetShade(int x, int y, BYTE shade) = 0;
  virtual BYTE GetShade(int x, int y) = 0;
  // patches inclusive!
  virtual void Update(const CTRect<int> &rcPatches) = 0;
  virtual void SetMarker(const CTPoint<int> *pPoints, int nNumPoints) = 0;
  // virtual void SetRoads( const struct SRoadItem *pItems, int nNumItems ) = 0;
  // rivers & roads
  // NOTE: this function uses TEMP BUFFER 0
  virtual void SampleCurve(const CVec3 *plots, int nNumPlots, float fStep,
                                   struct SVectorStripeObjectPoint **ppSamples, int *pnNumSamples) = 0;
  virtual void SmoothCurveWidth(SVectorStripeObjectPoint *points, int nNumPoints) = 0;
  virtual int AddRiver(const struct SVectorStripeObject &river) = 0;
  virtual bool UpdateRiver(int nID) = 0;
  virtual bool RemoveRiver(int nID) = 0;
  virtual int AddRoad(const struct SVectorStripeObject &road) = 0;
  virtual bool UpdateRoad(int nID) = 0;
  virtual bool RemoveRoad(int nID) = 0;
  // get internal terrain descriptor
  virtual const struct STerrainInfo & GetTerrainInfo() const = 0;
  // set descriptors
  virtual const struct STilesetDesc & GetTilesetDesc() const = 0;
  virtual const struct SCrossetDesc & GetCrossetDesc() const = 0;
  // virtual const struct SRoadsetDesc& GetRoadsetDesc() const = 0;
};

inline ITerrain *CreateTerrain()
{
  auto pTerrain = static_cast<ITerrain *>(GetCommonFactory()->CreateObject(TERRAIN_TERRAIN));
  pTerrain->Init(GetSingletonGlobal());
  return pTerrain;
}

#endif // __TERRAIN_H__