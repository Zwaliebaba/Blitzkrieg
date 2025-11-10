#ifndef __MULTIPLAYERTRANSCEIVER_H__
#define __MULTIPLAYERTRANSCEIVER_H__

#pragma once

#include "Transceiver.h"
#include "iMain.h"
#include "../StreamIO/StreamIOHelper.h"

interface IMultiplayer;
interface IAILogic;
interface IAILogicCommand;
interface ICommandsHistory;

class CTimeOut
{
  CPtr<IMultiplayer> pMultiplayer;

  int nMyNumber;
  int nTimeOutPlayer;

  NTimer::STime timeOutTime;
  NTimer::STime timeBWTimeOuts;
  NTimer::STime lastTimeOutTime;

public:
  CTimeOut() : nMyNumber(-1), nTimeOutPlayer(-1) {}

  void Init(IMultiplayer *pMultiplayer);
  void InitGameStart(int n);

  bool IsActive() const { return nTimeOutPlayer != -1; }
  const int GetTimeOutPlayer() const { return nTimeOutPlayer; }

  void SetTimeOut(int nPlayer);
  void UnsetTimeOut(bool bByClientCommand);
  void Segment();
};

class CMultiPlayerTransceiver : public CTRefCount<ITransceiver>
{
  OBJECT_SERVICE_METHODS(CMultiPlayerTransceiver);
  DECLARE_SERIALIZE;

  //
  struct SPlayerInfo
  {
    std::wstring szName;// player's name
    int nSide;// player side
    int nLogicID;// logical ID (for right sequencing)

    WORD totalLagTime;// in seconds
    NTimer::STime lastLagUpdateTime;
  };

  //
  static const int MAX_LATENCY;
  static const NTimer::STime TIME_TO_START_LAG_BY_NO_SEGMENT_DATA;
  //
  CPtr<IAILogic> pAILogic;// shortcut to AI logic
  CPtr<IMultiplayer> pMultiplayer;
  CStreamAccessor pktOutgoing;

  int nLatency;
  // must be a multiple of nLatency!
  int nSegmentsPackSize;

  int nNumPlayers;// number of players
  int nNumPlayersInMap;// maximum number of players on this map
  int nSegment;// current segment number
  int nMyNumber;// "our" player's number

  // whether the player sent all commands for each of the segments (bitmap by relative segment numbers)
  std::vector<WORD> segmFinished;
  // number of the last common segment received from the player
  std::vector<int> lastFinishedSegment;
  std::vector<int> timeOfLastSegmFinished;
  NTimer::STime lastTimeToCheckNoSegmDataLag;
  std::vector<int> noSegmDataLags;
  std::vector<int> loadingPlayers;
  int nLoadingPlayers;

  WORD wMask;

  // commands for each player ( [i][j] i - segment number, j - player number)
  using CAILogicCommandsList = std::list<CPtr<IAILogicCommand>>;
  CArray2D<CAILogicCommandsList> cmds;
  // all commands received from the client but not yet sent
  CAILogicCommandsList receivedCmds;

  using CPlayersList = std::list<SPlayerInfo>;
  CPlayersList players;

  // team history
  CPtr<ICommandsHistory> pCmdsHistory;
  // common segment number - for command history
  long nCommonSegment;

  bool bGameStarted;
  bool bHistoryPlaying;

  int nTimeToAllowDropByLag;// in sec

  bool bTotalOutOfSync;
  float fLastSentMultiplayerTime;

  bool bSpeedSet;

  CTimeOut timeOut;

  //
  // send a message about the end of a segment
  void SegmentFinished();
  // get the relative segment number in the past (with nLatency shifted back)
  const int GetPastSegmentNum(int nLatency) const;
  // get the total segment number in the past (with nLatency shift backwards)
  const int GetCommonPastSegmentNum() const;
  //
  void ProcessMultiplayerCommands();
  //
  void LoadGameSettings();
  void InitVariables();
  //
  void SendChatMessages();
  void SetPlayerLag(int nPlayer, bool bSet);
  void SetPlayerLoading(int nPlayer, bool bSet);
  void SetNoSegmentDataLags();
  void RemovePlayer(int nLogicID);

