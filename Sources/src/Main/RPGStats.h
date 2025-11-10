#ifndef __RPGSTATS_H__
#define __RPGSTATS_H__

#pragma once

#include "../AILogic/AIConsts.h"
#include "../Misc/BitData.h"
#include "../StreamIO/RandomGen.h"
#include <zlib.h>
#include "iMain.h"

inline int GetRandom(int nAverage, int nRandom) { return nRandom <= 0 ? nAverage : RandomCheck(nAverage - nRandom, nAverage + nRandom); }
inline float GetRandom(float fAverage, int nRandom) { return nRandom <= 0 ? fAverage : RandomCheck(fAverage - static_cast<float>(nRandom), fAverage + static_cast<float>(nRandom)); }
inline int GetPositiveRandom(int nAverage, int nRandom) { return Max(0, GetRandom(nAverage, nRandom)); }
inline float GetPositiveRandom(float fAverage, int nRandom) { return Max(0.0f, GetRandom(fAverage, nRandom)); }

// ************************************************************************************************************************ //
// **
// ** effects
// **
// **
// **
// ************************************************************************************************************************ //

struct SFlashEffect
{
  int nPower;
  int nDuration;
  //
  SFlashEffect() : nPower(-1), nDuration(0) {}
  const bool HasFlash() const { return (nPower > 0) && (nDuration > 10); }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;
    saver.Add("Power", &nPower);
    saver.Add("Duration", &nDuration);
    return 0;
  }

};

// ************************************************************************************************************************ //
// **
// ** user actions (64 bits)
// **
// **
// **
// ************************************************************************************************************************ //

class CUserActions
{
  DWORD actions[2];

public:
  CUserActions() { Clear(); }

  CUserActions(const DWORD _actions[2])
  {
    actions[0] = _actions[0];
    actions[1] = _actions[1];
  }

  CUserActions(const CUserActions &userActions)
  {
    actions[0] = userActions.actions[0];
    actions[1] = userActions.actions[1];
  }

  //
  const CUserActions &operator=(const DWORD _actions[2])
  {
    actions[0] = _actions[0];
    actions[1] = _actions[1];
    return *this;
  }

  const CUserActions &operator=(const CUserActions &_actions) { return this->operator=(_actions.actions); }
  //
  bool operator==(const DWORD _actions[2]) const { return (_actions[0] == actions[0]) && (_actions[1] == actions[1]); }
  bool operator==(const CUserActions &_actions) const { return this->operator==(_actions.actions); }
  bool operator!=(const DWORD _actions[2]) const { return (_actions[0] != actions[0]) || (_actions[1] != actions[1]); }
  bool operator!=(const CUserActions &_actions) const { return this->operator!=(_actions.actions); }
  //
  void operator|=(const CUserActions &ua)
  {
    actions[0] |= ua.actions[0];
    actions[1] |= ua.actions[1];
  }

  void operator&=(const CUserActions &ua)
  {
    actions[0] &= ua.actions[0];
    actions[1] &= ua.actions[1];
  }

  //
  void Clear() { actions[0] = actions[1] = 0; }
  bool IsEmpty() const { return (actions[0] | actions[1]) == 0; }
  //
  const bool HasAction(const int nAction) const
  {
    NI_ASSERT_SLOW_T((nAction >= 0) && (nAction <= 63), NStr::Format("Invalid action %d must be in [0..63]", nAction));
    const int nIndex = nAction >> 5;
    return actions[nIndex] & (1UL << (nAction - nIndex * 32));
  }

  void SetAction(const int nAction)
  {
    NI_ASSERT_SLOW_T((nAction >= 0) && (nAction <= 63), NStr::Format("Invalid action %d must be in [0..63]", nAction));
    const int nIndex = nAction >> 5;
    actions[nIndex] |= (1UL << (nAction - nIndex * 32));
  }

  void RemoveAction(const int nAction)
  {
    NI_ASSERT_SLOW_T((nAction >= 0) && (nAction <= 63), NStr::Format("Invalid action %d must be in [0..63]", nAction));
    const int nIndex = nAction >> 5;
    actions[nIndex] &= ~(1UL << (nAction - nIndex * 32));
  }

  //
  DWORD GetActions(const int nIndex) const { return actions[nIndex]; }

  void GetActions(DWORD *_actions) const
  {
    _actions[0] = actions[0];
    _actions[1] = actions[1];
  }

  void GetActions(CUserActions *pActions) const { GetActions(pActions->GetBuffer()); }

  void SetActions(const DWORD _actions[2])
  {
    actions[0] = _actions[0];
    actions[1] = _actions[1];
  }

  DWORD *GetBuffer() { return &(actions[0]); }
};

// ************************************************************************************************************************ //
// **
// ** common RPG stats (base)
// **
// **
// **
// ************************************************************************************************************************ //

struct SCommonRPGStats : IGDBObject
{
private:
  bool bCheckSumInitialized;
  uLong checkSum;// stats checksum
public:
  std::string szKeyName;// key name of this object - mainly used in the editor
  std::string szParentName;// parent object key name. 
  std::string szStatsType;// stat type - "crap", "mech", "infantry", "building", "weapon"
  //
  SCommonRPGStats() : bCheckSumInitialized(false) {}
  SCommonRPGStats(const char *pszStatsType) : bCheckSumInitialized(false), szStatsType(pszStatsType) {}
  SCommonRPGStats(const std::string &_szStatsType) : bCheckSumInitialized(false), szStatsType(_szStatsType) {}
  virtual ~SCommonRPGStats() {}

  const char * STDCALL GetName() const override { return szStatsType.c_str(); }
  const char * STDCALL GetParentName() const override { return szParentName.c_str(); }

  // convert from human units to AI
  virtual void STDCALL ToAIUnits() {}
  //
  virtual void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) {}
  // checking stats for correctness
  virtual bool STDCALL Validate() { return true; }
  //
  const uLong STDCALL GetCheckSum() const override
  {
    NI_ASSERT_T(bCheckSumInitialized, "Check sum hasn't been initialized");
    return checkSum;
  }

  //
  virtual const uLong STDCALL GetCheckSum()
  {
    if (bCheckSumInitialized) return checkSum;
    bCheckSumInitialized = true;
    return (checkSum = CalculateCheckSum());
  }

  virtual int STDCALL operator&(IDataTree &ss);

  virtual const uLong STDCALL CalculateCheckSum() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** basic RPG interfaces
// **
// **
// **
// ************************************************************************************************************************ //

// directions
// do not change the numbering of directions!!!
enum EArmorDirection
{
  RPG_FRONT = 0,
  RPG_LEFT = 1,
  RPG_BACK = 2,
  RPG_RIGHT = 3,
  RPG_TOP = 4,
  RPG_BOTTOM = 5,
};

struct SDefenseRPGStats
{
  int nArmorMin;// min armor strength
  int nArmorMax;// max armor strength
  float fSilhouette;// silhuettee for strike probability
  //
  SDefenseRPGStats() : nArmorMin(40), nArmorMax(90), fSilhouette(1.0f) {}
  //
  int operator&(IDataTree &ss);
};

struct SHPObjectRPGStats : SCommonRPGStats
{
  float fMaxHP;// object's maximum health
  std::vector<float> damagedHPs;// sequence of health percentages at which the object’s state changes
  float fRepairCost;// repair cost (in RU) to repair ONE hit point!!!
  SDefenseRPGStats defences[6];// defense stats for each direction
  //
  SHPObjectRPGStats(const char *pszType) :
    SCommonRPGStats(pszType), fMaxHP(100), fRepairCost(1) {}

  ~SHPObjectRPGStats() override {}

  //
  void STDCALL ToAIUnits() override;
  //
  float GetMapHP() const { return fMaxHP; }
  float GetHP(const float fHPPercentage) const { return fMaxHP * fHPPercentage; }

  int GetDamagedState(const float fHPPercentage) const
  {
    int i = damagedHPs.size();
    while (--i >= 0) { if (fHPPercentage > damagedHPs[i]) return i; }
    return 0;
  }

