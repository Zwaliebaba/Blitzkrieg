#if !defined(__FMT__UNIT_CREATION__H__)
#define __FMT__UNIT_CREATION__H__

#pragma once


struct SUCAircraft
{
  std::string szName;// aircraft name in Objecst.
  int nFormationSize;// how many planes per call
  int nPlanes;// how many planes does the player have?

  SUCAircraft() : nFormationSize(1), nPlanes(1) {}

  SUCAircraft(const SUCAircraft &rUCAircraft)
    : szName(rUCAircraft.szName),
      nFormationSize(rUCAircraft.nFormationSize),
      nPlanes(rUCAircraft.nPlanes) {}

  SUCAircraft &operator=(const SUCAircraft &rUCAircraft)
  {
    if (&rUCAircraft != this)
    {
      szName = rUCAircraft.szName;
      nFormationSize = rUCAircraft.nFormationSize;
      nPlanes = rUCAircraft.nPlanes;
    }
    return *this;
  }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    saver.Add("Name", &szName);
    saver.Add("FormationSize", &nFormationSize);
    saver.Add("Planes", &nPlanes);

    return 0;
  }

  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;

    saver.Add(1, &szName);
    saver.Add(2, &nFormationSize);
    saver.Add(3, &nPlanes);

    return 0;
  }
};


struct SUCAviation
{
  enum AIRCRAFT_TYPE
  {
    AT_SCOUT = 0,
    AT_FIGHTER = 1,
    AT_PARADROPER = 2,
    AT_BOMBER = 3,
    AT_BATTLEPLANE = 4,
    AT_COUNT = 5,
  };

  std::vector<SUCAircraft> aircrafts;// Aircraft by type
  // std::string szParadropSoldierName;		
  std::string szParadropSquadName;// Paratroopers
  int nParadropSquadCount;// Number of parachutists
  int nRelaxTime;// time interval between calls (sec)
  std::list<CVec3> vAppearPoints;// points of possible appearance (airfield coordinates)

  SUCAviation()
    : nParadropSquadCount(1), nRelaxTime(30) { Validate(); }

  SUCAviation(const SUCAviation &rUCAviation)
    : aircrafts(rUCAviation.aircrafts),
      szParadropSquadName(rUCAviation.szParadropSquadName),
      nParadropSquadCount(rUCAviation.nParadropSquadCount),
      nRelaxTime(rUCAviation.nRelaxTime),
      vAppearPoints(rUCAviation.vAppearPoints) { Validate(); }

  SUCAviation &operator=(const SUCAviation &rUCAviation)
  {
    if (&rUCAviation != this)
    {
      aircrafts = rUCAviation.aircrafts;
      szParadropSquadName = rUCAviation.szParadropSquadName;
      nParadropSquadCount = rUCAviation.nParadropSquadCount;
      nRelaxTime = rUCAviation.nRelaxTime;
      vAppearPoints = rUCAviation.vAppearPoints;

      Validate();
    }
    return *this;
  }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    saver.Add("Aircrafts", &aircrafts);
    saver.Add("ParadropSoldierName", &szParadropSquadName);
    saver.Add("ParadropSoldierCount", &nParadropSquadCount);
    saver.Add("RelaxTime", &nRelaxTime);
    saver.Add("AppearPoints", &vAppearPoints);

    if (saver.IsReading()) { Validate(); }

    return 0;
  }

  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;

    saver.Add(1, &aircrafts);
    saver.Add(2, &szParadropSquadName);
    saver.Add(3, &nRelaxTime);
    saver.Add(4, &vAppearPoints);
    saver.Add(5, &nParadropSquadCount);

    if (saver.IsReading()) { Validate(); }

    return 0;
  }

  void Validate() { while (aircrafts.size() < AT_COUNT) { aircrafts.push_back(SUCAircraft()); } }
};

struct SUnitCreation
{
  SUCAviation aviation;// aircraft
  std::string szPartyName;// country name

  SUnitCreation() {}

  SUnitCreation(const SUnitCreation &rUnitCreation)
    : aviation(rUnitCreation.aviation), szPartyName(rUnitCreation.szPartyName) {}

  SUnitCreation &operator=(const SUnitCreation &rUnitCreation)
  {
    if (&rUnitCreation != this)
    {
      aviation = rUnitCreation.aviation;
      szPartyName = rUnitCreation.szPartyName;
    }
    return *this;
  }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    saver.Add("Aviation", &aviation);
    saver.Add("PartyName", &szPartyName);
    return 0;
  }

  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;

    saver.Add(1, &aviation);
    saver.Add(7, &szPartyName);

    return 0;
  }
};

struct SUnitCreationInfo
{
  enum UNIT_TYPE
  {
    UT_PLAYER = 0,
    UT_ENEMY = 1,
    UT_NEUTRAL = 2,
    UT_COUNT = 3,
  };

  static const char *DEFAULT_AIRCRAFT_NAME[SUCAviation::AT_COUNT];
  static const char *DEFAULT_PARADROP_SOLDIER_NAME;
  static const char *DEFAULT_PARTY_NAME;
  static const int DEFAULT_RELAX_TIME;
  // static const char* DEFAULT_MINE_AP;
  // static const char* DEFAULT_MINE_AT;
  // static const char* DEFAULT_ARTILLERY_CREW_NAME;
  // static const char* DEFAULT_ENGINEERS_NAME;
  // static const char* DEFAULT_TEMP_RU_STORAGE_NAME;
  // static const char* DEFAULT_ANTI_TANK_OBJECT_NAME;

  std::vector<SUnitCreation> units;

  SUnitCreationInfo() { Validate(); }

  SUnitCreationInfo(const SUnitCreationInfo &rUnitCreationInfo)
    : units(rUnitCreationInfo.units) { Validate(); }

  SUnitCreationInfo &operator=(const SUnitCreationInfo &rUnitCreationInfo)
  {
    if (&rUnitCreationInfo != this)
    {
      units = rUnitCreationInfo.units;
      Validate();
    }
    return *this;
  }

  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;

    saver.Add("Units", &units);
    if (saver.IsReading()) { Validate(); }

    return 0;
  }

  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;

    saver.Add(1, &units);
    if (saver.IsReading()) { Validate(); }

    return 0;
  }

  void Validate();
};


#endif // __FMT__UNIT_CREATION__H__


// basement storage

/* *
 */