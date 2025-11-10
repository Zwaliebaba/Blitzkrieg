#ifndef __STATSYSTEM_H__
#define __STATSYSTEM_H__

#pragma once

struct SStatEntry
{
  std::string szName;// entry name
  std::string szValue;// value...
  double fCurr;// current value
  double fMin, fAve, fMax;// min/max/ave values
  //
  SStatEntry() : fCurr(0), fMin(0), fAve(0), fMax(0) {}
  SStatEntry(const char *pszName) : szName(pszName), fCurr(0), fMin(0), fAve(0), fMax(0) {}
  //
  int operator&(IStructureSaver &ss);
};

class CStatSystem : public IStatSystem
{
  OBJECT_NORMAL_METHODS(CStatSystem);
  DECLARE_SERIALIZE;
  //
  using CEntriesList = std::list<SStatEntry>;
  using CEntriesPtrMap = std::hash_map<std::string, SStatEntry *>;
  CEntriesList entriesList;// entries list for fast sequential access and sequence ordering
  CEntriesPtrMap entriesMap;// entries map for fast access
  //
  int nPosX, nPosY;// screen coordinates
public:
  // add/remove statistics entry
  void STDCALL AddEntry(const char *pszName) override
  {
    if (entriesMap.find(pszName) == entriesMap.end())
    {
      // add to list
      entriesList.push_back(SStatEntry(pszName));
      // add to map
      entriesMap[pszName] = &(entriesList.back());
    }
  }

  void STDCALL RemoveEntry(const char *pszName) override
  {
    if (entriesMap.find(pszName) != entriesMap.end())
    {
      // remove from map
      entriesMap.erase(pszName);
      // remove from list
      for (auto it = entriesList.begin(); it != entriesList.end(); ++it)
      {
        if (it->szName == pszName)
        {
          entriesList.erase(it);
          break;
        }
      }
    }
  }

  // update entry
  void STDCALL UpdateEntry(const char *pszName, double val) override
  {
    CEntriesPtrMap::iterator it = entriesMap.find(pszName);
    // add entry, if still not exist
    if (it == entriesMap.end())
    {
      AddEntry(pszName);
      it = entriesMap.find(pszName);
    }
    // update data
    it->second->fCurr = val;
    it->second->szValue = NStr::Format("%g", it->second->fCurr);
  }

  void STDCALL UpdateEntry(const char *pszName, const char *pszVal) override
  {
    CEntriesPtrMap::iterator it = entriesMap.find(pszName);
    // add entry, if still not exist
    if (it == entriesMap.end())
    {
      AddEntry(pszName);
      it = entriesMap.find(pszName);
    }
    // update data
    it->second->szValue = pszVal;
  }

  void STDCALL ResetEntry(const char *pszName) override
  {
    CEntriesPtrMap::iterator it = entriesMap.find(pszName);
    if (it != entriesMap.end())
    {
      it->second->fMin = it->second->fMax = it->second->fAve = it->second->fCurr = 0;
      it->second->szValue.clear();
    }
  }

  //
  void STDCALL SetPosition(int _nX, int _nY) override { nPosX = _nX, nPosY = _nY; }
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return true; }
  // drawing
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
};

#endif // __STATSYSTEM_H__