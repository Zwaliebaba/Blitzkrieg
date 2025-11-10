#ifndef __MESHANIMATION_H__
#define __MESHANIMATION_H__
// //////////////////////////////////////////////////////////// 
#include "Effectors.h"
// //////////////////////////////////////////////////////////// 
// ************************************************************************************************************************ //
// **
// ** shared mesh skeleton
// **
// **
// **
// ************************************************************************************************************************ //

struct SMeshSkeletonData : ISharedResource
{
  OBJECT_COMPLETE_METHODS(SMeshSkeletonData);
  DECLARE_SERIALIZE;
  SHARED_RESOURCE_METHODS(nRefData.a, "Mesh.Skeleton");

public:
  //
  struct SNodeData
  {
    // base info
    std::string szName;// node name
    SHMatrix matBase;// base placement
    int nIndex;// node index
    // procedural animation info
    CVec3 vAxis;// animation axis
    BYTE animType;// rotation (1) or translation (2)
    // children info
    std::vector<int> children;// children indices
  };

  //
  using CNodesList = std::vector<SNodeData>;
  CNodesList nodes;// all nodes
  int nTopNode;// top node index
  std::vector<int> locators;// locator indexes;
  //
  void STDCALL SwapData(ISharedResource *pResource) override
  {
    auto pRes = dynamic_cast<SMeshSkeletonData *>(pResource);
    NI_ASSERT_TF(pRes != 0, NStr::Format("shared resource is not a \"%s\"", typeid(*this).name()), return);
    //
    std::swap(nodes, pRes->nodes);
    std::swap(nTopNode, pRes->nTopNode);
    std::swap(locators, pRes->locators);
  }

  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
  // initialization
  void CreateFrom(const SSkeletonFormat &skeleton);
  //
  const SNodeData &GetNode(int nIndex) const { return nodes[nIndex]; }
  const SNodeData &GetTopNode() const { return nodes[nTopNode]; }

  const int GetNumNodes() const { return nodes.size(); }
  const SNodeData *GetNodes() const { return &(nodes[0]); }

  const int GetNumLocators() const { return locators.size(); }
  const int *GetLocatorIndices() const { return &(locators[0]); }
};

// ************************************************************************************************************************ //
// **
// ** procedural animation node
// **
// **
// **
// ************************************************************************************************************************ //

struct SProcAnimNode
{
  DWORD startTime;// absolute start time
  DWORD endTime;// absolute end time
  float fValue;// absolute value to react at 'endTime'
  float fAtom;// elementary increment
  // serialization
  int operator&(IStructureSaver &ss);
  //
  SProcAnimNode(DWORD _startTime = 0, DWORD _endTime = 1, float _fValue = 0)
    : startTime(_startTime), endTime(_endTime), fValue(_fValue), fAtom(0) {}

  //
  // recalculate elementary increment
  void CalcTransAtom(float fCurrVal) { fAtom = (fValue - fCurrVal) / static_cast<float>(endTime - startTime); }

  void CalcRotAtom(float fCurrVal)
  {
    float fAngle = fValue - fCurrVal;
    if (fabs(fAngle) > PI) fAngle = Sign(fAngle) * (fabs(fAngle) - FP_2PI);
    fAtom = fAngle / static_cast<float>(endTime - startTime);
  }

  float GetAtom() const { return fAtom; }
  // cut off this node starting from time time
  void CutoffFrom(float fCurrValue, DWORD lastTime, DWORD currTime)
  {
    const DWORD stime = Max(lastTime, startTime);
    fValue = fCurrValue + (fValue - fCurrValue) * static_cast<float>(currTime - stime) / static_cast<float>(endTime - stime);
    endTime = currTime;
  }
};

// //////////////////////////////////////////////////////////// 
// ************************************************************************************************************************ //
// **
// ** pre-defined animation node
// **
// **
// **
// ************************************************************************************************************************ //

struct SMeshAnimNodeData
{
  CVec3 vPos;
  CQuat qRot;
  //
  void Interpolate(const SMeshAnimNodeData &n1, const SMeshAnimNodeData &n2, float fCoeff)
  {
    vPos.Interpolate(n1.vPos, n2.vPos, fCoeff);
    qRot.Interpolate(n1.qRot, n2.qRot, fCoeff);
  }
};

