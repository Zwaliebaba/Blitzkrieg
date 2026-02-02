#ifndef __SERVERS_LIST_H__
#define __SERVERS_LIST_H__

#pragma once

#include "GameCreationInterfaces.h"
#include "ServerInfo.h"
#include "MessagesStore.h"

#include "../Net/NetDriver.h"

interface INetNodeAddress;
interface IMultiplayerMessage;

class CServersList : public IServersList
{
  CPtr<INetDriver> pNetDriver;

  using CServers = std::list<SServerInfo>;
  CServers servers;

  WORD wCurUniqueId;

  CMessagesStore messages;
  NTimer::STime lastServersCheck;

  //
  void AddServer(INetNodeAddress *pAddress, float fPing, const struct INetDriver::SGameInfo &gameInfo, bool bSameVersion);
  void RefreshServerInfo(const SServerInfo &info, bool bSameVersion);
  void RemoveServer(const SServerInfo &info);

  void RefreshServersList();

  const SServerInfo *FindServerByID(WORD wServerID) const;

protected:
  //
  void Init(INetDriver *pNetDriver);
  void DestroyNetDriver();

  INetDriver *GetNetDriver() const { return pNetDriver; }

  virtual interface INetDriver *CreateInGameNetDriver(int nPort) = 0;
  virtual void CreateInGameChat(CPtr<IChat> *pChat, interface INetDriver *pNetDriver) = 0;

public:
  CServersList() {}

  IMultiplayerMessage * GetMessage() override;
  void Segment() override;

  bool CanJoinToServerByID(WORD wServerID) override;
  bool IsNeedPassword(WORD wServerID) const override;
  interface IGameCreation * JoinToServerByID(WORD wServerID, CPtr<IChat> *pChat, bool bPasswordRequired, const std::string &szPassword) override;

  interface IGameCreation * JoinToServerByAddress(INetNodeAddress *pAddress, CPtr<IChat> *pChat, int nPort, bool bPasswordRequired, const std::string &szPassword) override;

  void Refresh() override;

  interface INetDriver * GetInGameNetDriver() const override { return nullptr; }
};

class CLanServersList : public CServersList
{
  OBJECT_COMPLETE_METHODS(CLanServersList);

protected:
  void CreateInGameChat(CPtr<IChat> *pChat, interface INetDriver *pNetDriver) override;
  interface INetDriver *CreateInGameNetDriver(int nPort) override;

public:
  CLanServersList() {}
  void Init();

  interface IGameCreation * CreateServer(const SGameInfo &gameInfo, const SQuickLoadMapInfo &mapInfo, CPtr<IChat> *pChat) override;
};

class CInternetServersList : public CServersList
{
  OBJECT_COMPLETE_METHODS(CInternetServersList);

protected:
  void CreateInGameChat(CPtr<IChat> *pChat, interface INetDriver *pNetDriver) override;
  interface INetDriver *CreateInGameNetDriver(int nPort) override;

public:
  CInternetServersList() {}
  void Init();

  interface IGameCreation * CreateServer(const struct SGameInfo &gameInfo, const struct SQuickLoadMapInfo &mapInfo, CPtr<IChat> *pChat) override;
};

#endif // __SERVERS_LIST_H__