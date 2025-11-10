#ifndef __GAME_CREATION_H__
#define __GAME_CREATION_H__

#pragma once

#include "GameCreationInterfaces.h"
#include "MessagesStore.h"
#include "ServerInfo.h"

#include "../StreamIO/StreamIOHelper.h"
#include "../RandomMapGen/MapInfo_Types.h"

#include "../Net/NetDriver.h"

interface IMultiplayerMessage;

enum EGameType
{
  EGT_LAN,
  EGT_GAME_SPY,
  EGT_ADDRESS_BOOK,
};

// general information processing
class CCommonGameCreationInfo
{
protected:
  SGameInfo gameInfo;
  CPlayers players;

  CMessagesStore messages;
  SQuickLoadMapInfo mapInfo;

  struct SSideInfo
  {
    int nMaxPlayers;
    std::string szName;

    SSideInfo() : nMaxPlayers(-1), szName("") {}
    SSideInfo(const char *pszName, const int _nMaxPlayers) : nMaxPlayers(_nMaxPlayers), szName(pszName) {}
  };

  using CSides = std::vector<SSideInfo>;
  CSides sides;

  CPtr<INetDriver> pInGameNetDriver;
  NTimer::STime lastPingMessageTime;
  NTimer::STime startSendMessagesTime;

  struct SPackedInfo
  {
  private:
    void PackFile(std::string szFileName, std::vector<BYTE> &packedFile, int &nRealSize);

  public:
    bool bPacked;

    std::vector<BYTE> packedMap;
    int nRealMapSize;
    std::string szMapFileName;

    std::vector<BYTE> packedScript;
    int nRealScriptSize;
    std::string szScriptFileName;

    std::vector<BYTE> txtFile;
    std::string szTXTFileName;

    SPackedInfo() : bPacked(false) {}
    void LoadAllFiles();
  };

  SPackedInfo packedInfo;

  //
  void DistributePlayersNumbers();
  void UpdatePlayersInfo();

  void SendPingMessage();

public:
  void Init();
  const bool GetPlayerInfo(const WORD *pszPlayerName, SPlayerInfo *pInfo) const;
  const bool GetOurPlayerInfo(SPlayerInfo *pInfo, int nOurLogicID) const;

  bool CanStartGame() const;
  bool IsAllPlayersInOneParty() const;

  virtual IMultiplayerMessage * STDCALL GetMessage() { return messages.GetMessage(); }

  void LoadSidesInformation();
  bool LoadMapInfo(bool bServer, bool bNeedCheckSums);
  void SetGlobalVars(int nOurLogicID);

  INetDriver *GetInGameNetDriver() const { return pInGameNetDriver; }
};

class CServerGameCreation : public IGameCreation, protected CCommonGameCreationInfo
{
  OBJECT_COMPLETE_METHODS(CServerGameCreation);

  CPtr<INetDriver> pOutGameNetDriver;
  bool bCanStartGame;

  // send game information outside
  void SendGameInfoOutside();
  void SendConnectionFailed();
  bool CheckConnection();
  int CreateNewLogicID();
  void ReadPlayerInfo(int nClientID, CStreamAccessor &pkt);
  //
  void ProcessMessages();
  void ProcessRemoveClient(int nClientID, CStreamAccessor &pkt);
  void ProcessNewClient(int nClientID, CStreamAccessor &pkt);
  void ProcessBroadcastMessage(int nClientID, CStreamAccessor &pkt);
  void ProcessDirectMessage(int nClientID, CStreamAccessor &pkt);
  void ChoosePlayerName(int nClientID, CStreamAccessor &pkt);

  void ConstructGameInfoPacket(CStreamAccessor &pkt);

  void UpdateLoadMap();
  void SendPacketStream(int nClientID, const std::vector<BYTE> &stream);

public:
  CServerGameCreation() : bCanStartGame(false) {}

  void Init(INetDriver *pInGameNetDriver, INetDriver *pOutGameNetDriver,
            const SGameInfo &gameInfo, const SQuickLoadMapInfo &mapInfo);

  void STDCALL LeftGame() override;
  void STDCALL KickPlayer(int nLogicID) override;
  void STDCALL ChangeGameSettings() override {}
  void STDCALL ChangePlayerSettings(const struct SPlayerInfo &info, const EPlayerSettings &eSettingsType) override;
  void STDCALL Launch() override {}

  void STDCALL Segment() override;

  interface IGamePlaying * STDCALL CreateGamePlaying() override;

  bool STDCALL CanStartGame() const override { return CCommonGameCreationInfo::CanStartGame(); }
  bool STDCALL IsAllPlayersInOneParty() const override { return CCommonGameCreationInfo::IsAllPlayersInOneParty(); }
  IMultiplayerMessage * STDCALL GetMessage() override { return CCommonGameCreationInfo::GetMessage(); }

  const bool STDCALL GetPlayerInfo(const WORD *pszPlayerName, SPlayerInfo *pInfo) const override { return CCommonGameCreationInfo::GetPlayerInfo(pszPlayerName, pInfo); }

