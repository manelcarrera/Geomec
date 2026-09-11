// ResultCache.cpp: implementation of the CResultCache class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "ResultCache.h"

#include "GeomecUtils.h"
#include "MeshDataCacher.h"
#include "StorageInterface.h"
#include "FormationBase.h"
#include "HorizonBase.h"
#include "WellCasingModel.h"
#include "Global.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "ElementGroup.h"
#include "ConsistencyGuard.h"
#include "HDF5StorageInterface.h"

#if 1
#include "MPKernel.h"
#else
// we're going to fake having tbb on linux until we get linkage right
#define FAKE_TBB

namespace tbb
{

template<typename Type>
class blocked_range
{ 
  Type m_begin;
  Type m_end;
public:
  blocked_range(Type b, Type e) : m_begin(b), m_end(e) {}
  Type begin() const { return m_begin; }
  Type end() const { return m_end; }
};

}

namespace mp
{

class CKernelDispatcher
{
  IProgressBase& m_progress;
  int&           m_current;
  bool           m_quit;
public:

  CKernelDispatcher(IProgressBase& progress, int& current) : m_progress(progress), m_current(current), m_quit(false) {}

  bool shouldQuit() const { return m_quit; }

  void Step(size_t step = 1)
  {
  try
  {
      m_progress.Step(step);
      m_current += step;
  }
  catch (CProgressCancel *e)
  {
      delete e;
      m_quit = true;
  }
  }

};

}

#endif






namespace
{

// We define a base class and four specialized wrappers for the different containers with elements which we can average
// and an iterator that keeps track of a current position.
// This iterator is "dumb"; any checking for valid ranges needs to be done outside.
// Multiple threads can have their own iterator indexing the same wrapper.
//
// Typical use with 0 <= BEGIN <= END <= wrapper's container.size():
//
// AccessorIterator it(BEGIN);
// for (size_t i = BEGIN; i < END; ++i)
//   geo::IElement e& = wrapper.element(it);
//   it.advance();
//

class AccessorIterator
{
  friend class FormationAccessor;
  friend class FaultAccessor;
  friend class ElementSetAccessor;
  friend class ElementPtrArrayAccessor;

  size_t setIndex;
  size_t index;

public:
  AccessorIterator(size_t idx = 0) : setIndex(0), index(idx) {}

  void reset() { setIndex = index = 0; }
  void advance() { ++index; }
};

class AccessorBase
{
public:
  AccessorBase() {}
  virtual ~AccessorBase() {}
  virtual const geo::IElement& element(AccessorIterator& accessorIterator) const = 0;
  virtual size_t size() const { return 0; }
};

class FormationAccessor : public AccessorBase
{
  const CFormationBase *m_pFormation;
public:
  FormationAccessor(const CFormationBase& formation) : m_pFormation(&formation) {}
  virtual const geo::IElement& element(AccessorIterator& accessorIterator) const
  {
  for (; accessorIterator.setIndex < m_pFormation->ElementSetSize(); ++accessorIterator.setIndex)
      if (accessorIterator.index >= m_pFormation->ElementSet(accessorIterator.setIndex).ElementSet().ElementSize())
    accessorIterator.index -= m_pFormation->ElementSet(accessorIterator.setIndex).ElementSet().ElementSize();
      else
    break;
  return m_pFormation->ElementSet(accessorIterator.setIndex).ElementSet().Element(accessorIterator.index);
  }
  virtual size_t size() const
  {
  size_t retval = 0;
  for (size_t i = 0; i < m_pFormation->ElementSetSize(); ++i)
      retval += m_pFormation->ElementSet(i).ElementSet().ElementSize();
  return retval;
  }
};

class FaultAccessor : public AccessorBase
{
  const CHorizonBase *m_pHorizon;
public:
  FaultAccessor(const CHorizonBase& horizon) : m_pHorizon(&horizon) {}
  virtual const geo::IElement& element(AccessorIterator& accessorIterator) const
  {
  return m_pHorizon->InterfaceElementGroup()->Element(accessorIterator.index);
  }
  virtual size_t size() const
  {
  return m_pHorizon->InterfaceElementGroup()->ElementSize();
  }
};

class ElementSetAccessor : public AccessorBase
{
  const geo::IElementSet* m_pElementSet;
public:
  ElementSetAccessor(const geo::IElementSet& elementSet) : m_pElementSet(&elementSet) {}
  virtual const geo::IElement& element(AccessorIterator& accessorIterator) const
  {
  return m_pElementSet->Element(accessorIterator.index);
  }
  virtual size_t size() const
  {
  return m_pElementSet->ElementSize();
  }
};

class ElementPtrArrayAccessor : public AccessorBase
{
  const geo::CPtrArray<geo::IElement>* m_pElementPtrArray;
public:
  ElementPtrArrayAccessor(const geo::CPtrArray<geo::IElement>& elementPtrArray) : m_pElementPtrArray(&elementPtrArray) {}
  virtual const geo::IElement& element(AccessorIterator& accessorIterator) const
  {
  return m_pElementPtrArray->Object(accessorIterator.index);
  }
  virtual size_t size() const
  {
  return m_pElementPtrArray->Size();
  }
};


// ConnectionInfo is used to make a connection map of a node to the elements that have the same node
// To this end, we make a vector with (&node, &element, node_index) and sort it afterwards;
// now a range of entries with the same node forms the connections of that node.
// This is faster than std::map.

struct ConnectionInfo
{
  const geo::INode    *node;
  const geo::IElement *element;
  size_t         node_index;

