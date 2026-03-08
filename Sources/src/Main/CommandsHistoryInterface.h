#ifndef __COMMANDS_HISTORY_INTERFACE_H__
#define __COMMANDS_HISTORY_INTERFACE_H__

#pragma once

#include "iMain.h"

interface ICommandsHistory : IRefCount
{
  // type ID
  enum { tidTypeID = MAIN_COMMANDS_HISTORY };

  // clearing everything where history is written before starting the mission,
  // remembering current RandomSeed and ScenarioTracker
  virtual void STDCALL PrepareToStartMission() = 0;

  // loading and initialization for multiplayer if -lh option is given
  virtual bool STDCALL LoadCommandLineHistory() = 0;

  // RandomSeed is loaded along with the commands themselves
  // return true if no error occurs
  virtual bool STDCALL Load(const char *pszFileName) = 0;
  // default option for testing only
  // if the name is empty, then what was set with the -sh parameter is used
  virtual void STDCALL Save(const char *pszFileName = "") = 0;

  virtual void STDCALL Clear() = 0;

  virtual void STDCALL AddCommand(int nSegment, interface IAILogicCommand *pCmd) = 0;
  virtual void STDCALL ExecuteSegmentCommands(int nSegment, interface ITransceiver *pTranceiver) = 0;
  virtual void STDCALL CheckStartMapCheckSum(int nCheckSum) = 0;

  virtual const int GetNumPlayersInMPGame() const = 0;
  virtual const int GetMPPlayerLogicID(int nPlayer) const = 0;
  virtual const int GetMPPlayerSide(int nPlayer) const = 0;

  virtual const char * STDCALL GetModName() const = 0;
  virtual const char * STDCALL GetModVersion() const = 0;
};

#endif // COMMANDS_HISTORY_INTERFACE_H__