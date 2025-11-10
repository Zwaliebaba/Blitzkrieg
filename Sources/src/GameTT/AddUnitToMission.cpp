#include "StdAfx.h"

#include "AddUnitToMission.h"

#include "../Main/ScenarioTracker.h"
#include "../Main/ScenarioTrackerTypes.h"
#include "../Common/ObjectStatus.h"
#include "../Main/gamestats.h"
#include "../RandomMapGen/MapInfo_Types.h"
#include "../UI/UIMessages.h"
#include "CommonId.h"
#include "UnitTypes.h"
#include "etypes.h"

SUnitClass unitClasses[] =
{
    RPG_CLASS_UNKNOWN, "_unknown",
    RPG_CLASS_ARTILLERY, "_artillery",
    RPG_CLASS_TANK, "_tank",
    RPG_CLASS_SNIPER, "_sniper",
    /* RPG_CLASS_HEAVY_ARTILLERY, "_hartillery",
     */
};
int nUnitClassesSize = ARRAY_SIZE(unitClasses);

SUnitClass unitTypes[] =
{
    RPG_TYPE_INFANTRY, "infantry_type",
    RPG_TYPE_TRANSPORT, "transport_type",
    RPG_TYPE_ARTILLERY, "artillery_type",
    RPG_TYPE_SPG, "spg_type",
    RPG_TYPE_ARMOR, "armor_type",
    RPG_TYPE_AVIATION, "aviation_type",
    RPG_TYPE_TRAIN, "train_type",
};
int nUnitTypesSize = ARRAY_SIZE(unitTypes);

CTRect<float> rcTechnicsInfoPanelMap(0, 0, 90.5f / 128.0f, 90.5f / 128.0f);

std::vector<std::vector<int>> CInterfaceAddUnitToMission::m_missionSlots;

const char *GetUnitClassName(int nUnitClass)
{
  for (int i = 0; i < nUnitClassesSize; i++) { if (unitClasses[i].nClass == nUnitClass) return unitClasses[i].pszName; }
  NI_ASSERT(0);
  return nullptr;
}

void FillUnitInfoItem(const SUnitBaseRPGStats *pRPG, IUIDialog *pItem, int nIndex, bool bFillCommanderName, const char *pszCommanderName)
{
  IUIElement *pHelpButton = pItem->GetChildByID(10000);
  if (pHelpButton) pHelpButton->SetWindowID(20000 + nIndex);// for encyclopedia
  pItem->SetWindowID(nIndex);
  FillUnitInfoItemNoIDs(pRPG, pItem, nIndex, bFillCommanderName, pszCommanderName);
}

void FillUnitInfoItemNoIDs(const SUnitBaseRPGStats *pRPG, IUIDialog *pItem, int nIndex, bool bFillCommanderName, const char *pszCommanderName)
{
  // nIndex is a Scenario Tracker index for unit, else -1
  ITextManager *pTextM = GetSingleton<ITextManager>();
  ITextureManager *pTM = GetSingleton<ITextureManager>();
  IScenarioTracker *pST = GetSingleton<IScenarioTracker>();

  const SGDBObjectDesc *pObjectDesc = GetSingleton<IObjectsDB>()->GetDesc(pRPG->szParentName.c_str());
  CPtr<IText> p1 = pTextM->GetDialog((pObjectDesc->szPath + "\\name").c_str());

  // set the unit name
  IUIElement *pElement = pItem->GetChildByID(20);
  pElement->SetWindowText(0, p1->GetString());

  IPlayerScenarioInfo *pPlayerInfo = pST->GetUserPlayer();

  IText *pCommanderName = nullptr;

  // commander's name
  if (bFillCommanderName)
  {
    IScenarioUnit *pUnit = pPlayerInfo->GetUnit(nIndex);
    pCommanderName = pUnit->GetName();
  }
  else if (pszCommanderName) pCommanderName = pTextM->GetString(pszCommanderName);

  if (pCommanderName)
  {
    pElement = pItem->GetChildByID(21);
    pElement->SetWindowText(0, pCommanderName->GetString());
  }

  if (bFillCommanderName)
  {
    IScenarioUnit *pUnit = pPlayerInfo->GetUnit(nIndex);
    // find out the unit's level
    const int nLevel = pUnit->GetValue(STUT_LEVEL);
    NI_ASSERT_T(nLevel >= 0 && nLevel < 4, "Unit's level is out of bounds");
    IUIElement *pElement = pItem->GetChildByID(30 + nLevel);
    if (pElement)
    {
      // find out the current and next level of experience of the unit
      const int nExp = pUnit->GetValue(STUT_EXP);
      const int nExpNextLevel = pUnit->GetValue(STUT_EXP_NEXT_LEVEL);
      //
      IText *pText = GetSingleton<ITextManager>()->GetDialog(NStr::Format("textes\\ui\\mission\\status\\tt_unit_level%d", nLevel));
      std::wstring wToolTip = pText != nullptr ? pText->GetString() : L"";
      wToolTip += NStr::ToUnicode(NStr::Format("(%d / %d)", nExp, nExpNextLevel));

      pElement->SetHelpContext(0, wToolTip.c_str());
      pElement->ShowWindow(UI_SW_SHOW);
    }
  }

  // display unit stats
  {
    static std::wstring szText;
    IUIElement *pElement = nullptr;

    SMissionStatusObject status;
    GetStatusFromRPGStats(&status, static_cast<const SMechUnitRPGStats *>(pRPG), false, false);

    for (int i = 0; i < 4; i++)
    {
      const int nID = 101 + i;
      NStr::ToUnicode(&szText, NStr::Format("%d", status.armors[i]));
      pElement = pItem->GetChildByID(nID);
      NI_ASSERT_T(pElement != 0, NStr::Format( "Can not find window id %d (armor string)", nID ));
      pElement->SetWindowText(0, szText.c_str());
    }

    for (int i = 0; i < 2; i++)
    {
      const int nID = 111 + i;
      NStr::ToUnicode(&szText, NStr::Format("%d", status.weaponstats[i]));
      pElement = pItem->GetChildByID(nID);
      NI_ASSERT_T(pElement != 0, NStr::Format( "Can not find window id %d (weapon string)", nID ));
      pElement->SetWindowText(0, szText.c_str());
    }
  }

  //
  // set the unit picture
  IUIElement *pPicture = pItem->GetChildByID(11);
  // install a map for the image
  pPicture->SetWindowMap(rcTechnicsInfoPanelMap);
  // download and install the texture
  IGFXTexture *pTexture = pTM->GetTexture((pObjectDesc->szPath + "\\icon").c_str());
  pPicture->SetWindowTexture(pTexture);

}

