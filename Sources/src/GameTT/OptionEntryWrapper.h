#ifndef __OPTIONENTRYWRAPPER_H__
#define __OPTIONENTRYWRAPPER_H__

#pragma once
#include "InterMission.h"
#include "iMission.h"
#include "UIOptions.h"

// UI representation of options
class CUIOption : public IUISetOptionsToUI, public IUIGetOptionsFromUI, public IRefCount
{
  OBJECT_COMPLETE_METHODS(CUIOption);
  //
  CPtr<IUIDialog> pDialog;
  CPtr<IOption> pOption;
  CPtr<IUIStatic> pOptionName;
  CPtr<IUIDialog> pSubDialog;

  // for selection
  std::vector<SOptionDropListValue> szSelections;
  int nCurSelection;
  int nStartingSelection;

  // initial parameters ( for canceling options changes )
  int nInitialSliderPos;// for remember slider position
  int nInitialSelection;// for remember initial selection
  std::wstring szInitialText;// initial text entry value
  int nInitialNumericEntry;// initial numeric entry value

  void SetSlider(int nInitialSliderPos);
  // store current value to option.
  void SaveOption();
  void ChangeSelection(int nCurSelection);

public:
  CUIOption() {}
  // init texts and dialog according to current option value and type
  CUIOption(IUIStatic *_pOptionName, IUIDialog *_pDialog, IOption *_pOption);

  // saves options an set current values ​​as initial;
  void Apply();

  // restores option as it was during creation.
  void CancelChanges();

  void OnClicked(bool bLeft);
  void OnSelected();
  void PositionChanged();
  bool IsOptionValid() const { return pOption.IsValid(); }

  // interface IUISetOptionsToUI
  // use tempBuffer
  void SetSelectionOption(const std::vector<SOptionDropListValue> &szSelections, int nDefault) override;
  void SetSliderOption(int nMin, int nMax, int nDefault) override;
  void SetTextOption(const wchar_t *pszEntry) override;
  void SetTextNumericOption(int nEntry) override;
  void SetTextGameSpyOption(const wchar_t *pszEntry) override;

  void ResetSelection() override;
  void ResetSlider() override;
  void ResetTextEntry() override;
  void ResetTextGameSpyEntry() override;
  void ResetNumericEntry() override;

  // interface IUIGetOptionsFromUI
  int GetSelectionOption() const override;
  int GetSliderOption() const override;
  const wchar_t * GetTextOption() const override;
  const wchar_t * GetTextGameSpyOption() const override;
  const int GetTextNumericOption() const override;
};

using OptionDescs = std::list<SOptionDesc>;

class COptionsListWrapper : public IRefCount
{
  OBJECT_COMPLETE_METHODS(COptionsListWrapper);

  std::vector<CPtr<CUIOption>> options;
  OptionDescs optionsDescs;
  CPtr<IUIListControl> pList;
  int nIDToStartFrom;// the ids of options windows will start from this value.
  DWORD dwFlags;

  CPtr<IOptionSystem> pSetOptionSystem;// where to set options (custom option system)
  bool bDisableChange;

  void InitList(bool bDefault);

public:
  COptionsListWrapper() {}
  COptionsListWrapper(DWORD _dwFlags, IUIListControl *_pList, int _nIDToStartFrom, IOptionSystem *pSystem = nullptr, bool bDisableChange = false);
  COptionsListWrapper(IUIListControl *_pList, OptionDescs &optionDescs, int _nIDToStartFrom, IOptionSystem *pSystem = nullptr, bool bDisableChange = false);
  virtual bool ProcessMessage(const SGameMessage &msg);

  void Apply();
  void ToDefault();
  void CancelChanges();
  void ShowWindow(int nCmdShow) { pList->ShowWindow(nCmdShow); }
  void DisableChange();
  bool IsChangesEnabled() const { return bDisableChange; }
};

#endif // __OPTIONENTRYWRAPPER_H__