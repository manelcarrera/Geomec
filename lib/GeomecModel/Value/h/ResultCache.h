// ResultCache.h: interface for the CResultCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTCACHE_H__CA673202_13C3_4408_8A1D_29B611EB234A__INCLUDED_)
#define AFX_RESULTCACHE_H__CA673202_13C3_4408_8A1D_29B611EB234A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace mdc
{
class CMeshDataCacher;
}

namespace geo
{
class IMesh;
}

class CModelBase;


class CResultCache  
{
public:
  CResultCache(CModelBase& model, const geo::IMesh& mesh, int nColumns, int nCacheSizeMb);
  CResultCache(CModelBase& model, const geo::IMesh& mesh, int nColumns);
  ~CResultCache();

  mdc::CMeshDataCacher& RawCacher();
  mdc::CMeshDataCacher* AveragedCacher();
  mdc::CMeshDataCacher& ActiveCacher();

  int NumColumns() const;

  // call this method if the contents of the raw cache are changed or
  //  if contents are added
  void InvalidateStorage();

  bool HasAveragedCache() const;

  void switchRawCacher(bool enableResultCache, int resultCacheSize, bool bForce = false);
  void resizeRawCacher(int oldResultCacheSize, int newResultCacheSize);

private:
  void DestroyCache(mdc::CMeshDataCacher* pCacher);
  void FinalizeStorage();

  static int validateCacheSize(int cacheSize);

private:
  CModelBase& m_model;
  const geo::IMesh& m_mesh;
  int m_nColumns;
  int m_nCacheSizeMb;

  // maintain two caches, one for the raw data, one for the averaged values
  mdc::CMeshDataCacher* m_pRawCache;
  mdc::CMeshDataCacher* m_pAveragedCache;
};

#endif // !defined(AFX_RESULTCACHE_H__CA673202_13C3_4408_8A1D_29B611EB234A__INCLUDED_)