  void UpdateLags();
  void SetPlayerAlive(int nPlayer);
  void SendIAmAlive();

public:
  void STDCALL Init(ISingleton *pSingleton, int nMultiplayerType) override;
  void STDCALL PreMissionInit() override {}
  // destruct
  void STDCALL Done() override;
  // 
  // set latency for smooth net playing
  void SetLatency(int nSegments) override;
  // perform segments for AI
  void STDCALL DoSegments() override;
  // register group of units to AI
  int STDCALL CommandRegisterGroup(IRefCount **pUnitsBuffer, int nLen) override;
  // unregistered group
  void STDCALL CommandUnregisterGroup(WORD wGroup) override;
  // send command to group of units
  void STDCALL CommandGroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) override;
  // set single command to call planes, reinforcements, etc. 
  int STDCALL CommandUnitCommand(const struct SAIUnitCmd *pCommand) override;
  // show different areas... this command actually has been doing only on client, but it needs group to be registered
  void STDCALL CommandShowAreas(int nGroupID, int nAreaType, bool bShow) override;
  // process pCommand received in the current segment
  void STDCALL AddCommandToSend(IAILogicCommand *pCommand) override;

  // number of players currently playing
  int STDCALL GetNumberOfPlayers() const override;

  bool STDCALL JoinToServer(const char *pszIPAddress, int nPort, bool bPasswordRequired, const char *pszPassword) override;
  void STDCALL CreateServer() override;
  void STDCALL InitByCreateServersList() override;

  // client commands
  void STDCALL CommandClientTogglePause() override;
  void STDCALL CommandClientSpeed(int nChange) override;
  void STDCALL CommandClientDropPlayer(const WORD *pszPlayerNick) override;

  void STDCALL CommandTimeOut(bool bSet) override;

  NTimer::STime STDCALL GetMultiplayerTime() override;

  void STDCALL LoadAllGameParameters() override;

  void STDCALL SetTotalOutOfSync() override;

  void STDCALL GameFinished() override;

  // for debug
  interface INetDriver * STDCALL GetInGameNetDriver() const override;
};

class CChangeTransceiverCommand : public CTRefCount<IInterfaceCommand>
{
  OBJECT_SERVICE_METHODS(CChangeTransceiverCommand);
  //
  int nTransceiverTypeID;// type id of the transceiver to create
  int nMultiplayerType;// LAN/Address Book/GameSpy
  bool bInitByServersList;
  NTimer::STime timeDelayed;

public:
  CChangeTransceiverCommand() : nTransceiverTypeID(-1), timeDelayed(0) {}
  //
  void STDCALL Configure(const char *pszConfig) override
  {
    if (!pszConfig) return;
    std::vector<std::string> szParams;
    NStr::SplitStringWithMultipleBrackets(pszConfig, szParams, ' ');

    NI_ASSERT_TF(NStr::IsDecNumber(szParams[0]), NStr::Format("Can't create transceiver %s", pszConfig), return);
    nTransceiverTypeID = NStr::ToInt(szParams[0]);

    bInitByServersList = false;
    if (szParams.size() >= 2)
    {
      NI_ASSERT_TF(NStr::IsDecNumber(szParams[1]), NStr::Format("Can't create transceiver %s", pszConfig), return);
      nMultiplayerType = NStr::ToInt(szParams[1]);
      if (szParams.size() == 3) bInitByServersList = true;
    }
    else nMultiplayerType = -1;
  }

  //
  void STDCALL Exec(IMainLoop *pML) override
  {
    NI_ASSERT_TF(nTransceiverTypeID != -1, "Can't create unknown transceiver", return);

    // delete all multiplayer info
    ITransceiver *pCurTransceiver = GetSingleton<ITransceiver>();
    if (auto pMultiTransceiver = dynamic_cast<CMultiPlayerTransceiver *>(pCurTransceiver)) pMultiTransceiver->Done();

    UnRegisterSingleton(ITransceiver::tidTypeID);
    ITransceiver *pTrans = CreateObject<ITransceiver>(nTransceiverTypeID);
    pTrans->Init(GetSingletonGlobal(), nMultiplayerType);
    if (bInitByServersList) pTrans->InitByCreateServersList();
    RegisterSingleton(ITransceiver::tidTypeID, pTrans);
  }

  //
  void STDCALL SetDelayedTime(const NTimer::STime &timeToExecute) override { timeDelayed = timeToExecute; }
  NTimer::STime STDCALL GetDelayedTime() const override { return timeDelayed; }
  //
  int STDCALL operator&(IStructureSaver &ss) override
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &nTransceiverTypeID);
    saver.Add(2, &timeDelayed);
    return 0;
  }
};

#endif // __MULTIPLAYERTRANSCEIVER_H__