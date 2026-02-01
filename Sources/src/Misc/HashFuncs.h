#pragma once

struct SDefaultPtrHash
{
  size_t operator()(const void *pData) const { return reinterpret_cast<uintptr_t>(pData); }
};

struct SPtrHash
{
  template<class T>
  size_t operator()(const CPtr<T> &a) const { return reinterpret_cast<uintptr_t>(a.GetPtr()); }

  template<class T>
  size_t operator()(const CObj<T> &a) const { return reinterpret_cast<uintptr_t>(a.GetPtr()); }
};
