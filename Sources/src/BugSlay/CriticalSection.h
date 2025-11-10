/* ----------------------------------------------------------------------
   */
/* ----------------------------------------------------------------------
     */

#ifndef _CRITICALSECTION_H
#define _CRITICALSECTION_H

/* ////////////////////////////////////////////////////////////////////
                        */
class CUseCriticalSection;

class CCriticalSection
{
public :
  CCriticalSection(void) { InitializeCriticalSection(&m_CritSec); }
  ~CCriticalSection() { DeleteCriticalSection(&m_CritSec); }

  friend CUseCriticalSection;

private :
  CRITICAL_SECTION m_CritSec;
};

/* ////////////////////////////////////////////////////////////////////
                      */
// lint -e1704
class CUseCriticalSection
{
public :
  CUseCriticalSection(const CCriticalSection &cs)
  {
    m_cs = &cs;
    EnterCriticalSection((LPCRITICAL_SECTION) &(m_cs->m_CritSec));
  }

  ~CUseCriticalSection()
  {
    LeaveCriticalSection((LPCRITICAL_SECTION) &(m_cs->m_CritSec));
    m_cs = nullptr;
  }

private :
  CUseCriticalSection(void) { m_cs = nullptr; }
  const CCriticalSection *m_cs;
};

// lint +e1704

#endif      // _CRITICALSECTION_H