#ifndef __SMOKINPARTICLESOURCE_H__
#define __SMOKINPARTICLESOURCE_H__

#pragma once

#include "PFX.h"
#include "SmokinParticleSourceData.h"
#include "KeyBasedParticleSource.h"

struct SExtendedParticleSource
{
  CPtr<IParticleSource> pSource;// what flies
  CVec3 vSpeed;// just speed
  CVec3 vWind;// wind for particles
  STrackContext contextSpeed;// contexts for calculating integrals
  STrackContext contextZSpeed;// 
  //
  int operator&(IStructureSaver &ss);
};

class CSmokinParticleSource : public IParticleSource, public IParticleSourceWithInfo
{
  OBJECT_NORMAL_METHODS(CSmokinParticleSource);
  DECLARE_SERIALIZE;
  CPtr<SSmokinParticleSourceData> pData;// source information
  NTimer::STime nStartTime;// time of birth
  NTimer::STime nLastUpdateTime;// time¤ last update¤
  CVec3 vPosition;// relative to source position
  float fDirectionPhi;// source direction in spherical coordinate system
  float fDirectionTheta;// source direction in spherical coordinate system
  CVec3 vDirection;// source direction excluding pData
  float lastError;// correction for a non-integer number of particles at the last generation
  std::list<SExtendedParticleSource> particles;// They
  float fScale;// scale of effect
  bool bStopped;// stop the effect
  STrackContext contextDensity;// context for integrating particle density
  using GetParticlePositionFunction = CVec3(float area, const CVec3 &vPosition);
  GetParticlePositionFunction *pfnGPPfunc;

public:
  // data retrieving for rendering
  interface IGFXTexture * STDCALL GetTexture() const override;
  const int STDCALL GetNumParticles() const override;
  void STDCALL FillParticleBuffer(SSimpleParticle *buff) const override;
  // position/direction
  const CVec3 STDCALL GetPos() const override;
  void STDCALL SetPos(const CVec3 &vPos) override;
  const CVec3 STDCALL GetDirection() const override;
  void STDCALL SetDirection(const SHMatrix &mDir) override;
  void STDCALL SetScale(float _fScale) override;
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
  virtual void Init(SSmokinParticleSourceData *_pData);
  int STDCALL GetOptimalUpdateTime() const override;
  void STDCALL SetSuspendedState(bool bState) override;
};

#endif