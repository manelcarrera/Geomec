#ifndef _VOLUMETRICSTRAINLOAD_H_
#define _VOLUMETRICSTRAINLOAD_H_

#include "FormationLoadGradientComponent.h"
#include "FormationLoadRepeaterComponent.h"

class TVolumetricStrain;
class CStrainQuantity;
class CVolumetricStrainLoad;

#include "StrainQuantity.h"
#include "GeomecModelVisitor.h"

class CVolumetricStrainLoadGradientComponent : public CFormationLoadGradientComponent<TVolumetricStrain, CStrainQuantity, CStrainQuantity>
{
private:
  typedef CFormationLoadComponent<TVolumetricStrain> TBase;

public:
  CVolumetricStrainLoadGradientComponent(CFormationLoadBase<TVolumetricStrain>& parent, bool bEmpty = false);
  CVolumetricStrainLoadGradientComponent(const CVolumetricStrainLoadGradientComponent& rhs);

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  ACCEPT_GEOMECMODELVISITORS(VisitVolumetricStrainLoadGradientComponent);
};

// just a dummy to satisfy the templates below
class CVolumetricStrainLoadGWCDummyComponent : public IValueComponentBase
{
private: // cannot instantiate
	CVolumetricStrainLoadGWCDummyComponent();
	CVolumetricStrainLoadGWCDummyComponent(CVolumetricStrainLoadGWCDummyComponent& rhs);

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

  ACCEPT_GEOMECMODELVISITORS(VisitVolumetricStrainLoadGWCDummyComponent);
};

class CVolumetricStrainLoadRepeaterComponent : public CFormationLoadRepeaterComponent<TVolumetricStrain, CVolumetricStrainLoadGradientComponent, CVolumetricStrainLoadRepeaterComponent, CVolumetricStrainLoadGWCDummyComponent>
{
public:
	CVolumetricStrainLoadRepeaterComponent(CVolumetricStrainLoad& temperature);
	CVolumetricStrainLoadRepeaterComponent(const CVolumetricStrainLoadRepeaterComponent& rhs);

	// IconId and TypeName
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;
	virtual QString TypeName() const;

	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	virtual QString ExportLabel() const;

	virtual const TFormationLoad& PreviousFormationLoad() const;
  virtual const IValueComponentBase& GlobalValues() const;

  ACCEPT_GEOMECMODELVISITORS(VisitVolumetricStrainLoadRepeaterComponent);

private:
  class CGlobalValuesComponent : public IValueComponentBase
  {
  public:
    typedef IValueDataInterfaceScalarTempl<CGlobalValuesComponent> IValueDataInterfaceGlobalVolumetricStrainScalar;

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
    IValueDataInterfaceGlobalVolumetricStrainScalar m_Data;
  };

  CGlobalValuesComponent m_GlobalValuesComponent;
};

typedef CFormationLoad<TVolumetricStrain, CVolumetricStrainLoadGradientComponent, CVolumetricStrainLoadRepeaterComponent, CVolumetricStrainLoadGWCDummyComponent> TVolumetricStrainLoadBase;

class CVolumetricStrainLoad : public TVolumetricStrainLoadBase
{
private:
  typedef TVolumetricStrainLoadBase TBase;

public:
  CVolumetricStrainLoad(CFormationBase& f, CDepletionStage& d, bool bEmpty);
	CVolumetricStrainLoad(CFormationBase &f, 
			     CDepletionStage &d,
			     const double &dVolStrain,
			     const CQuantity::UNIT unit = CQuantity::SI_UNIT); // Constant initialisation
  CVolumetricStrainLoad(const CVolumetricStrainLoad& rhs);

  CVolumetricStrainLoad& operator=(const CVolumetricStrainLoad& rhs);

	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual bool IsFirstValidDepletionStage() const;

  ACCEPT_GEOMECMODELVISITORS(VisitVolumetricStrainLoad);

private:
	virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress);
	virtual TVolumetricStrainLoadBase& FormationStageLoad(CFormationBase& formation, CDepletionStage& stage);

  class CVolumetricStrainLoadComponent : public CFormationLoadComponent<TVolumetricStrain>
  {
  private:
    typedef CFormationLoadComponent<TVolumetricStrain> TBase;

  public:
    CVolumetricStrainLoadComponent(CVolumetricStrainLoad& parent);
    CVolumetricStrainLoadComponent(const CVolumetricStrainLoadComponent& rhs);

	  virtual unsigned int TypeId() const;
	  virtual unsigned int IconId() const;
	  virtual QString TypeName() const;

	  virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	  virtual QString ExportLabel() const;
  	virtual bool Defined() const;

    virtual long SavedItems() const;

    virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
  };
};

#endif // _VOLUMETRICSTRAINLOAD_H_
