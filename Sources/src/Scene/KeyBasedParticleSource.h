#ifndef __KEYBASEDPARTICLESOURCE_H__
#define __KEYBASEDPARTICLESOURCE_H__

#pragma once

#include "PFX.h"
#include "ParticleSourceData.h"

class CKeyBasedParticleSource : public IParticleSource, public IParticleSourceWithInfo
{
  OBJECT_NORMAL_METHODS(CKeyBasedParticleSource);
  DECLARE_SERIALIZE;
  //
  CPtr<SParticleSourceData> pData;// source information
  NTimer::STime nStartTime;// time of birth
  NTimer::STime nLastUpdateTime;// time¤ last update¤
  NTimer::STime nLastParticleUpdate;// time of last update of generated particles
  CVec3 vPosition;// relative to source position
  float fDirectionPhi;// source direction in spherical coordinate system
  float fDirectionTheta;// source direction in spherical coordinate system
  CVec3 vDirection;// source direction excluding pData
  CPtr<IGFXTexture> pTexture;// texture with particles
  float lastError;// correction for a non-integer number of particles at the last generation
  std::vector<CTRect<float>> rcRects;// texture frame coordinates
  std::list<SExtendedParticle> particles;// They
  float fScale;// scale of effect
  bool bStopped;// stop the effect
  bool bSuspended;
  STrackContext contextDensity;// context for integrating particle density
  using GetParticlePositionFunction = CVec3(float area, const CVec3 &vPosition);
  GetParticlePositionFunction *pfnGPPfunc;// pointer to a function that determines the nature of the particle emission region
public:
  // data retrieving for rendering
  interface IGFXTexture * STDCALL GetTexture() const override;
  const int STDCALL GetNumParticles() const override;
  void STDCALL FillParticleBuffer(SSimpleParticle *buff) const override;
  // position/direction
  const CVec3 STDCALL GetPos() const override;
  void STDCALL SetPos(const CVec3 &vPos) override;
  const CVec3 STDCALL GetDirection() const override;
  void STDCALL SetScale(float _fScale) override;
  void STDCALL SetDirection(const SHMatrix &mDir) override;
  // update and work with time
  void STDCALL Update(const NTimer::STime &time) override;
  void STDCALL SetStartTime(const NTimer::STime &time) override;
  const NTimer::STime STDCALL GetStartTime() const override;
  const NTimer::STime STDCALL GetEffectLifeTime() const override;
  bool STDCALL IsFinished() const override;
  // statistics
  void STDCALL GetInfo(SParticleSourceInfo &info) override;
  float STDCALL GetArea() const override;
  void STDCALL Stop() override;
  //
  virtual void Init(SParticleSourceData *_pData);
  int STDCALL GetOptimalUpdateTime() const override;
  void STDCALL SetSuspendedState(bool bState) override;
};

class CParticleGenerator
{
  static float fStartAngle;
  static float nCurrParticle;
  static float fStep;

public:
  static CVec3 GetParticlePositionSquare(float area, const CVec3 &vPosition);
  static CVec3 GetParticlePositionDisk(float area, const CVec3 &vPosition);
  static CVec3 GetParticlePositionCircle(float area, const CVec3 &vPosition);
  static void ResetGenerator(int nNextNumParticles);
};

#endif // __KEYBASEDPARTICLESOURCE_H__