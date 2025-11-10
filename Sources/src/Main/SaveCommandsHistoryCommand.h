#ifndef __SAVE_COMMANDS_HISTORY_COMMAND_H__
#define __SAVE_COMMANDS_HISTORY_COMMAND_H__

#pragma once

#include "Transceiver.h"
#include "CommandsHistoryInterface.h"

class CSaveCommandsHistoryCommand : public IBaseCommand
{
  OBJECT_MINIMAL_METHODS(CSaveCommandsHistoryCommand);

  CPtr<ICommandsHistory> pHistory;

public:
  CSaveCommandsHistoryCommand(ICommandsHistory *_pHistory) : pHistory(_pHistory) {}
  // execute command
  void STDCALL Do() override { pHistory->Save(); }
  // un-execute command
  void STDCALL UnDo() override {}
  // can this command be un-executed
  bool STDCALL CanUnDo() override { return false; }
};

#endif // __SAVE_COMMANDS_HISTORY_COMMAND_H__