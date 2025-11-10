#include "StdAfx.h"

#include "FontManager.h"

#include "Font.h"

// remove all shared resource from this manager
void CFontManager::Clear(const EClearMode eMode, const int nUsage, const int nAmount)
{
  if (eMode == CLEAR_ALL) share.Clear();
  else share.ClearUnreferencedResources();
}

int CFontManager::operator&(IStructureSaver &ss)
{
  share.Serialize(&ss);
  return 0;
}