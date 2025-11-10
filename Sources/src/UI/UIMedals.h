#ifndef __UI_MEDALS_H__
#define __UI_MEDALS_H__

#include "UIBasic.h"
#include "UISlider.h"

class CUIMedals : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  CUIScrollBar *pScrollBar;// initialized at boot time and used to speed up access to components
  int nSpace;// text indentation to the left of the control and to the right of the scrollbar
  int nVTextSpace;// indentation of text at the top from the medal
  int nHSubSpace;// horizontal indentation of one medal from another
  int nVSubSpace;// vertical indentation of one medal from another
  int nNextPosX, nNextPosY;// next medal position
  int nMedalsCount;// number of medals
  std::vector<CTRect<float>> medalMaps;// to save texture coordinates of medals

public:
  CUIMedals() : nSpace(4), nVTextSpace(5), nHSubSpace(10), nVSubSpace(10),
                nNextPosX(0), nNextPosY(0), nMedalsCount(0) {}

  ~CUIMedals() override {}

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  void STDCALL ShowWindow(int _nCmdShow) override;

  void ClearMedals();
  void AddMedal(IGFXTexture *pTexture, const CTRect<float> &mapImageRect, const WORD *pszMedalsName);
  void UpdateMedals();

private:
  void ComputeHPositions();// calculated once when adding medals, then scrolling only vertically
  void UpdatePositions();// called when the ScrollBar position is updated
  void UpdateScrollbar();// called after all objectives have been added to update the ScrollBar
  void InitMaps();// to initialize texture coordinates
};

class CUIMedalsBridge : public IUIContainer, public CUIMedals
{
  OBJECT_NORMAL_METHODS(CUIMedalsBridge);
  DECLARE_SUPER(CUIMedals);
  DEFINE_UICONTAINER_BRIDGE;
};

#endif		// __UI_MEDALS_H__