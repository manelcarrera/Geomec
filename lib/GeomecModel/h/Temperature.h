#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "FormationLoadGradientComponent.h"
#include "FormationLoadRepeaterComponent.h"

class CTemperature;
class TTemperature;
class CTemperatureQuantity;
class CTemperatureGradientQuantity;

#include "GlobalValueObserver.h"

#include "TemperatureQuantity.h"
#include "TemperatureGradientQuantity.h"

class CTemperatureGradientComponent : public CFormationLoadGradientComponent<TTemperature, CTemperatureQuantity, CTemperatureGradientQuantity>
{
public:
	CTemperatureGradientComponent(CFormationLoadBase<TTemperature> &temperature, bool bEmpty = false);
	CTemperatureGradientComponent(const CTemperatureGradientComponent& rhs);

	// IconId and TypeName
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;
	virtual QString TypeName() const;

	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	virtual QString ExportLabel() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTemperatureGradientComponent);
};

// just a dummy to satisfy the templates below
class CTemperatureGWCDummyComponent : public IValueComponentBase
{
private: // cannot instantiate
	CTemperatureGWCDummyComponent();
	CTemperatureGWCDummyComponent(CTemperatureGWCDummyComponent& rhs);

public:
	const CTemperatureQuantity &ContactPressure() const;
	const CTemperatureQuantity &TransitionPressure() const;
	const CLengthQuantity &ContactDepth() const;
	const CLengthQuantity &TransitionDepth() const;
	const CTemperatureGradientQuantity &UpperGradient() const;
	const CTemperatureGradientQuantity &LowerGradient() const;
	bool TransitionFromPrevious() const;
	void Set(const double &dContactPressure,
			 const double &dTransitionPressure,
			 const double &dContactDepth,
			 const double &dTransitionDepth,
			 const double &dUpperGradient,
			 const double &dLowerGradient,
			 bool bTransitionFromPrevious,
			 const CQuantity::UNIT unit = CQuantity::SI_UNIT);

  ACCEPT_GEOMECMODELVISITORS(VisitTemperatureGWCDummyComponent);
};

class CTemperatureRepeaterComponent : public CFormationLoadRepeaterComponent<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent, CTemperatureGWCDummyComponent>
{
public:
	CTemperatureRepeaterComponent(CTemperature& temperature);
	CTemperatureRepeaterComponent(const CTemperatureRepeaterComponent& rhs);

	// IconId and TypeName
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;
	virtual QString TypeName() const;

	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	virtual QString ExportLabel() const;

	virtual const TFormationLoad& PreviousFormationLoad() const;
	virtual const IValueComponentBase& GlobalValues() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  ACCEPT_GEOMECMODELVISITORS(VisitTemperatureRepeaterComponent);
};

typedef CFormationLoad<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent, CTemperatureGWCDummyComponent> TTemperatureBase;

class CTemperature : public TTemperatureBase
{
public:
	CTemperature(CFormationBase &f, CDepletionStage &d, bool bEmpty = false);
	CTemperature(CFormationBase &f, 
			     CDepletionStage &d,
			     const double &dReferenceTemperature,
			     const double &dReferenceDepth,
			     const double &dGradient,
			     const CQuantity::UNIT unit = CQuantity::SI_UNIT); // Constant initialisation
	CTemperature(CFormationBase &f, 
			     CDepletionStage &d,
			     TTemperature &p);
	CTemperature(const CTemperature& rhs);
  CTemperature& operator=(const CTemperature& rhs);
  virtual ~CTemperature();

	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;

  const CTemperature *ParentTemperature() const;

  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual bool IsFirstValidDepletionStage() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTemperature);

private:
	virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress);
	virtual TTemperatureBase& FormationStageLoad(CFormationBase& formation, CDepletionStage& stage);

  CGlobalValueObserver <CTemperature, CGlobalTemperature> *
    m_GlobalTemperatureObserver;
};

#endif // _TEMPERATURE_H_
