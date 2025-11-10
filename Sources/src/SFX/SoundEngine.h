#ifndef __SOUNDENGINE_H__
#define __SOUNDENGINE_H__

#include "SampleSounds.h"
#include "StreamFadeOff.h"

using CSoundChannelMap = std::hash_map<ISound *, int, SDefaultPtrHash>;
using CChannelSoundMap = std::hash_map<int, CPtr<ISound>>;

class CSoundEngine : public ISFX
{
  OBJECT_NORMAL_METHODS(CSoundEngine);
  DECLARE_SERIALIZE;

  //
  struct SDriverInfo
  {
    std::string szDriverName;
    bool isHardware3DAccelerated;// this driver supports hardware accelerated 3d sound.
    bool supportEAXReverb;// this driver supports EAX reverb
    bool supportA3DOcclusions;// this driver supports (A3D) geometry occlusions
    bool supportA3DReflections;// this driver supports (A3D) geometry reflections
    bool supportReverb;// this driver supports EAX2/A3D3 reverb
  };

  struct SMelodyInfo
  {
    DECLARE_SERIALIZE;

  public:
    std::string szName;
    bool bLooped;
    void Clear() { szName.clear(); }
    bool IsValid() const { return !szName.empty(); }
    //
  };

  // initialization info - drivers
  using CDriversInfo = std::vector<SDriverInfo>;
  CDriversInfo drivers;// [0] is default driver
  //
  NTimer::STime timeLastUpdate;
  //
  // streams
  SMelodyInfo curMelody;
  SMelodyInfo nextMelody;// to fade melodies
  FSOUND_STREAM *pStreamingSound;// current streaming sound
  int nStreamingChannel;// channel of this streaming sound
  NTimer::STime timeStreamFinished;// time, last stream finished
  //
  // channels management
  CSoundChannelMap channelsMap;// sound => channel map
  CChannelSoundMap soundsMap;// channel => sound map
  //
  float fListenerDistance;// listener distance from anchor
  CVec3 vLastListenerPos;
  bool bInited;
  bool bEnableSFX;// enable SFXes playing
  bool bEnableStreaming;// enable streaming playing
  bool bSoundCardPresent;
  bool bPaused;// is all SFX sounds paused?
  bool bStreamingPaused;// is streaming sound paused
  //
  BYTE cSFXMasterVolume;// SFXes volume
  BYTE cStreamMasterVolume;// streams volume
  float fStreamCurrentVolume;// for fade streams ( 0.0f ... 1.0f )
  bool bStreamPlaying;

  CStreamFadeOff streamFadeOff;
  //
  void ClearChannels();
  //
  bool SearchDevices();
  // streaming
  void CloseStreaming();
  //
  void ReEnableSounds();
  //
  CSoundEngine();
  virtual ~CSoundEngine() { Done(); }

  void UpdateCameraPos(const CVec3 &vPos);

public:
  // internal-use service functions
  bool PlayNextMelody();
  void NotifyMelodyFinished();
  void MapSound(ISound *pSound, int nChannel);
  //
  BYTE STDCALL GetSFXMasterVolume() const override { return cSFXMasterVolume; }
  virtual BYTE STDCALL GetStreamMasterVolume() const { return cStreamMasterVolume; }
  //
  IRefCount * STDCALL QI(int nInterfaceTypeID) override;
  // init and close sound system
  bool STDCALL IsInitialized() override;
  bool STDCALL Init(HWND hWnd, int nDriver, ESFXOutputType output, int nMixRate, int nMaxChannels) override;
  void STDCALL Done() override;
  //
  // enable SFXes and streaming
  void STDCALL EnableSFX(bool bEnable) override
  {
    bEnableSFX = bEnable;
    ReEnableSounds();
  }

  void STDCALL EnableStreaming(bool bEnable) override
  {
    bEnableStreaming = bEnable;
    ReEnableSounds();
  }

  bool STDCALL IsSFXEnabled() const override { return bEnableSFX && bSoundCardPresent; }
  bool STDCALL IsStreamingEnabled() const override { return bEnableStreaming && bSoundCardPresent; }
  //
  // setup
  void STDCALL SetDistanceFactor(float fFactor) override;
  void STDCALL SetRolloffFactor(float fFactor) override;
  // set SFX master volume. 
  void STDCALL SetSFXMasterVolume(float fVolume) override
  {
    Clamp(fVolume, 0.0f, 1.0f);
    cSFXMasterVolume = static_cast<BYTE>(fVolume * 255.0f);
  }

  // set streams master volume. 
  void STDCALL SetStreamMasterVolume(float fVolume) override;
  //
  // streaming sound
  void STDCALL PlayStream(const char *pszFileName, bool bLooped = false, unsigned int nTimeToFadePrevious = 0) override;
  void STDCALL StopStream(unsigned int nTimeToFade = 0) override;
  bool STDCALL IsStreamPlaying() const override;
  void STDCALL SetStreamVolume(float fVolume) override;
  float STDCALL GetStreamVolume() const override;

  //
  // sample sounds
  int STDCALL PlaySample(ISound *pSound, bool bLooped = false, unsigned int nStartPos = 0) override;
  void STDCALL StopSample(ISound *pSound) override;
  void STDCALL UpdateSample(ISound *pSound) override;
  void STDCALL StopChannel(int nChannel) override;

  // Update sounds ( that is needed for 3D sounds )
  void STDCALL Update(interface ICamera *pCamera) override;
  //
  bool STDCALL Pause(bool bPause) override;
  bool STDCALL PauseStreaming(bool bPause) override;
  bool STDCALL IsPaused() override;
  bool STDCALL IsPlaying(ISound *pSound) override;

  unsigned int STDCALL GetCurrentPosition(ISound *pSound) override;
  void STDCALL SetCurrentPosition(ISound *pSound, unsigned int pos) override;

};

#endif // __SOUNDENGINE_H__