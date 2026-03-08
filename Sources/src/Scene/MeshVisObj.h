#ifndef __MESHVISOBJ_H__
#define __MESHVISOBJ_H__

#pragma once

#include "ObjVisObj.h"

template<class TEffector>
struct SEffector
{
  int nID;// effector ID
  int nPart;// bodypart to attach this effector to
  CPtr<TEffector> pEffector;// effector itself
  //
  SEffector() : nID(-1), nPart(-1) {}
  SEffector(int _nID, TEffector *_pEffector) : nID(_nID), nPart(-1), pEffector(_pEffector) {}
  SEffector(int _nID, TEffector *_pEffector, int _nPart) : nID(_nID), nPart(_nPart), pEffector(_pEffector) {}
  //
  const SHMatrix &GetMatrix() const { return pEffector->GetMatrix(); }
  //
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &nID);
    saver.Add(2, &nPart);
    saver.Add(3, &pEffector);
    return 0;
  }
};

using SMatrixEffector = SEffector<ISceneMatrixEffector>;

class CMeshVisObj : public CTObjVisObj<CTRefCount<IMeshVisObj>>
{
  OBJECT_SERVICE_METHODS(CMeshVisObj);
  DECLARE_SERIALIZE;
  //
  CPtr<IGFXMesh> pMesh;
  CPtr<IMeshAnimation> pAnim;// animation system
  CPtr<IGFXTexture> pTexture;// texture
  //
  SHMatrix matPlacement, matPlacement1;// placement and placement with effectors
  CQuat quat;// rotation
  CVec3 vScale;// scale
  bool bHasScale;// is scale != 1?
  SGFXMaterial material;// materail properties
  // matrix effectors
  using CEffectorsList = std::list<SMatrixEffector>;
  CEffectorsList effectors;
  CPtr<ISceneMaterialEffector> pMaterialEffector;
  //
  void RepositionIcons() override;
  void RepositionIconsLocal(DWORD placement);
  //
  ~CMeshVisObj() override {}

public:
  CMeshVisObj()
    : quat(QNULL), vScale(1, 1, 1), bHasScale(false), pMaterialEffector(nullptr)
  {
    Identity(&matPlacement);
    Zero(material);
    material.vAmbient = CVec4(1, 1, 1, 1);
    material.vDiffuse = CVec4(1, 1, 1, 1);
  }

  //
  bool Init(IGFXMesh *_pMesh, IMeshAnimation *_pAnim, IGFXTexture *_pTexture)
  {
    SetMeshAnim(_pMesh, _pAnim);
    SetTexture(_pTexture);
    return true;
  }

  void SetMeshAnim(IGFXMesh *_pMesh, IMeshAnimation *_pAnim)
  {
    pMesh = _pMesh;
    pAnim = _pAnim;
    effectors.clear();
    dwLastUpdateTime = 0;
  }

  void SetTexture(IGFXTexture *_pTexture) { pTexture = _pTexture; }
  // update state
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override;
  // scale
  void STDCALL SetScale(float sx, float sy, float sz) override
  {
    vScale.Set(sx, sy, sz);
    bHasScale = (sx != 1.0f) || (sy != 1.0f) || (sz != 1.0f);
  }

  // placement
  void STDCALL SetDirection(const int _nDirection) override
  {
    SetDir(_nDirection);
    quat.FromAngleAxis(ToRadian(static_cast<float>(GetDir()) / 65536.0f * 360.0f), 0, 0, 1);
  }

  void STDCALL SetPosition(const CVec3 &_pos) override { SetPos(_pos); }

  void STDCALL SetPlacement(const CVec3 &pos, const int nDir) override
  {
    SetPosition(pos);
    SetDirection(nDir);
  }

  // opacity
  void STDCALL SetOpacity(BYTE opacity) override { material.vDiffuse.a = static_cast<float>(opacity) / 255.0f; }

  void STDCALL SetColor(DWORD color) override
  {
    material.vDiffuse.Set(material.vDiffuse.a,
                          static_cast<float>((color >> 16) & 0xff) / 255.0f,
                          static_cast<float>((color >> 8) & 0xff) / 255.0f,
                          static_cast<float>((color) & 0xff) / 255.0f);
  }

  void STDCALL SetSpecular(DWORD color) override
  {
    material.vSpecular.Set(1.0f,
                           static_cast<float>((color >> 16) & 0xff) / 255.0f,
                           static_cast<float>((color >> 8) & 0xff) / 255.0f,
                           static_cast<float>((color) & 0xff) / 255.0f);
  }

  // hit test
  bool STDCALL IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) override;
  bool STDCALL IsHit(const SHMatrix &matTransform, const RECT &rect) override;
  // drawing
  bool STDCALL Draw(IGFX *pGFX) override;
  bool STDCALL DrawBB(IGFX *pGFX) override;
  bool STDCALL DrawShadow(IGFX *pGFX, const SHMatrix *pMatShadow, const CVec3 &vSunDir) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  // animation
  void STDCALL SetAnimation(const int nAnim) override { pAnim->SetAnimation(nAnim); }
  IAnimation * STDCALL GetAnimation() override { return pAnim; }
  //
  void STDCALL SetAnim(interface IAnimation *_pAnim) override { pAnim = dynamic_cast<IMeshAnimation *>(_pAnim); }
  IGFXMesh * STDCALL GetMesh() const override { return pMesh; }
  IGFXTexture * STDCALL GetTexture() const override { return pTexture; }
  const SHMatrix & STDCALL GetPlacement() const override { return matPlacement; }
  const SHMatrix & STDCALL GetPlacement1() const override { return matPlacement1; }
  const SHMatrix STDCALL GetBasePlacement() override;
  const SHMatrix * STDCALL GetMatrices() override { return pAnim->GetMatrices(matPlacement1); }
  const SHMatrix * STDCALL GetExtMatrices(const SHMatrix &matExternal) override;
  //
  DWORD STDCALL CheckForViewVolume(const SPlane *pViewVolumePlanes) override;
  //
  void STDCALL AddEffector(int nID, ISceneMatrixEffector *pEffector, int nPart = -1) override;
  void STDCALL RemoveEffector(int nID, int nPart = -1) override;
  void STDCALL AddMaterialEffector(ISceneMaterialEffector *pEffector) override;
  void STDCALL RemoveMaterialEffector() override;
};

#endif // __MESHVISOBJ_H__