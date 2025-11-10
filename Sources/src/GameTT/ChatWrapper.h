#ifndef __CHATMANAGER_H__
#define __CHATMANAGER_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

interface IWhisper
{
  virtual const WORD *GetDestinationName() = 0;
};

class CChatWrapper
{

  CPtr<IUIColorTextScroll> pChatText;
  CPtr<IUIEditBox> pChatEdit;
  CPtr<IMPToUICommandManager> pCommandManager;
  int nWhisperButton;
  IWhisper *pWhisper;

  bool bEmptyChat;

  void AddEditBoxText(bool bWhisper);
  void ClearEditBoxText();

public:
  void Init(IUIColorTextScroll *_pChatText,
            IUIEditBox *_pChatEdit,
            int _nWhisperButton,
            IWhisper *_pWhisper);

  // direct to local window
  void AddImportantText(const WORD *wszMessage);
  bool ProcessMessage(const SGameMessage &msg);
  void AddMessageToChat(const struct SChatMessage *pChatMessage);
};

#endif // __CHATMANAGER_H__