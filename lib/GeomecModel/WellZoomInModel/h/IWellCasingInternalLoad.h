#ifndef _IWELLCASINGINTERNALLOAD_H_
#define _IWELLCASINGINTERNALLOAD_H_

#include "IPlatform.h"

class CDepletionStage;
class IValueComponentBase;

#include "Point.h"

class CGraphNode;

#include "StorageNode.h"
#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "Box.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "FemAppEntryTypes.h"
#include "IPointSet.h"
#include "ivaluecomponent.h"

template <class VALUETYPE>
class IWellCasingInternalLoad : public IValueComposite
{
public:
  typedef enum
  {
  LM_REPEATER = 0,
  LM_GRADIENT = 1
  } TLoadingMode;

public:
  typedef CStorageNode::TSTREAM TSTREAM;
  typedef CStorageNode::TPROGRESS TPROGRESS;

  IWellCasingInternalLoad(CDepletionStage& stage);
  IWellCasingInternalLoad(const IWellCasingInternalLoad& rhs);

  IWellCasingInternalLoad& operator=(const IWellCasingInternalLoad& rhs);
  bool operator==(const IWellCasingInternalLoad& rhs) const;

  virtual IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0);
  virtual const IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0) const;
  virtual unsigned int ModeSize() const;

  const CDepletionStage& Stage() const;
  CDepletionStage& Stage();

  size_t DistributedSize() const;
  const VALUETYPE& DistributedValue(size_t nIndex) const;

  void DistributedOnly(bool b);
  bool DistributedOnly() const;

  void LoadingMode(TLoadingMode mode);
  TLoadingMode LoadingMode() const;

  virtual QString LoadName() const = 0;

  virtual const IWellCasingInternalLoad<VALUETYPE>& PreviousStageLoad() const = 0;
  virtual const IWellCasingInternalLoad<VALUETYPE>& NextStageLoad() const = 0;
  virtual geo::CPoint Min() const = 0;
  virtual geo::CPoint Max() const = 0;

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual	bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual void Modified(enum ModifiedHint uHint = Default);

  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress) = 0;
  virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress) = 0;
  virtual long SavedItemsComponents() const = 0;

private:
  CDepletionStage& m_stage;
  std::vector<VALUETYPE*> m_vcDistributed;
  bool m_bDistributedOnly;
  TLoadingMode m_LoadingMode;
};

/////

template <class VALUETYPE>
class IWellCasingInternalLoadComponent : public IValueComponentBase
{
public:
  typedef IValueDataInterfaceScalarTempl<IWellCasingInternalLoadComponent<VALUETYPE> > IValueDataInterfaceWellCasingInternalScalar;

  IWellCasingInternalLoadComponent(IWellCasingInternalLoad<VALUETYPE>& parent, unsigned int uMode);
  IWellCasingInternalLoadComponent(const IWellCasingInternalLoadComponent& rhs);
  IWellCasingInternalLoadComponent& operator=(const IWellCasingInternalLoadComponent& rhs);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();

  virtual unsigned int TypeId() const;

  const IWellCasingInternalLoad<VALUETYPE>& ParentLoad() const;
  IWellCasingInternalLoad<VALUETYPE>& ParentLoad();

  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual bool Defined() const;

  virtual bool Empty() const;

  virtual bool CanMap(const COpenGLNode& node) const;

  virtual double UnitFactor(CQuantity::UNIT unit) const = 0;

protected:
  IValueDataInterfaceWellCasingInternalScalar m_Data;
};

/////

template <class VALUETYPE>
class IWellCasingInternalLoadComponentGradient : public IWellCasingInternalLoadComponent<VALUETYPE>
{
public:
  IWellCasingInternalLoadComponentGradient(IWellCasingInternalLoad<VALUETYPE>& parent);
  IWellCasingInternalLoadComponentGradient(const IWellCasingInternalLoadComponentGradient& rhs);

  IWellCasingInternalLoadComponentGradient& operator=(const IWellCasingInternalLoadComponentGradient &rhs);
  bool operator==(const IWellCasingInternalLoadComponentGradient &rhs) const;

  void ReferenceValue(double d);
  double ReferenceValue() const;

  void ReferenceDepth(double d);
  double ReferenceDepth() const;

