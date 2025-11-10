#ifndef __SPRITEANIMATION_H__
#define __SPRITEANIMATION_H__

class CSpriteAnimation : public ISpriteAnimation
{
  OBJECT_COMPLETE_METHODS(CSpriteAnimation);
  DECLARE_SERIALIZE;
  //
  CPtr<SSpriteAnimationFormat> pAnimations;// all animations
  const SSpriteAnimationFormat::SSpriteAnimation *pAnimation;// current animation shortcut
  int nCurrAnim;// current animation
  CScaleTimer timer;// timer...
  std::vector<float> timescales;// time scaling coefficients for each animation
  int nCurrDirection;// current direction
  float fScale;// scaling coefficient
  SSpriteRect rect;// temporal storage for return value
  int nFrameIndex;// index for static non-animated frames
  //
  bool ReAcquireAnimation()
  {
    if ((pAnimations != nullptr) && (nCurrAnim != -1))
    {
      pAnimation = pAnimations->GetAnimation(nCurrAnim);
      if (pAnimation == nullptr)
      {
        nCurrAnim = -1;
        return false;
      }
      if (pAnimation->dirs.empty())
      {
        // NI_ASSERT_SLOW_T( !pAnimation->dirs.empty(), NStr::Format("Empty animation %d set for \"%s\"", nCurrAnim, pAnimations->GetSharedResourceName()) );
        pAnimation = nullptr;
        nCurrAnim = -1;
        return false;
      }
      timer.SetScale(timescales[nCurrAnim]);
      return true;
    }
    return false;
  }

public:
  CSpriteAnimation()
    : pAnimations(nullptr), pAnimation(nullptr), nCurrAnim(-1), nCurrDirection(0), fScale(1), nFrameIndex(-1) {}

  //
  bool Init(SSpriteAnimationFormat *_pAnimations);
  //
  void STDCALL Visit(IAnimVisitor *pVisitor) override;
  // set current time
  void STDCALL SetTime(DWORD time) override { timer.Update(time); }
  // set animation start time
  void STDCALL SetStartTime(DWORD time) override { timer.Reset(time); }
  // set animation time scale coeff
  void STDCALL SetAnimSpeedCoeff(float fCoeff) override
  {
    // CRAP{ can change animation speed only for movement animations (and idle - for projectiles and so on)
    if ((nCurrAnim == ANIMATION_IDLE) || (nCurrAnim == ANIMATION_MOVE) || (nCurrAnim == ANIMATION_CRAWL))
    {
      timer.SetScale(fCoeff);
      if (nCurrAnim != -1) timescales[nCurrAnim] = fCoeff;
    }
    // CRAP}
  }

  // change animation
  bool STDCALL SetAnimation(const int nAnim) override
  {
    nCurrAnim = nAnim;
    return ReAcquireAnimation();
  }

  int STDCALL GetAnimation() const override { return nCurrAnim; };
  // get current animation length
  int STDCALL GetLengthOf(const int nAnim) override
  {
    if (pAnimations)
    {
      const SSpriteAnimationFormat::SSpriteAnimation *pTempAnim = pAnimations->GetAnimation(nAnim);
      NI_ASSERT_SLOW_TF(pTempAnim != 0, NStr::Format("Can't get animation %d for the length check for \"%s\"", nAnim, pAnimations->GetSharedResourceName()), return 1);
      return pTempAnim->GetLength();
    }
    else return 1;
  }

  // set scaling coefficient
  void STDCALL SetScale(float _fScale) override { fScale = _fScale; }
  // set animation direction
  void STDCALL SetDirection(int nDirection) override { nCurrDirection = nDirection; }
  // get current mapping rect
  const SSpriteRect & STDCALL GetRect() override
  {
    if ((pAnimation == nullptr) && !ReAcquireAnimation()) return rect;
    if (nFrameIndex == -1) rect = pAnimation->GetRect(nCurrDirection, timer.Get());
    else rect = pAnimation->GetRect(nFrameIndex);
    //
    rect.rect *= fScale;
    return rect;
  }

