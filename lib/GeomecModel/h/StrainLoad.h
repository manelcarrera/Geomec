#ifndef _STRAINLOAD_H_
#define _STRAINLOAD_H_

#include "FormationLoadGradientComponent.h"
#include "FormationLoadRepeaterComponent.h"
#include "GeomecModelVisitor.h"

class CStrainQuantity;

namespace geo
{

class IParallelInitializationCallback;

} // namespace geo

#include "StrainQuantity.h"
#include "ValueType.h"
#include "ValueStrain.h"
#include "ValueTensor.h"

class CStrainType
{
public:
  virtual unsigned int FormationTypeIndex() const { return (unsigned int)-1; }
  
  virtual unsigned int TypeId() const { return 0; }
  virtual unsigned int GradientTypeId() const { return 0; }
  virtual unsigned int RepeaterTypeId() const { return 0; }

  virtual unsigned int GradientIconId() const { return 0; }
  virtual unsigned int InitialIconId() const { return 0; }
  virtual unsigned int ChangeIconId() const { return 0; }
  virtual unsigned int RepeaterIconId() const { return 0; }

  virtual QString TypeName() const { return ""; }
  virtual QString GradientTypeName() const { return ""; }
  virtual QString RepeaterTypeName() const { return ""; }

  virtual QString ExportLabel() const { return ""; }
};

class CNormalStrainType : public CStrainType
{
public:
  virtual unsigned int FormationTypeIndex() const { return 0; }

  virtual unsigned int TypeId() const { return IDT_VALUETYPE_NRMSTRAIN; }
  virtual unsigned int GradientTypeId() const { return IDT_COMPONENT_NRMSTRAIN_GRADIENT; }
  virtual unsigned int RepeaterTypeId() const { return IDT_COMPONENT_NRMSTRAIN_REPEATER; }

  virtual unsigned int GradientIconId() const { return IDI_COMPONENT_NRMSTRAIN_GRADIENT; }
  virtual unsigned int InitialIconId() const { return IDI_INITIAL_NRMSTRAIN; }
  virtual unsigned int ChangeIconId() const { return IDI_COMPONENT_NRMSTRAIN_REPEATER_CHANGE; }
  virtual unsigned int RepeaterIconId() const { return IDI_COMPONENT_NRMSTRAIN_REPEATER; }

  virtual QString TypeName() const { return getStringTableEntry(IDS_VALUENAME_NRMSTRAIN); }
  virtual QString GradientTypeName() const { return getStringTableEntry(IDS_COMPONENT_NRMSTRAIN_GRADIENT); }
  virtual QString RepeaterTypeName() const { return getStringTableEntry(IDS_COMPONENT_NRMSTRAIN_REPEATER); }

  virtual QString ExportLabel() const { return getStringTableEntry(IDS_ET_NRMSTRAIN); }
};

class CLateralStrainType : public CStrainType
{
public:
  virtual unsigned int FormationTypeIndex() const { return 1; }

  virtual unsigned int TypeId() const { return IDT_VALUETYPE_LATSTRAIN; }
  virtual unsigned int GradientTypeId() const { return IDT_COMPONENT_LATSTRAIN_GRADIENT; }
  virtual unsigned int RepeaterTypeId() const { return IDT_COMPONENT_LATSTRAIN_REPEATER; }

  virtual unsigned int GradientIconId() const { return IDI_COMPONENT_LATSTRAIN_GRADIENT; }
  virtual unsigned int InitialIconId() const { return IDI_INITIAL_LATSTRAIN; }
  virtual unsigned int ChangeIconId() const { return IDI_COMPONENT_LATSTRAIN_REPEATER_CHANGE; }
  virtual unsigned int RepeaterIconId() const { return IDI_COMPONENT_LATSTRAIN_REPEATER; }

  virtual QString TypeName() const { return getStringTableEntry(IDS_VALUENAME_LATSTRAIN); }
  virtual QString GradientTypeName() const { return getStringTableEntry(IDS_COMPONENT_LATSTRAIN_GRADIENT); }
  virtual QString RepeaterTypeName() const { return getStringTableEntry(IDS_COMPONENT_LATSTRAIN_REPEATER); }

  virtual QString ExportLabel() const { return getStringTableEntry(IDS_ET_LATSTRAIN); }
};


class CStrainLoadRepeaterComponent;
class CStrainLoadGWCDummyComponent;
class CStrainLoad;


