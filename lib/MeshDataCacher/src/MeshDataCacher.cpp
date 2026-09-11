// MeshDataCacher.cpp: implementation of the CMeshDataCacher class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshDataCacher.h"
#include "Cache.h"
#include "DataCell.h"
#include "StorageInterface.h"

#include "lbcx.h"

#ifdef _WIN64
typedef long long TSize;
#else
typedef long TSize;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace mdc {

CMeshDataCacher::CMeshDataCacher(const geo::IMesh &mesh, IStorageInterface *storage, int nCacheSizeMb)
    : m_mesh(mesh), m_pCache(0), m_storage(storage), m_size(mesh.ElementSize()), m_pivot(0), m_maxEltNodes(0),
      m_maxIfNodes(0) {
  std::vector<std::pair<int, int>> bodies;
  std::vector<std::pair<int, int>> faults;

  mesh.GetElementTypeRanges(bodies, faults, &m_maxEltNodes, &m_maxIfNodes);

  if (bodies.size() == 1 && faults.size() == 1 && bodies[0].first == 0 && bodies[0].second == faults[0].first &&
      faults[0].second == m_size) {
    m_pivot = bodies[0].second;
  }

  if (!m_pivot) {
    m_maxEltNodes = m_maxIfNodes = std::max(m_maxEltNodes, m_maxIfNodes);
  }

  int nrCells = 0;
  if (m_storage->Directory()) {
    nrCells = NrCells(nCacheSizeMb);
  }
  m_pCache = new CCache(*this, nrCells);
}

CMeshDataCacher::~CMeshDataCacher() {
  assert(m_pCache != 0);
  delete m_pCache;

  if (m_storage && !m_storage->OwnedByGuard())
    delete m_storage;
}

int CMeshDataCacher::Size() const { return m_pivot * m_maxEltNodes + (m_size - m_pivot) * m_maxIfNodes; }

int CMeshDataCacher::MaxNodeSize(int nElement) const { return nElement < m_pivot ? m_maxEltNodes : m_maxIfNodes; }

int CMeshDataCacher::Offset(int nElement, int nNode) const {
  if (nElement < m_pivot)
    return nElement * m_maxEltNodes + nNode;
  else
    return m_pivot * m_maxEltNodes + (nElement - m_pivot) * m_maxIfNodes + nNode;
}

int CMeshDataCacher::NrCells(int cacheSizeMb) {
  TSize cellsize = Size() * sizeof(double);

  // work in TSize type for large numbers in bytes
  TSize thousandtwentyfoursquared = 1024 * 1024;

  // the maximum cache size in bytes
  TSize maxcachesize = (TSize)cacheSizeMb * thousandtwentyfoursquared;

  TSize numcells = maxcachesize / cellsize;
  if (numcells > INT_MAX)
    numcells = INT_MAX; // cap for integer usage
  if (numcells < 64)
    numcells = 64;

  return (int)numcells;
}

void CMeshDataCacher::Resize(const char *directory, int cacheSizeMb) {
  assert(m_pCache != 0);

  if (m_storage->Directory() && !directory) {
    m_pCache->Resize(0);
  } else {
    assert(directory != 0 && (m_storage->Directory() == 0 || strcmp(m_storage->Directory(), directory) == 0));
    m_storage->Directory(directory);
    m_pCache->Resize(NrCells(cacheSizeMb));
  }
}

void CMeshDataCacher::GetChecksum(int nColumn, unsigned char *buffer) { m_pCache->GetChecksum(nColumn, buffer); }

void CMeshDataCacher::SetChecksum(int nColumn, unsigned char *buffer) { m_pCache->SetChecksum(nColumn, buffer); }

bool CMeshDataCacher::ChecksumOK(int nColumn, int *nStatus) { return m_pCache->ChecksumOK(nColumn, nStatus); }

void CMeshDataCacher::Flush() { m_pCache->Flush(); }

const double *CMeshDataCacher::Value(int nElement, int nColumn, int nNode) const {
  assert(m_pCache != 0);
  return m_pCache->Value(nElement, nColumn, nNode);
}

void CMeshDataCacher::Value(int nElement, int nColumn, int nNode, double val) {
  assert(m_pCache != 0);
  m_pCache->Value(nElement, nColumn, nNode, val);
}

void CMeshDataCacher::SaveCell(int nColumn, IStorageInterface *pStorageInterface) {
  CDataCell *pCell = m_pCache->Cell(0, nColumn, true);
  if (pCell)
    (pStorageInterface ? pStorageInterface : StorageInterface())->WriteBlock(*pCell);
}

bool CMeshDataCacher::StartCriticalSection(int nColumn, int nNumber) {
  assert(m_pCache != 0);
  return m_pCache->StartCriticalSection(nColumn, nNumber);
}

void CMeshDataCacher::EndCriticalSection() {
  assert(m_pCache != 0);
  m_pCache->EndCriticalSection();
}

IStorageInterface *CMeshDataCacher::StorageInterface() const { return m_storage; }

CMeshDataCacherTestAccessor::CMeshDataCacherTestAccessor(CMeshDataCacher &mdc) : m_MeshDataCacher(mdc) {}

// call this before any cells are initialized
void CMeshDataCacherTestAccessor::DisablePivot() {
  m_MeshDataCacher.m_pivot = 0;
  m_MeshDataCacher.m_maxEltNodes = m_MeshDataCacher.m_maxEltNodes =
      std::max(m_MeshDataCacher.m_maxEltNodes, m_MeshDataCacher.m_maxEltNodes);
}

int CMeshDataCacherTestAccessor::Length() const { return m_MeshDataCacher.m_pCache->m_nLength; }

int CMeshDataCacherTestAccessor::MaxLength() const { return m_MeshDataCacher.m_pCache->m_nMaxLength; }

void CMeshDataCacherTestAccessor::SaveColumn(int nColumn, const char *fileName, int nElement, int nSize) {
  CDataCell *cell = m_MeshDataCacher.m_pCache->Cell(0, nColumn, true);
  if (cell)
    cell->SaveAs(fileName, nElement, nSize);
}

} // namespace mdc
