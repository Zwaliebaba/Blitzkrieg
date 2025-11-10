#ifndef __SOUNDSCENE_H__
#define __SOUNDSCENE_H__


#include "../SFX/SFX.h"
#include "../Misc/2DArray.h"
#include "../Formats/fmtMap.h"
#include "../Misc/TypeConvertor.h"
#include "../Misc/FreeIDs.h"

#include "CellsConglomerateContainer.h"

struct SSoundSceneConsts
{
  static int SS_SOUND_CELL_SIZE;// side length per serving
  static NTimer::STime SS_MIX_DELTA;// maximum time difference
  // appearance of sounds with SFX_MIX_IF_TIME_EQUALS
  static NTimer::STime SS_UPDATE_PERIOD;// in milliseconds
  static NTimer::STime SS_SOUND_DIM_TIME;// sound decay time when removed
  static NTimer::STime SS_COMBAT_MUSIC_PLAY_WO_NOTIFY;// How long should a truce last before the combat music stops?
  static NTimer::STime SS_COMBAT_MUSIC_FADE;// decay time of combat music in milliseconds.

  static NTimer::STime SS_STREAMING_SILENT_PAUSE;// to calculate the silence time after Combat before starting
  static NTimer::STime SS_STREAMING_SILENT_PAUSE_RND;// IDLE
  static NTimer::STime SS_IDLE_PAUSE;

  static NTimer::STime SS_AMBIENT_SOUND_CHANGE_RANDOM;// for changing looped sounds from time to time
  static NTimer::STime SS_AMBIENT_SOUND_CHANGE;// for changing not looped sounds from time to time
  static int SS_AMBIENT_TERRAIN_SOUNDS;// number of playing terrain sounds simultaneously
  static float TERRAIN_SOUND_RADIUS_MIN;// in percent of screen size
  static float TERRAIN_SOUND_RADIUS_MAX;
  static float TERRAIN_CRITICAL_WEIGHT;
  static float DEFAULT_SCREEN_WIDTH;
  static float DEFAULT_SCREEN_HEIGHT;
  static int MAP_SOUND_CELL;

  static int TERRAIN_NONCYCLED_SOUNDS_MIN_RADIUS;
  static int TERRAIN_NONCYCLED_SOUNDS_MAX_RADIUS;

  static NTimer::STime MAP_SOUNDS_UPDATE;

  static float COMBAT_MUSIC_VOLUME;
  static float IDLE_MUSIC_VOLUME;
  static float COMBAT_SOUNDR_FEAR_RADIUS;
  static float COMBAT_FEAR_TIME;

  static NTimer::STime SS_MAP_SOUND_PERIOND;
  static NTimer::STime SS_MAP_SOUND_PERIOND_RANDOM;

  static int MIN_SOUND_COUNT_TO_PLAY_LOOPED;
};

// for sounds registered at a map point
class CMapSounds
{
  DECLARE_SERIALIZE;

  using RegisteredSounds = CTypeConvertor<std::string, WORD>;

  //
  class CMapSoundCell
  {
    DECLARE_SERIALIZE;

    // for similar map sounds
    struct SMapSounds
    {
      DECLARE_SERIALIZE;

    public:
      std::hash_map<WORD, CVec2> instanceIDs;
      int nCount;
      SMapSounds() : nCount(0) {}
    };

    struct SMaxCountPredicate
    {
      bool operator()(const std::pair<WORD, SMapSounds> &s1, const std::pair<WORD, SMapSounds> &s2) const { return s1.second.nCount > s2.second.nCount; }
    };

    struct SPlaying
    {
      WORD wSoundTypeID;// type of sound
      WORD wInstanceID;// instance of sound
      WORD wSceneID;// if added to scene, then scene ID
      //
      SPlaying() { Clear(); }

      void Clear()
      {
        wInstanceID = 0;
        wSceneID = 0;
        wSoundTypeID = 0;
      }
    };

    SPlaying playingLoopedSound;// current playing sound type (loop)
    SPlaying playingSound;// current unlooped sound

