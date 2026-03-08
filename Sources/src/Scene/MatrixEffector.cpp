#include "StdAfx.h"

#include "MatrixEffector.h"

// ************************************************************************************************************************ //
// **
// ** recoil
// **
// **
// **
// ************************************************************************************************************************ //

int CMatrixEffectorRecoil::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.Add(1, &matResult);
  saver.Add(2, &timeStart);
  saver.Add(3, &timeLife);
  saver.Add(4, &lastUpdateTime);
  saver.Add(5, &vAxis);
  saver.Add(6, &fAngle);
  return 0;
}

bool CMatrixEffectorRecoil::Update(const NTimer::STime &time)
{
  if (time < timeStart) return true;
  if (time >= timeStart + timeLife) return false;
  //
  NTimer::STime timeDiff = time - timeStart;
  if (timeDiff < timeLife / 10)
  {
    float fCoeff = static_cast<float>(timeDiff) / static_cast<float>(timeLife / 10);
    CQuat quat(fAngle * fCoeff, vAxis);
    quat.MinimizeRotationAngle();
    quat.DecompEulerMatrix(&matResult);
  }
  else if (timeDiff >= timeLife / 10)
  {
    float fCoeff = static_cast<float>(timeLife - timeDiff) / static_cast<float>(timeLife - timeLife / 10);
    CQuat quat(fAngle * fCoeff, vAxis);
    quat.MinimizeRotationAngle();
    quat.DecompEulerMatrix(&matResult);
  }
  else matResult = MONE;
  return true;
}