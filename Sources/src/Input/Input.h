#ifndef __INPUT_H__
#define __INPUT_H__

#pragma once

enum
{
  INPUT_BASE_VALUE = 0x10040000,
  INPUT_INPUT = INPUT_BASE_VALUE + 1,
  INPUT_BIND = INPUT_BASE_VALUE + 2,
  INPUT_SLIDER = INPUT_BASE_VALUE + 3,
};

enum EInputTextMode
{
  INPUT_TEXT_MODE_NOTEXT = 0,// don't process any text inputs
  INPUT_TEXT_MODE_TEXTONLY = 1,// convert all keyboard input to the text - don't allow any key event processing
  INPUT_TEXT_MODE_SYSKEYS = 2,// convert to text and allow special 'system' keys processing
  INPUT_TEXT_MODE_ALLKEYS = 3// allow all keys processing parallel with text conversion
};

enum EInputBindActivationType
{
  INPUT_BIND_ACTIVATION_TYPE_UNKNOWN = 0,// unknown activation type
  INPUT_BIND_ACTIVATION_TYPE_EVENT_DOWN = 1,// 'combo formed' event
  INPUT_BIND_ACTIVATION_TYPE_EVENT_UP = 2,// 'combo destroyed' event
  INPUT_BIND_ACTIVATION_TYPE_SLIDER_PLUS = 3,// add to slider value
  INPUT_BIND_ACTIVATION_TYPE_SLIDER_MINUS = 4// subtract from slider value
};

// ************************************************************************************************************************ //
// **
// ** input bind to setup and retrieve binds data
// **
// **
// **
// ************************************************************************************************************************ //

interface IInputBind : IRefCount
{
  // clear all bind data
  virtual void Clear() = 0;
  // setup data
  // add control to bind
  virtual void STDCALL AddControl(const char *pszControl) = 0;
  // set command to bind
  virtual void STDCALL SetCommand(const char *pszCommand, EInputBindActivationType eType) = 0;

  // retrieve data
  //
  // retrieve number of controls in this bind
  virtual int STDCALL GetNumControls() const = 0;
  // retrieve control name
  virtual const char * STDCALL GetControl(int nIndex) const = 0;
  // retrieve command name
  virtual const char * STDCALL GetCommand() const = 0;
  // retrieve bind activation type
  virtual EInputBindActivationType STDCALL GetActivationType() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** input slider - to work with SLIDER activation type binds
// **
// **
// **
// ************************************************************************************************************************ //

interface IInputSlider : IRefCount
{
  virtual float STDCALL GetDelta() = 0;
  // virtual float STDCALL GetSpeed() const = 0;
  virtual void STDCALL Reset() = 0;
};

// ************************************************************************************************************************ //
// **
// ** main input interface
// **
// **
// **
// ************************************************************************************************************************ //

interface IInput : IRefCount
{
  // type ID
  enum { tidTypeID = INPUT_INPUT };

  //
  virtual bool STDCALL Init(HWND hWnd) = 0;
  virtual bool STDCALL Done() = 0;
  // serialization
  int STDCALL operator&(IStructureSaver &ss) override = 0;
  virtual bool STDCALL SerializeConfig(IDataTree *pSS) = 0;
  virtual bool STDCALL IsChanged() const = 0;
  virtual void STDCALL Repair(IDataTree *pSS, bool bToDefault) = 0;
  // emulation
  virtual void STDCALL SetDeviceEmulationStatus(enum EDeviceType eDeviceType, bool bEmulate) = 0;
  virtual bool STDCALL IsEmulated(enum EDeviceType eDeviceType) const = 0;
  virtual void STDCALL EmulateInput(enum EDeviceType eDeviceType, int nControlID,
                                    int nValue, DWORD time, int nParam) = 0;
  // messages and sliders
  virtual void STDCALL PumpMessages(bool bFocus) = 0;
  virtual void STDCALL AddMessage(const SGameMessage &msg) = 0;
  virtual bool STDCALL GetMessage(SGameMessage *pMsg) = 0;
  virtual bool STDCALL GetTextMessage(STextMessage *pMsg) = 0;
  virtual IInputSlider * STDCALL CreateSlider(const char *pszName, float fPower = 1.0f) = 0;
  virtual void STDCALL ClearMessages() = 0;
  // binds. 
  virtual void STDCALL SetBindSection(const char *pszSectionName) = 0;
  virtual void STDCALL AddBind(const IInputBind *pBind) = 0;
  virtual void STDCALL RemoveBind(const IInputBind *pBind) = 0;
  // commands registration
  virtual void STDCALL RegisterCommand(const char *pszName, int nEventID) = 0;
  virtual void STDCALL UnRegisterCommand(const char *pszName) = 0;
  // text typing mode
  virtual EInputTextMode STDCALL GetTextMode() = 0;
  virtual bool STDCALL SetTextMode(EInputTextMode eMode) = 0;
  virtual void STDCALL SetCodePage(int nCodePage) = 0;
};

#endif // __INPUT_H__