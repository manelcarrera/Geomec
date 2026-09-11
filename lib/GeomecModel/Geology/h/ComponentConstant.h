#ifndef _ComponentConstant_h_
#define _ComponentConstant_h_

#include "FormationLoadBase.h"
#include "FormationLoadGradientComponent.h"
#include "PressureGradient.h"
#include "SinglePressure.h"

template <typename Pressure>
class CComponentConstant
    : public CFormationLoadGradientComponent<Pressure, CSinglePressure, CPressureGradientQuantity> {
public:
  CComponentConstant(CFormationLoadBase<Pressure> &pressure, bool bEmpty = false);
  CComponentConstant(const CComponentConstant<Pressure> &rhs);

  // IconId and TypeName
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  const CSinglePressure &ReferencePressure() const { return this->ReferenceValue(); }
};

template <typename Pressure>
CComponentConstant<Pressure>::CComponentConstant(CFormationLoadBase<Pressure> &pressure, bool bEmpty)
    : CFormationLoadGradientComponent<Pressure, CSinglePressure, CPressureGradientQuantity>(pressure, bEmpty) {}

template <typename Pressure>
CComponentConstant<Pressure>::CComponentConstant(const CComponentConstant<Pressure> &rhs)
    : CFormationLoadGradientComponent<Pressure, CSinglePressure, CPressureGradientQuantity>(rhs) {}

template <typename Pressure> unsigned int CComponentConstant<Pressure>::TypeId() const {
  return IDT_COMPONENT_PRESSURE_CONSTANT;
}

template <typename Pressure> unsigned int CComponentConstant<Pressure>::IconId() const {
  return IDI_COMPONENT_PRESSURE_CONSTANT;
}

template <typename Pressure> QString CComponentConstant<Pressure>::TypeName() const {
  return getStringTableEntry(IDS_COMPONENT_PRESSURE_CONSTANT);
}

template <typename Pressure> QString CComponentConstant<Pressure>::UnitName(const CQuantity::UNIT unit) const {
  QString sRet;
  if (unit == CQuantity::SI_UNIT)
    sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
  else
    sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

  return sRet;
}

template <typename Pressure> QString CComponentConstant<Pressure>::ExportLabel() const {
  return getStringTableEntry(IDS_ET_PRESSURE);
}

#endif // _ComponentConstant_h_
