#ifndef __SAMPLESOUNDS_H__
#define __SAMPLESOUNDS_H__

// ************************************************************************************************************************ //
// **
// ** base shared sound sample resource
// **
// **
// **
// ************************************************************************************************************************ //

class CSoundSample : public ISharedResource
{
  OBJECT_NORMAL_METHODS(CSoundSample);
  SHARED_RESOURCE_METHODS(nRefData.a, "Sound");
  //
  FSOUND_SAMPLE *sample;// FMOD sound sample
  int nMode;// FMOD sound sample mode
  bool bLooped;// is this sample looped ?
  float fMinDistance;// minimal distance
  //
  void Close()
  {
    if (sample) FSOUND_Sample_Free(sample);
    sample = 0;
  }

public:
  CSoundSample() : sample(nullptr), nMode(0), fMinDistance(45) {}
  ~CSoundSample() { Close(); }
  //
  void SetSample(FSOUND_SAMPLE *_sample)
  {
    Close();
    sample = _sample;
    if (sample) FSOUND_Sample_SetMinMaxDistance(sample, fMinDistance, 1000000000.0f);
  }

  int GetMode() const { return nMode; }
  bool IsLooped() const { return bLooped; }

  FSOUND_SAMPLE *GetInternalContainer()
  {
    Load();
    return sample;
  }

  //
  void Set3D(bool b3D) { nMode = b3D ? FSOUND_HW3D : FSOUND_2D; }
  void SetLoop(bool bEnable);

  void SetMinDistance(float _fMinDistance)
  {
    fMinDistance = _fMinDistance;
    if (sample) FSOUND_Sample_SetMinMaxDistance(sample, fMinDistance, 1000000000.0f);
  }

  //
  void STDCALL SwapData(ISharedResource *pResource) override;
  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
};

// ************************************************************************************************************************ //
// **
// ** other sounds
// **
// **
// **
// ************************************************************************************************************************ //

class CBaseSound : public ISound
{
  DECLARE_SERIALIZE;
  //
  CPtr<CSoundSample> pSample;
  int nChannel;

public:
  CBaseSound() : nChannel(-1) {}
  virtual ~CBaseSound() {}
  //
  void SetSample(CSoundSample *_pSample) { pSample = _pSample; }
  CSoundSample *GetSample() { return pSample; }
  int GetChannel() const { return nChannel; }
  void SetChannel(int _nChannel) { nChannel = _nChannel; }
  //
  bool IsPlaying()
  {
    if ((nChannel != -1) && FSOUND_IsPlaying(nChannel)) return FSOUND_GetCurrentSample(nChannel) == pSample->GetInternalContainer();
    return false;
  }

  // distance
  void STDCALL SetMinDistance(float fDistance) override { pSample->SetMinDistance(fDistance); }
  // looping
  void STDCALL SetLooping(bool bEnable, int nStart = -1, int nEnd = -1) override
  {
    FSOUND_Sample_SetLoopMode(pSample->GetInternalContainer(), bEnable ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
    if ((nStart != -1) && (nEnd != -1)) FSOUND_Sample_SetLoopPoints(pSample->GetInternalContainer(), nStart, nEnd);
  }

  unsigned int STDCALL GetLenght() override { return FSOUND_Sample_GetLength(pSample->GetInternalContainer()); }

  unsigned int STDCALL GetSampleRate() override
  {
    int freq = 44000;
    FSOUND_Sample_GetDefaults(pSample->GetInternalContainer(), &freq, 0, 0, 0);
    return freq;
  }

  void STDCALL SetVolume(float nVolume) override {}
  float STDCALL GetVolume() const override { return 1.0f; }
  void STDCALL SetPan(float nPan) override {}
  float STDCALL GetPan() const override { return 0.0f; }
};

class CSound2D : public CBaseSound
{
  OBJECT_NORMAL_METHODS(CSound2D);
  DECLARE_SERIALIZE;
  //
  float fVolume;
  float fPan;

public:
  CSound2D() : fVolume(1.0f), fPan(0.0f) {}
  ~CSound2D() override {}
  // visiting
  int STDCALL Visit(interface ISFXVisitor *pVisitor) override;
  //
  int STDCALL Play()
  {
    int nChannel = FSOUND_PlaySound(FSOUND_FREE, GetSample()->GetInternalContainer());
    SetChannel(nChannel);
    return nChannel;
  }

  void STDCALL SetPosition(const CVec3 &vPos3) override {}
  const CVec3 STDCALL GetPosition() override { return VNULL3; }

  void STDCALL SetVolume(float _fVolume) override { fVolume = _fVolume; }
  float STDCALL GetVolume() const override { return fVolume; }
  void STDCALL SetPan(float _fPan) override { fPan = _fPan; }
  float STDCALL GetPan() const override { return fPan; }

};

class CSound3D : public CBaseSound
{
  OBJECT_NORMAL_METHODS(CSound3D);
  DECLARE_SERIALIZE;
  //
  CVec3 vPos;// current position
  bool bDopplerFlag;
  NTimer::STime lastUpdateTime;
  CVec3 vLastPos;

public:
  CSound3D() : bDopplerFlag(false), lastUpdateTime(0), vLastPos(VNULL3) {}
  ~CSound3D() override {}
  // visiting
  int STDCALL Visit(interface ISFXVisitor *pVisitor) override;
  //
  int STDCALL Play();
  void STDCALL SetDopplerFlag(bool bDoppler) { bDopplerFlag = bDoppler; }
  void STDCALL SetPosition(const CVec3 &vPos3) override;
  const CVec3 STDCALL GetPosition() override { return vPos; }
};

#endif // __SAMPLESOUNDS_H__