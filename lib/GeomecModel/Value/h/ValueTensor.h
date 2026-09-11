// ValueTensor.h: interface for the CValueTensor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUETENSOR_H__A6505724_3ACD_42D5_AB87_8FAFB2F2B2D6__INCLUDED_)
#define AFX_VALUETENSOR_H__A6505724_3ACD_42D5_AB87_8FAFB2F2B2D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueComponent.h"
#include "ValueType.h"
#include "ValueTypes.h"
#include "mfTensor.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

// Define componentmode
template <unsigned int uComponentName,  // Resource string ID of the component name
          unsigned int uComponentIndex> // Index of the component in the value set
class CTensorComponentTemp : public CComponentTemp<uComponentName, uComponentIndex> {
public:
  CTensorComponentTemp(CValueType &value_type, IValueSet &value_set, const Units::CUnitType &unittype)
      : CComponentTemp<uComponentName, uComponentIndex>(value_type, value_set, unittype, Ranges::NoRange) {}

  CTensorComponentTemp(CValueType &value_type, const Units::CUnitType &unittype)
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
    case 3:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_XY);
      break;
    case 4:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_YZ);
      break;
    case 5:
      sTensor = getStringTableEntry(IDS_ET_TENSOR_XZ);
      break;
    default:
      // FIXME: MCR
      // assert(FALSE);
      break;
    }

    QString sPrefix = CComponentTemp<uComponentName, uComponentIndex>::ExportLabel();

    return sPrefix + sTensor;
  }
};

template <unsigned int uTypeId, unsigned int uExport, unsigned int uIconId, unsigned int uValueNameId>
class CValueTensorTemp : public CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport> {

public:
  CValueTensorTemp(IPointSet &point_set, const QString &strName, const Units::CUnitType &unittype)
      : CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExport>(point_set, strName) {
    new CTensorComponentTemp<IDS_COMPONENT_XX, 0>(*this, unittype);
    new CTensorComponentTemp<IDS_COMPONENT_YY, 1>(*this, unittype);
    new CTensorComponentTemp<IDS_COMPONENT_ZZ, 2>(*this, unittype);
    new CTensorComponentTemp<IDS_COMPONENT_XY, 3>(*this, unittype);
    new CTensorComponentTemp<IDS_COMPONENT_YZ, 4>(*this, unittype);
    new CTensorComponentTemp<IDS_COMPONENT_XZ, 5>(*this, unittype);
  }
};

class TStressTensor : public CValueTensorTemp<IDT_VALUETYPE_STRESS_TENSOR, IDS_ET_STRESS, IDI_VALUETYPE_STRESS_TENSOR,
                                              IDS_VALUENAME_STRESS_TENSOR> {
public:
  TStressTensor(IPointSet &point_set, const QString &strName)
      : CValueTensorTemp<IDT_VALUETYPE_STRESS_TENSOR, IDS_ET_STRESS, IDI_VALUETYPE_STRESS_TENSOR,
                         IDS_VALUENAME_STRESS_TENSOR>(point_set, strName, Units::StressUnit) {}

  ACCEPT_GEOMECMODELVISITORS(VisitTStressTensor);
};

class TStrainTensor : public CValueTensorTemp<IDT_VALUETYPE_STRAIN_TENSOR, IDS_ET_STRAIN, IDI_VALUETYPE_STRAIN_TENSOR,
                                              IDS_VALUENAME_STRAIN_TENSOR> {
public:
  TStrainTensor(IPointSet &point_set, const QString &strName)
      : CValueTensorTemp<IDT_VALUETYPE_STRAIN_TENSOR, IDS_ET_STRAIN, IDI_VALUETYPE_STRAIN_TENSOR,
                         IDS_VALUENAME_STRAIN_TENSOR>(point_set, strName, Units::DimensionLessUnit) {}

  ACCEPT_GEOMECMODELVISITORS(VisitTStrainTensor);
};
/*
class TChangeEffectiveStressTensor : public CValueTensorTemp<IDT_VALUETYPE_CHANGE_EFF_STRESS_TENSOR,
IDS_ET_CHANGE_EFF_STRESS, IDI_VALUETYPE_CHANGE_EFF_STRESS, IDS_VALUENAME_CHANGE_EFF_STRESS>
{
public:
  TChangeEffectiveStressTensor(IPointSet &point_set, const CString &strName)
  : CValueTensorTemp<IDT_VALUETYPE_CHANGE_EFF_STRESS_TENSOR, IDS_ET_CHANGE_EFF_STRESS, IDI_VALUETYPE_CHANGE_EFF_STRESS,
IDS_VALUENAME_CHANGE_EFF_STRESS>(point_set, strName, Units::StressUnit)
  {
  }
};*/

#endif // !defined(AFX_VALUETENSOR_H__A6505724_3ACD_42D5_AB87_8FAFB2F2B2D6__INCLUDED_)
