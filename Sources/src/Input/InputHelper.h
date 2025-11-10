#ifndef __INPUTHELPER_H__
#define __INPUTHELPER_H__

namespace NInput
{

  // one event description => textual name <=> programmer defined ID
  struct SRegisterCommandEntry
  {
    const char *pszName;
    int nID;
  };

  // commad registrator - register command and unregister in destructor
  class CCommandRegistrator
  {
    const SRegisterCommandEntry *pRegisters;
    CPtr<IInput> pInput;
    //
    void RegisterCommands(const SRegisterCommandEntry *_pRegisters)
    {
      NI_ASSERT_T((pInput != nullptr) && (_pRegisters != nullptr), "can't register commands with empty input or registers");
      pRegisters = _pRegisters;
      for (; _pRegisters->pszName != nullptr; ++_pRegisters) pInput->RegisterCommand(_pRegisters->pszName, _pRegisters->nID);
    }

    void UnregisterCommands() { if (pRegisters) { for (; pRegisters->pszName != nullptr; ++pRegisters) pInput->UnRegisterCommand(pRegisters->pszName); } }

  public:
    CCommandRegistrator() : pRegisters(nullptr) {}
    CCommandRegistrator(IInput *_pInput, const SRegisterCommandEntry *_pRegisters) : pInput(_pInput) { RegisterCommands(_pRegisters); }
    ~CCommandRegistrator() { UnregisterCommands(); }
    //
    void Init(IInput *_pInput) { pInput = _pInput; }

    void Init(IInput *_pInput, const SRegisterCommandEntry *_pRegisters)
    {
      pInput = _pInput;
      (*this) = _pRegisters;
    }

    void operator=(const SRegisterCommandEntry *_pRegisters)
    {
      UnregisterCommands();
      RegisterCommands(_pRegisters);
    }
  };

};

#endif // __INPUTHELPER_H__