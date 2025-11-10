#ifndef __CHEATS_H__
#define __CHEATS_H__

#pragma once

struct SCheats
{
  DECLARE_SERIALIZE;
  // Should bWarFog calculate fog?
  bool bWarFog;
  // for whom to calculate the fog
  int nPartyForWarFog;

  // load static 
  bool bLoadObjects;

  bool bTurnOffWarFog;
  bool bHistoryPlaying;
  //
  // whether this side is killable or not
  std::vector<BYTE> immortals;
  // Does this side kill the first time?
  std::vector<BYTE> firstShoot;

  bool bPasswordOK;

public:
  SCheats();

  void Init();
  void Clear() { Init(); }

  void SetWarFog(bool bWarFog);
  bool GetWarFog() const { return bWarFog; }

  void SetNPartyForWarFog(int nPartyForWarFog, bool bUnconditionly);
  const int GetNPartyForWarFog() const { return nPartyForWarFog; }

  void SetLoadObjects(bool bLoadObjects);
  bool GetLoadObjects() const { return bLoadObjects; }

  void SetTurnOffWarFog(bool bTurnOffWarFog);
  bool GetTurnOffWarFog() const { return bTurnOffWarFog; }

  void SetImmortals(int nParty, BYTE cValue);
  BYTE GetImmortals(const int nParty) const { return immortals[nParty]; }

  void SetFirstShoot(int nParty, BYTE cValue);
  BYTE GetFirstShoot(const int nParty) const { return firstShoot[nParty]; }

  void SetHistoryPlaying(bool _bHistoryPlaying) { bHistoryPlaying = _bHistoryPlaying; }
  bool IsHistoryPlaying() const { return bHistoryPlaying; }

  const unsigned long MakeCheckSum(const std::string &szPassword);
  void CheckPassword(const std::string &szPassword);

  bool IsPasswordOk() const { return bPasswordOK; }
};

#endif // __CHEATS_H__