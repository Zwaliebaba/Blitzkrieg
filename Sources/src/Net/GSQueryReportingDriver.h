#ifndef __GS_QUERY_REPORTING_DRIVER__
#define __GS_QUERY_REPORTING_DRIVER__
// //////////////////////////////////////////////
#pragma once
// //////////////////////////////////////////////
#include "NetDriver.h"
#include "../GameSpy/QueryReporting/gqueryreporting.h"

#include "../Misc/Thread.h"
#include "../Misc/Win32Helper.h"

// //////////////////////////////////////////////
class CGSQueryReportingDriver : public INetDriver, public CThread
{
  OBJECT_NORMAL_METHODS(CGSQueryReportingDriver);

  // thread
  NWin32Helper::CCriticalSection criticalSection;
  bool bInitialized;

  // callbacks
  static void qr_basic_callback(char *pszOutBuf, int nMaxLen, void *pUserData);
  static void qr_info_callback(char *pszOutBuf, int nMaxLen, void *pUserData);
  static void qr_rules_callback(char *pszOutBuf, int nMaxLen, void *pUserData);
  static void qr_players_callback(char *pszOutBuf, int nMaxLen, void *pUserData);
  //
  // callback helpers
  void QRBasicCallBack(char *pszOutBuf, int nMaxLen);
  void QRInfoCallBack(char *pszOutBuf, int nMaxLen);
  void QRRulesCallBack(char *pszOutBuf, int nMaxLen);
  void QRPlayersCallBack(char *pszOutBuf, int nMaxLen);

  //
  SGameInfo gameInfo;
  int nGamePort;

  //
  qr_t gsHandler;

protected:
  void Step() override;

public:
  CGSQueryReportingDriver();
  virtual ~CGSQueryReportingDriver();
  //
  bool STDCALL Init(APPLICATION_ID _nApplicationID, int _nGamePort, bool _bClientOnly) override;
  // get current state (active/inactive/connecting)
  EState STDCALL GetState() const override;
  // get reject reason (then)
  EReject STDCALL GetRejectReason() const override;
  // start game (server)
  void STDCALL StartGame() override;
  // sending start game info (for server)
  void STDCALL StartGameInfoSend(const SGameInfo &gameInfo) override;
  // stop sending game info (for server)
  void STDCALL StopGameInfoSend() override;
  // start accepting new players (for server)
  void STDCALL StartNewPlayerAccept() override;
  // stop accepting new players (for server)
  void STDCALL StopNewPlayerAccept() override;

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

  // unnecessary functions (related with messages sending)
  // get game info (for client)
  bool STDCALL GetGameInfo(int nIdx, INetNodeAddress *pAddr, bool *pWrongVersion, float *pPing, SGameInfo *pGameInfo) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  void STDCALL RefreshServersList() override { NI_ASSERT_T(false, "wrong call"); }

  // connect to the game with particular address
  void STDCALL ConnectGame(const INetNodeAddress *pAddr, IDataStream *pPwd) override { NI_ASSERT_T(false, "wrong call"); }

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

  // auxiliary multichannel functions
  void STDCALL AddChannel(const int nChannelID, const std::hash_set<BYTE> &channelMessages) override { NI_ASSERT_T(false, "wrong call"); }
  void STDCALL RemoveChannel(const int nChannelID) override { NI_ASSERT_T(false, "wrong call"); }

  bool STDCALL GetChannelMessage(EMessage *pMsg, int *pClientID, int *received, IDataStream *pPkt, const int nChannel) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }
};

// //////////////////////////////////////////////
#endif __GS_QUERY_REPORTING_DRIVER__