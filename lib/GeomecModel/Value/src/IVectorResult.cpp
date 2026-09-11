// IVectorResult.cpp: implementation of the IVectorResult class.
//
//////////////////////////////////////////////////////////////////////

#include "IVectorResult.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "ResultCache.h"
#include "VectorValueSet.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Implementation of IVectorResult
//////////////////////////////////////////////////////////////////////
IVectorResult::IVectorResult(unsigned int uName, CResultGroup &group) : IResult(uName, group) {}

IVectorResult::IVectorResult(const QString &sName, CResultGroup &group) : IResult(sName, group) {}

IVectorResult::~IVectorResult() {}

void IVectorResult::BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) {
  if (OnBuildComponent(stage, antype, nRegister)) {
    // new CFullVectorComponent(*this, stage, antype, nRegister);
    new CVectorComponent(IDS_RC_NORTHING, *this, VC_X, stage, antype, nRegister);
    new CVectorComponent(IDS_RC_EASTING, *this, VC_Y, stage, antype, nRegister);
    new CVectorComponent(IDS_RC_DEPTH, *this, VC_Z, stage, antype, nRegister);
    new CVectorComponent(IDS_RC_LENGTH, *this, VC_LENGTH, stage, antype, nRegister);
    new CFullVectorComponent(*this, stage, antype, nRegister);
  }
}

QString IVectorResult::ExportLabel(int nComponent) const {
  unsigned int uStringId = 0;
  switch (nComponent) {
  case VC_X:
    uStringId = IDS_ET_VECTOR_X;
    break;
  case VC_Y:
    uStringId = IDS_ET_VECTOR_Y;
    break;
  case VC_Z:
    uStringId = IDS_ET_VECTOR_Z;
    break;
  case VC_LENGTH:
    uStringId = IDS_ET_VECTOR_LENGTH;
    break;
  case VC_FULLVECTOR:
    uStringId = IDS_ET_VECTOR_FULL;
    break;
  default:
    assert(false);
    break;
  };

  return getStringTableEntry(uStringId);
}

//////////////////////////////////////////////////////////////////////
// Implementation of the IVectorComponent
//////////////////////////////////////////////////////////////////////
IVectorResult::IVectorComponent::IVectorComponent(unsigned int uComponentName, IVectorResult &parent,
                                                  VECTOR_COMPONENT component, const CDepletionStage &stage,
                                                  const CAnalysisType &antype, int nRegister)
    : IResultComponent(uComponentName, parent, stage, antype, nRegister, (int)component) {}

IVectorResult::IVectorComponent::IVectorComponent(IVectorResult &parent, const CDepletionStage &stage,
                                                  const CAnalysisType &antype, int nRegister)
    : IResultComponent("Full Vector", parent, stage, antype, nRegister, 4) {}

IVectorResult::CVectorComponent::CVectorComponent(unsigned int uComponentName, IVectorResult &parent,
                                                  VECTOR_COMPONENT component, const CDepletionStage &stage,
                                                  const CAnalysisType &antype, int nRegister)
    : IVectorComponent(uComponentName, parent, component, stage, antype, nRegister), m_component(component) {}

unsigned int IVectorResult::CVectorComponent::IconId() const { return IDI_RESULT; }

unsigned int IVectorResult::CVectorComponent::TypeId() const { return 0; }

IVectorResult::VECTOR_COMPONENT IVectorResult::CVectorComponent::Component() const { return m_component; }

QString IVectorResult::CVectorComponent::UnitName(const UNIT unit) const {
  const IVectorResult &result = dynamic_cast<const IVectorResult &>(Parent());
  return result.UnitName(unit);
}

geo::CValue IVectorResult::CVectorComponent::ValuePoint(const geo::IPoint &pt, const UNIT unit,
                                                        geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  return VectorToValue(pResult->VectorPoint(pt, *this, cb), unit);
}

void IVectorResult::CVectorComponent::MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values,
                                                      TMapType map_type, const UNIT unit,
                                                      geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  assert(values.size() == elm.NrOfPoints());
  for (int i = 0; i < values.size(); i++) {
    const geo::IVector &vec = pResult->VectorElement(elm, i, map_type, *this, cb);
    values[i] = VectorToValue(vec, unit);
  }
}

void IVectorResult::CVectorComponent::MapTensorElement(const geo::IElement &elm, std::vector<CTensor> &values,
                                                       TMapType map_type,
                                                       geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  assert(values.size() == elm.NrOfPoints());
  for (int i = 0; i < values.size(); i++) {
    values[i] = pResult->TensorElement(elm, i, map_type, *this, cb);
  }
}

