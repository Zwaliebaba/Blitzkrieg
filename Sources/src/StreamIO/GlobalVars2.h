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
  virtual IGlobalVarsIterator * CreateIterator() const = 0;
  //
  virtual int operator&(IDataTree &ss) = 0;
  int operator&(IStructureSaver &ss) override = 0;
};

#endif // __GLOBALVARS2_H__