#ifndef __UIINTERNAL_H__
#define __UIINTERNAL_H__

#include "../Main/TextSystem.h"
#include "../sfx/sfx.h"
#include "MaskSystem.h"

enum EUIWindowSubState
{
  E_NORMAL_STATE = 0,
  E_HIGHLIGHTED_STATE = 1,
  E_PUSHED_STATE = 2,// Do not change these IDs!
  E_DISABLED_STATE = 3,

  UI_ESS_FORCE_DWORD = 0x7fffffff
};

// the element state consists of 3 substates: normal, highlighted, suppressed and disabled
// in each of these substates you can change the entire appearance of the element
// as long as the element is considered rectangular.
// in principle, nothing prevents you from having an element of arbitrary geometry, but then checking to hit it will be more difficult
struct SWindowSubRect
{
  CTRect<float> rc;
  CTRect<float> mapa;
};

struct CUIWindowSubState
{
private:
  DECLARE_SERIALIZE;
  CObj<IManipulator> pManipulator;

public:
  std::vector<SWindowSubRect> subRects;

  CPtr<IGFXTexture> pTexture;// appearance - texture
  CPtr<IUIMask> pMask;
  DWORD color;
  DWORD specular;
  DWORD textColor;

  CUIWindowSubState() : color(0xffffffff), specular(0xff000000), textColor(0) {}
  int operator&(IDataTree &ss);

  IManipulator *GetManipulator();

  void CopyInternals(CUIWindowSubState *pSS) const;

private:
  /* struct SWindowTileRect
     */

  void SaveTextureAndSubRects(CTreeAccessor *pFile);
  void LoadTextureAndSubRects(CTreeAccessor *pFile);
  void LoadTileRects(CTreeAccessor *pFile);
};

struct CWindowState
{
private:
  DECLARE_SERIALIZE;
  CObj<IManipulator> pManipulator;

  void InitDependentInfo();

public:
  std::string szPushSound;// sound when you left-click on the control
  std::string szClickSound;// sound when clicking with the mouse in the control area (clicked and pressed inside)

  CUIWindowSubState subStates[4];// normal, highlighted, pinned and disabled
  std::string szKey;// key for the text system, using this key you can get the text in the window
  CPtr<IGFXText> pGfxText;// to display text

  std::string szToolKey;// key for loading a tooltip from text resources
  CPtr<IText> pToolText;// text displayed as a tooltip

  int operator&(IDataTree &ss);
  CWindowState() {}
  IManipulator *GetManipulator();

  // duplicate
  void CopyInternals(CWindowState *pS) const;
};

void LoadTileRectangles(CTreeAccessor *pFile, std::vector<SWindowSubRect> &subRects, DTChunkID sName, IGFXTexture *pTexture);
void SaveTextureAndMap(CTreeAccessor *pFile, IGFXTexture *pTexture, DTChunkID tName, const CTRect<float> &maps, DTChunkID mName);
void LoadTextureAndMap(CTreeAccessor *pFile, CPtr<IGFXTexture> *ppTexture, DTChunkID tName, CTRect<float> *pMaps, DTChunkID mName);
void SaveSound(CTreeAccessor *pFile, ISound *pSound, DTChunkID sName);
void LoadSound(CTreeAccessor *pFile, CPtr<ISound> *ppSound, DTChunkID sName);


#endif // __UIINTERNAL_H__