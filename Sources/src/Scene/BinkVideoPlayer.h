#ifndef __BINKVIDEOPLAYER_H__
#define __BINKVIDEOPLAYER_H__

#pragma once

#include <bink.h>

struct SImagePart
{
  CPtr<IGFXTexture> pTexture;// real vid-mem texture
  CTRect<long> rcSrcRect;// source rect to blit uncompressed bink from
  CTRect<long> rcDstRect;// destination rect to blit uncompressed bink data to (from source rect)
  CTRect<float> rcMaps;// mapping coords
  CTRect<float> rcRect;// drawing rect
  //
  SImagePart() : rcSrcRect(0, 0, 0, 0), rcDstRect(0, 0, 0, 0), rcMaps(0, 0, 0, 0), rcRect(0, 0, 0, 0) {}
};

using CImagesList = std::vector<SImagePart>;

class CBinkVideoPlayer : public CTRefCount<IVideoPlayer>
{
  OBJECT_SERVICE_METHODS(CBinkVideoPlayer);
  DECLARE_SERIALIZE;
  //
  CImagesList images;// all subimages
  CTRect<float> rcDstRect;// destination rect to render to
  bool bMaintainAspect;// do we need to maintain aspect ratio?
  DWORD dwCopyFlags;// copy flags
  DWORD dwPlayFlags;// play flags
  HBINK hBink;// bink main handler
  bool bLooped;// is looped movie?
  int nLastPlayedFrame;// last frame, which was played
  bool bStopped;// bink playing stopped, but handle was not closed
  // shading effects
  int nShadingEffectStart;// before draw
  int nShadingEffectFinish;// after draw
  //
  std::vector<char> buffer;// buffer to play bink from memory
  std::string szFileName;// bink file name (to restore)
  //
  bool OpenBink(const char *pszFileName, DWORD dwOpenFlags, DWORD dwFlags);
  // do one frame
  bool DoOneFrame(bool bCheckForStop = true);
  void CopyRects();
  void SetupRects();

public:
  CBinkVideoPlayer();
  ~CBinkVideoPlayer() override;
  // setup target to render video to. 
  void STDCALL SetTarget(interface IGFXTexture *pTexture, IGFX *pGFX) override;
  // set destination rect to render to
  void STDCALL SetDstRect(const RECT &_rcDstRect, bool bMaintainAspect) override;
  // set loop mode
  void STDCALL SetLoopMode(bool _bLooped) override { bLooped = _bLooped; }
  // playing position
  int STDCALL GetCurrentFrame() const override;
  bool STDCALL SetCurrentFrame(int nFrame) override;
  // shading effect
  void SetShadingEffect(const int nEffect, bool bStart) override
  {
    if (bStart) nShadingEffectStart = nEffect;
    else nShadingEffectFinish = nEffect;
  }

  // frame update
  bool STDCALL Update(const NTimer::STime &time, bool bForcedUpdate) override;
  // playing capabilities
  int STDCALL Play(const char *pszFileName, DWORD dwFlags, IGFX *pGFX, interface ISFX *pSFX) override;
  bool STDCALL Stop() override;
  bool STDCALL Pause(bool bPause) override;
  bool STDCALL IsPlaying() const override;
  // movie statistics:
  int STDCALL GetLength() const override;
  int STDCALL GetNumFrames() const override;
  bool STDCALL GetMovieSize(CVec2 *pSize) const override;
  // drawing
  bool STDCALL Draw(interface IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
};

#endif // __BINKVIDEOPLAYER_H__