    // lists by sound type
    using CellSounds = std::hash_map<WORD, SMapSounds>;
    CellSounds cellSounds;
    CellSounds cellLoopedSounds;
    NTimer::STime timeNextRun;// next sound time

    void RemoveSound(CellSounds *pCellSounds, WORD wInstanceID);

  public:
    CMapSoundCell() : timeNextRun(0) {}

    void AddSound(WORD wSoundID, const CVec2 &vPos, const RegisteredSounds &registeredSounds, WORD wInstanceID, bool bLooped);
    void RemoveSound(WORD wInstanceID, class CSoundScene *pScene);
    void Update(class CSoundScene *pScene, const RegisteredSounds &registeredSounds);
  };

  CFreeIds soundIDs;// for recording sounds
  CFreeIds instanceIDs;// each sound will have a unique ID
  RegisteredSounds registeredSounds;// list of names of sounds that are in the scene

  // 2d map of sound cells
  CArray2D<CMapSoundCell> mapCells;
  // cell - sound instance id
  std::hash_map<WORD, SIntPair> cells;

  CSoundScene *pSoundScene;
  NTimer::STime timeNextUpdate;

public:
  CMapSounds() : pSoundScene(nullptr), timeNextUpdate(0) {}
  void SetSoundScene(class CSoundScene *pSoundScene);
  void Update(interface ICamera *pCamera);
  void Clear();

  void InitSizes(int nSizeX, int nSizeY);
  WORD AddSound(const CVec2 &vPos, const char *szName);
  void RemoveSound(WORD wInstanceID);
};

class CSoundScene : public IRefCount
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CSoundScene);

public:
  static const NTimer::STime &GetCurTime();// so as not to transmit everywhere
  // to determine whether the sound lies within the map
  static bool IsInBounds(int x, int y);
  static IObjectsDB *GetObjectDB();

  // to store a list of sounds and issue them on request
  class CPlayList : public IRefCount
  {
    OBJECT_COMPLETE_METHODS(CPlayList);
    DECLARE_SERIALIZE;
    std::vector<std::string> melodies;
    int nIter;

  public:
    CPlayList() : nIter(0) {}
    void Reset();
    void Clear();
    void Shuffle();
    void AddMelody(const std::string &pszFileName);
    const char *GetNextMelody();
    const bool IsEmpty() const { return melodies.empty(); }
  };

  // for sounds coming from Terrain and from extended objects.
  class CTerrainSounds
  {
    DECLARE_SERIALIZE;

  public:
    // for sound storage and processing
    class CTerrainSound
    {
      DECLARE_SERIALIZE;

      struct SSoundInfo
      {
        DECLARE_SERIALIZE;

      public:
        CPtr<ISound> pSound;
        bool bPeaceful;
        SSoundInfo() {}

        SSoundInfo(CPtr<ISound> pSound, bool bPeaceful)
          : pSound(pSound), bPeaceful(bPeaceful) {}
      };

      using CCycledSounds = std::list<SSoundInfo>;
      CVec3 vSoundPos;// placement of nonCycleSound

      CCycledSounds cycledSounds;// cycle sounds from that terrain

      float fVolume;
      float fPan;
      CVec2 vOffset;// offset of this sound from camera
      NTimer::STime timeRestart;// time when sound must be restarted
      bool bMustPlay;
      bool bNeedUpdate;
      WORD wSound;

    public:
      CTerrainSound() : vSoundPos(VNULL3), fVolume(0.0f),
                        fPan(0.0f),
                        vOffset(VNULL2), timeRestart(0),
                        bMustPlay(false), bNeedUpdate(false), wSound(0) {}

      // returns true if update of sounds is needed.
      void SetSound(const char *pszName, NTimer::STime timeWhenRestart);
      bool HasCycleSound() const { return !cycledSounds.empty(); }
      void AddCycledSound(const char *szName, interface ISoundManager *pSoundManager);
      void StartCycledSounds(ISFX *pSFX, bool bNonPeacefulOnly);

      NTimer::STime GetRestartTime() { return timeRestart; }
      // if the camera position has changed IsNeedUpdate will return true
      void Update(const SSoundTerrainInfo &info,
                  const CVec3 &vCameraAnchor, const CVec2 &vScreenSize, float fRelativeVolume);
      void SetMustPlay(bool _bMustPlay);
      bool IsMustPlay() const { return bMustPlay; }

      bool IsNeedUpdate() const { return bNeedUpdate; }
      void DoUpdate(ISFX *pSFX);

      void StopSounds(ISFX *pSFX, bool bOnlyPeaceful);
    };

  private:
    CPtr<ISoundManager> pSoundManager;
    CPtr<ISFX> pSFX;
    CVec3 vCameraAncor;// to determine weather coordinates changed
    // for every terrain there will be separate sound
    using CSounds = std::map<BYTE, CTerrainSound>;
    CSounds terrainSounds;
    CPtr<ITerrain> pTerrain;// to get terrain sounds
    NTimer::STime lastUpdateTime;
    CVec2 vScreen;// screen sizes
    bool bMuteAll;// mute terrain sounds
  public:
    CTerrainSounds() : vCameraAncor(CVec3(-1, -1, -1)), lastUpdateTime(0), bMuteAll(false) {}
    void Init(interface ITerrain *pTerrain);
    void Update(interface ICamera *pCamera, bool bCombat);
    void Mute(bool bMute);
    void Clear();
  };

