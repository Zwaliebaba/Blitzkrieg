#ifndef __GRAPH_H__
#define __GRAPH_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
// undirected graph without loops with nonnegative edge weights
class CGraph
{
  DECLARE_SERIALIZE;

  // distances from v1 to vertices for the calculated path
  std::vector<float> dst;
  std::vector<int> pred;

  std::vector<int> graphComponent;

  int v1, v2;

  //
  void FindUpperComponent(int v);

protected:
  // for each vertex a list of neighboring
  std::vector<std::list<int>> nodes;
  int n;

public:
  CGraph() : v1(-1), v2(-1), n(0) {}
  void Clear();

  const int GetNNodes() const { return n; }
  void AddEdge(int v1, int v2);

  // calculate the path from v1 to v2
  void ComputePath(int v1, int v2);
  // display the length of the path calculated in ComputePath
  // if the path is not found, then -1
  const float GetPathLength() const;
  void GetPath(std::list<int> *pPath) const;

  // edge length between v1 and v2
  virtual const float GetEdgeLength(int v1, int v2) = 0;

  // Are vertices v1 and v2 in the same connected component?
  bool IsInOneGraphComponent(int v1, int v2);
};

// //////////////////////////////////////////////////////////// 
#endif // __GRAPH_H__