enum ECommands
{
  IMC_DEFAULT_UNITS = 10003,
  IMC_UNIT_INFO = 10006,
};

CInterfaceAddUnitToMission::CInterfaceAddUnitToMission() : CInterfaceInterMission("InterMission") {}

CInterfaceAddUnitToMission::~CInterfaceAddUnitToMission() {}

bool CInterfaceAddUnitToMission::Init()
{
  CInterfaceInterMission::Init();
  // SetBindSection( "intermission" );

  return true;
}

void CInterfaceAddUnitToMission::StartInterface()
{
  CInterfaceInterMission::StartInterface();
  pUIScreen = CreateObject<IUIScreen>(UI_SCREEN);
  pUIScreen->Load("ui\\AddUnitToMission");
  pUIScreen->Reposition(pGFX->GetScreenRect());

  DisplaySlotsFromST();

  pUIScreen->Reposition(pGFX->GetScreenRect());
  StoreScreen();
  pScene->AddUIScreen(pUIScreen);
}

bool CInterfaceAddUnitToMission::AddDefaultSlotsToST()
{
  NI_ASSERT_T(FALSE, "OLD INTERFACE");
  // we read information about the number of free seats from the card data
  /* std::string szMissionName = GetGlobalVar( "Mission.Current.Name", "" );
   */
  return true;
}

void CInterfaceAddUnitToMission::DisplaySlotsFromST()
{
  NI_ASSERT_T(FALSE, "OLD INTERFACE");
  /* ITextureManager *pTM = GetSingleton<ITextureManager>();
   */
}

void CInterfaceAddUnitToMission::UpdateUnitsList()
{
  NI_ASSERT_T(FALSE, "OLD INTERFACE");
  /* IUIShortcutBar *pSB = checked_cast<IUIShortcutBar *> ( pUIScreen->GetChildByID( 100 ) );
   */
}

void CInterfaceAddUnitToMission::EnableItem(IUIContainer *pItem, bool bEnable)
{
  pItem->EnableWindow(bEnable);
  IUIElement *pE = pItem->GetChildByID(20);
  pE->EnableWindow(bEnable);
  pE = pItem->GetChildByID(21);
  pE->EnableWindow(bEnable);
}

void CInterfaceAddUnitToMission::SelectItem()
{
  NI_ASSERT_T(FALSE, "OLD INTERFACE");
  /* //choose units ShortcutBar
   */
}

bool CInterfaceAddUnitToMission::ProcessMessage(const SGameMessage &msg)
{
  NI_ASSERT_T(FALSE, "OLD INTERFACE");
  /* if ( CInterfaceInterMission::ProcessMessage( msg ) )
     */

  //
  return false;
}