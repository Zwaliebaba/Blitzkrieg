#ifndef __NETA4_H_
#define __NETA4_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Streams.h"
#include "NetDriver.h"
#include "NetLowest.h"
#include "NetAcks.h"
#include "NetStream.h"
#include "NetServerInfo.h"
#include "NetLogin.h"
#include "NetPeer2Peer.h"

#include "../Misc/HPTimer.h"
#include "../Misc/Win32Helper.h"

#if !defined(_FINALRELEASE) || defined(_DEVVERSION)
#define __TEST_LAGS__
#endif // !defined(_FINALRELEASE) || 

namespace NNet
{

  class CNetDriver : public INetDriver
  {
    OBJECT_NORMAL_METHODS(CNetDriver);

  public:
    struct SServer
    {
      INetNodeAddress *pAddr;
      bool bWrongVersion;
      float fPing;
      CMemoryStream info;
    };

    struct SMessage
    {
      EMessage msg;
      int nClientID;
      std::vector<int> received;
      CMemoryStream pkt;
    };

  private:
    //
    struct SClientAddressInfo
    {
      CNodeAddress inetAddress;
      CNodeAddressSet localAddress;

      SClientAddressInfo() {}

      SClientAddressInfo(const CNodeAddress &_inetAddress, const CNodeAddressSet &_localAddress)
        : inetAddress(_inetAddress), localAddress(_localAddress) {}
    };

    //
    struct SPeer
    {
      CP2PTracker::UCID clientID;
      CNodeAddress currentAddr;
      SClientAddressInfo addrInfo;
      CAckTracker acks;
      CStreamTracker data;
      bool bTryShortcut;
    };

    struct SGameInfoWrapper : SGameInfo
    {
      SGameInfoWrapper() {}
      SGameInfoWrapper(const SGameInfo &gameInfo) : SGameInfo(gameInfo) {}

      void Pack(IDataStream *pDataStream);
      void Unpack(IDataStream *pDataStream);
    };

    //
    using CPeerList = std::list<SPeer>;
    CPeerList clients;
    EState state;
    EReject lastReject;
    NHPTimer::STime lastTime;
    CServerInfoSupport serverInfo;
    CLoginSupport login;
    CP2PTracker p2p;
    std::list<SMessage> msgQueue;
    bool bAcceptNewClients;
    CLinksManager links;
    int nGamePort;
    CNodeAddress addr;
    CNodeAddress gameHostAddress;

    SPeer *GetClientByAddr(const CNodeAddress &addr);
    SPeer *GetClient(CP2PTracker::UCID nID);
    void AddClient(const SClientAddressInfo &addr, CP2PTracker::UCID clientID);
    void AddNewP2PClient(const SClientAddressInfo &addr, CP2PTracker::UCID clientID);
    void RemoveClient(CP2PTracker::UCID nID);
    void StepInactive();
    void StepConnecting();
    void StepActive(float fDeltaTime);
    // step net driver
    void Step();

    void ProcessIncomingMessages();
    void ProcessLogin(const CNodeAddress &addr, CBitStream &bits);
    void ProcessNormal(const CNodeAddress &addr, CBitStream &bits);
    void AddOutputMessage(EMessage msg, const CP2PTracker::UCID &_from,
                          CMemoryStream &data, const std::vector<CP2PTracker::UCID> &received);

    void PollMessages(SPeer *pPeer);

    //
    // multichannel
    std::vector<std::hash_set<BYTE>> channelMsgTypes;

    struct SChannelMessage
    {
      EMessage msg;
      int nClientID;
      CPtr<IDataStream> pPkt;
    };

    std::vector<std::list<SChannelMessage>> channelMsgs;
    std::vector<int> existingChannels;

    bool bMultiChannel;

    //
    // thread
    HANDLE hThread;
    HANDLE hFinishReport;
    HANDLE hStopCommand;
    NWin32Helper::CCriticalSection criticalSection;

    // CRAP{ for traffic to winsock measurement
    NTimer::STime lastTrafficCheckTime;
    int nSent;
    // CRAP}

#ifdef __TEST_LAGS__
    NTimer::STime lastSendTime;
    NTimer::STime lastReceiveTime;
    int nMsgCanReceive;
    NTimer::STime lagPeriod;
    bool bPaused;
    bool bSendNow;
    bool bReceiveNow;

    std::list<CPtr<IDataStream>> msgToSendBroadcast;
    std::list<std::pair<int, CPtr<IDataStream>>> msgToSendDirect;

    bool AnalyzeLags();
#endif __TEST_LAGS__

    //
    void ResizeArrays(int nNewSize);
    void StepMultiChannel();

    virtual void STDCALL StartGameInfoSend(IDataStream *pData);

  public:
    CNetDriver();
    ~CNetDriver();
    //
    bool STDCALL Init(APPLICATION_ID _nApplicationID, int _nGamePort, bool _bClientOnly) override;
    // get current state (active/inactive/connecting)
    EState STDCALL GetState() const override { return state; }
    // get reject reason (then)
    EReject STDCALL GetRejectReason() const override { return lastReject; }
    // connect to the game with particular address
    void STDCALL ConnectGame(const INetNodeAddress *pAddr, IDataStream *pPwd) override;
    // start game (server)
    void STDCALL StartGame() override;
    // sending start game info (for server), should be compatible with gamespy
    void STDCALL StartGameInfoSend(const SGameInfo &gameInfo) override;
    // stop sending game info (for server)
    void STDCALL StopGameInfoSend() override;
    // start accepting new players (for server)
    void STDCALL StartNewPlayerAccept() override;
    // stop accepting new players (for server)
    void STDCALL StopNewPlayerAccept() override;
    // get game info (for client)
    bool STDCALL GetGameInfo(int nIdx, INetNodeAddress *pAddr, bool *pWrongVersion, float *pPing, SGameInfo *pGameInfo) override;
    // refresh servers list ( for client )
    void STDCALL RefreshServersList() override {}
    // send broadcast message for all
    bool STDCALL SendBroadcast(IDataStream *pPkt) override;
    // send direct message for client 'nClient'
    bool STDCALL SendDirect(int nClient, IDataStream *pPkt) override;
    // kick player 'nClient'
    void STDCALL Kick(int nClient) override;
    // get next message
    bool STDCALL GetMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt) override;
    // ping of the client, -1 if client doesn't exist
    const float STDCALL GetPing(int nClientID) override;
    // time since last message was received from this client
    const float STDCALL GetTimeSinceLastRecv(int nClientID) override;
    // 
    // CRAP functions to work with GameSpy
    //
    SOCKET STDCALL GetSocket() override;
    sockaddr * STDCALL GetSockAddr() override;

    //
    void STDCALL AddChannel(int nChannelID, const std::hash_set<BYTE> &channelMessages) override;
    void STDCALL RemoveChannel(int nChannelID) override;
    // received is not filled in!
    bool STDCALL GetChannelMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt, int nChannel) override;
    // for debug
    const char * STDCALL GetAddressByClientID(int nClientID) const override;

    void STDCALL PauseNet() override;
    void STDCALL UnpauseNet() override;
    void STDCALL SetLag(NTimer::STime period) override;

    // thread functions
    friend DWORD WINAPI TheThreadProc(LPVOID lpParameter);

    void StartThread();
    bool CanWork();
    void FinishThread();
  };

  INetDriver *MakeDriver(int nGamePort, bool bClientOnly = false);

};

#endif