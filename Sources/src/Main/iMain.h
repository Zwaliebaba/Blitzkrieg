#pragma once

#include "iMainClassIDs.h"

#undef GetMessage

// ************************************************************************************************************************ //
// **
// ** main loop, interface and interface command
// **
// **
// **
// ************************************************************************************************************************ //

interface IInterfaceObject : IRefCount
{
  virtual void Step(bool bAppActive) = 0;
  virtual bool ProcessUIMessage(const SGameMessage &msg) = 0;
  virtual bool ProcessTextMessage(const STextMessage &msg) = 0;
  virtual bool GetMessage(SGameMessage *pMsg) = 0;
};

interface IInterfaceBase : IInterfaceObject
{
  virtual bool Init() = 0;
  virtual void Done() = 0;
  virtual void OnGetFocus(bool bFocus) = 0;
  // transition to this interface from another full screen
  virtual void StartInterface() = 0;
};

interface IInterfaceCommand : IRefCount
{
  virtual void Exec(interface IMainLoop *pML) = 0;
  virtual void Configure(const char *pszConfig) {}
  //
  virtual void SetDelayedTime(const NTimer::STime &timeToExecute) = 0;
  virtual NTimer::STime GetDelayedTime() const { return 0; }
};

//
interface IMainLoop : IRefCount
{
  enum { tidTypeID = MAIN_MAIN_LOOP };

  //
  virtual void ConfigureNet(int nAppID, int nPort) = 0;
  //
  virtual const char * GetBaseDir() const = 0;
  //
  virtual bool StepApp(bool bActive) = 0;// return false on exit state
  virtual void Command(IInterfaceCommand *pCommand) = 0;
  virtual void Command(int nCommandID, const char *pszConfiguration) = 0;
  // work with interfaces
  virtual void ResetStack() = 0;
  virtual void SetInterface(IInterfaceBase *pNewInterface) = 0;
  virtual void PushInterface(IInterfaceBase *pNewInterface) = 0;
  virtual void PopInterface() = 0;
  virtual IInterfaceBase * GetInterface() const = 0;
  //
  virtual void Pause(bool _bPause, int _nPauseReason) = 0;
  virtual bool IsPaused() const = 0;
  virtual void EnableMessageProcessing(bool bEnable) = 0;
  // clear all unreferenced resources
  virtual void ClearResources(bool bClearAll = false) = 0;
  // scenario tracker storing/restoring for history replaying
  virtual void StoreScenarioTracker() = 0;
  virtual void RestoreScenarioTracker() = 0;
  // for external savings
  virtual void SerializeConfig(bool bRead, DWORD dwSerialize) = 0;
  virtual void Serialize(IStructureSaver *pSS, interface IProgressHook *pHook = nullptr) = 0;
};

// ************************************************************************************************************************ //
// **
// ** file inspector
// **
// **
// **
// ************************************************************************************************************************ //

interface IFilesInspector : IRefCount
{
  enum { tidTypeID = MAIN_FILES_INSPECTOR };

  // add new entry
  virtual bool AddEntry(const std::string &szName, interface IFilesInspectorEntry *pEntry) = 0;
  // remove entry
  virtual bool RemoveEntry(const std::string &szName) = 0;
  // get entry
  virtual interface IFilesInspectorEntry * GetEntry(const std::string &szName) = 0;
  // inspect storage
  virtual bool InspectStorage(IDataStorage *pStorage) = 0;
  // clear all entries
  virtual void Clear() = 0;
};

interface IFilesInspectorEntry : IRefCount
{
  // inspect one stream name
  virtual void InspectStream(const std::string &szName) = 0;
  // clear entry
  virtual void Clear() = 0;
};

interface IFilesInspectorEntryCollector : IFilesInspectorEntry
{
  virtual void Configure(const char *pszConfig) = 0;
  virtual const std::vector<std::string> & GetCollected() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** main global functions
// **
// **
// **
// ************************************************************************************************************************ //

namespace NMain
{
  bool Initialize(HWND hWnd3D, HWND nWndInput, HWND hWndSound, bool bGame);
  bool Finalize();
  bool IsInitialized();
  bool CanLaunch();
  void SetupGlobalVarConsts(class CTableAccessor &table);
  const SModuleDescriptor * GetModuleDesc(int nType);
  int LoadAllModules(const char *pszPath);
  void UnloadAllModules();
  //
  bool SwitchGame(bool bOn);
  // modules iterating
  const SModuleDescriptor *GetFirstModuleDesc();
  const SModuleDescriptor *GetNextModuleDesc();
  const std::string GetModuleFileNameByDesc(const SModuleDescriptor *pModule);
  bool SetGameDirectory();
  bool CheckBetaKey();
};

IMainLoop * CreateMainLoop();
IObjectFactory * GetMainObjectFactory();

#define SERIALIZE_CONFIG_OPTIONS		0x00000001
#define SERIALIZE_CONFIG_BINDS			0x00000002
#define SERIALIZE_CONFIG_HELPCALLS	0x00000004
bool SerializeConfig(bool bRead, DWORD dwSerialize);

// ************************************************************************************************************************ //
// **
// ** automagic
// **
// **
// **
// ************************************************************************************************************************ //

interface IRPGStatsAutomagic : IRefCount
{
  virtual const char * ToStr(int nVal) const = 0;
  virtual const int ToInt(const char *pszVal) const = 0;

  virtual bool IsLastStr(const char *pszVal) const = 0;
  virtual bool IsLastInt(int nVal) const = 0;

  virtual const char * GetFirstStr() const = 0;
  virtual const int GetFirstInt() const = 0;

  virtual const char * GetNextStr(const char *pszVal) = 0;
  virtual const int GetNextInt(int nVal) = 0;
};