  // get translation speed, attached to this animation
  float STDCALL GetSpeed() const override { return pAnimation ? pAnimation->fSpeed : 1; }
  // frame index setup
  void STDCALL SetFrameIndex(int nIndex) override { nFrameIndex = nIndex; }
  int STDCALL GetFrameIndex() override { return nFrameIndex; }
  // hit test
  const bool STDCALL IsHit(const CVec3 &relpos, const CVec2 &point, CVec2 *pShift) const override;
  const bool STDCALL IsHit(const CVec3 &relpos, const CTRect<float> &rcRect) const override;
  // scale timer
  const CScaleTimer & STDCALL GetScaleTimer() const override { return timer; }
  void STDCALL SetScaleTimer(const CScaleTimer &_timer) override { timer = _timer; }
};

class CComplexSprite : public CTRefCount<ISpriteAnimation>
{
  OBJECT_SERVICE_METHODS(CComplexSprite);
  DECLARE_SERIALIZE;
  //
  CPtr<SSpritesPack> pSprites;
  CScaleTimer timer;// timer...
  int nFrameIndex;// current frame index (if it is)
  SSpriteRect spriteRect;// temp sprite rect
  //
  const SSpritesPack::SSprite &GetSprite() const { return pSprites->sprites[nFrameIndex == -1 ? 0 : nFrameIndex]; }

public:
  bool Init(SSpritesPack *_pSprites)
  {
    pSprites = _pSprites;
    nFrameIndex = 0;
    return true;
  }

  //
  void STDCALL Visit(IAnimVisitor *pVisitor) override;
  // set current time
  void STDCALL SetTime(DWORD time) override { timer.Update(time); }
  // set animation start time
  void STDCALL SetStartTime(DWORD time) override { timer.Reset(time); }
  // set animation time scale coeff
  void STDCALL SetAnimSpeedCoeff(float fCoeff) override { timer.SetScale(fCoeff); }
  // change animation
  bool STDCALL SetAnimation(const int nAnim) override { return true; }
  int STDCALL GetAnimation() const override { return 0; };
  // get current animation length
  int STDCALL GetLengthOf(const int nAnim) override { return 1; }
  // set scaling coefficient
  void STDCALL SetScale(float _fScale) override {}
  // set animation direction
  void STDCALL SetDirection(int nDirection) override {}
  // get current mapping rect
  const SSpriteRect & STDCALL GetRect() override
  {
    const int nLocalFrameIndex = nFrameIndex == -1 ? 0 : nFrameIndex;
    const CTRect<float> &rcBoundBox = pSprites->sprites[nLocalFrameIndex].GetBoundBox();
    spriteRect.rect.Set(rcBoundBox.x1, rcBoundBox.y1, rcBoundBox.x2, rcBoundBox.y2);
    spriteRect.fDepthLeft = spriteRect.fDepthRight = pSprites->sprites[nLocalFrameIndex].fMinDepth;
    return spriteRect;
  }

  // get translation speed, attached to this animation
  float STDCALL GetSpeed() const override { return 1; }
  // frame index setup
  void STDCALL SetFrameIndex(int nIndex) override { nFrameIndex = nIndex; }
  int STDCALL GetFrameIndex() override { return nFrameIndex; }
  // hit test
  const bool STDCALL IsHit(const CVec3 &relpos, const CVec2 &point, CVec2 *pShift) const override;
  const bool STDCALL IsHit(const CVec3 &relpos, const CTRect<float> &rcRect) const override;
  // scale timer
  const CScaleTimer & STDCALL GetScaleTimer() const override { return timer; }
  void STDCALL SetScaleTimer(const CScaleTimer &_timer) override { timer = _timer; }
};

#endif // __SPRITEANIMATION_H__