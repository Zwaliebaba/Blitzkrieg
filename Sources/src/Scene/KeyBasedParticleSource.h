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
  interface IGFXTexture * GetTexture() const override;
  const int GetNumParticles() const override;
  void FillParticleBuffer(SSimpleParticle *buff) const override;
  // position/direction
  const CVec3 GetPos() const override;
  void SetPos(const CVec3 &vPos) override;
  const CVec3 GetDirection() const override;
  void SetScale(float _fScale) override;
  void SetDirection(const SHMatrix &mDir) override;
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
  virtual void Init(SParticleSourceData *_pData);
  int GetOptimalUpdateTime() const override;
  void SetSuspendedState(bool bState) override;
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