#ifndef __ANIMVISITOR_H__
#define __ANIMVISITOR_H__

#pragma once

#include "../Anim/Animation.h"

class CExtractAnimVisitor : public CTRefCount<IAnimVisitor>
{
  OBJECT_SERVICE_METHODS(CExtractAnimVisitor);
  //
  const SSpriteRect *pSpriteRect;
  const SSpritesPack::SSprite *pComplexSprite;

public:
  CExtractAnimVisitor() { Clear(); }

  void Clear()
  {
    pSpriteRect = nullptr;
    pComplexSprite = nullptr;
  }

  //
  const SSpriteRect *GetSpriteRect() const { return pSpriteRect; }
  const SSpritesPack::SSprite *GetComplexSprite() const { return pComplexSprite; }
  //
  void VisitSprite(const SSpriteRect *pSprite) override
  {
    pSpriteRect = pSprite;
    pComplexSprite = nullptr;
  }

  void VisitSprite(const SSpritesPack::SSprite *pSprite) override
  {
    pComplexSprite = pSprite;
    pSpriteRect = nullptr;
  }

  void VisitMesh(const SHMatrix *matrices, int nNumMatrices) override {}
  // UI elements visiting
  virtual void VisitUIRects(IGFXTexture *pTexture, const int nShadingEffect, SGFXRect2 *rects, const int nNumRects) {}
  virtual void VisitUIText(IGFXText *pText, const CTRect<float> &rcRect, const int nY, const DWORD dwColor, const DWORD dwFlags) {}
  virtual void VisitUICustom(interface IUIElement *pElement) {}
};

#endif // __ANIMVISITOR_H__