class CStrainLoadGradientComponent : public CFormationLoadGradientComponent<TStrain, CStrainQuantity, CStrainQuantity>
{
private:
  typedef CFormationLoadComponent<TStrain> TBase;
  CStrainType *m_Type;
  
  bool m_UseVolumetric;
  bool m_UseInPlane;

  CStrainQuantity m_ReferenceValueNormal;
  CStrainQuantity m_ReferenceValueLateral;

public:
  CStrainLoadGradientComponent(CStrainType &type, CFormationLoad<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>& parent, bool bEmpty = false);
  CStrainLoadGradientComponent(const CStrainLoadGradientComponent& rhs);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  virtual bool UseVolumetric() const;
  virtual void UseVolumetric(bool useVolumetric);
  virtual bool UseInPlane() const;
  virtual void UseInPlane(bool useInPlane);

  virtual const CStrainQuantity& ReferenceValueNormal() const;
  virtual const CStrainQuantity& ReferenceValueLateral() const;

  virtual void ReferenceValueNormal(double value, const CQuantity::UNIT unit = CQuantity::SI_UNIT);
  virtual void ReferenceValueLateral(double value, const CQuantity::UNIT unit = CQuantity::SI_UNIT);

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual bool operator==(const CStrainLoadGradientComponent &rhs) const;

  ACCEPT_GEOMECMODELVISITORS(VisitStrainLoadGradientComponent);
};



// just a dummy to satisfy the templates below
class CStrainLoadGWCDummyComponent : public IValueComponentBase
{
private: // cannot instantiate
  CStrainLoadGWCDummyComponent();
  CStrainLoadGWCDummyComponent(CStrainLoadGWCDummyComponent& rhs);

public:
  const CStrainQuantity &ContactPressure() const;
  const CStrainQuantity &TransitionPressure() const;
  const CLengthQuantity &ContactDepth() const;
  const CLengthQuantity &TransitionDepth() const;
  const CStrainQuantity &UpperGradient() const;
  const CStrainQuantity &LowerGradient() const;
  bool TransitionFromPrevious() const;
  void Set(const double &dContactPressure,
       const double &dTransitionPressure,
       const double &dContactDepth,
       const double &dTransitionDepth,
       const double &dUpperGradient,
       const double &dLowerGradient,
       bool bTransitionFromPrevious,
       const CQuantity::UNIT unit = CQuantity::SI_UNIT);

  ACCEPT_GEOMECMODELVISITORS(VisitStrainLoadGWCDummyComponent);
};



class CStrainLoadRepeaterComponent : public CFormationLoadRepeaterComponent<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>
{
  class CTensorData : public IValueDataInterfaceTensor
  {
  const CStrainLoad& m_Parent;
  const CStrainLoadRepeaterComponent& m_Component;

  mutable double m_Min;
  mutable double m_Max;

  // TODO: check whether this is a meaningful way to determine some "min" and "max" values of tensors
  void SetMinMax(const TValue& t) const;

  public:
  CTensorData(const CStrainLoad& parent, const CStrainLoadRepeaterComponent& component);

  virtual void ResetMinMax() const;

