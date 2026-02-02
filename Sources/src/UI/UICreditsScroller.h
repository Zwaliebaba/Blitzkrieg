#ifndef __UICREDITSSCROLLER_H__
#define __UICREDITSSCROLLER_H__

#pragma once

#include "UIBasic.h"


class CUICreditsScroller : public CSimpleWindow
{
  NTimer::STime nLastUpdate;
  bool bWorking;
  int nCurrOffset;
  int nMaxOffset;

public:
  CUICreditsScroller();
  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;
  bool Update(const NTimer::STime &currTime) override;
  void ShowWindow(int _nCmdShow) override;
};

class CUICreditsScrollerBridge : public IUICreditsScroller, public CUICreditsScroller
{
  OBJECT_NORMAL_METHODS(CUICreditsScrollerBridge);
  DECLARE_SUPER(CUICreditsScroller);
  DEFINE_UIELEMENT_BRIDGE;
};

#endif // __UICREDITSSCROLLER_H__