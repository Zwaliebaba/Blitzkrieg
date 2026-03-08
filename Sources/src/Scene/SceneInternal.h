#ifndef __SCENEINTERNAL_H__
#define __SCENEINTERNAL_H__

#pragma once

#include "../SFX/SFX.h"
#include "../UI/UI.h"
#include "../AILogic/AITypes.h"
#include "FixedObjList.h"

struct SParticleInfo
{
  CVec3 vPos;// position in 3D space
  float fSize;// half size
  DWORD color, specular;// color and specular
  CTRect<float> maps;// texture mapping coords
  float fAngle;// rotation angle
};

struct SVisObjDesc
{
  EObjVisType vistype;// visual type (sprite/mesh/effect)
  EObjGameType gametype;// game type (unit/building/object/etc.)
  CGDBPtr<SGDBObjectDesc> pDesc;// GDB descriptor
  bool bOutbound;// is this object can be placed out of map bounds ?
  //
  int operator&(IStructureSaver &ss);
};

using CVisObjDescMap = std::unordered_map<IVisObj *, SVisObjDesc, SDefaultPtrHash>;

struct SToolTip
{
  CPtr<IGFXText> pText;// graphics text
  CTRect<long> rcRect;// rect of this text
  DWORD dwBorderColor;// border rect color
  bool bHasText;//
  bool bHasFont;
  //
  SToolTip() : dwBorderColor(0xffffff00), bHasText(false), bHasFont(false) {}
  //
  void Init();
  void Clear();
  //
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &pText);
    saver.Add(2, &rcRect);
    saver.Add(3, &bHasText);
    saver.Add(4, &bHasFont);
    return 0;
  }
};

struct STemporalMesh
{
  CPtr<IGFXVertices> pVertices;
  CPtr<IGFXIndices> pIndices;
  CPtr<IGFXTexture> pTexture;
  int nShadingEffect;
  bool bTemporal;
  int nIndex;
};

struct SMeshPair2
{
  CPtr<IGFXTexture> pTexture;
  std::vector<BYTE> vertices;
  int nNumVertices;
  DWORD dwVertexFormat;
  std::vector<WORD> indices;
  EGFXPrimitiveType ePrimitiveType;
  int nShadingEffect;
  bool bTemporary;
};

// CRAP{ before minimap
struct SCircle
{
  CVec2 vCenter;
  float fRadius;
  NTimer::STime timeStart;
  NTimer::STime timeDuration;
};

// CRAP}

struct SRainDrop
{
  CVec3 vPos;
  float fLength;
  SRainDrop() { fLength = 100000.0f; };
};

struct SSnowFlake
{
  CVec3 vPos;
  float fPhase;
  SSnowFlake() { vPos.z = -100.0f; };
};

struct SSandParticle
{
  CVec3 vPos;
  CVec3 vPhase;
  bool bCone;
  bool bConeDraw;
  SSandParticle() { vPos.z = -100.0f; };
};

struct SClickMarker
{
  CVec3 vPos;
  NTimer::STime nStartTime;
};

using CSpriteVisList = std::list<const SBasicSpriteInfo *>;
using CMeshVisList = std::list<IMeshVisObj *>;
using CMeshObjList = std::list<CPtr<IMeshVisObj>>;
using CEffectObjList = std::list<CPtr<IEffectVisObj>>;
using CSpritesObjList = std::list<CPtr<ISpriteVisObj>>;
using CParticlesVisList = std::list<SParticleInfo>;
using CSceneObjectsList = std::list<CPtr<ISceneObject>>;
using CParticlesVisMap = std::unordered_map<IGFXTexture *, CParticlesVisList, SDefaultPtrHash>;

using CObjectsArea = CAreaMap<IObjVisObj>;

using CSpritesArea = CAreaMap<ISpriteVisObj>;
using CObjVisObjArea = CAreaMap<IObjVisObj>;
using CMeshesArea = CAreaMap<IMeshVisObj>;
using CEffectsArea = CAreaMap<IEffectVisObj>;
using CVisObjArea = CAreaMap<IVisObj>;
using CObjFixedArea = CFixedObjAreaMap<4, IObjVisObj>;

using CMechTraceArea = CStructAreaMap<SMechTrace>;
using CGunTraceArea = CStructAreaMap<SGunTrace>;


