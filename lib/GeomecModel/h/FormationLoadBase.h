#ifndef _FORMATIONLOADBASE_H_
#define _FORMATIONLOADBASE_H_

#include "FemAppEntryTypes.h"
#include "ivaluecomposite.h"

class CFormationBase;

#include "ModelBase.h"
#include "StreamVersion.h"
#ifdef IMPLEMENT_NEW_CH_CHECK
#include "ConvexHullCheck.h"
#endif

template <class VALUE_TYPE> class CFormationLoadBase : public IValueComposite {
  std::vector<VALUE_TYPE *> m_vcDistributed;
  bool m_bDistributedOnly;

public:
  enum eModeType { CONSTANT_MODE = 0, REPEAT_MODE, GWC_MODE };

public:
  // Construction / Destruction
  CFormationLoadBase(const QString &strName, CFemAppModel &model);
  CFormationLoadBase(unsigned int uName, CFemAppModel &model);
  CFormationLoadBase(const CFormationLoadBase &rhs);
  virtual ~CFormationLoadBase();

  // Assignment and equal
  bool operator==(const CFormationLoadBase &rhs) const;
  CFormationLoadBase &operator=(const CFormationLoadBase &rhs);

  // Distributed values
  size_t DistributedSize() const;
  const VALUE_TYPE &DistributedValue(size_t nIndex) const;

  // Usage of distributed value outside the hull
  void DistributedOnly(bool bDistributedOnly);
  bool DistributedOnly() const;

  // Graph handlers
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;

  // Area of definition
  virtual geo::CPoint Min() const = 0;
  virtual geo::CPoint Max() const = 0;

  // Save and load
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  virtual bool ReadOnly() const;

  virtual const CFormationBase *Formation() const;
  virtual CFormationBase *Formation();

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *pComponent);
};

template <class VALUE_TYPE>
CFormationLoadBase<VALUE_TYPE>::CFormationLoadBase(const QString &strName, CFemAppModel &model)
    : IValueComposite(strName, model), m_vcDistributed(), m_bDistributedOnly(false) {}

template <class VALUE_TYPE>
CFormationLoadBase<VALUE_TYPE>::CFormationLoadBase(unsigned int uName, CFemAppModel &model)
    : IValueComposite(uName, model), m_vcDistributed(), m_bDistributedOnly(false) {}

template <class VALUE_TYPE>
CFormationLoadBase<VALUE_TYPE>::CFormationLoadBase(const CFormationLoadBase &rhs)
    : IValueComposite(rhs), m_vcDistributed(rhs.m_vcDistributed), m_bDistributedOnly(rhs.m_bDistributedOnly) {}

template <class VALUE_TYPE> CFormationLoadBase<VALUE_TYPE>::~CFormationLoadBase() {}

template <class VALUE_TYPE> bool CFormationLoadBase<VALUE_TYPE>::operator==(const CFormationLoadBase &rhs) const {
  if (!IValueComposite::operator==(rhs))
    return false;

  return ((m_vcDistributed == rhs.m_vcDistributed) && (m_bDistributedOnly == rhs.m_bDistributedOnly));
}

template <class VALUE_TYPE>
CFormationLoadBase<VALUE_TYPE> &CFormationLoadBase<VALUE_TYPE>::operator=(const CFormationLoadBase &rhs) {
  IValueComposite::operator=(rhs);

  m_vcDistributed = rhs.m_vcDistributed;
  m_bDistributedOnly = rhs.m_bDistributedOnly;

  return *this;
}

template <class VALUE_TYPE> size_t CFormationLoadBase<VALUE_TYPE>::DistributedSize() const {
  return m_vcDistributed.size();
}

template <class VALUE_TYPE> const VALUE_TYPE &CFormationLoadBase<VALUE_TYPE>::DistributedValue(size_t nIndex) const {
  assert(nIndex < DistributedSize());
  assert(!m_vcDistributed[nIndex]->IsCopy());

  if (IsCopy() && m_vcDistributed[nIndex]->IsCopied())
    return (const VALUE_TYPE &)(m_vcDistributed[nIndex]->Copy());

  return *m_vcDistributed[nIndex];
}

template <class VALUE_TYPE> void CFormationLoadBase<VALUE_TYPE>::OnNewNeighbour(const CGraphNode &node) {
  VALUE_TYPE *pValue = (VALUE_TYPE *)(dynamic_cast<const VALUE_TYPE *>(&node));
  if (pValue) {
    m_vcDistributed.push_back(pValue);
    Modified();

    if (IValueComposite::ModeSize() > 0)
      Component().Modified();
  }

  IValueComposite::OnNewNeighbour(node);
}

template <class VALUE_TYPE> void CFormationLoadBase<VALUE_TYPE>::OnNeighbourDeleted(const CGraphNode &node) {
  for (typename std::vector<VALUE_TYPE *>::iterator it = m_vcDistributed.begin(); it != m_vcDistributed.end(); it++) {
    assert(!(*it)->IsCopy());
    if (*it == &node) {
      m_vcDistributed.erase(it);
      Modified();
      Component().Modified();
      break;
    }
  }

  IValueComposite::OnNeighbourDeleted(node);
}

template <class VALUE_TYPE> bool CFormationLoadBase<VALUE_TYPE>::DistributedOnly() const { return m_bDistributedOnly; }

