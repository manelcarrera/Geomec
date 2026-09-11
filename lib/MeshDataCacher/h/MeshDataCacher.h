// MeshDataCacher.h: interface for the CMeshDataCacher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHDATACACHER_H__3129F3DF_0F73_4C59_95F7_F9DC7A5170B3__INCLUDED_)
#define AFX_MESHDATACACHER_H__3129F3DF_0F73_4C59_95F7_F9DC7A5170B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

class CHDF5StorageInterface;

namespace mdc {

class CCache;
class CDataCell;
class CMeshDataCacherTestAccessor;
class IStorageInterface;
class CArchiveInterface;

class CMeshDataCacher  
{
  friend class CMeshDataCacherTestAccessor;
  friend class CDataCell;
  friend class CStorageInterfaceDefault;
  friend class ::CHDF5StorageInterface;

	const geo::IMesh &m_mesh;
	CCache *m_pCache;

  IStorageInterface *m_storage;

  int m_size;

  int m_pivot;

  int m_maxEltNodes;
  int m_maxIfNodes;


public:
	// cache size is in Mb (1024 * 1024 bytes), and is a real maximum, unless only one cell can fit, 512Mb by default
  // we get ownership of the storage interface
	CMeshDataCacher(const geo::IMesh &mesh, IStorageInterface *storage, int nCacheSizeMb = 512);
	virtual ~CMeshDataCacher();

  void Flush();

  void GetChecksum(int nColumn, unsigned char *buffer);
  void SetChecksum(int nColumn, unsigned char *buffer);
  bool ChecksumOK(int nColumn, int *nStatus = 0);

  void Resize(const char *directory, int cacheSize);

	// the returned pointer contains the data for the element
	// don't access beyond the element's number of nodes !
	// returns NULL if the values could not be found
	const double *Value(int nElement, int nColumn, int nNode = 0) const;

	void Value(int nElement, int nColumn, int nNode, double val); // to set the nodal values for the elements

  bool StartCriticalSection(int nColumn, int nNumber); // see CCache
  void EndCriticalSection();

  IStorageInterface *StorageInterface() const;

  void SaveCell(int nColumn, IStorageInterface *pStorageInterface = 0);

private:
  int Size() const;
  int MaxNodeSize(int nElement) const;
  int Offset(int nElement, int nNode = 0) const;

  int NrCells(int cacheSizeMb);
};


class CMeshDataCacherTestAccessor
{
  CMeshDataCacher& m_MeshDataCacher;

public:
  CMeshDataCacherTestAccessor(CMeshDataCacher& mdc);

  void DisablePivot();

  int Length() const;
  int MaxLength() const;

  void SaveColumn(int nColumn, const char *fileName, int nElement = 0, int nSize = 0);
};



} // namespace mdc

#endif // !defined(AFX_MESHDATACACHER_H__3129F3DF_0F73_4C59_95F7_F9DC7A5170B3__INCLUDED_)
