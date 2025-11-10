#ifndef __INPUTAPI_H__
#define __INPUTAPI_H__

using namespace NWin32Helper;

class CCombo;
struct SCommand;

#define AXIS_RANGE_VALUE 10000

// ************************************************************************************************************************ //
// **
// ** controls (different types)
// **
// **
// **
// ************************************************************************************************************************ //

// base control
class CControl : protected SControlDesc
{
  using CCombosList = std::list<CObj<CCombo>>;
  CCombosList combos;// list of combos to notify, then state changed
  float fPower;// control's power (value multiplier)
protected:
  void NotifyAllCombos(bool bActivated, DWORD time, int nParam);

public:
  CControl() : fPower(1.0f) {}
  CControl(const SControlDesc &desc) : SControlDesc(desc), fPower(1.0f) {}
  //
  const SControlDesc &GetDesc() const { return *this; }
  const int GetID() const { return nID; }
  // power
  void SetPower(const float _fPower) { fPower = _fPower; }
  const float GetPower() const { return fPower; }
  // change control's state. 
  virtual const bool ChangeState(int nNewState, DWORD time, int nParam, const CControl *pLastPressedKey) = 0;
  virtual const int GenerateRepeats(const DWORD time) { return 0; }
  // first time state initialization
  virtual void InitState(DWORD dwNewState) = 0;
  virtual const DWORD GetDataFromBuffer(const BYTE *pBuffer) = 0;
  // get current control's value
  virtual const float GetValue() const = 0;
  // is control active (pressed, rotated, moved in this pumping session, etc.)
  virtual const bool IsActive() const = 0;
  virtual void Deactivate(DWORD time) {}
  virtual const EControlType GetType() const = 0;
  //
  const std::list<CObj<CCombo>> &GetCombos() const { return combos; }
  void AddCombo(CCombo *pCombo);
  void RemoveCombo(CCombo *pCombo) { combos.remove(pCombo); }
  void SetBindSection(const std::string &szName);
  //
  bool Visit(IInputVisitor *pVisitor)
  {
    for (auto it = combos.begin(); it != combos.end(); ++it) { if (pVisitor->VisitCombo(*it) == false) return false; }
    return true;
  }
};

// key control
class CControlKey : public CControl
{
  CControl *pDBLCLK;// double click control
  CVec2 vLastPos;// last position, this key was pressed (changed only for dbl-clk generators)
  DWORD dwLastTimePressed;// last activation time
  DWORD dwLastRepeatedTime;// last repeat generation time
  int nRepeated;// repeated N times
  bool bPressed;// is this key pressed?
public:
  CControlKey()
    : pDBLCLK(nullptr), vLastPos(VNULL2), dwLastTimePressed(0), dwLastRepeatedTime(0), nRepeated(0), bPressed(false) {}

  CControlKey(const SControlDesc &desc)
    : CControl(desc), pDBLCLK(nullptr), vLastPos(VNULL2), dwLastTimePressed(0), dwLastRepeatedTime(0), nRepeated(0), bPressed(false) {}

  //
  void SetDoubleClickControl(CControl *_pDBLCLK) { pDBLCLK = _pDBLCLK; }
  const bool ChangeState(int nNewState, DWORD time, int nParam, const CControl *pLastPressedKey) override;
  const int GenerateRepeats(DWORD time) override;
  void InitState(const DWORD dwNewState) override { bPressed = (dwNewState & 0x80) != 0; }
  const DWORD GetDataFromBuffer(const BYTE *pBuffer) override { return *(pBuffer + (GetID() & 0xfff)); }
  const float GetValue() const override { return bPressed ? GetPower() : 0.0f; }
  const bool IsActive() const override { return bPressed; }
  const EControlType GetType() const override { return CONTROL_TYPE_KEY; }
};

// translation axis control (mouse)
class CControlAxis : public CControl
{
  int nAbsPos;// last absolute position
  int nOffset;// last offset
  bool bActive;// is this control active?
public:
  CControlAxis() : nAbsPos(0), nOffset(0), bActive(false) {}
  CControlAxis(const SControlDesc &desc) : CControl(desc), nAbsPos(0), nOffset(0), bActive(false) {}
  //
  const bool ChangeState(int nNewState, DWORD time, int nParam, const CControl *pLastPressedKey) override;
  void InitState(const DWORD dwNewState) override { nAbsPos = dwNewState; }
  const DWORD GetDataFromBuffer(const BYTE *pBuffer) override { return *((DWORD *) (pBuffer + (GetID() & 0xfff))); }
  const float GetValue() const override { return static_cast<float>(nOffset) * GetPower(); }
  const bool IsActive() const override { return bActive; }

  void Deactivate(DWORD time) override
  {
    bActive = false;
    NotifyAllCombos(false, time, 0);
  }

  const EControlType GetType() const override { return CONTROL_TYPE_AXIS; }
};

// rotational axis control (joystick)
class CControlAxisR : public CControl
{
  int nPos;// absolute position
public:
  CControlAxisR() : nPos(0) {}
  CControlAxisR(const SControlDesc &desc) : CControl(desc), nPos(0) {}
  //
  const bool ChangeState(int nNewState, DWORD time, int nParam, const CControl *pLastPressedKey) override;
  void InitState(const DWORD dwNewState) override { nPos = dwNewState; }
  const DWORD GetDataFromBuffer(const BYTE *pBuffer) override { return *((DWORD *) (pBuffer + (GetID() & 0xfff))); }
  const float GetValue() const override { return (static_cast<float>(nPos) / static_cast<float>(AXIS_RANGE_VALUE)) * GetPower(); }
  const bool IsActive() const override { return nPos != 0; }
  void Deactivate(DWORD time) override { if (!IsActive()) NotifyAllCombos(false, time, 0); }
  const EControlType GetType() const override { return CONTROL_TYPE_RAXIS; }
};

