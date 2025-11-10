#ifndef __UICOLORTEXTSCROLL_H__
#define __UICOLORTEXTSCROLL_H__

#pragma once

#include "UIScrollText.h"

using CVisibleString = std::pair<CPtr<ITextDialog>, CPtr<IGFXText>>;

class CUIColorTextScroll : public CUIScrollTextBox
{
  DECLARE_SERIALIZE;

public:
  // one entry;
  // displays in this way
  // 
  class CColorTextEntry : public IRefCount
  {
    DECLARE_SERIALIZE;
    OBJECT_COMPLETE_METHODS(CColorTextEntry);
    int nHeight;// height of this item
    int nY;// Y position of this item
    DWORD dwCaptionColor;// color of this item
    DWORD dwEntryColor;// color of this item
    std::wstring szCaptionString;
    int entryStartX;// position of entry start.
    std::vector<std::wstring> entryStrings;
    CVisibleString entry;
    CVisibleString caption;

    CVisibleString CreateString(const std::wstring &szSource, int nWidth, DWORD dwColor, int nRedLine);

  public:
    CColorTextEntry() {}
    CColorTextEntry(const wchar_t *pszCaptionText, DWORD dwCaptionColor,
                    const wchar_t *pszEntryText, DWORD dwEntryColor,
                    int nY, int nWidth);

    const int GetSizeY() const { return nHeight; }
    int Get1LineHeight() const;
    void Visit(interface ISceneVisitor *pVisitor, const CTRect<float> &border, int nYOffset);
  };

private:
  using CColorPair = std::pair<DWORD, DWORD>;
  std::vector<CColorPair> colors;// color indexes (CAPTION,ENTRY)

  using CTextEntrys = std::vector<CPtr<CColorTextEntry>>;
  CTextEntrys textEntrys;
  int nCurrentYSize;

protected:
  void RepositionText() override {}

public:
  CUIColorTextScroll() : nCurrentYSize(0) {}
  ~CUIColorTextScroll() override {}

  virtual void STDCALL AppendMessage(const WORD *pszCaption, const WORD *pszMessage,
                                     enum IUIColorTextScroll::EColorEntrys color);

  void STDCALL SetWindowText(int nState, const WORD *pszText) override;

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
};

class CUIColorTextScrollBridge : public IUIColorTextScroll, public CUIColorTextScroll
{
  OBJECT_NORMAL_METHODS(CUIColorTextScrollBridge);
  DECLARE_SUPER(CUIColorTextScroll);
  DEFINE_UICONTAINER_BRIDGE;

  void STDCALL AppendMessage(const WORD *pszCaption, const WORD *pszMessage,
                             const enum EColorEntrys color = E_COLOR_DEFAULT) override { CSuper::AppendMessage(pszCaption, pszMessage, color); }
};

#endif // __UICOLORTEXTSCROLL_H__