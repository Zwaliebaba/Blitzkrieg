#pragma once

#include "../StreamIO/OptionSystem.h"

// ui side

interface IUISetOptionsToUI
{
  // use tempBuffer
  virtual void SetSelectionOption(const std::vector<SOptionDropListValue> &szSelections, int nDefault) = 0;
  virtual void SetSliderOption(int nMin, int nMax, int nDefault) = 0;
  virtual void SetTextOption(const wchar_t*pszEntry) = 0;
  virtual void SetTextGameSpyOption(const wchar_t*pszEntry) = 0;
  virtual void SetTextNumericOption(int nEnntry) = 0;

  virtual void ResetSelection() = 0;
  virtual void ResetSlider() = 0;
  virtual void ResetTextEntry() = 0;
  virtual void ResetNumericEntry() = 0;
  virtual void ResetTextGameSpyEntry() = 0;
};

interface IUIGetOptionsFromUI
{
  virtual int GetSelectionOption() const = 0;
  virtual int GetSliderOption() const = 0;
  virtual const wchar_t* GetTextOption() const = 0;
  virtual const int GetTextNumericOption() const = 0;
  virtual const wchar_t* GetTextGameSpyOption() const = 0;
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
  virtual void Set(interface IUISetOptionsToUI *pSet) = 0;
  virtual void Get(interface IUIGetOptionsFromUI *pGet) = 0;
  virtual EOptionsType GetType() const = 0;
  virtual const char * GetName() const = 0;
  virtual void Apply() = 0;
  virtual void CancelChanges(interface IUISetOptionsToUI *pSet) = 0;
  virtual bool IsInstant() const = 0;
  virtual void SetOptionSystem(IOptionSystem *pSystem) = 0;
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
  void SetOptionSystem(IOptionSystem *pSystem) override { pOptionSystem = pSystem; }
  const char * GetName() const override { return szName.c_str(); }
  bool IsInstant() const override { return bInstant; }
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

  void Set(interface IUISetOptionsToUI *pSet) override { pSet->SetSelectionOption(selections, nSelection); }

  void Get(interface IUIGetOptionsFromUI *pGet) override { nSelection = pGet->GetSelectionOption(); }
  EOptionsType GetType() const override { return EOT_SELECTION; }

  void CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetSelection(); }

  void Apply() override { GetOptionSystem()->Set(GetName(), selections[nSelection].szProgName.c_str()); }
};

class COptionSlider : public COption
{
  OBJECT_COMPLETE_METHODS(COptionSlider);
  int nMin, nMax, nCur;

public:
  COptionSlider() {}

  COptionSlider(const char *pszName, const bool _bInstant, const int _nMin, const int _nMax, const int _nCur)
    : COption(pszName, _bInstant), nMin(_nMin), nMax(_nMax), nCur(_nCur) {}

  void Set(interface IUISetOptionsToUI *pSet) override { pSet->SetSliderOption(nMin, nMax, nCur); }

  void Get(interface IUIGetOptionsFromUI *pGet) override { nCur = pGet->GetSliderOption(); }
  EOptionsType GetType() const override { return EOT_SLIDER; }

  void CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetSlider(); }

  void Apply() override { GetOptionSystem()->Set(GetName(), variant_t(static_cast<long>(nCur))); }
};

// local player name
class COptionTextEntry : public COption
{
  OBJECT_COMPLETE_METHODS(COptionTextEntry);
  std::wstring szText;

public:
  COptionTextEntry() {}
  COptionTextEntry(const char *pszName, const bool _bInstant, const wchar_t*_pszText) : COption(pszName, _bInstant), szText(_pszText) {}

  void Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextOption(szText.c_str()); }

  void Get(interface IUIGetOptionsFromUI *pGet) override { szText = pGet->GetTextOption(); }
  EOptionsType GetType() const override { return EOT_TEXTENTRY; }

  void CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetTextEntry(); }

  void Apply() override { GetOptionSystem()->Set(GetName(), szText.c_str()); }
};

class COptionTextEntryGameSpyCharacters : public COption
{
  OBJECT_COMPLETE_METHODS(COptionTextEntryGameSpyCharacters);
  std::wstring szText;

public:
  COptionTextEntryGameSpyCharacters() {}
  COptionTextEntryGameSpyCharacters(const char *pszName, const bool _bInstant, const wchar_t*_pszText) : COption(pszName, _bInstant), szText(_pszText) {}

  void Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextGameSpyOption(szText.c_str()); }

  void Get(interface IUIGetOptionsFromUI *pGet) override { szText = pGet->GetTextGameSpyOption(); }
  EOptionsType GetType() const override { return EOT_GAMESPY_TEXTENTRY; }

  void CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetTextGameSpyEntry(); }

  void Apply() override { GetOptionSystem()->Set(GetName(), szText.c_str()); }
};

class COptionNumericEntry : public COption
{
  OBJECT_COMPLETE_METHODS(COptionNumericEntry);
  int nEntry;

public:
  COptionNumericEntry() {}
  COptionNumericEntry(const char *pszName, const bool _bInstant, const int _nEntry) : COption(pszName, _bInstant), nEntry(_nEntry) {}

  void Set(interface IUISetOptionsToUI *pSet) override { pSet->SetTextNumericOption(nEntry); }

  void Get(interface IUIGetOptionsFromUI *pGet) override { nEntry = pGet->GetTextNumericOption(); }
  EOptionsType GetType() const override { return EOT_NUMERICENTRY; }

  void CancelChanges(interface IUISetOptionsToUI *pSet) override { pSet->ResetNumericEntry(); }

  void Apply() override { GetOptionSystem()->Set(GetName(), variant_t(static_cast<long>(nEntry))); }
};
