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
    virtual int operator&(IDataTree &ss);
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
  void Draw(interface IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;
  // serializing...
  int operator&(IDataTree &ss) override;
  //
  virtual void SetValue(float fVal);
  virtual void ClearColors() { valueColors.clear(); }
  virtual void SetColor(float fVal, DWORD dwColor);
};

class CUINumberIndicatorBridge : public IUINumberIndicator, public CUINumberIndicator
{
  OBJECT_NORMAL_METHODS(CUINumberIndicatorBridge);
  DECLARE_SUPER(CUINumberIndicator);
  DEFINE_UIELEMENT_BRIDGE;
  void SetValue(float fVal) override { CSuper::SetValue(fVal); }
  void ClearColors() override { CSuper::ClearColors(); }
  void SetColor(float fVal, DWORD dwColor) override { CSuper::SetColor(fVal, dwColor); }
};

#endif // __UI_NUMBER_INDICATOR_H__