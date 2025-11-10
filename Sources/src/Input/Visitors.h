#ifndef __VISITORS_H__
#define __VISITORS_H__

#pragma once

class CCombo;
struct SCommand;

class CSetBindSectionVisitor : public IInputVisitor
{
  const std::string szBindSection;
  std::hash_set<CCombo *, SDefaultPtrHash> combos;

public:
  CSetBindSectionVisitor(const std::string &_szBindSection) : szBindSection(_szBindSection) {}
  //
  bool STDCALL VisitControl(class CControl *pControl) override;
  bool STDCALL VisitCombo(CCombo *pCombo) override;
  bool STDCALL VisitBind(class CBind *pBind) override { return false; }
  bool STDCALL VisitCommand(struct SCommand *pCommand) override { return false; }
};

class CFindBindVisitor : public IInputVisitor
{
  struct SBindStats
  {
    int nCounter;
    CCombo *pCombo;
    //
    SBindStats() : nCounter(0), pCombo(nullptr) {}
  };

  //
  using CControlsMap = std::hash_map<const CControl *, std::list<CBind *>, SDefaultPtrHash>;
  using CBindsMap = std::hash_map<CBind *, SBindStats, SDefaultPtrHash>;
  const SCommand *pCommand2Find;// command to find
  const EInputBindActivationType eType;// command activation type to find
  CControlsMap controls;// controls of the bind to find
  CBindsMap binds;// how many times each bind was found. 
  // then this is the bind we've searched for
  //
  CControl *pCurrControl;// currently checked control
  CCombo *pCurrCombo;// currently checked combo
  // found bind and combo
  mutable CCombo *pFoundCombo;// 
  mutable CBind *pFoundBind;//
public:
  CFindBindVisitor(const SCommand *_pCommand, EInputBindActivationType _eType,
                   const std::vector<const CControl *> &_controls);
  //
  bool STDCALL VisitControl(class CControl *pControl) override;
  bool STDCALL VisitCombo(CCombo *pCombo) override;
  bool STDCALL VisitBind(class CBind *pBind) override;
  bool STDCALL VisitCommand(struct SCommand *pCommand) override;
  //
  void FinalCheck() const;

  CCombo *GetCombo() const
  {
    FinalCheck();
    return pFoundCombo;
  }

  CBind *GetBind() const
  {
    FinalCheck();
    return pFoundBind;
  }
};

class CRemoveComboVisitor : public IInputVisitor
{
  CPtr<CCombo> pCombo2Remove;

public:
  CRemoveComboVisitor(CCombo *_pCombo) : pCombo2Remove(_pCombo) {}
  //
  bool STDCALL VisitControl(class CControl *pControl) override;
  bool STDCALL VisitCombo(CCombo *pCombo) override;
  bool STDCALL VisitBind(class CBind *pBind) override { return true; }
  bool STDCALL VisitCommand(struct SCommand *pCommand) override { return false; }
};

#endif // __VISITORS_H__