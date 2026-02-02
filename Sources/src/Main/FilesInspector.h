#ifndef __FILESINSPECTOR_H__
#define __FILESINSPECTOR_H__

#pragma once

#include "iMain.h"

class CFilesInspector : public CTRefCount<IFilesInspector>
{
  OBJECT_SERVICE_METHODS(CFilesInspector);
  DECLARE_SERIALIZE;
  //
  using SEntry = std::pair<std::string, CObj<IFilesInspectorEntry>>;
  using CEntriesList = std::list<SEntry>;
  CEntriesList entries;

public:
  bool AddEntry(const std::string &szName, IFilesInspectorEntry *pEntry) override;
  bool RemoveEntry(const std::string &szName) override;
  IFilesInspectorEntry * GetEntry(const std::string &szName) override;
  //
  bool InspectStorage(IDataStorage *pStorage) override;
  // clear all entries
  void Clear() override;
};

class CFilesInspectorEntryGDB : public CTRefCount<IFilesInspectorEntry>
{
  OBJECT_SERVICE_METHODS(CFilesInspectorEntryGDB);
  //
public:
  void InspectStream(const std::string &szName) override {}
  void Clear() override {}
};

class CFilesInspectorEntryCollector : public CTRefCount<IFilesInspectorEntryCollector>
{
  OBJECT_SERVICE_METHODS(CFilesInspectorEntryCollector);
  DECLARE_SERIALIZE;
  //
  std::vector<std::string> szNames;// collected names
  std::string szPrefix;// prefix name
  std::string szSuffix;// suffix name
  using ADD_IF_MATCHED = void(CFilesInspectorEntryCollector::*)(const std::string &szName);
  ADD_IF_MATCHED pfnAddIfMatched;// match function
  int nMatchType;// match function type
  //
  void AddIfPrefixMatched(const std::string &szName);
  void AddIfSuffixMatched(const std::string &szName);
  void AddIfBothMatched(const std::string &szName);
  //
  void SetMatchFunction(int _nMatchType);

public:
  CFilesInspectorEntryCollector()
    : pfnAddIfMatched(nullptr), nMatchType(-1) {}

  //
  void Configure(const char *pszConfig) override;
  const std::vector<std::string> & GetCollected() const override { return szNames; }
  void InspectStream(const std::string &szName) override;
  // clear entry
  void Clear() override { szNames.clear(); }
};

#endif // __FILESINSPECTOR_H__