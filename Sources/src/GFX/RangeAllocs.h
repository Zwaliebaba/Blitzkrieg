#ifndef __RANGEALLOCS_H__
#define __RANGEALLOCS_H__

#pragma once

// possible allocator return values
enum EAllocVals
{
  EAV_SUCCESS,// successive allocation
  EAV_NOFREE,// not enough free space
  EAV_NOSIZE,// total size is not big enough
  EAV_BUSY// can't allocate range before another is not freed
};

// allocator reset values
enum EAllocReset
{
  EAR_HAVEDATA,// allocator still contain allocated ranges
  EAR_NODATA,// no ranges allocated. 
};

// allocator for sequential allocation of static ranges without the possibility of freeing
class CStaticAllocator
{
  int nSize;// total size
  int nCurrPos;// current allocation position
public:
  CStaticAllocator(int _nSize)
    : nSize(_nSize), nCurrPos(0) {}

  //
  static int GetOptimalSize(int nDesiredSize, int nElementSize) { return nDesiredSize; }
  //
  EAllocVals Allocate(int nAmount, SRangeLimits *pRange)
  {
    if (nAmount > nSize) return EAV_NOSIZE;
    if (nCurrPos + nAmount > nSize) return EAV_NOFREE;
    pRange->first = nCurrPos;
    pRange->second = nAmount;
    nCurrPos += nAmount;
    return EAV_SUCCESS;
  }

  void Free(const SRangeLimits &range) {}
  // dump trash into main memory.
  EAllocReset FrameReset() const { return nCurrPos != 0 ? EAR_HAVEDATA : EAR_NODATA; }
  EAllocReset OverflowReset() const { return nCurrPos != 0 ? EAR_HAVEDATA : EAR_NODATA; }
  // checking for the presence of a memory block
  EAllocVals HasBlock(int nAmount) const
  {
    if (nAmount > nSize) return EAV_NOSIZE;
    if (nCurrPos + nAmount > nSize) return EAV_NOFREE;
    return EAV_SUCCESS;
  }
};

// sequential allocation allocator with reset on overflow
// for temporal buffers with instantaneous lifetime
class CTempAllocator
{
  int nSize;// total size
  int nCurrPos;// current allocation position
  bool bRangeAllocated;

public:
  CTempAllocator(int _nSize)
    : nSize(_nSize), nCurrPos(0) {}

  //
  static int GetOptimalSize(int nDesiredSize, int nElementSize) { return Max(nDesiredSize, 65536 / nElementSize); }
  //
  EAllocVals Allocate(int nAmount, SRangeLimits *pRange)
  {
    if (bRangeAllocated) return EAV_BUSY;
    if (nAmount > nSize) return EAV_NOSIZE;
    if (nCurrPos + nAmount > nSize) return EAV_NOFREE;
    pRange->first = nCurrPos;
    pRange->second = nAmount;
    nCurrPos += nAmount;
    bRangeAllocated = true;
    return EAV_SUCCESS;
  }

  void Free(const SRangeLimits &range) { bRangeAllocated = false; }
  // dump trash into main memory.
  EAllocReset FrameReset() const { return bRangeAllocated ? EAR_HAVEDATA : EAR_NODATA; }

  EAllocReset OverflowReset()
  {
    if (bRangeAllocated) return EAR_HAVEDATA;
    nCurrPos = 0;
    return EAR_NODATA;
  }

  // checking for the presence of a memory block
  EAllocVals HasBlock(int nAmount) const
  {
    if (bRangeAllocated) return EAV_BUSY;
    if (nAmount > nSize) return EAV_NOSIZE;
    if (nCurrPos + nAmount > nSize) return EAV_NOFREE;
    return EAV_SUCCESS;
  }
};

// random allocation/deallocation allocator based on power-2 allocation algorithm
// supports overflow reset
class CPow2Allocator
{
  std::vector<std::list<SRangeLimits>> ranges;
  //
  int GetTotalSize() const { return static_cast<int>(1UL << ranges.size()); }
  void SetSize(int nSize);
  bool Clear();
  //
  bool GetRangeFromChunk(int nChunk, SRangeLimits *pRange);
  bool AllocateChunk(int nOptimalChunk, int nChunk, const SRangeLimits &range, SRangeLimits *pResult);
  void FreeLocal(int nStart, int nChunk);

public:
  CPow2Allocator(int nSize) { SetSize(nSize); }
  //
  static int GetOptimalSize(int nDesiredSize, int nElementSize)
  {
    DWORD dwDesiredPow2 = GetNextPow2(nDesiredSize);
    DWORD dwOptimalPow2 = GetNextPow2(65536 / nElementSize);
    return static_cast<int>(Max(dwDesiredPow2, dwOptimalPow2));
  }

  //
  EAllocVals Allocate(int nAmount, SRangeLimits *pRange);
  void Free(const SRangeLimits &range);
  //
  EAllocVals HasBlock(int nAmount) const;
  //
  //
  //
  int GetNumBlocks() const;
  int GetFree() const;
  void TestRanges() const;
};

#endif // __RANGEALLOCS_H__