void IVectorResult::CVectorComponent::MapValueElement(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                                      const UNIT /*unit*/,
                                                      geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  vectors.resize(elm.NrOfPoints());
  for (int i = 0; i < vectors.size(); i++) {
    vectors[i] = pResult->VectorElement(elm, i, map_type, *this, cb);
  }
}

void IVectorResult::CVectorComponent::MapValueElementCB(const geo::IElement &elm, TVectorVec &vectors,
                                                        TMapType map_type, const UNIT /*unit*/,
                                                        geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  vectors.resize(elm.NrOfPoints());
  for (int i = 0; i < vectors.size(); i++) {
    vectors[i] = pResult->VectorElement(elm, i, map_type, *this, cb);
  }
}

bool IVectorResult::CVectorComponent::NeedParallelInitializationCallback() const { return true; }

geo::CValue IVectorResult::CVectorComponent::ValueToValue(const geo::CValue &value, UNIT unit) const {
  if (unit == CQuantity::SI_UNIT) {
    return value;
  } else {
    const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
    assert(pResult);
    return pResult->ConvertToField(value.Value());
  }
}

geo::CValue IVectorResult::CVectorComponent::VectorToValue(const geo::IVector &vector, UNIT unit) const {
  if (vector.Empty())
    return geo::CValue();
  if (unit == CQuantity::SI_UNIT) {
    switch (m_component) {
    case VC_X:
      return geo::CValue(vector.X());
    case VC_Y:
      return geo::CValue(vector.Y());
    case VC_Z:
      return geo::CValue(vector.Z());
    case VC_LENGTH:
      return geo::CValue(vector.Length());
    default:
      assert(false);
      // Bogus
    }
  } else {
    const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
    assert(pResult);
    switch (m_component) {
    case VC_X:
      return geo::CValue(pResult->ConvertToField(vector.X()));
    case VC_Y:
      return geo::CValue(pResult->ConvertToField(vector.Y()));
    case VC_Z:
      return geo::CValue(pResult->ConvertToField(vector.Z()));
    case VC_LENGTH:
      return geo::CValue(pResult->ConvertToField(vector.Length()));
    default:
      assert(false);
      // Bogus
    }
  }

  return geo::CValue();
}

//////////////////////////////////////////////////////////////////////
// Implementation of the CFullVectorComponent
//////////////////////////////////////////////////////////////////////

IVectorResult::CFullVectorComponent::CVectorData::CVectorData(const CFullVectorComponent &parent)
    : m_Parent(&parent), m_Min(DBL_MAX), m_Max(-DBL_MAX) {
  assert(m_Parent);
}

void IVectorResult::CFullVectorComponent::CVectorData::SetMinMax(const TValue & /*t*/) const {}

void IVectorResult::CFullVectorComponent::CVectorData::ResetMinMax() const {
  m_Min = DBL_MAX;
  m_Max = -DBL_MAX;
}

// TODO: check why CTensorData's minmax is doubles
IValueDomainVector::TMinMax
IVectorResult::CFullVectorComponent::CVectorData::MinMax(IProgressBase &, const IValueDataInterface::UNIT) const {
  return IValueDomainVector::TMinMax(geo::CVector(m_Min, m_Min, m_Min), geo::CVector(m_Max, m_Max, m_Max));
}

IValueDomainVector::TValue
IVectorResult::CFullVectorComponent::CVectorData::ValuePoint(const geo::IPoint & /*pt*/,
                                                             const IValueDataInterface::UNIT /*unit*/,
                                                             geo::IParallelInitializationCallback * /*cb*/) const {
  return IValueDomainVector::TValue();
}

IValueDomainVector::TValueVec IVectorResult::CFullVectorComponent::CVectorData::ValueElement(
    const geo::IElement &elt, const IValueDataInterface::UNIT unit, geo::IParallelInitializationCallback *cb) const {
  IValueDomainVector::TValueVec vectors;
  m_Parent->MapValueElementCB(elt, vectors, CValueType::MT_POINT, unit, cb);
  return vectors;
}

IValueDomainVector::TValue
IVectorResult::CFullVectorComponent::CVectorData::Average(IProgressBase &, const IValueDataInterface::UNIT) const {
  return IValueDomainVector::TValue();
}

