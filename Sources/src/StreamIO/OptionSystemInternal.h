#ifndef __OPTIONSSYSTEMINTERNAL_H__
#define __OPTIONSSYSTEMINTERNAL_H__

#pragma once

#include "OptionSystem.h"
#include "../Misc/VarSystemInternal.h"

struct SOption : SSerialVariantT
{
  int nEditorType;// editor type: 0 - checkbox, 1 - slider, 2 - droplist
  std::string szAction;// action to do then data changed
  std::string szActionFill;// action to fill with data
  DWORD dwFlags;// flags
  int nOrder;// sequential order of this opts
  SSerialVariantT defaultValue;// 
  bool bInstantApply;// option must be applied immidiately when changed.
  //
  SOption() : nEditorType(EOET_TEXT_ENTRY), dwFlags(0), nOrder(0) {}
  //
  void Set(const variant_t &var) { *(static_cast<SSerialVariantT *>(this)) = var; }
  const variant_t &Get() const { return *(static_cast<const variant_t *>(this)); }
  //
  int STDCALL operator&(IStructureSaver &ss) override
  {
    CSaverAccessor saver = &ss;
    saver.AddTypedSuper(1, static_cast<SSerialVariantT *>(this));
    saver.Add(2, &nEditorType);
    saver.Add(3, &szAction);
    saver.Add(4, &szActionFill);
    saver.Add(5, &dwFlags);
    saver.Add(6, &nOrder);
    saver.Add(7, &defaultValue);
    saver.Add(8, &bInstantApply);
    return 0;
  }

  int STDCALL operator&(IDataTree &ss) override
  {
    CTreeAccessor saver = &ss;
    saver.Add("EditorType", &nEditorType);
    saver.Add("Flags", &dwFlags);
    saver.Add("Order", &nOrder);
    saver.AddTypedSuper(static_cast<SSerialVariantT *>(this));
    saver.Add("Action", &szAction);
    saver.Add("ActionFill", &szActionFill);
    saver.Add("Default", &defaultValue);
    saver.Add("InstantApply", &bInstantApply);
    return 0;
  }
};

interface IOption;

class COptionSystem : public CTVarSystem<SOption, CTRefCount<IOptionSystem>>
{
  OBJECT_SERVICE_METHODS(COptionSystem);
  using CBase = CTVarSystem<SOption, CTRefCount<IOptionSystem>>;
  //
  mutable SOptionDesc descriptor;// temporal descriptor to store options in
  mutable std::vector<SOptionDropListValue> droplist;// temporal list for 'droplist' control (localized)

  void InnerSet(const std::string &szVarName, const variant_t &var);

public:
  COptionSystem();

  ~COptionSystem() override {}
  //
  bool STDCALL Set(const std::string &szVarName, const variant_t &var) override;
  // get option descriptor
  const SOptionDesc * STDCALL GetDesc(const std::string &szVarName) const override;
  // get values ​​for option droplist editor type
  const std::vector<SOptionDropListValue> & STDCALL GetDropValues(const std::string &szVarName) const override;
  // begin to iterate through all variables
  IOptionSystemIterator * STDCALL CreateIterator(DWORD dwMask = 0xffffffff) override;
  // serialize to configuration file
  bool STDCALL SerializeConfig(IDataTree *pSS) override;
  void STDCALL Init() override;
  void STDCALL Repair(IDataTree *pSS, bool bToDefault) override;
};

struct SOptionSorter
{
  void Sort(std::list<COptionSystem::CVarsMap::const_iterator> &vals);
};

class COptionMaskAccepter
{
  const DWORD dwMask;

public:
  COptionMaskAccepter(const DWORD _dwMask) : dwMask(_dwMask) {}
  const bool operator()(const SOption &opt) const { return dwMask == static_cast<DWORD>(-1) || (opt.dwFlags & dwMask) != 0; }
};

class COptionSystemIterator : public CTVarSystemIterator<COptionSystem, CTRefCount<IOptionSystemIterator>, SOptionSorter, COptionMaskAccepter>
{
  using CBase = CTVarSystemIterator<COptionSystem, CTRefCount<IOptionSystemIterator>, SOptionSorter, COptionMaskAccepter>;
  CPtr<COptionSystem> pOS;
  //
  COptionSystem *GetVS() const { return pOS; }

public:
  COptionSystemIterator(COptionSystem *pOS, DWORD dwMask);
  ~COptionSystemIterator() override {}
  // get option descriptor
  const SOptionDesc * STDCALL GetDesc() const override;
  // get values ​​for option droplist editor type
  const std::vector<SOptionDropListValue> & STDCALL GetDropValues() const override;
};

#endif // __OPTIONSSYSTEMINTERNAL_H__