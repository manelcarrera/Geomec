#ifndef _WELLCASINGINTERNALPRESSURE_H_
#define _WELLCASINGINTERNALPRESSURE_H_

#include "IWellCasingInternalLoad.h"
#include "ValueTypeFactory.h"

class CWellCasingInternalPressure : public IWellCasingInternalLoad<TPressure>
{
private:
  class CWellCasingInternalPressureComponentRepeater;
  class CWellCasingInternalPressureComponentGradient;

public:
  CWellCasingInternalPressure(CDepletionStage& stage);
  CWellCasingInternalPressure(const CWellCasingInternalPressure& rhs);

  CWellCasingInternalPressure& operator=(const CWellCasingInternalPressure& rhs);
  bool operator==(const CWellCasingInternalPressure& rhs) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Less(const CGraphNode& node) const;

  virtual QString LoadName() const;

  virtual const CWellCasingInternalPressure& PreviousStageLoad() const;
  virtual const CWellCasingInternalPressure& NextStageLoad() const;
  virtual geo::CPoint Min() const;
  virtual geo::CPoint Max() const;
  virtual QString TypeName() const;

  virtual bool Empty() const;

  const CWellCasingInternalPressureComponentRepeater& Repeater() const;
  CWellCasingInternalPressureComponentRepeater& Repeater();

  const CWellCasingInternalPressureComponentGradient& Gradient() const;
  CWellCasingInternalPressureComponentGradient& Gradient();

  virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItemsComponents() const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingInternalPressure);

private:
  class CWellCasingInternalPressureComponentRepeater : public IWellCasingInternalLoadComponentRepeater<TPressure>
  {
  public:
  CWellCasingInternalPressureComponentRepeater(CWellCasingInternalPressure& parent);
  CWellCasingInternalPressureComponentRepeater(const CWellCasingInternalPressureComponentRepeater& rhs);
  virtual unsigned int IconId() const;
  virtual double UnitFactor(CQuantity::UNIT unit) const;
  	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  	virtual QString ExportLabel() const;
  };

  class CWellCasingInternalPressureComponentGradient : public IWellCasingInternalLoadComponentGradient<TPressure>
  {
  public:
  CWellCasingInternalPressureComponentGradient(CWellCasingInternalPressure& parent);
  CWellCasingInternalPressureComponentGradient(const CWellCasingInternalPressureComponentGradient& rhs);
  virtual unsigned int IconId() const;
  virtual double UnitFactor(CQuantity::UNIT unit) const;
  	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  	virtual QString ExportLabel() const;
  };

  CWellCasingInternalPressureComponentRepeater* m_pRepeater;
  CWellCasingInternalPressureComponentGradient* m_pGradient;
};

#endif // _WELLCASINGINTERNALPRESSURE_H_
