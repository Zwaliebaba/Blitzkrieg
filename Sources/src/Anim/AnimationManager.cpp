#include "StdAfx.h"

#include "AnimationManager.h"

#include "SpriteAnimation.h"
#include "MeshAnimation.h"

// ************************************************************************************************************************ //
// **
// ** animation manager
// **
// **
// **
// ************************************************************************************************************************ //

ISpriteAnimation *CAnimationManager::GetSpriteAnimation(const char *pszName)
{
  const std::string szName = pszName;
  if (SSpriteAnimationFormat *pAnimations = sprites.Get(szName))
  {
    auto pAnim = new CSpriteAnimation();
    pAnim->Init(pAnimations);
    return pAnim;
  }
  if (SSpritesPack *pSprites = complexsprites.Get(szName))
  {
    auto pAnim = new CComplexSprite();
    pAnim->Init(pSprites);
    return pAnim;
  }
  return nullptr;
}

IMeshAnimation *CAnimationManager::GetMeshAnimation(const char *pszName)
{
  std::string szName = pszName;
  // get shared skeleton data
  SMeshSkeletonData *pSkeletonData = skeletons.Get(szName);
  if (pSkeletonData == nullptr) return nullptr;
  SMeshAnimationData *pMeshAnimation = meshanims.Get(szName);
  // create skeleton
  CMeshSkeleton *pSkeleton = CreateObject<CMeshSkeleton>(ANIM_MESH_SKELETON);
  pSkeleton->Init(pSkeletonData);
  // create animation system
  CMeshAnimation *pAnim = CreateObject<CMeshAnimation>(ANIM_MESH_ANIMATION);
  pAnim->Init(pSkeleton, pMeshAnimation);
  return pAnim;
}

void CAnimationManager::Clear(const EClearMode eMode, const int nUsage, const int nAmount)
{
  if (eMode == CLEAR_ALL)
  {
    sprites.Clear();
    complexsprites.Clear();
    skeletons.Clear();
    meshanims.Clear();
  }
  else
  {
    sprites.ClearUnreferencedResources();
    complexsprites.ClearUnreferencedResources();
    skeletons.ClearUnreferencedResources();
    meshanims.ClearUnreferencedResources();
  }
}

int CAnimationManager::operator&(IStructureSaver &ss)
{
  sprites.Serialize(&ss);
  skeletons.Serialize(&ss);
  meshanims.Serialize(&ss);
  complexsprites.Serialize(&ss);
  return 0;
}