// ************************************************************************************************************************ //
// **
// **device
// **
// **
// **
// ************************************************************************************************************************ //

struct SDevice : SDeviceDesc
{
  std::vector<CControl *> controls;// all controls of this device
  com_ptr<IDirectInputDevice8> pDevice;// input device itself
  DWORD dwBufferSize;// buffer size for one-time data retrieving
  bool bNeedResync;// need to resync
  bool bEmulated;// is this device emulated, or we must get it from DInput?
  //
  SDevice() : dwBufferSize(0), bNeedResync(false), bEmulated(false) {}

  ~SDevice()
  {
    // delete all controls
    for (auto it = controls.begin(); it != controls.end(); ++it) delete (*it);
    // unacquire device
    if (pDevice && !bEmulated) pDevice->Unacquire();
  }
};

// ************************************************************************************************************************ //
// **
// ** main LAPI class
// **
// **
// **
// ************************************************************************************************************************ //

class CInputAPI : public CTRefCount<IInput>
{
  using CDevicesList = std::vector<SDevice>;
  using CCharsList = std::list<STextMessage>;
  using CMessagesList = std::list<SGameMessage>;
  using CControlIDsMap = std::hash_map<int, CControl *>;
  using CControlNamesMap = std::hash_map<std::string, CControl *>;

  struct SStoredControl
  {
    CControl *pControl;//
    bool bActive;//
    SStoredControl(CControl *_pControl) : pControl(_pControl), bActive(true) {}
  };

  using CStoredControlsList = std::list<SStoredControl>;
  //
  HWND hWindow;// window handle
  com_ptr<IDirectInput8> pInput;
  CDevicesList devices;// all found devices
  CControlIDsMap controlIDs;// control-ID-to-control map
  CControlNamesMap controlNames;// control-name-to-control map
  // 
  CStoredControlsList activecontrols;// controls, which was activated on the last pump
  const CControl *pLastControlKey;// last pressed control of type KEY
  // text input mode
  EInputTextMode eTextMode;// current text input mode
  int nCodePage;// code page
  // messages to send out of the input
  CCharsList chars;// char messages during text input
  CMessagesList messages;// messages, generated during bind parsing
  // emulated messages
  std::deque<DIDEVICEOBJECTDATA> emulatedMessages;
  //
  DWORD dwLastPumpingTime;// 
  //
  bool bInitialized;// is input already initialized
  bool bCoopLevelSet;// is cooperative level already set ?
  bool bFocusCaptured;// is focus captured by this app ?
  //
  void AddDevice(struct SDeviceEnumDesc *pDesc, int nID);
  SDevice *GetDevice(int nID);
  bool SetFocus(bool bFocus);
  bool SetCoopLevel();
  //
  void Convert2Text(const DIDEVICEOBJECTDATA *pData, int nNumElements);
  void EventCame(const DIDEVICEOBJECTDATA *pEvent, int nParam);
  void NotifyControl(CControl *pControl, DWORD dwData);
  //
  void AddActiveControl(CControl *pControl);
  //
  void GenerateRepeats(CControl *pControl);

protected:
  const CControl *GetControlByName(const std::string &szName) const
  {
    CControlNamesMap::const_iterator pos = controlNames.find(szName);
    return pos != controlNames.end() ? pos->second : nullptr;
  }

  const CControl *GetControlByID(const int nID) const
  {
    CControlIDsMap::const_iterator pos = controlIDs.find(nID);
    return pos != controlIDs.end() ? pos->second : nullptr;
  }

  bool PumpMessagesLocal(bool bFocus);
  bool GetDeviceState(SDevice &device, std::vector<BYTE> &data);
  // configuring
  bool AddDoubleClick(const std::string &szControlName);
  bool SetPower(const std::string &szControlName, float fPower);
  //
  void VisitControls(IInputVisitor *pVisitor);

public:
  CInputAPI();
  ~CInputAPI() override;
  //
  bool STDCALL Init(HWND _hWindow) override;
  bool STDCALL Done() override;
  // emulation
  void STDCALL SetDeviceEmulationStatus(enum EDeviceType eDeviceType, bool bEmulate) override;
  bool STDCALL IsEmulated(enum EDeviceType eDeviceType) const override;
  void STDCALL EmulateInput(enum EDeviceType eDeviceType, int nControlID,
                            int nValue, DWORD time, int nParam) override;
  // messages
  void STDCALL PumpMessages(const bool bFocus) override { PumpMessagesLocal(bFocus); }
  void STDCALL AddMessage(const SGameMessage &msg) override { messages.push_back(msg); }
  bool STDCALL GetMessage(SGameMessage *pMsg) override;
  bool STDCALL GetTextMessage(STextMessage *pMsg) override;
  void STDCALL ClearMessages() override;
  // text typing mode
  EInputTextMode STDCALL GetTextMode() override { return eTextMode; }

  bool STDCALL SetTextMode(const EInputTextMode eMode) override
  {
    eTextMode = eMode == INPUT_TEXT_MODE_TEXTONLY ? INPUT_TEXT_MODE_SYSKEYS : eMode;
    return true;
  }

  void STDCALL SetCodePage(const int _nCodePage) override { nCodePage = _nCodePage; }
  //
  //
  const DWORD GetCurrentTime() const { return dwLastPumpingTime; }
  void AddEventLocal(const int nEventID, const int nParam) { messages.push_back(SGameMessage(nEventID, nParam)); }
  EInputTextMode GetTextModeLocal() const { return eTextMode; }
  //
  int STDCALL operator&(IStructureSaver &ss) override;
};

#endif // __INPUTAPI_H__