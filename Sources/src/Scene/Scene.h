#ifndef __SCENE_H__
#define __SCENE_H__

#pragma once

#include "../Formats/fmtSprite.h"

enum
{
  SCENE_BASE_VALUE = 0x10060000,
  SCENE_SCENE = SCENE_BASE_VALUE + 1,
  SCENE_VISOBJ_BUILDER = SCENE_BASE_VALUE + 2,
  SCENE_CAMERA = SCENE_BASE_VALUE + 3,
  SCENE_CURSOR = SCENE_BASE_VALUE + 4,
  SCENE_VISOBJ_SPRITE = SCENE_BASE_VALUE + 5,
  SCENE_VISOBJ_MESH = SCENE_BASE_VALUE + 6,
  SCENE_VISOBJ_EFFECT = SCENE_BASE_VALUE + 7,
  SCENE_ICON_BAR = SCENE_BASE_VALUE + 8,
  SCENE_ICON_TEXT = SCENE_BASE_VALUE + 9,
  SCENE_ICON_PIC = SCENE_BASE_VALUE + 10,
  SCENE_BOLD_LINE = SCENE_BASE_VALUE + 11,
  SCENE_VISOBJ_SQUAD = SCENE_BASE_VALUE + 12,
  SCENE_ICON_HP_BAR = SCENE_BASE_VALUE + 13,
  SCENE_VISOBJ_FLASH = SCENE_BASE_VALUE + 14,
  SCENE_VIDEO_PLAYER = SCENE_BASE_VALUE + 15,
  SCENE_TRANSITION = SCENE_BASE_VALUE + 16,
  SCENE_GAMMA_EFFECT = SCENE_BASE_VALUE + 17,
  SCENE_GAMMA_FADER = SCENE_BASE_VALUE + 18,

  SCENE_EFFECTOR_RECOIL = SCENE_BASE_VALUE + 20,
  SCENE_EFFECTOR_JOGGING = SCENE_BASE_VALUE + 21,
  SCENE_SOUNDSCENE = SCENE_BASE_VALUE + 22,
  SCENE_SOUNDSCENE_SUBSTSOUND = SCENE_BASE_VALUE + 23,
  SCENE_SOUNDSCENE_SOUND = SCENE_BASE_VALUE + 24,
  SCENE_SOUNDSCENE_PLAYLIST = SCENE_BASE_VALUE + 25,
  SCENE_SOUNDSCENE_SOUNDCELL = SCENE_BASE_VALUE + 26,

  PFX_MANAGER = SCENE_BASE_VALUE + 27,
  PFX_KEYBASED = SCENE_BASE_VALUE + 28,
  PFX_KEYDATA = SCENE_BASE_VALUE + 29,

  SCENE_EFFECTOR_MATERIAL = SCENE_BASE_VALUE + 30,

  PFX_COMPLEX_KEYDATA = SCENE_BASE_VALUE + 31,
  PFX_COMPLEX_SOURCE = SCENE_BASE_VALUE + 32,

  SCENE_FORCE_DWORD = 0x7fffffff
};

// sound effect
// behavior with several sounds of the same type
enum ESoundMixType
{
  SFX_MIX_IF_TIME_EQUALS,
  SFX_MIX_SUBSTITUTE,
  SFX_MIX_ALWAYS,
  SFX_INTERFACE,

  SFX_MIX_ALL = 0x7fffffff,
};

// to set how the sound behaves during combat.
enum ESoundCombatType
{
  ESCT_GENERIC = 0,// does not fade out during combat and is not the sound of combat
  ESCT_COMBAT = 1,
  ESCT_MUTE_DURING_COMBAT = 2,

  ESCT_ASK_RPG = 3,// sound type is set in stats
  ESCT_ALL = 0x7fffffff,
};

enum ESoundSceneMode
{
  ESSM_INTERMISSION_INTERFACE,
  ESSM_INGAME,
};

enum EVisObjSelectionState
{
  SGVOSS_UNSELECTED = 0,
  SGVOSS_PRESELECTED = 1,
  SGVOSS_SELECTED = 2,

  SGVOSS_FORCE_DWORD = 0x7fffffff
};

