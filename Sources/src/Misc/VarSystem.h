#ifndef __VARSSYSTEM_H__
#define __VARSSYSTEM_H__

#pragma once

interface IVarIterator : IRefCount
{
  // go to the next var
  virtual bool Next() = 0;
  // was all vars already iterated?
  virtual bool IsEnd() const = 0;
  // get current var
  virtual bool Get(variant_t *pVarName, variant_t *pVar) const = 0;
};

interface IVarSystem : IRefCount
{
  // get/set variable by name
  virtual bool Get(const std::string &szVarName, variant_t *pVar) const = 0;
  virtual bool Set(const std::string &szVarName, const variant_t &var) = 0;
  // remove variable by name or by match
  virtual bool Remove(const std::string &szVarName) = 0;
  virtual bool RemoveByMatch(const std::string &szVarMatch) = 0;
  // include/exclude variable by match to serialize
  virtual bool ChangeSerialize(const std::string &szVarMatch, bool bInclude) = 0;
  virtual bool IsChanged() const = 0;
};

#endif // __VARSSYSTEM_H__