#ifndef __CLIENTACKMANAGERINTERNAL_H__
#define __CLIENTACKMANAGERINTERNAL_H__

#include "iMission.h"
#include "../Misc/HashFuncs.h"
#include "../Common/Actions.h"

interface ITextManager;

class CClientAckManager : public IClientAckManager
{
  DECLARE_SERIALIZE;
  OBJECT_NORMAL_METHODS(CClientAckManager);

public:
  static int MIN_UNITS_TO_RUSH_ACK;
  static int MIN_UNITS_TO_TRAVEL_ACK;

private:
  enum EAcknowledgementColor
  {
    ACOL_INFORMATION,
  };

  enum ESoundPosition
  {
    ESP_FROM_INTERFACE,
    ESP_FROM_MAP,
  };

  //
  enum EAcknowledgementAdditionalSound
  {
    AAS_NONE,
    AAS_INFORMATION,
    AAS_TAKING_OFF,
  };

  //
  enum EAcknowledgementType
  {
    ACKT_POSITIVE,
    ACKT_NEGATIVE,
    ACKT_SELECTION,
    ACKT_NOTIFY,
    ACKT_BORED,
    ACKT_VOID,// doesn't play at all
  };

  //
  class CBoredUnitsContainer
  {
    DECLARE_SERIALIZE;
    using CBoredUnits = std::unordered_map<CPtr<IMOUnit>, bool, SPtrHash>;
    CBoredUnits boredUnits;
    int nCounter;// for optimization - size boredUnits
    NTimer::STime timeLastBored;// time for last bored sound
    void Copy(const CBoredUnitsContainer &cp);

  public:
    CBoredUnitsContainer();

    CBoredUnitsContainer operator=(const CBoredUnitsContainer &cp)
    {
      Copy(cp);
      return *this;
    }

    CBoredUnitsContainer(const CBoredUnitsContainer &cp) { Copy(cp); }

    int GetCount() const { return nCounter; }

    void AddUnit(interface IMOUnit *pUnit);
    void DelUnit(interface IMOUnit *pUnit);
    // if the time has come, it forces a random unit to send an Ack.
    // If ack was sent, then true;
    bool SendAck(NTimer::STime curTime,
                 EUnitAckType eBored,
                 IClientAckManager *pAckManager,
                 NTimer::STime timeInterval);
    void Clear();
  };

  // 
  struct SUnitAckInfo
  {
    EAcknowledgementType eType;
    std::string szTextKey;
    EAcknowledgementColor eColor;
    EAcknowledgementAdditionalSound eSound;
    ESoundPosition ePosition;
    int nTimeAfterPrevious;

    SUnitAckInfo() {}

    SUnitAckInfo(EAcknowledgementType _eType,
                 const char *_szKey,
                 EAcknowledgementColor _eColor,
                 EAcknowledgementAdditionalSound _eSound,
                 ESoundPosition _ePosition = ESP_FROM_INTERFACE,
                 int _nTimeAfterPrevious = 0)
      : eType(_eType), szTextKey(_szKey), eColor(_eColor), eSound(_eSound), ePosition(_ePosition), nTimeAfterPrevious(_nTimeAfterPrevious) {}
  };

  //
  struct SUnitAckInfoForLoad
  {
    std::string szAckName;
    std::string szTextKey;

    EAcknowledgementType eType;
    EAcknowledgementColor eColor;
    EAcknowledgementAdditionalSound eSound;
    ESoundPosition ePosition;
    int nTimeAfterPrevious;
    //
    SUnitAckInfoForLoad() {}

    SUnitAckInfoForLoad(const std::string &_szAckName,
                        EAcknowledgementType _eType,
                        const char *_szKey,
                        EAcknowledgementColor _eColor,
                        EAcknowledgementAdditionalSound _eSound,
                        int _nTimeAfterPrevious)
      : szAckName(_szAckName), szTextKey(_szKey), eType(_eType), eColor(_eColor), eSound(_eSound), nTimeAfterPrevious(_nTimeAfterPrevious) {}

    //
    int operator&(IDataTree &ss)
    {
      CTreeAccessor tree = &ss;
      tree.Add("AckName", &szAckName);
      tree.Add("AckType", &eType);
      tree.Add("TextKey", &szTextKey);
      tree.Add("ColorType", &eColor);
      tree.Add("SoundType", &eSound);
      tree.Add("Position", &ePosition);
      tree.Add("TimeAfterPrevious", &nTimeAfterPrevious);
      return 0;
    }

  };

