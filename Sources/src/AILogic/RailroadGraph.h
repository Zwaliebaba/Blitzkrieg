#ifndef __RAILROADGRAPH_H__
#define __RAILROADGRAPH_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "Graph.h"
#include "../Misc/Spline.h"
// //////////////////////////////////////////////////////////// 
interface IEdge;

class CEdgePoint : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CEdgePoint);
  DECLARE_SERIALIZE;

  IEdge *pEdge;
  int nPart;
  float fT;

public:
  CEdgePoint() : pEdge(nullptr), nPart(-1), fT(-1.0f) {}
  CEdgePoint(const CEdgePoint &edgePoint);
  CEdgePoint(IEdge *pEdge, const int nPart, const float fT) : pEdge(nullptr) { Init(pEdge, nPart, fT); }
  void Init(IEdge *pEdge, int nPart, float fT);

  const CVec2 Get2DPoint() const;
  const CVec2 GetTangent() const;
  IEdge *GetEdge() const { return pEdge; }
  bool IsLastPointOfEdge() const;
  bool IsEqual(CEdgePoint *pEdgePoint) const;

  const bool Less(const CEdgePoint &point) const
  {
    NI_ASSERT_T(pEdge == point.pEdge, "Can't compare edge point. Belong to different edges");
    if (nPart < point.nPart) return true;
    if (nPart == point.nPart && fT < point.fT) return true;
    return false;
  }

  // sets parameters for the same point on the reverse edge ( (v1, v2) -> (v2, v1) )
  void Reverse(IEdge *pReversedEdge);

  //
  const int GetNPart() const { return nPart; }
  const float GetT() const { return fT; }

  friend class CSplineEdge;
  friend class CZeroEdge;
};

// //////////////////////////////////////////////////////////// 
struct SEdgeLessFunctional
{
  bool operator ()(const CPtr<CEdgePoint> &point1, const CPtr<CEdgePoint> &point2) const { return point1->Less(*point2); }
};

// //////////////////////////////////////////////////////////// 
// distance between points on one edge
const float fabs(CEdgePoint *p1, CEdgePoint *p2);
// //////////////////////////////////////////////////////////// 
interface IEdge : IRefCount
{
  virtual IEdge *CreateEdge(CEdgePoint *p1, CEdgePoint *p2) = 0;
  virtual IEdge *CreateReversedEdge() const = 0;

  virtual void SetNodesNumbers(int _v1, int _v2) = 0;
  virtual const int GetNParts() const = 0;
  virtual const CVec2 GetCoordinate(int nPart, float fT) const = 0;
  virtual const CVec2 GetTangent(int nPart, float fT) const = 0;

  virtual const CVec2 GetTangentOfBegin() const = 0;
  virtual const CVec2 GetTangentOfEnd() const = 0;

  virtual CEdgePoint *CreateFirstEdgePoint() = 0;
  virtual CEdgePoint *CreateLastEdgePoint() = 0;

  virtual const int GetFirstNode() const = 0;
  virtual const int GetLastNode() const = 0;

  virtual const CVec2 GetFirst2DPoint() const = 0;
  virtual const CVec2 GetLast2DPoint() const = 0;

  // length for graph
  virtual const float GetLength() const = 0;
  // distance between two points on this edge
  virtual const float GetLength(CEdgePoint *p1, CEdgePoint *p2) = 0;
  virtual void GetClosestPoints(const CVec2 &vPoint, std::list<CPtr<CEdgePoint>> *pPoints, float *pfMinDist, float fTolerance = SConsts::CLOSEST_TO_RAILROAD_POINT_TOLERANCE) = 0;

  virtual bool IsLastPoint(int nPart, float fT) const = 0;

  virtual CEdgePoint *MakeIndent(const CVec2 &vPointToMeasureDist, CEdgePoint *p1, CEdgePoint *p2, float fDist) = 0;
};

// //////////////////////////////////////////////////////////// 
class CSplineEdge : public IEdge
{
  DECLARE_SERIALIZE;

  static const int N_PARTS_FOR_LENGTH_CALCULATING;

  struct SEdgePart
  {
    DECLARE_SERIALIZE;

  public:
    CAnalyticBSpline2 spline;
    float fTBegin, fTEnd;

    SEdgePart() : fTBegin(-1.0f), fTEnd(-1.0f) {}
  };

  std::vector<SEdgePart> edgeParts;

  // numbers of vertices of the beginning and end of the edge
  int v1, v2;
  float fEdgeLength;

