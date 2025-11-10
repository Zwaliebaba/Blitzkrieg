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
  virtual void STDCALL OutputString(int nControl, const WORD *pszString);
  virtual void STDCALL OutputValue(int nControl, float fVal);
  virtual void STDCALL SetUnitProperty(int nPropType, int nPropValue, const WORD *pszToolText);
  virtual void STDCALL SetUnitIcons(DWORD dwIcons);

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
};

class CUIStatusBarBridge : public IUIStatusBar, public CUIStatusBar
{
  OBJECT_NORMAL_METHODS(CUIStatusBarBridge);
  DECLARE_SUPER(CUIStatusBar);
  DEFINE_UICONTAINER_BRIDGE;
  //
  void STDCALL OutputString(int nControl, const WORD *pszText) override { CSuper::OutputString(nControl, pszText); }
  void STDCALL OutputValue(int nControl, float fVal) override { CSuper::OutputValue(nControl, fVal); }
  void STDCALL SetUnitProperty(int nPropType, int nPropValue, const WORD *pszToolText) override { CSuper::SetUnitProperty(nPropType, nPropValue, pszToolText); }
  void STDCALL SetUnitIcons(DWORD dwIcons) override { CSuper::SetUnitIcons(dwIcons); }
};

#endif		// __UI_STATUSBAR_H__