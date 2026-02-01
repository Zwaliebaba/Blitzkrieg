#pragma once

#undef GetMessage

interface IMultiplayerMessage;

class CMessagesStore
{
  std::list<CPtr<IMultiplayerMessage>> messages;
  CPtr<IMultiplayerMessage> pTakenMessage;

public:
  CMessagesStore() {}

  void AddMessage(IMultiplayerMessage *pMessage);
  IMultiplayerMessage *GetMessage();
};