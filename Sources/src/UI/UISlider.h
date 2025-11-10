#ifndef __UI_SLIDER_H__
#define __UI_SLIDER_H__


#include "UIButton.h"

class CUISlider : public CSimpleWindow
{
  DECLARE_SERIALIZE;
  //
  int m_nMin, m_nMax, m_nStep;
  int m_nPrevPos, m_nPos;
  int m_nKeyStep;// keyboard step
  int m_nElevatorWidth;// elevator size along the ruler
  int m_nLineWidth;// ruler width
  bool bVertical;// if true then vertical slider
  bool bSelElevator;// is the elevator allocated?

  CPtr<IGFXTexture> pSliderTexture;// appearance - texture
  CTRect<float> sliderMapa;

  // Before drawing, using this function, I calculate where to draw the elevator
  int ComputeElevatorCoord();
  // to move the elevator in response to mouse movement
  void UpdatePosition(int nCoord);

  // sending a message to the top about a change in the current position
  void NotifyPositionChanged();

public:
  CUISlider() : m_nMin(0), m_nMax(0), m_nStep(0), m_nPrevPos(-13), m_nPos(0), m_nKeyStep(20),
                m_nElevatorWidth(0), m_nLineWidth(0), bVertical(false), bSelElevator(false) {}

  ~CUISlider() override {}

  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  // cursor and actions
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override { return true; }

  // for¤ ScrollBar
  // here a change in position does not send a message to the top.
  // these functions should not be used externally, they are used only in the ScrollBar
  void SetPosition(int nPos);
  int GetPosition() { return m_nPos; }
  int GetMinValue() { return m_nMin; }
  int GetMaxValue() { return m_nMax; }

  void IncPosition(int nStep)
  {
    m_nPos += nStep;
    if (m_nPos > m_nMax) m_nPos = m_nMax;
  }

  void DecPosition(int nStep)
  {
    m_nPos -= nStep;
    if (m_nPos < m_nMin) m_nPos = m_nMin;
  }

  bool IsVertical() { return bVertical; }

  void SetMinValue(int nMin);
  void SetMaxValue(int nMax);
  void SetStep(int nStep) { m_nStep = nStep; }
};

class CUISliderBridge : public IUISlider, public CUISlider
{
  OBJECT_NORMAL_METHODS(CUISliderBridge);

public:
  DECLARE_SUPER(CUISlider);
  DEFINE_UIELEMENT_BRIDGE;
  void STDCALL SetMinValue(int nVal) override { CSuper::SetMinValue(nVal); }
  void STDCALL SetMaxValue(int nVal) override { CSuper::SetMaxValue(nVal); }
  void STDCALL SetStep(int nVal) override { CSuper::SetStep(nVal); }
  void STDCALL SetPosition(int nPos) override { CSuper::SetPosition(nPos); }
  int STDCALL GetPosition() override { return CSuper::GetPosition(); }
};

// The cat behaves like MultipleWindow in terms of message processing (it just passes them to childs)
// But in a different way Serialize does not save the list of childs, left, right buttons and the elevator is stored separately
class CUIScrollBar : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  //
  CUIButton *pMinButton;// initialized at boot time and used to speed up access to components
  CUIButton *pMaxButton;
  CUISlider *pSlider;
  int m_nButtonStep;// shift when pressing a button
  DWORD dwLastUpdateTime;

  // sending a message to the top about a change in the current position
  void NotifyPositionChanged();
  bool IsVertical() { return pSlider->IsVertical(); }

public:
  CUIScrollBar() : pMinButton(nullptr), pMaxButton(nullptr), pSlider(nullptr), m_nButtonStep(1), dwLastUpdateTime(0) {}
  ~CUIScrollBar() override {}

  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  /* //state
   */

  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
  bool STDCALL Update(const NTimer::STime &currTime) override;

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // for¤ internal use¤
  void SetPosition(int nPos) { pSlider->SetPosition(nPos); }
  int GetPosition() { return pSlider->GetPosition(); }
  int GetMinValue() { return pSlider->GetMinValue(); }
  int GetMaxValue() { return pSlider->GetMaxValue(); }

  void SetMinValue(int nMin) { pSlider->SetMinValue(nMin); }
  void SetMaxValue(int nMax) { pSlider->SetMaxValue(nMax); }
  void SetStep(int nStep) { pSlider->SetStep(nStep); }
  void SetButtonStep(int nVal) { m_nButtonStep = nVal; }
};


class CUIScrollBarBridge : public IUIScrollBar, public CUIScrollBar
{
  OBJECT_NORMAL_METHODS(CUIScrollBarBridge);
  DECLARE_SUPER(CUIScrollBar);
  DEFINE_UICONTAINER_BRIDGE;
  void STDCALL SetMinValue(int nVal) override { CSuper::SetMinValue(nVal); }
  void STDCALL SetMaxValue(int nVal) override { CSuper::SetMaxValue(nVal); }
  void STDCALL SetStep(int nVal) override { CSuper::SetStep(nVal); }
  void STDCALL SetButtonStep(int nVal) override { CSuper::SetButtonStep(nVal); }
  void STDCALL SetPosition(int nPos) override { CSuper::SetPosition(nPos); }
  int STDCALL GetPosition() override { return CSuper::GetPosition(); }
};

#endif		// __UI_SLIDER_H__