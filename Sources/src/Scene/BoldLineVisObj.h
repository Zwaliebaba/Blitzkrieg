#ifndef __BOLDLINEVISOBJ_H__
#define __BOLDLINEVISOBJ_H__

#pragma once

class CBoldLineVisObj : public IBoldLineVisObj
{
  OBJECT_NORMAL_METHODS(CBoldLineVisObj);
  DECLARE_SERIALIZE;
  //
  CVec3 vStart;// start of the line
  CVec3 vEnd;// end of the line
  float fWidth;// lines width
  DWORD color;// color
  bool bSetuped;// is this line completely setup?
  CVec3 corners[4];
  //
  bool IsSetuped() const { return bSetuped; }
  void SetupLocal();

public:
  CBoldLineVisObj();
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool STDCALL Draw(interface IGFX *pGFX) override { return false; }
  void STDCALL Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void STDCALL Setup(const CVec3 &vStart, const CVec3 &vEnd, float fWidth, DWORD color) override;
};

#endif // __BOLDLINEVISOBJ_H__