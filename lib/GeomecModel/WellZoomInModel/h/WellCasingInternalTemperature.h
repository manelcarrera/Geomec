#ifndef _WELLCASINGINTERNALTEMPERATURE_H_
#define _WELLCASINGINTERNALTEMPERATURE_H_

#include "IWellCasingInternalLoad.h"
#include "ValueTypeFactory.h"

class CWellCasingInternalTemperature : public IWellCasingInternalLoad<TTemperature> {
private:
  class CWellCasingInternalTemperatureComponentRepeater;
  class CWellCasingInternalTemperatureComponentGradient;

public:
  CWellCasingInternalTemperature(CDepletionStage &stage);
  CWellCasingInternalTemperature(const CWellCasingInternalTemperature &rhs);

  CWellCasingInternalTemperature &operator=(const CWellCasingInternalTemperature &rhs);
  bool operator==(const CWellCasingInternalTemperature &rhs) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Less(const CGraphNode &node) const;

  virtual QString LoadName() const;

  virtual const CWellCasingInternalTemperature &PreviousStageLoad() const;
  virtual const CWellCasingInternalTemperature &NextStageLoad() const;
  virtual geo::CPoint Min() const;
  virtual geo::CPoint Max() const;
  virtual QString TypeName() const;

  virtual bool Empty() const;
  virtual long SavedItems() const;

  const CWellCasingInternalTemperatureComponentRepeater &Repeater() const;
  CWellCasingInternalTemperatureComponentRepeater &Repeater();

  const CWellCasingInternalTemperatureComponentGradient &Gradient() const;
  CWellCasingInternalTemperatureComponentGradient &Gradient();

  virtual void LoadComponents(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveComponents(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItemsComponents() const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingInternalTemperature);

private:
  class CWellCasingInternalTemperatureComponentRepeater
      : public IWellCasingInternalLoadComponentRepeater<TTemperature> {
  public:
    CWellCasingInternalTemperatureComponentRepeater(CWellCasingInternalTemperature &parent);
    CWellCasingInternalTemperatureComponentRepeater(const CWellCasingInternalTemperatureComponentRepeater &rhs);
    virtual unsigned int IconId() const;
    virtual double UnitFactor(CQuantity::UNIT unit) const;
    virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual QString ExportLabel() const;
  };

  class CWellCasingInternalTemperatureComponentGradient
      : public IWellCasingInternalLoadComponentGradient<TTemperature> {
  public:
    CWellCasingInternalTemperatureComponentGradient(CWellCasingInternalTemperature &parent);
    CWellCasingInternalTemperatureComponentGradient(const CWellCasingInternalTemperatureComponentGradient &rhs);
    virtual unsigned int IconId() const;
    virtual double UnitFactor(CQuantity::UNIT unit) const;
    virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual QString ExportLabel() const;
  };

  CWellCasingInternalTemperatureComponentRepeater *m_pRepeater;
  CWellCasingInternalTemperatureComponentGradient *m_pGradient;
};

#endif // _WELLCASINGINTERNALTEMPERATURE_H_