static constexpr float AREA_MAP_CELL_SIZE_IN_TILES = 8;
static const float AREA_MAP_CELL_SIZE = AREA_MAP_CELL_SIZE_IN_TILES * fWorldCellSize;

class CSoundScene;

class CScene : public IScene
{
  OBJECT_COMPLETE_METHODS(CScene);
  DECLARE_SERIALIZE;
  // managers and LAPIs
  CPtr<IGFX> pGFX;// main graphics LAPI
  CPtr<ISFX> pSFX;// main sounds LAPI
  CPtr<IGameTimer> pTimer;// main game timer
  CPtr<CSoundScene> pSoundScene;
  // scene data
  CVisObjDescMap objdescs;// object descriptors
  //
  /* CObjectsArea areaUnits;								 */
  // shell holes
  CObjFixedArea areaCraters;// all craters in fixed area map
  // dynamic objects
  CObjVisObjArea areaUnits;// sprite and mesh units
  CMeshesArea meshGraveyardArea;// dead mesh units
  CVisObjArea effectsArea;// all effects
  // static objects
  CSpritesArea spriteObjectsArea;// sprite objects - buildings, trees, other stuff...
  // terrain objects
  CObjVisObjArea terraObjectsArea;// terrain objects
  // shadows
  CSpritesArea shadowObjectsArea;// shadow objects
  //
  CMeshObjList outboundObjects;// objects, which are out of map bounds
  CMeshObjList outboundObjects2;// coastal artillery and others like it
  CEffectObjList outboundEffects;// outbound effect objects
  CSpritesObjList outboundSprites;// outbound sprite units
  CMechTraceArea mechTracesArea;// mech traces
  CGunTraceArea gunTracesArea;// gun traces
  CSceneObjectsList alwaysObjects;// always visible objects
  // UI screens
  std::list<CPtr<IUIScreen>> uiScreens;
  CPtr<IUIScreen> pMissionScreen;
  // lines
  std::list<CPtr<IBoldLineVisObj>> boldLines;
  using CSquadVisObjList = std::list<CPtr<ISquadVisObj>>;
  CSquadVisObjList squads;// squad icons on the screen - 'who-in-the-container' interface
  // temporal meshes
  std::list<STemporalMesh> tempmeshes;//
  std::list<SMeshPair2> meshpairs2;
  // CRAP{ circles
  std::list<SCircle> circles;
  // CRAP}
  // weather
  enum EStormType
  {
    ST_RAIN,
    ST_SNOW,
    ST_SAND
  };

  enum EStormCondition
  {
    SC_NONE,
    SC_STARTING,
    SC_ON,
    SC_FINISHING
  };

