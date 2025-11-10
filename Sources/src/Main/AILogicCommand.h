#ifndef __AI_LOGIC_COMMAND__
#define __AI_LOGIC_COMMAND__

#pragma once

interface IAILogicCommand : IRefCount
{
  // to serialize it in XML
  virtual int STDCALL operator&(IDataTree &ss) = 0;
  //
  virtual void Execute(interface IAILogic *pAILogic) = 0;
  //
  virtual void Store(IDataStream *pStream) = 0;
  virtual void Restore(IDataStream *pStream) = 0;
  // Do I need to save it in the command history?
  virtual bool NeedToBeStored() const = 0;
};

#endif // __AI_LOGIC_COMMAND__