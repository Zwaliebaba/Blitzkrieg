#if !defined(__UIMiniMap__)
#define __UIMiniMap__

#pragma once

#include "../GFX/GFX.h"
#include "../GFX/GFXHelper.h"
#include "UIBasic.h"
#include "../AILogic/AIConsts.h"
#include "../AILogic/AITypes.h"

struct STextureMarker
{
  CTRect<float> textureRect;// 0.0f...1.0f
  CTRect<int> screenRect;// including HotSpot
  CTPoint<int> size;

  // constructors
  STextureMarker()
    : textureRect(0.0f, 0.0f, 0.0f, 0.0f), screenRect(0, 0, 0, 0), size(0, 0) {}

  STextureMarker(const CTRect<float> &rTextureRect, const CTRect<int> &rScreenRect, const CTPoint<int> &rSize)
    : textureRect(rTextureRect), screenRect(rScreenRect), size(rSize) {}

  STextureMarker(const STextureMarker &rTextureMarker)
    : textureRect(rTextureMarker.textureRect), screenRect(rTextureMarker.screenRect), size(rTextureMarker.size) {}

  STextureMarker &operator=(const STextureMarker &rTextureMarker)
  {
    if (&rTextureMarker != this)
    {
      textureRect = rTextureMarker.textureRect;
      screenRect = rTextureMarker.screenRect;
      size = rTextureMarker.size;
    }
    return *this;
  }

  // serializing...
  virtual int STDCALL operator&(IDataTree &ss);
  virtual int STDCALL operator&(IStructureSaver &ss);
};


struct SMiniMapMarker : STextureMarker
{
  std::string szName;// for marker type feedback
  CVec2 vPos;// //0.0f ... 1.0f
  bool bActive;// is this point active now?
  int nID;// Marker ID
  NTimer::STime timeStart;// display time
  NTimer::STime timeDuration;// display duration
  // constructors
  SMiniMapMarker()
    : vPos(VNULL2), bActive(false), nID(-1), timeStart(0), timeDuration(0) {}

  SMiniMapMarker(const STextureMarker &rTextureMarker, const std::string &rszName, const CVec2 &vPos, bool _bActive, int _nID, const NTimer::STime &rTimeStart, const NTimer::STime &rTimeDuration)
    : STextureMarker(rTextureMarker), szName(rszName), vPos(vPos), bActive(_bActive), nID(_nID), timeStart(rTimeStart), timeDuration(rTimeDuration) {}

  SMiniMapMarker(const SMiniMapMarker &rMiniMapMarker)
    : STextureMarker(rMiniMapMarker.textureRect, rMiniMapMarker.screenRect, rMiniMapMarker.size), szName(rMiniMapMarker.szName), vPos(rMiniMapMarker.vPos), bActive(rMiniMapMarker.bActive), nID(rMiniMapMarker.nID), timeStart(rMiniMapMarker.timeStart), timeDuration(rMiniMapMarker.timeDuration) {}

  SMiniMapMarker &operator=(const SMiniMapMarker &rMiniMapMarker)
  {
    if (&rMiniMapMarker != this)
    {
      textureRect = rMiniMapMarker.textureRect;
      screenRect = rMiniMapMarker.screenRect;
      szName = rMiniMapMarker.szName;
      vPos = rMiniMapMarker.vPos;
      bActive = rMiniMapMarker.bActive;
      nID = rMiniMapMarker.nID;
      timeStart = rMiniMapMarker.timeStart;
      timeDuration = rMiniMapMarker.timeDuration;
    }
    return *this;
  }

  int STDCALL operator&(IStructureSaver &ss) override;
};


struct SMiniMapCircle
{
  CVec2 vCenter;
  float fRadius;
  NTimer::STime timeStart;
  NTimer::STime timeDuration;
  int nStyle;
  WORD wColor;
  LPARAM lParam;

  SMiniMapCircle()
    : vCenter(VNULL2), fRadius(0.0f), timeStart(0), timeDuration(0), nStyle(0), wColor(0xFFFF), lParam(0) {}

  SMiniMapCircle(const CVec2 &rvCenter, float _fRadius, const NTimer::STime &rTimeStart, const NTimer::STime &rTimeDuration, int _nStyle, WORD _wColor, LPARAM _lParam)
    : vCenter(rvCenter), fRadius(_fRadius), timeStart(rTimeStart), timeDuration(rTimeDuration), nStyle(_nStyle), wColor(_wColor), lParam(_lParam) {}