  void Gradient(double d);
  double Gradient() const;

  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;

private:
  double m_dReferenceValue;
  double m_dReferenceDepth;
  double m_dGradient;
};

/////

template <class VALUETYPE>
class IWellCasingInternalLoadComponentRepeater : public IWellCasingInternalLoadComponent<VALUETYPE>
{
public:
  IWellCasingInternalLoadComponentRepeater(IWellCasingInternalLoad<VALUETYPE>& parent);
  IWellCasingInternalLoadComponentRepeater(const IWellCasingInternalLoadComponentRepeater& rhs);

  const IWellCasingInternalLoadComponent<VALUETYPE>& PreviousStageComponent() const;

  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual long SavedItems() const;
};

///// IWellCasingInternalLoad

template <class VALUETYPE>
IWellCasingInternalLoad<VALUETYPE>::IWellCasingInternalLoad(CDepletionStage& stage)
: IValueComposite(stage.Name().toStdString().c_str(), stage.Model()),
  m_stage(stage),
  m_bDistributedOnly(false),
  m_LoadingMode(stage.Initial() ? LM_GRADIENT : LM_REPEATER)
{
}

template <class VALUETYPE>
IWellCasingInternalLoad<VALUETYPE>::IWellCasingInternalLoad(const IWellCasingInternalLoad& rhs)
: IValueComposite(rhs),
  m_stage(rhs.m_stage),
  m_vcDistributed(rhs.m_vcDistributed),
  m_bDistributedOnly(rhs.m_bDistributedOnly),
  m_LoadingMode(rhs.m_LoadingMode)
{
}

template <class VALUETYPE>
IWellCasingInternalLoad<VALUETYPE>& IWellCasingInternalLoad<VALUETYPE>::operator=(const IWellCasingInternalLoad& rhs)
{
  if(!(*this == rhs))
  {
  IPlatform* platform = IPlatform::instance();

  platform->trace("Start IValueComposite::operator=");
  IValueComposite::operator=(rhs);
  platform->trace("End IValueComposite::operator=");

  m_vcDistributed    = rhs.m_vcDistributed;
  m_bDistributedOnly = rhs.m_bDistributedOnly;
  m_LoadingMode      = rhs.m_LoadingMode;
  }

  return *this;
}

template <class VALUETYPE>
bool IWellCasingInternalLoad<VALUETYPE>::operator==(const IWellCasingInternalLoad& rhs) const
{
  if(IValueComposite::operator==(rhs))
  return false;

  return (
  m_vcDistributed    == rhs.m_vcDistributed    &&
  m_bDistributedOnly == rhs.m_bDistributedOnly &&
  m_LoadingMode      == rhs.m_LoadingMode        );
}

template <class VALUETYPE>
IValueComponentBase& IWellCasingInternalLoad<VALUETYPE>::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/)
{
  return IValueComposite::Component(0, m_LoadingMode);
}

template <class VALUETYPE>
const IValueComponentBase& IWellCasingInternalLoad<VALUETYPE>::Component(unsigned int /*uComponent*/, unsigned int /*uMode*/) const
{
  return IValueComposite::Component(0, m_LoadingMode);
}

template <class VALUETYPE>
unsigned int IWellCasingInternalLoad<VALUETYPE>::ModeSize() const
{
  return 1;
}

template <class VALUETYPE>
const CDepletionStage& IWellCasingInternalLoad<VALUETYPE>::Stage() const
{
  return m_stage;
}

template <class VALUETYPE>
CDepletionStage& IWellCasingInternalLoad<VALUETYPE>::Stage()
{
  return m_stage;
}

template <class VALUETYPE>
size_t IWellCasingInternalLoad<VALUETYPE>::DistributedSize() const
{
  return m_vcDistributed.size();
}

