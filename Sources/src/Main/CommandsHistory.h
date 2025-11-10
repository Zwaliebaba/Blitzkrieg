#ifndef __COMMANDS_HISTORY_H__
#define __COMMANDS_HISTORY_H__

#pragma once

#include "CommandsHistoryInterface.h"

#include <zlib.h>

interface IAILogicCommand;
interface IRandomGenSeed;
interface IScenarioTracker;

class CCommandsHistory : public ICommandsHistory
{
  OBJECT_COMPLETE_METHODS(CCommandsHistory);
  DECLARE_SERIALIZE;

  using CHistory = std::hash_map<int, std::list<CPtr<IAILogicCommand>>>;
  CHistory savingHistory;
  CHistory loadedHistory;

  uLong startMapCheckSum;

  CPtr<IRandomGenSeed> pStartRandomSeed;
  CPtr<IScenarioTracker> pStartScenarioTracker;

  bool bLoadedFromCommandLine;
  bool bLoadedHistory;
  bool bStored;// scenario tracker stored

  uLong checkSumMap;
  uLong checkSumRes;

  struct SMPPlayerInfo
  {
    int nLogicID;
    int nSide;

    int operator&(IDataTree &ss)
    {
      CTreeAccessor saver = &ss;
      saver.Add("LogicID", &nLogicID);
      saver.Add("Side", &nSide);
      return 0;
    }
  };

  std::vector<SMPPlayerInfo> players;

  std::string szModName;
  std::string szModVersion;
  //
  void InvalidHistory(const char *pMessage);

public:
  CCommandsHistory() : startMapCheckSum(0), bLoadedFromCommandLine(false), bLoadedHistory(false), bStored(false) {}

  void STDCALL PrepareToStartMission() override;
  bool STDCALL LoadCommandLineHistory() override;
  bool STDCALL Load(const char *pszFileName) override;
  void STDCALL Save(const char *pszFileName) override;
  void STDCALL Clear() override;

  void STDCALL AddCommand(int nSegment, interface IAILogicCommand *pCmd) override;
  void STDCALL ExecuteSegmentCommands(int nSegment, interface ITransceiver *pTranceiver) override;
  void STDCALL CheckStartMapCheckSum(int nCheckSum) override;

  const int GetNumPlayersInMPGame() const override { return players.size(); }
  const int GetMPPlayerLogicID(int nPlayer) const override;
  const int GetMPPlayerSide(int nPlayer) const override;

  const char * STDCALL GetModName() const override { return szModName.c_str(); }
  const char * STDCALL GetModVersion() const override { return szModVersion.c_str(); }
};

#endif __COMMANDS_HISTORY_H__