  const bool STDCALL GetOurPlayerInfo(SPlayerInfo *pInfo) const override { return CCommonGameCreationInfo::GetOurPlayerInfo(pInfo, 0); }

  void STDCALL SetNewGameSettings(const SMultiplayerGameSettings &settings) override;

  interface INetDriver * STDCALL GetInGameNetDriver() const override { return CCommonGameCreationInfo::GetInGameNetDriver(); }
};

class CClientGameCreation : public IGameCreation, protected CCommonGameCreationInfo
{
  OBJECT_COMPLETE_METHODS(CClientGameCreation);

  int nOurLogicID;
  bool bGameStarted;
  bool bModChanged;

  //
  bool CheckConnection();
  void SendConnectionFailed();

  //
  void ProcessMessages();
  void ProcessDirectMessage(int nClientID, CStreamAccessor &pkt);
  void ProcessBroadcastMessage(int nClientID, CStreamAccessor &pkt);
  void ProcessPlayerLeft(int nClientID, CStreamAccessor &pkt);
  // broadcast message with player info receive, reply with my info should be sent
  void ProcessNewPlayerJoinedInfo(int nClientID, CStreamAccessor &pkt);
  // direct message with player info received
  void ProcessNewPlayerInfo(int nClientID, CStreamAccessor &pkt);
  void ProcessLogicIDSet(int nClientID, CStreamAccessor &pkt);
  void ProcessGameInfo(int nClientID, CStreamAccessor &pkt);
  void ProcessKickedPlayer(int nClientID, CStreamAccessor &pkt);
  void ProcessRemoveClient(int nClientID, CStreamAccessor &pkt);
  void ProcessGameStarted(CStreamAccessor &pkt);
  void ProcessReceivedOwnName(int nClientID, CStreamAccessor &pkt);
  void ProcessGameIsAlreadyStarted();
  void ProcessGameSettingsChanged(CStreamAccessor &pkt);
  void ContinueLoadMapInfo();

  class CLoadMap
  {
    CPtr<INetDriver> pNetDriver;
    int nServer;

    enum ELoadState { ELS_NONE, ELS_WAIT_FOR_SERVER_ID, ELS_LOADING };

    ELoadState eState;

    int nCompressedSize, nRealSize;
    int nReceived;

    CClientGameCreation *pClientGameCreation;
    std::vector<BYTE> stream;

    std::string szFileName;

    int nTotalSize, nTotalReceived;

    //
    void ProcessWaitForServerID();
    void ProcessLoading();
    void ProcessMapPacket(CStreamAccessor &pkt);
    void ProcessMapLoadFinished();
    void ProcessReceivePackedFileInfo(CStreamAccessor &pkt);
    void ProcessReceiveFileInfo(CStreamAccessor &pkt);
    void AllLoadFinished();

  public:
    CLoadMap() : nServer(-1), eState(ELS_NONE), pClientGameCreation(nullptr) {}
    void Init(INetDriver *pNetDriver, CClientGameCreation *pClientGameCreation);
    void SetServer(const int _nServer) { nServer = _nServer; }

    void Segment();
  };

  CLoadMap loadMap;

public:
  CClientGameCreation() : bModChanged(false) {}

  void Init(INetDriver *pInGameNetDriver, bool bPasswordRequired, const std::string &szPassword);

  void STDCALL LeftGame() override {}
  void STDCALL KickPlayer(int nLogicID) override;
  void STDCALL ChangeGameSettings() override {}
  void STDCALL ChangePlayerSettings(const struct SPlayerInfo &info, const EPlayerSettings &eSettingsType) override;
  void STDCALL Launch() override {}

  void STDCALL Segment() override;

  IMultiplayerMessage * STDCALL GetMessage() override { return CCommonGameCreationInfo::GetMessage(); }

  bool STDCALL CanStartGame() const override { return CCommonGameCreationInfo::CanStartGame(); }
  bool STDCALL IsAllPlayersInOneParty() const override { return CCommonGameCreationInfo::IsAllPlayersInOneParty(); }
  interface IGamePlaying * STDCALL CreateGamePlaying() override;

  const bool STDCALL GetPlayerInfo(const WORD *pszPlayerName, SPlayerInfo *pInfo) const override { return CCommonGameCreationInfo::GetPlayerInfo(pszPlayerName, pInfo); }

  const bool STDCALL GetOurPlayerInfo(SPlayerInfo *pInfo) const override { return CCommonGameCreationInfo::GetOurPlayerInfo(pInfo, nOurLogicID); }

  void STDCALL SetNewGameSettings(const SMultiplayerGameSettings &settings) override {}

  void STDCALL ModChanged() override { bModChanged = true; }

  interface INetDriver * STDCALL GetInGameNetDriver() const override { return CCommonGameCreationInfo::GetInGameNetDriver(); }

  void MapLoaded();
};

#endif // __GAME_CREATION_H__