using SMeshAnimData = CArray2D<SMeshAnimNodeData>;
// //////////////////////////////////////////////////////////// 
// ************************************************************************************************************************ //
// **
// ** non-shared personal skeleton
// **
// **
// **
// ************************************************************************************************************************ //

struct SSkeletonNode
{
  using CProcAnimNodesList = std::list<SProcAnimNode>;
  using CChildrenList = std::vector<SSkeletonNode>;
  int nIndex;// skeleton node index
  // current procedural animation state
  int nAnimType;
  DWORD lastTime;// last time, animation was updated
  float fValue;// value, which corresponds to the 'lastTime'
  CProcAnimNodesList procNodes;// all procedural animation nodes
  // children info
  CChildrenList children;// children list
  // serialization
  int operator&(IStructureSaver &ss);
  //
  // recalculate the atom for the node based on the current value
  void CalcAtom(SProcAnimNode *pNode) const
  {
    if (nAnimType == 1) pNode->CalcRotAtom(fValue);
    else pNode->CalcTransAtom(fValue);
  }

  // rewind time to 'time'
  void RollTo(DWORD time)
  {
    for (auto it = procNodes.begin(); it != procNodes.end();)
    {
      if (it->startTime < time)
      {
        if (it->endTime <= time)// if the time of this node has passed completely
        {
          fValue = it->fValue;
          lastTime = it->endTime;
          // let's delete it
          it = procNodes.erase(it);
          // let's recalculate the atom for the next node
          if (it != procNodes.end()) CalcAtom(&(*it));
        }
        else// if we are somewhere inside this node
        {
          // let's interpolate to the current time from the last update
          fValue += it->GetAtom() * (time - Max(lastTime, it->startTime));
          lastTime = time;
          break;
        }
      }
      else
      {
        lastTime = time;
        break;
      }
    }
  }

  // skip all nodes starting from 'time'
  void SkipFrom(DWORD time)
  {
    for (auto it = procNodes.begin(); it != procNodes.end(); ++it)
    {
      if (it->startTime <= time)
      {
        if (it->endTime >= time)
        {
          it->CutoffFrom(fValue, lastTime, time);
          CalcAtom(&(*it));
        }
        else
        {
          procNodes.erase(it, procNodes.end());
          break;
        }
      }
      else
      {
        procNodes.erase(it, procNodes.end());
        break;
      }
    }
  }

  //
  void CutProceduralAnimation(const NTimer::STime &time)
  {
    RollTo(time);
    SkipFrom(time);
  }

  //
  void CalcNode(DWORD time, CMatrixStack<32> &mstack, SHMatrix *pMatrix, SMatrixEffectorsList *pEffectors,
                const SMeshSkeletonData *pSkeleton, const SMeshAnimData *pAnimation, const NTimer::STime &timeDiff);
  //
  //
  SSkeletonNode()
  {
    nIndex = 0;
    lastTime = 0;
    fValue = 0;
  }

  //
  //
  // get current matrices (perform calculation!!!)
  void GetMatrix(DWORD time, CMatrixStack<32> &mstack, SHMatrix *pMatrices, SMatrixEffectorsList *pEffectors,
                 const SMeshSkeletonData *pSkeleton, const SMeshAnimData *pAnimation, const NTimer::STime &timeDiff);
  void GetMatrixNoRecurse(DWORD time, CMatrixStack<32> &mstack, SHMatrix *pMatrix, SMatrixEffectorsList *pEffectors,
                          const SMeshSkeletonData *pSkeleton, const SMeshAnimData *pAnimation, const NTimer::STime &timeDiff);
  // add procedural node
  void AddProcedural(DWORD currTime, DWORD _startTime, DWORD _endTime, float _fValue)
  {
    // process previous nodes
    RollTo(Min(currTime, _startTime));
    SkipFrom(_startTime);
    // create new node
    SProcAnimNode node(_startTime, _endTime, _fValue);
    // calc atom for the new node
    if (procNodes.empty()) CalcAtom(&node);
    // store this node
    procNodes.push_back(node);
  }

  //
  int GetNumNodes() const
  {
    int nNumNodes = 1;
    for (auto it = children.begin(); it != children.end(); ++it) nNumNodes += it->GetNumNodes();
    return nNumNodes;
  }
};