  CTRect<float> viewableTerrainRect;
  int nLastWeatherUpdate;
  bool bWeatherInitialized;
  EStormCondition eWeatherCondition;
  bool bWeatherOn;
  EStormType eCurrSetting;
  WORD wAmbientID;
  int nNextRandomSound;
  float fChangeSpeed;
  // rain
  int nRainDensity;
  std::vector<SRainDrop> rainDrops;
  CVec3 vRainDir;
  float fRainHeight;
  DWORD dwRainTopColor;
  DWORD dwRainBottomColor;
  // snow
  int nMinSnowDensity;
  int nMaxSnowDensity;
  std::vector<SSnowFlake> snowFlakes;
  float fSnowHeight;
  DWORD dwSnowColor;
  float fSnowFallingSpeed;
  float fSnowAmplitude;
  float fSnowFrequency;
  // sand
  int nSandDensity;
  std::vector<SSandParticle> sandParticles;
  float fSandHeight;
  CVec2 vSandCone;
  CVec2 vConeSpeed;
  float fConeRadius;
  int nLastConeGenerated;
  float fSandAmplitude;
  float fSandFrequency;
  CVec3 vSandWind;
  float fSandSpeed;
  float fSandConeSpeed;
  CPtr<IGFXTexture> pSandTexture;
  // markers drawing data
  bool bDrawArrow;
  bool bRotateMarkers;
  std::list<SClickMarker> clickMarkers;
  std::list<CVec3> posMarkers;
  float fArrowBegin;
  int nMarkerLifetime;
  CVec3 vArrowStart;
  CVec3 vArrowDir;
  DWORD dwArrowColor;
  DWORD dwMarkerColor;
  // range areas
  using CShootAreasList = std::vector<SShootAreas>;
  CShootAreasList areas;
  //
  SToolTip tooltip;// current tooltip
  //
  CPtr<ITerrain> pTerrain;
  CTPoint<int> vMapSize;// map size (in world units)
  CPtr<ICursor> pCursor;
  // own objects
  CObj<IFrameSelection> pFrameSelection;// frame selection object
  CObj<IStatSystem> pStatSystem;// statistics system
  // haze params
  bool bEnableHaze;// depth of field emulation through haze
  DWORD dwHazeColorTop;// haze color at the top of the area
  DWORD dwHazeColorBottom;// haze color at the bottom of the area
  DWORD dwGunTraceColor;// tracer color
  float fTraceLen;
  float fHazeHeight;// haze height
  // enables:
  bool bEnableUnits;// draw units
  bool bEnableObjects;// ...objects
  bool bEnableBBs;// bounding boxes
  bool bEnableEffects;// effects
  bool bEnableTerrain;// terrain
  bool bEnableNoise;// add noise to terrain
  bool bEnableShadows;// shadows
  bool bEnableGrid;// grid on the terrain
  bool bEnableWarFog;// fog'o'war
  bool bEnableDepthComplexity;// scene depth complexity. 
  bool bEnableShowBorder;// drawing stripes along the edges
  bool bShowUI;// show user interface
  // sprites drawing pipeline
  float fZBias;// vertical z-bias to keep z-buffer happy
  float fZBias2;// horizontal z-bias to keep z-buffer happy
  SHMatrix matTransform;// world => screen transformation matrix
  NTimer::STime tTransformUpdateTime;// last time of the transformation matrix update
  // CRAP{ // we need to come up with a lighting system
  SGFXLightDirectional sunlight;
  SGFXMaterial material;
  // CRAP}
  CPtr<IGFXTexture> pTrackTexture;// texture for tank marks
  bool AddSpriteObject(ISpriteVisObj *pObj, EObjGameType eGameType);
  bool AddMeshObject(IMeshVisObj *pObj, EObjGameType eGameType);
  bool AddEffectObject(IVisObj *pObj, EObjGameType eGameType);
  bool RemoveSpriteObject(ISpriteVisObj *pObj, EObjGameType eGameType);
  bool RemoveMeshObject(IMeshVisObj *pObj, EObjGameType eGameType);
  bool RemoveEffectObject(IVisObj *pObj, EObjGameType eGameType);
  bool RemoveOutboundObject(IVisObj *pObj, EObjGameType eGameType);

  template<class TYPE>
  bool AddObjectToArea(IVisObj *pObj, CAreaMap<TYPE> &area)
  {
    if (area.IsInArea(pObj->GetPosition()))
    {
      area.Add(static_cast<TYPE *>(pObj));
      return true;
    }
    objdescs.erase(pObj);
    pObj->AddRef();
    pObj->Release();
    return false;
  }

