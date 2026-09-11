#include "FractTensor.h"
#include "ValueTypes.h"
#include "resourceIDS.h"
#include "resourceIDI.h"

CFractTensor::CFractTensor(IPointSet &point_set, const QString &strName)
   : CMatParamsTensor(IDS_ET_FRACT_TENSOR, IDT_VALUETYPE_FRACT_TENSOR, IDI_VALUETYPE_FRACT_TENSOR, point_set, strName)
{
  AddComponents();
}

void CFractTensor::AddComponents()
{
  AddComponent(IDS_VALUENAME_HIGH_FRACT_DENS, IDS_ET_HIGH_FRACT_DENS, IDT_VALUETYPE_HIGH_FRACT_DENS,
  Units::PerLengthUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_HIGH_FRACT_DENS_INCL, IDS_ET_HIGH_FRACT_DENS_INCL, IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,
  Units::AngleUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_HIGH_FRACT_DENS_AZI, IDS_ET_HIGH_FRACT_DENS_AZI, IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,
  Units::AngleUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_LOW_FRACT_DENS, IDS_ET_LOW_FRACT_DENS, IDT_VALUETYPE_LOW_FRACT_DENS,
  Units::PerLengthUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_LOW_FRACT_DENS_INCL, IDS_ET_LOW_FRACT_DENS_INCL, IDT_VALUETYPE_LOW_FRACT_DENS_INCL,
  Units::AngleUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_LOW_FRACT_DENS_AZI, IDS_ET_LOW_FRACT_DENS_AZI, IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
  Units::AngleUnit, Ranges::NoRange);
  AddComponent(IDS_VALUENAME_INTER_FRACT_DENS, IDS_ET_INTER_FRACT_DENS, IDT_VALUETYPE_INTER_FRACT_DENS,
  Units::PerLengthUnit, Ranges::NoRange);
}