#include "StdAfx.h"

#include "InputSlider.h"

#include "InputBinder.h"

CInputSlider::CInputSlider(CInputBinder *_pInput, SCommand *_pCommand, float _fCoeff)
  : pInput(_pInput), pCommand(_pCommand), fCoeff(_fCoeff)
{
  pCommand->Register(true);
  dwLastTime = pInput->GetCurrentTime();
  fLastValue = pCommand->GetAccValue(dwLastTime);
}

void CInputSlider::Reset()
{
  dwLastTime = pInput->GetCurrentTime();
  fLastValue = pCommand->GetAccValue(dwLastTime);
}

CInputSlider::~CInputSlider() { if (pCommand) pCommand->Register(false); }

float CInputSlider::GetDelta()
{
  dwLastTime = pInput->GetCurrentTime();
  const float fValue = pCommand->GetAccValue(dwLastTime);
  const float fDelta = static_cast<float>(fValue - fLastValue);
  fLastValue = fValue;
  return fDelta * fCoeff * 0.001f;
}

/* float CInputSlider::GetSpeed() const
 */