  // visibility
  using CPatchesList = std::list<std::pair<int, int>>;
  void SelectPatches(ICamera *pCamera, float fPatchesX, float fPatchesY, float fPatchSize, CPatchesList *pPatches);
  void SelectPatches2(const CVec3 &vCamera, const CVec2 &vCameraX, const CVec2 &vCameraY,
                      float fPatchesX, float fPatchesY, float fPatchSize, CPatchesList *pPatches);
  void FormVisibilityLists(ICamera *pCamera, ISceneVisitor *pVisitor);
  //
  void DrawSprites(CSpriteVisList &sprites);
  void DrawTerraObjects(CSpriteVisList &terraObjects);
  // particles
  void DrawParticles(const CParticlesVisList &particles);
  void DrawSingleParticlesPack(const CParticlesVisList &particles, int nNumParticles);
  // mech traces
  void DrawMechTraces(const std::list<SMechTrace> &traces);
  // gun traces
  void DrawGunTraces(const std::list<SGunTrace> &traces);
  void DrawMarkers();
  //
  void UpdateTransformMatrix();
  // set area maps size (in area cell units)
  void SetAreaMapSize(int nSizeX, int nSizeY);
  // set map size (in world units)
  void SetMapSize(int nSizeX, int nSizeY);
  //
  // void UpdateAttachedSounds( IVisObj *pObj, const CVec3 &vPos );
  // void RemoveAttachedSounds( IVisObj *pObj );
  void RandomizeRainDrop(SRainDrop &drop);
  void RandomizeSnowFlake(SSnowFlake &flake);
  void RandomizeSand(SSandParticle &particle);
  void UpdateWeather();
  void DrawRain();
  void DrawSnow();
  void DrawSand();

public:
  CScene();
  //
  bool STDCALL Init(ISingleton *pSingleton) override;
  //
  void STDCALL SetSeason(int nSeason) override;
  void STDCALL InitMapSounds(const struct CMapSoundInfo *pSound, int nElements) override;
  void STDCALL InitMusic(const std::string &szPartyName) override;
  void STDCALL InitTerrainSound(interface ITerrain *pTerrain) override;
  //
  void STDCALL SetTerrain(ITerrain *pTerrain) override;
  interface ITerrain * STDCALL GetTerrain() override { return pTerrain; }
  // add/remove/change position of the object
  bool STDCALL AddObject(IVisObj *pObject, EObjGameType eGameType, const SGDBObjectDesc *pDesc) override;
  bool STDCALL AddCraterObject(IVisObj *pObject, EObjGameType eGameType) override;
  bool STDCALL AddOutboundObject(IVisObj *pObject, EObjGameType eGameType) override;
  // CRAP { this is such crap because of coastal artillery
  bool STDCALL AddOutboundObject2(IVisObj *pObject, EObjGameType eGameType) override;
  // CRAP}
  void STDCALL AddMechTrace(const SMechTrace &trace) override;
  void STDCALL AddGunTrace(const SGunTrace &trace) override;

  bool STDCALL AddSceneObject(ISceneObject *pObject) override
  {
    CPtr<ISceneObject> pObj = pObject;
    RemoveSceneObject(pObject);
    alwaysObjects.push_back(pObject);
    return true;
  }

  bool STDCALL RemoveObject(IVisObj *pObject) override;

  bool STDCALL RemoveSceneObject(ISceneObject *pObject) override
  {
    if (pObject) alwaysObjects.remove(pObject);
    else alwaysObjects.clear();
    return true;
  }

  bool STDCALL MoveObject(IVisObj *pObject, const CVec3 &vPos) override;
  //
  bool STDCALL AddLine(IBoldLineVisObj *pLine) override
  {
    boldLines.push_back(pLine);
    return true;
  }

  bool STDCALL RemoveLine(IBoldLineVisObj *pLine) override
  {
    boldLines.remove(pLine);
    return true;
  }

  // set areas for fire ranges, zeroing, etc. 
  void STDCALL SetAreas(const SShootAreas *areas, int nNumAreas) override;
  void STDCALL GetAreas(struct SShootAreas **areas, int *pnNumAreas) override;
  // UI screen
  bool STDCALL AddUIScreen(interface IUIScreen *pUIScreen) override;
  bool STDCALL RemoveUIScreen(interface IUIScreen *pUIScreen) override;
  interface IUIScreen * STDCALL GetUIScreen() override;
  void STDCALL SetMissionScreen(interface IUIScreen *pMissionScreen) override;
  interface IUIScreen * STDCALL GetMissionScreen() override;

  // add/remove sound object

  void STDCALL SetSoundPos(WORD wID, const CVec3 &vPos) override;
  bool STDCALL IsSoundFinished(WORD wID) override;
  void STDCALL RemoveSound(WORD wID) override;
  WORD STDCALL AddSound(const char *pszName,
                        const CVec3 &vPos,
                        ESoundMixType eMixType,
                        ESoundAddMode eAddMode,
                        ESoundCombatType eCombatType = ESCT_GENERIC,
                        int nMinRadius = 0,
                        int nMaxRadius = 0,
                        unsigned int nTimeAfterStart = 0) override;

  WORD STDCALL AddSoundToMap(const char *pszName, const CVec3 &vPos) override;
  void STDCALL RemoveSoundFromMap(WORD wInstanceID) override;