private:
  // interactive music
  class CStreamingSounds
  {
    // interrupt sounds at markers, start at segments.
    // for a short period without sound - forget it.
    DECLARE_SERIALIZE;

    // for load
    using CMusicFiles = std::vector<std::string>;

    struct SMusicSettings
    {
      CMusicFiles combat;
      CMusicFiles explore;

      int operator&(IDataTree &ss)
      {
        CTreeAccessor saver = &ss;
        saver.Add("Combat", &combat);
        saver.Add("Explore", &explore);
        return 0;
      }
    };

    using CMusicSettingsList = std::hash_map<std::string/* Party Name */, SMusicSettings>;

    enum EStreamingSoundsState
    {
      ESSS_IDLE,
      ESSS_FADE_IDLE,
      ESSS_IDLE_PAUSE,
      ESSS_START_COMBAT,
      ESSS_COMBAT,
      ESSS_COMBAT_FADE,
      ESSS_COMBAT_RESTORE_VOLUME,
      ESSS_START_COMBAT_AFTER_LOAD,
      ESSS_PAUSE,
    };

    CPtr<ISFX> pSFX;
    EStreamingSoundsState eState;
    CPtr<IGameTimer> pGameTimer;

    bool bCombatNotify;// turn on the battalion commander
    NTimer::STime timeLastCombatNotify;// time of the last segment with fighting
    NTimer::STime timeLastUpdate;//
    NTimer::STime timeDesiredPause;// counted pause between Combat and Idle

    CPtr<CPlayList> pIdle;// 
    CPtr<CPlayList> pCombat;// 

    void StartIdleMusic();
    void StartCombatMusic();
    void Init(class CPlayList *pIdle, class CPlayList *pCombat);

  public:
    CStreamingSounds();

    void Clear();
    // 
    void Init(const std::string &szPartyName);
    // launch (or continuation) of combat music
    void CombatNotify();
    // 
    void Update();
  };

  // for Hash table
  struct SIntPairHash
  {
    size_t operator()(const SIntPair &v) const { return v.x * RAND_MAX + v.y; }
  };

  struct SUpdatedCell
  {
    int nFormerRadius;
    int nNewRadius;
    SIntPair vCell;

    SUpdatedCell(const SIntPair &vCell, const int nFormerRadius, const int nNewRadius)
      : nFormerRadius(nFormerRadius), nNewRadius(nNewRadius), vCell(vCell) {}
  };

  using CUpdatedCells = std::list<SUpdatedCell>;

