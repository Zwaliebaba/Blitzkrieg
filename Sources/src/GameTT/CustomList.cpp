#include "StdAfx.h"

#include "../Main/GameStats.h"
#include "CommonId.h"
#include "CustomList.h"

CInterfaceCustomList::~CInterfaceCustomList() {}

void CInterfaceCustomList::FillListFromCurrentDir()
{
  auto pList = checked_cast<IUIListControl *>(pUIScreen->GetChildByID(1000));
  NI_ASSERT(pList != 0);
  ITextManager *pTextM = GetSingleton<ITextManager>();
  IDataStorage *pStorage = GetSingleton<IDataStorage>();

  // remove all items from ListControl
  for (int i = pList->GetNumberOfItems() - 1; i >= 0; i--) { pList->RemoveItem(i); }

  // enumerate all available files and dirs
  dirsList.clear();
  filesList.clear();
  std::vector<std::string> dirs;// temporary directory storage
  std::vector<std::string> files;

  auto pCollector = checked_cast<IFilesInspectorEntryCollector *>(GetSingleton<IFilesInspector>()->GetEntry(szCollectorName.c_str()));
  const std::vector<std::string> &tutorialFiles = pCollector->GetCollected();
  std::unordered_set<std::string> setOfDirs;// here I will save all the directories

  for (int i = 0; i < tutorialFiles.size(); i++)
  {
    if (strncmp(tutorialFiles[i].c_str(), szCurrentDir.c_str(), szCurrentDir.size())) continue;// not equal

    // check that there are no nested directories on the right
    std::string szCurrentName = tutorialFiles[i].c_str() + szCurrentDir.size();
    int nPos = szCurrentName.rfind('\\');
    if (nPos != std::string::npos)// there are more directories to the right
    {
      // let's take the directory
      szCurrentName = szCurrentName.substr(0, szCurrentName.find('\\'));
      setOfDirs.insert(szCurrentName);
      continue;
    }
  }

  for (std::unordered_set<std::string>::iterator it = setOfDirs.begin(); it != setOfDirs.end(); ++it)
  {
    // find all non-empty subdirectories inside a given directory
    std::string szCmpDir = szCurrentDir + *it;
    szCmpDir += '\\';
    std::unordered_set<std::string> setOfSubDirs;
    for (int i = 0; i < tutorialFiles.size(); i++)
    {
      std::string szCurrentName = tutorialFiles[i];
      if (strncmp(szCurrentName.c_str(), szCmpDir.c_str(), szCmpDir.size())) continue;// not equal
      szCurrentName = szCurrentName.substr(szCmpDir.size());

      // Let's check that these are subdirectories¤
      int nPos = szCurrentName.rfind('\\');
      if (nPos != std::string::npos)// there are more directories to the right
      {
        szCurrentName = szCurrentName.substr(0, nPos);
        setOfSubDirs.insert(szCurrentName);
        continue;
      }

      // let's check what kind of file it is
      nPos = szCurrentName.rfind('.');
      if (nPos == std::string::npos) continue;

      std::string szExtension = szCurrentName.substr(nPos);
      for (int k = 0; k < fileMasks.size(); k++)
      {
        if (szExtension == fileMasks[k].c_str() + 1)
        {
          // the file mask matches, add to the list of files
          std::string szName = szCmpDir;
          szName += szCurrentName;
          files.push_back(szName);
        }
      }
    }

    for (std::unordered_set<std::string>::iterator it = setOfSubDirs.begin(); it != setOfSubDirs.end(); ++it) { dirs.push_back(*it); }
  }

  int nIndex = 0;
  if (szCurrentDir.size() > szTopDir.size()) dirs.push_back("..");

  // add strings to list control
  for (int i = 0; i < dirs.size(); i++)
  {
    pList->AddItem();
    IUIListRow *pRow = pList->GetItem(nIndex);
    pRow->SetUserData(nIndex);
    nIndex++;

    // set them to directories
    auto pContainer = checked_cast<IUIContainer *>(pRow->GetElement(0));
    dirsList.push_back(dirs[i]);
    std::wstring wszTemp;
    NStr::ToUnicode(&wszTemp, dirs[i]);
    pContainer->SetWindowText(0, wszTemp.c_str());

    IUIElement *pElement = pContainer->GetChildByID(1);
    NI_ASSERT_T(pElement != 0, "Invalid list control name dialog, it should contain icon");
    pElement->SetState(0);// directories¤

    /* std::string szFullName = szCurrentDir;
         */
  }

  for (int k = 0; k < files.size(); k++)
  {
    std::string szName = files[k];
    // We read the information from the file as if it contains SBasicGameStats
    if (pStorage->IsStreamExist(szName.c_str()) == false) continue;
    {
      const int nPos = szName.rfind('.');
      if (nPos != std::string::npos) szName.resize(nPos);
    }
    const SBasicGameStats *pStats = NGDB::GetGameStats<SBasicGameStats>(szName.c_str(), IObjectsDB::BASIC);
    if ((pStats == nullptr) || pStats->szHeaderText.empty()) continue;

    CPtr<IText> p = pTextM->GetDialog(pStats->szHeaderText.c_str());
    NI_ASSERT_T(p != 0, (std::string("Can not get text by key") + pStats->szHeaderText).c_str());
    if (!p) continue;

    // fill the internal list of file names
    filesList.push_back(szName.substr(szCurrentDir.size()));

    // add the localized name to the list on the screen
    pList->AddItem();
    IUIListRow *pRow = pList->GetItem(nIndex);
    pRow->SetUserData(nIndex);
    nIndex++;

    // set the file name
    auto pContainer = checked_cast<IUIContainer *>(pRow->GetElement(0));
    pContainer->SetWindowText(0, p->GetString());

    IUIElement *pElement = pContainer->GetChildByID(1);
    NI_ASSERT_T(pElement != 0, "Invalid list control name dialog, it should contain icon");
    pElement->SetState(1);// file
    // TODO instead of 1 put the corresponding mask

    FillListItem(pRow, szName);
  }

  pList->InitialUpdate();

  // Let's determine the currently selected element
  if (pList->GetNumberOfItems() < nBeginSelItem) nBeginSelItem = pList->GetNumberOfItems();
  pList->SetSelectionItem(nBeginSelItem);

  pUIScreen->Reposition(pGFX->GetScreenRect());
}

bool CInterfaceCustomList::ProcessMessage(const SGameMessage &msg)
{
  if (CInterfaceBaseList::ProcessMessage(msg)) return true;

  switch (msg.nEventID)
  {
    case IMC_CANCEL:
    {
      IMainLoop *pML = GetSingleton<IMainLoop>();
      CloseInterface();
      return true;
    }
      /* case IMC_OK:
             */
  }

  //
  return false;
}