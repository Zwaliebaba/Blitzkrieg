#ifndef __NET_DRIVER_H__
#define __NET_DRIVER_H__

#include <winsock2.h>

using APPLICATION_ID = unsigned int;

enum
{
  NET_BASE_VALUE = 0x100d0000,
  NET_NET = NET_BASE_VALUE + 1,
  NET_NET_DRIVER = NET_BASE_VALUE + 2,
  NET_NODE_ADDRESS = NET_BASE_VALUE + 3,

  NET_FORCE_DWORD = 0x7fffffff
};

interface INetNodeAddress : IRefCount
{
  virtual void Clear() = 0;
  //
  virtual bool SetInetName(const char *pszHost, int nDefaultPort) = 0;
  virtual const char * GetName(bool bResolve = true) const = 0;
  virtual const char * GetFastName() const = 0;
  //
  virtual bool IsSameIP(const INetNodeAddress *pAddress) const = 0;
  virtual unsigned int GetIP() const = 0;
  //
  virtual sockaddr * GetSockAddr() = 0;
};

interface INetDriver : IRefCount
{
  enum { tidTypeID = NET_NET_DRIVER };

  enum EState
  {
    INACTIVE,
    ACTIVE,
    CONNECTING
  };

  enum EReject
  {
    NONE,
    TIMEOUT,
    BANNED,
    WRONG_VERSION,
    MAXPLAYERS_REACHED,
    PASSWORD_FAILED,
    ALREADY_CONNECTED,
    FORBIDDEN
  };

  enum EMessage
  {
    NEW_CLIENT,
    REMOVE_CLIENT,
    DIRECT,
    BROADCAST,
    SERVER_DEAD,
  };

  enum EServerGameMode
  {
    ESGM_WAIT,// waiting for players to join
    ESGM_SETTINGS,// players are determining game parameters, no joining allowed
    ESGM_CLOSEDPLAYING,// game is in progress, no joining allowed
    ESGM_OPENPLAYING,// game is in progress, players may still join
    ESGM_DEBRIEFING,// game is over, stats/info is being shown, no joining allowed
    ESGM_EXITING,// server is shutting down, remove from server list
  };

  struct SGameInfo
  {
    std::wstring wszServerName;
    long nHostPort;
    std::wstring wszMapName;
    std::string szGameType;
    int nCurPlayers, nMaxPlayers;
    EServerGameMode eGameMode;

    bool bPasswordRequired;

    std::string szModName;
    std::string szModVersion;

    CPtr<IDataStream> pGameSettings;

    SGameInfo()
      : wszServerName(L""), nHostPort(0), wszMapName(L""), szGameType(""),
        nCurPlayers(0), nMaxPlayers(0), eGameMode(ESGM_SETTINGS), bPasswordRequired(false),
        szModName(""), szModVersion("") {}

    SGameInfo(const SGameInfo &gameInfo) :
      wszServerName(gameInfo.wszServerName), nHostPort(gameInfo.nHostPort), wszMapName(gameInfo.wszMapName),
      szGameType(gameInfo.szGameType), nCurPlayers(gameInfo.nCurPlayers), nMaxPlayers(gameInfo.nMaxPlayers),
      eGameMode(gameInfo.eGameMode), bPasswordRequired(gameInfo.bPasswordRequired),
      szModName(gameInfo.szModName), szModVersion(gameInfo.szModVersion), pGameSettings(gameInfo.pGameSettings) {}

    bool operator==(const SGameInfo &gameInfo) const
    {
      return
          wszServerName == gameInfo.wszServerName &&
          nHostPort == gameInfo.nHostPort &&
          wszMapName == gameInfo.wszMapName &&
          szGameType == gameInfo.szGameType &&
          nCurPlayers == gameInfo.nCurPlayers &&
          nMaxPlayers == gameInfo.nMaxPlayers &&
          eGameMode == gameInfo.eGameMode &&
          bPasswordRequired == gameInfo.bPasswordRequired &&
          szModName == gameInfo.szModName &&
          szModVersion == gameInfo.szModVersion;
    }
  };

  //
  virtual bool Init(APPLICATION_ID _nApplicationID, int _nGamePort, bool _bClientOnly) = 0;
  // get current state (active/inactive/connecting)
  virtual EState GetState() const = 0;
  // get reject reason (then)
  virtual EReject GetRejectReason() const = 0;
  // connect to the game with particular address
  virtual void ConnectGame(const INetNodeAddress *pAddr, IDataStream *pPwd) = 0;
  // start game (server)
  virtual void StartGame() = 0;
  // sending start game info (for server)
  virtual void StartGameInfoSend(const SGameInfo &gameInfo) = 0;
  // stop sending game info (for server)
  virtual void StopGameInfoSend() = 0;
  // start accepting new players (for server)
  virtual void StartNewPlayerAccept() = 0;
  // stop accepting new players (for server)
  virtual void StopNewPlayerAccept() = 0;
  // get game info (for client)
  virtual bool GetGameInfo(int nIdx, INetNodeAddress *pAddr, bool *pWrongVersion, float *pPing, SGameInfo *pGameInfo) = 0;
  // refresh servers list ( for client )
  virtual void RefreshServersList() = 0;
  // send broadcast message for all
  virtual bool SendBroadcast(IDataStream *pPkt) = 0;
  // send direct message for client 'nClient'
  virtual bool SendDirect(int nClient, IDataStream *pPkt) = 0;
  // kick player 'nClient'
  virtual void Kick(int nClient) = 0;
  // get next message. 
  virtual bool GetMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt) = 0;
  // ping of the client, -1 if client doesn't exist
  virtual const float GetPing(int nClientID) = 0;
  // time since last message was received from this client
  virtual const float GetTimeSinceLastRecv(int nClientID) = 0;
  // 
  // CRAP functions to work with GameSpy
  //
  virtual SOCKET GetSocket() = 0;
  virtual sockaddr * GetSockAddr() = 0;

  // auxiliary multichannel functions
  virtual void AddChannel(int nChannelID, const std::unordered_set<BYTE> &channelMessages) = 0;
  virtual void RemoveChannel(int nChannelID) = 0;

  virtual bool GetChannelMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt, int nChannel) = 0;

  // for debug of lagging net
  virtual void PauseNet() {}
  virtual void UnpauseNet() {}
  virtual void SetLag(const NTimer::STime period) {}

  // for debug
  virtual const char * GetAddressByClientID(const int nClientID) const { return "Unknown"; }
};

#endif // __NET_DRIVER_H__