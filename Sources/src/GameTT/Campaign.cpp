#include "StdAfx.h"

#include "Campaign.h"

#include "../Main/gamestats.h"
#include "../Main/ScenarioTracker.h"
#include "MainMenu.h"
#include "CommonId.h"
#include "UIConsts.h"
#include "../StreamIO/OptionSystem.h"

enum ENewCommands
{
  IMC_NEXT_CHAPTER = 10003,
  IMC_TOTAL_ENCYCLOPEDIA = 10004,
  IMC_SAVE = 10005,
  IMC_SHOW_NEXT_CHAPTER_DIALOG = 8888,
  IMC_NEXT_CHAPTER_CONFIRM = 8889,
};

static constexpr NInput::SRegisterCommandEntry commands[] =
{
    {"inter_ok", IMC_OK},
    {"inter_cancel", IMC_CANCEL},
    {nullptr, 0}
};

CInterfaceCampaign::~CInterfaceCampaign() {}

int CInterfaceCampaign::operator &(interface IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.AddTypedSuper(1, static_cast<CInterfaceInterMission *>(this));

  saver.Add(2, &chapterNameToButtonIndexMap);

  return 0;
}

const SCampaignStats *CInterfaceCampaign::ReadCampaignStats()
{
  const int nCampaign = GetGlobalVar("Campaign.Current", 0);

  std::string szPartyName = CUIConsts::GetPartyNameByNumber(nCampaign);
  std::string szCampaignName = "scenarios\\campaigns\\" + szPartyName + "\\" + szPartyName;

  // upload campaign information
  const SCampaignStats *pStats = NGDB::GetGameStats<SCampaignStats>(szCampaignName.c_str(), IObjectsDB::CAMPAIGN);
  return pStats;
}

bool CInterfaceCampaign::Init()
{
  CInterfaceInterMission::Init();
  commandMsgs.Init(pInput, commands);

  return true;
}

void CInterfaceCampaign::PlayCampaignMusic()
{
  const SCampaignStats *pStats = ReadCampaignStats();
  NI_ASSERT_T(pStats != 0, "Invalid campaign stats");
  if (pStats == nullptr) return;

  if (pStats->szInterfaceMusic.size() > 0)
  {
    // start the sound
    int nTimeToFade = GetGlobalVar("Sound.TimeToFade", 5000);
    GetSingleton<IScene>()->SetSoundSceneMode(ESSM_INTERMISSION_INTERFACE);
    GetSingleton<ISFX>()->PlayStream(pStats->szInterfaceMusic.c_str(), true, nTimeToFade);
  }
}