template <class VALUETYPE>
const VALUETYPE& IWellCasingInternalLoad<VALUETYPE>::DistributedValue(size_t nIndex) const
{
  assert(nIndex < m_vcDistributed.size());
  return *m_vcDistributed[nIndex];
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::DistributedOnly(bool b)
{
  m_bDistributedOnly = b;
}

template <class VALUETYPE>
bool IWellCasingInternalLoad<VALUETYPE>::DistributedOnly() const
{
  return m_bDistributedOnly;
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::LoadingMode(TLoadingMode mode)
{
  assert(!m_stage.Initial() || mode == LM_GRADIENT);

  m_LoadingMode = mode;
}

template <class VALUETYPE>
typename IWellCasingInternalLoad<VALUETYPE>::TLoadingMode IWellCasingInternalLoad<VALUETYPE>::LoadingMode() const
{
  return m_LoadingMode;
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::OnNewNeighbour(const CGraphNode &node)
{
  VALUETYPE* pVT = (VALUETYPE*)dynamic_cast<const VALUETYPE*>(&node);
  if(pVT)
  {
  m_vcDistributed.push_back(pVT);

  // only for redrawing purposes
  if(!static_cast<const CModelBase&>(Model()).Loading())
  {
      Modified();
      if(IValueComposite::ModeSize() > 0)
    Component().Modified();
  }
  }

  IValueComposite::OnNewNeighbour(node);
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::OnNeighbourDeleted(const CGraphNode &node)
{
  typename std::vector<VALUETYPE*>::iterator it;
  for(it = m_vcDistributed.begin(); it != m_vcDistributed.end(); ++it)
  {
  assert(!(*it)->IsCopy());
  if(*it == &node)
  {
      m_vcDistributed.erase(it);
      Modified();
      Component().Modified();
      break;
  }
  }

  IValueComposite::OnNeighbourDeleted(node);
}

template <class VALUETYPE>
bool IWellCasingInternalLoad<VALUETYPE>::ConnectItem(const CGraphNode &item)
{
  assert(CanConnectItem(item));

  const VALUETYPE* pVT = dynamic_cast<const VALUETYPE*>(&item);

  if(pVT)
  {
  assert(!IsLinkedTo(*pVT));
  LinkTo(const_cast<VALUETYPE&>(*pVT));
  return true;
  }

  return false;
}

template <class VALUETYPE>
bool IWellCasingInternalLoad<VALUETYPE>::CanConnectItem(const CGraphNode &item) const
{
  const VALUETYPE* pVT = dynamic_cast<const VALUETYPE*>(&item);
  if(pVT)
  return pVT->Component().Defined() && !IsLinkedTo(*pVT);

  return false;
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::Modified(enum ModifiedHint uHint)
{
  // send modified for all dependent (i.e. 'same as previous') loads
  if(!m_stage.Last())
  {
  IWellCasingInternalLoad<VALUETYPE>& NextLoad = const_cast<IWellCasingInternalLoad<VALUETYPE>&>(NextStageLoad());
  if((!NextLoad.DistributedOnly() || NextLoad.DistributedSize() == 0) && NextLoad.LoadingMode() == LM_REPEATER)
      NextLoad.Modified(uHint);
  }

  IValueComposite::Modified(uHint);
}

template <class VALUETYPE>
long IWellCasingInternalLoad<VALUETYPE>::SavedItems() const
{
  long lRet = IValueComposite::SavedItems();
  lRet += m_vcDistributed.size();
  lRet += 2;
  lRet += SavedItemsComponents();
  return lRet;
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*((CModelBase&)Model()).GraphEntry(MD_BASE_VALUE_COMPOSITE);

  IValueComposite::LoadStream(stream, version, progress);

  int n;
  stream >> n;

  int i;
  for(i = 0; i < n; ++i)
  {
  int nIndex;
  stream >> nIndex;
    assert(composite_entry.FindIndex(nIndex));
    VALUETYPE *pValue = (VALUETYPE*)composite_entry.FindIndex(nIndex);
    assert(pValue);
    LinkTo(*pValue);
  progress.Step();
  }

  stream >> n;
  m_bDistributedOnly = (n != 0);
  progress.Step();

  stream >> n;
  m_LoadingMode = (TLoadingMode)n;
  progress.Step();

  LoadComponents(stream, version, progress);
}

template <class VALUETYPE>
void IWellCasingInternalLoad<VALUETYPE>::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IValueComposite::SaveStream(stream, progress);

  stream << int(m_vcDistributed.size());
  for(size_t i = 0; i < m_vcDistributed.size(); ++i)
  {
  stream << m_vcDistributed[i]->Index();
  progress.Step();
  }

  stream << int(m_bDistributedOnly ? 1 : 0);
  progress.Step();

  stream << int(m_LoadingMode);
  progress.Step();

  SaveComponents(stream, progress);
}


///// IWellCasingInternalLoadComponent

template <class VALUETYPE>
IWellCasingInternalLoadComponent<VALUETYPE>::IWellCasingInternalLoadComponent(IWellCasingInternalLoad<VALUETYPE>& parent, unsigned int uMode)
: IValueComponentBase(parent, 0, uMode), m_Data(*this)
{
}

template <class VALUETYPE>
IWellCasingInternalLoadComponent<VALUETYPE>::IWellCasingInternalLoadComponent(const IWellCasingInternalLoadComponent& rhs)
: IValueComponentBase(rhs), m_Data(rhs.m_Data)
{
}

template <class VALUETYPE>
IWellCasingInternalLoadComponent<VALUETYPE>& IWellCasingInternalLoadComponent<VALUETYPE>::operator=(const IWellCasingInternalLoadComponent& rhs)
{
  IValueComponentBase::operator=(rhs);
  return *this;
}

template <class VALUETYPE>
IValueComponentBase::TValueComponentType IWellCasingInternalLoadComponent<VALUETYPE>::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

template <class VALUETYPE>
const IValueDataInterfaceScalar& IWellCasingInternalLoadComponent<VALUETYPE>::ScalarData() const
{
  return m_Data;
}

template <class VALUETYPE>
IValueDataInterfaceScalar& IWellCasingInternalLoadComponent<VALUETYPE>::ScalarData()
{
  return m_Data;
}

template <class VALUETYPE>
unsigned int IWellCasingInternalLoadComponent<VALUETYPE>::TypeId() const
{
  return 0;
}

template <class VALUETYPE>
const IWellCasingInternalLoad<VALUETYPE>& IWellCasingInternalLoadComponent<VALUETYPE>::ParentLoad() const
{
  return static_cast<const IWellCasingInternalLoad<VALUETYPE>&>(Parent());
}

template <class VALUETYPE>
IWellCasingInternalLoad<VALUETYPE>& IWellCasingInternalLoadComponent<VALUETYPE>::ParentLoad()
{
  return static_cast<IWellCasingInternalLoad<VALUETYPE>&>(Parent());
}

template <class VALUETYPE>
IValueDomainScalar::TMinMax IWellCasingInternalLoadComponent<VALUETYPE>::MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
  geo::CBox box(ParentLoad().Min(), ParentLoad().Max());
  return ScalarData().MinMax(progressBase, box, unit);
}

template <class VALUETYPE>
IValueDomainScalar::TValue IWellCasingInternalLoadComponent<VALUETYPE>::Average(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
  geo::CBox box(ParentLoad().Min(), ParentLoad().Max());
  return ScalarData().Average(progressBase, box, unit);
}

template <class VALUETYPE>
IValueDomainScalar::TValueVec IWellCasingInternalLoadComponent<VALUETYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet(elm.NrOfNodes());
  std::vector<int> vcNrOfValue(elm.NrOfNodes(), 0);
  for(size_t i = 0; i < ParentLoad().DistributedSize(); ++i)
  {
  const VALUETYPE& vt = ParentLoad().DistributedValue(i);
  IValueDomainScalar::TValueVec vcValue = vt.Component().ScalarData().ValueElement(elm, unit, cb);
  int n;
  for(n = 0; n < elm.NrOfNodes(); ++n)
  {
      if((ParentLoad().DistributedOnly() || vt.PointSet().PointInConvexHull(elm.Node(n))) && vcValue[n].Valid())
      {
    if(vcNrOfValue[n] == 0)
          vcRet[n] = vcValue[n];
    else
          vcRet[n] = vcRet[n] + vcValue[n];
    ++vcNrOfValue[n];
      }
  }
  }

  for(size_t i = 0; i < elm.NrOfNodes(); ++i)
  {
  if(vcNrOfValue[i] > 1)
      vcRet[i] = vcRet[i] / vcNrOfValue[i];
  }

  return vcRet;
}

template <class VALUETYPE>
IValueDomainScalar::TValue IWellCasingInternalLoadComponent<VALUETYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  double dResult = 0;
  int nValues = 0;
  for(size_t i = 0; i < ParentLoad().DistributedSize(); ++i)
  {
  const VALUETYPE& vt = ParentLoad().DistributedValue(i);
  if(ParentLoad().DistributedOnly() || vt.PointSet().PointInConvexHull(pt))
  {
      geo::CValue val = vt.Component().ScalarData().ValuePoint(pt, unit, cb);
      if(!val.Valid())
    return geo::CValue(); // invalid
      dResult += val.Value();
      ++nValues;
  }
  }

  if(nValues > 1)
  {
  dResult /= nValues;
  return geo::CValue(dResult);
  }

  return geo::CValue();
}

template <class VALUETYPE>
bool IWellCasingInternalLoadComponent<VALUETYPE>::Defined() const
{
  return true;
}

template <class VALUETYPE>
bool IWellCasingInternalLoadComponent<VALUETYPE>::Empty() const
{
  return false;
}

template <class VALUETYPE>
bool IWellCasingInternalLoadComponent<VALUETYPE>::CanMap(const COpenGLNode& /*node*/) const
{
  // only on the casing
  //return dynamic_cast<const CWellCasingSteel*>(&node) != 0;
  return true;
}


///// IWellCasingInternalLoadComponentGradient

template <class VALUETYPE>
IWellCasingInternalLoadComponentGradient<VALUETYPE>::IWellCasingInternalLoadComponentGradient(IWellCasingInternalLoad<VALUETYPE>& parent)
: IWellCasingInternalLoadComponent<VALUETYPE>(parent, IWellCasingInternalLoad<VALUETYPE>::LM_GRADIENT),
  m_dReferenceValue(0),
  m_dReferenceDepth(0),
  m_dGradient(0)
{
}

template <class VALUETYPE>
IWellCasingInternalLoadComponentGradient<VALUETYPE>::IWellCasingInternalLoadComponentGradient(const IWellCasingInternalLoadComponentGradient& rhs)
: IWellCasingInternalLoadComponent<VALUETYPE>(rhs),
  m_dReferenceValue(rhs.m_dReferenceValue),
  m_dReferenceDepth(rhs.m_dReferenceDepth),
  m_dGradient(rhs.m_dGradient)
{
}

template <class VALUETYPE>
IWellCasingInternalLoadComponentGradient<VALUETYPE>& IWellCasingInternalLoadComponentGradient<VALUETYPE>::operator=(const IWellCasingInternalLoadComponentGradient &rhs)
{
  if(!(*this == rhs))
  {
  IWellCasingInternalLoadComponent<VALUETYPE>::operator=(rhs);

  m_dReferenceValue = rhs.m_dReferenceValue;
  m_dReferenceDepth = rhs.m_dReferenceDepth;
  m_dGradient       = rhs.m_dGradient;
  }

  return *this;
}

template <class VALUETYPE>
bool IWellCasingInternalLoadComponentGradient<VALUETYPE>::operator==(const IWellCasingInternalLoadComponentGradient &rhs) const
{
  if(!IWellCasingInternalLoadComponent<VALUETYPE>::operator==(rhs))
  return false;

  return (
  fabs(m_dReferenceValue - rhs.m_dReferenceValue) < 1e-8 &&
  fabs(m_dReferenceDepth - rhs.m_dReferenceDepth) < 1e-8 &&
  fabs(m_dGradient       - rhs.m_dGradient      ) < 1e-8);
}

template <class VALUETYPE>
void IWellCasingInternalLoadComponentGradient<VALUETYPE>::ReferenceValue(double d)
{
  m_dReferenceValue = d;
}

template <class VALUETYPE>
double IWellCasingInternalLoadComponentGradient<VALUETYPE>::ReferenceValue() const
{
  return m_dReferenceValue;
}

template <class VALUETYPE>
void IWellCasingInternalLoadComponentGradient<VALUETYPE>::ReferenceDepth(double d)
{
  m_dReferenceDepth = d;
}

template <class VALUETYPE>
double IWellCasingInternalLoadComponentGradient<VALUETYPE>::ReferenceDepth() const
{
  return m_dReferenceDepth;
}

template <class VALUETYPE>
void IWellCasingInternalLoadComponentGradient<VALUETYPE>::Gradient(double d)
{
  m_dGradient = d;
}

template <class VALUETYPE>
double IWellCasingInternalLoadComponentGradient<VALUETYPE>::Gradient() const
{
  return m_dGradient;
}

template <class VALUETYPE>
IValueDomainScalar::TValueVec IWellCasingInternalLoadComponentGradient<VALUETYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet = IWellCasingInternalLoadComponent<VALUETYPE>::ValueElement(elm, unit, cb);

  int i;
  for(i = 0; i < vcRet.size(); ++i)
  {
  if(!vcRet[i].Valid())
      vcRet[i] = this->UnitFactor(unit) * (m_dReferenceValue + m_dGradient * (elm.Node(i).Z() - m_dReferenceDepth));
  }

  return vcRet;
}

