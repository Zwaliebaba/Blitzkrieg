#include "stdafx.h"

#include "GameCreationMessages.h"
#include "../GameTT/MultiplayerCommandManager.h"
#include "../GameTT/MuliplayerToUIConsts.h"

// **********************************************************************
// *CConnectionFailed*
// **********************************************************************

void CConnectionFailed::SendToUI()
{
  IMPToUICommandManager *pCommandManager = GetSingleton<IMPToUICommandManager>();
  pCommandManager->AddCommandToUI(
      SToUICommand(EMTUC_CONNECTION_FAILED, new SNotificationSimpleParam(eReason))
      );
  /* IConsoleBuffer *pBuffer = GetSingleton<IConsoleBuffer>();
     */
};

// **********************************************************************
// *CPlayerInfoRefreshed*
// **********************************************************************

void CPlayerInfoRefreshed::SendToUI()
{
  IMPToUICommandManager *pCommandManager = GetSingleton<IMPToUICommandManager>();

  pCommandManager->AddCommandToUI(
      SToUICommand(EMTUC_UPDATE_PLAYER_INFO,
                   new SUIPlayerInfo(info.nLogicID, szSide.c_str(), info.bReady, info.fPing, info.szName.c_str(), info.cMapLoadProgress)
          )
      );

  /* IConsoleBuffer *pBuffer = GetSingleton<IConsoleBuffer>();
     */
}

// **********************************************************************
// *CPlayerDeleted*
// **********************************************************************

void CPlayerDeleted::SendToUI()
{
  IMPToUICommandManager *pCommandManager = GetSingleton<IMPToUICommandManager>();
  CPtr<SUIPlayerInfo> pInfo = new SUIPlayerInfo(nLogicID, "", false, -1, wszPlayerName.c_str(), 100);

  if (eReason == ER_LEFT) pCommandManager->AddCommandToUI(SToUICommand(EMTUC_PLAYER_LEFT, pInfo));
  else pCommandManager->AddCommandToUI(SToUICommand(EMTUC_PLAYER_KICKED, pInfo));

  /* IConsoleBuffer *pBuffer = GetSingleton<IConsoleBuffer>();
     */
}

// **********************************************************************
// *CGameInfoReceived*
// **********************************************************************

void CGameInfoReceived::SendToUI()
{
  CPtr<SUIStagingRoomConfigure> pInfo = new SUIStagingRoomConfigure();
  pInfo->szGameName = gameInfo.szGameName;
  if (gameInfo.bMapLoaded) pInfo->szMapLocation = gameInfo.szMapName;
  else pInfo->szMapLocation = "";

  pInfo->nPlayersMax = gameInfo.nMaxPlayers;
  pInfo->bServer = bServer;
  pInfo->nLocalPlayerID = nOurID;
  pInfo->serverSettings = gameInfo.gameSettings;

  IMPToUICommandManager *pCommandManager = GetSingleton<IMPToUICommandManager>();
  pCommandManager->AddCommandToUI(SToUICommand(EMTUC_CONFIGURE_STAGING_ROOM, pInfo));

  /* IConsoleBuffer *pBuffer = GetSingleton<IConsoleBuffer>();
     */
}

// **********************************************************************
// *CGameStarted*
// **********************************************************************

void CGameStarted::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_START_GAME, nullptr)); }

// **********************************************************************
// *CWrongResources*
// **********************************************************************

void CWrongResources::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_WRONG_RESOURCES, nullptr)); }

// **********************************************************************
// *CWrongMap*
// **********************************************************************

void CWrongMap::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_WRONG_MAP, nullptr)); }

// **********************************************************************
// *CNoMap*
// **********************************************************************

void CNoMap::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_NO_MAP, nullptr)); }

// **********************************************************************
// *CWrongPassword*
// **********************************************************************

void CWrongPassword::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_WRONG_PASSWORD, nullptr)); }

// **********************************************************************
// *CGameIsAlreadyStarted*
// **********************************************************************

void CGameIsAlreadyStarted::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_GAME_IS_ALREADY_STARTED, nullptr)); }

// **********************************************************************
// *CCanStartGameState*
// **********************************************************************

void CCanStartGameState::SendToUI()
{
  CPtr<SNotificationSimpleParam> pInfo = new SNotificationSimpleParam(bCanStartGame);
  GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_ALLOW_START_GAME, pInfo));
}

// **********************************************************************
// *CGameSettingsChanged*
// **********************************************************************

void CGameSettingsChanged::SendToUI()
{
  CPtr<SServerNewSettings> pNewSettings = new SServerNewSettings(settings);
  GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_SERVER_SETTINGS_CHANGED, pNewSettings));
}

// **********************************************************************
// *CCreateStagingRoom*
// **********************************************************************

void CCreateStagingRoom::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_CREATE_STAGING_ROOM, nullptr)); }

// **********************************************************************
// *CAIMKicked*
// **********************************************************************

void CAIMKicked::SendToUI() { GetSingleton<IMPToUICommandManager>()->AddCommandToUI(SToUICommand(EMTUC_AIM_KICKED, nullptr)); }