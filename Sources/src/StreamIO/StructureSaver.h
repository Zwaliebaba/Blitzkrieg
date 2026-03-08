#ifndef __STRUCTURESAVER_H__
#define __STRUCTURESAVER_H__

// ************************************************************************************************************************ //
// **
// ** structure saver interface
// ** for writing data to the stream in the form of chunks
// **
// **
// ************************************************************************************************************************ //

using SSChunkID = char;
interface IStructureSaver : IRefCount
{
  enum EAccessMode
  {
    READ = 1,// open SS to read data from
    WRITE = 2// open SS to write data to
  };

  enum EStoreMode
  {
    ALL = 1,// store data and store objects with re-creation, etc... - complete save/load mode
    DATAONLY = 2// store data only. 
  };

  // start new complex chunk
  virtual bool STDCALL StartChunk(SSChunkID idChunk) = 0;
  // finish complex chunk
  virtual void STDCALL FinishChunk() = 0;
  // simply raw data chunk
  virtual void STDCALL DataChunk(SSChunkID idChunk, void *pData, int nSize) = 0;
  // data stream as data chunk
  virtual void STDCALL DataChunk(IDataStream *pStream) = 0;
  // count number of subchunks in the given chunk
  virtual int STDCALL CountChunks(SSChunkID idChunk) = 0;
  // set number of subchunks in the given chunk
  virtual void STDCALL SetChunkCounter(int nCount) = 0;
  // is structure saver opened in the READ mode?
  virtual bool STDCALL IsReading() const = 0;
  // loading an object and recreating it
  virtual IRefCount * STDCALL LoadObject() = 0;
  // recording the object and data needed to recreate it when loaded
  virtual void STDCALL StoreObject(IRefCount *pObj) = 0;
  // get a pointer to the game database
  virtual interface IGDB * STDCALL GetGDB() = 0;
};

// ************************************************************************************************************************ //
// **
// ** data tree interface
// ** for writing/reading data from a stream in the form of a tree.
// ** differs from IStructureSaver in that it is intended only for data (does not support object restoration)
// ** and also writes information into text files in a form convenient for later viewing in a third-party editor.
// ** By and large, this interface is redundant.
// ** But this is due to the fact that it is necessary to write data to a text file like this:
// ** so that later they can be conveniently read in some third-party viewer
// **
// ************************************************************************************************************************ //

using DTChunkID = const char *;
interface IDataTree : IRefCount
{
  enum EAccessMode
  {
    WRITE = 1,
    READ = 2
  };

  // is opened in the READ mode?
  virtual bool STDCALL IsReading() const = 0;
  // start new complex chunk
  virtual int STDCALL StartChunk(DTChunkID idChunk) = 0;
  // finish complex chunk
  virtual void STDCALL FinishChunk() = 0;
  // simply data chunk: text, integer, fp
  virtual int STDCALL GetChunkSize() = 0;
  virtual bool STDCALL RawData(void *pData, int nSize) = 0;
  virtual bool STDCALL StringData(char *pData) = 0;
  virtual bool STDCALL StringData(wchar_t*pData) = 0;
  virtual bool STDCALL DataChunk(DTChunkID idChunk, int *pData) = 0;
  virtual bool STDCALL DataChunk(DTChunkID idChunk, double *pData) = 0;
  // array data serialization (special case)
  virtual int STDCALL CountChunks(DTChunkID idChunk) = 0;
  virtual bool STDCALL SetChunkCounter(int nCount) = 0;
  virtual int STDCALL StartContainerChunk(DTChunkID idChunk) = 0;
  virtual void STDCALL FinishContainerChunk() = 0;
};

// ************************************************************************************************************************ //
// **
// ** save/load system
// ** creating a saver and registering objects derived from IRefCount
// ** for subsequent automatic creation of them upon loading
// **
// ************************************************************************************************************************ //

interface ISaveLoadSystem
{
  // common factory
  virtual void STDCALL AddFactory(IObjectFactory *pFactory) = 0;
  virtual IObjectFactory * STDCALL GetCommonFactory() = 0;
  virtual void STDCALL SetGDB(IGDB *pGDB) = 0;
  // structure and text tree savers
  virtual IStructureSaver * STDCALL CreateStructureSaver(IDataStream *pStream, IStructureSaver::EAccessMode eAccessMode,
                                                         interface IProgressHook *pLoadHook = nullptr) = 0;
  virtual IDataTree * STDCALL CreateDataTreeSaver(IDataStream *pStream, IDataTree::EAccessMode eAccessMode,
                                                  DTChunkID idBaseNode = "base") = 0;
  // storage opening/creating
  virtual IDataStorage * STDCALL OpenStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) = 0;
  virtual IDataStorage * STDCALL CreateStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) = 0;
  // database and data table opening
  virtual IDataBase * STDCALL OpenDataBase(const char *pszName, DWORD dwAccessMode, DWORD type = DB_TYPE_INI) = 0;
  virtual IDataTable * STDCALL OpenDataTable(IDataStream *pStream, const char *pszBaseNode = "base") = 0;
  virtual IDataTable * STDCALL OpenIniDataTable(IDataStream *pStream) = 0;
};