enum ESceneObjectType
{
  SCENE_OBJECT_TYPE_CURSOR = 1,
  SCENE_OBJECT_TYPE_CAMERA = 2,
  SCENE_OBJECT_TYPE_FRAME_SELECTION = 3,
  SCENE_OBJECT_TYPE_STAT_SYSTEM = 4,
  SCENE_OBJECT_TYPE_ICON = 5,
  SCENE_OBJECT_TYPE_SQUAD = 6,

  SCENE_OBJECT_TYPE_FORCE_DWORD = 0x7fffffff
};

struct SMechTrace
{
  NTimer::STime birthTime;
  NTimer::STime deathTime;
  DWORD dwColor;
  CVec3 vCorners[4];
  CVec3 vPos;
  int nNumTracks;
  float alpha;
  //
  const CVec3 &GetPosition() const { return vPos; }
  bool operator==(const SMechTrace &obj) const { return birthTime == obj.birthTime && vPos == obj.vPos; }
};

struct SGunTrace
{
  NTimer::STime birthTime;
  NTimer::STime deathTime;
  CVec3 vPoints[4];
  CVec3 vDir;
  CVec3 vStart;
  //
  const CVec3 &GetPosition() const { return vStart; }
  bool operator==(const SGunTrace &obj) const { return birthTime == obj.birthTime && vStart == obj.vStart; }
};

struct SBasicSpriteInfo
{
  enum EType { TYPE_NORMAL_SPRITE, TYPE_COMPLEX_SPRITE };

  const EType type;
  CVec3 pos;// position in 3D space
  DWORD color, specular;// color(/w alpha) and specular(/w fog)
  interface IGFXTexture *pTexture;// sprite's texture
  CVec3 relpos;// relative screen position (one frame valid only!!!)
  DWORD dwCheckFlags;// screen check flags (low WORD) and priority (high WORD)
  //
  SBasicSpriteInfo(EType _type) : type(_type), pTexture(nullptr), dwCheckFlags(0) {}
};

struct SSpriteInfo : SBasicSpriteInfo
{
  CTRect<float> maps;// texture mapping coords
  CTRect<short> rect;// rect with respect to sprite's zero point
  float fDepthLeft;// left depth
  float fDepthRight;// right depth
  //
  SSpriteInfo() : SBasicSpriteInfo(TYPE_NORMAL_SPRITE), fDepthLeft(0), fDepthRight(0) {}
};

struct SComplexSpriteInfo : SBasicSpriteInfo
{
  const SSpritesPack::SSprite *pSprite;// complex sprite data
  //
  SComplexSpriteInfo() : SBasicSpriteInfo(TYPE_COMPLEX_SPRITE), pSprite(nullptr) {}
};

// ************************************************************************************************************************ //
// **
// ** scene visitor interface
// **
// **
// **
// ************************************************************************************************************************ //

interface ISceneVisitor : IRefCount
{
  // billboard sprite object
  virtual void VisitSprite(const SBasicSpriteInfo *pObj, int nType, int nPriority) = 0;
  // mesh object
  virtual void VisitMeshObject(interface IMeshVisObj *pObj, int nType, int nPriority) = 0;
  // particles
  virtual void VisitParticles(interface IParticleSource *pObj) = 0;
  // unknown scene object
  virtual void VisitSceneObject(interface ISceneObject *pObj) = 0;
  // text object
  virtual void VisitText(const CVec3 &vPos, const char *pszText, interface IGFXFont *pFont, DWORD color) = 0;
  // bold line object
  virtual void VisitBoldLine(CVec3 *corners, float fWidth, DWORD color) = 0;
  // mech trace object
  virtual void VisitMechTrace(const SMechTrace &trace) = 0;
  // gun trace object
  virtual void VisitGunTrace(const SGunTrace &trace) = 0;
  // UI elements visiting
  virtual void VisitUIRects(interface IGFXTexture *pTexture, int nShadingEffect, struct SGFXRect2 *rects, int nNumRects) = 0;
  virtual void VisitUIText(interface IGFXText *pText, const CTRect<float> &rcRect, int nY, DWORD dwColor, DWORD dwFlags) = 0;
  virtual void VisitUICustom(interface IUIElement *pElement) = 0;
};