  virtual int GetArmor(const int n) const { return (defences[n].nArmorMin + defences[n].nArmorMax) / 2; }
  virtual int GetMinPossibleArmor(const int n) const { return defences[n].nArmorMin; }
  virtual int GetMaxPossibleArmor(const int n) const { return defences[n].nArmorMax; }
  virtual int GetRandomArmor(const int n) const { return RandomCheck(defences[n].nArmorMin, defences[n].nArmorMax); }
  //
  virtual const CUserActions *GetUserActions(bool bActionsBy) const { return nullptr; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

enum EAIClass
{
  AI_CLASS_WHEEL = 0x00000001,
  AI_CLASS_HALFTRACK = 0x00000002,
  AI_CLASS_TRACK = 0x00000004,
  AI_CLASS_HUMAN = 0x00000008,

  AI_CLASS_TECHNICS = AI_CLASS_WHEEL | AI_CLASS_HALFTRACK | AI_CLASS_TRACK,
  AI_CLASS_ANY = AI_CLASS_TECHNICS | AI_CLASS_HUMAN,

  AI_CLASS_FORCE_DWORD = 0x7fffffff
};

struct SStaticObjectRPGStats : SHPObjectRPGStats
{
  DWORD dwAIClasses;// AI classes, which cannot go through this object
  bool bBurn;// does the building burn after reaching 50% hp
  std::string szEffectExplosion;// death with explosion (projectile hit)
  std::string szEffectDeath;// "silent" death - squished by tank

  SStaticObjectRPGStats(const char *pszType);
  ~SStaticObjectRPGStats() override {}
  //
  virtual const CVec2 & STDCALL GetOrigin(int nIndex = -1) const = 0;
  virtual const CArray2D<BYTE> & STDCALL GetPassability(int nIndex = -1) const = 0;
  virtual const CVec2 & STDCALL GetVisOrigin(int nIndex = -1) const = 0;
  virtual const CArray2D<BYTE> & STDCALL GetVisibility(int nIndex = -1) const = 0;
  //
  void STDCALL ToAIUnits() override;
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** weapon and base gun stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SWeaponRPGStats : SCommonRPGStats
{
  struct SShell
  {
    enum ETrajectoryType
    {
      TRAJECTORY_LINE = 0,
      TRAJECTORY_HOWITZER = 1,
      TRAJECTORY_BOMB = 2,
      TRAJECTORY_CANNON = 3,
      TRAJECTORY_ROCKET = 4,
      TRAJECTORY_GRENADE = 5
    };

    enum EDamageType
    {
      DAMAGE_HEALTH = 0,// ordinary shell with physical damage
      DAMAGE_MORALE = 1,// morale shell - does damage to unit's morale
      DAMAGE_FOG = 2,// fog screen shell
    };

    // shell type parameters
    EDamageType eDamageType;
    int nPiercing;// (cucumbers) armor penetration
    int nPiercingRandom;// (cucumbers) random for armor penetration
    float fDamagePower;// (HP <=> HP) actually, harm...
    int nDamageRandom;// (HP <=> HP) random for harm
    float fArea, fArea2;// (meters <=> AI points) radius of the coverage area from one projectile
    float fSpeed;// (meters/second <=> AI points/tick) projectile flight speed
    float fTraceSpeedCoeff;// coefficient 
    float fTraceProbability;// probability of a tracer appearing when fired [0;1]
    float fDetonationPower;// degree of camera shake when a projectile explodes
    ETrajectoryType trajectory;// trajectory type
    float fBrokeTrackProbability;// probability of breaking the truck
    // parameters for visualizing and voicing effects
    std::string szFireSound;// the sound of an infantryman firing
    std::string szEffectGunFire;// shot from a cannon with this shell
    std::string szEffectTrajectory;// projectile flight (smoke, etc.)
    std::string szEffectHitDirect;// direct hit
    std::string szEffectHitMiss;// hit visually, but missed the combat system
    std::string szEffectHitReflect;// hit, but did not penetrate the armor
    std::string szEffectHitGround;// hit the ground
    std::string szEffectHitWater;// got into the water
    std::string szEffectHitAir;// hit the air - for anti-aircraft artillery during barrage fire
    std::vector<std::string> szCraters;// craters after explosion
    SFlashEffect flashFire;// flash on firing
    SFlashEffect flashExplosion;// flash on explosion

    union
    {
      float fFireRate;
      int nFireRate;
    };// (bullets/minute <=> ticks between bullets in the queue) rate of fire
    union
    {
      float fRelaxTime;
      int nRelaxTime;
    };// (seconds <=> ticks) recovery time after a shot

    CArray1Bit specials;// specialist. 
    //
    SShell();
    //
    float GetRandomDamage() const { return GetPositiveRandom(fDamagePower, nDamageRandom); }
    int GetRandomPiercing() const { return GetPositiveRandom(nPiercing, nPiercingRandom); }
    int GetMaxPossiblePiercing() const { return nPiercing + nPiercingRandom; }
    int GetMinPossiblePiercing() const { return Max(0, nPiercing - nPiercingRandom); }
    const bool HasCraters() const { return !szCraters.empty(); }
    const std::string &GetRandomCrater() const { return szCraters[rand() % szCraters.size()]; }
    // convert from human units to AI
    bool ToAIUnits();
    //
    int operator&(IDataTree &ss);

    const uLong CalculateCheckSum() const;
  };

  //
  float fDispersion;// (meters <=> AI points) "falling" (according to Tolstoy)
  // NOTE{ aiming - shoot in burst (nAmmoPerBurst*fFireRate) - relax. 
  union
  {
    float fAimingTime;
    int nAimingTime;
  };// (seconds <=> ticks) time to aim
  // NOTE}
  int nAmmoPerBurst;// (pieces <=> pieces) how many rounds goes into a queue
  float fRangeMax;// (meters <=> AI points) how far it hits
  float fRangeMin;// (meters <=> AI points) closer cannot shoot

  // CRAP{ belongs to the gun, not the weapon itself
  int nCeiling;// (levels <=> levels) how much the cannon shoots up
  // CRAP}
  float fRevealRadius;// detection radius for anti-artillery warfare (??? <=> AI points)

  WORD wDeltaAngle;// (degrees <=> degrees65535) how much the gunner can bend the gun by force of his will. 
  std::vector<SShell> shells;// all possible types of shells for this gun
  //
  SWeaponRPGStats();
  ~SWeaponRPGStats() override {}

  void STDCALL ToAIUnits() override;
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

struct SBaseGunRPGStats
{
  std::string szWeapon;// the gun itself (link)
  // gun priority: 0 - you need to stop to shoot, > 0 - everything else, no need to stop
  int nPriority;// priority of this gun
  bool bPrimary;// primary or secondary gun (to display projectiles)
  const SWeaponRPGStats *pWeapon;// weapon shortcut
  int nAmmo;// max amount of ammo in this gun
  WORD wDirection;// this gun direction
  float fReloadCost;// reload cost (in RU) to reload ONE shell of any type for this gun
  //
  SBaseGunRPGStats();
  //
  virtual bool STDCALL RetrieveShortcuts(IObjectsDB *pGDB);

  //
  virtual int STDCALL operator&(IDataTree &ss);

  //
  virtual const uLong STDCALL CalculateCheckSum() const;
};

// ************************************************************************************************************************ //
// **
// **static object stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SObjectBaseRPGStats : SStaticObjectRPGStats
{
  CArray2D<BYTE> passability;// passability of AI tiles of this object
  CVec2 vOrigin;// object zero point for passability
  CArray2D<BYTE> visibility;// degree of gaze absorption
  CVec2 vVisOrigin;// object's zero point by visibility
  //
  std::string szAmbientSound;// ambient sounds set
  std::string szCycledSound;// cycled sounds set

  //
  SObjectBaseRPGStats(const char *pszType);
  ~SObjectBaseRPGStats() override {}
  //
  void STDCALL ToAIUnits() override;
  //
  const CVec2 & STDCALL GetOrigin(const int nIndex = -1) const override { return vOrigin; }
  const CArray2D<BYTE> & STDCALL GetPassability(const int nIndex = -1) const override { return passability; }

  const CVec2 & STDCALL GetVisOrigin(const int nIndex = -1) const override { return vVisOrigin; }
  const CArray2D<BYTE> & STDCALL GetVisibility(const int nIndex = -1) const override { return visibility; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

struct STerraObjSetRPGStats : SStaticObjectRPGStats
{
  struct SSegment
  {
    CArray2D<BYTE> passability;// passability of AI tiles of this object
    CVec2 vOrigin;// object zero point for passability
    CArray2D<BYTE> visibility;// degree of gaze absorption
    CVec2 vVisOrigin;// object's zero point by visibility
    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  //
  std::vector<SSegment> segments;// all available segments
  std::vector<int> fronts;// segments to use as a front and side
  std::vector<int> backs;// segments to use as a back
  //
  STerraObjSetRPGStats() : SStaticObjectRPGStats("TerraObj") {}
  ~STerraObjSetRPGStats() override {}
  //
  void STDCALL ToAIUnits() override;
  //
  const CVec2 & STDCALL GetOrigin(const int nIndex = -1) const override { return segments[nIndex].vOrigin; }
  const CArray2D<BYTE> & STDCALL GetPassability(const int nIndex = -1) const override { return segments[nIndex].passability; }

  const CVec2 & STDCALL GetVisOrigin(const int nIndex = -1) const override { return segments[nIndex].vVisOrigin; }
  const CArray2D<BYTE> & STDCALL GetVisibility(const int nIndex = -1) const override { return segments[nIndex].visibility; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

struct SObjectRPGStats : SObjectBaseRPGStats
{
  SObjectRPGStats() : SObjectBaseRPGStats("ObjDesc") {}
  ~SObjectRPGStats() override {}
  //
  int STDCALL operator&(IDataTree &ss) override;
};

struct SBuildingRPGStats : SObjectBaseRPGStats
{
  enum EType
  {
    TYPE_BULDING = 0,
    TYPE_MAIN_RU_STORAGE = 1,
    TYPE_TEMP_RU_STORAGE = 2,
    TYPE_DOT = 3
  };

  // entrance description
  struct SEntrance
  {
    CVec3 vPos;// entrance placement
    bool bStormable;// is this entrance stormable?

    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  // shoot slot description
  struct SSlot
  {
    CVec3 vPos;// position
    union
    {
      float fDirection;
      WORD wDirection;
    };// (angle <=> angle65536) fire/sight direction
    union
    {
      float fAngle;
      WORD wAngle;
    };// (angle <=> angle65536) fire/sight angle
    SHMatrix matDirection;// direction (as wDirection), but ready for visualization purposes
    float fSightMultiplier;// sight multiplier
    float fCoverage;// coverage for the unit in this slot [0..1]
    //
    SBaseGunRPGStats gun;// mounted gun
    union
    {
      float fRotationSpeed;
      WORD wRotationSpeed;
    };// (seconds per revolution <=> degrees65535/tick) vertical rotation speed
    // vis info
    bool bBeforeSprite;// is this slot before main sprite?
    bool bShowFlashes;// do we need to show flashes from this position?
    CVec2 vPicturePosition;// 2D picture position
    CVec3 vWorldPosition;// 3D position, calculated from picture one
    //
    SSlot();

    bool ToAIUnits();
    bool Validate();
    bool RetrieveShortcuts(IObjectsDB *pGDB);
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  // fire point description, fire in a building
  struct SFirePoint
  {
    CVec3 vPos;// position
    float fDirection;// (angle <=> angle in radians) direction
    float fVerticalAngle;// (angle <=> angle in radians) angle from horizontal plane in the vertical plane, which crosses 'fDirection'
    std::string szFireEffect;// particle fire effect
    // vis info
    CVec2 vPicturePosition;// 2D picture position
    CVec3 vWorldPosition;// 3D position, calculated from picture one
    //
    SFirePoint();

    bool ToAIUnits();
    bool HasFireEffect() const { return !szFireEffect.empty(); }
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  // direction explosion description, explosion in the direction of fire
  struct SDirectionExplosion
  {
    CVec3 vPos;// position
    float fDirection;// (angle <=> angle in radians) direction
    float fVerticalAngle;// (angle <=> angle in radians) angle from horizontal plane in the vertical plane, which crosses 'fDirection'
    // vis info
    CVec2 vPicturePosition;// 2D picture position
    CVec3 vWorldPosition;// 3D position, calculated from picture one
    //
    SDirectionExplosion();

    bool ToAIUnits();
    int operator&(IDataTree &ss);
  };

  //
  EType eType;// type of this building (see enum above for possible types)
  //
  int nRestSlots;// slots for resting
  int nMedicalSlots;// slots for medical assistance
  const SBaseGunRPGStats *pPrimaryGun;// primary gun (mounted) for data extracting for stats
  //
  std::vector<SSlot> slots;// shoot slots...
  std::vector<SEntrance> entrances;// entrances
  std::vector<SFirePoint> firePoints;// fire points...
  std::vector<SFirePoint> smokePoints;// smoke points when a building is destroyed
  std::string szSmokeEffect;// smoke effect, one for all
  //
  enum EDirectionExplosionType
  {
    // do not change these IDs
    E_FRONT_LEFT,
    E_FRONT_RIGHT,
    E_BACK_RIGHT,
    E_BACK_LEFT,
    E_TOP_CENTER,
  };

  std::vector<SDirectionExplosion> dirExplosions;// direction explosions, 5 in total, see EDirectionExplosionType
  std::string szDirExplosionEffect;// direction explosion effect, one for all
  //
  SBuildingRPGStats();
  ~SBuildingRPGStats() override {}
  //
  void STDCALL ToAIUnits() override;
  bool STDCALL Validate() override;
  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;

  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** unit RPG types and classes
// **
// **
// **
// ************************************************************************************************************************ //

enum EUnitRPGType
{
  // main types
  RPG_TYPE_INFANTRY = 0x00010000,
  RPG_TYPE_TRANSPORT = 0x00020000,
  RPG_TYPE_ARTILLERY = 0x00040000,
  RPG_TYPE_SPG = 0x00080000,
  RPG_TYPE_ARMOR = 0x00100000,
  RPG_TYPE_AVIATION = 0x00200000,
  RPG_TYPE_TRAIN = 0x00400000,
  // infantry
  RPG_TYPE_SOLDIER = 0x00010001,
  RPG_TYPE_ENGINEER = 0x00010002,
  RPG_TYPE_SNIPER = 0x00010003,
  RPG_TYPE_OFFICER = 0x00010004,
  // transport
  RPG_TYPE_TRN_CARRIER = 0x00020001,
  RPG_TYPE_TRN_SUPPORT = 0x00020002,
  RPG_TYPE_TRN_MEDICINE = 0x00020003,
  RPG_TYPE_TRN_TRACTOR = 0x00020004,
  RPG_TYPE_TRN_MILITARY_AUTO = 0x00020005,
  RPG_TYPE_TRN_CIVILIAN_AUTO = 0x00020006,
  // artillery
  RPG_TYPE_ART_GUN = 0x00040001,
  RPG_TYPE_ART_HOWITZER = 0x00040002,
  RPG_TYPE_ART_HEAVY_GUN = 0x00040003,
  RPG_TYPE_ART_AAGUN = 0x00040004,
  RPG_TYPE_ART_ROCKET = 0x00040005,
  RPG_TYPE_ART_SUPER = 0x00040006,
  RPG_TYPE_ART_MORTAR = 0x00040007,
  RPG_TYPE_ART_HEAVY_MG = 0x00040008,
  // SPG
  RPG_TYPE_SPG_ASSAULT = 0x00080001,
  RPG_TYPE_SPG_ANTITANK = 0x00080002,
  RPG_TYPE_SPG_SUPER = 0x00080003,
  RPG_TYPE_SPG_AAGUN = 0x00080004,
  // armor
  RPG_TYPE_ARM_LIGHT = 0x00100001,
  RPG_TYPE_ARM_MEDIUM = 0x00100002,
  RPG_TYPE_ARM_HEAVY = 0x00100003,
  RPG_TYPE_ARM_SUPER = 0x00100004,
  // aviation
  RPG_TYPE_AVIA_SCOUT = 0x00200001,
  RPG_TYPE_AVIA_BOMBER = 0x00200002,
  RPG_TYPE_AVIA_ATTACK = 0x00200003,
  RPG_TYPE_AVIA_FIGHTER = 0x00200004,
  RPG_TYPE_AVIA_SUPER = 0x00200005,
  RPG_TYPE_AVIA_LANDER = 0x00200006,
  // train
  RPG_TYPE_TRAIN_LOCOMOTIVE = 0x00400001,
  RPG_TYPE_TRAIN_CARGO = 0x00400002,
  RPG_TYPE_TRAIN_CARRIER = 0x00400003,
  RPG_TYPE_TRAIN_SUPER = 0x00400004,
  RPG_TYPE_TRAIN_ARMOR = 0x00400005
};

inline EUnitRPGType GetMainType(EUnitRPGType type) { return static_cast<EUnitRPGType>(type & 0xffff0000); }
inline bool IsInfantry(EUnitRPGType type) { return (type & RPG_TYPE_INFANTRY) != 0; }
inline bool IsTransport(EUnitRPGType type) { return (type & RPG_TYPE_TRANSPORT) != 0; }
inline bool IsArtillery(EUnitRPGType type) { return (type & RPG_TYPE_ARTILLERY) != 0; }
inline bool IsSPG(EUnitRPGType type) { return (type & RPG_TYPE_SPG) != 0; }
inline bool IsArmor(EUnitRPGType type) { return (type & RPG_TYPE_ARMOR) != 0; }
inline bool IsAviation(EUnitRPGType type) { return (type & RPG_TYPE_AVIATION) != 0; }
inline bool IsTrain(EUnitRPGType type) { return (type & RPG_TYPE_TRAIN) != 0; }

enum EUnitRPGClass
{
  RPG_CLASS_UNKNOWN = 0,
  RPG_CLASS_ARTILLERY = 1,
  RPG_CLASS_TANK = 2,
  RPG_CLASS_SNIPER = 3,
  RPG_CLASS_FORCE_DWORD = 0x7fffffff
};

inline const EUnitRPGClass GetRPGClass(const EUnitRPGType eType)
{
  switch (eType)
  {
    case RPG_TYPE_ART_GUN:
    case RPG_TYPE_ART_AAGUN:
    case RPG_TYPE_ART_ROCKET:
    case RPG_TYPE_ART_HOWITZER:
    case RPG_TYPE_ART_HEAVY_GUN:
    case RPG_TYPE_ART_SUPER:
      return RPG_CLASS_ARTILLERY;
    case RPG_TYPE_SPG_SUPER:
    case RPG_TYPE_SPG_AAGUN:
    case RPG_TYPE_SPG_ASSAULT:
    case RPG_TYPE_SPG_ANTITANK:
    case RPG_TYPE_ARM_LIGHT:
    case RPG_TYPE_ARM_MEDIUM:
    case RPG_TYPE_ARM_SUPER:
    case RPG_TYPE_ARM_HEAVY:
      return RPG_CLASS_TANK;
    case RPG_TYPE_SNIPER:
      return RPG_CLASS_SNIPER;
  }
  return RPG_CLASS_UNKNOWN;
}

class CRPGStatsAutomagic : public IRPGStatsAutomagic
{
  OBJECT_COMPLETE_METHODS(CRPGStatsAutomagic);

  using CI2SMap = std::hash_map<int, std::string>;
  using CS2IMap = std::hash_map<std::string, int>;
  //
  CI2SMap i2s;
  CS2IMap s2i;
  std::string szUnknown;
  //
public:
  CRPGStatsAutomagic();
  //
  const char * STDCALL ToStr(const int nVal) const override
  {
    CI2SMap::const_iterator it = i2s.find(nVal);
    return it != i2s.end() ? it->second.c_str() : szUnknown.c_str();
  }

  const int STDCALL ToInt(const char *pszVal) const override
  {
    CS2IMap::const_iterator it = s2i.find(pszVal);
    return it != s2i.end() ? it->second : -1;
  }

  const char * STDCALL GetFirstStr() const override;
  const int STDCALL GetFirstInt() const override;
  bool STDCALL IsLastStr(const char *pszVal) const override;
  bool STDCALL IsLastInt(int nVal) const override;
  const char * STDCALL GetNextStr(const char *pszVal) override;
  const int STDCALL GetNextInt(int nVal) override;
};

// ************************************************************************************************************************ //
// **
// ** Acknowledgment types
// **
// **
// **
// ************************************************************************************************************************ //

enum EUnitAckType
{
  ACK_POSITIVE = 0,
  ACK_NEGATIVE = 1,
  ACK_SELECTED = 2,

  ACK_NONE = 3,

  // attack (do not change the attack numbering, because it is numbered by priority - the lower the number, the higher the priority)
  _ACK_ATTACK_BEGIN = 5,
  ACK_INVALID_TARGET = _ACK_ATTACK_BEGIN,
  ACK_DONT_SEE_THE_ENEMY = 6,
  ACK_NOT_IN_ATTACK_ANGLE = 7,
  ACK_NOT_IN_FIRE_RANGE = 8,
  ACK_ENEMY_IS_TO_CLOSE = 9,
  ACK_NO_AMMO = 10,
  ACK_CANNOT_PIERCE = 11,
  ACK_CANNOT_FIND_PATH_TO_TARGET = 12,
  ACK_ENEMY_ISNT_IN_FIRE_SECTOR = 13,
  _ACK_ATTACK_END = ACK_ENEMY_ISNT_IN_FIRE_SECTOR,

  // movement
  ACK_CANNOT_MOVE_NEED_TO_BE_TOWED_TO_MOVE = 14,
  ACK_CANNOT_MOVE_TRACK_DAMAGED = 15,

  // supply
  ACK_GOING_TO_STORAGE = 16,
  ACK_CANNOT_SUPPLY_NOT_PATH = 17,
  ACK_NO_RESOURCES_CANT_FIND_DEPOT = 18,
  ACK_NO_RESOURCES_CANT_FIND_PATH_TO_DEPOT = 19,
  ACK_CANNOT_MOVE_WAITING_FOR_LOADERS = 20,
  ACK_START_SERVICE_RESUPPLY = 21,
  ACK_START_SERVICE_REPAIR = 22,

  // ACK_NO_CANNOT_BUILD_STORAGE_HERE = 23,


  // aviation
  ACK_PLANE_TAKING_OFF = 24,
  ACK_PLANE_LEAVING = 25,

  // you need to be installed to run this command
  ACK_NEED_INSTALL = 26,

  ACK_NO_ENGINEERS_CANNOT_REACH_BUILDPOINT = 27,
  // ACK_NO_ENGINEERS_BUILD_INPOSSIBLE = 28,

  ACK_NO_TOO_HEAVY_ARTILLERY_FOR_TRANSPORT = 29,
  ACK_NO_CANNOT_UNHOOK_ARTILLERY_HERE = 30,

  // ACK_NO_CANNOT_BUILD_PIT_HERE = 31,
  // ACK_NO_CANNOT_LEAVE_PIT = 32,
  // ACK_NO_CANNOT_ENTER_PIT_TOO_SMALL = 33,
  // ACK_NO_CANNOT_ENTER_PIT_BUSY = 34,

  // ACK_NO_CANNOT_BUILD_ANTITANK_HERE = 35,

  _ACK_BORED_BEGIN = 36,
  ACK_BORED_RUSH = _ACK_BORED_BEGIN,// in an aggressive formation attack. 
  ACK_BORED_ATTACK = 37,
  ACK_BORED_LOW_AMMO = 38,
  ACK_BORED_NO_AMMO = 39,
  ACK_BORED_IDLE = 40,

  ACK_BORED_SNIPER_SNEAK = 41,
  ACK_BORED_MINIMUM_MORALE = 42,
  ACK_BORED_LOW_HIT_POINTS = 43,
  ACK_BORED_INFANTRY_TRAVEL = 44,

  _ACK_BORED_END = ACK_BORED_INFANTRY_TRAVEL,


  ACK_GETTING_AMMO = 45,
  ACK_ATTACKING_AVIATION = 46,
  // ACK_BUILDING_COMPLETE = 47,
  ACK_BEING_ATTACKED_BY_AVIATION = 48,

  ACK_NO_CANNOT_HOOK_ARTILLERY_NO_PATH = 49,
  ACK_SELECTION_TO_MUCH = 50,
  ACK_PLANE_REACH_POINT_START_ATTACK = 51,

  ACK_KILLED_ENEMY = 52,

  ACK_BUILDING_FINISHED = 53,
  ACK_CANNOT_START_BUILD = 54,
  ACK_CANNOT_FINISH_BUILD = 55,

  ACK_KILLED_ENEMY_INFANTRY = 56,
  ACK_KILLED_ENEMY_AVIATION = 57,
  ACK_KILLED_ENEMY_TANK = 58,
  ACK_UNIT_DIED = 59,

  // must always have the highest number
  _ACK_END,
};

// ************************************************************************************************************************ //
// **
// ** Acks stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SAckRPGStats : SCommonRPGStats
{
  struct SType
  {
    struct SAck
    {
      std::string szSound;// reference to sound
      float fProbability;// probability of this ack
      //
      int operator&(IDataTree &ss);
    };

    //
    EUnitAckType eType;// ack type
    std::vector<SAck> acks;// all acks of this type
    //
    int operator&(IDataTree &ss);
  };

  //
  std::vector<SType> types;// all types
  //
  SAckRPGStats();
  ~SAckRPGStats() override;
  //
  void STDCALL ToAIUnits() override;
  //
  bool ChooseAcknowledgement(float fRandom, EUnitAckType type, std::string *pResult) const;
  //
  int STDCALL operator&(IDataTree &ss) override;

  const uLong STDCALL CalculateCheckSum() const override { return 0; }
};

// ************************************************************************************************************************ //
// **
// ** base unit stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SUnitBaseRPGStats : SHPObjectRPGStats
{
  // animation descriptor
  struct SAnimDesc
  {
    int nLength;// length of this animation (in ticks)
    int nAction;// action point (in ticks with respect to animation's start)
    int nIndex;// order index of all animations list
    int nAABB_A;// AABB for animation index (-1 if no). 
    int nAABB_D;// AABB for final state index (-1 if no). 
    //
    SAnimDesc() : nLength(0), nAction(0), nIndex(-1), nAABB_A(-1), nAABB_D(-1) {}

    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;
      saver.Add("Length", &nLength);
      saver.Add("Action", &nAction);
      saver.Add("Index", &nIndex);
      saver.Add("AABB_A", &nAABB_A);
      saver.Add("AABB_D", &nAABB_D);
      return 0;
    }
  };

  //
  struct SAABBDesc
  {
    CVec2 vCenter;// center
    CVec2 vHalfSize;// half size
    //
    int ToAIUnits();

    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;
      saver.Add("Center", &vCenter);
      saver.Add("HalfSize", &vHalfSize);
      return 0;
    }
  };

  //
  EUnitRPGType type;// unit type
  EAIClass aiClass;// AI class of this unit
  //
  std::vector<std::string> szAcksNames;// names of acks sets
  std::vector<const SAckRPGStats *> pAcksSets;// shortcuts
  //
  const SBaseGunRPGStats *pPrimaryGun;// primary gun for data extracting for stats
  int nAmmos[2];// primary and secondary max ammo counts (for visualization)
  //
  float fSight;// (meters <=> AI tiles) sight range
  float fSightPower;// sight power
  float fSpeed;// (kilometers/hour <=> dots/tick) speed on the road
  float fRotateSpeed;// (degrees/sec <=> angle 65536/tick) speed of rotation (in direction units (2pi = 65536))
  float fPassability;// (cucumbers) permeability
  int nPriority;// priority to "give way" to the road
  float fCamouflage;// (% of meters) camouflage ability - alien vision multiplier
  // max and min - are calculated for the minimum possible armor for each side
  int nMaxArmor;// maximum armor \| 
  int nMinArmor;// minimum armor /| 
  // installation
  union
  {
    float fUninstallRotate;
    int nUninstallRotate;
  };// uninstall for rotation and install from it
  union
  {
    float fUninstallTransport;
    int nUninstallTransport;
  };// uninstall for transporting and install from it

  int nBoundTileRadius;// (AI tiles) radius that limits the physical object in tiles

  float fWeight;// (kilograms <=> kilograms) weight
  float fPrice;// (cucumbers) unit value

  // axis-aligned bounding box
  CVec2 vAABBCenter;// aabb center
  CVec2 vAABBHalfSize;// aabb half size
  CVec2 vAABBVisCenter;
  CVec2 vAABBVisHalfSize;
  std::vector<SAABBDesc> aabb_as;// AABBs for animation process
  std::vector<SAABBDesc> aabb_ds;// AABBs for animation finish
  float fSmallAABBCoeff;// small AABB coeff
  //
  CArray1Bit availCommands;// available commands, which unit can do
  CArray1Bit availExposures;// available exposures, which one can do with this unit
  CUserActions availUserActions;// re-map AI commands to user actions
  CUserActions availUserExposures;// re-map AI commands to user actions
  //
  std::vector<std::vector<SAnimDesc>> animdescs;// animation descriptions by animation types
  //
  SUnitBaseRPGStats(const char *pszType);
  ~SUnitBaseRPGStats() override {}
  //
  void STDCALL ToAIUnits() override;
  //	
  int GetAnimTime(int nAnim) const { return nAnim >= animdescs.size() || animdescs[nAnim].empty() ? 0 : animdescs[nAnim][0].nLength; }
  int GetAnimActionTime(int nAnim) const { return nAnim >= animdescs.size() || animdescs[nAnim].empty() ? 0 : animdescs[nAnim][0].nAction; }
  // chooses sound for given ack type and writes it to passed string
  // returns false if no acknowledgement is chosen
  virtual bool STDCALL ChooseAcknowledgement(float fRandom, EUnitAckType type, std::string *str, int nSet) const;
  const EUnitRPGClass GetRPGClass() const { return ::GetRPGClass(type); }
  EUnitRPGType GetMainType() const { return ::GetMainType(type); }
  int IsInfantry() const { return ::IsInfantry(type); }
  int IsTransport() const { return ::IsTransport(type); }
  int IsArtillery() const { return ::IsArtillery(type); }
  int IsSPG() const { return ::IsSPG(type); }
  int IsArmor() const { return ::IsArmor(type); }
  int IsAviation() const { return ::IsAviation(type); }
  int IsTrain() const { return ::IsTrain(type); }

  int GetArmor(int n) const override = 0;
  int GetMinPossibleArmor(int n) const override = 0;
  int GetMaxPossibleArmor(int n) const override = 0;
  int GetRandomArmor(int n) const override = 0;
  virtual const SBaseGunRPGStats &GetGun(int n) const = 0;
  //
  template<class TGun>
  void CountPrimaryGuns(std::vector<TGun> &guns)
  {
    // find min and max gun priority
    int nMin = 1000000000, nMax = -1000000000;
    for (typename std::vector<TGun>::const_iterator it = guns.begin(); it != guns.end(); ++it)
    {
      nMin = Min(nMin, it->nPriority);
      nMax = Max(nMax, it->nPriority);
    }
    // set priority and count ammos
    Zero(nAmmos);
    for (typename std::vector<TGun>::iterator it = guns.begin(); it != guns.end(); ++it)
    {
      if ((pPrimaryGun == nullptr) && (it->nPriority == nMin)) pPrimaryGun = &(*it);
      it->bPrimary = it->nPriority == nMin;
      nAmmos[it->bPrimary ? 0 : 1] += it->nAmmo;
    }
  }

  // 
  template<class TGun>
  void CountShellTypes(std::vector<TGun> &guns)
  {
    // count shell types
    int nDamageTypes[3] = {0, 0, 0};
    for (typename std::vector<TGun>::const_iterator it = guns.begin(); it != guns.end(); ++it) { for (std::vector<SWeaponRPGStats::SShell>::const_iterator shell = it->pWeapon->shells.begin(); shell != it->pWeapon->shells.end(); ++shell) nDamageTypes[shell->eDamageType] = 1; }
    // set actions
    if (nDamageTypes[0] + nDamageTypes[1] + nDamageTypes[2] > 1)
    {
      AddCommand(ACTION_COMMAND_CHANGE_SHELLTYPE);
      availUserActions.SetAction(USER_ACTION_CHANGE_SHELL);
      if (nDamageTypes[SWeaponRPGStats::SShell::DAMAGE_HEALTH] != 0) availUserActions.SetAction(USER_ACTION_USE_SHELL_DAMAGE);
      if (nDamageTypes[SWeaponRPGStats::SShell::DAMAGE_MORALE] != 0) availUserActions.SetAction(USER_ACTION_USE_SHELL_AGIT);
      if (nDamageTypes[SWeaponRPGStats::SShell::DAMAGE_FOG] != 0) availUserActions.SetAction(USER_ACTION_USE_SHELL_SMOKE);
    }
    else
    {
      RemoveCommand(ACTION_COMMAND_CHANGE_SHELLTYPE);
      availUserActions.RemoveAction(USER_ACTION_CHANGE_SHELL);
      availUserActions.RemoveAction(USER_ACTION_USE_SHELL_DAMAGE);
      availUserActions.RemoveAction(USER_ACTION_USE_SHELL_AGIT);
      availUserActions.RemoveAction(USER_ACTION_USE_SHELL_SMOKE);
    }
  }

  //
  const bool HasCommand(const int nCmd) const
  {
    // NI_ASSERT_T( (nCmd >= 0 && nCmd < availCommands.GetSize()) || (nCmd >= 1000), NStr::Format( "Wrong command ( %d )\n", nCmd ) );
    return nCmd >= 1000 ? true : (nCmd < availCommands.GetSize() ? availCommands.GetData(nCmd) : false);
  }

  void AddCommand(const int nCmd)
  {
    NI_ASSERT_T(nCmd < 1000, NStr::Format( "Wrong command ( %d )\n", nCmd ));
    if (availCommands.GetSize() <= nCmd) availCommands.SetSize(nCmd + 1);
    availCommands.SetData(nCmd);
  }

  void RemoveCommand(const int nCmd)
  {
    // NI_ASSERT_T( nCmd >= 0 && nCmd < availCommands.GetSize(), NStr::Format( "Wrong command ( %d )\n", nCmd ) );
    if (nCmd >= 0 && nCmd < availCommands.GetSize()) availCommands.RemoveData(nCmd);
  }

  //
  void GetUserActions(CUserActions *pActions) const { availUserActions.GetActions(pActions); }
  const bool HasUserAction(const int nAction) const { return availUserActions.HasAction(nAction); }
  //
  const std::vector<SAnimDesc> *GetAnims(const int nType) const { return nType < animdescs.size() ? &(animdescs[nType]) : nullptr; }
  //
  const CUserActions *GetUserActions(bool bActionsBy) const override { return bActionsBy ? &availUserActions : &availUserExposures; }
  //
  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** mech unit stats (tank, auto, artillery, etc.)
// **
// **
// **
// ************************************************************************************************************************ //

struct SMechUnitRPGStats : SUnitBaseRPGStats
{
  struct SConstraint
  {
    union
    {
      float fMin;
      WORD wMin;
    };

    union
    {
      float fMax;
      WORD wMax;
    };

    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);
  };

  struct SGun : SBaseGunRPGStats
  {
    int nShootPoint;// shot point index
    bool bRecoil;// is there any recoil when firing?
    float fRecoilLength;// recoil length
    DWORD recoilTime;// release rollback time
    int nModelPart;// part of the model that represents this gun (index).
    // only makes sense for 'main gun'. 
    int nRecoilShakeTime;// flinch time (msec)
    float fRecoilShakeAngle;// flinch angle (radian)
    //
    SGun();
    int STDCALL operator&(IDataTree &ss) override;

    //
    const uLong STDCALL CalculateCheckSum() const override;
  };

  struct SPlatform
  {
    union
    {
      float fHorizontalRotationSpeed;
      WORD wHorizontalRotationSpeed;
    };// (seconds per full rotation (360 degrees) <=> degrees65535/tick) horizontal rotation speed
    union
    {
      float fVerticalRotationSpeed;
      WORD wVerticalRotationSpeed;
    };// (seconds per revolution <=> degrees65535/tick) vertical rotation speed
    int nModelPart;// part of the model that represents this platform (index)
    SConstraint constraint;// constraint for procedural animation
    DWORD dwGunCarriageParts;// barrel "machine" - for all barrels that can be aimed vertically
    SConstraint constraintVertical;// restriction on vertical aiming of the barrel
    // guns on the platform.
    // the main gun goes first
    int nFirstGun;// first platform gun index in the 'guns' array
    int nNumGuns;// number of guns on this platform
    //
    SPlatform();
    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  struct SArmor
  {
    union
    {
      float fMin;
      int nMin;
    };// armor value for one side
    union
    {
      float fMax;
      int nMax;
    };// random for armor value
    //
    SArmor() : nMin(0), nMax(0) {}
    bool ToAIUnits();
    int operator&(IDataTree &ss);
  };

  //
  struct SJoggingParams
  {
    // value = fAmp1 * cos( time/fPeriod1*2pi + fPhase1 ) + fAmp2 * cos( time/fPeriod2*2pi + fPhase2 )
    float fPeriod1, fPeriod2;// 'cos' wave periods
    float fAmp1, fAmp2;// -~- amplitude
    float fPhase1, fPhase2;// -~- phase
    //
    SJoggingParams();
    int operator&(IDataTree &ss);
  };

  //
  std::vector<SGun> guns;// all guns in the next order: all guns on the base platform, all guns on the turret
  std::vector<SPlatform> platforms;// all platforms. 
  //
  SArmor armors[6];// (cucumbers) armor from 6 directions (see EDirection)
  float fTowingForce;// (kilograms <=> kilograms) pulling force
  int nCrew;// number of crew members, required to operate
  int nPassangers;// number of possible passengers to carry
  //
  float fTurnRadius;// (meters <=> AI points) turning radius
  // additional points...
  std::vector<int> exhaustPoints;// exhaust points
  std::vector<int> damagePoints;// explosion points upon death
  int nTowPoint;// towing point
  int nEntrancePoint;// entry point to the machine
  std::vector<int> peoplePointIndices;// points of location of people near this machine
  int nFatalitySmokePoint;// point for releasing the effect at fatality
  int nShootDustPoint;// point for dust emission when fired from MLRS/howitzers. 
  // real positions of some points needed for quick access from AI
  CVec2 vTowPoint;// 2D position of towing point
  CVec2 vEntrancePoint;// 2D entry point position
  std::vector<CVec2> vPeoplePoints;// 2D positions of people's location points with this machine
  CVec2 vAmmoPoint;// 2D point for ammo box
  std::vector<std::vector<CVec2>> vGunners;// gunners information in the different modes: operate, rotate, move
  CVec2 vHookPoint;// 2D wagon hookup point (only for wagons)
  CVec2 vFrontWheel;// 2D point of the front wheels of the carriage (only for carriages)
  CVec2 vBackWheel;// 2D point of the rear wheels of the carriage (only for carriages)
  // effects
  std::string szEffectDiesel;// diesel smoke effect
  std::string szEffectSmoke;// smoke effect
  std::string szEffectWheelDust;// dust from under the wheels
  std::string szEffectShootDust;// dust when shot
  std::string szEffectFatality;// fatality effect
  std::string szEffectEntrenching;// entrenching for technology
  std::string szEffectDisappear;// effect when the 'corpse' of a vehicle disappears
  // jogging params
  SJoggingParams jx, jy, jz;// jogging in 3 axes
  // tracks
  bool bLeavesTracks;// does it leave traces
  float fTrackWidth;// width of one track as a percentage of the width of AABB
  float fTrackOffset;// edge offset AABB
  float fTrackStart;// indentation of the beginning of the track from the beginning of AABB
  float fTrackEnd;// indentation of the end of the track from the end of AABB
  float fTrackIntensity;// track intensity (1 - alpha)
  int nTrackLifetime;// track lifetime
  // sounds
  std::string szSoundMoveStart;// start movement
  std::string szSoundMoveCycle;// cycle movement sound
  std::string szSoundMoveStop;// stop movement
  // locators, different for 3 models
  // for aviation units
  float fMaxHeight;

  union
  {
    float fDivingAngle;
    WORD wDivingAngle;
  };

  union
  {
    float fClimbAngle;
    WORD wClimbingAngle;
  };

  union
  {
    float fTiltAngle;
    WORD wTiltAngle;
  };

  float fTiltRatio;
  // when a vehicle dies, a death crater remains underneath it
  std::vector<std::string> deathCraters;
  //
  SMechUnitRPGStats();
  ~SMechUnitRPGStats() override {}
  //
  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;
  // convert from human units to AI
  void STDCALL ToAIUnits() override;
  // checking stats for correctness
  bool STDCALL Validate() override;
  //
  int GetArmor(const int n) const override { return (armors[n].nMin + armors[n].nMax) / 2; }
  int GetMinPossibleArmor(const int n) const override { return armors[n].nMin; }
  int GetMaxPossibleArmor(const int n) const override { return armors[n].nMax; }
  int GetRandomArmor(const int n) const override { return RandomCheck(armors[n].nMin, armors[n].nMax); }
  const SBaseGunRPGStats &GetGun(const int n) const override { return guns[n]; }
  //
  bool HasDieselEffect() const { return !szEffectDiesel.empty(); }
  bool HasSmokeEffect() const { return !szEffectSmoke.empty(); }
  bool HasMoveStartSound() const { return !szSoundMoveStart.empty(); }
  bool HasMoveSound() const { return !szSoundMoveCycle.empty(); }
  bool HasMoveStopSound() const { return szSoundMoveStop.empty(); }
  //
  const std::vector<CVec2> *GetGunners(const int nMode) const { return nMode < vGunners.size() ? &(vGunners[nMode]) : nullptr; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** infantry stats (soldier, officer, engineer, etc.)
// **
// **
// **
// ************************************************************************************************************************ //

struct SInfantryRPGStats : SUnitBaseRPGStats
{
  struct SGun : SBaseGunRPGStats
  {
    int STDCALL operator&(IDataTree &ss) override;
  };

  //
  std::vector<SGun> guns;// guns[0] - main gun (rifle, machinegun, etc.), guns[1] - (if it is) secondary weapon - grenade
  // 'attack' animation in 'stand' and in 'lie' positions
  bool bCanAttackUp;// can attack a hundred
  bool bCanAttackDown;// can attack while lying down
  // visual speed
  float fRunSpeed;// running speed
  float fCrawlSpeed;// crawling speed
  // animation lengths
  std::vector<int> animtimes;// length of all animations in msec (0 if no such animation)
  // AI settings
  /* std::vector<int> commands;						 */
  //
  SInfantryRPGStats();
  ~SInfantryRPGStats() override {}

  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;
  //
  void STDCALL ToAIUnits() override;
  //
  int GetArmor(const int n) const override { return 0; }
  int GetMinPossibleArmor(const int n) const override { return 0; }
  int GetMaxPossibleArmor(const int n) const override { return 0; }
  int GetRandomArmor(const int n) const override { return 0; }
  const SBaseGunRPGStats &GetGun(const int n) const override { return guns[n]; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** entrenchment set stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SEntrenchmentRPGStats : SHPObjectRPGStats
{
  enum
  {
    ENTRENCHMENT_LINE = 0x00000001,
    ENTRENCHMENT_FIREPLACE = 0x00000002,
    ENTRENCHMENT_TERMINATOR = 0x00000004,
    ENTRENCHMENT_ARC = 0x00000008
  };

  enum EEntrenchSegmType
  {
    EST_LINE = 0,
    EST_FIREPLACE = 1,
    EST_TERMINATOR = 2,
    EST_ARC = 3
  };

  struct SSegmentRPGStats
  {
    std::string szModel;// model name
    CVec2 vFirePlace;// fireplace position (VNULL2 - no fireplace)
    CVec2 vAABBCenter;// AABB center 2D point
    CVec3 vAABBHalfSize;// AABB half size (x, y, z)
    float fCoverage;// coverage for the unit in the fireplace
    std::vector<CVec2> fireplaces;// all fireplaces in this segment

    EEntrenchSegmType eType;
    //
    SSegmentRPGStats() : vFirePlace(VNULL2) {}
    //
    float GetLength() const;
    float GetHalfLength() const;
    const CVec2 GetVisFirePlace() const;
    const CVec2 GetVisAABBCenter() const;
    const CVec3 GetVisAABBHalfSize() const;
    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  //
  std::vector<SSegmentRPGStats> segments;
  //
  std::vector<int> lines;
  std::vector<int> fireplaces;
  std::vector<int> terminators;
  std::vector<int> arcs;
  //
  SEntrenchmentRPGStats();
  ~SEntrenchmentRPGStats() override {}
  //
  int GetIndexLocal(int nIndex, const std::vector<int> &indices, const char *pszName, int *pCurRandomSeed = nullptr) const
  {
    // NI_ASSERT_SLOW_TF( !indices.empty() && (nIndex < indices.size()), NStr::Format("Can't find any \"%s\" segment for entrenchment", pszName), return -1 );
    if (nIndex == -1)
    {
      if (pCurRandomSeed == nullptr) return indices[rand() % indices.size()];
      *pCurRandomSeed %= indices.size();
      return indices[*pCurRandomSeed];
    }
    return indices[nIndex];
  }

  //
  int GetLineIndex(int nIndex = -1) const { return GetIndexLocal(nIndex, lines, "line", nullptr); }
  int GetFirePlaceIndex(int nIndex = -1) const { return GetIndexLocal(nIndex, fireplaces, "fireplace", nullptr); }
  int GetTerminatorIndex(int nIndex = -1) const { return GetIndexLocal(nIndex, terminators, "terminator", nullptr); }
  int GetArcIndex(int nIndex = -1) const { return GetIndexLocal(nIndex, arcs, "arc", nullptr); }
  //
  int GetLineIndex(int *pCurRandomSeed) const { return GetIndexLocal(-1, lines, "line", pCurRandomSeed); }
  int GetFirePlaceIndex(int *pCurRandomSeed) const { return GetIndexLocal(-1, fireplaces, "fireplace", pCurRandomSeed); }
  int GetTerminatorIndex(int *pCurRandomSeed) const { return GetIndexLocal(-1, terminators, "terminator", pCurRandomSeed); }
  int GetArcIndex(int *pCurRandomSeed) const { return GetIndexLocal(-1, arcs, "arc", pCurRandomSeed); }
  //
  const SSegmentRPGStats &GetSegmentStats(const int nIndex) const
  {
    NI_ASSERT_SLOW_T(nIndex >= 0 && nIndex < segments.size(), "Invalid segment index");
    return segments[nIndex];
  }

  //
  void STDCALL ToAIUnits() override;
  //
  virtual const int STDCALL GetTypeFromIndex(int nIndex) const;
  virtual const int STDCALL GetIndexFromType(int nType, int *pCurRandomSeed = nullptr) const;
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** fence set stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SFenceRPGStats : SStaticObjectRPGStats
{
  enum
  {
    FENCE_DIRECTION_0 = 0x00000001,
    FENCE_DIRECTION_1 = 0x00000002,
    FENCE_DIRECTION_2 = 0x00000004,
    FENCE_DIRECTION_3 = 0x00000008,
    FENCE_TYPE_NORMAL = 0x00010000,
    FENCE_TYPE_LDAMAGE = 0x00020000,
    FENCE_TYPE_RDAMAGE = 0x00040000,
    FENCE_TYPE_CDAMAGE = 0x00080000,
  };

  //
  struct SSegmentRPGStats
  {
    CArray2D<BYTE> passability;// passability of AI tiles of this segment
    CVec2 vOrigin;// segment zero point for passability
    CArray2D<BYTE> visibility;// degree of gaze absorption
    CVec2 vVisOrigin;// segment zero point for visibility
    int nIndex;// sprite index
    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  //
  struct SDir
  {
    // segment indices from the common array
    std::vector<int> centers;// center segment indicators
    std::vector<int> ldamages;// left-damaged segment indicators
    std::vector<int> rdamages;// right-damaged segment indicators
    std::vector<int> cdamages;// center-damaged (destroyed) segment indicators
    //
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  //
  std::vector<SSegmentRPGStats> stats;
  std::vector<SDir> dirs;// all available directions
  //
  SFenceRPGStats() : SStaticObjectRPGStats("Fence") {}
  ~SFenceRPGStats() override {}
  //
  void STDCALL ToAIUnits() override;
  //
  const CVec2 & STDCALL GetOrigin(const int nIndex = -1) const override
  {
    NI_ASSERT_SLOW_T(nIndex > -1 && nIndex < stats.size(), NStr::Format("Index %d for the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), stats.size()));
    return stats[nIndex].vOrigin;
  }

  const CArray2D<BYTE> & STDCALL GetPassability(const int nIndex = -1) const override
  {
    NI_ASSERT_SLOW_T(nIndex > -1 && nIndex < stats.size(), NStr::Format("Index %d for the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), stats.size()));
    return stats[nIndex].passability;
  }

  const CVec2 & STDCALL GetVisOrigin(const int nIndex = -1) const override
  {
    NI_ASSERT_SLOW_T(nIndex > -1 && nIndex < stats.size(), NStr::Format("Index %d for the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), stats.size()));
    return stats[nIndex].vVisOrigin;
  }

  const CArray2D<BYTE> & STDCALL GetVisibility(const int nIndex = -1) const override
  {
    NI_ASSERT_SLOW_T(nIndex > -1 && nIndex < stats.size(), NStr::Format("Index %d for the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), stats.size()));
    return stats[nIndex].visibility;
  }

  // helper functions
  int GetIndexLocal(int nIndex, const std::vector<int> &indices, const char *pszDirName, int *pRandomSeed) const
  {
    NI_ASSERT_TF(indices.size() > 0, NStr::Format("Fence set \"%s\" must have at least one %s tile", szKeyName.c_str(), pszDirName), return 0);
    if (nIndex > 0 && nIndex < indices.size())
    {
      NI_ASSERT_TF(indices[nIndex] == stats[indices[nIndex]].nIndex, NStr::Format("Indices in %s fence stats \"%s\" must be the same!", pszDirName, szKeyName.c_str()), return 0);
      return indices[nIndex];
    }
    else
    {
      if (pRandomSeed == nullptr) nIndex = rand() % indices.size();
      else
      {
        *pRandomSeed %= indices.size();
        nIndex = *pRandomSeed;
      }

      return indices[nIndex];
    }
  }

  //
  int GetCenterIndex(int nDir, int nIndex = -1) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(nIndex, dirs[nDir].centers, "center", nullptr);
  }

  int GetLDamageIndex(int nDir, int nIndex = -1) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(nIndex, dirs[nDir].ldamages, "ldamage", nullptr);
  }

  int GetRDamageIndex(int nDir, int nIndex = -1) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(nIndex, dirs[nDir].rdamages, "rdamage", nullptr);
  }

  int GetCDamageIndex(int nDir, int nIndex = -1) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(nIndex, dirs[nDir].cdamages, "cdamage", nullptr);
  }

  //
  int GetCenterIndex(int nDir, int *pCurRandomSeed) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(-1, dirs[nDir].centers, "center", pCurRandomSeed);
  }

  int GetLDamageIndex(int nDir, int *pCurRandomSeed) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(-1, dirs[nDir].ldamages, "ldamage", pCurRandomSeed);
  }

  int GetRDamageIndex(int nDir, int *pCurRandomSeed) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(-1, dirs[nDir].rdamages, "rdamage", pCurRandomSeed);
  }

  int GetCDamageIndex(int nDir, int *pCurRandomSeed) const
  {
    NI_ASSERT_TF(nDir > -1 && nDir < dirs.size(), NStr::Format("Direction (%d) for fence must be in range [0..%d]", nDir, dirs.size()), return 0);
    return GetIndexLocal(-1, dirs[nDir].cdamages, "cdamage", pCurRandomSeed);
  }

  //
  virtual const int STDCALL GetTypeFromIndex(int nIndex) const;
  virtual const int STDCALL GetIndexFromType(int nType, int *pCurRandomSeed = nullptr) const;
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** squad RPG stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SSquadRPGStats : SHPObjectRPGStats
{
  enum ESquadType
  {
    RIFLEMANS = 0,
    INFANTRY = 1,
    SUBMACHINEGUNNERS = 2,
    MACHINEGUNNERS = 3,
    AT_TEAM = 4,
    MORTAR_TEAM = 5,
    SNIPERS = 6,
    GUNNERS = 7,
    ENGINEERS = 8,
  };

  enum EEVents
  {
    HIT_NEAR = 0,// platoon under fire
  };

  //
  struct SFormation
  {
    enum EType
    {
      DEFAULT = 0,
      MOVEMENT = 1,
      DEFENSIVE = 2,
      OFFENSIVE = 3,
      SNEAK = 4
    };

    struct SEntry
    {
      std::string szSoldier;// soldier name
      const SInfantryRPGStats *pSoldier;// shortcut
      CVec2 vPos;// (vis point <=> AI point) position, relative to formation center
      union
      {
        float fDir;
        int nDir;
      };// (degree <=> degree65535) soldier direction, relative to formation direction
      //
      int operator&(IDataTree &ss);
      void RetrieveShortcuts(IObjectsDB *pGDB);
      bool ToAIUnits();

      //
      const uLong CalculateCheckSum() const;
    };

    //
    EType type;// formation type
    //
    std::vector<SEntry> order;// all soldiers in this formation
    // bonuses of formation
    BYTE cLieFlag;// 0 - standard behavior, 1 - always stand, 2 - always lie down
    float fSpeedBonus;
    float fDispersionBonus;
    float fFireRateBonus;
    float fRelaxTimeBonus;
    float fCoverBonus;
    float fVisibleBonus;

    std::vector<int> changesByEvent;// which formation to move to based on an event, -1 - don’t move anywhere
    //
    int operator&(IDataTree &ss);
    void RetrieveShortcuts(IObjectsDB *pGDB);
    bool ToAIUnits();

    SFormation();

    //
    const uLong CalculateCheckSum() const;
  };

  //
  std::string szIcon;// icon picture
  ESquadType type;// squad type
  std::vector<std::string> memberNames;// squad members
  std::vector<const SInfantryRPGStats *> members;// shortcuts to member RPG stats
  std::vector<SFormation> formations;// available formations
  CUserActions availActions;// available actions by this formation
  CUserActions availExposures;// available exposures to this formation
  //
  SSquadRPGStats() : SHPObjectRPGStats("Squad") {}
  ~SSquadRPGStats() override {}
  //
  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;
  //
  void STDCALL ToAIUnits() override;
  //
  bool STDCALL Validate() override;
  //
  const CUserActions *GetUserActions(bool bActionsBy) const override { return bActionsBy ? &availActions : &availExposures; }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** mine RPG stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SMineRPGStats : SObjectBaseRPGStats
{
  enum EType
  {
    INFANTRY = 0,
    TECHNICS = 1
  };

  //
  std::string szWeapon;// weapon name
  const SWeaponRPGStats *pWeapon;// weapon shortcut
  EType type;// anti-infantry or anti-tank
  //
  float fWeight;// weight required to operate
  std::string szFlagModel;// flag above a detected mine
  //
  SMineRPGStats();
  ~SMineRPGStats() override {}
  //
  void STDCALL RetrieveShortcuts(IObjectsDB *pGDB) override;
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** bridge RPG stats
// **
// **
// **
// ************************************************************************************************************************ //

// the bridge itself locates tiles for all unit classes, and dwAIClasses specifies classes that cannot travel across the bridge
struct SBridgeRPGStats : SStaticObjectRPGStats
{
private:
  virtual const int STDCALL GetIndexFromTypeLocal(int nType, int nDamageState, int *pCurRandomSeed) const;

public:
  enum
  {
    BRIDGE_SPAN_TYPE_BEGIN = 0x00000001,
    BRIDGE_SPAN_TYPE_CENTER = 0x00000002,
    BRIDGE_SPAN_TYPE_END = 0x00000004,
  };

  enum EDirection
  {
    VERTICAL = 0,
    HORIZONTAL = 1
  };

  struct SSegmentRPGStats
  {
    enum EType
    {
      SLAB = 0,
      GIRDER = 1
    };

    //
    EType eType;// segment type - slab or girder
    // pass/vis and origin params
    CArray2D<BYTE> passability;// passability of AI tiles of this segment
    CVec2 vOrigin;// segment zero point for passability
    CArray2D<BYTE> visibility;// degree of gaze absorption
    CVec2 vVisOrigin;// segment zero point for visibility
    //
    CVec3 vRelPos;// part relative position with respect to the span center
    std::string szModel;// model name
    int nFrameIndex;// index of the sprite inside packed model
    //
    SSegmentRPGStats();
    //
    bool ToAIUnits();
    int operator&(IDataTree &ss);

    //
    const uLong CalculateCheckSum() const;
  };

  //
  struct SSpan
  {
    // indices
    int nSlab;// slab part of the span segment index
    int nBackGirder;// back part of the span's girder segment index
    int nFrontGirder;// front part of the span's girder segment index
    // lengthes
    float fWidth;// span width
    float fLength;// span length
    //
    int operator&(IDataTree &ss);
  };

  //
  struct SDamageState
  {
    std::vector<SSpan> spans;// all spans of the bridge
    std::vector<int> begins;// indices of the 'begin' segments in the 'spans'
    std::vector<int> lines;// indices of the 'line' segments in the 'spans'
    std::vector<int> ends;// indices of the 'end' segments in the 'spans'
    //
    int operator&(IDataTree &ss);
  };

  //
  // fire point description, fire on the bridge
  struct SFirePoint
  {
    CVec3 vPos;// position
    float fDirection;// (angle <=> angle in radians) direction
    float fVerticalAngle;// (angle <=> angle in radians) angle from horizontal plane in the vertical plane, which crosses 'fDirection'
    std::string szFireEffect;// particle fire effect
    // vis info
    CVec2 vPicturePosition;// 2D picture position
    CVec3 vWorldPosition;// 3D position, calculated from picture one
    //
    SFirePoint();

    bool ToAIUnits();
    bool HasFireEffect() const { return !szFireEffect.empty(); }
    int operator&(IDataTree &ss);
  };

  // direction explosion description, explosion in the direction of fire
  struct SDirectionExplosion
  {
    CVec3 vPos;// position
    float fDirection;// (angle <=> angle in radians) direction
    float fVerticalAngle;// (angle <=> angle in radians) angle from horizontal plane in the vertical plane, which crosses 'fDirection'
    // vis info
    CVec2 vPicturePosition;// 2D picture position
    CVec3 vWorldPosition;// 3D position, calculated from picture one
    //
    SDirectionExplosion();

    bool ToAIUnits();
    int operator&(IDataTree &ss);
  };

  //
  EDirection direction;// vertical or horizontal bridge
  std::vector<SSegmentRPGStats> segments;// all segments
  SDamageState states[3];// alive, damaged and completely destroyed bridge stats
  //
  std::vector<SFirePoint> firePoints;// fire points...
  std::vector<SFirePoint> smokePoints;// smoke points, when the bridge is destroyed
  std::string szSmokeEffect;// smoke effect, one for all
  //
  enum EDirectionExplosionType
  {
    // do not change these IDs
    E_FRONT_LEFT,
    E_FRONT_RIGHT,
    E_BACK_RIGHT,
    E_BACK_LEFT,
    E_TOP_CENTER,
  };

  std::vector<SDirectionExplosion> dirExplosions;// direction explosions, 5 in total, see EDirectionExplosionType
  std::string szDirExplosionEffect;// direction explosion effect, one for all
  //
  SBridgeRPGStats();
  ~SBridgeRPGStats() override {}
  //
  int GetIndexLocal(int nIndex, const std::vector<int> &indices, const char *pszName, int *pRandomSeed = nullptr) const
  {
    // NI_ASSERT_SLOW_TF( !indices.empty() && (nIndex < indices.size()), NStr::Format("Can't find any \"%s\" segment for bridge", pszName), return -1 );
    if (nIndex == -1)
    {
      if (pRandomSeed == nullptr) return indices[rand() % indices.size()];
      *pRandomSeed %= indices.size();
      return indices[*pRandomSeed];
    }
    return indices[nIndex];
  }

  //
  int GetRandomBeginIndex(int nIndex = -1, int nState = 0) const { return GetIndexLocal(nIndex, states[nState].begins, "begin", nullptr); }
  int GetRandomLineIndex(int nIndex = -1, int nState = 0) const { return GetIndexLocal(nIndex, states[nState].lines, "line", nullptr); }
  int GetRandomEndIndex(int nIndex = -1, int nState = 0) const { return GetIndexLocal(nIndex, states[nState].ends, "end", nullptr); }
  //
  int GetRandomBeginIndex(int nIndex, int nState, int *pRandomSeed) const { return GetIndexLocal(nIndex, states[nState].begins, "begin", pRandomSeed); }
  int GetRandomLineIndex(int nIndex, int nState, int *pRandomSeed) const { return GetIndexLocal(nIndex, states[nState].lines, "line", pRandomSeed); }
  int GetRandomEndIndex(int nIndex, int nState, int *pRandomSeed) const { return GetIndexLocal(nIndex, states[nState].ends, "end", pRandomSeed); }
  //
  bool IsBeginIndex(int nIndex) const
  {
    return (std::find(states[0].begins.begin(), states[0].begins.end(), nIndex) != states[0].begins.end()) ||
           (std::find(states[1].begins.begin(), states[1].begins.end(), nIndex) != states[1].begins.end()) ||
           (std::find(states[2].begins.begin(), states[2].begins.end(), nIndex) != states[2].begins.end());
  }

  bool IsEndIndex(int nIndex) const
  {
    return (std::find(states[0].ends.begin(), states[0].ends.end(), nIndex) != states[0].ends.end()) ||
           (std::find(states[1].ends.begin(), states[1].ends.end(), nIndex) != states[1].ends.end()) ||
           (std::find(states[2].ends.begin(), states[2].ends.end(), nIndex) != states[2].ends.end());
  }

  bool IsLineIndex(int nIndex) const
  {
    return (std::find(states[0].lines.begin(), states[0].lines.end(), nIndex) != states[0].lines.end()) ||
           (std::find(states[1].lines.begin(), states[1].lines.end(), nIndex) != states[1].lines.end()) ||
           (std::find(states[2].lines.begin(), states[2].lines.end(), nIndex) != states[2].lines.end());
  }

  //
  const SSegmentRPGStats &GetSegmentStats(const int nIndex) const
  {
    NI_ASSERT_SLOW_T(nIndex >= 0 && nIndex < segments.size(), NStr::Format("Index %d for the segments of the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), segments.size()));
    return segments[nIndex];
  }

  //
  const SSpan &GetSpanStats(const int nIndex, const int nState = 0) const
  {
    NI_ASSERT_SLOW_T(nIndex >= 0 && nIndex < states[nState].spans.size(), NStr::Format("Index %d for the spans of the \"%s\"must be in the range [0..%d]", nIndex, szKeyName.c_str(), states[nState].spans.size()));
    return states[nState].spans[nIndex];
  }

  // in the next four functions 'nIndex' does not mean 'segment', but 'span', from which you need to pull out the 'nSlab' segment and return its data
  const CVec2 & STDCALL GetOrigin(const int nIndex = -1) const override
  {
    const int nSegment = GetSpanStats(nIndex).nSlab;
    return GetSegmentStats(nSegment).vOrigin;
  }

  const CArray2D<BYTE> & STDCALL GetPassability(const int nIndex = -1) const override
  {
    const int nSegment = GetSpanStats(nIndex).nSlab;
    return GetSegmentStats(nSegment).passability;
  }

  const CVec2 & STDCALL GetVisOrigin(const int nIndex = -1) const override
  {
    const int nSegment = GetSpanStats(nIndex).nSlab;
    return GetSegmentStats(nSegment).vVisOrigin;
  }

  const CArray2D<BYTE> & STDCALL GetVisibility(const int nIndex = -1) const override
  {
    const int nSegment = GetSpanStats(nIndex).nSlab;
    return GetSegmentStats(nSegment).visibility;
  }

  //
  void STDCALL ToAIUnits() override;
  //

  virtual const int STDCALL GetTypeFromIndex(int nIndex, int nDamageState = 0) const;
  virtual const int STDCALL GetIndexFromType(const int nType, const int nDamageState = 0) const { return GetIndexFromTypeLocal(nType, nDamageState, nullptr); }
  virtual const int STDCALL GetIndexFromType(const int nType, int *pCurRandomSeed) const { return GetIndexFromTypeLocal(nType, 0, pCurRandomSeed); }
  //
  int STDCALL operator&(IDataTree &ss) override;

  //
  const uLong STDCALL CalculateCheckSum() const override;
};

// ************************************************************************************************************************ //
// **
// ** sound stats
// **
// **
// **
// ************************************************************************************************************************ //

struct SSoundRPGStats : SCommonRPGStats
{
  struct SSound
  {
    std::string szPathName;// sound file path name
    float fMinDist;// minimal distance
    float fMaxDist;// distance
    float fProbability;// prabability inside set
    bool bPeacefull;// should we shut this sound up during combat?
    //
    SSound();
    //
    int operator&(IDataTree &ss);
  };

  //
  std::vector<SSound> sounds;
  bool bLooped;// is whole set looped
  //
  int virtual GetRandomSoundIndex() const;
  //
  SSoundRPGStats();
  ~SSoundRPGStats() override;
  //
  void STDCALL ToAIUnits() override;
  int STDCALL operator&(IDataTree &ss) override;

  const uLong STDCALL CalculateCheckSum() const override { return 0; }
};

// ************************************************************************************************************************ //
// **
// **exp levels
// **
// **
// **
// ************************************************************************************************************************ //

struct SAIExpLevel : IGDBObject
{
  struct SLevel
  {
    int nExp;
    float fBonusSpeed;
    float fBonusRelaxTime;
    float fBonusDispersion;
    float fBonusSight;
    float fBonusFireRange;
    float fBonusRotate;
    //
    SLevel() : nExp(0), fBonusSpeed(1.0f), fBonusRelaxTime(1.0f), fBonusDispersion(1.0f), fBonusSight(1.0f),
               fBonusFireRange(1.0f), fBonusRotate(1.0f) {}

    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;

      saver.Add("Exp", &nExp);
      saver.Add("BonusSpeed", &fBonusSpeed);
      saver.Add("BonusRelaxTime", &fBonusRelaxTime);
      saver.Add("BonusDispersion", &fBonusDispersion);
      saver.Add("BonusSight", &fBonusSight);
      saver.Add("BonusFireRange", &fBonusFireRange);
      saver.Add("BonusRotate", &fBonusRotate);

      return 0;
    }
  };

  //
  std::string szTypeName;
  EUnitRPGType eType;
  std::vector<SLevel> levels;
  //
  SAIExpLevel() : levels(1) {}

  const char * STDCALL GetName() const override { return "AIExpLevel"; }
  const char * STDCALL GetParentName() const override { return szTypeName.c_str(); }
  const uLong STDCALL GetCheckSum() const override { return 0L; }
  //
  virtual int STDCALL operator&(IDataTree &ss);
};

#endif // __RPGSTATS_H__