void CInterfaceCampaign::StartInterface()
{
  CInterfaceInterMission::StartInterface();
  pUIScreen = CreateObject<IUIScreen>(UI_SCREEN);
  pUIScreen->Load("ui\\common\\campaign");
  pUIScreen->Reposition(pGFX->GetScreenRect());

  // upload campaign information
  const SCampaignStats *pStats = ReadCampaignStats();
  NI_ASSERT_T(pStats != 0, "Invalid campaign stats");
  if (pStats == nullptr) return;

  PlayCampaignMusic();

  // set the correct size for the map image control
  auto pMap = checked_cast<IUIContainer *>(pUIScreen->GetChildByID(100));
  IGFXTexture *pTexture = GetSingleton<ITextureManager>()->GetTexture(pStats->szMapImage.c_str());
  NI_ASSERT_T(pTexture != 0, "Campaign map texture is invalid");
  pMap->SetWindowTexture(pTexture);
  CTRect<float> rc(0.0f, 0.0f, pStats->mapImageRect.x2, pStats->mapImageRect.y2);
  pMap->SetWindowMap(rc);
  pMap->SetWindowPlacement(nullptr, &CVec2(pStats->mapImageRect.x1, pStats->mapImageRect.y1));

  std::string szCampaignName;

  {
    // Incrementing the global variable for the number of visits in Campaign
    std::string szVarName = "Campaign.";
    // const int nCampaign = GetGlobalVar( "campaign", 0 );
    // std::string szPartyName = CUIConsts::GetPartyNameByNumber( nCampaign );
    const std::string szPartyName = GetSingleton<IScenarioTracker>()->GetUserPlayer()->GetGeneralSide();
    szCampaignName = "scenarios\\campaigns\\" + szPartyName + "\\" + szPartyName;
    szVarName += szCampaignName;
    szVarName += ".Visited";
    int nVisitCount = GetGlobalVar(szVarName.c_str(), 0);
    nVisitCount++;
    SetGlobalVar(szVarName.c_str(), nVisitCount);

    if (nVisitCount == 1)
    {
      // This is the first entry into the campaign, you need to read all the chapters and set their states
      int nActiveChapterIndex = -1;
      for (int i = 0; i < pStats->chapters.size(); i++)
      {
        std::string szChapterVarName = pStats->chapters[i].szChapter;
        NStr::ToLower(szChapterVarName);
        szChapterVarName = "Chapter." + szChapterVarName;
        szChapterVarName += ".Status";

        if (pStats->chapters[i].bVisible)
        {
          if (nActiveChapterIndex == -1)
          {
            nActiveChapterIndex = i;
            SetGlobalVar(szChapterVarName.c_str(), 1);
          }
          else SetGlobalVar(szChapterVarName.c_str(), 0);
        }
        else SetGlobalVar(szChapterVarName.c_str(), 0);
      }

      NI_ASSERT_T(nActiveChapterIndex != -1, "Can not choose chapter to select, possibly they are all invisible");
      SetGlobalVar("Chapter.Current.Name", pStats->chapters[nActiveChapterIndex].szChapter.c_str());
    }
  }

  // Load chapters
  CPtr<IDataStream> pFlagStream = GetSingleton<IDataStorage>()->OpenStream("ui\\common\\chapterbutton.xml", STREAM_ACCESS_READ);
  CPtr<IDataTree> pDT = CreateDataTreeSaver(pFlagStream, IDataTree::READ);
  CTreeAccessor saver = pDT;
  ITextManager *pTM = GetSingleton<ITextManager>();

  chapterNameToButtonIndexMap.clear();
  int nChapterIndex = 0;
  int nCheatEnabledChapters = GetGlobalVar("Cheat.Enable.Chapters", -1);
  for (int i = 0; i < pStats->chapters.size(); ++i)
  {
    int nChapterStatus = 0;
    // 0 if invisible
    // 1 if available for allocation
    // 2 if already passed, then it is visible, but cannot be selected

    if (nCheatEnabledChapters != -1) nChapterStatus = 1;

    if (!nChapterStatus)
    {
      // let's take a global var with information about this chapter
      std::string szVarName = pStats->chapters[i].szChapter;
      NStr::ToLower(szVarName);
      szVarName = "Chapter." + szVarName;
      szVarName += ".Status";
      nChapterStatus = GetGlobalVar(szVarName.c_str(), 0);
    }

    if (!nChapterStatus) continue;

    CPtr<IUIElement> pChapterButton;
    saver.Add("Element", &pChapterButton);

    CVec2 pos, size;
    CTRect<float> rect;
    pChapterButton->GetWindowPlacement(&pos, &size, &rect);

    CVec2 vPos = pStats->chapters[i].vPosOnMap;
    vPos.x -= size.x / 2;
    vPos.y -= size.y / 2;
    pChapterButton->SetWindowPlacement(&vPos, nullptr);
    pMap->AddChild(pChapterButton);

    if (nChapterStatus == 2)
    {
      // have already completed this chapter
      pChapterButton->SetWindowID(999);
      pChapterButton->EnableWindow(false);
    }
    else
    {
      // according to the new concept, only the current chapter is visible, and the new available chapter, if there is one, is invisible
      std::string szCurrentChapterName = GetGlobalVar("Chapter.Current.Name", "");
      NI_ASSERT_T(szCurrentChapterName.size() > 0, "There is no global var Chapter.Current.Name");
      // NI_ASSERT_T( pStats->chapters[i].szChapter == szCurrentChapterName, "Error: " )
      pChapterButton->SetWindowID(1000 + nChapterIndex);
      chapterNameToButtonIndexMap[pStats->chapters[i].szChapter] = pChapterButton->GetWindowID();
      nChapterIndex++;

      // if cheats are enabled, then all chapters are visible
      if (nCheatEnabledChapters == -1 && pStats->chapters[i].szChapter != szCurrentChapterName) { pChapterButton->ShowWindow(UI_SW_HIDE); }
    }
  }
  SetGlobalVar("NumberOfButtons", nChapterIndex - 1);

  do
  {
    // Let's check if the transition to the next chapter is available, then the Next button becomes available
    if (nCheatEnabledChapters != -1) break;

    std::string szVarName = "Chapter.New.Available";
    std::string szNewChapter = GetGlobalVar(szVarName.c_str(), "");
    if (szNewChapter.size() == 0) break;

    IUIElement *pButton = pUIScreen->GetChildByID(10003);
    NI_ASSERT_T(pButton != 0, "There is no Next Chapter button");
    pButton->EnableWindow(true);
  } while (false);

  // set the title text
  IUIElement *pHeader = pUIScreen->GetChildByID(20000);
  NI_ASSERT_T(pHeader != 0, "Invalid campaign header control");
  CPtr<IText> p2 = pTM->GetDialog(pStats->szHeaderText.c_str());
  if (p2) { pHeader->SetWindowText(0, p2->GetString()); }

  {
    // set the active chapter button to the active state
    std::string szChapterName = GetGlobalVar("Chapter.Current.Name", "");
    NI_ASSERT_T(szChapterName.size() > 0, "There is no global var Chapter.Current.Name");
    CChapterNameToButtonIndex::iterator findIt = chapterNameToButtonIndexMap.find(szChapterName.c_str());
    NI_ASSERT_T(findIt != chapterNameToButtonIndexMap.end(), NStr::Format( "Can not find chapter button ID by name %s", szChapterName.c_str() ));
    IUIElement *pChapterButton = pMap->GetChildByID(findIt->second);
    pChapterButton->SetState(1);

    // loading information about the chapter
    const SChapterStats *pStats = NGDB::GetGameStats<SChapterStats>(szChapterName.c_str(), IObjectsDB::CHAPTER);
    SetDescriptionText(pStats);
  }

  pUIScreen->Reposition(pGFX->GetScreenRect());
  // add UI screen to scene
  pScene->AddUIScreen(pUIScreen);

  // GetSingleton<IScenarioTracker>()->StartCampaign( szCampaignName, CAMPAIGN_TYPE_SINGLE );
}