  ConnectionInfo(const geo::INode& n, const geo::IElement& e, size_t i) : node(&n), element(&e), node_index(i) {}

  bool operator<(const ConnectionInfo& rhs) const
  {
  return *node < *rhs.node;
  }
};

struct ConnectionInfoIndex
{
  size_t index;
  size_t size;

  ConnectionInfoIndex(size_t index, size_t size) : index(index), size(size) {}
};


// Two tasks that do the work.
// TaskConnectionInfo builds the connection map of the (wrapped) container.
// TaskAveraging uses the connection map to build the AveragedCacher.

class TaskConnectionInfo
{
  const AccessorBase *m_object;

#ifndef FAKE_TBB
  tbb::concurrent_vector<ConnectionInfo>& m_data;
#else
  std::vector<ConnectionInfo>& m_data;
#endif

  mp::CKernelDispatcher *m_dispatcher;

public:
#ifndef FAKE_TBB
  TaskConnectionInfo(const AccessorBase *object, tbb::concurrent_vector<ConnectionInfo>& data) : m_object(object), m_data(data), m_dispatcher(0) {}

  tbb::concurrent_vector<ConnectionInfo>& data() const { return m_data; }
#else
  TaskConnectionInfo(const AccessorBase *object, std::vector<ConnectionInfo>& data) : m_object(object), m_data(data), m_dispatcher(0) {}

