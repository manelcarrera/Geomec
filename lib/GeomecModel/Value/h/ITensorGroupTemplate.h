#if !defined(AFX_TENSOR_GROUP_TEMPLATE_H_INCLUDED_)
#define AFX_TENSOR_GROUP_TEMPLATE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITensorGroup.h"
#include "ModelBase.h"
#include "WellCasingModel.h"
#include "WellPoint.h"
#include "NewWellPoint.h"

// combine well::CWellPoint and geo::CElementPoint, we need both at a certain point...
class CElementWellPoint : public geo::CElementPoint
{
public:
  CElementWellPoint(const geo::IElement& element, const geo::IPoint& wellpoint)
  : geo::CElementPoint(element, wellpoint),
  m_wellpoint(wellpoint)
  {
  }

  const geo::IPoint& WellPoint() const
  {
  return m_wellpoint;
  }

private:
  const geo::IPoint& m_wellpoint;
};

class _ITensorResultCache
{
public:
  _ITensorResultCache() {}
  virtual ~_ITensorResultCache() {}
};

template<class T>
class _ResultCache : public _ITensorResultCache
{
public:
  typedef std::pair<geo::CVector, double> TDerived;
  typedef std::vector<TDerived> TDerivedVec;
  TDerivedVec            m_derived[4];
  bool                   m_bPrincipalValid;
  bool                   m_bCylindricalValid;
  std::vector<T>         m_value;
  geo::CPoint            m_point;
  const geo::IElement*   m_element;
  typedef CValueType::TMapEnum TMapType;
  TMapType               m_map_type;
  const CDepletionStage* m_stage;
  bool				   m_bChange;
  _ResultCache(): m_bPrincipalValid(false), m_bCylindricalValid(false), m_element(0), m_map_type(CValueType::MT_NONE), m_bChange(false) {}
};

class ITensorParallelInitializationCallback : public geo::IParallelInitializationCallback
{
public:
  ITensorParallelInitializationCallback() {}
  virtual ~ITensorParallelInitializationCallback() {}
  virtual _ITensorResultCache& GetCache(const ITensorGroup *caller, const IResultComponent& component) const = 0;
};



template<class TENSOR_TYPE>
class ITensorGroupTemplate : public ITensorGroup
{
public:
  // Define fetch functions
  typedef CValueType::TMapEnum TMapType;
  typedef CTensorValueSet<TENSOR_TYPE> TTensorValueSet;
  typedef TTensorValueSet (CResultRegister::*TValueSetFunction)(const CDepletionStage& stage, 
                                  const CAnalysisType& antype, 
                                  bool bChange) const;
  typedef std::vector<TENSOR_TYPE> TTensorValueVec;

private:
  typedef std::vector<_ResultCache<TENSOR_TYPE> > TCache;
//	typedef std::pair<TCache, TCache> TCachePair;
//	typedef std::vector<TCachePair> TCacheVec;
  typedef std::map<CAnalysisType, TCache> TCacheMap;
  typedef std::vector<TCacheMap> TCacheVec;
  mutable TCacheVec m_cache;

public:

  class CParallelInitializationCallback : public ITensorParallelInitializationCallback
  {
  CModelBase& m_model;
  mutable TCacheVec m_cache;
  public:
  CParallelInitializationCallback(CModelBase &model);
  virtual ~CParallelInitializationCallback();
  virtual _ITensorResultCache& GetCache(const ITensorGroup *caller, const IResultComponent& component) const;
  };


public:
  _ResultCache<TENSOR_TYPE>& GetCache(const IResultComponent& component, geo::IParallelInitializationCallback *callback) const;

private:
  void BuildCache() const;

  const TENSOR_TYPE& TensorAtPoint(const geo::IPoint& point, 
      const IResultComponent& component, geo::IParallelInitializationCallback *cb) const;

  const TTensorValueVec& TensorAtElement(const geo::IElement& element, 
                   TMapType map_type,
                   const IResultComponent& component, geo::IParallelInitializationCallback *cb) const;