void CInterfaceCampaign::SetDescriptionText(const SChapterStats *pStats)
{
  // set the description text
  auto pDesc = checked_cast<IUIElement *>(pUIScreen->GetChildByID(2000));
  ITextManager *pTM = GetSingleton<ITextManager>();
  NI_ASSERT_T(pDesc != 0, "Invalid mission text description control");
  CPtr<IText> pText = pTM->GetDialog(pStats->szDescriptionText.c_str());
  NI_ASSERT_T(pText != 0, NStr::Format("Can't find description text \"%s\"", pStats->szDescriptionText.c_str()));
  if (pText) pDesc->SetWindowText(0, pText->GetString());
  auto pChapterHeader = checked_cast<IUIElement *>(pUIScreen->GetChildByID(20001));
  CPtr<IText> pHeaderText = pTM->GetDialog(pStats->szHeaderText.c_str());
  if (pHeaderText) pChapterHeader->SetWindowText(0, pHeaderText->GetString());
}

void CInterfaceCampaign::OnGetFocus(bool bFocus)
{
  CInterfaceInterMission::OnGetFocus(bFocus);

  if (bFocus)
  {
    if (GetGlobalVar("ExitCampaign.Confirm", 0))
    {
      RemoveGlobalVar("ExitCampaign.Confirm");
      OnCancel();
    }
    if (GetGlobalVar("NextChapter.Confirmed", 0))
    {
      FinishInterface(MISSION_COMMAND_CAMPAIGN, nullptr);
      RemoveGlobalVar("NextChapter.Confirmed");
    }
  }
}

