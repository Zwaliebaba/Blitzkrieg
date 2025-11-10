#ifndef __CUSTOMMESSAGEREACTION_H__
#define __CUSTOMMESSAGEREACTION_H__

#pragma once

class CCustomMessageReaction
{
  DECLARE_SERIALIZE;

  using MESSAGE_REACTION = void(*)(class CInterfaceScreenBase *_pInterface);

  struct SReactionDescriptor
  {
    MESSAGE_REACTION pfnReaction;
    SReactionDescriptor() {}

    SReactionDescriptor(MESSAGE_REACTION _pfnReaction)
      : pfnReaction(_pfnReaction) {}
  };

  using CReactions = std::unordered_map<std::string, SReactionDescriptor>;
  CReactions reactions;

public:
  void Init();
  void Clear();
  void LaunchReaction(const std::string &szCutomReactionName, class CInterfaceScreenBase *_pInterface);
};

#endif // __CUSTOMMESSAGEREACTION_H__