// ************************************************************************************************************************ //
// **
// ** effectors
// **
// **
// **
// ************************************************************************************************************************ //

interface ISceneEffector : IRefCount
{
  virtual bool Update(const NTimer::STime &time) = 0;
  virtual void SetupTimes(const NTimer::STime &timeStart, const NTimer::STime &timeLife) = 0;
};

interface ISceneMatrixEffector : ISceneEffector
{
  virtual const SHMatrix & GetMatrix() const = 0;
};

interface ISceneEffectorRecoil : ISceneMatrixEffector
{
  virtual void SetupData(float fAngle, const CVec3 &vAxis) = 0;
};

interface ISceneEffectorJogging : ISceneMatrixEffector
{
  virtual void SetupData(float fWeightCoeff) = 0;
};

interface ISceneMaterialEffector : ISceneEffector
{
  virtual BYTE GetAlpha() const = 0;
  virtual DWORD GetSpecular() const = 0;
  virtual void SetupData(BYTE maxAlpha, DWORD maxSpecular) = 0;
};

// ************************************************************************************************************************ //
// **
// ** basic scene object
// **
// **
// **
// ************************************************************************************************************************ //

interface ISceneObject : IRefCount
{
  // update object
  virtual bool Update(const NTimer::STime &time, bool bForced = false) = 0;
  // drawing
  virtual bool Draw(interface IGFX *pGFX) = 0;
  // visiting
  virtual void Visit(interface ISceneVisitor *pVisitor, int nType = -1) = 0;
};

// ************************************************************************************************************************ //
// **
// ** icons
// **
// **
// **
// ************************************************************************************************************************ //

static constexpr DWORD ICON_ALIGNMENT_LEFT = 0x00000001;
static constexpr DWORD ICON_ALIGNMENT_HCENTER = 0x00000002;
static constexpr DWORD ICON_ALIGNMENT_RIGHT = 0x00000004;
static constexpr DWORD ICON_ALIGNMENT_TOP = 0x00000008;
static constexpr DWORD ICON_ALIGNMENT_VCENTER = 0x00000010;
static constexpr DWORD ICON_ALIGNMENT_BOTTOM = 0x00000020;
static constexpr DWORD ICON_PLACEMENT_VERTICAL = 0x00000040;
static constexpr DWORD ICON_PLACEMENT_HORIZONTAL = 0x00000080;
interface ISceneIcon : ISceneObject
{
  // position
  virtual void SetPosition(const CVec3 &vPos) = 0;
  virtual void Reposition(const CVec3 &vPos) = 0;
  virtual const CVec2 GetSize() = 0;
  // color
  virtual void SetColor(DWORD color) = 0;
  virtual void SetAlpha(BYTE alpha) = 0;
  // enable this icon
  virtual void Enable(bool bEnable) = 0;
};

interface ISceneIconBar : ISceneIcon
{
  // size and length percentage
  virtual void LockBarColor() = 0;
  virtual void UnlockBarColor() = 0;
  virtual void SetBorderColor(DWORD dwColor) = 0;
  virtual void ForceThinIcon() = 0;
  virtual void SetSize(const CVec2 &vSize, bool bHorizontal = true) = 0;
  virtual void SetLength(float fPercentage) = 0;
};

interface ISceneIconText : ISceneIcon
{
  virtual void SetFont(interface IGFXFont *pFont) = 0;
  virtual void SetText(const char *pszText) = 0;
};

interface ISceneIconPic : ISceneIcon
{
  virtual void SetTexture(interface IGFXTexture *pTexture) = 0;
  virtual void SetRect(const CTRect<short> &rect, const CTRect<float> &maps) = 0;
};

// ************************************************************************************************************************ //
// **
// ** vis objects
// **
// **
// **
// ************************************************************************************************************************ //

