#include "StdAfx.h"

#include "MatrixEffectorLeveling.h"
// //////////////////////////////////////////////////////////// 
CMatrixEffectorLeveling::CMatrixEffectorLeveling()
  : lastQuat(QNULL)
{
  fPhi = fTheta = 0;
  fCoeff = 0;
  lastUpdateTime = 0;
  timeSetuped = 0;
}

// //////////////////////////////////////////////////////////// 
// get the angles needed to rotate this normal about the X (phi) and Y (theta) axes
// to match it with the Z axis
inline void GetAngles(const CVec3 &vNormal, float *pfPhi, float *pfTheta)
{
  // phi - rotation in the ZY plane relative to the X axis
  // cos( phi ) = Z*N(x=0) = (0, 0, 1) * (0, Ny, Nz) / sqrt( Ny**2 + Nz**2 );
  {
    const float fLen2 = fabs2(vNormal.y, vNormal.z);
    *pfPhi = fLen2 < 1e-8f ? 0 : vNormal.z / sqrt(fLen2);
    *pfPhi = -Sign(vNormal.y) * acos(Clamp(*pfPhi, -1.0f, 1.0f));
  }
  // theta - rotation in the ZX plane relative to the Y axis
  // cos( theta ) = Z*N(y=0) = Nz / fabs( Nx, Nz )
  {
    const float fLen2 = fabs2(vNormal.x, vNormal.z);
    *pfTheta = fLen2 < 1e-8f ? 0 : vNormal.z / sqrt(fLen2);
    *pfTheta = Sign(vNormal.x) * acos(Clamp(*pfTheta, -1.0f, 1.0f));
  }
}

// //////////////////////////////////////////////////////////// 
bool CMatrixEffectorLeveling::Update(const NTimer::STime &time)
{
  /* float fAddCoeff = float( time - lastUpdateTime ) / 1000.0f * 2.0f;
   */
  constexpr float fAddCoeff = 1;
  CQuat q1(fPhi * fAddCoeff, V3_AXIS_X);
  CQuat q2(fTheta * fAddCoeff, V3_AXIS_Y);
  q1.MinimizeRotationAngle();
  q2.MinimizeRotationAngle();
  lastQuat = q1;
  lastQuat *= q2;
  matResult.Set(lastQuat);
  fCoeff += fAddCoeff;
  //
  lastUpdateTime = time;
  //
  return true;
}

// //////////////////////////////////////////////////////////// 
int CMatrixEffectorLeveling::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.Add(1, &matResult);
  saver.Add(2, &dwReferNormale);
  saver.Add(3, &vDesiredNormale);
  saver.Add(4, &fPhi);
  saver.Add(5, &fTheta);
  saver.Add(6, &lastUpdateTime);
  saver.Add(7, &timeSetuped);
  saver.Add(8, &lastQuat);
  saver.Add(9, &fCoeff);
  return 0;
}

// //////////////////////////////////////////////////////////// 
void CMatrixEffectorLeveling::SetupData(const CVec3 &vNormal, const NTimer::STime &currTime)
{
  // NI_ASSERT_T( vNormal.z > 0, NStr::Format("normal (z = %g) must be > 0", vNormal.z) );
  if (vNormal.z <= 0)
  {
    // NStr::DebugTrace( "normal (z = %g) must be > 0\n", vNormal.z );
    return;
  }
  DWORD dwNormale = Vec3ToDWORD(vNormal);
  if (dwNormale != dwReferNormale)
  {
    GetAngles(vNormal, &fPhi, &fTheta);
    timeSetuped = currTime;
  }
  vDesiredNormale = vNormal;
  dwReferNormale = dwNormale;
  fCoeff = 0;
}

// //////////////////////////////////////////////////////////// 