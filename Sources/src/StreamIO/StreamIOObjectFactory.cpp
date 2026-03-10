#include "pch.h"

#include "StreamIOObjectFactory.h"
#include "StreamIOTypes.h"

#include "MemFileSystem.h"
#include "RandomGenInternal.h"

CStreamIOObjectFactory theStreamIOObjectFactory;

CStreamIOObjectFactory::CStreamIOObjectFactory()
{
  REGISTER_CLASS(this, STREAMIO_MEMORY_STREAM, CMemFileStream);
  REGISTER_CLASS(this, STREAMIO_RANDOM_GEN_SEED, CRandomGenSeed);
}

// ************************************************************************************************************************ //
// **
// ** module descriptor and additional procedures
// **
// **
// **
// **
// ************************************************************************************************************************ //

IObjectFactory * STDCALL GetStreamIOObjectFactory() { return &theStreamIOObjectFactory; }