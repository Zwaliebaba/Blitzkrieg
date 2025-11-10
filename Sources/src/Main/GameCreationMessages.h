#ifndef __GAME_CREATION_MESSAGES_H__
#define __GAME_CREATION_MESSAGES_H__

#pragma once

#include "Messages.h"
#include "ServerInfo.h"

#include "../Net/NetDriver.h"

class CConnectionFailed : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CConnectionFailed);

  INetDriver::EReject eReason;

public:
  CConnectionFailed() {}
  CConnectionFailed(const INetDriver::EReject &_eReason) : eReason(_eReason) {}

  const EMultiplayerMessages GetMessageID() const override { return CONNECTION_FAILED; }
  void SendToUI() override;
};

class CPlayerInfoRefreshed : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CPlayerInfoRefreshed);

  SPlayerInfo info;
  std::string szSide;
  int nLoadMapProgress;

public:
  CPlayerInfoRefreshed() {}

  CPlayerInfoRefreshed(const SPlayerInfo &_info, const char *pszSide)
    : info(_info), szSide(pszSide) {}

  const EMultiplayerMessages GetMessageID() const override { return PLAYER_INFO_REFRESHED; }
  void SendToUI() override;
};

class CPlayerDeleted : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CPlayerDeleted);

public:
  enum EReasons { ER_LEFT, ER_KICKED };

private:
  enum EReasons eReason;
  int nLogicID;
  std::wstring wszPlayerName;

public:
  CPlayerDeleted() {}

  CPlayerDeleted(const int _nLogicID, const std::wstring _wszPlayerName, const EReasons _eReason)
    : eReason(_eReason), nLogicID(_nLogicID), wszPlayerName(_wszPlayerName) {}

  const EMultiplayerMessages GetMessageID() const override { return PLAYER_DELETED; }
  void SendToUI() override;
};

class CGameInfoReceived : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CGameInfoReceived);

  SGameInfo gameInfo;
  bool bServer;
  int nOurID;

public:
  CGameInfoReceived() {}

  CGameInfoReceived(const SGameInfo &_gameInfo, bool _bServer, const int _nOurID)
    : gameInfo(_gameInfo), bServer(_bServer), nOurID(_nOurID) {}

  const EMultiplayerMessages GetMessageID() const override { return GAME_INFO_RECEIVED; }
  void SendToUI() override;
};

class CGameStarted : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CGameStarted);

public:
  CGameStarted() {}

  const EMultiplayerMessages GetMessageID() const override { return GAME_STARTED; }
  void SendToUI() override;
};

class CWrongResources : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CWrongResources);

public:
  CWrongResources() {}

  const EMultiplayerMessages GetMessageID() const override { return WRONG_RESOURCES; }
  void SendToUI() override;
};

class CWrongMap : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CWrongMap);

public:
  CWrongMap() {}

  const EMultiplayerMessages GetMessageID() const override { return WRONG_MAP; }
  void SendToUI() override;
};

class CNoMap : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CNoMap);

public:
  CNoMap() {}

  const EMultiplayerMessages GetMessageID() const override { return NO_MAP; }
  void SendToUI() override;
};

class CWrongPassword : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CWrongPassword);

public:
  CWrongPassword() {}

  const EMultiplayerMessages GetMessageID() const override { return WRONG_PASSWORD; }
  void SendToUI() override;
};

class CGameIsAlreadyStarted : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CGameIsAlreadyStarted);

public:
  CGameIsAlreadyStarted() {}

  const EMultiplayerMessages GetMessageID() const override { return GAME_IS_ALREADY_STARTED; }
  void SendToUI() override;
};

class CCanStartGameState : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CCanStartGameState);

  bool bCanStartGame;

  CCanStartGameState() {}

public:
  explicit CCanStartGameState(const bool _bCanStartGame) : bCanStartGame(_bCanStartGame) {}

  const EMultiplayerMessages GetMessageID() const override { return CAN_START_GAME; }
  void SendToUI() override;
};

class CGameSettingsChanged : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CGameSettingsChanged);

  SMultiplayerGameSettings settings;

public:
  CGameSettingsChanged() {}
  explicit CGameSettingsChanged(const SMultiplayerGameSettings &_settings) : settings(_settings) {}

  const EMultiplayerMessages GetMessageID() const override { return GAME_INFO_CHANGED; }
  void SendToUI() override;
};

class CCreateStagingRoom : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CCreateStagingRoom);

public:
  CCreateStagingRoom() {}

  const EMultiplayerMessages GetMessageID() const override { return CREATE_STAGING_ROOM; }
  void SendToUI() override;
};

class CAIMKicked : public IMultiplayerMessage
{
  OBJECT_COMPLETE_METHODS(CAIMKicked);

public:
  CAIMKicked() {}

  const EMultiplayerMessages GetMessageID() const override { return AIM_KICKED; }
  void SendToUI() override;
};

#endif // __GAME_CREATION_MESSAGES_H__