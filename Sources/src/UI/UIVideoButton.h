#ifndef __UIVIDEO_BUTTON_H__
#define __UIVIDEO_BUTTON_H__

#include "UIBasic.h"
#include "../Scene/Scene.h"


class CUIVideoButton : public CSimpleWindow
{
  DECLARE_SERIALIZE;
  std::string szBinkFile;
  CPtr<IVideoPlayer> pVideoPlayer;

  void InitVideoPlayer();
  void Play();

public:
  CUIVideoButton() {}

  // serializing...
  int operator&(IDataTree &ss) override;

  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;

  void Reposition(const CTRect<float> &rcParent) override;
  bool Update(const NTimer::STime &currTime) override;

  virtual int GetCurrentFrame();
  virtual bool SetCurrentFrame(int nFrame);
};

class CUIVideoButtonBridge : public IUIVideoButton, public CUIVideoButton
{
  OBJECT_NORMAL_METHODS(CUIVideoButtonBridge);
  DECLARE_SUPER(CUIVideoButton);
  DEFINE_UIELEMENT_BRIDGE;

  int GetCurrentFrame() override { return CSuper::GetCurrentFrame(); }
  bool SetCurrentFrame(int nFrame) override { return CSuper::SetCurrentFrame(nFrame); }
};


#endif // __UIVIDEO_BUTTON_H__