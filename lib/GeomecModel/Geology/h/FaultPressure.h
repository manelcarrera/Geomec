#ifndef _FAULTPRESSURE_H_
#define _FAULTPRESSURE_H_

class CFaultPressureGradientComponent;
class CFaultPressureMinComponent;
class CFaultPressureMaxComponent;
class CFaultPressureMaxGradientComponent;
class CHorizonBase;
class CDepletionStage;
class CFormationBase;

class CFaultPressure;

namespace geo
{
class CInterfaceElement;
class IInterfaceElement;
}

class TPressure;

#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "LengthQuantity.h"
#include "SinglePressure.h"
#include "PressureGradient.h"
#include "GeomecModelVisitor.h"

class CFaultPressureComponent : public IValueComponentBase
{
public:
  typedef IValueDataInterfaceScalarTempl<CFaultPressureComponent> IValueDataInterfaceFaultPressureScalar;

  CFaultPressureComponent(CFaultPressure& faultpressure, unsigned int uMode = 0);
  CFaultPressureComponent(const CFaultPressureComponent& rhs);
  CFaultPressureComponent& operator=(const CFaultPressureComponent& rhs);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();

  const CFaultPressure& FaultPressure() const;
  CFaultPressure& FaultPressure();

  QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual bool Defined() const;

  virtual geo::CValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual bool CanMap(const COpenGLNode& node) const;

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressureComponent);

protected:
  IValueDomainScalar::TValueVec InterfaceValues3D(const geo::CInterfaceElement& iface, bool bMax, const CQuantity::UNIT unit) const;
  IValueDomainScalar::TValueVec InterfaceValues(const geo::IInterfaceElement& iface, bool bMax, const CQuantity::UNIT unit) const;
  std::pair<const geo::IElement*, const geo::IElement*> GetFrontAndBack(const geo::IInterfaceElement& iface) const;
  const CFormationBase& Formation(const geo::IElement& elm) const;

  IValueDataInterfaceFaultPressureScalar m_Data;
};

class CFaultPressure : public IValueComposite
{
private:
  class CComponentForwarder : public CFaultPressureComponent
  {
  public:
    CComponentForwarder(CFaultPressure& faultpressure);
    CComponentForwarder(const CComponentForwarder& rhs);

    virtual unsigned int TypeId() const;
    virtual unsigned int IconId() const;
    virtual QString TypeName() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  private:
    const IValueComponentBase& ActiveComponent() const;
  };

public:
  typedef enum _eModeType
  {
    MT_MAXIMUM = 0,
    MT_MINIMUM = 1,
    MT_GRADIENT = 2,

    // for private use only!
    MT_FORWARDER = 3,

  // newer type(s)
  MT_MAXGRADIENT = 4

  } TModeType;

  CFaultPressure(CHorizonBase& fault, CDepletionStage& dstage, TModeType mode = MT_MINIMUM);
  CFaultPressure(const CFaultPressure& rhs);
  virtual ~CFaultPressure();

  CFaultPressure& operator=(const CFaultPressure& rhs);
  bool operator==(const CFaultPressure& rhs) const;

  TModeType Mode() const;
  void Mode(TModeType mode);

  // components
  const CFaultPressureGradientComponent& Gradient() const;
  CFaultPressureGradientComponent& Gradient();

  const CFaultPressureMinComponent& MinComponent() const;
  CFaultPressureMinComponent& MinComponent();

  const CFaultPressureMaxComponent& MaxComponent() const;
  CFaultPressureMaxComponent& MaxComponent();

  const CFaultPressureMaxGradientComponent& MaxGradientComponent() const;
  CFaultPressureMaxGradientComponent& MaxGradientComponent();

  // distributed values
  size_t DistributedSize() const;
  const TPressure& DistributedValue(size_t nIndex) const;

  // usage of distributed values outside their convex hull
  bool DistributedOnly() const;
  void DistributedOnly(bool bDistributedOnly);

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool CanDisconnectItem(const CGraphNode& item) const;

  // Save and load
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  // Access to modes
  virtual unsigned int ModeSize() const;							// Returns number of modes > 0
  virtual QString ModeName(unsigned int uMode = 0) const;		// Returns name of the mode 

  // Component interface ...
  virtual unsigned int ComponentSize(unsigned int uMode = 0) const;
  virtual IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0);
  virtual const IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0) const;

  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual unsigned int IconId() const;
  virtual const QString &Name() const;
  virtual bool Less(const CGraphNode &node) const;

  const CHorizonBase& Fault() const;
  CHorizonBase& Fault();

  const CDepletionStage& DepletionStage() const;
  CDepletionStage& DepletionStage();

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressure);

private:
  const CComponentForwarder& Forwarder() const;
  CComponentForwarder& Forwarder();

private:
  CHorizonBase& m_fault;
  CDepletionStage& m_stage;
  TModeType m_mode;
  std::vector<const TPressure*> m_vcDistributed;
  bool m_bDistributedOnly;
};

class CFaultPressureGradientComponent : public CFaultPressureComponent
{
public:
  CFaultPressureGradientComponent(CFaultPressure& faultpressure);
  CFaultPressureGradientComponent(const CFaultPressureGradientComponent& rhs);

  CFaultPressureGradientComponent& operator=(const CFaultPressureGradientComponent& rhs);
  bool operator==(const CFaultPressureGradientComponent& rhs) const;

  const CLengthQuantity& ReferenceDepth() const;
  const CSinglePressure& ReferencePressure() const;
  const CPressureGradientQuantity& Gradient() const;

  void ReferenceDepth(const double& value, const CQuantity::UNIT unit = CQuantity::SI_UNIT);
  void ReferencePressure(const double& value, const CQuantity::UNIT unit = CQuantity::SI_UNIT);
  void Gradient(const double& value, const CQuantity::UNIT unit = CQuantity::SI_UNIT);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressureGradientComponent);

private:
  CLengthQuantity m_depth;
  CSinglePressure m_refpressure;
  CPressureGradientQuantity m_gradient;
};

class CFaultPressureMinComponent : public CFaultPressureComponent
{
public:
  CFaultPressureMinComponent(CFaultPressure& faultpressure);
  CFaultPressureMinComponent(const CFaultPressureMinComponent& rhs);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual bool CanComputeOnPoints() {return false;}

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressureMinComponent);
};

class CFaultPressureMaxComponent : public CFaultPressureComponent
{
public:
  CFaultPressureMaxComponent(CFaultPressure& faultpressure);
  CFaultPressureMaxComponent(const CFaultPressureMaxComponent& rhs);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual bool CanComputeOnPoints() {return false;}

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressureMaxComponent);
};

class CFaultPressureMaxGradientComponent : public CFaultPressureComponent
{
public:
  CFaultPressureMaxGradientComponent(CFaultPressure& faultpressure);
  CFaultPressureMaxGradientComponent(const CFaultPressureMaxGradientComponent& rhs);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual bool CanComputeOnPoints() {return false;}

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  ACCEPT_GEOMECMODELVISITORS(VisitFaultPressureMaxGradientComponent);

private:
  void DetermineValues() const;

private:
  mutable CSinglePressure m_refpressure;
  mutable CPressureGradientQuantity m_gradient;
};

#endif // _FAULTPRESSURE_H_
