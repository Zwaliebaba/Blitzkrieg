#ifndef __MAPSETTINGSWRAPPER_H__
#define __MAPSETTINGSWRAPPER_H__

#pragma once

#include "../RandomMapGen/MapInfo_Types.h"
#include "../StreamIO/OptionSystem.h"
#include "MultiplayerCommandManager.h"
// #include "OptionEntryWrapper.h"
#include "../UI/UI.h"
class COptionsListWrapper;

class CMapSettingsWrapper : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CMapSettingsWrapper);

public:
  struct CFakeOptionSystem : IOptionSystem
  {
    OBJECT_COMPLETE_METHODS(CFakeOptionSystem);
    bool bServer;// server stores options.
  public:
    SMultiplayerGameSettings settings;// copy of settings.

    CFakeOptionSystem() {}

    CFakeOptionSystem(const bool _bIsServer)
      : bServer(_bIsServer) {}

    bool Set(const std::string &szVarName, const variant_t &var) override;
    bool Get(const std::string &szVarName, variant_t *pVar) const override;

    // all others forward to main option system
    const SOptionDesc * GetDesc(const std::string &szVarName) const override { return GetSingleton<IOptionSystem>()->GetDesc(szVarName); }
    const std::vector<SOptionDropListValue> & GetDropValues(const std::string &szVarName) const override { return GetSingleton<IOptionSystem>()->GetDropValues(szVarName); }
    IOptionSystemIterator * CreateIterator(const DWORD dwMask = 0xffffffff) override { return GetSingleton<IOptionSystem>()->CreateIterator(dwMask); }
    bool SerializeConfig(IDataTree *pSS) override { return GetSingleton<IOptionSystem>()->SerializeConfig(pSS); }
    bool Remove(const std::string &szVarName) override { return GetSingleton<IOptionSystem>()->Remove(szVarName); }
    bool RemoveByMatch(const std::string &szVarMatch) override { return GetSingleton<IOptionSystem>()->RemoveByMatch(szVarMatch); }
    bool ChangeSerialize(const std::string &szVarMatch, bool bInclude) override { return GetSingleton<IOptionSystem>()->ChangeSerialize(szVarMatch, bInclude); }
    bool IsChanged() const override { return GetSingleton<IOptionSystem>()->IsChanged(); }
    void Init() override {}
    void Repair(IDataTree *pSS, const bool bToDefault) override {}
  };

private:
  CPtr<IUIListControl> pList;
  CPtr<IUIStatic> pGameType;
  CPtr<CFakeOptionSystem> pOptionSystem;
  CPtr<COptionsListWrapper> pListWrapper;
  bool bCanChange;// is
  int nFlag;// options flag


public:
  CMapSettingsWrapper() {}
  CMapSettingsWrapper(bool bCanChange, int bFlag);
  const SMultiplayerGameSettings &GetSettings();
  const SMultiplayerGameSettings &CMapSettingsWrapper::GetSettingsWOApply() const;

  virtual bool ProcessMessage(const SGameMessage &msg);
  void Init(IUIListControl *_pList, IUIStatic *_pGameType);
  void Init(const SMultiplayerGameSettings &_settings);

  void SetGameType(int /* SQuickLoadMapInfo::EMultiplayerMapType */ nGameType);
  void Apply();

};

#endif // __MAPSETTINGSWRAPPER_H__