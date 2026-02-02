#ifndef __EFFECTVISOBJ_H__
#define __EFFECTVISOBJ_H__

#pragma once

#include "../Formats/fmtEffect.h"

template<class TYPE>
struct SEffect
{
  DWORD dwStart;// start time (with respect to parent effect start time)
  DWORD dwEnd;// end time (with respect to parent effect start time)
  bool bActive;// is effect active&
  CVec3 vPos;// position (with respect to parent effect position)
  CVec3 vRelPos;
  CPtr<TYPE> pObj;// effect object
  //
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &dwStart);
    saver.Add(2, &dwEnd);
    saver.Add(3, &vPos);
    saver.Add(4, &vRelPos);
    saver.Add(5, &pObj);
    return 0;
  }
};

using SSpriteEffect = SEffect<ISpriteVisObj>;
using SParticleEffect = SEffect<IParticleSource>;

class CEffectVisObj : public IEffectVisObj
{
  OBJECT_NORMAL_METHODS(CEffectVisObj);
  DECLARE_SERIALIZE;
  // subeffects
  std::vector<SSpriteEffect> sprites;// sprite subeffects
  std::vector<SParticleEffect> particles;// particle subeffects
  std::string szSoundName;// sound, attacked to this effect
  //
  DWORD dwStartTime;// effect start time
  DWORD dwDuration;// total effect duration
  //
  CVec3 vPos;// world position
  int nDirection;// direction...
  EVisObjSelectionState selectionState;// selection state
  //
  bool bStopped;// is effect stopped?
  bool bSuspended;

public:
  CEffectVisObj() : dwStartTime(0), dwDuration(0), bStopped(false), bSuspended(false) {}
  CEffectVisObj(const std::string &_szSoundName) : szSoundName(_szSoundName), dwStartTime(0), dwDuration(0), bStopped(false), bSuspended(false) {}
  // update state
  bool Update(const NTimer::STime &time, bool bForced = false) override;
  // placement
  void SetDirection(int nDir) override;
  void SetPosition(const CVec3 &pos) override { vPos = pos; }

  void SetPlacement(const CVec3 &pos, const int nDir) override
  {
    SetPosition(pos);
    SetDirection(nDir);
  }

  const CVec3 & GetPosition() const override { return vPos; }
  int GetDirection() const override { return 0; }
  // selection / selection test
  void Select(EVisObjSelectionState state) override { selectionState = state; }
  EVisObjSelectionState GetSelectionState() const override { return selectionState; }
  bool IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) override { return false; }
  bool IsHit(const SHMatrix &matTransform, const RECT &rect) override { return false; }
  // opacity
  void SetOpacity(BYTE opacity) override
  {
    for (auto it = sprites.begin(); it != sprites.end(); ++it) it->pObj->SetOpacity(opacity);
    /* for ( std::vector<SParticleEffect>::iterator it = particles.begin(); it != particles.end(); ++it )
       */
  }

  void SetColor(DWORD color) override {}
  void SetSpecular(DWORD color) override {}
  // scale
  void SetScale(float fScale) override;
  // drawing
  bool Draw(IGFX *pGFX) override;
  // visiting
  void Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  // data retrieving.
  const std::string & GetSoundEffect() const override { return szSoundName; }
  // NOTE: this function uses temp buffer 0
  void GetSpriteEffects(const SSpriteInfo ***ppEffects, int *pnNumEffects, bool bAll) override;
  void GetParticleEffects(IParticleSource ***ppEffects, int *pnNumEffects, bool bAll) override;
  //
  void SetStartTime(DWORD time) override { dwStartTime = time; }
  void SetEffectDirection(const SHMatrix &matrix) override;
  bool IsFinished(const NTimer::STime &time) override;
  void CalibrateDuration(const NTimer::STime &timeDuration) override;
  void Stop() override;
  void SetSuspendedState(bool bState) override;
  //
  void AddSpriteEffect(ISpriteVisObj *pObj, DWORD dwStart, int nRepeat, const CVec3 &vPos);
  void AddParticleEffect(IParticleSource *pObj, DWORD dwStart, int nDuration, const CVec3 &vPos);
};

#endif // __EFFECTVISOBJ_H__