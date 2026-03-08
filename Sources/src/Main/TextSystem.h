#pragma once

enum
{
  TEXT_BASE_VALUE = 0x100a0000,
  TEXT_MANAGER = TEXT_BASE_VALUE + 1,
  TEXT_STRING = TEXT_BASE_VALUE + 2,
  TEXT_DIALOG = TEXT_BASE_VALUE + 3,
};

interface IText : ISharedResource
{
  virtual const wchar_t * STDCALL GetString() const = 0;
  virtual const int STDCALL GetLength() const = 0;
  virtual void STDCALL SetText(const wchar_t*pszText) = 0;
  //
  virtual bool STDCALL IsChanged() const = 0;
  virtual void STDCALL ResetChanged() = 0;
};

interface ITextString : IText {};

interface ITextDialog : IText {};

interface ITextManager : ISharedManager
{
  // type ID
  enum { tidTypeID = TEXT_MANAGER };

  //
  virtual bool STDCALL AddTextFile(const char *pszFileName) = 0;
  virtual IText * STDCALL GetString(const char *pszKey) = 0;
  virtual IText * STDCALL GetDialog(const char *pszKey) = 0;
  virtual const char * STDCALL GetTextName(IText *pText) = 0;
};

IObjectLoader * STDCALL GetTextLoader();

