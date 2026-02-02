#ifndef __PFX_H__
#define __PFX_H__

#pragma once
#include "Scene.h"

struct SParticleSourceInfo
{
  float fMaxCount;// peak number of particles
  float fMaxSize;// peak size
  float fAverageSize;// average particle size
  float fAverageCount;// average number of particles
};

#pragma pack( 1 )
struct SPartColor
{
  union
  {
    DWORD color;

    struct
    {
      BYTE b;
      BYTE g;
      BYTE r;
      BYTE a;
    };
  };

  operator DWORD() const { return color; }
};
#pragma pack()

struct SSimpleParticle
{
  CTRect<float> rcMaps;
  CVec3 vPosition;
  SPartColor color;
  float fSize;
  float fAngle;
};

interface IParticleSource : IRefCount
{
  // data retrieving for rendering
  virtual interface IGFXTexture * GetTexture() const = 0;
  virtual const int GetNumParticles() const = 0;
  virtual void FillParticleBuffer(SSimpleParticle *buff) const = 0;
  // position/direction
  virtual const CVec3 GetPos() const = 0;
  virtual void SetPos(const CVec3 &vPos) = 0;
  virtual const CVec3 GetDirection() const = 0;
  virtual void SetDirection(const SHMatrix &mDir) = 0;
  virtual void SetScale(float _fScale) = 0;
  // update and work with time
  virtual void Update(const NTimer::STime &time) = 0;
  virtual void SetStartTime(const NTimer::STime &time) = 0;
  virtual const NTimer::STime GetStartTime() const = 0;
  virtual const NTimer::STime GetEffectLifeTime() const = 0;
  virtual bool IsFinished() const = 0;
  virtual float GetArea() const = 0;
  virtual void Stop() = 0;
  virtual int GetOptimalUpdateTime() const = 0;
  virtual void SetSuspendedState(bool bState) = 0;
};

interface IParticleSourceWithInfo
{
  virtual void GetInfo(SParticleSourceInfo &info) = 0;
};

interface IParticleManager : ISharedManager
{
  // type ID
  enum { tidTypeID = PFX_MANAGER };

  //
  virtual IParticleSource * GetKeyBasedSource(const char *pszName) = 0;
  virtual IParticleSource * GetSmokinParticleSource(const char *pszName) = 0;
  //
  virtual void SetQuality(float fQuality) = 0;
};

#endif // __PFX_H__