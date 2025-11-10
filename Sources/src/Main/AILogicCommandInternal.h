#ifndef __AI_LOGIC_COMMAND_INTERNAL_H__
#define __AI_LOGIC_COMMAND_INTERNAL_H__

#pragma once

#include "AILogicCommand.h"
#include "NetMessages.h"
#include "../Common/Actions.h"
#include "../zlib/zconf.h"

class CRegisterGroupCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_REGISTER_GROUP };

private:
  OBJECT_COMPLETE_METHODS(CRegisterGroupCommand);
  DECLARE_SERIALIZE;
  //
  std::vector<int> unitsIDs;// IDs of all objects in group
  WORD wID;// ID of the group
public:
  CRegisterGroupCommand() {}
  CRegisterGroupCommand(IRefCount **pUnitsBuffer, int nLen, WORD wID, IAILogic *pAILogic);
  //
  void Execute(IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  //
  bool NeedToBeStored() const override { return true; }

  int STDCALL operator&(IDataTree &ss) override;
};

class CUnregisterGroupCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_UNREGISTER_GROUP };

private:
  OBJECT_COMPLETE_METHODS(CUnregisterGroupCommand);
  DECLARE_SERIALIZE;
  //
  WORD wGroup;// ID of the group
public:
  CUnregisterGroupCommand() {}
  CUnregisterGroupCommand(WORD wGroup);
  //
  void Execute(IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  //
  bool NeedToBeStored() const override { return true; }

  int STDCALL operator&(IDataTree &ss) override;
};

class CGroupCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_GROUP_COMMAND };

private:
  OBJECT_COMPLETE_METHODS(CGroupCommand);
  DECLARE_SERIALIZE;
  //
  SAIUnitCmd command;// command itself
  int nObjId;// object ID for 'target-object' command
  WORD wGroup;// group ID, this command for
  bool bPlaceInQueue;// do we need place this command in the group's queue
public:
  CGroupCommand() {}
  CGroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue, IAILogic *pAILogic);
  //
  void Execute(IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  //
  bool NeedToBeStored() const override { return true; }

  int STDCALL operator&(IDataTree &ss) override;
};

class CUnitCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_UNIT_COMMAND };

private:
  OBJECT_COMPLETE_METHODS(CUnitCommand);
  DECLARE_SERIALIZE;
  //
  SAIUnitCmd command;// command itself
  WORD wID;// group ID - result of this command :)
  int nPlayer;// player number
public:
  CUnitCommand() {}
  CUnitCommand(const struct SAIUnitCmd *pCommand, WORD wID, int _nPlayer);
  //
  void Execute(IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  //
  bool NeedToBeStored() const override { return true; }

  int STDCALL operator&(IDataTree &ss) override;
};

class CShowAreasCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_SHOW_AREAS };

private:
  OBJECT_COMPLETE_METHODS(CShowAreasCommand);
  DECLARE_SERIALIZE;
  //
  WORD wGroupID;// group ID to show area
  int nAreaType;// type of the area to show
  bool bShow;// show or hide area
public:
  CShowAreasCommand() {}

  CShowAreasCommand(const WORD _wGroupID, const int _nAreaType, const bool _bShow)
    : wGroupID(_wGroupID), nAreaType(_nAreaType), bShow(_bShow) {}

  //
  void Execute(IAILogic *pAILogic) override;
  // this class should not send data over the network!!!
  void Store(IDataStream *pStream) override {}
  void Restore(IDataStream *pStream) override {}
  //
  bool NeedToBeStored() const override { return true; }

  int STDCALL operator&(IDataTree &ss) override;
};

class CControlSumCheckCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_CHECK_SUM };

private:
  OBJECT_COMPLETE_METHODS(CControlSumCheckCommand);
  DECLARE_SERIALIZE;

  int nPlayer;
  uLong ulCheckSum;

  // won't save!
  static std::vector<std::list<uLong>> checkSums;

public:
  static WORD wMask;
  CControlSumCheckCommand() {}

  CControlSumCheckCommand(const int _nPlayer, const uLong _ulCheckSum)
    : nPlayer(_nPlayer), ulCheckSum(_ulCheckSum) {}

  void Execute(IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  //
  bool NeedToBeStored() const override { return false; }

  static void Check(int nOurNumber);
  static void Init(WORD wMask);

  int STDCALL operator&(IDataTree &ss) override;
};

class CDropPlayerCommand : public IAILogicCommand
{
public:
  enum { tidTypeID = NGM_ID_COMMAND_DROP_PLAYER };

private:
  OBJECT_COMPLETE_METHODS(CDropPlayerCommand);
  DECLARE_SERIALIZE;

  int nPlayerToDrop;

public:
  CDropPlayerCommand() : nPlayerToDrop(-1) {}
  explicit CDropPlayerCommand(const int _nPlayerToDrop) : nPlayerToDrop(_nPlayerToDrop) {}

  //
  void Execute(interface IAILogic *pAILogic) override;
  //
  void Store(IDataStream *pStream) override;
  void Restore(IDataStream *pStream) override;
  // Do I need to save it in the command history?
  bool NeedToBeStored() const override { return true; }

  // to serialize it in XML
  int STDCALL operator&(IDataTree &ss) override;
};

#endif // __AI_LOGIC_COMMAND_INTERNAL_H__