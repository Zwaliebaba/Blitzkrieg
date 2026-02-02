#ifndef __VIDEOCHECK_H__
#define __VIDEOCHECK_H__

#pragma once

namespace NVideoCheck
{

  struct SVideoMemory
  {
    struct SMemory
    {
      DWORD dwTotal;
      DWORD dwFree;
    };

    //
    SMemory local;
    SMemory nonlocal;
    SMemory texture;
  };

  // get API name (DirectX)
  const wchar_t * GetAPIName();
  // get video memory amount
  bool GetVideoMemory(SVideoMemory *pMemory);

};

#endif // __VIDEOCHECK_H__