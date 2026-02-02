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
  virtual const wchar_t * GetString() const = 0;
  virtual const int GetLength() const = 0;
  virtual void SetText(const wchar_t*pszText) = 0;
  //
  virtual bool IsChanged() const = 0;
  virtual void ResetChanged() = 0;
};

interface ITextString : IText {};

interface ITextDialog : IText {};

interface ITextManager : ISharedManager
{
  // type ID
  enum { tidTypeID = TEXT_MANAGER };

  //
  virtual bool AddTextFile(const char *pszFileName) = 0;
  virtual IText * GetString(const char *pszKey) = 0;
  virtual IText * GetDialog(const char *pszKey) = 0;
  virtual const char * GetTextName(IText *pText) = 0;
};

IObjectLoader * GetTextLoader();

