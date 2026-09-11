#ifndef _CALCULATEDTEMPERATURE_H_
#define _CALCULATEDTEMPERATURE_H_

class IResultComponent;
class CModelBase;
class CFormationBase;
class CDepletionStage;

#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "GeomecModelVisitor.h"

class CCalculatedTemperature : public IValueComposite
{
private:
  class CCalculatedTemperatureComponent : public IValueComponentBase
  {
  public:
    typedef IValueDataInterfaceScalarTempl<CCalculatedTemperatureComponent> IValueDataInterfaceCalcTempScalar;

    CCalculatedTemperatureComponent(CCalculatedTemperature& parent);
    CCalculatedTemperatureComponent(CCalculatedTemperature& parent, CModelBase& ResultSource);

    virtual TValueComponentType Type() const;

    virtual const IValueDataInterfaceScalar& ScalarData() const;
    virtual IValueDataInterfaceScalar& ScalarData();

    virtual QString ExportLabel() const;
    virtual	QString UnitName(const UNIT unit = IQuantityDouble::SI_UNIT) const;
    virtual bool Defined() const;
  	virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const UNIT unit) const;
    virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const UNIT unit) const;
    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;

	  virtual bool Empty() const;
	  virtual long SavedItems() const;
    virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint = Default);

  private:
    const IResultComponent& ResultComponent() const;

  private:
    CCalculatedTemperature& m_parent;
    CModelBase& m_ResultSource; // model from which to take the temperature results value

  protected:
    IValueDataInterfaceCalcTempScalar m_Data;
  };

public:
  CCalculatedTemperature(CFormationBase& formation, CDepletionStage& stage);

  const CDepletionStage& DepletionStage() const;
  CDepletionStage& DepletionStage();

  const CFormationBase* Formation() const;
  CFormationBase* Formation();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Less(const CGraphNode& rhs) const;

	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual void OnNeighbourDeleted(const CGraphNode& node);

  ACCEPT_GEOMECMODELVISITORS(VisitCalculatedTemperature);

private:
  CFormationBase& m_formation;
  CDepletionStage& m_stage;
};

#endif // _CALCULATEDTEMPERATURE_H_
