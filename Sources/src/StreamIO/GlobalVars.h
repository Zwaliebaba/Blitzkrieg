#pragma once

#include "../Misc/FileUtils.h"

class CGlobalVars : public IGlobalVars
{
  OBJECT_NORMAL_METHODS(CGlobalVars);
  //
  using CValuesMap = std::unordered_map<std::string, std::string>;
  CValuesMap values;

  using CWValuesMap = std::unordered_map<std::string, std::wstring>;
  CWValuesMap wValues;

public:
  const char * STDCALL GetVar(const char *pszValueName) const override
  {
    CValuesMap::const_iterator pos = values.find(pszValueName);
    return pos == values.end() ? nullptr : pos->second.c_str();
  }

  void STDCALL SetVar(const char *pszValueName, const char *pszValue) override { values[pszValueName] = pszValue; }

  void STDCALL RemoveVar(const char *pszValueName) override { values.erase(pszValueName); }

  void STDCALL RemoveVarsByMatch(const char *pszValueMatch) override
  {
    const int nMatchLen = strlen(pszValueMatch);
    // search in values
    for (CValuesMap::iterator it = values.begin(); it != values.end();)
    {
      if (it->first.compare(0, nMatchLen, pszValueMatch) == 0) values.erase(it++);
      else ++it;
    }
    // search in wValues
    for (CWValuesMap::iterator it = wValues.begin(); it != wValues.end();)
    {
      if (it->first.compare(0, nMatchLen, pszValueMatch) == 0) wValues.erase(it++);
      else ++it;
    }
  }

  void STDCALL SerializeVarsByMatch(IDataTree *pSS, const char *pszValueMatch) override
  {
    CTreeAccessor saver = pSS;

    if (!saver.IsReading())
    {
      const int nMatchLen = strlen(pszValueMatch);
      CValuesMap valuesToSave;
      for (CValuesMap::const_iterator it = values.begin(); it != values.end(); ++it) { if (it->first.compare(0, nMatchLen, pszValueMatch) == 0) valuesToSave[it->first] = it->second; }
      saver.Add("GlobalVars", &valuesToSave);

      CWValuesMap wValuesToSave;
      for (CWValuesMap::const_iterator wIt = wValues.begin(); wIt != wValues.end(); ++wIt) { if (wIt->first.compare(0, nMatchLen, pszValueMatch) == 0) wValuesToSave[wIt->first] = wIt->second; }
      saver.Add("GlobalWVars", &wValuesToSave);
    }
    else
    {
      CValuesMap loadedValues;
      saver.Add("GlobalVars", &loadedValues);
      for (CValuesMap::const_iterator iter = loadedValues.begin(); iter != loadedValues.end(); ++iter)
      {
        const std::string name = iter->first;
        const std::string value = iter->second;
        values[name] = value;
      }

      CWValuesMap loadedWValues;
      saver.Add("GlobalWVars", &loadedWValues);
      for (CWValuesMap::const_iterator wIter = loadedWValues.begin(); wIter != loadedWValues.end(); ++wIter) wValues[wIter->first] = wIter->second;
    }
  }

  //
  void STDCALL SetVar(const char *pszValueName, const wchar_t*pszValue) override { wValues[pszValueName] = pszValue; }

  const wchar_t* STDCALL GetWVar(const char *pszValueName) const override
  {
    CWValuesMap::const_iterator pos = wValues.find(pszValueName);
    return pos == wValues.end() ? nullptr : pos->second.c_str();
  }

  void STDCALL RemoveWVar(const char *pszValueName) override { wValues.erase(pszValueName); }

  bool STDCALL DumpVars(const char *pszFileName) override
  {
    NFile::CFile file;
    if (file.Open(pszFileName, NFile::CFile::modeWrite | NFile::CFile::modeCreate))
    {
      std::map<std::string, std::string> sortmap;
      for (CValuesMap::const_iterator it = values.begin(); it != values.end(); ++it) sortmap[it->first] = it->second;
      for (std::map<std::string, std::string>::const_iterator it = sortmap.begin(); it != sortmap.end(); ++it)
      {
        const char *pszString = NStr::Format("%s = %s\n", it->first.c_str(), it->second.c_str());
        file.Write(pszString, strlen(pszString));
      }
      return true;
    }
    return false;
  }

  //
  int STDCALL operator&(IStructureSaver &ss) override
  {
    CSaverAccessor saver = &ss;
    // CRAP{ do not save video mode in save and options
    if (saver.IsReading())
    {
      std::list<std::pair<std::string, std::string>> vals2restore;
      for (CValuesMap::const_iterator it = values.begin(); it != values.end(); ++it) { if ((it->first.compare(0, 4, "GFX.") == 0) || (it->first.compare(0, 8, "Options.") == 0)) vals2restore.push_back(std::pair<std::string, std::string>(it->first, it->second)); }
      //
      saver.Add(1, &values);
      //
      for (std::list<std::pair<std::string, std::string>>::const_iterator it = vals2restore.begin(); it != vals2restore.end(); ++it) values[it->first] = it->second;
    }
    else
    {
      CValuesMap values1 = values;
      std::list<std::string> vals2erase;
      for (CValuesMap::const_iterator it = values1.begin(); it != values1.end(); ++it) { if ((it->first.compare(0, 4, "GFX.") == 0) || (it->first.compare(0, 8, "Options.") == 0)) vals2erase.push_back(it->first); }
      for (std::list<std::string>::const_iterator it = vals2erase.begin(); it != vals2erase.end(); ++it) values1.erase(*it);
      saver.Add(1, &values1);
    }
    // CRAP}
    saver.Add(2, &wValues);

    return 0;
  }
};
