#ifndef __FMODSTUB_H__
#define __FMODSTUB_H__

#pragma once

// FMOD 3.x Stub - Provides no-op implementations for FMOD functions
// This allows the project to compile without the FMOD library.
// TODO: Replace with XAudio2 implementation

struct FSOUND_SAMPLE { void *data; unsigned int length; int frequency; int channels; };
struct FSOUND_STREAM { void *data; };

constexpr int FSOUND_FREE = -1;
constexpr int FSOUND_UNMANAGED = -2;
constexpr int FSOUND_STEREOPAN = 128;
constexpr int FSOUND_LOOP_OFF = 0x00000001;
constexpr int FSOUND_LOOP_NORMAL = 0x00000002;
constexpr int FSOUND_LOADMEMORY = 0x00008000;
constexpr int FSOUND_2D = 0x00020000;
constexpr int FSOUND_HW3D = 0x00001000;
constexpr unsigned int FSOUND_CAPS_HARDWARE = 0x00000001;
constexpr unsigned int FSOUND_CAPS_EAX2 = 0x00000002;
constexpr unsigned int FSOUND_CAPS_GEOMETRY_OCCLUSIONS = 0x00000004;
constexpr unsigned int FSOUND_CAPS_GEOMETRY_REFLECTIONS = 0x00000008;

enum FSOUND_OUTPUTTYPES { FSOUND_OUTPUT_NOSOUND, FSOUND_OUTPUT_WINMM, FSOUND_OUTPUT_DSOUND, FSOUND_OUTPUT_A3D };
enum FSOUND_MIXERTYPES { FSOUND_MIXER_BLENDMODE, FSOUND_MIXER_MMXP5, FSOUND_MIXER_MMXP6, FSOUND_MIXER_QUALITY_FPU, FSOUND_MIXER_QUALITY_MMXP5, FSOUND_MIXER_QUALITY_MMXP6 };

constexpr int FSOUND_INIT_USEDEFAULTMIDISYNTH = 0x0001;
constexpr float FMOD_VERSION = 3.75f;

inline float FSOUND_GetVersion() { return FMOD_VERSION; }
inline int FSOUND_GetError() { return 0; }
inline signed char FSOUND_SetOutput(int) { return 1; }
inline signed char FSOUND_SetDriver(int) { return 1; }
inline signed char FSOUND_SetHWND(void*) { return 1; }
inline signed char FSOUND_Init(int, int, unsigned int) { return 1; }
inline void FSOUND_Close() {}
inline int FSOUND_GetNumDrivers() { return 1; }
inline const char* FSOUND_GetDriverName(int) { return "Stub Audio Driver"; }
inline signed char FSOUND_GetDriverCaps(int, unsigned int* c) { if (c) *c = 0; return 1; }
inline void* FSOUND_GetOutputHandle() { return nullptr; }
inline int FSOUND_GetMixer() { return FSOUND_MIXER_QUALITY_FPU; }
inline int FSOUND_GetChannelsPlaying() { return 0; }

inline FSOUND_SAMPLE* FSOUND_Sample_Load(int, const char*, unsigned int, int) { return nullptr; }
inline void FSOUND_Sample_Free(FSOUND_SAMPLE*) {}
inline signed char FSOUND_Sample_SetMinMaxDistance(FSOUND_SAMPLE*, float, float) { return 1; }
inline signed char FSOUND_Sample_SetLoopMode(FSOUND_SAMPLE*, unsigned int) { return 1; }
inline signed char FSOUND_Sample_SetLoopPoints(FSOUND_SAMPLE*, int, int) { return 1; }
inline unsigned int FSOUND_Sample_GetLength(FSOUND_SAMPLE*) { return 0; }
inline signed char FSOUND_Sample_GetDefaults(FSOUND_SAMPLE*, int* f, int* v, int* p, int* r) { if (f) *f = 44100; if (v) *v = 255; if (p) *p = 128; if (r) *r = 0; return 1; }

inline int FSOUND_PlaySound(int, FSOUND_SAMPLE*) { return -1; }
inline int FSOUND_PlaySoundEx(int, FSOUND_SAMPLE*, void*, signed char) { return -1; }
inline signed char FSOUND_StopSound(int) { return 1; }
inline signed char FSOUND_SetPaused(int, signed char) { return 1; }
inline signed char FSOUND_IsPlaying(int) { return 0; }
inline FSOUND_SAMPLE* FSOUND_GetCurrentSample(int) { return nullptr; }
inline unsigned int FSOUND_GetCurrentPosition(int) { return 0; }
inline signed char FSOUND_SetCurrentPosition(int, unsigned int) { return 1; }

inline signed char FSOUND_SetVolume(int, int) { return 1; }
inline signed char FSOUND_SetPan(int, int) { return 1; }
inline int FSOUND_GetVolume(int) { return 255; }
inline int FSOUND_GetPan(int) { return 128; }

inline signed char FSOUND_3D_SetAttributes(int, float*, float*) { return 1; }
inline signed char FSOUND_3D_Listener_SetAttributes(float*, float*, float, float, float, float, float, float) { return 1; }
inline signed char FSOUND_3D_Listener_SetDistanceFactor(float) { return 1; }
inline signed char FSOUND_3D_Listener_SetRolloffFactor(float) { return 1; }

inline FSOUND_STREAM* FSOUND_Stream_OpenFile(const char*, unsigned int, int) { return nullptr; }
inline signed char FSOUND_Stream_Close(FSOUND_STREAM*) { return 1; }
inline int FSOUND_Stream_Play(int, FSOUND_STREAM*) { return -1; }
inline signed char FSOUND_Stream_SetEndCallback(FSOUND_STREAM*, void*, int) { return 1; }
inline signed char FSOUND_Stream_SetSynchCallback(FSOUND_STREAM*, void*, int) { return 1; }

#endif // __FMODSTUB_H__
