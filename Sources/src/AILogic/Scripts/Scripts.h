#ifndef __SCRIPT_FUNCTIONS_H__
#define __SCRIPT_FUNCTIONS_H__

#pragma once

#include "../../LuaLib/Script.h"
#include "../../Formats/fmtMap.h"
#include "../AIHashFuncs.h"

interface IUpdatableObj;
interface IScenarioUnit;

class CScripts
{
  DECLARE_SERIALIZE;

  static const int TIME_TO_CHECK_SUSPENDED_REINF;

  struct SScriptInfo
  {
    DECLARE_SERIALIZE;

  public:
    NTimer::STime period;
    NTimer::STime lastUpdate;
    int nRepetitions;

    std::string szName;

    SScriptInfo() : period(0), lastUpdate(0), nRepetitions(-1), szName("") {}
  };

  Script script;

  std::string szScriptFile;

  // active scripts
  std::unordered_map<int, SScriptInfo> activeScripts;
  // by the name of the script - himself
  std::unordered_map<std::string, int> name2script;

  // group number - units
  std::unordered_map<int, std::list<CPtr<IUpdatableObj>>> groups;

  // reinforcement number - reinforcement object
  struct SReinforcementObject
  {
    DECLARE_SERIALIZE;

  public:
    SMapObjectInfo mapObject;
    CGDBPtr<SHPObjectRPGStats> pStats;
    CPtr<IScenarioUnit> pScenarioUnit;

    SReinforcementObject() {}

    SReinforcementObject(const SMapObjectInfo &_mapObject, const SHPObjectRPGStats *_pStats, IScenarioUnit *_pScenarioUnit)
      : mapObject(_mapObject), pStats(_pStats), pScenarioUnit(_pScenarioUnit) {}
  };

  using CReinfList = std::list<SReinforcementObject>;
  std::unordered_map<int, CReinfList> reinforcs;
  // delayed (nowhere to put) reinforcements
  CReinfList suspendedReinforcs;
  CReinfList::iterator reinforcsIter;
  NTimer::STime lastTimeToCheckSuspendedReinforcs;

  std::unordered_map<int, int> reservePositions;

  // unit - script group number
  std::unordered_map<int, int> groupUnits;

  // for segment
  std::unordered_map<int, SScriptInfo>::iterator segmIter;

  std::unordered_map<std::string, SScriptArea> areas;

  bool bKill;

  CPtr<IConsoleBuffer> pConsole;
  bool bShowErrors;

  //
  bool ReadScriptFile();

  int KillActiveScript(std::string szName);

  // remove all invalid units at the beginning of this group
  void DelInvalidBegin(int targetId);

  // display an error message
  void OutScriptError(const char *pszString);

  // provide new links to reinforcements
  void SetNewLinksToReinforcement(CReinfList *pReinf, std::unordered_map<int, int> *pOld2NewLinks);
  //
  bool CanLandWithShift(const SMapObjectInfo &mapObject, IObjectsDB *pIDB, CVec2 *pvShift);
  bool CanFormationLand(const SMapObjectInfo &mapObject, IObjectsDB *pIDB, const CVec2 &vShift = VNULL2);
  bool CanUnitLand(const SMapObjectInfo &mapObject, IObjectsDB *pIDB, const CVec2 &vShift = VNULL2);

  void LandReinforcementWithoutLandCheck(CReinfList *pReinf, const CVec2 &vShift);
  void LandSuspendedReiforcements();

  //
  static int ProcessCommand(struct lua_State *state, bool bPlaceInQueue);

  //	
  interface ICheckObjects
  {
    virtual bool IsGoodObj(class CExistingObject *pObj) const = 0;
  };

  int GetCheckObjectsInScriptArea(const SScriptArea &area, const interface ICheckObjects &check);

  void SendShowReinoforcementPlacementFeedback(std::list<CVec2> *pCenters);

public:
  ~CScripts();

  int GetScriptID(IUpdatableObj *pObj) const;
  void AddObjToScriptGroup(IUpdatableObj *pObj, int nGroup);
  void AddUnitToReinforcGroup(const SMapObjectInfo &mapObject, int nGroup, const struct SHPObjectRPGStats *pStats, IScenarioUnit *pScenarioUnit);
  // remove all invalid units in the group,
  void DelInvalidUnits(int scriptId);

