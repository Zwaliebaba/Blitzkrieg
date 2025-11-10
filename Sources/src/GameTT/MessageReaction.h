#ifndef __MESSAGEREACTION_H__
#define __MESSAGEREACTION_H__

#pragma once
#include "iMission.h"

// encapsulate (atomic) reaction on specific message

interface IMessageReaction : IRefCount
{
  // return 0 means that message should be considered not processed
  virtual bool STDCALL Execute() = 0;
};

// container, encapsulate logic of messages reaction for some object

interface IMessageLink : IRefCount
{
  virtual IMessageReaction * STDCALL Configure(int nMessageID, int nParam) = 0;
};

enum EMessageLink
{
  EML_ESCAPE_MENU,

};


// convert from text value to its representation


interface ILoadHelper : IRefCount
{
  virtual int STDCALL Get(const std::string &szLoaded) = 0;
};


// IN ORDER TO switch to id's from strings.


enum ELoadHelperID
{
  ELH_ATOM_REACTION_TYPE,// Commands->Reactions->Type
  ELH_INCOMING_MESSAGE_ID,// Commands->IncomingMessage->first
  ELH_INCOMING_MESSAGE_NPARAM,// Commands->IncomingMessage->second
  ELH_MESSAGE_TO_INPUT_PARAM1,// Commands->Reactions->Param1 (if type == EMART_MESSAGE_TO_INPUT )
  ELH_MESSAGE_TO_INPUT_PARAM2,// Commands->Reactions->Param2 (if type == EMART_MESSAGE_TO_INPUT )
  ELH_MESSAGE_TO_MAINLOOP_PARAM1,// Commands->Reactions->Param1 (if type == EMART_MESSAGE_TO_MAINLOOP )
  ELH_ATOM_CUSTOM_CHECK_KEY,// Commands->CustomCheck
  ELH_ATOM_CUSTOM_CHECK_RETURN,// Commands->CustomCheckReturn
  ELH_PAUSE_TYPE// pause type
};

// parameters for custom check
using CCustomCheckParams = std::vector<std::string>;

// container of messages links (may be some specific message links)

interface IMessageLinkContainer : IRefCount
{
  // type ID
  enum { tidTypeID = GAMETT_MESSAGELINK_CONTAINER };

  virtual void STDCALL SetInterface(class CInterfaceScreenBase *pInterface) = 0;
  // tries to process messages via all message links. 
  // implements reaction by self
  virtual bool STDCALL ProcessMessage(const SGameMessage &msg) = 0;

  virtual void Init() = 0;
  virtual void Clear() = 0;
  // access to message links by specific ID
  virtual IMessageLink * STDCALL GetMessageLink(enum EMessageLink eLinkID) = 0;

  // creation and registration of messages links
  virtual void STDCALL RegisterMessageLink(IMessageLink *pMessageLink, enum EMessageLink eLinkID) = 0;
  virtual void STDCALL LoadMessageLink(const std::string &szFile, enum EMessageLink eLinkID) = 0;

  // access to load helper
  virtual ILoadHelper * STDCALL GetLoadHelper(int /* ELoadHelperID */nLoadHelperID) = 0;

  // custom check call
  virtual int STDCALL CustomCheck(int nCustomCheckKey, const CCustomCheckParams &checkParams) = 0;

  // custom reaction launch
  virtual void STDCALL CustomReaction(const std::string &szCustomReactionName) = 0;

  // set text to desider window
  virtual void STDCALL SetWindowText(int nElementID, const WORD *pszText) = 0;
};

#endif // __MESSAGEREACTION_H__