public:
  // after using a sound, it ends playing it. 
  // sounds - they should only end when no one is playing them
  class CSubstSound : public IRefCount
  {
    OBJECT_NORMAL_METHODS(CSubstSound);
    DECLARE_SERIALIZE;
    CPtr<ISFX> pSFX;
    CPtr<ISound> pSample;

  public:
    CSubstSound() {}
    CSubstSound(ISound *pSound, ISFX *pSFX) : pSFX(pSFX), pSample(pSound) {}
    virtual ~CSubstSound() { if (pSFX) pSFX->StopSample(pSample); }
    ISound *GetSound() { return pSample; }
  };

  // sound
  class CSound : public IRefCount
  {
    OBJECT_NORMAL_METHODS(CSound);
    DECLARE_SERIALIZE;
    WORD wID;// 
    CPtr<ISound> pSample;// sound to play
    CPtr<CSubstSound> pSubstitute;// sound that is played instead of pSample

    ESoundCombatType eCombatType;// sound behavior during combat
    NTimer::STime timeBegin;// start of sound playback.
    NTimer::STime timeToPlay;// sound start time
    NTimer::STime timeBeginDim;

    std::string szName;
    ESoundMixType eMixType;
    CVec3 vPos;
    bool bLooped;

    int nMaxRadius, nMinRadius;

    bool bStartedMark;// the sound has already started
    bool bFinishedMark;// the sound has already played
    bool bDimMark;// start sound fading
  public:
    CSound() {}
    CSound(WORD wID,
           const std::string &szName,
           interface ISound *pSound,
           enum ESoundMixType eMixType,
           const CVec3 &vPos,
           bool bLooped,
           ESoundCombatType eCombatType,
           float fMinRadius,
           float fMaxRadius);
    virtual ~CSound();

    // how many samples have already been played?
    unsigned int GetSamplesPassed();
    bool IsTimeToFinish();

    // time this sound plays
    NTimer::STime GetPlayTime() const { return timeToPlay; }

    // sound replacement
    bool IsSubstituted() const { return pSubstitute != nullptr; }
    CSubstSound *GetSubst();
    void Substitute(CSubstSound *_pSubstitute, NTimer::STime nStartTime);
    void UnSubstitute();

    bool IsLooped() const { return bLooped; }

    // the beginning of this sound
    void MarkStarted();
    bool IsMarkedStarted() const { return bStartedMark; }
    // end of audio playback
    bool IsMarkedFinished() const { return bFinishedMark; }
    void MarkFinished(bool bFinished = true) { bFinishedMark = bFinished; }
    // for sound attenuation when moving away
    void MarkToDim(NTimer::STime time);
    bool IsMarkedForDim() const { return bDimMark; }
    // Due to attenuation (time) or distance, the volume may not be at full volume.
    float GetVolume(NTimer::STime time, float fDist) const;


    void SetBeginTime(NTimer::STime time);
    const NTimer::STime &GetBeginTime() const { return timeBegin; }


    int GetRadiusMax() const;// distance (in cells) at which this sound is heard
    ISound *GetSound();
    ESoundMixType GetMixType() const { return eMixType; }
    const WORD GetID() { return wID; }
    const std::string &GetName() const;

    void SetPos(const class CVec3 &vPos);
    const CVec3 &GetPos() const { return vPos; }

    ESoundCombatType GetCombatType() const { return eCombatType; }
  };

  // cell containing sound
  class CSoundCell : public IRefCount
  {
    DECLARE_SERIALIZE;
    OBJECT_COMPLETE_METHODS(CSoundCell);
    int nRadius;// radius of sound of this cell (in cells)
    // std::list< SIntPair > hearableCells;	
    using CSounds = std::list<CPtr<CSound>>;
    CSounds sounds;// the sounds of this cage
    void RecountForDelete();
    NTimer::STime timeLastCombatHear;
    bool IsSoundHearable(const CSound *pSound, int nRadius) const;

  public:
    CSoundCell();
    void Clear();

    int GetRadius() const { return nRadius; }
    void SetRadius(int nRad) { nRadius = nRad; }
    void AddSound(class CSound *pSound);// adds sound and recalculates sound radius
    void RemoveSound(WORD wID, ISFX *pSFX = nullptr);// removes sound and recalculates sound radius
    CSound *GetSound(WORD wID);

    // remove sounds with ID == 0 that have completed
    // Mark all completed sounds as completed
    void Update(ISFX *pSFX);

    // access to the list of audible cells.
    // void AddHearCell( const SIntPair &cell);
    // void RemoveHearCell( const SIntPair &cell);
    // replace cell registration with new
    // void ReplaceHearCell( const SIntPair &vFormerCell, const SIntPair &vNewCell );


    bool HasSounds() const { return sounds.begin() != sounds.end(); }
    // bool HearSounds() const { return hearableCells.begin() != hearableCells.end(); 

    void SetLastHearCombat(NTimer::STime hearTime);
    bool IsCombat() const;


    // for all sounds that are heard at a distance greater than nRadius and have not yet finished playing
    template<class TEnumFunc>
    void EnumHearableSounds(int nRadius, TEnumFunc func)
    {
      for (auto it = sounds.begin(); it != sounds.end(); ++it) { if (IsSoundHearable(*it, nRadius)) func(*it); }
    }

    // to cycle through all sounds.
    template<class TEnumFunc>
    void EnumAllSounds(TEnumFunc func, int nRadius)
    {
      if (nRadius <= GetRadius())
      {
        for (auto it = sounds.begin(); it != sounds.end(); ++it)
        {
          if (IsSoundHearable(*it, nRadius)) func(*it);
          else func(*it, false);
        }
      }
      else
      {
        // this means none of the sounds of this cell are heard
        for (auto it = sounds.begin(); it != sounds.end(); ++it) func(*it, false);
      }
    }
  };

  using CSamplesList = std::list<CPtr<ISound>>;
  using CSoundsList = std::list<CPtr<CSound>>;
  using CHearableSounds = std::hash_map<std::string/* subst name */, CSoundsList>;
  using CSoundSubstTable = std::hash_map<std::string/* sound name */, std::string/* subst name */>;

  // typedef std::hash_map< SIntPair, CSoundCell, SIntPairHash > CSoundCells;
  using CSoundCellsInBounds = CArray2D<CPtr<CSoundCell>>;
  using CSoundCellsOutOfBounds = std::hash_map<SIntPair, CPtr<CSoundCell>, SIntPairHash>;
  using CSoundCellsWithSound = std::hash_map<SIntPair, CPtr<CSoundCell>, SIntPairHash>;

  // to collect sounds that are heard in the cage and sort them by
  // to the sounds that replace them
  class CSoundsCollector
  {
    CSoundSubstTable &substTable;
    CHearableSounds &sounds;
    CSoundCellsWithSound &cellsWSound;
    CSoundsList &muteSounds;

  public:
    CSoundsCollector(CSoundSubstTable &substTable, CHearableSounds &sounds, CSoundCellsWithSound &cellsWSound, CSoundsList &muteSounds)
      : substTable(substTable), sounds(sounds), cellsWSound(cellsWSound), muteSounds(muteSounds) {}

    void operator()(int nRadius, const SIntPair &vCell);
    void operator()(class CSound *sound, bool bHearable = true);

    void CalcMuteSounds();
  };

