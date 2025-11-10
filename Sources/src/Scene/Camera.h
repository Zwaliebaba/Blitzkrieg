#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma once

#include "../Input/Input.h"

class CCamera : public ICamera
{
  OBJECT_COMPLETE_METHODS(CCamera);
  DECLARE_SERIALIZE;

  //
  struct SEarthQuake
  {
    float fAmplitude;
    float fAttenuation;
    float fDuration;
    float fTime;
    //
    SEarthQuake() {}

    SEarthQuake(float _fAmplitude, float _fAttenuation, float _fDuration)
      : fAmplitude(_fAmplitude), fAttenuation(_fAttenuation), fDuration(_fDuration), fTime(0) {}
  };

  //
  CVec3 vAnchor;// anchor point on the terrain's surface
  CVec3 vAnchor1;// what you need to focus on
  float fRod;// distance from the anchor to the camera
  float fPitch, fYaw;// yaw and pitch of the camera
  CVec3 vPos;// temporary storage for position
  CVec2 vScrollSpeed;// additional scrolling speed
  CTRect<float> rcBounds;// bounds rect
  //
  float fEQAttenuation;// earthquake wave attenuation
  float fEQPeriod;// earthquake wave period
  NTimer::STime timeEQDuration;// earthquake duration
  std::list<SEarthQuake> earthquakes;// earthquakes
  // sliders
  CPtr<ITimeSlider> pTimeSlider;
  CPtr<IInputSlider> pFwd, pStrafe, pZoom;
  // last unchanged position
  CVec3 vLastAnchor;// last position
  NTimer::STime timeLast;// last time of the position above
public:
  CCamera();
  //
  void STDCALL Init(ISingleton *pSingleton) override;
  void STDCALL SetBounds(int x1, int y1, int x2, int y2) override { rcBounds.Set(x1, y1, x2, y2); }
  // placement functions
  void STDCALL SetPlacement(const CVec3 &vAnchor, float fDist, float fPitch, float fYaw) override;
  void STDCALL SetAnchor(const CVec3 &_vAnchor) override { vAnchor = _vAnchor; }
  const SHMatrix STDCALL GetPlacement() const override;
  const CVec3 STDCALL GetPos() const override { return vPos; }
  const CVec3 STDCALL GetAnchor() override { return vAnchor1; }
  // last unchanged position
  void STDCALL GetLastPos(CVec3 *pvAnchor, NTimer::STime *pTime) const override
  {
    *pvAnchor = vLastAnchor;
    *pTime = timeLast;
  }

  // scrolling
  void STDCALL ResetSliders() override;
  void STDCALL SetScrollSpeedX(float fSpeed) override { vScrollSpeed.x = fSpeed; }
  void STDCALL SetScrollSpeedY(float fSpeed) override { vScrollSpeed.y = fSpeed; }
  // eqrthquake
  void STDCALL AddEarthquake(const CVec3 &vPos, float fPower) override;
  //
  void STDCALL Update() override;
};

#endif // __CAMERA_H__