// direction is an angle of rotation around z-axis, direction is lied in the range [0..65536) = [0..2*pi)
interface IVisObj : ISceneObject
{
  // placement
  virtual void SetDirection(int nDirection) = 0;
  virtual void SetPosition(const CVec3 &pos) = 0;
  virtual void SetPlacement(const CVec3 &pos, int nDir) = 0;
  virtual const CVec3 & GetPosition() const = 0;
  virtual int GetDirection() const = 0;
  // opacity & color
  virtual void SetOpacity(BYTE opacity) = 0;
  virtual void SetColor(DWORD color) = 0;
  virtual void SetSpecular(DWORD color) = 0;
  // selection / selection test
  virtual void Select(EVisObjSelectionState state) = 0;
  virtual EVisObjSelectionState GetSelectionState() const = 0;
  virtual bool IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) = 0;
  virtual bool IsHit(const SHMatrix &matTransform, const RECT &rect) = 0;
};

interface IObjVisObj : IVisObj
{
  // scale
  virtual void SetScale(float sx, float sy, float sz) = 0;
  // scene game type
  virtual void SetGameType(DWORD dwType) = 0;
  // animations
  virtual void SetAnimation(int nAnim) = 0;
  virtual interface IAnimation * GetAnimation() = 0;
  // icons
  virtual void AddIcon(ISceneIcon *pIcon, int nID, const CVec3 &vAddValue, const CVec3 &vAddStep,
                               int nPriority, DWORD placement, bool bReposition = true) = 0;
  virtual void RemoveIcon(int nID, bool bReposition = true) = 0;
  virtual ISceneIcon * GetIcon(int nID) const = 0;
  // visibility
  virtual bool IsVisible() const = 0;
  virtual void SetVisible(bool bVisible) = 0;
  //
  virtual void SetPriority(int nPriority) = 0;
};

interface ISpriteVisObj : IObjVisObj
{
  virtual const SSpriteInfo * GetSpriteInfo() const = 0;
  virtual interface IGFXTexture * GetTexture() const = 0;
};

interface IMeshVisObj : IObjVisObj
{
  virtual bool DrawBB(interface IGFX *pGFX) = 0;
  virtual bool DrawShadow(interface IGFX *pGFX, const SHMatrix *pMatShadow, const CVec3 &vSunDir) = 0;
  //
  virtual void SetAnim(interface IAnimation *pAnim) = 0;
  virtual interface IGFXMesh * GetMesh() const =0;
  virtual interface IGFXTexture * GetTexture() const = 0;
  virtual const SHMatrix & GetPlacement() const = 0;
  virtual const SHMatrix & GetPlacement1() const = 0;
  virtual const SHMatrix GetBasePlacement() = 0;
  virtual const SHMatrix * GetMatrices() = 0;
  virtual const SHMatrix * GetExtMatrices(const SHMatrix &matExternal) = 0;
  //
  virtual DWORD CheckForViewVolume(const SPlane *pViewVolumePlanes) = 0;
  // effectors
  virtual void AddEffector(int nID, ISceneMatrixEffector *pEffector, int nPart = -1) = 0;
  virtual void RemoveEffector(int nID, int nPart = -1) = 0;
  virtual void AddMaterialEffector(ISceneMaterialEffector *pEffector) = 0;
  virtual void RemoveMaterialEffector() = 0;
};

interface IEffectVisObj : IVisObj
{
  virtual void SetStartTime(DWORD time) = 0;
  virtual void SetEffectDirection(const SHMatrix &matrix) = 0;
  virtual bool IsFinished(const NTimer::STime &time) = 0;
  virtual void CalibrateDuration(const NTimer::STime &timeDuration) = 0;
  virtual void Stop() = 0;
  virtual void SetSuspendedState(bool bState) = 0;
  // scale
  virtual void SetScale(float fScale) = 0;
  // data retrieving.
  virtual const std::string & GetSoundEffect() const = 0;
  // NOTE: all of this functions are uses temp buffer 0
  virtual void GetSpriteEffects(const SSpriteInfo ***ppEffects, int *pnNumEffects, bool bAll = false) = 0;
  virtual void GetParticleEffects(interface IParticleSource ***ppEffects, int *pnNumEffects, bool bAll = false) = 0;
};

interface IFlashVisObj : IVisObj
{
  virtual void Setup(const NTimer::STime &timeStart, const NTimer::STime &timeDuration, int nPower, DWORD dwColor) = 0;
};

