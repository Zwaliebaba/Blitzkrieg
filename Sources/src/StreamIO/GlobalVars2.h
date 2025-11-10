#ifndef __GLOBALVARS2_H__
#define __GLOBALVARS2_H__

#include "../Misc/VarSystem.h"

// ************************************************************************************************************************ //
// **
// ** global vars system and iterator
// **
// **
// **
// ************************************************************************************************************************ //

interface IGlobalVarsIterator : IVarIterator {};

interface IGlobalVars2 : IVarSystem
{
  virtual IGlobalVarsIterator * STDCALL CreateIterator() const = 0;
  //
  virtual int STDCALL operator&(IDataTree &ss) = 0;
  int STDCALL operator&(IStructureSaver &ss) override = 0;
};

#endif // __GLOBALVARS2_H__