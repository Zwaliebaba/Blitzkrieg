#ifndef __UICONSTS_H__
#define __UICONSTS_H__

#pragma once
#include "../Main/GameStats.h"

class CUIConsts
{
public:
  static std::string GetPartyNameByNumber(int nCampaign);

  static const wchar_t *GetLocalPartyName(const char *pszPartyKey);
  static const wchar_t *GetGamesListTitle(enum EMultiplayerConnectionType eType);

  static std::string ConstructOptionKey(const char *pszOptionName, const char *pszSelectionName);
  static std::string CreateTexturePathFromMapPath(const char *pszMapPath);
  static void CreateDescription(const struct SChapterStats::SMission *pStats, std::wstring *pDescription, bool bNeedBonuses);

  static const wchar_t *GetMapTypeString(int /* SQuickLoadMapInfo::EMultiplayerMapType */ nGameType);
  static std::string GetCampaignNameAddition();

};

#endif // __UICONSTS_H__