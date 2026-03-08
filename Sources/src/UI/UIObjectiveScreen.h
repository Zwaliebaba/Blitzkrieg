#ifndef __UI_OBJECTIVE_SCREEN_H__
#define __UI_OBJECTIVE_SCREEN_H__

#include "UIBasic.h"
#include "UIDialog.h"

class CUIObjectiveScreen : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  IUIShortcutBar *pSB;// initialized at boot time and used to speed up access to components
  bool bShowAllObjectives;

public:
  CUIObjectiveScreen() : pSB(nullptr), bShowAllObjectives(false) {}
  ~CUIObjectiveScreen() override {}

  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override { return pSB->OnMouseWheel(vPos, mouseState, fDelta); }
  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  void STDCALL ShowWindow(int _nCmdShow) override;
};

class CUIObjectiveScreenBridge : public IUIContainer, public CUIObjectiveScreen
{
  OBJECT_NORMAL_METHODS(CUIObjectiveScreenBridge);
  DECLARE_SUPER(CUIObjectiveScreen);
  DEFINE_UICONTAINER_BRIDGE;
};

#endif		// __UI_OBJECTIVE_SCREEN_H__