// //////////////////////////////////////////////////////////// 
class CMeshSkeleton : public IRefCount
{
  OBJECT_NORMAL_METHODS(CMeshSkeleton);
  DECLARE_SERIALIZE;

  SSkeletonNode topnode;// top node
  std::vector<SSkeletonNode *> nodes;// all nodes vector (for the fast indexed access)
  CPtr<SMeshSkeletonData> pSkeleton;// base skeleton info
public:
  void Init(SMeshSkeletonData *_pSkeleton);
  // 
  int GetNumNodes() const { return pSkeleton->nodes.size(); }
  void GetMatrices(DWORD time, CMatrixStack<32> &mstack, SHMatrix *pMatrices,
                   SMatrixEffectorsList *pEffectors, const SMeshAnimData *pAnimation, const NTimer::STime &timeDiff);
  void GetBaseMatrix(DWORD time, CMatrixStack<32> &mstack, SHMatrix *pMatrix,
                     SMatrixEffectorsList *pEffectors, const SMeshAnimData *pAnimation, const NTimer::STime &timeDiff);
  int GetTopNodeIndex() const { return topnode.nIndex; }
  // procedural animation
  void AddProceduralNode(int nNodeIdx, DWORD currTime, DWORD startTime, DWORD endTime, float fValue) { nodes[nNodeIdx]->AddProcedural(currTime, startTime, endTime, fValue); }

  void CutProceduralAnimation(const NTimer::STime &time, const int nModelPart)
  {
    if (nModelPart == -1) { for (auto it = nodes.begin(); it != nodes.end(); ++it) (*it)->CutProceduralAnimation(time); }
    else if ((nModelPart >= 0) && (nModelPart < nodes.size())) nodes[nModelPart]->CutProceduralAnimation(time);
  }

  // data-for-editor retrieving
  int GetNumLocators() const { return pSkeleton->locators.size(); }
  const int *GetAllLocatorIndices() const { return &(pSkeleton->locators[0]); }

  void GetAllLocatorNames(const char **ppBuffer, int nBufferSize) const
  {
    for (std::vector<int>::const_iterator it = pSkeleton->locators.begin(); it != pSkeleton->locators.end(); ++it)
    {
      NI_ASSERT_TF(nBufferSize != 0, "Wrong buffer size was passed to function", return);
      *ppBuffer = pSkeleton->nodes[*it].szName.c_str();
      ++ppBuffer;
      --nBufferSize;
    }
  }

  void GetAllNodeNames(const char **ppBuffer, int nBufferSize) const
  {
    for (SMeshSkeletonData::CNodesList::const_iterator it = pSkeleton->nodes.begin(); it != pSkeleton->nodes.end(); ++it)
    {
      NI_ASSERT_TF(nBufferSize != 0, "Wrong buffer size was passed to function", return);
      *ppBuffer = it->szName.c_str();
      ++ppBuffer;
      --nBufferSize;
    }
  }
};

// //////////////////////////////////////////////////////////// 
struct SMeshAnimationData : ISharedResource
{
  OBJECT_NORMAL_METHODS(SMeshAnimationData);
  SHARED_RESOURCE_METHODS(nRefData.a, "Mesh.Animation");

public:
  using CAnimationsList = std::vector<SMeshAnimData>;
  CAnimationsList animations;
  bool HasAnimation(const int nAnim) const { return nAnim < animations.size(); }

  const SMeshAnimData *GetAnimation(const int nAnim)
  {
    if (!HasAnimation(nAnim)) return nullptr;
    const SMeshAnimData *pAnim = &(animations[nAnim]);
    return pAnim->IsEmpty() ? nullptr : pAnim;
  }

  //
  int STDCALL operator&(IStructureSaver &ss) override { return 0; }
  //
  void STDCALL SwapData(ISharedResource *pResource) override
  {
    auto pRes = dynamic_cast<SMeshAnimationData *>(pResource);
    NI_ASSERT_TF(pRes != 0, NStr::Format("shared resource is not a \"%s\"", typeid(*this).name()), return);
    std::swap(animations, pRes->animations);
  }

  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
};

