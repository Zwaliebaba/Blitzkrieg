#ifndef __MATERIALEFFECTOR_H__
#define __MATERIALEFFECTOR_H__

#pragma once

class CMaterialEffector : ISceneMaterialEffector
{
  OBJECT_NORMAL_METHODS(CMaterialEffector);
  DECLARE_SERIALIZE;
  NTimer::STime nStartTime;
  NTimer::STime nDuration;
  BYTE bAlpha;
  DWORD dwSpecular;
  float fCoeff;

public:
  CMaterialEffector() : nStartTime(0), nDuration(1), bAlpha(0xFF), dwSpecular(0xFF000000), fCoeff(0.0f) {}
  bool STDCALL Update(const NTimer::STime &time) override;
  void STDCALL SetupTimes(const NTimer::STime &timeStart, const NTimer::STime &timeLife) override;
  BYTE STDCALL GetAlpha() const override;
  DWORD STDCALL GetSpecular() const override;
  void STDCALL SetupData(BYTE bMaxAlpha, DWORD dwMaxSpecular) override;
};

#endif // __MATERIALEFFECTOR_H__