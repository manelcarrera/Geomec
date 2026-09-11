// ValueVector.h: interface for the CValueVectorTemp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUEVECTOR_H__9FE10D95_95EA_422F_8E7F_3E667253916F__INCLUDED_)
#define AFX_VALUEVECTOR_H__9FE10D95_95EA_422F_8E7F_3E667253916F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueComponent.h"
#include "ValueType.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

// Define componentmode
template <unsigned int uComponentName,  // Resource string ID of the component name
          unsigned int uComponentIndex> // Index of the component in the value set
class CVectorComponentTemp : public CComponentTemp<uComponentName, uComponentIndex> {
public:
  CVectorComponentTemp(CValueType &value_type, IValueSet &value_set, const Units::CUnitType &unittype)
      : CComponentTemp<uComponentName, uComponentIndex>(value_type, value_set, unittype, Ranges::NoRange) {}

  CVectorComponentTemp(CValueType &value_type, const Units::CUnitType &unittype)
      : CComponentTemp<uComponentName, uComponentIndex>(value_type, unittype, Ranges::NoRange) {}

  virtual QString ExportLabel() const {
    QString sTensor;
    switch (uComponentIndex) {
    case 0:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_XX);
      break;
    case 1:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_YY);
      break;
    case 2:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_ZZ);
      break;
    default:
      assert(false);
      break;
    }

    QString sPrefix = CComponentTemp<uComponentName, uComponentIndex>::ExportLabel();

    return sPrefix + sTensor;
  }
};

template <unsigned int uTypeId, unsigned int uExport, unsigned int uIconId, unsigned int uValueNameId>
class CValueVectorTemp : public CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport> {

public:
  CValueVectorTemp(IPointSet &point_set, const QString &strName, const Units::CUnitType &unittype)
      : CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport>(point_set, strName) {
    new CVectorComponentTemp<IDS_COMPONENT_XX, 0>(*this, unittype);
    new CVectorComponentTemp<IDS_COMPONENT_YY, 1>(*this, unittype);
    new CVectorComponentTemp<IDS_COMPONENT_ZZ, 2>(*this, unittype);
  }
};

class TDisplacementVector : public CValueVectorTemp<IDT_VALUETYPE_DISPLACEMENT, IDS_ET_DISPLACEMENT,
                                                    IDI_VALUETYPE_DISPLACEMENT, IDS_VALUENAME_DISP_VECTOR> {
public:
  TDisplacementVector(IPointSet &point_set, const QString &strName)
      : CValueVectorTemp<IDT_VALUETYPE_DISPLACEMENT, IDS_ET_DISPLACEMENT, IDI_VALUETYPE_DISPLACEMENT,
                         IDS_VALUENAME_DISP_VECTOR>(point_set, strName, Units::LengthUnit) {}

  ACCEPT_GEOMECMODELVISITORS(VisitTDisplacementVector);
};

#endif // !defined(AFX_VALUEVECTOR_H__9FE10D95_95EA_422F_8E7F_3E667253916F__INCLUDED_)
