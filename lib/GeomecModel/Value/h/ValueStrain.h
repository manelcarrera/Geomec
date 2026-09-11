#ifndef _VALUESTRAIN_H_
#define _VALUESTRAIN_H_

#include "ValueComponent.h"
#include "ValueType.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

// Define componentmode
template <unsigned int uComponentName,  // Resource string ID of the component name
          unsigned int uComponentIndex> // Index of the component in the value set
class CStrainComponentTemp : public CComponentTemp<uComponentName, uComponentIndex> {
public:
  CStrainComponentTemp(CValueType &value_type, IValueSet &value_set, const Units::CUnitType &unittype)
      : CComponentTemp<uComponentName, uComponentIndex>(value_type, value_set, unittype, Ranges::NoRange) {}

  CStrainComponentTemp(CValueType &value_type, const Units::CUnitType &unittype)
      : CComponentTemp<uComponentName, uComponentIndex>(value_type, unittype, Ranges::NoRange) {}

  virtual QString ExportLabel() const {
    QString sStrain;
    switch (uComponentIndex) {
    case 0:
      sStrain = getStringTableEntry(IDS_ET_VOLUMETRIC_STRAIN);
      break;
    case 1:
      sStrain = getStringTableEntry(IDS_ET_NORMAL_STRAIN);
      break;
    case 2:
      sStrain = getStringTableEntry(IDS_ET_LATERAL_STRAIN);
      break;
    default:
      assert(false);
      break;
    }

    QString sPrefix = CComponentTemp<uComponentName, uComponentIndex>::ExportLabel();

    return sPrefix + sStrain;
  }
};

template <unsigned int uTypeId, unsigned int uExport, unsigned int uIconId, unsigned int uValueNameId>
class CValueStrainTemp : public CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport> {

public:
  CValueStrainTemp(IPointSet &point_set, const QString &strName, const Units::CUnitType &unittype)
      : CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport>(point_set, strName) {
    new CStrainComponentTemp<IDS_COMPONENT_VOLUMETRIC_STRAIN, 0>(*this, unittype);
    new CStrainComponentTemp<IDS_COMPONENT_NORMAL_STRAIN, 1>(*this, unittype);
    new CStrainComponentTemp<IDS_COMPONENT_LATERAL_STRAIN, 2>(*this, unittype);
  }
};

class TStrain
    : public CValueStrainTemp<IDT_VALUETYPE_STRAIN, IDS_ET_STRAIN, IDI_VALUETYPE_STRAIN, IDS_VALUENAME_STRAIN> {
public:
  TStrain(IPointSet &point_set, const QString &strName)
      : CValueStrainTemp<IDT_VALUETYPE_STRAIN, IDS_ET_STRAIN, IDI_VALUETYPE_STRAIN, IDS_VALUENAME_STRAIN>(
            point_set, strName, Units::LengthUnit) {}

  ACCEPT_GEOMECMODELVISITORS(VisitTStrain);
};

#endif