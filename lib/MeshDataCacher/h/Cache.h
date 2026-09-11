// Cache.h: interface for the CCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHE_H__D10ADD51_A8FE_4777_9A16_0478E557A86B__INCLUDED_)
#define AFX_CACHE_H__D10ADD51_A8FE_4777_9A16_0478E557A86B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

namespace mdc {

class CDataCell;
class CMeshDataCacher;
class CMeshDataCacherTestAccessor;


class CCache  
{
  friend class CMeshDataCacherTestAccessor;

  class CCellList
  {
    CDataCell &m_cell;
    CCellList *m_prev;
    CCellList *m_next;

  public:
    CCellList(CCellList *prev, CCellList *next, CDataCell &cell);
    virtual ~CCellList();

    CCellList *Next();
    CCellList *Prev();

    CDataCell &Cell();

    void Detach();
    void InsertBefore(CCellList *list);
  };

  const CMeshDataCacher& m_meshdatacacher;
  int m_nLength;
  int m_nMaxLength;
  int m_nMaxLengthDefined;

  int m_nSuccessiveFailedWrites;

  CCellList *m_first;
  CCellList *m_last;

  int m_nOldMaxLength;
  int m_nCriticalSectionSize;
  std::vector<CDataCell *> m_vcCriticalSection;

public:
  CDataCell *Cell(int nElement, int nColumn, bool bReading);
private:
  CDataCell *CellImpl(int nElement, int nColumn, bool bReading);

public:
  // use length = 0 for infinite cache size (no swapping to disk)
  CCache(const CMeshDataCacher &cacher, int length = 0);
  virtual ~CCache();

  void Resize(int length);

  int MaxLength() const;
  int MaxLengthDefined() const;

  void GetChecksum(int nColumn, unsigned char *buffer);
  void SetChecksum(int nColumn, unsigned char *buffer);
  bool ChecksumOK(int nColumn, int *nStatus = 0);

  void Flush();

  const double *Value(int nElement, int nColumn, int nNode = 0); // don't access beyond the element's number of nodes !
  void Value(int nElement, int nColumn, int nNode, double val); // to set the nodal values for the elements

  bool StartCriticalSection(int nColumn, int nNumber); // preload cells nColumn ... nColumn + nNumber - 1; additive
  void EndCriticalSection(); // free them; only needs to be called once
};

}

#endif // !defined(AFX_CACHE_H__D10ADD51_A8FE_4777_9A16_0478E557A86B__INCLUDED_)
