#ifndef __LAN_CHAT_H__
#define __LAN_CHAT_H__

#pragma once

#include "GameCreationInterfaces.h"
#include "MessagesStore.h"

interface INetDriver;

class CLanChat : public IChat
{
  OBJECT_COMPLETE_METHODS(CLanChat);

  CPtr<INetDriver> pNetDriver;
  CMessagesStore messages;

public:
void InitGSChat(const wchar_t *pszUserName) override {}
void InitInGameChat(INetDriver *pNetDriver) override;
void DestroyInGameChat() override {}

void SendMessage(const wchar_t *pszMessage, const SPlayerInfo &ourPlayer) override;
void SendWhisperMessage(const wchar_t *pszMessage, const SPlayerInfo &toPlayer, const SPlayerInfo &ourPlayer) override;
void SendMessage(const wchar_t *pszMessage, const wchar_t *wszToPlayer, const bool bWhisper) override {}
void Segment() override;

  IMultiplayerMessage * GetMessage() override;

  void UserModeChanged(const EUserMode eMode) override {};
};

#endif // __LAN_CHAT_H__