template <class VALUETYPE>
IValueDomainScalar::TValue IWellCasingInternalLoadComponentGradient<VALUETYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue val = IWellCasingInternalLoadComponent<VALUETYPE>::ValuePoint(pt, unit, cb);
  if(!val.Valid())
  val = geo::CValue(this->UnitFactor(unit) * (m_dReferenceValue + m_dGradient * (pt.Z() - m_dReferenceDepth)));

  return val;
}

template <class VALUETYPE>
void IWellCasingInternalLoadComponentGradient<VALUETYPE>::LoadStream(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& progress)
{
  stream >> m_dReferenceValue >> m_dReferenceDepth >> m_dGradient;
  progress.Step();
}

template <class VALUETYPE>
void IWellCasingInternalLoadComponentGradient<VALUETYPE>::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << m_dReferenceValue << m_dReferenceDepth << m_dGradient;
  progress.Step();
}

template <class VALUETYPE>
long IWellCasingInternalLoadComponentGradient<VALUETYPE>::SavedItems() const
{
  return 1;
}


///// IWellCasingInternalLoadComponentRepeater

template <class VALUETYPE>
IWellCasingInternalLoadComponentRepeater<VALUETYPE>::IWellCasingInternalLoadComponentRepeater(IWellCasingInternalLoad<VALUETYPE>& parent)
: IWellCasingInternalLoadComponent<VALUETYPE>(parent, IWellCasingInternalLoad<VALUETYPE>::LM_REPEATER)
{
}

