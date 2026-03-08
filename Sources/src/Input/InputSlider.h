#ifndef __INPUTSLIDER_H__
#define __INPUTSLIDER_H__

#pragma once

class CInputBinder;

class CInputSlider : public CTRefCount<IInputSlider>
{
  OBJECT_SERVICE_METHODS(CInputSlider);
  //
  CPtr<CInputBinder> pInput;// hook to input to get last input time
  struct SCommand *pCommand;// hook to command to unregister
  float fLastValue;// last sampled value
  DWORD dwLastTime;// last sample time
  float fCoeff;// coeff
public:
  CInputSlider() : pCommand(nullptr) {}
  CInputSlider(CInputBinder *_pInput, SCommand *_pCommand, float _fCoeff);
  ~CInputSlider() override;
  //
  float STDCALL GetDelta() override;
  // virtual float STDCALL GetSpeed() const;
  void STDCALL Reset() override;
};

#endif // __INPUTSLIDER_H__