bool CInterfaceCampaign::ProcessMessage(const SGameMessage &msg)
{
  if (CInterfaceInterMission::ProcessMessage(msg)) return true;

  if (msg.nEventID >= 1000 && msg.nEventID - 1000 < chapterNameToButtonIndexMap.size())
  {
    for (CChapterNameToButtonIndex::iterator it = chapterNameToButtonIndexMap.begin(); it != chapterNameToButtonIndexMap.end(); ++it)
    {
      if (it->second == msg.nEventID)
      {
        // loading information about the chapter
        const SChapterStats *pStats = NGDB::GetGameStats<SChapterStats>(it->first.c_str(), IObjectsDB::CHAPTER);
        SetDescriptionText(pStats);
        break;
      }
    }
  }

  switch (msg.nEventID)
  {
    case IMC_CANCEL:
    {
      GetSingleton<IMainLoop>()->Command(MISSION_COMMAND_MESSAGE_BOX,
                                         NStr::Format("%s;%s;1;ExitCampaign.Confirm", "Textes\\UI\\MessageBox\\exit_campaign_caption",
                                                      "Textes\\UI\\MessageBox\\exit_campaign_message"));

      return true;
    }

    case IMC_SAVE:
      GetSingleton<IMainLoop>()->Command(MISSION_COMMAND_IM_SAVE_MISSION, nullptr);

      return true;
    case IMC_OK:
    {
      int nCheatEnabledChapters = GetGlobalVar("Cheat.Enable.Chapters", -1);
      if (nCheatEnabledChapters != -1)
      {
        // read the chapter number
        auto pChapters = checked_cast<IUIContainer *>(pUIScreen->GetChildByID(100));
        NI_ASSERT_T(pChapters != 0, "Can't find element 100 - chapters!");
        // find the highlighted button
        int nButton = 1000;
        IUIElement *pButton = nullptr;
        do
        {
          pButton = pChapters->GetChildByID(nButton);
          if (pButton->GetState() == 1) break;// dedicated
          nButton++;
        } while (pButton != nullptr);
        NI_ASSERT_T(pButton != 0, "Can't find selected chapter");

        CChapterNameToButtonIndex::iterator it = chapterNameToButtonIndexMap.begin();
        for (; it != chapterNameToButtonIndexMap.end(); ++it)
        {
          if (it->second == nButton)
          {
            SetGlobalVar("Chapter.Current.Name", it->first.c_str());
            break;
          }
        }
        NI_ASSERT_T(it != chapterNameToButtonIndexMap.end(), "Error: can not find name of chapter. (Cheats are enabled).");
      }

      FinishInterface(MISSION_COMMAND_CHAPTER, nullptr);
      return true;
    }

    case IMC_TOTAL_ENCYCLOPEDIA:
      FinishInterface(MISSION_COMMAND_TOTAL_ENCYCLOPEDIA, nullptr);
      return true;

    case IMC_NEXT_CHAPTER:

      // GetSingleton<IInput>()->AddMessage( SGameMessage( IMC_SHOW_NEXT_CHAPTER_DIALOG ) );
      GetSingleton<IMainLoop>()->Command(MISSION_COMMAND_NEXT_CHAPTER, "1");

      return true;

    case IMC_NEXT_CHAPTER_CONFIRM:
    {
      const SCampaignStats *pStats = ReadCampaignStats();
      auto pMap = checked_cast<IUIContainer *>(pUIScreen->GetChildByID(100));

      // reset the selected state of the current chapter
      {
        std::string szChapterName = GetGlobalVar("Chapter.Current.Name", "");
        NI_ASSERT_T(szChapterName.size() > 0, "There is no global var Chapter.Current.Name");

        // set global var to indicate that this chapter has already been passed
        std::string szVarName = "Chapter." + szChapterName + ".Status";
        SetGlobalVar(szVarName.c_str(), 2);

        CChapterNameToButtonIndex::iterator findIt = chapterNameToButtonIndexMap.find(szChapterName.c_str());
        NI_ASSERT_T(findIt != chapterNameToButtonIndexMap.end(), NStr::Format( "Can not find chapter button ID by name %s", szChapterName.c_str() ));
        IUIElement *pChapterButton = pMap->GetChildByID(findIt->second);
        pChapterButton->SetState(0);
        pChapterButton->EnableWindow(false);
      }

      // move on to the next chapter
      std::string szVarName = "Chapter.New.Available";
      std::string szNewChapter = GetGlobalVar(szVarName.c_str(), "");
      NI_ASSERT_T(szNewChapter.size() != 0, "Error in using Next chapter button");
      int nCheatEnabledChapters = GetGlobalVar("Cheat.Enable.Chapters", -1);
      NI_ASSERT_T(szNewChapter.size() != -1, "Error in using Next chapter button");

      RemoveGlobalVar(szVarName.c_str());
      SetGlobalVar("Chapter.Current.Name", szNewChapter.c_str());

      // set the button with the new chapter as active
      CChapterNameToButtonIndex::iterator findIt = chapterNameToButtonIndexMap.find(szNewChapter.c_str());
      NI_ASSERT_T(findIt != chapterNameToButtonIndexMap.end(), NStr::Format( "Can not find chapter button ID by name %s", szNewChapter.c_str() ));
      IUIElement *pChapterButton = pMap->GetChildByID(findIt->second);
      pChapterButton->ShowWindow(UI_SW_SHOW);
      pChapterButton->SetState(1);

      // disable jump button
      IUIElement *pButton = pUIScreen->GetChildByID(10003);
      NI_ASSERT_T(pButton != 0, "There is no Next Chapter button");
      pButton->EnableWindow(false);

      break;
    }
  }

  //
  return false;
}

void CInterfaceCampaign::OnCancel()
{
  CInterfaceMainMenu::PlayIntermissionSound();
  // Let's check if we're in a custom campaign
  int nCustomCampaign = GetGlobalVar("Custom.Campaign", 0);
  if (nCustomCampaign)
  {
    RemoveGlobalVar("Custom.Campaign");
    FinishInterface(MISSION_COMMAND_MAIN_MENU, "5");
    GetSingleton<IMainLoop>()->Command(MISSION_COMMAND_CUSTOM_CAMPAIGN, nullptr);
  }
  else FinishInterface(MISSION_COMMAND_MAIN_MENU, "2");
}