template <class VALUE_TYPE> void CFormationLoadBase<VALUE_TYPE>::DistributedOnly(bool bDistributedOnly) {
  m_bDistributedOnly = bDistributedOnly;
}

template <class VALUE_TYPE> bool CFormationLoadBase<VALUE_TYPE>::ConnectItem(const CGraphNode &item) {
  assert(CanConnectItem(item));

  const VALUE_TYPE *pDistri = dynamic_cast<const VALUE_TYPE *>(&item);

  if (pDistri) {
    assert(!IsLinkedTo(*pDistri));
    LinkTo(const_cast<VALUE_TYPE &>(*pDistri));
    return true;
  }

  return false;
}

template <class VALUE_TYPE> bool CFormationLoadBase<VALUE_TYPE>::CanConnectItem(const CGraphNode &item) const {
  const VALUE_TYPE *pDistri = dynamic_cast<const VALUE_TYPE *>(&item);

  if (pDistri)
    return (pDistri->Component().Defined()) && (!IsLinkedTo(*pDistri));

  return false;
}

template <class VALUE_TYPE>
void CFormationLoadBase<VALUE_TYPE>::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  TValueCompositeEntry &composite_entry =
      (TValueCompositeEntry &)*(static_cast<CModelBase &>(Model())).RootModel().GraphEntry(MD_BASE_VALUE_COMPOSITE);

  // Pre defined and extrapolated are saved here from 3.0.21
  if (CStreamVersion(3, 0, 20) < version) {
    int distriOnly;
    stream >> distriOnly;
    DistributedOnly(distriOnly != 0);
  }

  // Load the size of the disbuted values
  int distriSize;
  stream >> distriSize;

  // Load and link indices
  for (int i = 0; i < distriSize; i++) {

    int nIndex;
    stream >> nIndex;
    composite_entry.LinkNodeToIndex(*this, nIndex);
    /*
        assert(composite_entry.FindIndex(nIndex));
        VALUE_TYPE *pValue = (VALUE_TYPE*)composite_entry.FindIndex(nIndex);
        assert(pValue);
        LinkTo(*pValue);
    */
  }

  if (CStreamVersion(4, 1, 38) < version) {
    IValueComposite::LoadStream(stream, version, progress);
  }
}

template <class VALUE_TYPE> void CFormationLoadBase<VALUE_TYPE>::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  // Pre defined and extrapolated are saved here from 3.0.21
  int distriOnly = DistributedOnly();
  stream << distriOnly;

  // Save size of distributed values
  int distributedSize = (long)DistributedSize();
  stream << distributedSize;
  for (size_t i = 0; i < DistributedSize(); i++) {
    stream << DistributedValue(i).Index();
    progress.Step();
  }

  IValueComposite::SaveStream(stream, progress);
}

template <class VALUE_TYPE> long CFormationLoadBase<VALUE_TYPE>::SavedItems() const { return (long)DistributedSize(); }

template <class VALUE_TYPE> bool CFormationLoadBase<VALUE_TYPE>::ReadOnly() const { return false; }

template <class VALUE_TYPE> const CFormationBase *CFormationLoadBase<VALUE_TYPE>::Formation() const { return 0; }

template <class VALUE_TYPE> CFormationBase *CFormationLoadBase<VALUE_TYPE>::Formation() { return 0; }

template <class VALUE_TYPE>
bool CFormationLoadBase<VALUE_TYPE>::PrepareMapping(const geo::IElementSet *pElementSet,
                                                    const IValueComponentBase * /*pValueComponent*/) {
  bool retval = true;

  for (size_t i = 0; i < DistributedSize(); ++i) {
    if (!const_cast<IPointSet &>(DistributedValue(i).PointSet()).PrepareMapping(pElementSet))
      retval = false;
  }

  return retval;
}

#ifdef IMPLEMENT_NEW_CH_CHECK
template <typename VALUE_TYPE> class FormationBaseTemplDistributedSetWrapper : public IDistributedSetWrapper {
  const CFormationLoadBase<VALUE_TYPE> &parent;

public:
  FormationBaseTemplDistributedSetWrapper(const CFormationLoadBase<VALUE_TYPE> &formationBase)
      : parent(formationBase) {}

  virtual size_t DistributedSize() { return parent.DistributedSize(); }

  virtual const IPointSet &DistributedPointset(size_t index) { return parent.DistributedValue(index).PointSet(); }

  virtual void DistributedValue(size_t index, const geo::IElement &elm, const CDoubleQuantity::UNIT unit,
                                IValueDomainScalar::TValueVec &vcValues) {
    vcValues = parent.DistributedValue(index).Component().ScalarData().ValueElement(elm, unit);
  }

  virtual void DistributedValue(size_t index, const geo::IElement &elm, const CDoubleQuantity::UNIT unit,
                                IValueDomainVector::TValueVec &vcValues) {}
  virtual void DistributedValue(size_t index, const geo::IElement &elm, const CDoubleQuantity::UNIT unit,
                                IValueDomainTensor::TValueVec &vcValues) {}

  virtual bool DistributedOnly() { return parent.DistributedOnly(); }
  virtual bool DoAveraging() { return true; }
};
#endif

#endif // _FORMATIONLOADBASE_H_