  std::vector<ConnectionInfo>& data() const { return m_data; }
#endif
  size_t size() const { return m_object->size(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void operator()(const tbb::blocked_range<size_t>& r) const
  {
  AccessorIterator accessorIterator(r.begin());

  for (size_t i = r.begin(); i < r.end(); ++i)
  {
      const geo::IElement& element = m_object->element(accessorIterator);
      int nNodes = element.NrOfNodes();
      for(int n = 0; n < nNodes; ++n)
      {
    const geo::INode& node = element.Node(n);

    m_data.push_back(ConnectionInfo(node, element, n));
      }

      accessorIterator.advance();
  }
  if (m_dispatcher)
      m_dispatcher->Step(r.size());
  }
};

class TaskAveraging
{
#ifndef FAKE_TBB
  tbb::concurrent_vector<ConnectionInfo>& m_data;
  tbb::concurrent_vector<ConnectionInfoIndex>& m_index;
#else
  std::vector<ConnectionInfo>& m_data;
  std::vector<ConnectionInfoIndex>& m_index;
#endif

  CResultCache& m_cache;

  size_t m_column;

  mp::CKernelDispatcher *m_dispatcher;

public:
#ifndef FAKE_TBB
  TaskAveraging(tbb::concurrent_vector<ConnectionInfo>& data, tbb::concurrent_vector<ConnectionInfoIndex>& index, CResultCache& cache)
  : m_data(data), m_index(index), m_cache(cache), m_column(0), m_dispatcher(0) {}

  tbb::concurrent_vector<ConnectionInfo>& data() const { return m_data; }
#else
  TaskAveraging(std::vector<ConnectionInfo>& data, std::vector<ConnectionInfoIndex>& index, CResultCache& cache)
  : m_data(data), m_index(index), m_cache(cache), m_column(0), m_dispatcher(0) {}

  std::vector<ConnectionInfo>& data() const { return m_data; }
#endif

  size_t size() const { return m_index.size(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setColumn(size_t c) { m_column = c; }

  void operator()(const tbb::blocked_range<size_t>& r) const
  {
  for (size_t it = r.begin(); it < r.end(); ++it)
  {
      const size_t i = m_index[it].index;
      const size_t j = i + m_index[it].size;
      size_t numbers = 0;
      double values = 0;

      for (size_t k = i; k < j; ++k)
      {
    const  geo::IElement& element = *m_data[k].element;
    size_t index = m_data[k].node_index;

    const double *value = m_cache.RawCacher().Value(element.Index(), m_column);
    if (value)
    {
          ++numbers;
          values += value[index];
    }
      }

      if (numbers > 0)
      {
    for (size_t k = i; k < j; ++k)
    {
          const geo::IElement& element = *m_data[k].element;
          size_t index = m_data[k].node_index;

          m_cache.AveragedCacher()->Value(element.Index(), m_column, index, values / numbers);
    }
      }
  }
  if (m_dispatcher)
      m_dispatcher->Step(r.size());
  }
};



class CResultAverager
{
public:
  CResultAverager(const AccessorBase& object, CResultCache& cache);
  bool AverageResults(IProgressBase& progdlg, int& nCurrentStep, int nProgressSteps);

private:
  const AccessorBase *m_object;
  CResultCache& m_cache;
};



CResultAverager::CResultAverager(const AccessorBase& object, CResultCache& cache)
: m_object(&object),
  m_cache(cache)
{
}

#ifdef FAKE_TBB
bool CResultAverager::AverageResults(IProgressBase& progdlg, int& nCurrentStep, int /*nProgressSteps*/)
#else
bool CResultAverager::AverageResults(IProgressBase& progdlg, int& /*nCurrentStep*/, int /*nProgressSteps*/)
#endif
{
#ifdef FAKE_TBB
  size_t objectSize = m_object->size();

  std::vector<ConnectionInfo> info;
  info.reserve(objectSize);

  std::vector<ConnectionInfoIndex> infoIndex;

  TaskConnectionInfo taskConnectionInfo(m_object, info);
  TaskAveraging taskAveraging(info, infoIndex, m_cache);

  mp::CKernelDispatcher dispatcher(progdlg, nCurrentStep);

  taskConnectionInfo.setDispatcher(&dispatcher);
  taskAveraging.setDispatcher(&dispatcher);


  taskConnectionInfo(tbb::blocked_range<size_t>(0, objectSize));

  if (dispatcher.shouldQuit())
  return false;

  std::sort(info.begin(), info.end());

  infoIndex.reserve(info.size() / 4);

  for (size_t i = 0; i < info.size();)
  {
  const geo::INode& node = *info[i].node;

  size_t j = i + 1;
  while (j < info.size() && node == *info[j].node)
      ++j;

  infoIndex.push_back(ConnectionInfoIndex(i, j - i));

  i = j;
  }

  for (size_t c = 0; c < m_cache.NumColumns(); ++c)
  {
  m_cache.RawCacher().Value(0, c);
  m_cache.AveragedCacher()->Value(0, c);

  taskAveraging.setColumn(c);

  taskAveraging(tbb::blocked_range<size_t>(0, info.size()));

  if (dispatcher.shouldQuit())
      return false;
  }
#else
  size_t objectSize = m_object->size();

  tbb::concurrent_vector<ConnectionInfo> info;
  info.reserve(objectSize);

  tbb::concurrent_vector<ConnectionInfoIndex> infoIndex;


  TaskConnectionInfo taskConnectionInfo(m_object, info);
  TaskAveraging taskAveraging(info, infoIndex, m_cache);


  mp::CKernelDispatcher dispatcher;


  if (objectSize < 4000) // guess work
  {
  mp::CKernelSequential<TaskConnectionInfo> ciKernel;
  mp::CKernelSequential<TaskAveraging> aKernel;

  mp::IDispatchedTask *disTaskInit1 = NEW_DISPATCH_TASK(mp::CKernelSequential, TaskConnectionInfo)(ciKernel, taskConnectionInfo);
  dispatcher.launch(progdlg, disTaskInit1);

  if (dispatcher.Canceled())
      return false;


  std::sort(info.begin(), info.end());


  // unfortunately, tbb slows down a lot if we try to adjust for "non-aligned" indices in-situ, so we gather the index info here and feed that to tbb
  infoIndex.reserve(info.size() / 4);

  for (size_t i = 0; i < info.size();)
  {
      const geo::INode& node = *info[i].node;

      size_t j = i + 1;
      while (j < info.size() && node == *info[j].node)
    ++j;

      infoIndex.push_back(ConnectionInfoIndex(i, j - i));

      i = j;
  }
  

  for (size_t c = 0; c < m_cache.NumColumns(); ++c)
  {
      m_cache.RawCacher().Value(0, c);
      m_cache.AveragedCacher()->Value(0, c);

      taskAveraging.setColumn(c);

      mp::IDispatchedTask *disTaskInit2 = NEW_DISPATCH_TASK(mp::CKernelSequential, TaskAveraging)(aKernel, taskAveraging);
      dispatcher.launch(progdlg, disTaskInit2);

      if (dispatcher.Canceled())
    return false;
  }
  }
  else
  {
  mp::CKernelParallel<TaskConnectionInfo> ciKernel;
  mp::CKernelParallel<TaskAveraging> aKernel;

  mp::IDispatchedTask *disTaskInit1 = NEW_DISPATCH_TASK(mp::CKernelParallel, TaskConnectionInfo)(ciKernel, taskConnectionInfo);
  dispatcher.launch(progdlg, disTaskInit1);

  if (dispatcher.Canceled())
      return false;

  tbb::parallel_sort(info.begin(), info.end());


  // unfortunately, tbb slows down a lot if we try to adjust for "non-aligned" indices in-situ, so we gather the index info here and feed that to tbb
  infoIndex.reserve(info.size() / 4);

  for (size_t i = 0; i < info.size();)
  {
      const geo::INode& node = *info[i].node;

      size_t j = i + 1;
      while (j < info.size() && node == *info[j].node)
    ++j;

      infoIndex.push_back(ConnectionInfoIndex(i, j - i));

      i = j;
  }
  

  for (size_t c = 0; c < m_cache.NumColumns(); ++c)
  {
      m_cache.RawCacher().Value(0, c);
      m_cache.AveragedCacher()->Value(0, c);

      taskAveraging.setColumn(c);

      mp::IDispatchedTask *disTaskInit2 = NEW_DISPATCH_TASK(mp::CKernelParallel, TaskAveraging)(aKernel, taskAveraging);
      dispatcher.launch(progdlg, disTaskInit2);
  
      if (dispatcher.Canceled())
    return false;
  }
  }
#endif

  return true;
}


} // anonymous namespace


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultCache::CResultCache(CModelBase& model, const geo::IMesh& mesh, int nColumns, int nCacheSizeMb)
: m_model(model),
  m_mesh(mesh),
  m_nColumns(nColumns),
  m_nCacheSizeMb(validateCacheSize(nCacheSizeMb)),
  m_pAveragedCache(0)
{
  QString tmp = CreateTempDirExt(CTempPath::TEMP_GENERAL);
  mdc::IStorageInterface *pStorageInterface = model.GetConsistencyGuard()->HDF5StorageInterface(model.Index(), nColumns / 150, 30);
  if (!pStorageInterface)
  {
  pStorageInterface = new mdc::CStorageInterfaceDefault(tmp.toStdString().c_str());
  }
  else
  pStorageInterface->Directory(tmp.toStdString().c_str());
  m_pRawCache = new mdc::CMeshDataCacher(mesh, pStorageInterface, m_nCacheSizeMb);
}

CResultCache::CResultCache(CModelBase& model, const geo::IMesh& mesh, int nColumns)
: m_model(model),
  m_mesh(mesh),
  m_nColumns(nColumns),
  m_nCacheSizeMb(0),
  m_pAveragedCache(0)
{
  mdc::IStorageInterface *pStorageInterface = model.GetConsistencyGuard()->HDF5StorageInterface(model.Index(), nColumns / 150, 30);
  if (!pStorageInterface)
  {
  pStorageInterface = new mdc::CStorageInterfaceDefault;
  }
  m_pRawCache = new mdc::CMeshDataCacher(mesh, pStorageInterface);
}

CResultCache::~CResultCache()
{
  DestroyCache(m_pAveragedCache);
  DestroyCache(m_pRawCache);
}

mdc::CMeshDataCacher& CResultCache::RawCacher()
{
  return *m_pRawCache;
}

mdc::CMeshDataCacher* CResultCache::AveragedCacher()
{
  if(!m_pAveragedCache)
  FinalizeStorage();

  return m_pAveragedCache;
}

mdc::CMeshDataCacher& CResultCache::ActiveCacher()
{
  if(m_model.AverageResults())
  {
  if(AveragedCacher())
      return *AveragedCacher();
  else
      m_model.AverageResults(false); // user cancelled averaging operation
  }

  return RawCacher();
}

int CResultCache::NumColumns() const
{
  return m_nColumns;
}

void CResultCache::InvalidateStorage()
{
  DestroyCache(m_pAveragedCache);
  m_pAveragedCache = 0;
}

bool CResultCache::HasAveragedCache() const
{
  return m_pAveragedCache != 0;
}

void CResultCache::switchRawCacher(bool enableResultCache, int resultCacheSize, bool bForce)
{
  if (enableResultCache)
  {
  m_nCacheSizeMb = bForce ? resultCacheSize : validateCacheSize(resultCacheSize);

  QString tmpDir = m_pRawCache->StorageInterface()->Directory() ? QString(m_pRawCache->StorageInterface()->Directory()) : CreateTempDirExt(CTempPath::TEMP_GENERAL);
  
  m_pRawCache->Resize(tmpDir.toStdString().c_str(), m_nCacheSizeMb);

  if (m_pAveragedCache)
  {
      tmpDir = m_pAveragedCache->StorageInterface()->Directory() ? QString(m_pAveragedCache->StorageInterface()->Directory()) : CreateTempDirExt(CTempPath::TEMP_GENERAL);

      m_pAveragedCache->Resize(tmpDir.toStdString().c_str(), m_nCacheSizeMb);
  }
  }
  else
  {
  m_pRawCache->Resize(0, m_nCacheSizeMb);
  if (m_pAveragedCache)
      m_pAveragedCache->Resize(0, m_nCacheSizeMb);
  }

}

void CResultCache::resizeRawCacher(int /*oldResultCacheSize*/,
  int newResultCacheSize)
{
  switchRawCacher(true, newResultCacheSize);
}

// private

void CResultCache::DestroyCache(mdc::CMeshDataCacher* pCacher)
{
  if(pCacher)
  {
  if (pCacher->StorageInterface()->Directory())
      VERIFY(RemoveDir(pCacher->StorageInterface()->Directory()));
  delete pCacher;
  }
}

// to be called after storing values in the raw cacher, so the averaged values can be generated
void CResultCache::FinalizeStorage()
{
  assert(m_pAveragedCache == 0);

  QString msg = "Calculating smoothed result values";
  std::auto_ptr <IProgressBase> progdlg(_g->prog()->create(eProgress::Dual, msg, true, 1 ));
  progdlg->NextJob(msg);

  // the avaraged cache is not going to the HDF5 file; create a disk cache instead
  if (m_pRawCache->StorageInterface()->Directory()) // swapping to disk?
  {
  QString tmp = CreateTempDirExt(CTempPath::TEMP_GENERAL);
    m_pAveragedCache = new mdc::CMeshDataCacher(m_mesh, new mdc::CStorageInterfaceDefault(tmp.toStdString().c_str()), m_nCacheSizeMb);
  }
  else
  {
  m_pAveragedCache = new mdc::CMeshDataCacher(m_mesh, new mdc::CStorageInterfaceDefault);
  }

  const THorizonBaseEntry& horizon_entry = static_cast<const THorizonBaseEntry&>(*m_model.GraphEntry(MD_BASE_HORIZON));
  const THorizonBaseEntry::TNodeSet& stHorizonNodes = horizon_entry.EntryNodes();
  THorizonBaseEntry::TNodeSet::const_iterator ithor;

  int nCurrentStep = 0;
  int nProgressSteps = 0;

  // count the total number of progress steps (number of elements)
  for(ithor = stHorizonNodes.begin(); ithor != stHorizonNodes.end(); ++ithor)
  {
  const CHorizonBase& horizon = **ithor;
  if(horizon.Slip())
      nProgressSteps += horizon.InterfaceElementGroup()->ElementSize();
  }

  const TFormationBaseEntry& formation_entry = static_cast<const TFormationBaseEntry&>(*m_model.GraphEntry(MD_BASE_FORMATION));
  const TFormationBaseEntry::TNodeSet& stFormationNodes = formation_entry.EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator itfor;
  for(itfor = stFormationNodes.begin(); itfor != stFormationNodes.end(); ++itfor)
  {
  const CFormationBase& formation = **itfor;
  int i;
  for(i = 0; i < formation.ElementSetSize(); ++i)
      nProgressSteps += formation.ElementSet(i).ElementSet().ElementSize();
  }

  const CWellCasingModel* pCasingModel = dynamic_cast<const CWellCasingModel*>(&m_model);
  if(pCasingModel)
  {
  nProgressSteps += pCasingModel->Mesh().SteelElements().ElementSize();
  nProgressSteps += pCasingModel->Mesh().CementElements().ElementSize();
  nProgressSteps += pCasingModel->Mesh().CementInterfaceElements().Size();
  }

  // there will be two passes over the elements, the first is over the elements once, the second is over the elements times the number of columns
  nProgressSteps += nProgressSteps * NumColumns();

  class CScopeGuard
  {
  public:
  CScopeGuard(mdc::CMeshDataCacher** ppAveragedCacher, IProgressBase& progdlg)
  : m_ppAveragedCacher(ppAveragedCacher),
      m_dlg(progdlg)
  {
  }

  ~CScopeGuard()
  {
      if(m_ppAveragedCacher)
      {
    delete *m_ppAveragedCacher;
    *m_ppAveragedCacher = 0;
      }
  }

  void ReleaseCacher()
  {
      m_ppAveragedCacher = 0;
  }

  private:
  mdc::CMeshDataCacher** m_ppAveragedCacher;
  IProgressBase& m_dlg;
  };

  CScopeGuard scopeguard(&m_pAveragedCache, *progdlg);

  progdlg->AddSteps(nProgressSteps);

  for(ithor = stHorizonNodes.begin(); ithor != stHorizonNodes.end(); ++ithor)
  {
  const CHorizonBase& horizon = **ithor;
  if(horizon.Slip())
  {
      FaultAccessor fa(horizon);
      CResultAverager fault_averager(fa, *this);
      if(!fault_averager.AverageResults(*progdlg, nCurrentStep, nProgressSteps))
    return;
  }
  }

  for(itfor = stFormationNodes.begin(); itfor != stFormationNodes.end(); ++itfor)
  {
  const CFormationBase& formation = **itfor;
  FormationAccessor fa(formation);
  CResultAverager formation_averager(fa, *this);
  if(!formation_averager.AverageResults(*progdlg, nCurrentStep, nProgressSteps))
      return;
  }

  if(pCasingModel)
  {
  {
      ElementSetAccessor esa(pCasingModel->Mesh().SteelElements());
      CResultAverager steel_averager(esa, *this);
      if(!steel_averager.AverageResults(*progdlg, nCurrentStep, nProgressSteps))
    return;
  }

  {
      ElementSetAccessor esa(pCasingModel->Mesh().CementElements());
      CResultAverager cement_averager(esa, *this);
      if(!cement_averager.AverageResults(*progdlg, nCurrentStep, nProgressSteps))
    return;
  }

  {
      ElementPtrArrayAccessor epaa(pCasingModel->Mesh().CementInterfaceElements());
      CResultAverager cementinterface_averager(epaa, *this);
      if(!cementinterface_averager.AverageResults(*progdlg, nCurrentStep, nProgressSteps))
    return;
  }
  }

  scopeguard.ReleaseCacher();
}


int CResultCache::validateCacheSize(int cacheSize)
{
  // make sure it's not too small
  if(cacheSize < 128)
  cacheSize = 128;

  return cacheSize;
}