  // length of part nPart from fBegin to fEnd
  const float CalculateLengthOfEdgePart(int nPart, float fBegin, float fEnd);
  // length of the entire rib
  void CalculateEdgeLength();
  void Init(CEdgePoint *p1, CEdgePoint *p2);
  CEdgePoint *MakeIndentOnOneSpline(const CVec2 &vPointToMeasureDist, int nPart, float fTBegin, float fTEnd, float fDist);

public:
  CSplineEdge() : v1(-1), v2(-1), fEdgeLength(-1) {}
  explicit CSplineEdge(const struct SVectorStripeObject &edgeDescriptor);
  //
  CSplineEdge(CEdgePoint *p1, CEdgePoint *p2) { Init(p1, p2); }

  IEdge *CreateReversedEdge() const override;

  void SetNodesNumbers(const int _v1, const int _v2) override
  {
    v1 = _v1;
    v2 = _v2;
  }

  const int GetNParts() const override { return edgeParts.size(); }
  const CVec2 GetCoordinate(int nPart, float fT) const override;
  const CVec2 GetTangent(int nPart, float fT) const override;

  const CVec2 GetTangentOfBegin() const override;
  const CVec2 GetTangentOfEnd() const override;

  CEdgePoint *CreateFirstEdgePoint() override;
  CEdgePoint *CreateLastEdgePoint() override;

  const int GetFirstNode() const override { return v1; }
  const int GetLastNode() const override { return v2; }

  const CVec2 GetFirst2DPoint() const override;
  const CVec2 GetLast2DPoint() const override;

  const float GetLength() const override
  {
    NI_ASSERT_T(fEdgeLength >= 0.0f, "Edge length hasn't been initialized");
    return fEdgeLength;
  }

  // distance between two points on this edge
  const float GetLength(CEdgePoint *p1, CEdgePoint *p2) override;
  void GetClosestPoints(const CVec2 &vPoint, std::list<CPtr<CEdgePoint>> *pPoints, float *pfMinDist, float fTolerance = SConsts::CLOSEST_TO_RAILROAD_POINT_TOLERANCE) override;

  CEdgePoint *MakeIndent(const CVec2 &vPointToMeasureDist, CEdgePoint *p1, CEdgePoint *p2, float fDist) override;

  bool IsLastPoint(int nPart, float fT) const override;
};

// //////////////////////////////////////////////////////////// 
class CSimpleSplineEdge : public CSplineEdge
{
  OBJECT_COMPLETE_METHODS(CSimpleSplineEdge);
  DECLARE_SERIALIZE;

public:
  CSimpleSplineEdge() {}
  explicit CSimpleSplineEdge(const struct SVectorStripeObject &edgeDescriptor) : CSplineEdge(edgeDescriptor) {}
  //
  CSimpleSplineEdge(CEdgePoint *p1, CEdgePoint *p2) : CSplineEdge(p1, p2) {}

  IEdge *CreateEdge(CEdgePoint *p1, CEdgePoint *p2) override;
};

// //////////////////////////////////////////////////////////// 
class CZeroEdge : public IEdge
{
  OBJECT_COMPLETE_METHODS(CZeroEdge);
  DECLARE_SERIALIZE;

  CVec2 vFirstPoint, vDir;
  float fTBegin, fTEnd;
  int v1, v2;
  float fLength;

public:
  CZeroEdge() : fLength(-1.0f) {}
  // transition from point p1 of one edge to point p2 of another edge along the zero edge
  CZeroEdge(CEdgePoint *p1, CEdgePoint *p2);

  IEdge *CreateEdge(CEdgePoint *p1, CEdgePoint *p2) override;
  IEdge *CreateReversedEdge() const override;

  void SetNodesNumbers(const int _v1, const int _v2) override
  {
    v1 = _v1;
    v2 = _v2;
  }

  const int GetNParts() const override { return 1; }
  const CVec2 GetCoordinate(int nPart, float fT) const override;
  const CVec2 GetTangent(int nPart, float fT) const override;

  const CVec2 GetTangentOfBegin() const override;
  const CVec2 GetTangentOfEnd() const override;

  CEdgePoint *CreateFirstEdgePoint() override;
  CEdgePoint *CreateLastEdgePoint() override;

  const int GetFirstNode() const override { return v1; }
  const int GetLastNode() const override { return v2; }

  const CVec2 GetFirst2DPoint() const override { return vFirstPoint + vDir * fTBegin; }
  const CVec2 GetLast2DPoint() const override { return vFirstPoint + vDir * fTEnd; }

  const float GetLength() const override
  {
    NI_ASSERT_T(fLength >= 0.0f, "Edge length hasn't been initialized");
    return 0;
  }

  // distance between two points on this edge
  const float GetLength(CEdgePoint *p1, CEdgePoint *p2) override { return 0; }
  void GetClosestPoints(const CVec2 &vPoint, std::list<CPtr<CEdgePoint>> *pPoints, float *pfMinDist, float fTolerance = SConsts::CLOSEST_TO_RAILROAD_POINT_TOLERANCE) override;