template <class VALUETYPE>
IWellCasingInternalLoadComponentRepeater<VALUETYPE>::IWellCasingInternalLoadComponentRepeater(const IWellCasingInternalLoadComponentRepeater& rhs)
: IWellCasingInternalLoadComponent<VALUETYPE>(rhs)
{
}

template <class VALUETYPE>
const IWellCasingInternalLoadComponent<VALUETYPE>& IWellCasingInternalLoadComponentRepeater<VALUETYPE>::PreviousStageComponent() const
{
  const IWellCasingInternalLoad<VALUETYPE>& parent =
  IWellCasingInternalLoadComponent <VALUETYPE> ::ParentLoad();
  assert(!parent.Stage().Initial());
  return static_cast<const IWellCasingInternalLoadComponent<VALUETYPE>&>(parent.PreviousStageLoad().Component());
}

template <class VALUETYPE>
IValueDomainScalar::TValueVec IWellCasingInternalLoadComponentRepeater<VALUETYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet = IWellCasingInternalLoadComponent<VALUETYPE>::ValueElement(elm, unit, cb);

  bool bHasInvalid = false;
  for(size_t i = 0; i < vcRet.size(); ++i)
  {
  if(!vcRet[i].Valid())
  {
      bHasInvalid = true;
      break;
  }
  }

  if(bHasInvalid)
  {
  IValueDomainScalar::TValueVec vcRetPrev = PreviousStageComponent().ValueElement(elm, unit, cb);
  for(size_t i = 0; i < vcRet.size(); ++i)
  {
      assert(vcRetPrev[i].Valid());
      if(!vcRet[i].Valid())
    vcRet[i] = vcRetPrev[i];
  }
  }

  return vcRet;
}

template <class VALUETYPE>
IValueDomainScalar::TValue IWellCasingInternalLoadComponentRepeater<VALUETYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue val = IWellCasingInternalLoadComponent<VALUETYPE>::ValuePoint(pt, unit, cb);

  if(!val.Valid())
  {
  val = PreviousStageComponent().ValuePoint(pt, unit, cb);
  assert(val.Valid());
  }

  return val;
}

template <class VALUETYPE>
long IWellCasingInternalLoadComponentRepeater<VALUETYPE>::SavedItems() const
{
  return 0;
}


#endif // _IWELLCASINGINTERNALLOAD_H_
