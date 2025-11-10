#include "StdAfx.h"

#include "SampleSounds.h"


// ************************************************************************************************************************ //
// **
// ** base shared sound sample resource
// **
// **
// **
// ************************************************************************************************************************ //

void CSoundSample::SwapData(ISharedResource *pResource)
{
  auto pRes = dynamic_cast<CSoundSample *>(pResource);
  NI_ASSERT_TF(pRes != 0, NStr::Format("shared resource is not a \"%s\"", typeid(*this).name()), return);
  //
  std::swap(sample, pRes->sample);
}

void CSoundSample::SetLoop(bool bEnable)
{
  bLooped = bEnable;
  if (sample) FSOUND_Sample_SetLoopMode(sample, bEnable ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
}

bool CSoundSample::Load(const bool bPreLoad)
{
  if ((sample != nullptr) || bPreLoad) return true;
  const std::string szStreamName = GetSharedResourceFullName();
  //
  CPtr<IDataStream> pStream = GetSingleton<IDataStorage>()->OpenStream(szStreamName.c_str(), STREAM_ACCESS_READ);
  if (pStream == nullptr) return false;
  const int nSize = pStream->GetSize();
  std::vector<char> buffer(nSize);
  const int nCheck = pStream->Read(&(buffer[0]), nSize);
  //
  NI_ASSERT_SLOW_TF(nCheck == nSize, "Readed size doesn't match requested", return false);
  FSOUND_SAMPLE *sample = FSOUND_Sample_Load(FSOUND_UNMANAGED, &(buffer[0]), GetMode() | FSOUND_LOADMEMORY, nSize);
  if (sample == nullptr) return false;
  SetSample(sample);
  return true;
}

// ************************************************************************************************************************ //
// **
// ** sounds...
// **
// **
// **
// ************************************************************************************************************************ //

int CSound2D::Visit(interface ISFXVisitor *pVisitor) { return pVisitor->VisitSound2D(this); }

int CSound3D::Visit(interface ISFXVisitor *pVisitor) { return pVisitor->VisitSound3D(this, vPos); }

void CSound3D::SetPosition(const CVec3 &vPos3)
{
  // FMOD treats +X as right, +Y as up, and +Z as forwards
  CVec3 vLocalPos(vPos3.x, vPos3.z, vPos3.y);
  if (IsPlaying())
  {
    if (!bDopplerFlag) FSOUND_3D_SetAttributes(GetChannel(), static_cast<float *>(vLocalPos.m), 0);// 0 because we do not use the Doppler effect
    else
    {
      // let's calculate the speed
      /* NTimer::STime time = GetSingleton<IGameTimer>()->GetGameTime();
       */
      FSOUND_3D_SetAttributes(GetChannel(), static_cast<float *>(vLocalPos.m), 0);
    }
  }
  //
  vPos = vLocalPos;
}

int CSound3D::Play()
{
  FSOUND_SAMPLE *sample = GetSample()->GetInternalContainer();
  int nChannel = -1;
  if (FSOUND_SAMPLE *sample = GetSample()->GetInternalContainer())
  {
    //
    // nChannel = FSOUND_PlaySound3DAttrib( FSOUND_FREE, sample, -1, -1, -1, vPos.m, 0 );

    // nChannel = FSOUND_PlaySoundEx( FSOUND_FREE, sample, 0, true );
    // FSOUND_3D_SetAttributes( nChannel, vPos.m, 0 );
    // FSOUND_SetPaused( nChannel, false );
    nChannel = FSOUND_PlaySound(FSOUND_FREE, sample);
  }
  SetChannel(nChannel);
  return nChannel;
}