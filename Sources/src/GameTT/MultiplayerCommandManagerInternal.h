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
  void AddCommandToUI(SToUICommand &cmd) override;
  void AddNotificationFromUI(SFromUINotification &notify) override;

  // recieveing. 
  // returns false;
  bool GetCommandToUI(SToUICommand *pCmd) override;
  bool GetNotificationFromUI(SFromUINotification *pNotify) override;

  bool PeekCommandToUI(SToUICommand *pCmd) override;
  bool PeekNotificationFromUI(SFromUINotification *pNotify) override;

  // chat
  SChatMessage * GetChatMessageFromUI() override;
  SChatMessage * GetChatMessageToUI() override;
  SChatMessage * PeekChatMessageToUI() override;
  void AddChatMessageToUI(SChatMessage *pMessage) override;
  void AddChatMessageFromUI(SChatMessage *pMessage) override;

  void InitUISide() override;

  void SetConnectionType(const enum EMultiplayerConnectionType eType) override { eConnectionType = eType; }
  enum EMultiplayerConnectionType GetConnectionType() const override { return eConnectionType; }

  void DelayedNotification(SFromUINotification &notify) override;
  void SendDelayedNotification() override;
};

#endif // !defined(AFX_MULTIPLAYERCOMMANDMANAGERINTERNAL_H__9EC0CE88_5727_4A04_BE2F_8F45320AD784__INCLUDED_)