  SMiniMapCircle(const SMiniMapCircle &rCircle)
    : vCenter(rCircle.vCenter), fRadius(rCircle.fRadius), timeStart(rCircle.timeStart), timeDuration(rCircle.timeDuration), nStyle(rCircle.nStyle), wColor(rCircle.wColor), lParam(rCircle.lParam) {}

  SMiniMapCircle &operator=(const SMiniMapCircle &rCircle)
  {
    if (&rCircle != this)
    {
      vCenter = rCircle.vCenter;
      fRadius = rCircle.fRadius;
      timeStart = rCircle.timeStart;
      timeDuration = rCircle.timeDuration;
      nStyle = rCircle.nStyle;
      wColor = rCircle.wColor;
      lParam = rCircle.lParam;
    }
    return *this;
  }
};


class CMarkPixelFunctional
{
  CTextureLock<SGFXColor4444> *pTextureLock;
  SGFXColor4444 color;
  CTPoint<int> size;

public:
  CMarkPixelFunctional(CTextureLock<SGFXColor4444> *_pTextureLock, SGFXColor4444 _color, const CTPoint<int> &_size)
    : pTextureLock(_pTextureLock), color(_color), size(_size) {}

  void operator()(int nXPos, int nYPos)
  {
    if ((nXPos >= 0) &&
        (nXPos < size.x) &&
        (nYPos >= 0) &&
        (nYPos < size.y)) { (*pTextureLock)[size.y - 1 - nYPos][nXPos] = color; }
  }
};

// The vertexes are located as follows:
// in texture: (in AI tiles the Y axis is inverted)
// 0 ---------------- 3
// |  
// |  
// |  
// |  
// |  
// 1 ---------------- 2
// on the screen (in control)
// 0
// /\
// /\
// /\
// 1 / \ 3
// \/
// \/
// \/
// \/
// 2

class CUIMiniMap : public CSimpleWindow
{
  DECLARE_SERIALIZE;

  const static char MARKERS_TYPES_FILE_NAME[];
  const static char MARKERS_TEXTURE_NAME[];
  const static char MARKERS_TYPES_NAME[];

  // Texture values ​​pWarFog and pWarFogTexture (0 - tile closed, 1 - tile open)
  SGFXColor4444 pWarFogValues[SAIConsts::VIS_POWER + 1];
  // Unit lights depending on the player
  SGFXColor4444 pPartyColors[SAIConsts::MAX_NUM_OF_PLAYERS + 1];
  // Screen frame light
  DWORD dwScreenFrameColor;
  DWORD dwScreenFrameColorShadow;
  // MiniMap horizontal size (read from XML file)
  // the vertical size of MiniMap is nSize / 2
  int nSize;
  int nPlayersCount;
  // The size of the cross marking the unit (0 - dot, 1 - cross 3x3, 2 - cross 5x5 ...)
  int nUnitCrossSize;

  // size of the battlefield in VIS tiles (taken from AITerrain during initialization)
  CTPoint<int> terrainSize;
  // number of filled VIS tiles at the moment
  int nFiledVISTiles;

  // array of AI objects (Filled in according to information from AILogic)
  std::vector<SMiniMapUnitInfo> units;
  // array of firing radii (Filled in according to information from AILogic)
  std::vector<SShootAreas> shootAreas;
  // list of artillery circles (Filled in according to information from AILogic)
  std::list<SMiniMapCircle> circles;

  // list of items indicated by a flag
  std::hash_map<std::string, STextureMarker> markersTypes;
  std::list<SMiniMapMarker> markers;

  bool isWarFogNeedUpdate;
  bool isInstantObjectsNeedUpdate;

  // Fog Of War texture (Filled in according to information from AILogic)
  CPtr<IGFXTexture> pWarFog;
  // Fog Of War texture (It is directly located in video memory and is drawn on the screen)
  CPtr<IGFXTexture> pWarFogTexture;
  // texture with units, firing radii, etc. 
  CPtr<IGFXTexture> pInstantObjects;
  // texture with units, firing radii, etc. 
  CPtr<IGFXTexture> pInstantObjectsTexture;
  // Mini Map texture (Loaded from resources)
  CPtr<IGFXTexture> pBackgroundTexture;
  // Objective texture (Loaded from resources)
  CPtr<IGFXTexture> pMarkerTexture;

  // time of the previous update (in game time units)
  // DWORD dwPreviousUpdateTime;
  // timeout for updating information about objects on MiniMap
  // DWORD dwRefreshTimeout;

