#ifndef __ACKMANAGER_H__
#define __ACKMANAGER_H__

// //////////////////////////////////////////////////////////// 
#include "../Common/Actions.h"
#include "../misc/HashFuncs.h"
#include "AIHashFuncs.h"
// //////////////////////////////////////////////////////////// 
class CAIUnit;

// //////////////////////////////////////////////////////////// 
class CAckManager
{
  DECLARE_SERIALIZE;

  using CUnitBoredPresence = std::pair<CPtr<CAIUnit>, bool>;
  using CBoredPresence = std::unordered_map<int/* unit unique ID */, CUnitBoredPresence>;
  using CAckTypeBoredPrecence = std::unordered_map<int, CBoredPresence>;
  CAckTypeBoredPrecence bored;

  using CAcknowledgments = std::vector<SAIAcknowledgment>;
  CAcknowledgments acknowledgements;// accumulates Acknolegments coming from AI

  void AddAcknowledgment(const SAIAcknowledgment &ack);

public:
  CAckManager();
  virtual ~CAckManager();
  // issuing Acknowledgments to the client
  void UpdateAcknowledgments(SAIAcknowledgment **pAckBuffer, int *pnLen);
  // issuing Bored Acknowledgments to the client
  void UpdateAcknowledgments(SAIBoredAcknowledgement **pAckBuffer, int *pnLen);

  // for BORED acknowledgements
  void RegisterAsBored(EUnitAckType eAck, class CAIUnit *pObject);
  void UnRegisterAsBored(EUnitAckType eAck, class CAIUnit *pObject);


  void AddAcknowledgment(EUnitAckType eAck, struct IRefCount *pObject, int nSet = 0);
  void Clear();

  void UnitDead(class CAIUnit *pObject);
};
#endif // __ACKMANAGER_H__