  CEdgePoint *MakeIndent(const CVec2 &vPointToMeasureDist, CEdgePoint *p1, CEdgePoint *p2, float fDist) override;

  bool IsLastPoint(int nPart, float fT) const override;
};

// //////////////////////////////////////////////////////////// 
class CRailroad : public CSplineEdge
{
  OBJECT_COMPLETE_METHODS(CRailroad);
  DECLARE_SERIALIZE;

  std::vector<CPtr<CEdgePoint>> intersectionPoints;
  int nIntersectionPoints;

  std::unordered_map<CPtr<CEdgePoint>, int, SDefaultPtrHash> intersectionPointToGraphNode;

public:
  CRailroad() : intersectionPoints(10), nIntersectionPoints(0) {}
  explicit CRailroad(const struct SVectorStripeObject &edgeDescriptor) : CSplineEdge(edgeDescriptor), intersectionPoints(10), nIntersectionPoints(0) {}

  void AddIntersectionPoint(CEdgePoint *pPoint);
  void SetEdges(class CRailroadGraph *pGraph);

  IEdge *CreateEdge(CEdgePoint *p1, CEdgePoint *p2) override { return nullptr; }

  const int GetNodeByIntersectionPoint(CEdgePoint *pPoint);
};

// //////////////////////////////////////////////////////////// 
class CRailroadsIntersection
{
  DECLARE_SERIALIZE;

  CPtr<CEdgePoint> p1;
  CPtr<CEdgePoint> p2;

public:
  CRailroadsIntersection() {}
  CRailroadsIntersection(CEdgePoint *_p1, CEdgePoint *_p2) : p1(_p1), p2(_p2) {}

  CEdgePoint *GetPoint1() const { return p1; }
  CEdgePoint *GetPoint2() const { return p2; }
};

// //////////////////////////////////////////////////////////// 
class CRailroadGraphConstructor
{
  DECLARE_SERIALIZE;

  static const int F_RAILROAD_WIDTH_2;

  using CRailroadsList = std::list<CPtr<CRailroad>>;
  CRailroadsList railroads;
  std::list<CRailroadsIntersection> intersections;

  //
  void SpliceRailroad(CRailroad *pRailroad, std::vector<CPtr<CEdgePoint>> *pPoints, int *pnLen);
  void FindIntersections(CRailroad *pRailroad1, CRailroad *pRailroad2);
  void AddIntersectionEdge(const CRailroadsIntersection &intersection, class CRailroadGraph *pGraph);

public:
  CRailroadGraphConstructor() {}

  void Construct(const struct STerrainInfo &terrain, class CRailroadGraph *pGraph);
};

// //////////////////////////////////////////////////////////// 
struct SPointInfo
{
  int v;
  CVec2 vDir;

  SPointInfo() : v(-1) {}
  SPointInfo(int _v, const CVec2 &_vDir) : v(_v), vDir(_vDir) {}
};

// //////////////////////////////////////////////////////////// 
class CRailroadGraph : public CGraph
{
  DECLARE_SERIALIZE;

  std::unordered_map<DWORD, CObj<IEdge>> edges;
  std::vector<CObj<CEdgePoint>> edgeNodes;

  //
  void LookForPoint(int v, const CVec2 &vDir, std::unordered_set<int> *pVisitedPoints, std::list<SPointInfo> *pPointsList);
  // returns v2 such that a train from an edge with direction vDir could travel to edge ( v, v2 ) and dir was as close as possible to vDir
  // if v2 is not found, then returns -1
  void GetMovablePoint(int v, const CVec2 &vDir, std::unordered_set<int> *pVisitedPoints, std::list<SPointInfo> *pPointsList);

public:
  CRailroadGraph() : edgeNodes(10) {}
  void Clear();

  void AddEdge(IEdge *pEdge);
  const float GetEdgeLength(int v1, int v2) override;

  // vConnectionNode - the nearest vertices must lie in the same connected component as vConnectionNode, if vConnectionNode == -1, then in any
  void GetClosestPoints(const CVec2 &vPoint, std::list<CPtr<CEdgePoint>> *pPoints, float *pfMinDist, int vConnectionNode, float fTolerance = SConsts::CLOSEST_TO_RAILROAD_POINT_TOLERANCE);

  // edge along two vertices
  IEdge *GetEdge(int v1, int v2);
  // edgePoint by vertex
  CEdgePoint *GetEdgePoint(int v) const;

  CEdgePoint *MakeIndent(const CVec2 &vDir, CEdgePoint *pPoint, float fDist);
};

// //////////////////////////////////////////////////////////// 
#endif // __RAILROADGRAPH_H__