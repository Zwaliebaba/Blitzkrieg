#ifndef __SAVELOADSYSTEM_H__
#define __SAVELOADSYSTEM_H__

#pragma once

#include "../Misc/BasicObjectFactory.h"

class CSaveLoadSystem : public ISaveLoadSystem
{
  CBasicObjectFactory *pFactory;
  IGDB *pGDB;

public:
  CSaveLoadSystem();
  virtual ~CSaveLoadSystem();
  // common factory
  void AddFactory(IObjectFactory *pFactory) override;
  IObjectFactory * GetCommonFactory() override { return pFactory; }
  void SetGDB(IGDB *_pGDB) override { pGDB = _pGDB; }
  // structure and text tree savers
  IStructureSaver * CreateStructureSaver(IDataStream *pStream, IStructureSaver::EAccessMode eAccessMode,
                                                 interface IProgressHook *pLoadHook) override;
  IDataTree * CreateDataTreeSaver(IDataStream *pStream, IDataTree::EAccessMode eAccessMode, DTChunkID idBaseNode) override;
  // storage opening/creating
  IDataStorage * OpenStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) override;
  IDataStorage * CreateStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) override;
  // database and data table opening
  IDataBase * OpenDataBase(const char *pszName, DWORD dwAccessMode, DWORD type = DB_TYPE_INI) override;
  IDataTable * OpenDataTable(IDataStream *pStream, const char *pszBaseNode = "base") override;
  IDataTable * OpenIniDataTable(IDataStream *pStream) override;
};

#endif // __SAVELOADSYSTEM_H__