IVectorResult::CFullVectorComponent::CFullVectorComponent(IVectorResult &parent, const CDepletionStage &stage,
                                                          const CAnalysisType &antype, int nRegister)
    : IVectorComponent(parent, stage, antype, nRegister), m_Data(*this) {}

unsigned int IVectorResult::CFullVectorComponent::IconId() const { return IDI_RESULT; }

unsigned int IVectorResult::CFullVectorComponent::TypeId() const { return 0; }

QString IVectorResult::CFullVectorComponent::UnitName(const UNIT unit) const {
  const IVectorResult &result = dynamic_cast<const IVectorResult &>(Parent());
  return result.UnitName(unit);
}

geo::CValue IVectorResult::CFullVectorComponent::ValuePoint(const geo::IPoint & /*pt*/, const UNIT /*unit*/,
                                                            geo::IParallelInitializationCallback * /*cb*/) const {
  return geo::CValue();
}

#if 0
void IVectorResult::CVectorComponent::MapTensorElement(const geo::IElement& elm, std::vector<CTensor> & values, TMapType map_type) const
{
  const IVectorResult *pResult = dynamic_cast<const IVectorResult*>(&Parent());
  assert(values.size() == elm.NrOfPoints());
  for (int i = 0; i < values.size(); i++) {
    values[i] = pResult->TensorElement(elm, i, map_type, *this);
  }
}
#endif

void IVectorResult::CFullVectorComponent::MapValueElement(const geo::IElement & /*elm*/,
                                                          IValueDomainScalar::TValueVec & /*values*/,
                                                          TMapType /*map_type*/, UNIT /*unit*/,
                                                          geo::IParallelInitializationCallback * /*cb*/) const {}

void IVectorResult::CFullVectorComponent::MapValueElement(const geo::IElement &elm, TVectorVec &vectors,
                                                          TMapType map_type, const UNIT /*unit*/,
                                                          geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  vectors.resize(elm.NrOfPoints());
  for (int i = 0; i < vectors.size(); i++) {
    vectors[i] = pResult->VectorElement(elm, i, map_type, *this, cb);
  }
}

void IVectorResult::CFullVectorComponent::MapValueElementCB(const geo::IElement &elm, TVectorVec &vectors,
                                                            TMapType map_type, const UNIT /*unit*/,
                                                            geo::IParallelInitializationCallback *cb) const {
  const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&Parent());
  vectors.resize(elm.NrOfPoints());
  for (int i = 0; i < vectors.size(); i++) {
    vectors[i] = pResult->VectorElement(elm, i, map_type, *this, cb);
  }
}

bool IVectorResult::CFullVectorComponent::NeedParallelInitializationCallback() const { return true; }

IValueComponentBase::TValueComponentType IVectorResult::CFullVectorComponent::Type() const {
  return IValueComponentBase::VECTOR;
}

const IValueDataInterfaceVector *IVectorResult::CFullVectorComponent::VectorData() const { return &m_Data; }

IValueDataInterfaceVector *IVectorResult::CFullVectorComponent::VectorData() { return &m_Data; }

void IVectorResult::CFullVectorComponent::ResetMinMax() const { m_Data.ResetMinMax(); }