  using IValueDataInterfaceTempl<IValueDataInterface::TENSOR, IValueDomainTensor>::MinMax;
  virtual TMinMax MinMax(IProgressBase &,const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
  virtual TValue ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual TValueVec ValueElement(const geo::IElement &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual TValue Average(IProgressBase&, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
  };

  CTensorData m_TensorData;

  class CScalarData : public IValueDataInterfaceScalar
  {
  const CStrainLoad& m_Parent;
  const CStrainLoadRepeaterComponent& m_Component;

  public:
  CScalarData(const CStrainLoad& parent, const CStrainLoadRepeaterComponent& component);

  //using IValueDataInterfaceTempl<IValueDataInterface::SCALAR, IValueDomainSCALAR>::MinMax;
  virtual TMinMax MinMax(IProgressBase &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
  virtual TValue ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual TValueVec ValueElement(const geo::IElement &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual TValue Average(IProgressBase&, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
  };

  CScalarData m_ScalarData;

  CStrainType *m_Type;
public:
  typedef std::vector<CTensor> TTensorVec;

  CStrainLoadRepeaterComponent(CStrainType &type, CStrainLoad& strain);
  CStrainLoadRepeaterComponent(const CStrainLoadRepeaterComponent& rhs);

  // IconId and TypeName
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;
  virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  virtual const TFormationLoad& PreviousFormationLoad() const;
  virtual const IValueComponentBase& GlobalValues() const;

  virtual IValueComponentBase::TValueComponentType Type() const;
  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();
  virtual const IValueDataInterfaceTensor* TensorData() const;
  virtual IValueDataInterfaceTensor* TensorData();

  ACCEPT_GEOMECMODELVISITORS(VisitStrainLoadRepeaterComponent);


private:
  class CGlobalValuesComponent : public IValueComponentBase
  {
  public:
  typedef IValueDataInterfaceScalarTempl<CGlobalValuesComponent> IValueDataInterfaceGlobalStrainScalar;

  CGlobalValuesComponent(IValueComposite& parent);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();

  virtual QString ExportLabel() const;
  virtual	QString UnitName(const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual bool Defined() const;
  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Empty() const;
  virtual long SavedItems() const;

  protected:
      IValueDataInterfaceGlobalStrainScalar m_Data;
  };

  CGlobalValuesComponent m_GlobalValuesComponent;
};



class CStrainLoad : public CFormationLoad<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent>
{
private:
  typedef CFormationLoad<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent, CStrainLoadGWCDummyComponent> TBase;
  CStrainType *m_Type;

  // Temporary solution; this should use m_vcDistributed, but that one is based on the derived VALUE_TYPE, whereas we want to use a base class of that
  std::vector<const CGraphNode*> m_vcDistributedNodes;

  // Important for Diana, as we need to write full tensor differently than normal/lateral
  // set after a call to ValueTensor
  mutable bool m_bEncounteredFullTensor;

  struct TmpTensorValue
  {
  double value;
  bool   empty;

  TmpTensorValue() : value(0), empty(true) {}

  void Value(double v) { value = v; empty = false; }
  };

  struct TmpTensor
  {
  TmpTensorValue values[6];
  };

public:

  CStrainLoad(CStrainType &t, CFormationBase& f, CDepletionStage& d, bool bEmpty);
  CStrainLoad(CStrainType &t, CFormationBase &f, 
           CDepletionStage &d,
           const double &dVolStrain,
           const CQuantity::UNIT unit = CQuantity::SI_UNIT); // Constant initialisation
  CStrainLoad(const CStrainLoad& rhs);

  CStrainLoad& operator=(const CStrainLoad& rhs);

  bool EncounteredFullTensor() const;
  bool IsTensor() const;
  bool IsVolumetric() const;
  bool IsPlanar() const;
  bool IsPlanar(bool &fullTensor) const;

  const CStrainLoad *ParentStrainLoad() const;

  virtual bool IsSimilar(const CStrainLoad& rhs, bool bCheckDistribution = true) const;

  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  template<typename TYPE>
  const TYPE* StrainComponent() const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual bool IsFirstValidDepletionStage() const;

  virtual std::vector<CTensor> ValueTensor(const geo::IElement &elm, bool &bIsStrainTensor) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm, geo::IParallelInitializationCallback *cb = 0) const;

  virtual size_t DistributedSize() const
  {
    return m_vcDistributedNodes.size();
  }

  virtual const CGraphNode& DistributedValue(size_t nIndex) const
  {
    assert(nIndex < DistributedSize());
    assert(!m_vcDistributedNodes[nIndex]->IsCopy());

    if(IsCopy() && m_vcDistributedNodes[nIndex]->IsCopied())
      return m_vcDistributedNodes[nIndex]->Copy();

    return *m_vcDistributedNodes[nIndex];
  }

  ACCEPT_GEOMECMODELVISITORS(VisitStrainLoad);

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *);

protected:
  virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress);
  virtual CStrainLoad& FormationStageLoad(CFormationBase& formation, CDepletionStage& stage);

  class CStrainLoadComponent : public CFormationLoadComponent<TStrain>
  {
  private:
  typedef CFormationLoadComponent<TStrain> TBase;

  public:
  CStrainLoadComponent(CStrainLoad& parent);
  CStrainLoadComponent(const CStrainLoadComponent& rhs);

    virtual unsigned int TypeId() const;
    virtual unsigned int IconId() const;
    virtual QString TypeName() const;

    virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual QString ExportLabel() const;
  	virtual bool Defined() const;

  virtual long SavedItems() const;
  };
};




//typedef CStrainLoad<TStrain> CStrainLoad;


#endif // _STRAINLOAD_H_
