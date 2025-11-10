#ifndef __PARTICLESOURCEDATA_H__
#define __PARTICLESOURCEDATA_H__

#pragma once

#include "PFX.h"
#include "Track.h"

struct SExtendedParticle : SSimpleParticle
{
  NTimer::STime birthTime;// time of birth
  NTimer::STime deathTime;// time of death
  float fSpin;// angular velocity
  CVec3 vSpeed;// just speed
  CVec3 vWind;// wind for particle
  float fOpacity;// initial transparency
  STrackContext contextSpeed;// contexts for calculating integrals
  STrackContext contextZSpeed;
  STrackContext contextSpin;
};

enum
{
  PSA_TYPE_SQUARE = 0,
  PSA_TYPE_DISK = 1,
  PSA_TYPE_CIRCLE = 2
};

struct SParticleSourceData : CTRefCount<ISharedResource>
{
  OBJECT_SERVICE_METHODS(SParticleSourceData);
  DECLARE_SERIALIZE;
  SHARED_RESOURCE_METHODS(nRefData, "ParticleSource.Data");

public:
  SParticleSourceData();

  bool bComplexParticleSource;// source type, if true, complex particle source
  // generator parameters
  int nLifeTime;// lifetime of the entire source
  float fGravity;// gravity parameter (has nothing to do with physics)
  int nTextureDX;// how many frames in X (for an animated texture)
  int nTextureDY;// how many frames in Y (for an animated texture)
  std::string szTextureName;// texture name
  CVec3 vWind;// wind
  CVec3 vDirection;// source direction
  int nAreaType;// type of area for particle generation
  float fRadialWind;// radial wind force
  CTrack trackGenerateArea;// size of the area from which particles fly out
  CTrack trackDensity;// number of particles born per unit. 
  CTrack trackBeginSpeed;// initial velocity of the particle upon departure
  CTrack trackBeginSpeedRandomizer;// her randomizer
  CTrack trackBeginAngleRandomizer;// Departure angle randomizer (in radians, from 0 to PI)
  CTrack trackLife;// How long does a particle live after generation?
  CTrack trackLifeRandomizer;// previous randomizer (from 0 to 1)
  CTrack trackGenerateSpin;// initial angular velocity at departure
  CTrack trackGenerateSpinRandomizer;// randomizer of the previous one (let it be specified, but not yet used)
  CTrack trackGenerateOpacity;// initial transparency at departure (0 - 255)
  // parameters of one particle
  CTrack trackSpin;// coefficient 
  CTrack trackSpeed;// coefficient 
  CTrack trackSpeedRnd;// its randomizer (0-1)
  CTrack trackWeight;// particle mass (has nothing to do with physical mass)
  CTrack trackTextureFrame;// frame in texture [0..1]
  CTrack trackSize;// particle size (0-1)
  CTrack trackOpacity;// transparency coefficient (0-1)
  CTrack trackIntegralMass;// antiderivative of g*m(t), multiplied by the speed coefficient (without its randomness) (no need to save, calculated as you go)
  float fDensityCoeff;// coefficient for density, taken from the setting
  //
  virtual void STDCALL Init();
  virtual void STDCALL InitIntegrals();
  void STDCALL SwapData(ISharedResource *pResource) override;
  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
  //
  virtual int STDCALL operator&(IDataTree &ss);
};

#endif // __PARTICLESOURCEDATA_H__