  void STDCALL SetSoundSceneMode(enum ESoundSceneMode eSoundSceneMode) override;
  void STDCALL UpdateSound(interface ICamera *pCamera) override;
  void STDCALL CombatNotify() override;

  // additional objects
  int STDCALL AddMeshPair(IGFXVertices *pVertices, IGFXIndices *pIndices, IGFXTexture *pTexture, int nShadingEffect, bool bTemporary) override;
  int STDCALL AddMeshPair2(void *vertices, int nNumVertices, int nVertexSize, DWORD dwFormat,
                           WORD *indices, int nNumIndices, EGFXPrimitiveType ePrimitiveType,
                           IGFXTexture *pTexture, int nShadingEffect, bool bTemporary) override;
  bool STDCALL RemoveMeshPair(int nID) override;
  // CRAP{ fake object - circle for artillery reveal - remove, then minimap will be
  void STDCALL AddCircle(const CVec3 &vCenter, const float fRadius, const NTimer::STime &start, const NTimer::STime &duration) override
  {
    circles.push_back(SCircle());
    SCircle &circle = circles.back();
    circle.vCenter.Set(vCenter.x, vCenter.y);
    circle.fRadius = fRadius;
    circle.timeStart = start;
    circle.timeDuration = duration;
  }

  // CRAP}
  // tooltip
  void STDCALL SetToolTip(interface IText *pText, const CVec2 &vPos, const CTRect<float> &rcOut, DWORD dwColor = 0) override;
  // transfer UNIT to graveyard
  bool STDCALL TransferToGraveyard(IVisObj *pObject) override;
  // set visible objects
  void STDCALL SetVisibleObjects(IVisObj **ppObjects, int nNumObjects) override;
  void STDCALL SetWarFog(struct SAIVisInfo *pObjects, int nNumObjects) override;
  // remove all visual objects - clear scene
  void STDCALL Clear() override;
  // retrieve all objects from scene
  int STDCALL GetNumSceneObjects() const override;
  int STDCALL GetAllSceneObjects(std::pair<const SGDBObjectDesc *, CVec3> *pBuffer) const override;
  //
  IFrameSelection * STDCALL GetFrameSelection() override { return pFrameSelection; }
  IStatSystem * STDCALL GetStatSystem() override { return pStatSystem; }
  //
  void STDCALL Draw(ICamera *pCamera) override;
  // enables
  bool STDCALL ToggleShow(int nTypeID) override;
  // picking objects
  void STDCALL Pick(const CVec2 &point, std::pair<IVisObj *, CVec2> **ppObjects, int *pnNumObjects, EObjGameType type, bool bVisible) override;
  void STDCALL Pick(const CTRect<float> &rcRect, std::pair<IVisObj *, CVec2> **ppObjects, int *pnNumObjects, EObjGameType type, bool bVisible) override;
  // 3D <=> 2D position transforms
  void STDCALL GetPos3(CVec3 *pPos, const CVec2 &pos, bool bOnZero = false) override;
  void STDCALL GetPos2(CVec2 *pPos, const CVec3 &pos) override;
  void STDCALL GetScreenCoords(const CVec3 &pos, CVec3 *vScreen) override;
  // check object visibility
  bool IsVisible(IObjVisObj *pObj) const
  {
    if (bEnableWarFog) return pObj->IsVisible();
    return true;
  }

  const SVisObjDesc *GetDesc(IVisObj *pVisObj) const
  {
    auto pos = objdescs.find(pVisObj);
    return pos == objdescs.end() ? nullptr : &(pos->second);
  }

  void STDCALL SetDirectionalArrow(const CVec3 &vStart, const CVec3 &vEnd, bool bDraw) override;
  void STDCALL SetClickMarker(const CVec3 &vPos) override;
  void STDCALL SetPosMarker(const CVec3 &vPos) override;
  void STDCALL SetRotationStartAngle(float fAngle, bool bRotate = true) override;
  void STDCALL FlashPosMarkers() override;
  void STDCALL ResetPosMarkers() override;
  void STDCALL SwitchWeather(bool bOn) override;
  bool STDCALL IsRaining() override;
  void STDCALL SetWeatherQuality(float fCoeff) override;
  void STDCALL Reposition() override;
};

float Sin(float fAngle);
float Cos(float fAngle);

#endif // __SCENEINTERNAL_H__