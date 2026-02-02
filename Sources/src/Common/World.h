#pragma once

#undef GetMessage

enum
{
  WCB_YOU_WIN = 0x00300001,
  WCB_YOU_LOOSE = 0x00300002,
  WCB_DRAW = 0x00300003,

  WCB_FORCE_DWORD = 0x7fffffff
};

// //////////////////////////////////////////////////////////// 
enum ESeason
{
  SEASON_SUMMER = 0,
  SEASON_WINTER = 1,
  SEASON_AFRIKA = 2
};

// //////////////////////////////////////////////////////////// 
interface IWorld : IRefCount
{
  // set climate-geographical zone settings
  virtual void SetSeason(int nSeason) = 0;
  // startup initialization (mission begin)
  virtual void Init(interface ISingleton *pSingleton) = 0;
  // remove all from all!!!
  virtual void Clear() = 0;
  // general world update from AI
  virtual void Update(const NTimer::STime &currTime) = 0;
  // message processing
  virtual bool ProcessMessage(const SGameMessage &msg) = 0;
  virtual bool GetMessage(SGameMessage *pMsg) = 0;

  virtual void GetAviationCircles(interface IUIMiniMap *pMinimap, NTimer::STime curTime) = 0;
};

// //////////////////////////////////////////////////////////// 
interface IWorldClient : IWorld
{
  // mission start
  virtual void Start() = 0;
  // selection
  virtual void Select(const CVec2 &vPos) = 0;
  virtual int Select(interface IVisObj **objects, int nNumObjects) = 0;
  virtual void ResetSelection(interface IVisObj *pObj = nullptr) = 0;
  virtual void PreSelect(interface IVisObj **objects, int nNumObjects) = 0;
  virtual void ResetPreSelection(interface IVisObj *pObj = nullptr) = 0;
  // manipulation
  virtual void MoveObject(interface IVisObj *pObj, const CVec3 &vPos) = 0;
  // action...
  virtual void OnMouseMove(const CVec2 &vPos, interface IUIElement *pElement) = 0;
  virtual void BeginAction(const SGameMessage &msg) {}
  virtual void DoAction(const SGameMessage &msg) = 0;
};

