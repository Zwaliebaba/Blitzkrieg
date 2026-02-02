#ifndef __UI_STATUSBAR_H__
#define __UI_STATUSBAR_H__

#include "UIButton.h"
constexpr int PROP_SIZE = 2;


class CUIStatusBar : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  using CIDVector = std::vector<int>;
  CIDVector idVectors[PROP_SIZE];
  int nVisibleWindowPropIDs[PROP_SIZE];
  DWORD dwActiveIcons;
  CVec2 vInitialIconPos;

public:
  CUIStatusBar();
  virtual void OutputString(int nControl, const wchar_t *pszString);
  virtual void OutputValue(int nControl, float fVal);
  virtual void SetUnitProperty(int nPropType, int nPropValue, const wchar_t *pszToolText);
  virtual void SetUnitIcons(DWORD dwIcons);

  // serializing...
  int operator&(IDataTree &ss) override;
};

class CUIStatusBarBridge : public IUIStatusBar, public CUIStatusBar
{
  OBJECT_NORMAL_METHODS(CUIStatusBarBridge);
  DECLARE_SUPER(CUIStatusBar);
  DEFINE_UICONTAINER_BRIDGE;
  //
  void OutputString(int nControl, const wchar_t *pszText) override { CSuper::OutputString(nControl, pszText); }
  void OutputValue(int nControl, float fVal) override { CSuper::OutputValue(nControl, fVal); }
  void SetUnitProperty(int nPropType, int nPropValue, const wchar_t *pszToolText) override { CSuper::SetUnitProperty(nPropType, nPropValue, pszToolText); }
  void SetUnitIcons(DWORD dwIcons) override { CSuper::SetUnitIcons(dwIcons); }
};

#endif		// __UI_STATUSBAR_H__