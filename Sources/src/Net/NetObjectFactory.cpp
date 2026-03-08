#include "StdAfx.h"
#include "NetObjectFactory.h"
#include "NetA4.h"

static CNetObjectFactory theNetObjectFactory;

CNetObjectFactory::CNetObjectFactory()
{
  REGISTER_CLASS(this, NET_NET_DRIVER, NNet::CNetDriver);
  REGISTER_CLASS(this, NET_NODE_ADDRESS, NNet::CNodeAddressWrap);
}

// ************************************************************************************************************************ //
// **
// ** module descriptor and additional procedures
// **
// **
// **
// **
// ************************************************************************************************************************ //

static SModuleDescriptor theModuleDescriptor("Network", NET_NET, 0x0100, &theNetObjectFactory, nullptr);
const SModuleDescriptor * STDCALL GetModuleDescriptor() { return &theModuleDescriptor; }