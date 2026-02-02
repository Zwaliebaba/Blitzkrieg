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
  interface IGFXTexture * GetTexture() const override;
  const int GetNumParticles() const override;
  void FillParticleBuffer(SSimpleParticle *buff) const override;
  // position/direction
  const CVec3 GetPos() const override;
  void SetPos(const CVec3 &vPos) override;
  const CVec3 GetDirection() const override;
  void SetDirection(const SHMatrix &mDir) override;
  void SetScale(float _fScale) override;
  // update and work with time
  void Update(const NTimer::STime &time) override;
  void SetStartTime(const NTimer::STime &time) override;
  const NTimer::STime GetStartTime() const override;
  const NTimer::STime GetEffectLifeTime() const override;
  bool IsFinished() const override;
  // statistics
  void GetInfo(SParticleSourceInfo &info) override;
  float GetArea() const override;
  void Stop() override;
  //
  virtual void Init(SSmokinParticleSourceData *_pData);
  int GetOptimalUpdateTime() const override;
  void SetSuspendedState(bool bState) override;
};

#endif