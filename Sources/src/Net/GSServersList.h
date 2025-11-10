#ifndef __GS_SERVERS_LIST_H__
#define __GS_SERVERS_LIST_H__

#pragma once

#include "NetDriver.h"
#include "../GameSpy/cengine/goaceng.h"

#include "../Misc/Thread.h"
#include "../Misc/Win32Helper.h"

class CGSServersListDriver : public INetDriver, public CThread
{
  OBJECT_NORMAL_METHODS(CGSServersListDriver);

  // thread
  NWin32Helper::CCriticalSection criticalSection;

  GServerList serverList;
  static const int SERVER_LIST_UPDATE_PERIOD;
  NTimer::STime endOfLastUpdate;
  bool bUpdating;

  struct SServerInfo
  {
    SGameInfo gameInfo;
    CPtr<INetNodeAddress> pAddr;
    float fPing;
    std::string szIP;
    int nNetVersion;

    SServerInfo() : fPing(0.0f) {}

    bool operator==(const SServerInfo &serverInfo)
    {
      return
          gameInfo == serverInfo.gameInfo && pAddr->IsSameIP(serverInfo.pAddr) && fPing == serverInfo.fPing;
    }
  };

  std::list<SServerInfo> servers;
  int nNetVersion;

  //
  static void ListCallBack(GServerList serverList, int nMsg, void *pInstance, void *pParam1, void *pParam2);
  void List(GServerList ServerList, int nMsg, void *pParam1, void *pParam2);

  void AddServer(GServer server);

protected:
  void Step() override;

public:
  CGSServersListDriver();
  virtual ~CGSServersListDriver();
  //
  bool STDCALL Init(APPLICATION_ID _nApplicationID, int _nGamePort, bool _bClientOnly) override;
  // get current state (active/inactive/connecting)
  EState STDCALL GetState() const override;
  // get reject reason (then)
  EReject STDCALL GetRejectReason() const override;
  // get game info (for client)
  bool STDCALL GetGameInfo(int nIdx, INetNodeAddress *pAddr, bool *pWrongVersion, float *pPing, SGameInfo *pGameInfo) override;
  // refresh servers list ( for client )
  void STDCALL RefreshServersList() override;

  // unnecessary functions
  // connect to the game with particular address
  void STDCALL ConnectGame(const INetNodeAddress *pAddr, IDataStream *pPwd) override { NI_ASSERT_T(false, "wrong call"); }
  // start game (server)
  void STDCALL StartGame() override { NI_ASSERT_T(false, "wrong call"); }
  // sending start game info (for server)
  void STDCALL StartGameInfoSend(const SGameInfo &gameInfo) override { NI_ASSERT_T(false, "wrong call"); }
  // stop sending game info (for server)
  void STDCALL StopGameInfoSend() override { NI_ASSERT_T(false, "wrong call"); }
  // start accepting new players (for server)
  void STDCALL StartNewPlayerAccept() override { NI_ASSERT_T(false, "wrong call"); }
  // stop accepting new players (for server)
  void STDCALL StopNewPlayerAccept() override { NI_ASSERT_T(false, "wrong call"); }
  // send broadcast message for all
  bool STDCALL SendBroadcast(IDataStream *pPkt) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  // send direct message for client 'nClient'
  bool STDCALL SendDirect(int nClient, IDataStream *pPkt) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  // kick player 'nClient'
  void STDCALL Kick(int nClient) override { NI_ASSERT_T(false, "wrong call"); }
  // get next message. 
  bool STDCALL GetMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  // ping of the client, -1 if client doesn't exist
  const float STDCALL GetPing(const int nClientID) override
  {
    NI_ASSERT_T(false, "wrong call");
    return 0.0f;
  }

  // time since last message was received from this client
  const float STDCALL GetTimeSinceLastRecv(const int nClientID) override
  {
    NI_ASSERT_T(false, "wrong call");
    return 0.0f;
  }

  // 
  // ???
  SOCKET STDCALL GetSocket() override
  {
    NI_ASSERT_T(false, "wrong call");
    return 0;
  }

  sockaddr * STDCALL GetSockAddr() override
  {
    NI_ASSERT_T(false, "wrong call");
    return nullptr;
  }

  // auxiliary multichannel functions
  void STDCALL AddChannel(const int nChannelID, const std::unordered_set<BYTE> &channelMessages) override { NI_ASSERT_T(false, "wrong call"); }
  void STDCALL RemoveChannel(const int nChannelID) override { NI_ASSERT_T(false, "wrong call"); }

  bool STDCALL GetChannelMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt, const int nChannel) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }
};

#endif // __GS_SERVERS_LIST_H__