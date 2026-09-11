// Cache.cpp: implementation of the CCache class.
//
//////////////////////////////////////////////////////////////////////

#include "Cache.h"
#include "DataCell.h"
#include "MeshDataCacher.h"
#include "GlobalMessage.h"
#include <tbb/spin_rw_mutex.h>

namespace {
tbb::spin_rw_mutex myGlobalMutex;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace mdc {

CCache::CCache(const CMeshDataCacher &cacher, int length)
  : m_meshdatacacher(cacher)
  , m_nLength(0)
  ,	m_nMaxLength(length)
  , m_nMaxLengthDefined(length)
  , m_nSuccessiveFailedWrites(0)
  ,	m_first(0)
  ,	m_last(0)
  , m_nCriticalSectionSize(0)
{
}

CCache::~CCache()
{
  CCellList *pl = m_first;
  while(pl && pl->Next())
  {
    // list deletes cell
    assert(pl->Next()->Prev() == pl);
    pl = pl->Next();
    delete pl->Prev();
  }

  assert(pl == m_last); // might both be NULL

  if(m_last)
    delete m_last;

  // TODO: clear cache directory
}

int CCache::MaxLength() const
{
  return m_nMaxLength;
}

int CCache::MaxLengthDefined() const
{
  return m_nMaxLengthDefined;
}

void CCache::Resize(int length)
{
  if (length == 0)
  {
  m_nMaxLength = m_nMaxLengthDefined = 0;
  return; // we may have cells on disk, but they will be loaded when needed, and never written again
  }

  if (length < 3)
  length = 3;

  m_nMaxLength = m_nMaxLengthDefined = length;

  bool bValid = true;

  while (bValid && m_nLength > m_nMaxLength)
  {
    CCellList *pl = m_last;

  if (pl->Cell().Dirty())
      bValid = pl->Cell().WriteData();

  if (bValid)
  {
      m_last = m_last->Prev();

      pl->Detach();

      --m_nLength;
  }
  }

  if (!bValid)
  {
  _m()->msg("Failure to (fully) adjust the result cache size. Please check whether the disk is full.");
  }
}

void CCache::GetChecksum(int nColumn, unsigned char *buffer)
{
  CDataCell *cell = CellImpl(0, nColumn, true);

  if (cell)
  cell->GetChecksum(buffer);
}

void CCache::SetChecksum(int nColumn, unsigned char *buffer)
{
  CDataCell *cell = CellImpl(0, nColumn, true);

  if (cell)
  cell->SetChecksum(buffer);
}

bool CCache::ChecksumOK(int nColumn, int *nStatus)
{
  CDataCell *cell = CellImpl(0, nColumn, true);

  if (nStatus)
  *nStatus = -1;

  return cell && cell->ChecksumOK(nStatus);
}

void CCache::Flush()
{
  CCellList *pl;
  bool bValid = true;

  for (pl = m_first; pl != 0 && bValid; pl = pl->Next())
  {
    if (pl->Cell().Dirty())
      bValid = pl->Cell().WriteData();
  }
  if (!bValid)
  {
  _m()->msg("Failure to flush the result cache. Please check whether the disk is full.");
  }
}


// Normally, the calls to StartCriticalSection pre-load all the results
// In case this was not complete, we will add them on the fly
// We keep track of results we really don't have by setting the pointer to 1,
// instead of 0.
//
// TODO: these on-the-fly additions need to be replaced with pre-loads;
// perhaps we can keep track of this in the future; otherwise, we need more testing,
// specifically of large user-land models

#define ABSENT  (CDataCell *)1
#define IF_VALID_RETURN(data)     if (nColumn < m_vcCriticalSection.size()) \
                  data = m_vcCriticalSection[nColumn]; \
                                  if (data == ABSENT) \
                  return 0; \
                                  if (data) \
                  return data

CDataCell *CCache::Cell(int nElement, int nColumn, bool bReading)
{
  if (m_nCriticalSectionSize > 0)
  {
  tbb::spin_rw_mutex::scoped_lock lock(myGlobalMutex, false);

  CDataCell *data = 0;

  IF_VALID_RETURN(data);
  
  if (!lock.upgrade_to_writer()) // re-check conditions as we lost the lock during upgrade
  {
      IF_VALID_RETURN(data);
  }

  StartCriticalSection(nColumn, 1);

  data = m_vcCriticalSection[nColumn];

  if (!data)
      m_vcCriticalSection[nColumn] = ABSENT;

  return data;
  }

  return CellImpl(nElement, nColumn, bReading);
}

CDataCell *CCache::CellImpl(int nElement, int nColumn, bool /*bReading*/)
{
  // try cells in cache first
  CCellList *pFound = 0;

  CCellList *pl;
  for(pl = m_first; pl != 0; pl = pl->Next())
  {
    if(pl->Cell().Contains(nColumn))
    {
      pFound = pl;
      break;
    }
  }

  // must read new values from file if none found and reading
  bool bReread = (pFound == 0);
  if(!pFound)
  {
  bool bValid = true;

  if (m_nLength == m_nMaxLength && m_nMaxLength > 0 && m_last)
  {
      // the last cell will fall off, so write it if necessary
      if(m_last->Cell().Dirty())
    bValid = m_last->Cell().WriteData();

      if (bValid)
      {
    pFound = m_last; // we will reuse this cell after it's been moved to the first position

    pFound->Cell().ReuseFor(nColumn);

    m_nSuccessiveFailedWrites = 0;
      }
      else
      {
    ++m_nSuccessiveFailedWrites;

    if (m_nSuccessiveFailedWrites == 1)
          _m()->msg("Failure to write to the result cache; keeping blocks in memory. Please check whether the disk is full.");
    
    ++m_nMaxLength;
      }
  }
  if (!pFound)
  {
      assert(m_nLength < m_nMaxLength || m_nMaxLength == 0);
      ++m_nLength;

      // generate a new cell and prepend it to the list of cells (will be the new first)
      CDataCell *pCell = new CDataCell(m_meshdatacacher, nColumn);
      CCellList *pList = new CCellList(0, m_first, *pCell);
      m_first = pList;
      if(!m_last)
    m_last = pList;

      pFound = pList;
  }
  }

  if(pFound != m_first && m_first != m_last)
  {
    // reorganize
    if(pFound == m_last) m_last = m_last->Prev();
    pFound->Detach();

    pFound->InsertBefore(m_first);
    m_first = pFound;
  }

  if(bReread)
  {
    bool bRead = m_first->Cell().ReadData(nElement);
  if (!bRead)
  {
      _m()->msg("Failed to read from the result cache. Please check your results.");
      return 0;
  }
  }

  return &m_first->Cell();
}

const double *CCache::Value(int nElement, int nColumn, int nNode)
{
  CDataCell *pCell = Cell(nElement, nColumn, true);
  if(!pCell) return 0;

  return pCell->Value(nElement, nNode);
}

void CCache::Value(int nElement, int nColumn, int nNode, double val)
{
  CDataCell *pCell = Cell(nElement, nColumn, false);
  assert(pCell != 0);

  pCell->Value(nElement, nNode, val);
}

bool CCache::StartCriticalSection(int nColumn, int nNumber)
{
  m_nOldMaxLength = m_nMaxLength;
  if (m_nMaxLength > 0 && m_nMaxLength < nNumber + m_nCriticalSectionSize)
  {
  m_nMaxLength = nNumber + m_nCriticalSectionSize;
  }

  if (m_vcCriticalSection.size() < nColumn + nNumber)
  {
  if (m_vcCriticalSection.size() == 0)
      m_vcCriticalSection.reserve(std::max(1024, nColumn + 30));
  m_vcCriticalSection.resize(nColumn + nNumber, 0);
  }

  //int nTempSize = m_nCriticalSectionSize;
  //m_nCriticalSectionSize = 0;

  for (int i = nColumn; i < nColumn + nNumber; ++i)
  {
  if (m_vcCriticalSection[i] == 0)
  {
      CDataCell *data = CellImpl(0, i, true);

      m_vcCriticalSection[i] = data;

      ++m_nCriticalSectionSize;
  }
  }

  //m_nCriticalSectionSize = nTempSize;

  return true;
}

void CCache::EndCriticalSection()
{
  m_nMaxLength = m_nOldMaxLength;
  m_vcCriticalSection.clear();
  m_nCriticalSectionSize = 0;
}


// CCellList implementation
CCache::CCellList::CCellList(CCellList *prev, CCellList *next, CDataCell &cell) :
  m_cell(cell),
  m_prev(prev),
  m_next(next)
{
  if(prev) prev->m_next = this;
  if(next) next->m_prev = this;
}

CCache::CCellList::~CCellList()
{
  delete &m_cell;
}

CCache::CCellList *CCache::CCellList::Next()
{
  return m_next;
}

CCache::CCellList *CCache::CCellList::Prev()
{
  return m_prev;
}

CDataCell &CCache::CCellList::Cell()
{
  return m_cell;
}

void CCache::CCellList::Detach()
{
  if(m_prev) m_prev->m_next = m_next;
  if(m_next) m_next->m_prev = m_prev;

  m_prev = 0;
  m_next = 0;
}

void CCache::CCellList::InsertBefore(CCellList *list)
{
  assert(m_prev == 0); // must detach first
  assert(m_next == 0); // must detach first

  m_prev = list->m_prev;
  list->m_prev = this;

  if(m_prev)
  {
    assert(m_prev->Next() == list);
    m_prev->m_next = this;
  }

  m_next = list;
}

} // namespace mdc
