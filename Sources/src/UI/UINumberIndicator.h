#ifndef __UI_NUMBER_INDICATOR_H__
#define __UI_NUMBER_INDICATOR_H__

#include "UIBasic.h"

class CUINumberIndicator : public CSimpleWindow
{
  DECLARE_SERIALIZE;

  struct SValueColor
  {
    DECLARE_SERIALIZE;

  public:
    virtual int STDCALL operator&(IDataTree &ss);
    bool operator <(const SValueColor &v) const { return fVal < v.fVal; }
    float fVal;
    DWORD dwColor;
  };

  std::vector<SValueColor> valueColors;
  float m_fVal;

  void SortValues();

public:
  CUINumberIndicator() : m_fVal(0.0f) {}
  ~CUINumberIndicator() override {}

  //
  void STDCALL Draw(interface IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  //
  virtual void STDCALL SetValue(float fVal);
  virtual void STDCALL ClearColors() { valueColors.clear(); }
  virtual void STDCALL SetColor(float fVal, DWORD dwColor);
};

class CUINumberIndicatorBridge : public IUINumberIndicator, public CUINumberIndicator
{
  OBJECT_NORMAL_METHODS(CUINumberIndicatorBridge);
  DECLARE_SUPER(CUINumberIndicator);
  DEFINE_UIELEMENT_BRIDGE;
  void STDCALL SetValue(float fVal) override { CSuper::SetValue(fVal); }
  void STDCALL ClearColors() override { CSuper::ClearColors(); }
  void STDCALL SetColor(float fVal, DWORD dwColor) override { CSuper::SetColor(fVal, dwColor); }
};

#endif // __UI_NUMBER_INDICATOR_H__