// //////////////////////////////////////////////////////////// 
class CMeshAnimation : public IMeshAnimation, public IMeshAnimationEdit
{
  OBJECT_COMPLETE_METHODS(CMeshAnimation);
  DECLARE_SERIALIZE;
  //
  CPtr<SMeshAnimationData> animations;// shared animations data
  const SMeshAnimData *pAnimation;// current animation shortcut
  int nCurrAnim;// current animation index
  // animation parameters
  std::vector<SHMatrix> matrices;// mesh animation matrices...
  DWORD dwStartTime;// animation start time
  DWORD dwCurrTime;// current time
  float fSpeedCoeff;// animation speed coefficient
  // effectors
  std::vector<SMatrixEffectorsList> effectors;
  // shared skeleton data
  CPtr<CMeshSkeleton> pSkeleton;// skeleton
  //
  bool ReAcquireAnimation()
  {
    if ((animations != nullptr) && (nCurrAnim != -1))
    {
      if (pAnimation == nullptr)
      {
        pAnimation = animations->GetAnimation(nCurrAnim);
        if (pAnimation == nullptr) nCurrAnim = -1;
      }
      return pAnimation != nullptr;
    }
    return false;
  }

  const NTimer::STime GetTimeDiff() const { return MINT(fSpeedCoeff * (dwCurrTime - dwStartTime)); }

public:
  CMeshAnimation() : pAnimation(nullptr), nCurrAnim(-1), dwStartTime(0), fSpeedCoeff(1) {}
  //
  void Update(const NTimer::STime &time);
  //
  void STDCALL Visit(IAnimVisitor *pVisitor) override;
  //
  void STDCALL SetTime(DWORD time) override { dwCurrTime = time; }
  void STDCALL SetStartTime(DWORD time) override { dwStartTime = time; }
  void STDCALL SetAnimSpeedCoeff(float fCoeff) override { fSpeedCoeff = fCoeff; }

  bool STDCALL SetAnimation(const int nAnim) override
  {
    if (nCurrAnim == nAnim) return ReAcquireAnimation();
    nCurrAnim = nAnim;
    pAnimation = nullptr;
    const bool bRetVal = ReAcquireAnimation();
    if (bRetVal) fSpeedCoeff = 1;
    return bRetVal;
  }

  int STDCALL GetAnimation() const override { return nCurrAnim; };
  int STDCALL GetLengthOf(const int nAnim) override { return 1; }
  int STDCALL GetNumNodes() const override { return matrices.size(); }
  const SHMatrix * STDCALL GetMatrices(const SHMatrix &matBase) override;
  const SHMatrix * STDCALL GetCurrMatrices() const override { return &(matrices[0]); }
  void STDCALL GetBaseMatrix(const SHMatrix &matBase, SHMatrix *pResult) override;
  // "manual" animation
  void STDCALL AddProceduralNode(int nNodeIdx, DWORD currTime, DWORD startTime, DWORD endTime, float fValue) override;
  void STDCALL CutProceduralAnimation(const NTimer::STime &time, int nModelPart = -1) override;
  // effectors
  void STDCALL AddEffector(IMatrixEffector *pEffector, int nID, int nPart) override;
  void STDCALL RemoveEffector(int nID, int nPart) override;
  IMatrixEffector * STDCALL GetEffector(int nID, int nPart) override;
  // all nodes access
  void STDCALL GetAllNodeNames(const char **ppBuffer, int nBufferSize) const override { pSkeleton->GetAllNodeNames(ppBuffer, nBufferSize); }
  // locators access
  int STDCALL GetNumLocators() const override { return pSkeleton->GetNumLocators(); }
  const int * STDCALL GetAllLocatorIndices() const override { return pSkeleton->GetAllLocatorIndices(); }
  void STDCALL GetAllLocatorNames(const char **ppBuffer, int nBufferSize) const override { pSkeleton->GetAllLocatorNames(ppBuffer, nBufferSize); }
  //
  void Init(CMeshSkeleton *_pSkeleton, SMeshAnimationData *_animations)
  {
    pSkeleton = _pSkeleton;
    animations = _animations;
    matrices.resize(pSkeleton->GetNumNodes());
    effectors.resize(pSkeleton->GetNumNodes());
  }
};

// //////////////////////////////////////////////////////////// 
#endif // __MESHANIMATION_H__