// functions for working with the save/load system and the global factory
extern ISaveLoadSystem *g_pGlobalSaveLoadSystem;
inline ISaveLoadSystem *GetSLS() { return g_pGlobalSaveLoadSystem; }
// save/load system
inline IStructureSaver *CreateStructureSaver(IDataStream *pStream, IStructureSaver::EAccessMode eAccessMode,
                                             interface IProgressHook *pLoadHook = nullptr) { return GetSLS()->CreateStructureSaver(pStream, eAccessMode, pLoadHook); }

inline IDataTree *CreateDataTreeSaver(IDataStream *pStream, IDataTree::EAccessMode eAccessMode,
                                      DTChunkID idBaseNode = "base") { return GetSLS()->CreateDataTreeSaver(pStream, eAccessMode, idBaseNode); }

// global factory
inline void AddFactory(IObjectFactory *pFactory) { GetSLS()->AddFactory(pFactory); }
inline IObjectFactory *GetCommonFactory() { return GetSLS()->GetCommonFactory(); }

template<class TYPE>
TYPE *CreateObject(IObjectFactory *pFactory, int nTypeID) { return static_cast<TYPE *>(pFactory->CreateObject(nTypeID)); }

template<class TYPE>
TYPE *CreateObject(int nTypeID) { return CreateObject<TYPE>(GetCommonFactory(), nTypeID); }

// ************************************************************************************************************************ //
// **
// ** open/create storage/file helper functions
// **
// **
// **
// ************************************************************************************************************************ //

inline IDataStorage *OpenStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) { return GetSLS()->OpenStorage(pszName, dwAccessMode, type); }

inline IDataStorage *CreateStorage(const char *pszName, DWORD dwAccessMode, DWORD type = STORAGE_TYPE_FILE) { return GetSLS()->CreateStorage(pszName, dwAccessMode, type); }

inline IDataStream *OpenFileStream(const std::string &szFullName, DWORD dwAccessMode)
{
  const int nPos = szFullName.rfind('\\');
  const std::string szPathName = nPos != std::string::npos ? szFullName.substr(0, nPos + 1) : ".\\";
  const std::string szFileName = nPos != std::string::npos ? szFullName.substr(nPos + 1) : szFullName;
  CPtr<IDataStorage> pStorage = OpenStorage(szPathName.c_str(), dwAccessMode, STORAGE_TYPE_FILE);
  return pStorage->OpenStream(szFileName.c_str(), dwAccessMode);
}

inline IDataStream *CreateFileStream(const std::string &szFullName, DWORD dwAccessMode)
{
  const int nPos = szFullName.rfind('\\');
  const std::string szPathName = nPos != std::string::npos ? szFullName.substr(0, nPos + 1) : ".\\";
  const std::string szFileName = nPos != std::string::npos ? szFullName.substr(nPos + 1) : szFullName;
  CPtr<IDataStorage> pStorage = CreateStorage(szPathName.c_str(), dwAccessMode, STORAGE_TYPE_FILE);
  return pStorage->CreateStream(szFileName.c_str(), dwAccessMode);
}

// ************************************************************************************************************************ //
// **
// ** open database/datatable
// **
// **
// **
// ************************************************************************************************************************ //

inline IDataBase *OpenDataBase(const char *pszName, DWORD dwAccessMode, DWORD type = DB_TYPE_INI) { return GetSLS()->OpenDataBase(pszName, dwAccessMode, type); }
inline IDataTable *OpenDataTable(IDataStream *pStream, const char *pszBaseNode = "base") { return GetSLS()->OpenDataTable(pStream, pszBaseNode); }
inline IDataTable *OpenIniDataTable(IDataStream *pStream) { return GetSLS()->OpenIniDataTable(pStream); }

#ifndef _DONT_USE_SINGLETON
namespace NDB
{
  inline IDataTable *OpenDataTable(const char *pszName)
  {
    CPtr<IDataStream> pStream = GetSingleton<IDataStorage>()->OpenStream(pszName, STREAM_ACCESS_READ);
    NI_ASSERT_TF(pStream != 0, NStr::Format("Can't open stream \"%s\"", pszName), return 0);
    return ::OpenDataTable(pStream);
  }
};
#endif // _DONT_USE_SINGLETON

#endif // __STRUCTURESAVER_H__