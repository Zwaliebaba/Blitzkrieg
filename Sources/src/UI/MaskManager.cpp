#include "StdAfx.h"

#include "MaskManager.h"
#include "UIMask.h"

// ************************************************************************************************************************ //
// **
// ** mask manager
// **
// **
// **
// ************************************************************************************************************************ //

bool CMaskManager::Init()
{
  maskShare.Init();
  return true;
}

// remove all shared resource from this manager
void CMaskManager::Clear(const EClearMode eMode, const int nUsage, const int nAmount)
{
  if (eMode == CLEAR_ALL) maskShare.Clear();
  else maskShare.ClearUnreferencedResources();
}

int CMaskManager::operator&(IStructureSaver &ss)
{
  maskShare.Serialize(&ss);
  return 0;
}