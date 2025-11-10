#include "stdafx.h"

#include "Trigonometry.h"

namespace NTrg
{
  static constexpr int NPOWER = 18;
  static constexpr int ACCURACY = 262144;// 2^NPOWER
  static std::vector<float> values;

  void Init()
  {
    CPtr<IDataStream> pStream = GetSingleton<IDataStorage>()->OpenStream("sin.arr", STREAM_ACCESS_READ);
    CPtr<IStructureSaver> pSaver = CreateStructureSaver(pStream, IStructureSaver::READ);
    CSaverAccessor saver = pSaver;
    saver.Add(1, &values);
    values.push_back(1.0f);
  }

  float Sin(float fAlpha)
  {
    /* //crap{ for test
         */
    int nSign = 1 - 2 * static_cast<int>(bit_cast<DWORD>(fAlpha) >> 31);
    fAlpha = fabsf(fAlpha);

    int nIndex = static_cast<int>(fAlpha / FP_PI2 * ACCURACY) & (4 * ACCURACY - 1);

    nSign = nSign * (1 - 2 * (nIndex >> (NPOWER + 1)));
    nIndex = nIndex & (ACCURACY * 2 - 1);

    const int nDecr = nIndex >> NPOWER;
    nIndex = nIndex * (1 - nDecr) + (2 * ACCURACY - nIndex) * nDecr;

    return nSign * values[nIndex];

    /* //
     */
  }

  float ASin(float fSin)
  {
    const float fSign = 1 - 2 * static_cast<int>(bit_cast<DWORD>(fSin) >> 31);
    fSin = fabsf(fSin);

    auto iter = std::lower_bound(values.begin(), values.end(), fSin);
    const int nDistance = std::distance(values.begin(), iter);

    return fSign * FP_PI2 * (static_cast<float>(nDistance) / static_cast<float>(ACCURACY));
  }
};