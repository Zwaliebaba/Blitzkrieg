#ifndef __BASE_LIST_H__
#define __BASE_LIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "../Misc/FileUtils.h"
#include "iMission.h"

class CInterfaceBaseList : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfaceBaseList);
  //
  std::vector<std::string> dirsList;// directory list
  std::vector<std::string> filesList;// file list
  std::list<int> stack;// stack for saving selected items, needed for moving up directories
  // input
  NInput::CCommandRegistrator commandMsgs;
  // private:
  virtual void FillListFromCurrentDir();
  friend class CInterfaceCustomList;

protected:
  // these variables are initialized in the constructors of derived classes
  std::vector<std::string> fileMasks;// for some lists there are multiple file types, so the array
  std::string szCurrentDir;// current directory
  std::string szTopDir;// we don’t go higher than this directory
  std::string szInterfaceName;// interface file name

  enum E_SORT_TYPES
  {
    E_SORT_BY_NAME,
    E_SORT_BY_TIME,
  };

  int nSortType;
  int bStorageFiles;
  int nFirstSortColumn;
  bool bOnlyDirs;// can enumerate only dirs
  int nBeginSelItem;
  bool bNotDiveIntoSubdirs;// don't display directories and don't allow to enter to subdirectories

  CInterfaceBaseList() : CInterfaceScreenBase("Current"), nSortType(0), bStorageFiles(false),
                         nFirstSortColumn(0), bOnlyDirs(false), nBeginSelItem(0), bNotDiveIntoSubdirs(false) {}

  // disable explicit destruction
  ~CInterfaceBaseList() override;

  bool STDCALL StepLocal(bool bAppActive) override;
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // functions I overload
  virtual bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr);// fill the current line in the list
  virtual bool OnOk(const std::string &szFullFileName);// the user has selected a file, let's process the selection
  virtual bool IsIgnoreSelection() const { return false; }// user may not select, but enter to edit box
  virtual bool OnOk() { return false; }// no selection, use edit box input.
  virtual void PrepareList(std::vector<std::string> *pFiles) {}// inspects list (may add something)
public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

/* class CICBaseList : public CInterfaceCommandBase<CInterfaceBaseList, MISSION_INTERFACE_BASE_LIST>
 */


// Functor for EnumerateFiles, enumerates all files in a directory and takes their relative name
class CGetAllDirsRelative
{
  std::vector<std::string> *pFileVector;
  std::string szInitDir;

public:
  CGetAllDirsRelative(const char *pszDir, std::vector<std::string> *pFiles) : pFileVector(pFiles), szInitDir(pszDir) {}
  //
  void operator()(const NFile::CFileIterator &it);
};


#endif // __BASE_LIST_H__