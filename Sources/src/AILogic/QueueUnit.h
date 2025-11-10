#ifndef __QUEUE_UNIT__
#define __QUEUE_UNIT__

#pragma once

class CAICommand;
interface IUnitState;

class CQueueUnit
{
  DECLARE_SERIALIZE;

  // command decks
  static CDecksSet<CObj<CAICommand>> cmds;

  // state
  CPtr<IUnitState> pState;

  bool bCmdFinished;// the state ended on its own

  CObj<CAICommand> pCmdCurrent;// the current command being executed by this unit

  NTimer::STime lastChangeStateTime;

  //
  void PopCmd(int nID);

public:
  void Init();

  virtual const WORD GetID() const = 0;

  bool IsEmptyCmdQueue() const;
  CAICommand *GetCurCmd() const;
  const bool IsCurCmdFinished() const { return bCmdFinished; }

  static void Clear();
  static void CheckCmdsSize(int id);
  static void DelCmdQueue(int id);

  virtual interface IStatesFactory *GetStatesFactory() const = 0;
  virtual IUnitState *GetState() const { return pState; }
  virtual void SetCurState(interface IUnitState *pState);

  // the current command is placed at the head of the queue, and pCommand is on top
  virtual void InsertUnitCommand(class CAICommand *pCommand);
  // the current command is interrupted, and pCommand is placed at the head of the queue
  virtual void PushFrontUnitCommand(class CAICommand *pCommand);
  virtual void UnitCommand(CAICommand *pCommand, bool bPlaceInQueue, bool bOnlyThisUnitCommand);

  void SetCommandFinished();
  virtual bool CanCommandBeExecuted(class CAICommand *pCommand) = 0;
  // can the command be executed based on the unit's stats
  virtual bool CanCommandBeExecutedByStats(class CAICommand *pCommand) = 0;

  void Segment();

  // returns the command on top of the command deck
  class CAICommand *GetNextCommand() const;
  class CAICommand *GetLastCommand() const;

  // clear the command queue and reset the current state
  void KillStatesAndCmdsInfo();
  virtual void SendAcknowledgement(EUnitAckType ack, bool bForce = false) = 0;
  virtual void SendAcknowledgement(CAICommand *pCommand, EUnitAckType ack, bool bForce = false) = 0;

  // queue the current pUnit command and all commands from its queue
  void InitWCommands(CQueueUnit *pUnit);

  virtual const NTimer::STime GetNextSegmTime() const { return 0; }
  // reset the time that passes between segment calls for a unit
  virtual void NullSegmTime() {}

  const NTimer::STime GetLastChangeStateTime() const { return lastChangeStateTime; }

  virtual void FreezeByState(bool bFreeze) = 0;

  const int GetBeginCmdsIter() const;
  const int GetEndCmdsIter() const;
  const int GetNextCmdsIter(int nIter) const;
  class CAICommand *GetCommand(int nIter) const;

  friend class CStaticMembers;
};

#endif // __QUEUE_UNIT__