  void Init(const SLoadMapInfo &mapInfo);
  void InitAreas(const SScriptArea scriptAreas[], int nLen);
  void Load(const std::string &szScriptFile);

  void Segment();

  void CallScriptFunction(const char *pszCommand);

  //
  // script functions
  //
  static int Error_out(struct lua_State *state);

  //
  // params: <"name of script"> <periodicity> [<number of repetitions>]; 
  static int RunScript(struct lua_State *state);
  // params: none; 
  static int Suicide(struct lua_State *state);
  // params: <"name of script">; 
  static int KillScript(struct lua_State *state);
  //
  // params: <number of player> <x coord of the circle> <y coord> <radius>; 
  static int GetNUnitsInCircle(struct lua_State *state);
  //
  // params: <number of player> <"name of script area">; 
  static int GetNUnitsInArea(struct lua_State *state);
  //
  // params: <number of script group> <"name of script area">; 
  static int GetNScriptUnitsInArea(struct lua_State *state);
  //
  // params: <number of script group>; 
  static int GetNUnitsInScriptGroup(struct lua_State *state);
  //	
  // params: <number of reinforcement>; 
  static int LandReinforcement(struct lua_State *state);
  //
  // params: <number of winner party>; 
  static int Win(struct lua_State *state);

  // no parameters
  static int Draw(struct lua_State *state);
  //
  // params: none; 
  static int Loose(struct lua_State *state);
  //
  // params: <command> <script group id> <necessary command parameters>; 
  static int GiveCommand(struct lua_State *state);

  // params: <command> <script group id> <necessary command parameters>; 
  static int GiveQCommand(struct lua_State *state);

  // params: none; 
  static int ShowActiveScripts(struct lua_State *state);

  // params: <party of warfog>; 
  static int ChangeWarFog(struct lua_State *state);

  // params: <party of player : -1 - anyplayer> <type of aviation: -1 - any aviation>; 
  static int EnableAviation(struct lua_State *state);

  // params: <party of player : -1 - anyplayer> <type of aviation: -1 - any aviation>; 
  // double function because of compatibility.
  static int DisableAviation(struct lua_State *state);

  // params: <number of script group> <number of new player>; 
  static int ChangePlayer(struct lua_State *state);

  // params: <number of player> <number of mode>; 
  // nMode = 0 - remove god mode completely
  // nMode = 1 - indestructible
  // nMode = 2 - indestructible and kill on the first try
  // nMode = 3 - kill on the first try
  // nMode = 4 - remove only indestructibility
  // nMode = 5 - remove only the first kill
  static int God(struct lua_State *state);

  // params: <name of global var> <integer value of global var>; 
  static int SetIGlobalVar(struct lua_State *state);
  // params: <name of global var> <float value of global var>; 
  static int SetFGlobalVar(struct lua_State *state);
  // params: <name of global var> <string value of global var>; 
  static int SetSGlobalVar(struct lua_State *state);

  // params: <name of global var> <default interger value of global var>; 
  static int GetIGlobalVar(struct lua_State *state);
  // params: <name of global var> <default float value of global var>; 
  static int GetFGlobalVar(struct lua_State *state);
  // params: <name of global var> <default string value of global var>; 
  static int GetSGlobalVar(struct lua_State *state);

  // params: <object's script id>; 
  static int GetObjectHPs(struct lua_State *state);

  // params: <number of party>; 
  static int GetNUnitsInParty(struct lua_State *state);
  // the formation is counted as one unit
  static int GetNUnitsInPartyUF(struct lua_State *pState);
  // the formation is counted as one unit
  static int GetNUnitsInPlayerUF(struct lua_State *pState);

  // params: <script id of squad> <number of new formation>; 
  static int ChangeFormation(struct lua_State *state);

  // params: <format string> <float parameter>...<float parameter>; 
  // out trace info to the console
  static int Trace(struct lua_State *state);
  // params: <format string> <float parameter>...<float parameter>; 
  // out trace info on display
  static int DisplayTrace(struct lua_State *state);

  // params: <number of objective> <new value of objective>; 
  static int ObjectiveChanged(struct lua_State *state);

  // params: <script id of unit>; 
  static int GetNAmmo(struct lua_State *state);

  // params: <script id>; 
  static int GetPartyOfUnits(struct lua_State *state);