  void CreateMiniMapTextures();
  // returns the coordinates of the zero point in texture coordinates ( isTopLeft == true - the Y axis is inverted)
  // returns the coordinates of the zero point in fog of war coordinates ( isTopLeft == false )
  void GetZeroPoint(float *pfXZeroPoint, float *pfYZeroPoint, bool isTopLeft = true);
  // returns the coordinates of a point in texture coordinates ( isTopLeft == true - the Y axis is inverted)
  // returns the coordinates of a point in fog of war coordinates ( isTopLeft == false )
  void PointToTextureMiniMap(float fXPos, float fYPos, float *pfXMiniMapPos, float *pfYMiniMapPos, bool isLeftTop = true);
  // returns the coordinates of a point in world coordinates ( isTopLeft == true - the Y axis is inverted)
  // returns the coordinates of a point in world coordinates ( isTopLeft == false )
  void TextureMiniMapToPoint(float fXMiniMapPos, float fYMiniMapPos, float *pfXPos, float *pfYPos, bool isLeftTop = true);
  // Y coordinate at point X, obtained through the equation of a line passing through two points
  float GetYByX(float fX, float fX0, float fY0, float fX1, float fY1)
  {
    NI_ASSERT_SLOW_T(( fX1 - fX0 ) != 0,
                     NStr::Format( "Devision by zero: (%f)", fX1 - fX0 ));
    return (fY1 * (fX - fX0) - fY0 * (fX - fX1)) / (fX1 - fX0);

  }

  // point X along the Y coordinate, obtained through the equation of a line passing through two points
  float GetXByY(float fY, float fX0, float fY0, float fX1, float fY1)
  {
    NI_ASSERT_SLOW_T(( fY1 - fY0 ) != 0,
                     NStr::Format( "Devision by zero: (%f)", fY1 - fY0 ));
    return (fX1 * (fY - fY0) - fX0 * (fY - fY1)) / (fY1 - fY0);
  }

  // Returns the edge of the screen frame on a miniMap with the edges cut off, in world coordinates
  // Sequence of points, as in edge 0-1
  void GetVerticalClippedScreenEdge(const CTPoint<float> &v0, const CTPoint<float> &v1, std::vector<CTPoint<float>> *pvPoints);
  // Returns the edge of the screen frame on a miniMap with the edges cut off, in world coordinates
  // Sequence of points, as in edge 1-2
  void GetHorizontalClippedScreenEdge(const CTPoint<float> &v1, const CTPoint<float> &v2, std::vector<CTPoint<float>> *pvPoints);
  // Returns the screen frame on the miniMap with the edges cut off, in world coordinates
  // Requires translation to local screen coordinates of the control using the PointToTextureMiniMap() method
  void GetClippedScreenFrame(std::vector<CTPoint<float>> *pvPoints, IGFX *pGFX);

  // Condition for calling the SetScreenSize() method: if it is not called, the minimap will not be displayed
  bool IsInitialized()
  {
    return ((terrainSize.x > 0) &&
            (terrainSize.y > 0));
  }

  // Checking whether a point belongs to a minimap rectangle
  bool InMiniMap(float fXPos, float fYPos)
  {
    return ((fXPos >= 0) &&
            (fXPos <= terrainSize.x) &&
            (fYPos >= 0) &&
            (fYPos <= terrainSize.y));
  }

  //
  void DrawFireRanges(CTextureLock<SGFXColor4444> *pTextureLock);

public:
  CUIMiniMap()
    : dwScreenFrameColor(0xFFAFAFAF), dwScreenFrameColorShadow(0xFF4F4F4F), nSize(0), nPlayersCount(2), nUnitCrossSize(1), terrainSize(0, 0), nFiledVISTiles(0), isWarFogNeedUpdate(false), isInstantObjectsNeedUpdate(false)
  {
    for (int nFogValue = 0; nFogValue <= SAIConsts::VIS_POWER; ++nFogValue)
    {
      WORD a = 8 - 8 * nFogValue / SAIConsts::VIS_POWER;
      pWarFogValues[nFogValue] = a << 12;
    }

    // CRAP{ it is necessary to initialize the player colors externally
    pPartyColors[0] = 0xF0F0;
    pPartyColors[1] = 0xFF00;
    pPartyColors[2] = 0xF00F;
    pPartyColors[3] = 0xFFF0;
    pPartyColors[4] = 0xF0FF;
    pPartyColors[5] = 0xFF0F;
    pPartyColors[6] = 0xFFFF;
    pPartyColors[7] = 0xFF80;
    pPartyColors[8] = 0xFF08;
    pPartyColors[9] = 0xF8F0;
    pPartyColors[10] = 0xF0F8;
    pPartyColors[11] = 0xF80F;
    pPartyColors[12] = 0xF08F;
    pPartyColors[13] = 0xFF88;
    pPartyColors[14] = 0xF8F8;
    pPartyColors[15] = 0xF88F;
    pPartyColors[16] = 0x0000;
    /* *
     */
    // }CRAP
  }

