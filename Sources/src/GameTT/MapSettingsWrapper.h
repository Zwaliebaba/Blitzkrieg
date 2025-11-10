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

    bool STDCALL Set(const std::string &szVarName, const variant_t &var) override;
    bool STDCALL Get(const std::string &szVarName, variant_t *pVar) const override;

    // all others forward to main option system
    const SOptionDesc * STDCALL GetDesc(const std::string &szVarName) const override { return GetSingleton<IOptionSystem>()->GetDesc(szVarName); }
    const std::vector<SOptionDropListValue> & STDCALL GetDropValues(const std::string &szVarName) const override { return GetSingleton<IOptionSystem>()->GetDropValues(szVarName); }
    IOptionSystemIterator * STDCALL CreateIterator(const DWORD dwMask = 0xffffffff) override { return GetSingleton<IOptionSystem>()->CreateIterator(dwMask); }
    bool STDCALL SerializeConfig(IDataTree *pSS) override { return GetSingleton<IOptionSystem>()->SerializeConfig(pSS); }
    bool STDCALL Remove(const std::string &szVarName) override { return GetSingleton<IOptionSystem>()->Remove(szVarName); }
    bool STDCALL RemoveByMatch(const std::string &szVarMatch) override { return GetSingleton<IOptionSystem>()->RemoveByMatch(szVarMatch); }
    bool STDCALL ChangeSerialize(const std::string &szVarMatch, bool bInclude) override { return GetSingleton<IOptionSystem>()->ChangeSerialize(szVarMatch, bInclude); }
    bool STDCALL IsChanged() const override { return GetSingleton<IOptionSystem>()->IsChanged(); }
    void STDCALL Init() override {}
    void STDCALL Repair(IDataTree *pSS, const bool bToDefault) override {}
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

  virtual bool STDCALL ProcessMessage(const SGameMessage &msg);
  void Init(IUIListControl *_pList, IUIStatic *_pGameType);
  void Init(const SMultiplayerGameSettings &_settings);

  void SetGameType(int /* SQuickLoadMapInfo::EMultiplayerMapType */ nGameType);
  void Apply();

};

#endif // __MAPSETTINGSWRAPPER_H__