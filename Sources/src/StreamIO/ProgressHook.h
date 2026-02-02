#ifndef __PROGRESSHOOK_H__
#define __PROGRESSHOOK_H__

// ************************************************************************************************************************ //
// **
// ** progress indicator hook
// **
// **
// **
// ************************************************************************************************************************ //

interface IProgressHook : IRefCount
{
  // set total num steps
  virtual void SetNumSteps(int nRange, float fPercentage = 1.0f) = 0;
  // do one step
  virtual void Step() = 0;
  // recover during loading
  virtual void Recover() = 0;
  // get/set current position
  virtual void SetCurrPos(int nPos) = 0;
  virtual int GetCurrPos() const = 0;
  // initialization/deinitialization
  virtual void Stop() = 0;
};

interface IMovieProgressHook : IProgressHook
{
  enum EProgressType
  {
    PT_MAPGEN = 1,
    PT_LOAD = 2,
    PT_NEWMISSION = 3,
    PT_MINIMAP = 4,
    PT_TOTAL_ENCYCLOPEDIA_LOAD = 5,
    PT_CONNECTING_TO_SERVER = 6,
  };

  virtual void Init(EProgressType eType) = 0;
  virtual void Init(const std::string &szMovieName) = 0;
};

#endif // __PROGRESSHOOK_H__