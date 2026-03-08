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
  void STDCALL VisitSprite(const SSpriteRect *pSprite) override
  {
    pSpriteRect = pSprite;
    pComplexSprite = nullptr;
  }

  void STDCALL VisitSprite(const SSpritesPack::SSprite *pSprite) override
  {
    pComplexSprite = pSprite;
    pSpriteRect = nullptr;
  }

  void STDCALL VisitMesh(const SHMatrix *matrices, int nNumMatrices) override {}
  // UI elements visiting
  virtual void STDCALL VisitUIRects(IGFXTexture *pTexture, const int nShadingEffect, SGFXRect2 *rects, const int nNumRects) {}
  virtual void STDCALL VisitUIText(IGFXText *pText, const CTRect<float> &rcRect, const int nY, const DWORD dwColor, const DWORD dwFlags) {}
  virtual void STDCALL VisitUICustom(interface IUIElement *pElement) {}
};

#endif // __ANIMVISITOR_H__