interface IBoldLineVisObj : ISceneObject
{
  virtual void Setup(const CVec3 &vStart, const CVec3 &vEnd, float fWidth, DWORD color) = 0;
};

interface ISquadVisObj : ISceneObject
{
  struct SData
  {
    float fHealth;
    float fAmmo;
  };

  virtual void SetPosition(const CVec2 &vPos) = 0;
  virtual bool UpdateData(SData *pObjects, int nNumObjects) = 0;
  virtual bool ToggleSelection() = 0;
};

// ************************************************************************************************************************ //
// **
// ** frame selection
// **
// **
// **
// ************************************************************************************************************************ //

interface IFrameSelection : ISceneObject
{
  virtual void Begin(const CVec3 &point) = 0;
  virtual void End() = 0;
  virtual void Update(const CVec3 &point) = 0;
  virtual void Reset() = 0;
  //
  virtual CVec3 GetBeginPoint() = 0;
  virtual CVec3 GetEndPoint() = 0;
  virtual bool IsActive() = 0;
};

// ************************************************************************************************************************ //
// **
// ** statistics system
// **
// **
// **
// ************************************************************************************************************************ //

interface IStatSystem : ISceneObject
{
  // add/remove statistics entry
  virtual void AddEntry(const char *pszName) = 0;
  virtual void RemoveEntry(const char *pszName) = 0;
  // update entry
  virtual void UpdateEntry(const char *pszName, double val) = 0;
  virtual void UpdateEntry(const char *pszName, const char *pszVal) = 0;
  virtual void ResetEntry(const char *pszName) = 0;
  // position
  virtual void SetPosition(int nX, int nY) = 0;
};

// ************************************************************************************************************************ //
// **
// **camera
// **
// **
// **
// ************************************************************************************************************************ //

interface ICamera : IRefCount
{
  // type ID
  enum { tidTypeID = SCENE_CAMERA };

  //
  virtual void Init(ISingleton *pSingleton) = 0;
  virtual void SetBounds(int x1, int y1, int x2, int y2) = 0;
  // placement functions
  virtual void SetPlacement(const CVec3 &vAnchor, float fDist, float fPitch, float fYaw) = 0;
  virtual void SetAnchor(const CVec3 &_vAnchor) = 0;
  virtual const SHMatrix GetPlacement() const = 0;
  virtual const CVec3 GetPos() const = 0;
  virtual const CVec3 GetAnchor() = 0;
  // last unchanged position
  virtual void GetLastPos(CVec3 *pvPos, NTimer::STime *pTime) const = 0;
  // scrolling
  virtual void ResetSliders() = 0;
  virtual void SetScrollSpeedX(float fSpeed) = 0;
  virtual void SetScrollSpeedY(float fSpeed) = 0;
  // eqrthquake
  virtual void AddEarthquake(const CVec3 &vPos, float fPower) = 0;
  //
  virtual void Update() = 0;
};

// ************************************************************************************************************************ //
// **
// ** cursor
// **
// **
// **
// ************************************************************************************************************************ //

interface ICursor : ISceneObject
{
  // type ID
  enum { tidTypeID = SCENE_CURSOR };

  //
  enum EUpdateMode
  {
    UPDATE_MODE_WINDOWS = 1,
    UPDATE_MODE_INPUT = 2,
  };

  //
  virtual void Init(ISingleton *pSingleton) = 0;
  virtual void Done() = 0;
  virtual void Clear() = 0;
  virtual void SetUpdateMode(EUpdateMode _eUpdateMode) = 0;
  virtual void OnSetCursor() = 0;
  //
  virtual void RegisterMode(int nMode, const char *pszPictureName, int nSizeX, int nSizeY, int hotX, int hotY, WORD wResourceID) = 0;
  virtual bool SetMode(int nMode) = 0;
  virtual bool SetModifier(int nMode) = 0;
  virtual void Show(bool bShow) = 0;
  virtual bool IsShown() const = 0;

