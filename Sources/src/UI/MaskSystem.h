#ifndef __MASK_SYSTEM_H__
#define __MASK_SYSTEM_H__

enum
{
  MASK_BASE_VALUE = 0x100c0000,
  MASK_MANAGER = MASK_BASE_VALUE + 1,
  UI_MASK = MASK_BASE_VALUE + 2,
};

interface IUIMask : ISharedResource
{
  virtual const CArray2D<BYTE> * GetMask() = 0;
};

interface IMaskManager : ISharedManager
{
  // type ID
  enum { tidTypeID = MASK_MANAGER };

  //
  virtual IUIMask * GetMask(const char *pszKey) = 0;
  virtual const char * GetMaskName(IUIMask *pMask) = 0;
};

IObjectLoader * GetMaskLoader();

#endif		// __MASK_MANAGER_H__