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
  void Begin(const CVec3 &point) override
  {
    vBegin = vEnd = point;
    bActive = true;
  }

  void End() override { bActive = false; }
  void Update(const CVec3 &point) override { if (bActive) vEnd = point; }

  void Reset() override
  {
    vBegin = vEnd = VNULL3;
    bActive = false;
  }

  CVec3 GetBeginPoint() override { return vBegin; }
  CVec3 GetEndPoint() override { return vEnd; }
  bool IsActive() override { return bActive; }
  // update object
  bool Update(const NTimer::STime &time, bool bForced = false) override { return true; }
  //
  bool Draw(IGFX *pGFX) override;
  // visiting
  void Visit(ISceneVisitor *pVisitor, int nType = -1) override;
};

#endif // __FRAMESELECTION_H__