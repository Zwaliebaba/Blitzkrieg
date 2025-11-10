#ifndef __NETLOWEST_H_
#define __NETLOWEST_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>
#include <string>
// //////////
class CMemoryStream;

// //////////
namespace NNet
{
  // //////////
  class CNodeAddress
  {
    sockaddr addr;

  public:
    CNodeAddress() { memset(&addr, 0, sizeof(addr)); }

    void Clear() { memset(&addr, 0, sizeof(addr)); }
    //
    bool SetInetName(const char *pszHost, int nDefaultPort);
    std::string GetName(bool bResolve = true) const;
    std::string GetFastName() const { return GetName(false); }
    //
    bool SameIP(const CNodeAddress &a) const { return memcmp(((char *) &a.addr) + 4, ((char *) &addr) + 4, 4) == 0; }
    unsigned int GetIP() const { return ((unsigned int *) (&addr))[1]; }

    bool operator ==(const CNodeAddress &a) const { return memcmp(&addr, &a.addr, sizeof(addr)) == 0; }
    bool operator !=(const CNodeAddress &a) const { return memcmp(&addr, &a.addr, sizeof(addr)) != 0; }

    sockaddr *GetSockAddr() { return &addr; }

    friend class CLinksManager;
  };

  // //////////
  constexpr int N_MAX_HOST_HOMES = 4;

  class CNodeAddressSet
  {
    unsigned short nPort;
    int ips[N_MAX_HOST_HOMES];

  public:
    void Clear() { Zero(*this); }
    bool GetAddress(int n, CNodeAddress *pRes) const;
    friend class CLinksManager;
  };

  // //////////
  class CNodeAddressWrap : public INetNodeAddress
  {
    OBJECT_NORMAL_METHODS(CNodeAddressWrap);
    //
    CNodeAddress address;
    mutable std::string szTempString;
    //
    void STDCALL Clear() override { address.Clear(); }
    //
    bool STDCALL SetInetName(const char *pszHost, int nDefaultPort) override { return address.SetInetName(pszHost, nDefaultPort); }

    const char * STDCALL GetName(bool bResolve = true) const override
    {
      szTempString = address.GetName(bResolve);
      return szTempString.c_str();
    }

    const char * STDCALL GetFastName() const override
    {
      szTempString = address.GetFastName();
      return szTempString.c_str();
    }

    //
    bool STDCALL IsSameIP(const INetNodeAddress *pAddress) const override { return address.SameIP(static_cast<const CNodeAddressWrap *>(pAddress)->GetCNodeAddress()); }
    unsigned int STDCALL GetIP() const override { return address.GetIP(); }
    //
    sockaddr * STDCALL GetSockAddr() override { return address.GetSockAddr(); }

  public:
    const CNodeAddress &GetCNodeAddress() const { return address; }
    CNodeAddress &GetCNodeAddress() { return address; }
  };

  // //////////
  // abstraction from messaging level
  class CLinksManager
  {
    SOCKET s;
    CNodeAddress broadcastAddr;

  public:
    CLinksManager();
    ~CLinksManager();
    bool Start(int nPort);
    void Finish();
    bool MakeBroadcastAddr(CNodeAddress *pRes, int nPort) const;
    bool IsLocalAddr(const CNodeAddress &test) const;
    bool Send(const CNodeAddress &dst, CMemoryStream &pkt) const;
    bool Recv(CNodeAddress *pSrc, CMemoryStream *pPkt) const;
    SOCKET GetSocket() const;
    bool GetSelfAddress(CNodeAddressSet *pRes) const;
  };

  // //////////
}

// //////////
#endif