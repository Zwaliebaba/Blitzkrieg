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
  void STDCALL InitGSChat(const WORD *pszUserName) override {}
  void STDCALL InitInGameChat(INetDriver *pNetDriver) override;
  void STDCALL DestroyInGameChat() override {}

  void STDCALL SendMessage(const WORD *pszMessage, const SPlayerInfo &ourPlayer) override;
  void STDCALL SendWhisperMessage(const WORD *pszMessage, const SPlayerInfo &toPlayer, const SPlayerInfo &ourPlayer) override;
  void STDCALL SendMessage(const WORD *pszMessage, const WORD *wszToPlayer, const bool bWhisper) override {}
  void STDCALL Segment() override;

  IMultiplayerMessage * STDCALL GetMessage() override;

  void STDCALL UserModeChanged(const EUserMode eMode) override {};
};

#endif // __LAN_CHAT_H__