  CTensor RotatedTensor(const ITensor& tensor, const geo::IVector& dir) const;
  CTensor OrientedTensor(const geo::IPoint& point, const geo::IVector& dir, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  CTensor OrientedTensor(const geo::IElement& element, int nNodeIndex, TMapType map_type, const geo::IVector& dir, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  CTensor WellPathOrientedTensor(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  geo::CValue AxialValue(const CTensor& rotated_tensor) const;
  geo::CValue RadialMaxValue(const CTensor& rotated_tensor) const;
  geo::CValue RadialMinValue(const CTensor& rotated_tensor) const;
  geo::CValue ShearValueInPlane(const CTensor& rotated_tensor) const;
  geo::CValue ShearDipAngleInPlane(const CTensor& rotated_tensor) const;

public:
  ITensorGroupTemplate(unsigned int uResultName, TValueSetFunction function, CResultGroup& group);
  ITensorGroupTemplate(const QString &sResultName, TValueSetFunction function, CResultGroup& group);

  virtual bool Change() const = 0;

  virtual bool NeedParallelInitializationCallback() const { return true; }
  virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback() { return new CParallelInitializationCallback(static_cast<CModelBase&>(Model())); }
  virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *) { return true;  }
  virtual void FinishMapping() {}

  virtual const ITensor& TensorPoint(const geo::IPoint& point,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  virtual const ITensor& TensorElement(const geo::IElement& element, 
                                     int nNodeIndex, 
                     TMapType map_type,
                     const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;


  const TENSOR_TYPE& TensorAtPoint(const geo::IPoint& point,
                   const IResultComponent& component,
                   const CDepletionStage& stage,
                   const CAnalysisType& antype,
                   bool bChange, geo::IParallelInitializationCallback *cb = 0) const;

  const TTensorValueVec& TensorAtElement(const geo::IElement& element,
                   TMapType map_type,
                   const IResultComponent& component,
                   const CDepletionStage& stage,
                   const CAnalysisType& antype,
                   bool bChange, geo::IParallelInitializationCallback *cb = 0) const;

  virtual const TPrincipal& VectorPoint  (const geo::IPoint& point, 
                      CVectorComposite::PRINCIPAL_DIRECTION direction,
                      const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  virtual const TPrincipal& VectorElement(const geo::IElement& element, 
                      CVectorComposite::PRINCIPAL_DIRECTION direction,
                      int nNodeIndex, 
                      TMapType map_type,
                      const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  virtual void ClearCache();
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  geo::CValue WellPathAxialValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  geo::CValue WellPathRadialMaxValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  geo::CValue WellPathRadialMinValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  IValueDomainScalar::TValueVec AxialValueElement(const geo::IElement& element, 
                                                        TMapType map_type,
                            const IResultComponent& component,
                            const geo::IVector& dir, geo::IParallelInitializationCallback *cb = 0) const;
  IValueDomainScalar::TValueVec RadialMaxValueElement(const geo::IElement& element, 
                                                            TMapType map_type,
                              const IResultComponent& component,
                              const geo::IVector& dir, geo::IParallelInitializationCallback *cb = 0) const;
  IValueDomainScalar::TValueVec RadialMinValueElement(const geo::IElement& element, 
                                                            TMapType map_type,
                              const IResultComponent& component,
                              const geo::IVector& dir, geo::IParallelInitializationCallback *cb = 0) const;

  IValueDomainScalar::TValueVec ShearValueInPlaneElement(const geo::IElement& element,
                                                       TMapType map_type,
                                                       const IResultComponent& component,
                                                       const geo::IVector& dir, geo::IParallelInitializationCallback *cb = 0) const;

  IValueDomainScalar::TValueVec ShearDipAngleInPlaneElement(const geo::IElement& element,
                                                          TMapType map_type,
                                                          const IResultComponent& component,
                                                          const geo::IVector& dir, geo::IParallelInitializationCallback *cb = 0) const;

  IValueDomainScalar::TValueVec WellPathAxialValueElement(const geo::IElement& element, 
                                                        TMapType map_type,
                            const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  IValueDomainScalar::TValueVec WellPathRadialMaxValueElement(const geo::IElement& element, 
                                                            TMapType map_type,
                              const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
  IValueDomainScalar::TValueVec WellPathRadialMinValueElement(const geo::IElement& element, 
                                                            TMapType map_type,
                              const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

private:
  static void getWellElementRotation(const geo::IElement& elt, geo::IVector& vec);

  const TPrincipal& PrincipalVectorPoint(const geo::IPoint& point,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  const TPrincipal& PrincipalVectorElement(const geo::IElement& element,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  int nNodeIndex,
  TMapType map_type,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  const TPrincipal& CylindricalVectorPoint(const geo::IPoint& point,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

  const TPrincipal& CylindricalVectorElement(const geo::IElement& element,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  int nNodeIndex,
  TMapType map_type,
  const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;

private:
  TValueSetFunction m_function;
};

template<class TENSOR_TYPE>
ITensorGroupTemplate<TENSOR_TYPE>::ITensorGroupTemplate(unsigned int uResultName, TValueSetFunction function, CResultGroup& group)
: ITensorGroup(uResultName, group), m_function(function)
{
}

template<class TENSOR_TYPE>
ITensorGroupTemplate<TENSOR_TYPE>::ITensorGroupTemplate(const QString &sResultName, TValueSetFunction function, CResultGroup& group)
: ITensorGroup(sResultName, group), m_function(function)
{
}

template<class TENSOR_TYPE>
const TENSOR_TYPE& ITensorGroupTemplate<TENSOR_TYPE>::TensorAtPoint(const geo::IPoint& point,
                                  const IResultComponent& component,
                                  const CDepletionStage& stage,
                                  const CAnalysisType& antype,
                                  bool bChange, geo::IParallelInitializationCallback *cb) const
{
  assert(!point.Empty());
  // Is the point in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache( component, cb );
  if( !cache.m_element && !cache.m_point.Empty() && point == cache.m_point && cache.m_stage == &stage && bChange == cache.m_bChange ) {
    assert(cache.m_value.size() == 1);
    return cache.m_value[0];
  }

  cache.m_element         = 0;
  cache.m_point           = point;
  cache.m_bPrincipalValid = false;
  cache.m_stage           = &stage;
  cache.m_bChange         = bChange;
  if(cache.m_value.size() != 1) cache.m_value.resize(1);
  cache.m_value[0] = (component.ResultRegister().*m_function)(stage, antype, bChange).ValuePoint(point, cb);
  return cache.m_value[0];
}

template<class TENSOR_TYPE>
const TENSOR_TYPE& ITensorGroupTemplate<TENSOR_TYPE>::TensorAtPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  return TensorAtPoint(point, component, component.Stage(), component.AnalysisType(), Change(), cb);
}

/*!
  Build a cache for retrieving the result from the result register
*/
template<class TENSOR_TYPE>
void ITensorGroupTemplate<TENSOR_TYPE>::BuildCache() const
{
  assert(m_cache.size() == 0);
  CModelBase& model = (CModelBase&)Model();
  for(int i = 0; i < model.ResultRegisterSize(); i++) {
  TCacheMap cache_map;
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_LINEAR),              TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_NONLIN),              TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_HEAT),                TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_MIXTURE),             TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_MIXTURE_CONTAINMENT), TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
//		TCachePair cache_pair( TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size() ),
//			                   TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size() ) );
//		m_cache.push_back(cache_pair);
    m_cache.push_back(cache_map);
  }

  assert( m_cache.size() == model.ResultRegisterSize() ); 
}

/*!
  Clears cache when results become invalid
*/
template<class T>
void ITensorGroupTemplate<T>::ClearCache()
{
  m_cache.clear();
}

template<class T>
void ITensorGroupTemplate<T>::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  ClearCache();
  ITensorGroup::OnNeighbourModified(node, uHint);
}

template<class TENSOR_TYPE>
const ITensor& ITensorGroupTemplate<TENSOR_TYPE>::TensorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  return TensorAtPoint(point, component, cb);
}

template<class TENSOR_TYPE>
const ITensor& ITensorGroupTemplate<TENSOR_TYPE>::TensorElement(const geo::IElement& element, 
                                int nNodeIndex, 
                                TMapType map_type,
                const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  return TensorAtElement(element, map_type, component, cb)[nNodeIndex];
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TTensorValueVec&
ITensorGroupTemplate<TENSOR_TYPE>::TensorAtElement(const geo::IElement& element,
                           TMapType map_type,
                           const IResultComponent& component,
                           const CDepletionStage& stage,
                           const CAnalysisType& antype,
                           bool bChange, geo::IParallelInitializationCallback *cb) const
{
  // Is the element in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache( component, cb );
  if( (cache.m_element == &element) && (map_type == cache.m_map_type) && cache.m_point.Empty() && cache.m_stage == &stage && bChange == cache.m_bChange ) {
    assert(cache.m_value.size() == element.NrOfPoints());
    return cache.m_value;
  }

  cache.m_element         = &element;
  cache.m_map_type        = map_type;
  cache.m_point           = geo::CPoint();
  cache.m_bPrincipalValid = false;
  cache.m_stage           = &stage;
  cache.m_bChange         = bChange;
  if(cache.m_value.size() != element.NrOfPoints()) cache.m_value.resize(element.NrOfPoints());
  (component.ResultRegister().*m_function)(stage, antype, bChange).MapValueElement(element, cache.m_value, map_type, cb);
  return cache.m_value;
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TTensorValueVec&
ITensorGroupTemplate<TENSOR_TYPE>::TensorAtElement( const geo::IElement& element, 
                   				TMapType map_type,
                          const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  return TensorAtElement(element, map_type, component, component.Stage(), component.AnalysisType(), Change(), cb);
}

template<class TENSOR_TYPE>
ITensorGroupTemplate<TENSOR_TYPE>::CParallelInitializationCallback::CParallelInitializationCallback(CModelBase& model)
  : m_model(model)
{
  for (int i = 0; i < m_model.ResultRegisterSize(); i++) {
  TCacheMap cache_map;
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_LINEAR), TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_NONLIN), TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_HEAT), TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_MIXTURE), TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  cache_map.insert(typename TCacheMap::value_type(CAnalysisType(CAnalysisType::AT_MIXTURE_CONTAINMENT), TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
  m_cache.push_back(cache_map);
  }

  assert(m_cache.size() == model.ResultRegisterSize());
}

template<class TENSOR_TYPE>
ITensorGroupTemplate<TENSOR_TYPE>::CParallelInitializationCallback::~CParallelInitializationCallback()
{
}

template<class TENSOR_TYPE>
_ITensorResultCache& ITensorGroupTemplate<TENSOR_TYPE>::CParallelInitializationCallback::GetCache(const ITensorGroup* /*caller*/, const IResultComponent& component) const
{
  return m_cache[component.RegisterIndex()][component.AnalysisType()][component.Stage().Index()];
}


template<class TENSOR_TYPE>
_ResultCache<TENSOR_TYPE>& ITensorGroupTemplate<TENSOR_TYPE>::GetCache(const IResultComponent& component, geo::IParallelInitializationCallback *callback) const
{
  ITensorParallelInitializationCallback *cb = dynamic_cast<ITensorParallelInitializationCallback *>(callback);
  if (cb)
  return static_cast<_ResultCache<TENSOR_TYPE>&>(cb->GetCache(this, component));
  
  if (!m_cache.size()) BuildCache();

  return m_cache[component.RegisterIndex()][component.AnalysisType()][component.Stage().Index()];
//	if(component.Linear())
//		return m_cache[component.RegisterIndex()].first[component.Stage().Index()];
//	return m_cache[component.RegisterIndex()].second[component.Stage().Index()];
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal&
          ITensorGroupTemplate<TENSOR_TYPE>::PrincipalVectorPoint(const geo::IPoint& point, 
                                   CVectorComposite::PRINCIPAL_DIRECTION direction,
                                   const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  assert(!point.Empty());
  // Is the point in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache( component, cb );
  if(( !cache.m_element && !cache.m_point.Empty() && point == cache.m_point ) && cache.m_bPrincipalValid) {
    assert(cache.m_derived[direction].size() == 1);
    return cache.m_derived[direction][0];
  }

  const ITensor& tensor = TensorPoint(point, component, cb);
  for(int i = 0; i < 3; i++) {
    if(cache.m_derived[i].size() != 1) cache.m_derived[i].resize(1);
  }
  PrincipalDirections(cache.m_derived[CVectorComposite::PD_MAX][0].first,
            cache.m_derived[CVectorComposite::PD_MAX][0].second,
            cache.m_derived[CVectorComposite::PD_MED][0].first,
            cache.m_derived[CVectorComposite::PD_MED][0].second,
            cache.m_derived[CVectorComposite::PD_MIN][0].first,
            cache.m_derived[CVectorComposite::PD_MIN][0].second,
            tensor);

  cache.m_bPrincipalValid = true;

  return VectorPoint(point, direction, component, cb );
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal& 
          ITensorGroupTemplate<TENSOR_TYPE>::PrincipalVectorElement(const geo::IElement& element, 
                                   CVectorComposite::PRINCIPAL_DIRECTION direction,
                                   int nNodeIndex, 
                                   TMapType map_type,
                                   const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  // Is the element in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache( component, cb );
  if( (cache.m_element == &element) && 
    (map_type == cache.m_map_type) && 
    cache.m_point.Empty() &&
    cache.m_bPrincipalValid) {
    assert(cache.m_value.size() == element.NrOfPoints());
    assert(cache.m_derived[direction].size() == element.NrOfPoints());
    return cache.m_derived[direction][nNodeIndex];
  }


  int i;
  for(i = 0; i < 3; i++) {
    if(cache.m_derived[i].size() != element.NrOfPoints()) cache.m_derived[i].resize(element.NrOfPoints());
  }
  for(i = 0; i < element.NrOfPoints(); i++) {
    const ITensor& tensor = TensorElement(element, i, map_type, component, cb);
    PrincipalDirections(cache.m_derived[CVectorComposite::PD_MAX][i].first,
              cache.m_derived[CVectorComposite::PD_MAX][i].second,
        			    cache.m_derived[CVectorComposite::PD_MED][i].first,
              cache.m_derived[CVectorComposite::PD_MED][i].second,
              cache.m_derived[CVectorComposite::PD_MIN][i].first,
              cache.m_derived[CVectorComposite::PD_MIN][i].second,
              tensor);

  }
  cache.m_bPrincipalValid = true;

  return VectorElement(element, direction, nNodeIndex, map_type, component, cb);
}

template<class TENSOR_TYPE>
void ITensorGroupTemplate<TENSOR_TYPE>::getWellElementRotation(const geo::IElement& elt, geo::IVector& vec)
{
  double x_0 = 0, y_0 = 0, z_0 = 0;
  double x_1 = 0, y_1 = 0, z_1 = 0;

  for (int i = 0; i < 8; ++i)
  {
  const geo::INode& node = elt.Node(i);

  switch (i)
  {
  case 0:
      x_0 += node.X();
      y_1 += node.Y();
      z_0 += node.Z();
      break;
  case 1:
      x_1 += node.X();
      y_1 += node.Y();
      z_0 += node.Z();
      break;
  case 2:
      x_1 += node.X();
      y_0 += node.Y();
      z_0 += node.Z();
      break;
  case 3:
      x_0 += node.X();
      y_0 += node.Y();
      z_0 += node.Z();
      break;
  case 4:
      x_0 += node.X();
      y_1 += node.Y();
      z_1 += node.Z();
      break;
  case 5:
      x_1 += node.X();
      y_1 += node.Y();
      z_1 += node.Z();
      break;
  case 6:
      x_1 += node.X();
      y_0 += node.Y();
      z_1 += node.Z();
      break;
  case 7:
      x_0 += node.X();
      y_0 += node.Y();
      z_1 += node.Z();
      break;
  }
  }

  vec.Set((x_1 - x_0) / 4, (y_1 - y_0) / 4, (z_1 - z_0) / 4);
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal&
ITensorGroupTemplate<TENSOR_TYPE>::CylindricalVectorPoint(const geo::IPoint& point,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  assert(!point.Empty());
  // Is the point in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache(component, cb);
  if ((!cache.m_element && !cache.m_point.Empty() && point == cache.m_point) && cache.m_bCylindricalValid) {
  assert(cache.m_derived[direction].size() == 1);
  return cache.m_derived[direction][0];
  }

  const ITensor& tensor = TensorPoint(point, component, cb);
  if (cache.m_derived[direction].size() != 1) cache.m_derived[direction].resize(1);
  
  const IWellMesh *pMesh = dynamic_cast<const IWellMesh *>(&static_cast<const CModelBase&>(Model()).Mesh());
  if (pMesh)
  {
  std::vector<int> elts = pMesh->Mesh().ElementsAt(point);

  if (!elts.empty())
  {
      geo::CVector vecDir;
      getWellElementRotation(pMesh->Mesh().Element(elts[0]), vecDir);

      CTensor rotated_tensor = RotatedTensor(tensor, vecDir);

      cache.m_derived[direction][0].first = geo::CVector(rotated_tensor.XX(), rotated_tensor.YY(), rotated_tensor.ZZ());
      cache.m_derived[direction][0].second = cache.m_derived[direction][0].first.Length();
  }
  }

  cache.m_bCylindricalValid = true;

  return VectorPoint(point, direction, component, cb);
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal&
ITensorGroupTemplate<TENSOR_TYPE>::CylindricalVectorElement(const geo::IElement& element,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  int nNodeIndex,
  TMapType map_type,
  const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  // Is the element in the cache?
  _ResultCache<TENSOR_TYPE>& cache = GetCache(component, cb);
  if ((cache.m_element == &element) &&
  (map_type == cache.m_map_type) &&
  cache.m_point.Empty() &&
  cache.m_bCylindricalValid) {
  assert(cache.m_value.size() == element.NrOfPoints());
  assert(cache.m_derived[direction].size() == element.NrOfPoints());
  return cache.m_derived[direction][nNodeIndex];
  }

  if (cache.m_derived[direction].size() != element.NrOfPoints()) cache.m_derived[direction].resize(element.NrOfPoints());

  const IWellMesh *pMesh = dynamic_cast<const IWellMesh *>(&static_cast<const CModelBase&>(Model()).Mesh());

  if (pMesh)
  {
  geo::CVector vecDir;
  getWellElementRotation(element, vecDir);

  for (int i = 0; i < element.NrOfPoints(); i++) {
      const ITensor& tensor = TensorElement(element, i, map_type, component, cb);

      CTensor rotated_tensor = RotatedTensor(tensor, vecDir);

      cache.m_derived[direction][i].first = geo::CVector(rotated_tensor.XX(), rotated_tensor.YY(), rotated_tensor.ZZ());
      cache.m_derived[direction][i].second = cache.m_derived[direction][i].first.Length();
  }
  }
  cache.m_bCylindricalValid = true;

  return VectorElement(element, direction, nNodeIndex, map_type, component, cb);
}


template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal&
ITensorGroupTemplate<TENSOR_TYPE>::VectorPoint(const geo::IPoint& point,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  if (direction == CVectorComposite::CYLINDRICAL)
  return CylindricalVectorPoint(point, direction, component, cb);
  else
  return PrincipalVectorPoint(point, direction, component, cb);
}

template<class TENSOR_TYPE>
const typename ITensorGroupTemplate<TENSOR_TYPE>::TPrincipal&
ITensorGroupTemplate<TENSOR_TYPE>::VectorElement(const geo::IElement& element,
  CVectorComposite::PRINCIPAL_DIRECTION direction,
  int nNodeIndex,
  TMapType map_type,
  const IResultComponent& component, geo::IParallelInitializationCallback*cb) const
{
  if (direction == CVectorComposite::CYLINDRICAL)
  return CylindricalVectorElement(element, direction, nNodeIndex, map_type, component, cb);
  else
  return PrincipalVectorElement(element, direction, nNodeIndex, map_type, component, cb);
}





template <class TENSOR_TYPE>
CTensor ITensorGroupTemplate<TENSOR_TYPE>::RotatedTensor(const ITensor& tensor, const geo::IVector& dir) const
{
  CTensor rotated_tensor;

  if(!tensor.Empty())
  {
  // the horizontal component of the maximum dip direction
  geo::CVector vecHorizontal(-dir.X(), -dir.Y(), 0);

  // if dir is not vertical
  if(vecHorizontal != geo::CVector::NullVector)
  {
      // rotate the northing axis to this maximum dip direction first
      double dAngle = geo::CVector::Xaxis.AngleDeg(vecHorizontal);
      // dAngle is the smallest angle, make sure to add 180 deg if necessary
      if(vecHorizontal.Y() < 0)
    dAngle += 180;
      CTensor temp_tensor = tensor.RotateTensorDeg(geo::CVector::Zaxis, dAngle);

      // the 'dir' vector in this new rotated axis system
      geo::CVector dirrot(-vecHorizontal.Length(), 0, dir.Z());

      // we need to rotate the angle between dirrot and the Z axis around the negative easting axis
      dAngle = geo::CVector::Zaxis.AngleDeg(dirrot);
      rotated_tensor = temp_tensor.RotateTensorDeg(-geo::CVector::Yaxis, dAngle);
  }
  else
  {
      rotated_tensor = tensor;
  }
  }

  return rotated_tensor;
}

template <class TENSOR_TYPE>
CTensor ITensorGroupTemplate<TENSOR_TYPE>::OrientedTensor(const geo::IPoint& point, const geo::IVector& dir, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  // the tensor at the requested point
  const ITensor& tensor = TensorPoint(point, component, cb);
  return RotatedTensor(tensor, dir);
}

template <class TENSOR_TYPE>
CTensor ITensorGroupTemplate<TENSOR_TYPE>::OrientedTensor(const geo::IElement& element, int nNodeIndex, TMapType map_type, const geo::IVector& dir, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  const ITensor& tensor = TensorElement(element, nNodeIndex, map_type, component, cb);
  return RotatedTensor(tensor, dir);
}

template <class TENSOR_TYPE>
CTensor ITensorGroupTemplate<TENSOR_TYPE>::WellPathOrientedTensor(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  const well::CWellPoint* pWellPoint;
  const CNewWellPoint* pNewWellPoint;

  const CElementWellPoint* pElementWellPoint = dynamic_cast<const CElementWellPoint*>(&point);
  if(pElementWellPoint)
  {
  pWellPoint = dynamic_cast<const well::CWellPoint*>(&pElementWellPoint->WellPoint());
  pNewWellPoint = dynamic_cast<const CNewWellPoint*>(&pElementWellPoint->WellPoint());
  }
  else
  {
  pWellPoint = dynamic_cast<const well::CWellPoint*>(&point);
  pNewWellPoint = dynamic_cast<const CNewWellPoint*>(&point);
  }
  if (  ( !pWellPoint || pWellPoint->Empty() )
     && ( !pNewWellPoint || pNewWellPoint->Empty() ) // waij TFS 74145
     )
  return CTensor();

  // direction vector of the line element
  geo::CVector vecDir;
  if ( pWellPoint )
  vecDir = pWellPoint->GetDirection();
  else
  vecDir = pNewWellPoint->GetDirection();

  vecDir = vecDir.UnitVector();

  return OrientedTensor(point, vecDir, component, cb);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::AxialValue(const CTensor& rotated_tensor) const
{
  if(rotated_tensor.Empty())
  return geo::CValue();

  return geo::CValue(rotated_tensor.ZZ());
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::WellPathAxialValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  CTensor rotated_tensor = WellPathOrientedTensor(point, component, cb);
  return AxialValue(rotated_tensor);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::RadialMaxValue(const CTensor& rotated_tensor) const
{
  if(rotated_tensor.Empty())
  return geo::CValue();

  double sigx = rotated_tensor.XX();
  double sigy = rotated_tensor.YY();
  double tau  = rotated_tensor.XY();

  double dMax = 0.5 * (sigx + sigy) + sqrt(pow((0.5 * (sigx - sigy)), 2) + tau * tau);
  return geo::CValue(dMax);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::WellPathRadialMaxValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  CTensor rotated_tensor = WellPathOrientedTensor(point, component, cb);
  return RadialMaxValue(rotated_tensor);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::RadialMinValue(const CTensor& rotated_tensor) const
{
  if(rotated_tensor.Empty())
  return geo::CValue();

  double sigx = rotated_tensor.XX();
  double sigy = rotated_tensor.YY();
  double tau  = rotated_tensor.XY();

  double dMin = 0.5 * (sigx + sigy) - sqrt(pow((0.5 * (sigx - sigy)), 2) + tau * tau);
  return geo::CValue(dMin);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::ShearValueInPlane(const CTensor& rotated_tensor) const
{
  if(rotated_tensor.Empty())
  return geo::CValue();

  double sigxz = rotated_tensor.XZ();
  double sigyz = rotated_tensor.YZ();
  double tau = sqrt(sigxz*sigxz + sigyz*sigyz);

  return geo::CValue(tau);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::ShearDipAngleInPlane(const CTensor& rotated_tensor) const
{
  if(rotated_tensor.Empty())
  return geo::CValue();

  double sigxz = rotated_tensor.XZ();
  double sigyz = rotated_tensor.YZ();
  double angle = atan2(fabs(sigyz), fabs(sigxz));

  // convert to degrees
  angle *= 180 / PI;

  return geo::CValue(angle);
}

template <class TENSOR_TYPE>
geo::CValue ITensorGroupTemplate<TENSOR_TYPE>::WellPathRadialMinValuePoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  CTensor rotated_tensor = WellPathOrientedTensor(point, component, cb);
  return RadialMinValue(rotated_tensor);
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::AxialValueElement(const geo::IElement& element, 
                                                                                  TMapType map_type,
                                                                                  const IResultComponent& component,
                                                                                  const geo::IVector& dir, geo::IParallelInitializationCallback *cb) const
{
  int np = element.NrOfNodes();
  if(!np)
  np = element.NrOfPoints();

  IValueDomainScalar::TValueVec vcVal(np);

  int i;
  for(i = 0; i < np; ++i)
  {
  CTensor rotated_tensor = OrientedTensor(element, i, map_type, dir, component, cb);
  vcVal[i] = AxialValue(rotated_tensor);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::RadialMaxValueElement(const geo::IElement& element, 
                                                                                      TMapType map_type,
                                                                                      const IResultComponent& component,
                                                                                      const geo::IVector& dir, geo::IParallelInitializationCallback *cb) const
{
  int np = element.NrOfNodes();
  if(!np)
  np = element.NrOfPoints();

  IValueDomainScalar::TValueVec vcVal(np);

  int i;
  for(i = 0; i < np; ++i)
  {
  CTensor rotated_tensor = OrientedTensor(element, i, map_type, dir, component, cb);
  vcVal[i] = RadialMaxValue(rotated_tensor);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::RadialMinValueElement(const geo::IElement& element, 
                                                                                      TMapType map_type,
                                                                                      const IResultComponent& component,
                                                                                      const geo::IVector& dir, geo::IParallelInitializationCallback *cb) const
{
  int np = element.NrOfNodes();
  if(!np)
  np = element.NrOfPoints();

  IValueDomainScalar::TValueVec vcVal(np);

  int i;
  for(i = 0; i < np; ++i)
  {
  CTensor rotated_tensor = OrientedTensor(element, i, map_type, dir, component, cb);
  vcVal[i] = RadialMinValue(rotated_tensor);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::ShearValueInPlaneElement(const geo::IElement& element,
                                            TMapType map_type,
                                            const IResultComponent& component,
                                            const geo::IVector& dir, geo::IParallelInitializationCallback *cb) const
{
  int np = element.NrOfNodes();
  if(!np)
  np = element.NrOfPoints();

  IValueDomainScalar::TValueVec vcVal(np);

  int i;
  for(i = 0; i < np; ++i)
  {
  CTensor rotated_tensor = OrientedTensor(element, i, map_type, dir, component, cb);
  vcVal[i] = ShearValueInPlane(rotated_tensor);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::ShearDipAngleInPlaneElement(const geo::IElement& element,
                                                                                           TMapType map_type,
                                                                                           const IResultComponent& component,
                                                                                           const geo::IVector& dir, geo::IParallelInitializationCallback *cb) const
{
  int np = element.NrOfNodes();
  if(!np)
  np = element.NrOfPoints();

  IValueDomainScalar::TValueVec vcVal(np);

  int i;
  for(i = 0; i < np; ++i)
  {
  CTensor rotated_tensor = OrientedTensor(element, i, map_type, dir, component, cb);
  vcVal[i] = ShearDipAngleInPlane(rotated_tensor);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::WellPathAxialValueElement(const geo::IElement& element, 
                                                                           TMapType map_type,
                                                                           const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcVal(element.NrOfNodes());

  const IWellMesh *pMesh = dynamic_cast<const IWellMesh *>(&static_cast<const CModelBase&>(Model()).Mesh());
  if (pMesh)
  {
  geo::CVector vecDir = pMesh->WellDirectionAt(element);
  vcVal = AxialValueElement(element, map_type, component, vecDir, cb);
  }
  else
  {
  const geo::ILine* pLine = dynamic_cast<const geo::ILine*>(&element);

  if (pLine)
      vcVal = AxialValueElement(element, map_type, component, geo::CVector(*pLine), cb);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::WellPathRadialMaxValueElement(const geo::IElement& element, 
                                                                           TMapType map_type,
                                                                           const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcVal(element.NrOfNodes());

  const IWellMesh *pMesh = dynamic_cast<const IWellMesh *>(&static_cast<const CModelBase&>(Model()).Mesh());
  if (pMesh)
  {
  geo::CVector vecDir = pMesh->WellDirectionAt(element);
  vcVal = RadialMaxValueElement(element, map_type, component, vecDir, cb);
  }
  else
  {
  const geo::ILine* pLine = dynamic_cast<const geo::ILine*>(&element);
  
  if (pLine)
      vcVal = RadialMaxValueElement(element, map_type, component, geo::CVector(*pLine), cb);
  }

  return vcVal;
}

template <class TENSOR_TYPE>
IValueDomainScalar::TValueVec ITensorGroupTemplate<TENSOR_TYPE>::WellPathRadialMinValueElement(const geo::IElement& element, 
                                                                           TMapType map_type,
                                                                           const IResultComponent& component, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcVal(element.NrOfNodes());

  const IWellMesh *pMesh = dynamic_cast<const IWellMesh *>(&static_cast<const CModelBase&>(Model()).Mesh());
  if (pMesh)
  {
  geo::CVector vecDir = pMesh->WellDirectionAt(element);
  vcVal = RadialMinValueElement(element, map_type, component, vecDir, cb);
  }
  else
  {
  const geo::ILine* pLine = dynamic_cast<const geo::ILine*>(&element);
  
  if (pLine)
      vcVal = RadialMinValueElement(element, map_type, component, geo::CVector(*pLine), cb);
  }

  return vcVal;
}

#endif // !defined(AFX_TENSOR_GROUP_TEMPLATE_H_INCLUDED_)