private:
  enum ESoundSceneMode eSoundSceneMode;
  CFreeIds freeIDs;// sound ID table
  std::hash_map<WORD, SIntPair> soundIDs;// in which cell is the sound located?

  ISFX *pSFX;
  ISoundManager *pSoundManager;
  IGameTimer *pGameTimer;
  static IObjectsDB *pObjectsDB;

  CUpdatedCells updatedCells;// for intermediate storage

  NTimer::STime timeLastUpdate;
  SIntPair vFormerCameraCell;// to track camera movement
  static SIntPair vLimit;// maximum card sizes

  CSoundSubstTable substTable;// sound replacement table

  CStreamingSounds streamingSounds;

  CHearableSounds interfaceSounds;// sounds from the interface
  std::hash_set<int> finishedInterfaceSounds;
  std::hash_set<int> deletedInterfaceSounds;

  CTerrainSounds terrainSounds;
  CMapSounds mapSounds;

  CSoundCellsInBounds soundCellsInBounds;// sounds on the map
  CSoundCellsOutOfBounds soundCellsOutOfBounds;// sounds behind the scenes (mainly airplanes)
  CSoundCellsWithSound soundCellsWithSound;// list of all cells with sounds
  CVec2 vScreenResize;

  CCellsConglomerateContainer cellsPHS;
  static NTimer::STime curTime;// so as not to transmit everywhere

  // helper functions
  CSoundCell *GetSoundCell(const CSoundScene::SIntPair &vCell);

  void AddSound(const SIntPair &vCell, CSound *s);
  void To2DSoundPos(const CVec3 &vPos, CVec3 *pv2DPos);
  // ordinary update
  void UpdatePHSMap(const SIntPair &vCell, int nFormerRadius, int nNewRadius);

  void UpdateCombatMap(const CSoundScene::SIntPair &vCell, CSound *pSound);
  void CalcVolNPan(float *fVolume, float *fPan, const CVec3 &vSound, float fMaxHear);
  void MuteSounds(CSoundsList *muteSounds);

  void MixInterfaceSounds();
  void MixMixedAlways(CHearableSounds &sounds, const CVec3 &vCameraPos);
  void MixSingle(CHearableSounds &sounds, const CVec3 &vCameraPos);
  void MixMixedWithDelta(CHearableSounds &sounds, const CVec3 &vCameraPos);
  void Mix(CSoundsList &curSounds,
           CSoundsList::iterator begin_iter,
           CSoundsList::iterator end_iter,
           const std::string &substName,
           const CVec3 &vCameraPos,
           ESoundMixType eMixType,
           int nMixMinimum,
           bool bDelete = true);


  // to sort sounds by start time
  class CSoundStartTimePredicate
  {
  public:
    bool operator()(CSound *one, CSound *two) { return one->GetBeginTime() < two->GetBeginTime(); }
  };

  // to search for sounds whose beginning lies no further than timeDelta
  // from timeBase
  class CSoundsWithinDeltaPredicate
  {
    NTimer::STime timeToCompare;

  public:
    CSoundsWithinDeltaPredicate(NTimer::STime timeBase, NTimer::STime timeDelta)
      : timeToCompare(timeBase + timeDelta) {}

    bool operator()(const CSound *sound) { return sound->GetBeginTime() >= timeToCompare; }
  };

  void InitConsts();