  ~CUIMiniMap() override {}

  // set the field size to receive information on fog of war, fog textures will be created based on these sizes
  // size in VIS tiles
  virtual void STDCALL SetTerrainSize(int nXTerrainSize, int nYTerrainSize, int _nPlayersCount);
  // set map texture
  virtual void STDCALL SetBackgroundTexture(IGFXTexture *_pBackgroundTexture) { pBackgroundTexture = _pBackgroundTexture; }

  // add information about fog of war, taken from AILogic by calling the method:
  // if the last piece of information is received, true is returned
  // otherwise, false is returned
  virtual bool STDCALL AddWarFogData(const BYTE *pVizBuffer, int nLength);
  // add information about units, taken from AILogic by calling the method:
  virtual void STDCALL AddUnitsData(const struct SMiniMapUnitInfo *pUnitsBuffer, int nUnitsCount);
  // add information about displayed radii and firing angles
  virtual void STDCALL AddFireRangeAreas(const struct SShootAreas *pShootAreasBuffer, int nShootAreasCount);
  //
  virtual void STDCALL AddCircle(const CVec2 &vCenter, float fRadius, int nStyle, WORD wColor, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative, LPARAM lParam);
  virtual int STDCALL AddMarker(const std::string &rszName, const CVec2 &vPos, bool _bActive, int _nID, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative);
  virtual void STDCALL ActivateMarker(int _nID, bool _bActive);
  virtual void STDCALL ActivateMarker(const std::string &rszName, bool _bActive);
  virtual void STDCALL RemoveMarker(int _nID);
  virtual void STDCALL RemoveMarker(const std::string &rszName);

  // from UIControl interface
  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // Mouse moving
  bool STDCALL IsInside(const CVec2 &vPos) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  // update
  bool STDCALL Update(const NTimer::STime &currTime) override;
  // drawing
  void STDCALL Draw(interface IGFX *_pGFX) override = 0;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
};


class CUIMiniMapBridge : public IUIMiniMap, public CUIMiniMap
{
  OBJECT_NORMAL_METHODS(CUIMiniMapBridge);
  DECLARE_SUPER(CUIMiniMap);
  DEFINE_UIELEMENT_BRIDGE;

  void STDCALL SetTerrainSize(int nXTerrainSize, int nYTerrainSize, int _nPlayersCount) override { CSuper::SetTerrainSize(nXTerrainSize, nYTerrainSize, _nPlayersCount); }
  void STDCALL SetBackgroundTexture(IGFXTexture *_pBackgroundTexture) override { CSuper::SetBackgroundTexture(_pBackgroundTexture); }
  bool STDCALL AddWarFogData(const BYTE *pVizBuffer, int nLength) override { return CSuper::AddWarFogData(pVizBuffer, nLength); }
  void STDCALL AddUnitsData(const struct SMiniMapUnitInfo *pUnitsBuffer, int nUnitsCount) override { CSuper::AddUnitsData(pUnitsBuffer, nUnitsCount); }
  void STDCALL AddFireRangeAreas(const struct SShootAreas *pShootAreasBuffer, int nShootAreasCount) override { CSuper::AddFireRangeAreas(pShootAreasBuffer, nShootAreasCount); }
  void STDCALL AddCircle(const CVec2 &vCenter, const float fRadius, int nStyle, WORD wColor, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative, LPARAM lParam) override { CSuper::AddCircle(vCenter, fRadius, nStyle, wColor, rStart, rDuration, bRelative, lParam); }
  //
  int STDCALL AddMarker(const std::string &rszName, const CVec2 &vPos, bool _bActive, int _nID, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative) override { return CSuper::AddMarker(rszName, vPos, _bActive, _nID, rStart, rDuration, bRelative); }
  void STDCALL ActivateMarker(int _nID, bool _bActive) override { CSuper::ActivateMarker(_nID, _bActive); }
  void STDCALL ActivateMarker(const std::string &rszName, bool _bActive) override { CSuper::ActivateMarker(rszName, _bActive); }
  void STDCALL RemoveMarker(int _nID) override { CSuper::RemoveMarker(_nID); }
  void STDCALL RemoveMarker(const std::string &rszName) override { CSuper::RemoveMarker(rszName); }
};
#endif // #if !defined(__UIMiniMap__)