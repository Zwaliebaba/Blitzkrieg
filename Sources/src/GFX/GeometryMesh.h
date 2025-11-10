#ifndef __GEOMETRYMESH_H__
#define __GEOMETRYMESH_H__

#pragma once

struct SSingleMesh
{
  // primary mesh
  CPtr<IGFXVertices> pVertices;// vertex buffer
  CPtr<IGFXIndices> pIndices;// index buffer
  // mesh BVs
  SGFXAABB aabb;// axis-aligned bounding box
  SGFXBoundSphere sphere;// bounding sphere
  //
  int nMatrixIndex;// matrix index to be transformed with
  int nPriority;// priority for rendering of this figure
  //
  SSingleMesh() {}
  SSingleMesh(IGFXVertices *_pVertices, IGFXIndices *_pIndices, const SGFXAABB &_aabb, const SGFXBoundSphere &_sphere);
};

class CGeometryMesh : public IGFXMesh
{
public:
  using CMeshesList = std::vector<SSingleMesh>;

private:
  OBJECT_COMPLETE_METHODS(CGeometryMesh);
  DECLARE_SERIALIZE;
  SHARED_RESOURCE_METHODS(nRefData.a, "Mesh");
  //
  CMeshesList figures;
  SGFXBoundSphere sphere;
  SGFXAABB aabb;

public:
  void AddSingleMesh(const SSingleMesh &mesh, int nPriority)
  {
    figures.push_back(mesh);
    figures.back().nMatrixIndex = figures.size() - 1;
    figures.back().nPriority = nPriority;
  }

  void SortMeshes();
  void Clear() { figures.clear(); }
  //
  const CMeshesList &GetFigures()
  {
    if (figures.empty()) Load();
    return figures;
  }

  //
  void SetAABB(const CVec3 &vCenter, const CVec3 &vHalfSize);
  //
  void STDCALL SwapData(ISharedResource *pResource) override
  {
    auto pRes = dynamic_cast<CGeometryMesh *>(pResource);
    NI_ASSERT_TF(pRes != 0, "shared resource is not a CGeometryMesh", return);
    //
    std::swap(figures, pRes->figures);
    std::swap(sphere, pRes->sphere);
    std::swap(aabb, pRes->aabb);
  }

  // internal container clearing
  void STDCALL ClearInternalContainer() override { figures.clear(); }
  bool STDCALL Load(bool bPreLoad = false) override;
  // bounding volume checks
  const SGFXBoundSphere & STDCALL GetBS() override { return sphere; }
  const SGFXAABB & STDCALL GetAABB() override { return aabb; }
  bool STDCALL IsHit(const CVec2 &vPos, const SHMatrix *matrices) override;
};

#endif // __GEOMETRYMESH_H__