#ifndef __INIFILE_H__
#define __INIFILE_H__

#pragma once

struct SEntry
{
  using TKey = std::string;
  using TVal = std::string;
  TKey key;
  TVal val;
  //
  SEntry() {}
  SEntry(TKey &_key, TVal &_val) : key(_key), val(_val) {}
  //
  SEntry &operator=(const SEntry &entry)
  {
    key = entry.key;
    val = entry.val;
    return *this;
  }

  SEntry &operator=(const TVal &_val)
  {
    val = _val;
    return *this;
  }
};

struct SRow
{
  using TKey = std::string;
  using TVal = SEntry;
  using CValList = std::list<TVal>;
  using CValMap = std::hash_map<TVal::TKey, TVal *>;
  TKey key;
  CValList elist;
  CValMap emap;
  //
  TVal &operator[](const TVal::TKey &key)
  {
    CValMap::iterator pos = emap.find(key);
    if (pos != emap.end()) return *(pos->second);
    elist.push_back(TVal());
    elist.back().key = key;
    emap[key] = &(elist.back());
    return elist.back();
  }

  //
  CValMap::iterator find(const TVal::TKey &key) { return emap.find(key); }
  CValMap::const_iterator find(const TVal::TKey &key) const { return emap.find(key); }
};

struct STable
{
  using TKey = std::string;
  using TVal = SRow;
  using CValList = std::list<TVal>;
  using CValMap = std::hash_map<TVal::TKey, TVal *>;
  TKey key;
  CValList elist;
  CValMap emap;
  //
  TVal &operator[](const TVal::TKey &key)
  {
    CValMap::iterator pos = emap.find(key);
    if (pos != emap.end()) return *(pos->second);
    elist.push_back(TVal());
    elist.back().key = key;
    emap[key] = &(elist.back());
    return elist.back();
  }

  //
  CValMap::iterator find(const TVal::TKey &key) { return emap.find(key); }
  CValMap::const_iterator find(const TVal::TKey &key) const { return emap.find(key); }
};

struct SIniFileEntry
{
  int nOrder;
  std::string szData;
};

class CIniFile : public IDataTable
{
  OBJECT_MINIMAL_METHODS(CIniFile);
  //
  std::string szIniFileName;// name of ini-file to read from
  DWORD dwAccessMode;
  //
  STable table;
  bool bChanged;
  //
  bool CanWrite() const { return dwAccessMode & TABLE_ACCESS_WRITE; }
  bool CanRead() const { return dwAccessMode & TABLE_ACCESS_READ; }

  const SRow *GetRow(const char *pszName) const
  {
    auto pos = table.find(pszName);
    return pos == table.emap.end() ? nullptr : pos->second;
  }

  SRow *GetRow(const char *pszName)
  {
    STable::CValMap::const_iterator pos = table.find(pszName);
    return pos == table.emap.end() ? nullptr : pos->second;
  }

  SEntry *GetEntry(const char *pszRow, const char *pszName)
  {
    SRow *pRow = GetRow(pszRow);
    if (!pRow) return nullptr;
    auto pos = pRow->find(pszName);
    return pos == pRow->emap.end() ? nullptr : pos->second;
  }

  const SEntry *GetEntry(const char *pszRow, const char *pszName) const
  {
    const SRow *pRow = GetRow(pszRow);
    if (!pRow) return nullptr;
    auto pos = pRow->find(pszName);
    return pos == pRow->emap.end() ? nullptr : pos->second;
  }

  //
  void LoadTables(const std::string &szString);

public:
  CIniFile()
    : bChanged(false) {}

  virtual ~CIniFile();
  //
  bool Open(const char *pszFileName, DWORD dwAccessMode);
  bool Load(IDataStream *pStream);
  //
  int STDCALL GetRowNames(char *pszBuffer, int nBufferSize) override;
  int STDCALL GetEntryNames(const char *pszRow, char *pszBuffer, int nBufferSize) override;
  // section cleaning
  void STDCALL ClearRow(const char *pszRowName) override
  {
    if (SRow *pRow = GetRow(pszRowName))
    {
      pRow->emap.clear();
      pRow->elist.clear();
    }
  }

  // get
  int STDCALL GetInt(const char *pszRow, const char *pszEntry, int defval) override;
  double STDCALL GetDouble(const char *pszRow, const char *pszEntry, double defval) override;
  const char * STDCALL GetString(const char *pszRow, const char *pszEntry, const char *defval, char *pszBuffer, int nBufferSize) override;
  int STDCALL GetRawData(const char *pszRow, const char *pszEntry, void *pBuffer, int nBufferSize) override;
  // set
  void STDCALL SetInt(const char *pszRow, const char *pszEntry, int val) override;
  void STDCALL SetDouble(const char *pszRow, const char *pszEntry, double val) override;
  void STDCALL SetString(const char *pszRow, const char *pszEntry, const char *val) override;
  void STDCALL SetRawData(const char *pszRow, const char *pszEntry, const void *pBuffer, int nBufferSize) override;
};

#endif // __INIFILE_H__