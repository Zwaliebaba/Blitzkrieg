#ifndef __INPUTBIND_H__
#define __INPUTBIND_H__

#pragma once

class CInputBind : public CTRefCount<IInputBind>
{
  OBJECT_SERVICE_METHODS(CInputBind);
  //
  std::string szCommand;
  EInputBindActivationType eType;
  std::vector<std::string> controls;

public:
  // clear all bind data
  void Clear() override
  {
    szCommand.clear();
    controls.clear();
  }

  // setup data
  // add control to bind
  void STDCALL AddControl(const char *pszControl) override { controls.push_back(pszControl); }
  // set command to bind
  void STDCALL SetCommand(const char *pszCommand, const EInputBindActivationType _eType) override
  {
    szCommand = pszCommand;
    eType = _eType;
  }

  // retrieve data
  //
  // retrieve number of controls in this bind
  int STDCALL GetNumControls() const override { return controls.size(); }
  // retrieve control name
  const char * STDCALL GetControl(const int nIndex) const override { return controls[nIndex].c_str(); }
  // retrieve command name
  const char * STDCALL GetCommand() const override { return szCommand.c_str(); }
  // retrieve bind activation type
  EInputBindActivationType STDCALL GetActivationType() const override { return eType; }
};

#endif // __INPUTBIND_H__