CVectorResult::CParallelInitializationCallback::CParallelInitializationCallback(CModelBase &model) : m_model(model) {
  assert(m_cache.size() == 0);
  for (int i = 0; i < m_model.ResultRegisterSize(); i++) {
    TCacheMap cache_map;
    cache_map.insert(TCacheMap::value_type(
        CAnalysisType::AT_LINEAR, TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(
        CAnalysisType::AT_NONLIN, TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(
        CAnalysisType::AT_HEAT, TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(
        CAnalysisType::AT_MIXTURE, TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(
        TCacheMap::value_type(CAnalysisType::AT_MIXTURE_CONTAINMENT,
                              TCache(m_model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    m_cache.push_back(cache_map);
  }

  assert(m_cache.size() == model.ResultRegisterSize());
}

CVectorResult::CParallelInitializationCallback::~CParallelInitializationCallback() {}

CVectorResult::_ResultCache &
CVectorResult::CParallelInitializationCallback::GetCache(const IResultComponent &component) const {
  return m_cache[component.RegisterIndex()][component.AnalysisType()][component.Stage().Index()];
}

//////////////////////////////////////////////////////////////////////
// Implementation of CVectorResult
//////////////////////////////////////////////////////////////////////
CVectorResult::CVectorResult(unsigned int uName, TValueSetFunction function, CResultGroup &group, bool bChange)
    : IVectorResult(uName, group), m_function(function), m_bChange(bChange), m_callback(0) {}

CVectorResult::CVectorResult(const QString &sName, TValueSetFunction function, CResultGroup &group, bool bChange)
    : IVectorResult(sName, group), m_function(function), m_bChange(bChange), m_callback(0) {}

void CVectorResult::BuildCache() const {
  assert(m_cache.size() == 0);
  CModelBase &model = (CModelBase &)Model();
  for (int i = 0; i < model.ResultRegisterSize(); i++) {
    TCacheMap cache_map;
    cache_map.insert(TCacheMap::value_type(CAnalysisType::AT_LINEAR,
                                           TCache(model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(CAnalysisType::AT_NONLIN,
                                           TCache(model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(CAnalysisType::AT_HEAT,
                                           TCache(model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(CAnalysisType::AT_MIXTURE,
                                           TCache(model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    cache_map.insert(TCacheMap::value_type(CAnalysisType::AT_MIXTURE_CONTAINMENT,
                                           TCache(model.ResultRegister(i).DepletionStageEntry().EntryNodes().size())));
    //		TCachePair cache_pair( TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size() ),
    //			                   TCache( model.ResultRegister(i).DepletionStageEntry().EntryNodes().size() ) );
    //		m_cache.push_back(cache_pair);
    m_cache.push_back(cache_map);
  }

  assert(m_cache.size() == model.ResultRegisterSize());
}

/*!
  Clears cache when results become invalid
*/
void CVectorResult::ClearCache() { m_cache.clear(); }

void CVectorResult::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  ClearCache();
  IVectorResult::OnNeighbourModified(node, uHint);
}

CVectorResult::_ResultCache &CVectorResult::GetCache(const IResultComponent &component,
                                                     geo::IParallelInitializationCallback *callback) const {
  CParallelInitializationCallback *cb = dynamic_cast<CParallelInitializationCallback *>(callback);
  if (cb)
    return cb->GetCache(component);

  if (!m_cache.size())
    BuildCache();

  return m_cache[component.RegisterIndex()][component.AnalysisType()][component.Stage().Index()];
}

const geo::IVector &CVectorResult::VectorPoint(const geo::IPoint &point, const IResultComponent &component,
                                               geo::IParallelInitializationCallback *cb) const {
  assert(!point.Empty());
  // Is the point in the cache?
  _ResultCache &cache = GetCache(component, cb);
  if (!cache.m_element && !cache.m_point.Empty() && point == cache.m_point) {
    assert(cache.m_value.size() == 1);
    return cache.m_value[0];
  }

  cache.m_element = 0;
  cache.m_point = point;
  if (cache.m_value.size() != 1)
    cache.m_value.resize(1);
  cache.m_value[0] = (component.ResultRegister().*m_function)(component.Stage(), component.AnalysisType(), Change())
                         .ValuePoint(point, cb);
  return cache.m_value[0];
}

const geo::IVector &CVectorResult::VectorElement(const geo::IElement &element, int nNodeIndex, TMapType map_type,
                                                 const IResultComponent &component,
                                                 geo::IParallelInitializationCallback *cb) const {
  // Is the element in the cache?
  _ResultCache &cache = GetCache(component, cb);
  if ((cache.m_element == &element) && (map_type == cache.m_map_type) && cache.m_point.Empty()) {
    assert(cache.m_value.size() == element.NrOfPoints());
    return cache.m_value[nNodeIndex];
  }

  cache.m_element = &element;
  cache.m_map_type = map_type;
  cache.m_point = geo::CPoint();
  if (cache.m_value.size() != element.NrOfPoints())
    cache.m_value.resize(element.NrOfPoints());
  (component.ResultRegister().*m_function)(component.Stage(), component.AnalysisType(), Change())
      .MapValueElement(element, cache.m_value, map_type, cb);
  return cache.m_value[nNodeIndex];
}

geo::IParallelInitializationCallback *CVectorResult::GetParallelInitializationCallback() {
  return new CParallelInitializationCallback(static_cast<CModelBase &>(Model()));
}

bool CVectorResult::PrepareMapping(const geo::IElementSet * /*pElementSet*/,
                                   const IValueComponentBase * /*pValueComponent*/) {
  return true;
}

void CVectorResult::FinishMapping() {
  static_cast<CModelBase &>(Model()).ResultRegister().Cache().ActiveCacher().EndCriticalSection();
  m_callback = 0;
}
