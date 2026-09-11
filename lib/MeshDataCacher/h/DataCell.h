// DataCell.h: interface for the CDataCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATACELL_H__8EC51C53_B6D5_4D2A_BCEA_79BC1263F5B2__INCLUDED_)
#define AFX_DATACELL_H__8EC51C53_B6D5_4D2A_BCEA_79BC1263F5B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Checksum.h"

class CHDF5StorageInterface;

namespace mdc {

class CMeshDataCacher;


class CDataCell  
{
  friend class CCache;
  friend class CMeshDataCacherTestAccessor;
  friend class CStorageInterfaceDefault;
  friend class ::CHDF5StorageInterface;

  const CMeshDataCacher &m_cacher;
  double *m_pData; // the values for the elements
  int m_nColumn;
  bool m_bDirty;

  char *FileName();

  void ReuseFor(int nColumn);

  void InitData(bool bReinitialize = false);
  bool ReadData(int nElement);
  bool WriteData();

  CChecksum m_checkSum;

  void GetChecksum(unsigned char *buffer);
  void SetChecksum(unsigned char *buffer);
  bool ChecksumOK(int *nStatus = 0);

  // for testing
  void SaveAs(const char *fileName, int nElement = 0, int nSize = 0);

public:
  CDataCell(const CMeshDataCacher &cacher, int nColumn);
  virtual ~CDataCell();

  bool Contains(int nColumn) const;

  const double *Value(int nElement, int nNode = 0); // don't access beyond the element's number of nodes !
  void Value(int nElement, int nNode, double val); // to set the nodal values for the elements

  bool Dirty() const;
  const CMeshDataCacher &Cacher() const;
};

} // namespace mdc

#endif // !defined(AFX_DATACELL_H__8EC51C53_B6D5_4D2A_BCEA_79BC1263F5B2__INCLUDED_)
