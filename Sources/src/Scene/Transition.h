#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#pragma once

class CTransition : public CTRefCount<ITransition>
{
  OBJECT_SERVICE_METHODS(CTransition);
  DECLARE_SERIALIZE;
  //
  NTimer::STime timeStart;// start time
  float fAlphaStart;// start alpha value
  float fAlphaEnd;// end alpha value
  float fAlpha;// current alpha value
  bool bInfinite;// infinite updates
public:
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override;
  // drawing
  bool STDCALL Draw(interface IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
  //
  int STDCALL Start(const char *pszVideoName, DWORD dwAddFlags, const NTimer::STime &currTime, bool bFadeIn) override;
};

#endif // __TRANSITION_H__