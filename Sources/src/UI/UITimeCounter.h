#ifndef __UI_TIME_COUNTER_H__
#define __UI_TIME_COUNTER_H__

#include "UIBasic.h"


class CUITimeCounter : public CSimpleWindow
{
  DECLARE_SERIALIZE;
  //
  DWORD dwBeginTime, dwRingTime;
  bool bNeedAnimate;// Do I need to calculate animation?
  float fBegin, fEnd, fCurrent;// animation position
  bool bVertical;// vertical or horizontal animation
  DWORD dwCounterColor, dwBGColor, dwDisabledCounterColor;// counter color

  const DWORD GetCounterColor();

public:
  CUITimeCounter() : dwBeginTime(0), dwRingTime(0), bNeedAnimate(false),
                     fBegin(0), fEnd(0), fCurrent(0), bVertical(true), dwCounterColor(0xffffffff), dwBGColor(0xff000000), dwDisabledCounterColor(0xffffffff) {}

  ~CUITimeCounter() override {}

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  bool STDCALL Update(const NTimer::STime &currTime) override;
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
};

class CUITimeCounterBridge : public IUITimeCounter, public CUITimeCounter
{
  OBJECT_NORMAL_METHODS(CUITimeCounterBridge);

public:
  DECLARE_SUPER(CUITimeCounter);
  DEFINE_UIELEMENT_BRIDGE;
};


#endif		// __UI_TIME_COUNTER_H__