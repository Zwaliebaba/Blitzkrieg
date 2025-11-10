#ifndef __FRAMESELECTION_H__
#define __FRAMESELECTION_H__

#pragma once

class CFrameSelection : public IFrameSelection
{
  OBJECT_MINIMAL_METHODS(CFrameSelection);
  //
  CVec3 vBegin;
  CVec3 vEnd;
  bool bActive;

public:
  CFrameSelection() : vBegin(VNULL3), vEnd(VNULL3), bActive(false) {}
  //
  void STDCALL Begin(const CVec3 &point) override
  {
    vBegin = vEnd = point;
    bActive = true;
  }

  void STDCALL End() override { bActive = false; }
  void STDCALL Update(const CVec3 &point) override { if (bActive) vEnd = point; }

  void STDCALL Reset() override
  {
    vBegin = vEnd = VNULL3;
    bActive = false;
  }

  CVec3 STDCALL GetBeginPoint() override { return vBegin; }
  CVec3 STDCALL GetEndPoint() override { return vEnd; }
  bool STDCALL IsActive() override { return bActive; }
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return true; }
  //
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
};

#endif // __FRAMESELECTION_H__