  virtual void SetBounds(int x1, int y1, int x2, int y2) = 0;
  virtual void Acquire(bool bAcqire) = 0;
  virtual void LockPos(bool bLock) = 0;
  virtual void SetPos(int nX, int nY) = 0;
  virtual const CVec2 GetPos() = 0;
  virtual void ResetSliders() = 0;
  // last unchanged position
  virtual void GetLastPos(CVec2 *pvPos, NTimer::STime *pTime) const = 0;
  //
  virtual void SetSensitivity(float fSensitivity) = 0;
};

// ************************************************************************************************************************ //
// **
// **video player
// **
// **
// **
// ************************************************************************************************************************ //

interface IVideoPlayer : ISceneObject
{
  enum
  {
    PLAY_FROM_MEMORY = 0x00000001,
    PLAY_FROM_HANDLE = 0x00000002,
    PLAY_WITH_ALPHA = 0x00000004,
    PLAY_INFINITE = 0x00000008,
    PLAY_LOOPED = 0x00000010,
    COPY_ALL = 0x00000020
  };

  // setup target to render video to. 
  virtual void SetTarget(interface IGFXTexture *pTexture, interface IGFX *pGFX) = 0;
  // set destination rect to render to
  virtual void SetDstRect(const RECT &rcDstRect, bool bMaintainAspect) = 0;
  // set loop mode
  virtual void SetLoopMode(bool bLooped) = 0;
  // playing position
  virtual int GetCurrentFrame() const = 0;
  virtual bool SetCurrentFrame(int nFrame) = 0;
  // shading effect
  virtual void SetShadingEffect(int nEffect, bool bStart) = 0;
  // playing capabilities
  // function Play returns movie length in milliseconds
  virtual int Play(const char *pszFileName, DWORD dwFlags, interface IGFX *pGFX, interface ISFX *pSFX) = 0;
  virtual bool Stop() = 0;
  virtual bool Pause(bool bPause) = 0;
  virtual bool IsPlaying() const = 0;
  // movie statistics:
  virtual int GetLength() const = 0;
  virtual int GetNumFrames() const = 0;
  virtual bool GetMovieSize(CVec2 *pSize) const = 0;
};

// ************************************************************************************************************************ //
// **
// ** transition screen
// **
// **
// **
// ************************************************************************************************************************ //

interface ITransition : ISceneObject
{
  virtual int Start(const char *pszVideoName, DWORD dwAddFlags, const NTimer::STime &currTime, bool bFadeIn) = 0;
};

interface IGammaEffect : ISceneObject
{
  virtual void Init(float fGammaR, float fGammaG, float fGammaB,
                            const NTimer::STime &timeStart, const NTimer::STime &timeDuration) = 0;
};

// ************************************************************************************************************************ //
// **
// ** main scene interface
// **
// **
// **
// ************************************************************************************************************************ //

enum
{
  SCENE_SHOW_HAZE = 0,
  SCENE_SHOW_UNITS = 1,
  SCENE_SHOW_OBJECTS = 2,
  SCENE_SHOW_BBS = 3,
  SCENE_SHOW_SHADOWS = 4,
  SCENE_SHOW_EFFECTS = 5,
  SCENE_SHOW_TERRAIN = 6,
  SCENE_SHOW_GRID = 7,
  SCENE_SHOW_WARFOG = 8,
  SCENE_SHOW_DEPTH_COMPLEXITY = 9,
  SCENE_SHOW_UI = 10,
  SCENE_SHOW_NOISE = 11,
  SCENE_SHOW_BORDER = 12,

  SCENE_SHOW_FORCE_DWORD = 0x7fffffff
};

// ************************************************************************************************************************ //
// ** to add sound to the scene
// ************************************************************************************************************************ //

enum ESoundAddMode
{
  SAM_LOOPED_NEED_ID,// The sound ID will return, the sound will be looped
  SAM_NEED_ID,// sound ID will be returned
  SAM_ADD_N_FORGET,// returns 0, the scene itself will remove the sound.
};

interface IScene : IRefCount
{
  // type ID
  enum { tidTypeID = SCENE_SCENE };

