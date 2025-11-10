#pragma once

#include "../StreamIO/OptionSystem.h"

// ui side

interface IUISetOptionsToUI
{
  // use tempBuffer
  virtual void STDCALL SetSelectionOption(const std::vector<SOptionDropListValue> &szSelections, int nDefault) = 0;
  virtual void STDCALL SetSliderOption(int nMin, int nMax, int nDefault) = 0;
  virtual void STDCALL SetTextOption(const wchar_t*pszEntry) = 0;
  virtual void STDCALL SetTextGameSpyOption(const wchar_t*pszEntry) = 0;
  virtual void STDCALL SetTextNumericOption(int nEnntry) = 0;

  virtual void STDCALL ResetSelection() = 0;
  virtual void STDCALL ResetSlider() = 0;
  virtual void STDCALL ResetTextEntry() = 0;
  virtual void STDCALL ResetNumericEntry() = 0;
  virtual void STDCALL ResetTextGameSpyEntry() = 0;
};

interface IUIGetOptionsFromUI
{
  virtual int STDCALL GetSelectionOption() const = 0;
  virtual int STDCALL GetSliderOption() const = 0;
  virtual const wchar_t* STDCALL GetTextOption() const = 0;
  virtual const int STDCALL GetTextNumericOption() const = 0;
  virtual const wchar_t* STDCALL GetTextGameSpyOption() const = 0;
};

enum EOptionsType
{
  EOT_SELECTION = 0,
  EOT_SLIDER = 1,
  EOT_TEXTENTRY = 2,
  EOT_NUMERICENTRY = 3,
  EOT_GAMESPY_TEXTENTRY = 4,
};

// other side


// for manipulate with options. 
interface IOption : IRefCount
{
  virtual void STDCALL Set(interface IUISetOptionsToUI *pSet) = 0;
  virtual void STDCALL Get(interface IUIGetOptionsFromUI *pGet) = 0;
  virtual EOptionsType STDCALL GetType() const = 0;
  virtual const char * STDCALL GetName() const = 0;
  virtual void STDCALL Apply() = 0;
  virtual void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) = 0;
  virtual bool STDCALL IsInstant() const = 0;
  virtual void STDCALL SetOptionSystem(IOptionSystem *pSystem) = 0;
};

// for getting options.
interface IOptionContainer
{
  // use tempBuffer
  virtual void GetOptions(interface IOption **ppOptions, int *pnCount) = 0;
  virtual void Apply() = 0;
};

class COption : public IOption
{
  std::string szName;
  bool bInstant;
  CPtr<IOptionSystem> pOptionSystem;

protected:
  IOptionSystem *GetOptionSystem()
  {
    if (pOptionSystem.IsValid()) return pOptionSystem;
    return GetSingleton<IOptionSystem>();
  }

  COption() {}
  COption(const char *pszName, const bool _bInstant) : szName(pszName), bInstant(_bInstant) {}

public:
  void STDCALL SetOptionSystem(IOptionSystem *pSystem) override { pOptionSystem = pSystem; }
  const char * STDCALL GetName() const override { return szName.c_str(); }
  bool STDCALL IsInstant() const override { return bInstant; }
};

// 
class COptionSelection : public COption
{
  OBJECT_COMPLETE_METHODS(COptionSelection);
  std::vector<SOptionDropListValue> selections;// list of options
  int nSelection;// current selection
public:
  COptionSelection() : nSelection(0) {}

  COptionSelection(const char *pszName, const bool _bInstant, const std::vector<SOptionDropListValue> &_selections, const char *_pszDefault)
    : COption(pszName, _bInstant), selections(_selections)
  {
    const std::string szDefault = _pszDefault;
    nSelection = selections.size() - 1;
    for (; nSelection > 0; --nSelection)
      if (selections[nSelection].szProgName == szDefault) break;

  }

  void STDCALL Set(interface IUISetOptionsToUI *pSet) override { pSet->SetSelectionOption(selections, nSelection); }

  void STDCALL Get(interface IUIGetOptionsFromUI *pGet) override { nSelection = pGet->GetSelectionOption(); }
  EOptionsType STDCALL GetType() const override { return EOT_SELECTION; }

  void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetSelection(); }

  void STDCALL Apply() override { GetOptionSystem()->Set(GetName(), selections[nSelection].szProgName.c_str()); }
};

class COptionSlider : public COption
{
  OBJECT_COMPLETE_METHODS(COptionSlider);
  int nMin, nMax, nCur;

public:
  COptionSlider() {}

  COptionSlider(const char *pszName, const bool _bInstant, const int _nMin, const int _nMax, const int _nCur)
    : COption(pszName, _bInstant), nMin(_nMin), nMax(_nMax), nCur(_nCur) {}

  void STDCALL Set(interface IUISetOptionsToUI *pSet) override { pSet->SetSliderOption(nMin, nMax, nCur); }

  void STDCALL Get(interface IUIGetOptionsFromUI *pGet) override { nCur = pGet->GetSliderOption(); }
  EOptionsType STDCALL GetType() const override { return EOT_SLIDER; }

  void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetSlider(); }

  void STDCALL Apply() override { GetOptionSystem()->Set(GetName(), variant_t(static_cast<long>(nCur))); }
};

// local player name
class COptionTextEntry : public COption
{
  OBJECT_COMPLETE_METHODS(COptionTextEntry);
  std::wstring szText;

public:
  COptionTextEntry() {}
  COptionTextEntry(const char *pszName, const bool _bInstant, const wchar_t*_pszText) : COption(pszName, _bInstant), szText(_pszText) {}

  void STDCALL Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextOption(szText.c_str()); }

  void STDCALL Get(interface IUIGetOptionsFromUI *pGet) override { szText = pGet->GetTextOption(); }
  EOptionsType STDCALL GetType() const override { return EOT_TEXTENTRY; }

  void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetTextEntry(); }

  void STDCALL Apply() override { GetOptionSystem()->Set(GetName(), szText.c_str()); }
};

class COptionTextEntryGameSpyCharacters : public COption
{
  OBJECT_COMPLETE_METHODS(COptionTextEntryGameSpyCharacters);
  std::wstring szText;

public:
  COptionTextEntryGameSpyCharacters() {}
  COptionTextEntryGameSpyCharacters(const char *pszName, const bool _bInstant, const wchar_t*_pszText) : COption(pszName, _bInstant), szText(_pszText) {}

  void STDCALL Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextGameSpyOption(szText.c_str()); }

  void STDCALL Get(interface IUIGetOptionsFromUI *pGet) override { szText = pGet->GetTextGameSpyOption(); }
  EOptionsType STDCALL GetType() const override { return EOT_GAMESPY_TEXTENTRY; }

  void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetTextGameSpyEntry(); }

  void STDCALL Apply() override { GetOptionSystem()->Set(GetName(), szText.c_str()); }
};

class COptionNumericEntry : public COption
{
  OBJECT_COMPLETE_METHODS(COptionNumericEntry);
  int nEntry;

public:
  COptionNumericEntry() {}
  COptionNumericEntry(const char *pszName, const bool _bInstant, const int _nEntry) : COption(pszName, _bInstant), nEntry(_nEntry) {}

  void STDCALL Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextNumericOption(nEntry); }

  void STDCALL Get(interface IUIGetOptionsFromUI *pGet) override { nEntry = pGet->GetTextNumericOption(); }
  EOptionsType STDCALL GetType() const override { return EOT_NUMERICENTRY; }

  void STDCALL CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetNumericEntry(); }

  void STDCALL Apply() override { GetOptionSystem()->Set(GetName(), variant_t(static_cast<long>(nEntry))); }
};
