#if !defined(__FMT__SOUND__H__)
#define __FMT__SOUND__H__


#pragma once

struct CMapSoundInfo
{
  std::string szName;// sound name
  CVec3 vPos;// home point

  // ------------------------------------------------------------------------------------------------------------------
  int operator&(IDataTree &ss);
  int operator&(IStructureSaver &ss);
};

using TMapSoundInfoList = std::vector<CMapSoundInfo>;

#endif // #if !defined(__FMT__SOUND__H__)