  //
  struct SAck
  {
    DECLARE_SERIALIZE;

  public:
    int /* EUnitAckType */ eAck;
    std::string sound;
    int /* ESoundmixType */ eMixType;
    bool operator==(const SAck &ack) const { return eAck == ack.eAck; }
  };

  using CAcks = std::list<SAck>;

  //	
  struct SUnitAck
  {
    DECLARE_SERIALIZE;

  public:
    CAcks acks;// turn Ack who still lose
    WORD wSoundID;// this sound is playing
    int /* EUnitAckType */ eCurrentAck;// this Ack is playing now
    NTimer::STime timeRun;// time to lose AckPisitive
    SUnitAck()
      : wSoundID(0), eCurrentAck(-1), timeRun(0) {}
  };

  // acknowledgment of dead unit
  struct SDeathAck
  {
    DECLARE_SERIALIZE;

  public:
    std::string szSoundName;
    CVec3 vPos;
    NTimer::STime timeSinceStart;
    SDeathAck() {}

    SDeathAck(const CVec3 &_vPos, const std::string &_szSoundName, const unsigned int nTimeSinceStart)
      : szSoundName(_szSoundName), vPos(_vPos), timeSinceStart(nTimeSinceStart) {}
  };

  //
  using CUnitAcksInfo = std::unordered_map<int, SUnitAckInfo>;
  using CUnitAcksPresence = std::unordered_map<int, NTimer::STime>;
  using CUnitsAcks = std::unordered_map<CPtr<IMOUnit>, SUnitAck, SDefaultPtrHash>;
  using CDeathAcks = std::list<SDeathAck>;

  // to select a given type of ask
  class CAckPredicate
  {
    const EAcknowledgementType eType;
    CUnitAcksInfo &info;

  public:
    CAckPredicate(CUnitAcksInfo &info, EAcknowledgementType eType) : eType(eType), info(info) {}
    bool operator()(const SAck &a) { return eType == info[a.eAck].eType; }
  };

  // NTimer::STime timeNextBored;

  ITextManager *pTextManager;
  IConsoleBuffer *pConsoleBuffer;
  IGameTimer *pGameTimer;

  CPtr<IMOUnit> pLastSelected;
  int nSelectionCounter;

  // here is information about units registered in bored states
  using BoredUnits = std::unordered_map<int, CBoredUnitsContainer>;
  BoredUnits boredUnits;

  CUnitAcksPresence acksPresence;// the presence of a given Ack in the sound at a given moment
  CUnitsAcks unitAcks;// to store all current Acks
  CDeathAcks deathAcks;// for death acknowledgements;
  NTimer::STime timeLastDeath;

  // not serialized.
  CUnitAcksInfo acksInfo;// Ack data
  std::unordered_map<std::string, int> loadHelper;
  // constants
  int MIN_ACK_RADIUS;
  int MAX_ACK_RADIUS;
  int TIME_ACK_WAIT;
  int NUM_SELECTIONS_BEFORE_F_OFF;
  int ACK_BORED_INTERVAL;
  int ACK_BORED_INTERVAL_RANDOM;

  void InitConsts();

  const char *GetAdditionalSound(EAcknowledgementAdditionalSound eSound);
  DWORD GetMessageColor(enum EAcknowledgementColor eColor);
  void RegisterAck(SUnitAck *ack, NTimer::STime curTime);
  void UnregisterAck(SUnitAck *ack);

public:
  CClientAckManager();
  void STDCALL Init() override;
  void STDCALL Clear() override;
  bool STDCALL IsNegative(enum EUnitAckType eAck) override;

  void STDCALL AddDeathAcknowledgement(const CVec3 &vPos, const std::string &sound, unsigned int nTimeSinceStart) override;
  void STDCALL AddAcknowledgement(interface IMOUnit *pUnit, enum EUnitAckType eAck, const std::string &sound, int nSet, unsigned int nTimeSinceStart = 0) override;
  void STDCALL UnitDead(struct SMapObject *pUnit, interface IScene *pScene) override;
  void STDCALL Update(interface IScene *pScene) override;
  void STDCALL RegisterAsBored(EUnitAckType eBored, interface IMOUnit *pObject) override;
  void STDCALL UnRegisterAsBored(EUnitAckType eBored, interface IMOUnit *pObject) override;
};

#endif // __CLIENTACKMANAGERINTERNAL_H__