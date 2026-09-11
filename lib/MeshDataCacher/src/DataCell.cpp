// DataCell.cpp: implementation of the CDataCell class.
//
//////////////////////////////////////////////////////////////////////

#include "DataCell.h"
#include "MeshDataCacher.h"
#include "StorageInterface.h"

#include <stdio.h>
#include <algorithm>

#include <limits>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace mdc {

CDataCell::CDataCell(const CMeshDataCacher &cacher, int nColumn)
  :	m_cacher(cacher)
  ,	m_pData(0)
  ,	m_nColumn(nColumn)
  ,	m_bDirty(false)
  , m_checkSum(cacher.m_size, cacher.m_pivot, cacher.m_maxEltNodes, cacher.m_maxIfNodes)
{
}

CDataCell::~CDataCell()
{
	if(m_pData) delete [] m_pData;
}

void CDataCell::ReuseFor(int nColumn)
{
  m_nColumn = nColumn;
  InitData(true);
}

bool CDataCell::Contains(int nColumn) const
{
	return m_nColumn == nColumn;
}

const double *CDataCell::Value(int nElement, int nNode)
{
	if(!m_pData)
		return 0;

	int offset = m_cacher.Offset(nElement, nNode);
	
	if(m_pData[offset] == DBL_MAX)
		return 0;

	return &m_pData[offset];
}

void CDataCell::Value(int nElement, int nNode, double val)
{
	InitData();

	m_pData[m_cacher.Offset(nElement, nNode)] = val;

	m_bDirty = true;
  m_checkSum.reset();
}


void CDataCell::InitData(bool bReinitialize)
{
  int nSize = m_cacher.Size();

  if (!m_pData)
  {
  	m_pData = new double[nSize];
    bReinitialize = true;
  }

  if (bReinitialize)
  {
  	for (int i = 0; i < nSize; ++i)
	  {
		  m_pData[i] = std::numeric_limits<double>::quiet_NaN();
	  }

    m_checkSum.reset();
  }
}




bool CDataCell::ReadData(int /*nElement*/)
{
  return Cacher().StorageInterface()->ReadBlock(*this);
}

bool CDataCell::WriteData()
{
  return Cacher().StorageInterface()->WriteBlock(*this);
}



void CDataCell::GetChecksum(unsigned char *buffer)
{
  // pre: buffer has enough room for size
  memcpy(buffer, &m_checkSum, m_checkSum.size);
}

void CDataCell::SetChecksum(unsigned char *buffer)
{
  unsigned int size = *(short *)buffer;

  assert(size <= m_checkSum.size);

  memcpy(&m_checkSum, buffer, size);
}

bool CDataCell::ChecksumOK(int *nStatus)
{
  assert(m_checkSum.type == 2); // only one we support right now; type 1 may exist in some dev models

  if (nStatus)
    *nStatus = 0;

  if (!m_pData)
    return false;

  if (nStatus)
    *nStatus = 1;

  if (m_checkSum.seed.first == 0)
  {
    srand(time(0));
    m_checkSum.seed.first = rand();
    m_checkSum.seed.second = rand();
    if (nStatus)
      *nStatus += 2;
  }

  uint128 hash = CityHash128WithSeed((const char *)m_pData, m_cacher.Size() * sizeof(double), m_checkSum.seed);

  if (m_checkSum.hash.first == 0)
  {
    m_checkSum.hash = hash;
    if (nStatus)
      *nStatus += 4;
  }

  if (nStatus && m_checkSum.hash == hash)
    *nStatus += 8;

  return m_checkSum.hash == hash;
}



void CDataCell::SaveAs(const char *fileName, int nElement, int nSize)
{
  if (!m_pData)
    return;

  FILE *fp = fopen(fileName, "w+");

  if (!fp)
    return;

  if (nSize == 0 || nSize > m_cacher.m_mesh.ElementSize())
    nSize = m_cacher.m_mesh.ElementSize();

  for (size_t i = nElement; i < nSize; ++i)
  {
    for (size_t j = 0; j < m_cacher.MaxNodeSize(i); ++j)
      fprintf(fp, "% 6zd  % 2zd %f\n", i, j, m_pData[m_cacher.Offset(i, j)]);
  }

  fclose(fp);
}



bool CDataCell::Dirty() const
{
	return m_bDirty;
}

const CMeshDataCacher &CDataCell::Cacher() const
{
	return m_cacher;
}



} // namespace mdc