  //
  virtual bool Init(ISingleton *pSingleton) = 0;
  //
  virtual void SetSeason(int nSeason) = 0;
  virtual void InitMusic(const std::string &szPartyName) = 0;
  virtual void InitMapSounds(const struct CMapSoundInfo *pSound, int nElements) =0;
  // to generate sounds from the terrain
  virtual void InitTerrainSound(interface ITerrain *pTerrain) = 0;
  // NOTE: terrain MUST BE loaded before this operation!!!
  // NOTE: if terrain's size was changed, call SetTerrain() again
  virtual void SetTerrain(interface ITerrain *pTerrain) = 0;
  virtual interface ITerrain * GetTerrain() = 0;
  // add/remove visual objects
  virtual bool AddObject(IVisObj *pObject, EObjGameType eGameType, const SGDBObjectDesc *pDesc = nullptr) = 0;
  virtual bool AddCraterObject(IVisObj *pObject, EObjGameType eGameType) = 0;
  virtual bool AddOutboundObject(IVisObj *pObject, EObjGameType eGameType) = 0;
  // CRAP { this is such crap because of coastal artillery
  virtual bool AddOutboundObject2(IVisObj *pObject, EObjGameType eGameType) = 0;
  // CRAP}
  virtual void AddMechTrace(const SMechTrace &trace) = 0;
  virtual void AddGunTrace(const SGunTrace &trace) = 0;
  virtual bool AddSceneObject(ISceneObject *pObject) = 0;
  virtual bool RemoveObject(IVisObj *pObject) = 0;
  virtual bool RemoveSceneObject(ISceneObject *pObject) = 0;
  virtual bool MoveObject(IVisObj *pObject, const CVec3 &vPos) = 0;
  virtual bool AddUIScreen(interface IUIScreen *pUIScreen) = 0;
  virtual bool RemoveUIScreen(interface IUIScreen *pUIScreen) = 0;
  virtual interface IUIScreen * GetUIScreen() = 0;
  virtual void SetMissionScreen(interface IUIScreen *pMissionScreen) = 0;
  virtual interface IUIScreen * GetMissionScreen() = 0;
  virtual bool AddLine(IBoldLineVisObj *pLine) = 0;
  virtual bool RemoveLine(IBoldLineVisObj *pLine) = 0;
  // set areas for fire ranges, zeroing, etc. 
  virtual void SetAreas(const struct SShootAreas *areas, int nNumAreas) = 0;
  virtual void GetAreas(struct SShootAreas **areas, int *pnNumAreas) = 0;
  // add/remove sound object


  virtual void SetSoundPos(WORD wID, const CVec3 &vPos) = 0;
  virtual bool IsSoundFinished(WORD wID) = 0;
  virtual void RemoveSound(WORD wID) = 0;
  virtual WORD AddSound(const char *pszName,
                                const CVec3 &vPos,
                                ESoundMixType eMixType,
                                ESoundAddMode eAddMode,
                                ESoundCombatType eCombatType = ESCT_GENERIC,
                                int nMinRadius = 0,
                                int nMaxRadius = 0,
                                unsigned int nTimeAfterStart = 0) = 0;

  virtual WORD AddSoundToMap(const char *pszName, const CVec3 &vPos) = 0;
  virtual void RemoveSoundFromMap(WORD wInstanceID) = 0;
  virtual void UpdateSound(interface ICamera *pCamera) = 0;
  virtual void CombatNotify() =0;
  virtual void SetSoundSceneMode(enum ESoundSceneMode eSoundSceneMode) = 0;

