#ifndef __GLOBALVARSINTERNAL_H__
#define __GLOBALVARSINTERNAL_H__

#pragma once

#include "GlobalVars2.h"
#include "../Misc/VarSystemInternal.h"

class CGlobalVars2 : public CTVarSystem<SSerialVariantT, CTRefCount<IGlobalVars2>>
{
  OBJECT_SERVICE_METHODS(CGlobalVars2);
  using CBase = CTVarSystem<SSerialVariantT, CTRefCount<IGlobalVars2>>;

public:
  CGlobalVars2();
  ~CGlobalVars2() override {}
  //
  IGlobalVarsIterator * STDCALL CreateIterator() const override;
};

struct SGlobalVarsSorter
{
  void Sort(std::list<CGlobalVars2::CVarsMap::const_iterator> &vals);
};

using CGlobalVarsIterator = CTVarSystemIterator<CGlobalVars2, CTRefCount<IGlobalVarsIterator>, SGlobalVarsSorter>;
/* class CGlobalVarsIterator : public CTVarSystemIterator<CGlobalVars2, CTRefCount<IGlobalVarsIterator>, SGlobalVarsSorter>
 */

#endif // __GLOBALVARSINTERNAL_H__