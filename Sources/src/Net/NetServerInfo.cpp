#include "StdAfx.h"
#include "NetServerInfo.h"
#include "NetDriverConsts.h"

// ///////////
namespace NNet
{

  CServerInfoSupport::CServerInfoSupport(APPLICATION_ID _nApplicationID)
    : fTime(0), fRequestDelay(0), applicationID(_nApplicationID), bDoReply(false) {}

  void CServerInfoSupport::Step(float fDeltaTime)
  {
    fRequestDelay -= fDeltaTime;
    fTime += fDeltaTime;
    // remove outdated information about servers
    for (auto i = servers.begin(); i != servers.end();)
    {
      if (i->fValidTimeLeft > 0)
      {
        i->fValidTimeLeft -= fDeltaTime;
        if (i->fValidTimeLeft <= 0)
        {
          /* if ( book.HasAddr( (*i)->addr ) )
           */
          i = servers.erase(i);
        }
        else ++i;
      }
      else ++i;
    }
  }

  void CServerInfoSupport::ReplyServerInfoRequest(CBitStream &bits, CBitStream *pDstBits)
  {
    float fReqTime;
    bits.Read(fReqTime);
    pDstBits->Write(fReqTime);
    pDstBits->Write(applicationID);
    short nSize = serverInfo.GetSize();
    pDstBits->Write(nSize);
    pDstBits->Write(serverInfo.GetBuffer(), serverInfo.GetSize());
  }

  CServerInfoSupport::SServerInfo &CServerInfoSupport::GetInfo(const CNodeAddress &addr)
  {
    for (auto i = servers.begin(); i != servers.end(); ++i) { if (i->addr == addr) return *i; }
    servers.push_back();
    servers.size();// guard from optimization bug
    SServerInfo &b = servers.back();
    b.addr = addr;
    return b;
  }

  void CServerInfoSupport::ProcessServerInfo(const CNodeAddress &addr, CBitStream &bits)
  {
    APPLICATION_ID appID;
    float fReqSent;
    //
    bits.Read(fReqSent);
    bits.Read(appID);
    // check that application is correct
    if ((appID & 0xFFFFFF00) != (applicationID & 0xFFFFFF00)) return;

    SServerInfo &info = GetInfo(addr);
    info.bWrongVersion = appID != applicationID;
    info.fPing = fTime - fReqSent;
    short nSize;
    bits.Read(nSize);
    info.info.SetSizeDiscard(nSize);
    bits.Read(info.info.GetBufferForWrite(), nSize);
    info.fValidTimeLeft = SNetDriverConsts::FP_SERVER_LIST_TIMEOUT;
    /* if ( links.IsLocalAddr( addr ) )
     */
    //
  }

  void CServerInfoSupport::WriteRequest(CBitStream *pBits) { pBits->Write(fTime); }

  bool CServerInfoSupport::CanSendRequest(const CNodeAddress &broadcast, std::vector<CNodeAddress> *pDest)
  {
    if (fRequestDelay <= 0)
    {
      fRequestDelay = 1;
      pDest->resize(0);
      pDest->push_back(broadcast);
      return true;
    }
    return false;
  }

}