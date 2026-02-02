#ifndef __SQUADVISOBJ_H__
#define __SQUADVISOBJ_H__

#pragma once

class CSquadVisObj : public CTRefCount<ISquadVisObj>
{
  OBJECT_SERVICE_METHODS(CSquadVisObj);
  DECLARE_SERIALIZE;
  //
  std::vector<SData> units;// units in this squad
  CPtr<IGFXTexture> pIcon;// icon texture
  CVec2 vPos;// position on the screen
  bool bSelected;// is this squad selected
  //
  CSquadVisObj();

public:
  void SetIcon(IGFXTexture *_pIcon) { pIcon = _pIcon; }
  // update object
  bool Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  // drawing
  bool Draw(interface IGFX *pGFX) override;
  // visiting
  void Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
  // set screen position
  void SetPosition(const CVec2 &_vPos) override { vPos = _vPos; }
  // data update
  bool UpdateData(SData *pObjects, int nNumObjects) override;
  // toggle selection - select/unselect
  bool ToggleSelection() override
  {
    bSelected = !bSelected;
    return bSelected;
  }
};

#endif // __SQUADVISOBJ_H__