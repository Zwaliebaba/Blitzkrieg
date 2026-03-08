#ifndef __DATABASE_H__
#define __DATABASE_H__

#pragma once

class CIniFileDataBase : public IDataBase
{
  OBJECT_MINIMAL_METHODS(CIniFileDataBase);
  //
  std::string szBase;
  DWORD dwStorageAccessMode;

public:
  CIniFileDataBase(const char *pszName, DWORD dwAccessMode);
  // create and open a table with the specified name and access rights
  IDataTable * STDCALL CreateTable(const char *pszName, DWORD dwAccessMode) override;
  // open an existing table with the specified name and access rights
  IDataTable * STDCALL OpenTable(const char *pszName, DWORD dwAccessMode) override;
  // kill storage element
  bool STDCALL DestroyElement(const char *pszName) override;
  // rename element
  bool STDCALL RenameElement(const char *pszOldName, const char *pszNewName) override;
};

#endif // __DATABASE_H__