#ifndef __FMTEFFECT_H__
#define __FMTEFFECT_H__

#pragma once

struct SSpriteEffectDesc
{
  std::string szPath;// effect path name
  int nStart;// start time (relative to parent effect) in msec.
  int nRepeat;// repeat counter
  CVec3 vPos;// relative position

  int operator&(IDataTree &ss);
};

struct SParticleEffectDesc
{
  std::string szPath;// effect path name
  int nStart;// effect start time
  int nDuration;// effect duration
  CVec3 vPos;// relative position
  float fScale;// לאסרעאב

  int operator&(IDataTree &ss);
};

struct SSmokinParticleEffectDesc
{
  std::string szPath;// effect path name
  int nStart;// effect start time
  int nDuration;// effect duration
  CVec3 vPos;// relative position
  float fScale;// לאסרעאב

  int operator&(IDataTree &ss);
};

struct SEffectDesc
{
  using CSpritesList = std::vector<SSpriteEffectDesc>;
  using CParticlesList = std::vector<SParticleEffectDesc>;
  using CSmokinParticlesList = std::vector<SSmokinParticleEffectDesc>;
  CSpritesList sprites;// sprite effects
  CParticlesList particles;// particle effects
  CSmokinParticlesList smokinParticles;// smokin particle effects
  std::string szSound;// sound effect

  int operator&(IDataTree &ss);
};

#endif // __FMTEFFECT_H__