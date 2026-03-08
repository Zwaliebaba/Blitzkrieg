#ifndef __SMOKIN_PARTICLESOURCEDATA_H__
#define __SMIKIN_PARTICLESOURCEDATA_H__

#pragma once

#include "Track.h"

struct SSmokinParticleSourceData : CTRefCount<ISharedResource>
{
  OBJECT_SERVICE_METHODS(SSmokinParticleSourceData);
  SHARED_RESOURCE_METHODS(nRefData, "ComplexParticleSource.Data");

public:
  SSmokinParticleSourceData();

  bool bComplexParticleSource;// source type, if true, complex particle source
  // generator parameters
  int nLifeTime;// lifetime of the entire source
  float fGravity;// gravity parameter (has nothing to do with physics)
  CVec3 vWind;// wind
  CVec3 vDirection;// source direction
  CTrack trackGenerateArea;// size of the area from which particles fly out
  CTrack trackDensity;// number of particles born per unit. 
  CTrack trackBeginSpeed;// initial velocity of the particle upon departure
  CTrack trackBeginSpeedRandomizer;// her randomizer
  CTrack trackBeginAngleRandomizer;// Departure angle randomizer
  // parameters of one particle
  CTrack trackSpeed;// coefficient 
  CTrack trackSpeedRnd;// coefficient randomizer 
  CTrack trackWeight;// particle mass (has nothing to do with physical mass)
  int nAreaType;// type of area for particle generation
  float fRadialWind;// radial wind force
  std::string szParticleEffectName;// name of the scatter effect
  CTrack trackIntegralMass;// antiderivative of g*m(t), multiplied by the speed coefficient (without its randomness)
  float fDensityCoeff;// coefficient for density, taken from the setting
  int nUpdateStep;// number of milliseconds between updates
  //

  void STDCALL SwapData(ISharedResource *pResource) override;
  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
  //
  int STDCALL operator&(IStructureSaver &ss) override;
  virtual int STDCALL operator&(IDataTree &ss);
  virtual void STDCALL InitIntegrals();
};

#endif