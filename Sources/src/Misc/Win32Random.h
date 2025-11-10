#ifndef __WIN32RANDOM_H__
#define __WIN32RANDOM_H__

#pragma once

namespace NWin32Random
{
  void Seed(int nSeed);
  unsigned int Random();
  __forceinline unsigned int Random(const unsigned int uMax) { return Random() % uMax; }
  __forceinline int Random(const int nMin, const int nMax) { return nMin + static_cast<int>(NWin32Random::Random((const unsigned int) (nMax - nMin + 1))); }
  __forceinline float Random(const float fMin, const float fMax) { return fMin + static_cast<float>(float(NWin32Random::Random()) / float(RAND_MAX) * (fMax - fMin)); }

  __forceinline unsigned int RandomCheck(const unsigned int uMax) { return uMax == 0 ? 0 : Random(uMax); }
  __forceinline int RandomCheck(const int nMin, const int nMax) { return nMax < nMin ? nMin : Random(nMin, nMax); }
  __forceinline float RandomCheck(const float fMin, const float fMax) { return fMax < fMin ? fMin : Random(fMin, fMax); }
};

#endif // __WIN32RANDOM_H__