  static int ReserveAviationForTimes(struct lua_State *pState);

  // params: <script id> <damage value>; 
  // if damage == 0, then the object is destroyed
  // if damage < 0, then the object is healed
  static int DamageObject(struct lua_State *pState);

  // params: <script id>; 
  // returns 0, if state is unknown or not set
  // returns -1, if unit doesn't exist
  static int GetUnitState(struct lua_State *pState);

  // params: <script id>; 
  // returns -3, is the object doesn't exist,
  // returns -2, if it isn't a squad
  // returns -1, if it is a disbanded squad,
  // returns number of squad order, if it's a squad
  static int GetSquadInfo(struct lua_State *pState);

  // params: <script id>; 
  // returns -1, is the object doesn't exist or not a unit
  // returns 0, if isn't following
  // returns 1, if is following
  static int IsFollowing(struct lua_State *pState);

  // params: <script id>; 
  // returns -1 if unit doesn't exist or the object isn't a unit
  static int GetFrontDir(struct lua_State *pState);

  // params: <script id>; 
  static int IsWarehouseConnected(struct lua_State *pState);

  // params: <script id>; 
  static int IsUnitUnderSupply(struct lua_State *pState);
  // params: <script id>; 
  static int GetUnitMorale(struct lua_State *pState);

  // params: <script id>; 
  static int GetActiveShellType(struct lua_State *pState);

  // params: <request string>; 
  static int AskClient(struct lua_State *pState);

  // params: none; 
  static int RandomFloat(struct lua_State *pState);
  // params:n; 
  static int RandomInt(struct lua_State *pState);

  // params: <script id> <nParam>; 
  // nParam == 1 - select, nParam == 0 - deselect
  static int ChangeSelection(struct lua_State *pState);

  // params: <none>; 
  static int GetPlayersMask(struct lua_State *pState);
  // params: <player>; 
  static int IsPlayerPresent(struct lua_State *pState);

  // params: <script id>; 
  // if script group doesn't exist, return ( -1, -1 )
  static int GetObjCoord(struct lua_State *pState);

  // params: <name of script area>; 
  static int GetScriptAreaParams(struct lua_State *pState);

  // params: <bool>; 
  static int SwitchWeather(struct lua_State *pState);
  // params: <bool>; 
  static int SwitchWeatherAutomatic(struct lua_State *pState);

  // params: <diplomatic side>; 
  static int GetNUnitsInSide(struct lua_State *pState);

  // params: <script id>; 
  static int AddIronMan(struct lua_State *state);

  // params: <difficulty level>; 
  static int SetDifficultyLevel(struct lua_State *state);
  static int SetCheatDifficultyLevel(struct lua_State *state);

  // params: <script id of reinforcement to delete>; 
  static int DeleteReinforcement(struct lua_State *pState);

  // params: <"name of script area"> <1 - open, 0 - close>; 
  static int ViewZone(struct lua_State *pState);

  // params: <script id of unit>; 
  static int IsStandGround(struct lua_State *pState);
  // params: <script id of unit>; 
  static int IsEntrenched(struct lua_State *pState);

  // params: <script area name>; 
  static int GetNAPFencesInScriptArea(struct lua_State *pState);
  // params: <script area name>; 
  static int GetNAntitankInScriptArea(struct lua_State *pState);
  // params: <script area name>; 
  static int GetNFencesInScriptArea(struct lua_State *pState);
  // params: <script area name>; 
  static int GetNTrenchesInScriptArea(struct lua_State *pState);
  // params: <script area name>; 
  static int GetNMinesInScriptArea(struct lua_State *pState);

  // params player's number. 
  static int GetAviationState(struct lua_State *state);

  static int Password(struct lua_State *pState);
  //
  // for internal usage
  static int ReturnScriptIDs(struct lua_State *pState);

  //
  static int SetGameSpeed(struct lua_State *pState);

  // params: <name of unit type> <number of party>; 
  // works slowly!
  static int GetNUnitsOfType(struct lua_State *pState);
  // params: none; 
  static int GetMapSize(struct lua_State *pState);

  //
  // for debug
  // params: none; 
  static int CallAssert(struct lua_State *pState);

  //
  static Script::SRegFunction pRegList[];
};

#endif // __SCRIPT_FUNCTIONS_H__