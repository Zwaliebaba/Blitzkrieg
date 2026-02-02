#ifndef __SFX_H__
#define __SFX_H__

#pragma once

enum
{
  SFX_BASE_VALUE = 0x10090000,
  SFX_SFX = SFX_BASE_VALUE + 1,
  SFX_SAMPLE = SFX_BASE_VALUE + 2,
  SFX_SOUND_MANAGER = SFX_BASE_VALUE + 3,
  SFX_SOUND_2D = SFX_BASE_VALUE + 4,
  SFX_SOUND_3D = SFX_BASE_VALUE + 5,
  SFX_PLAY_LIST = SFX_BASE_VALUE + 6
};

enum ESFXOutputType
{
  SFX_OUTPUT_NO,
  SFX_OUTPUT_WINMM,
  SFX_OUTPUT_DSOUND,
  SFX_OUTPUT_A3D
};

interface ISound : IRefCount
{
  // visiting
  virtual int Visit(interface ISFXVisitor *pVisitor) = 0;
  // position
  virtual void SetPosition(const CVec3 &vPos3) = 0;
  virtual const CVec3 GetPosition() = 0;
  // min distance (in world points /* see. fWorldCellSize */)
  virtual void SetMinDistance(float fDistance) = 0;
  // looping
  virtual void SetLooping(bool bEnable, int nStart = -1, int nEnd = -1) = 0;

  // 0.0f...1.0f
  virtual void SetVolume(float nVolume) =0;
  virtual float GetVolume() const =0;

  // -1.0f...1.0f
  virtual void SetPan(float nPan) =0;
  virtual float GetPan() const =0;

  // sound duration in samples
  virtual unsigned int GetLenght() =0;
  virtual unsigned int GetSampleRate() =0;

};

interface IPlayList : IRefCount
{
  enum { ORDER_SEQUENTIAL, ORDER_RANDOM, ORDER_CYCLE };

  // clear all melodies
  virtual void Clear() = 0;
  // sequence order
  virtual void SetSequenceOrder(int nOrder) = 0;
  // add new melody to the end of the play list
  virtual void AddMelody(const char *pszFileName) = 0;
  // get next melody for playing
  virtual const char * GetNextMelody() = 0;
};

interface ISoundManager : ISharedManager
{
  // type ID
  enum { tidTypeID = SFX_SOUND_MANAGER };

  //
  virtual ISound * GetSound2D(const char *pszName) = 0;
  virtual ISound * GetSound3D(const char *pszName) = 0;
  //
  virtual const char * GetSoundName(ISound *pSound) = 0;
};

interface ISFX : IRefCount
{
  enum { tidTypeID = SFX_SFX };

  //
  IRefCount * QI(int nInterfaceTypeID) override = 0;
  // Init and close sound system
  virtual bool IsInitialized() = 0;
  virtual bool Init(HWND hWnd, int nDriver, ESFXOutputType output, int nMixRate, int nMaxChannels) = 0;
  virtual void Done() = 0;
  //
  // enable SFXes and streaming
  virtual void EnableSFX(bool bEnable) = 0;
  virtual void EnableStreaming(bool bEnable) = 0;
  virtual bool IsSFXEnabled() const =0;
  virtual bool IsStreamingEnabled() const =0;
  //
  // setup
  virtual void SetDistanceFactor(float fFactor) = 0;
  virtual void SetRolloffFactor(float fFactor) = 0;
  // set SFX master volume. 
  virtual void SetSFXMasterVolume(float fVolume) = 0;
  virtual BYTE GetSFXMasterVolume() const = 0;
  // set streams master volume. 
  virtual void SetStreamMasterVolume(float fVolume) = 0;
  //
  // streaming sound
  virtual void PlayStream(const char *pszFileName, bool bLooped = false, unsigned int nTimeToFadePrevious = 0) = 0;
  virtual void StopStream(unsigned int nTimeToFade = 0) = 0;
  virtual bool IsStreamPlaying() const =0;
  virtual void SetStreamVolume(float fVolume) = 0;
  virtual float GetStreamVolume() const = 0;
  //
  // sample sounds
  virtual int PlaySample(ISound *pSound, bool bLooped = false, unsigned int nStartPos = 0) = 0;
  // update sound acording to internal parameters
  virtual void UpdateSample(ISound *pSound) = 0;
  virtual void StopSample(ISound *pSound) = 0;
  virtual void StopChannel(int nChannel) = 0;
  //
  // Update sounds ( that is needed for 3D sounds )
  virtual void Update(interface ICamera *pCamera) = 0;
  //
  virtual bool Pause(bool bPause) = 0;
  virtual bool PauseStreaming(bool bPause) = 0;
  virtual bool IsPaused() = 0;

  virtual bool IsPlaying(ISound *pSound) =0;

  // current position of the slot
  virtual unsigned int GetCurrentPosition(ISound *pSound) =0;
  virtual void SetCurrentPosition(ISound *pSound, unsigned int pos) =0;
};

#endif // __SFX_H__