public:
  CSoundScene();
  void Clear();

  void Init(int nMaxX, int nMaxY);
  void InitScreenResolutionConsts();// must be called after all screen resolution changes

  void InitTerrain(interface ITerrain *pTerrain);
  void InitMap(const struct CMapSoundInfo *pSound, int nElements);
  /* void InitMusic( const char **ppszCombat, const int nCombatSize,
                   */
  void InitMusic(const std::string &szPartyName);

  void SetTerrain(interface ITerrain *pTerrain) { terrainSounds.Init(pTerrain); }
  // if a battle has started or is ongoing, then call this function
  void CombatNotify();

  void MuteTerrain(bool bMute);

  WORD AddSound(const char *pszName,
                const CVec3 &vPos,
                enum ESoundMixType eMixMode,
                enum ESoundAddMode eAddMode,
                enum ESoundCombatType eCombatType,
                int nMinRadius,
                int nMaxRadius,
                unsigned int nTimeAfterStart = 0);

  // removes sound from the scene. 
  void RemoveSound(WORD wID);
  // gives a new position to the sound.
  void SetSoundPos(WORD wID, const class CVec3 &vPos);

  bool IsFinished(WORD wID);

  void Update(interface ICamera *pCamera);

  void SetMode(enum ESoundSceneMode eSoundSceneMode);
  enum ESoundSceneMode GetMode() { return eSoundSceneMode; };

  WORD AddSoundToMap(const char *pszName, const CVec3 &vPos);
  void RemoveSoundFromMap(WORD wInstanceID);
};

#endif // __SOUNDSCENE_H__