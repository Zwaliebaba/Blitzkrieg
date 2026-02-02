#ifndef __TEXT_OBJECT_H__
#define __TEXT_OBJECT_H__

#pragma once

#include "TextSystem.h"

class CTextString : public ITextString
{
  OBJECT_COMPLETE_METHODS(CTextString);
  SHARED_RESOURCE_METHODS(nRefData.a, "Text.String");
  DECLARE_SERIALIZE;
  //
  std::wstring szString;
  bool bChanged;

public:
  void SwapData(ISharedResource *pResource) override;
  const wchar_t * GetString() const override { return szString.c_str(); }
  const int GetLength() const override { return szString.size(); }

  void SetText(const wchar_t *pszText) override
  {
    szString = pszText == nullptr ? L"" : pszText;
    bChanged = true;
  }

  //
  bool IsChanged() const override { return bChanged; }
  void ResetChanged() override { bChanged = false; }
  // internal container clearing
  void ClearInternalContainer() override {}
  bool Load(const bool bPreLoad = false) override { return false; }
};

class CTextDialog : public ITextDialog
{
  OBJECT_COMPLETE_METHODS(CTextDialog);
  SHARED_RESOURCE_METHODS(nRefData.a, "Text.Dialog");
  //
  std::wstring szString;
  bool bChanged;

public:
  void SwapData(ISharedResource *pResource) override;
  const wchar_t * GetString() const override { return szString.c_str(); };
  const int GetLength() const override { return szString.size(); }
  void SetText(const wchar_t *pszText) override;
  //
  bool IsChanged() const override { return bChanged; }
  void ResetChanged() override { bChanged = false; }
  // internal container clearing
  void ClearInternalContainer() override {}
  bool Load(bool bPreLoad = false) override;
};

#endif		// __TEXT_OBJECT_H__