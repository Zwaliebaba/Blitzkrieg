// MultiplayerCommandManagerInternal.h: interface for the MultiplayerCommandManagerInternal class.
//
// /////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIPLAYERCOMMANDMANAGERINTERNAL_H__9EC0CE88_5727_4A04_BE2F_8F45320AD784__INCLUDED_)
#define AFX_MULTIPLAYERCOMMANDMANAGERINTERNAL_H__9EC0CE88_5727_4A04_BE2F_8F45320AD784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MultiplayerCommandManager.h"

class CMPToUICommandManager : public IMPToUICommandManager
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CMPToUICommandManager);

  std::list<SToUICommand> commandsToUI;
  std::list<SFromUINotification> notificationsFromUI;

  std::list<CPtr<SChatMessage>> chatMessagesToUI;
  std::list<CPtr<SChatMessage>> chatMessagesFromUI;
  CPtr<SChatMessage> pTakenMessage;

  EMultiplayerConnectionType eConnectionType;

  SFromUINotification delayedNotificaion;

  void Clear()
  {
    commandsToUI.clear();
    notificationsFromUI.clear();
    chatMessagesToUI.clear();
    chatMessagesFromUI.clear();
    delayedNotificaion.Clear();
  }

public:
  // adding
  void STDCALL AddCommandToUI(SToUICommand &cmd) override;
  void STDCALL AddNotificationFromUI(SFromUINotification &notify) override;

  // recieveing. 
  // returns false;
  bool STDCALL GetCommandToUI(SToUICommand *pCmd) override;
  bool STDCALL GetNotificationFromUI(SFromUINotification *pNotify) override;

  bool STDCALL PeekCommandToUI(SToUICommand *pCmd) override;
  bool STDCALL PeekNotificationFromUI(SFromUINotification *pNotify) override;

  // chat
  SChatMessage * STDCALL GetChatMessageFromUI() override;
  SChatMessage * STDCALL GetChatMessageToUI() override;
  SChatMessage * STDCALL PeekChatMessageToUI() override;
  void STDCALL AddChatMessageToUI(SChatMessage *pMessage) override;
  void STDCALL AddChatMessageFromUI(SChatMessage *pMessage) override;

  void STDCALL InitUISide() override;

  void STDCALL SetConnectionType(const enum EMultiplayerConnectionType eType) override { eConnectionType = eType; }
  enum EMultiplayerConnectionType STDCALL GetConnectionType() const override { return eConnectionType; }

  void STDCALL DelayedNotification(SFromUINotification &notify) override;
  void STDCALL SendDelayedNotification() override;
};

#endif // !defined(AFX_MULTIPLAYERCOMMANDMANAGERINTERNAL_H__9EC0CE88_5727_4A04_BE2F_8F45320AD784__INCLUDED_)