  // additional objects
  virtual int AddMeshPair(interface IGFXVertices *pVertices, interface IGFXIndices *pIndices, interface IGFXTexture *pTexture, int nShadingEffect, bool bTemporary) = 0;
  virtual int AddMeshPair2(void *vertices, int nNumVertices, int nVertexSize, DWORD dwFormat,
                                   WORD *indices, int nNumIndices, enum EGFXPrimitiveType ePrimitiveType,
                                   IGFXTexture *pTexture, int nShadingEffect, bool bTemporary) = 0;
  virtual bool RemoveMeshPair(int nID) = 0;
  // CRAP{ fake object - circle for artillery reveal - remove, then minimap will be
  virtual void AddCircle(const CVec3 &vCenter, float fRadius, const NTimer::STime &start, const NTimer::STime &duration) = 0;
  // CRAP}
  // tooltip
  virtual void SetToolTip(interface IText *pText, const CVec2 &vPos, const CTRect<float> &rcOut, DWORD dwColor = 0) = 0;
  // transfer UNIT to graveyard
  virtual bool TransferToGraveyard(IVisObj *pObject) = 0;
  // set visible objects
  virtual void SetVisibleObjects(IVisObj **ppObjects, int nNumObjects) = 0;
  virtual void SetWarFog(struct SAIVisInfo *pObjects, int nNumObjects) = 0;
  // remove all visual objects - clear scene
  virtual void Clear() = 0;
  // retrieve all objects from scene
  virtual int GetNumSceneObjects() const = 0;
  virtual int GetAllSceneObjects(std::pair<const SGDBObjectDesc *, CVec3> *pBuffer) const = 0;
  //
  virtual IFrameSelection * GetFrameSelection() = 0;
  virtual IStatSystem * GetStatSystem() = 0;
  //
  virtual void Draw(interface ICamera *pCamera) = 0;
  // enables
  virtual bool ToggleShow(int nTypeID) = 0;
  // picking objects
  // NOTE: this functions are using 'temp buffer 0'
  virtual void Pick(const CVec2 &point, std::pair<IVisObj *, CVec2> **ppObjects, int *pnNumObjects, EObjGameType type, bool bVisible = true) = 0;
  virtual void Pick(const CTRect<float> &rcRect, std::pair<IVisObj *, CVec2> **ppObjects, int *pnNumObjects, EObjGameType type, bool bVisible = true) = 0;
  // 3D <=> 2D position transforms
  virtual void GetPos3(CVec3 *pPos, const CVec2 &pos, bool bOnZero = false) = 0;
  virtual void GetPos2(CVec2 *pPos, const CVec3 &pos) = 0;
  virtual void GetScreenCoords(const CVec3 &pos, CVec3 *vScreen) = 0;

  virtual void SetDirectionalArrow(const CVec3 &vStart, const CVec3 &vEnd, bool bDraw) = 0;
  virtual void SetClickMarker(const CVec3 &vPos) = 0;
  virtual void SetPosMarker(const CVec3 &vPos) = 0;
  virtual void SetRotationStartAngle(float fAngle, bool bRotate = true) = 0;
  virtual void FlashPosMarkers() = 0;
  virtual void ResetPosMarkers() = 0;

  virtual void SwitchWeather(bool bOn) = 0;

  virtual bool IsRaining() = 0;
  virtual void SetWeatherQuality(float fCoeff) = 0;

  virtual void Reposition() = 0;
};

// ************************************************************************************************************************ //
// **
// ** visualization objects builder
// **
// **
// **
// ************************************************************************************************************************ //

interface IVisObjBuilder : IRefCount
{
  // type ID
  enum { tidTypeID = SCENE_VISOBJ_BUILDER };

  //
  virtual bool Init(ISingleton *pSingleton) = 0;
  //
  virtual IVisObj * BuildObject(const char *pszName, const char *pszName2, EObjVisType type) = 0;
  virtual ISceneObject * BuildSceneObject(const char *pszName, ESceneObjectType eType, int nSubtype = -1) = 0;
  virtual const char * GetEffectSound(const std::string &szName) = 0;
  virtual bool ChangeObject(IVisObj *pObj, const char *pszModelName, const char *pszTextureName, EObjVisType type) = 0;
  virtual bool ChangeSceneObject(ISceneObject *pObj, const char *pszName, ESceneObjectType eType, int nSubtype = -1) = 0;
  virtual void Clear() = 0;
};

// ************************************************************************************************************************ //
// **
// ** scene helper functions
// **
// **
// **
// ************************************************************************************************************************ //

namespace NScene
{
  template<class TYPE>
  TYPE *BuildObject(IVisObjBuilder *pVOB, const char *pszName, EObjVisType eVisType) { return static_cast<TYPE *>(pVOB->BuildObject(pszName, eVisType)); }
};

#endif // __SCENE_H__