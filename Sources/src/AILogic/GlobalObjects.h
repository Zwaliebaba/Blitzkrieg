#ifndef __GLOBAL_OBJECTS_H__
#define __GLOBAL_OBJECTS_H__

#pragma once

namespace NGlobalObjects
{
  void Clear();
  void Serialize(SSChunkID idChunk, IStructureSaver &ss);
};

#endif // __GLOBAL_OBJECTS_H__