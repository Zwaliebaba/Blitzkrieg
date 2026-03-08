#ifndef __MATRIXEFFECTOR_H__
#define __MATRIXEFFECTOR_H__

#pragma once

class CMatrixEffectorRecoil : public ISceneEffectorRecoil
{
  OBJECT_NORMAL_METHODS(CMatrixEffectorRecoil);
  DECLARE_SERIALIZE;
  //
  SHMatrix matResult;// result matrix
  NTimer::STime timeStart;// effect start time
  NTimer::STime timeLife;// effect lifetime
  NTimer::STime lastUpdateTime;// last update time
  // rotation parameters
  CVec3 vAxis;
  float fAngle;

public:
  CMatrixEffectorRecoil() : matResult(MONE), lastUpdateTime(0), vAxis(V3_AXIS_X), fAngle(0) {}
  //
  void STDCALL SetupTimes(const NTimer::STime &_timeStart, const NTimer::STime &_timeLife) override
  {
    timeStart = _timeStart;
    timeLife = _timeLife;
  }

  void STDCALL SetupData(float _fAngle, const CVec3 &_vAxis) override
  {
    fAngle = _fAngle;
    vAxis = _vAxis;
  }

  bool STDCALL Update(const NTimer::STime &time) override;
  const SHMatrix & STDCALL GetMatrix() const override { return matResult; }
};

class CMatrixEffectorJogging : public ISceneEffectorJogging
{
  OBJECT_NORMAL_METHODS(CMatrixEffectorJogging);
  DECLARE_SERIALIZE;
  //
  SHMatrix matResult;// result matrix
  NTimer::STime timeStart;// effect start time
  NTimer::STime lastUpdateTime;// last update time
  // jogging params
  float fWeightCoeff;// weight of the techniques for jogging
public:
  CMatrixEffectorJogging() : matResult(MONE), lastUpdateTime(0), fWeightCoeff(1.0f) {}
  //
  void STDCALL SetupTimes(const NTimer::STime &_timeStart, const NTimer::STime &_timeLife) override { timeStart = _timeStart; }

  void STDCALL SetupData(float _fWeightCoeff) override { fWeightCoeff = _fWeightCoeff; }

  bool STDCALL Update(const NTimer::STime &time) override;
  const SHMatrix & STDCALL GetMatrix() const override { return matResult; }
};

#endif // __MATRIXEFFECTOR_H__