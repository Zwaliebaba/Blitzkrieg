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
  int STDCALL operator&(IDataTree &ss) override;

  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  bool STDCALL Update(const NTimer::STime &currTime) override;

  virtual int STDCALL GetCurrentFrame();
  virtual bool STDCALL SetCurrentFrame(int nFrame);
};

class CUIVideoButtonBridge : public IUIVideoButton, public CUIVideoButton
{
  OBJECT_NORMAL_METHODS(CUIVideoButtonBridge);
  DECLARE_SUPER(CUIVideoButton);
  DEFINE_UIELEMENT_BRIDGE;

  int STDCALL GetCurrentFrame() override { return CSuper::GetCurrentFrame(); }
  bool STDCALL SetCurrentFrame(int nFrame) override { return CSuper::SetCurrentFrame(nFrame); }
};


#endif // __UIVIDEO_BUTTON_H__