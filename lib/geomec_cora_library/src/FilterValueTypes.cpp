#include <cassert>

#include "FilterValueTypes.h"
#include "ValueTypes.h"
#include "Materials.h"
#include "mlMaterial.h"
#include "MaterialHelperFactory.h"
#include "LibraryMaterial.h"

namespace cora
{

CFilterValueTypes::CFilterValueTypes(int materialModel)
: m_excludedValueTypes(fillExcludedValueTypes(materialModel))
, m_allLinearValueTypes(fillAllLinearValueTypes())
, m_materialModel(materialModel)
{
}

bool CFilterValueTypes::isValueTypeAllowed(size_t valueType) const
{
  if (isValueTypeLinear(valueType))
  {
  return isValueTypeExcluded(valueType) ? false : true;
  }
  else
  {
  if (m_materialModel == MM_CAMCLAY)
  {
      return isValueTypeExcluded(valueType) ? false : true;
  }
  else if (m_materialModel == MM_MOHRCOULOMB)
  {
      return true;
  }
  else
  {
      return false;
  }
  }
}

bool CFilterValueTypes::isValueTypeExcluded(size_t valueType) const
{
  return (m_excludedValueTypes.find(valueType) != m_excludedValueTypes.end());
}

bool CFilterValueTypes::isValueTypeLinear(size_t valueType) const
{
  return (m_allLinearValueTypes.find(valueType) != m_allLinearValueTypes.end());
}

// private

// static

CFilterValueTypes::TFilterSet CFilterValueTypes::fillExcludedValueTypes(
  int materialModel)
{
  TFilterSet excludedValueTypes;
  std::pair <TFilterSet::iterator, bool> insertedExcludedValueType;

  // linear

  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_GRAINSTIFFNESS);  // "Grain Bulk Modulus"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_DYNUNISTIFFNESS); // "Dynamic Uniaxial Stiffness"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_DYNSHEARSTIFFNESS); // "Dynamic Shear Stiffness"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_VELOCITYS); // "Vs"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_POROSITY);  // "Porosity"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_PERMEA);  // "Permeability"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_VISCOSITY); // "Viscosity"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_FLUID_BULK_MOD);  // "Fluid bulk modulus"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_FLUID_DENSITY); // "Fluid Density"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_VELOCITYP); // "Vp"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_THERMAL_EXPANSION); // "Volumetric Thermal Expansion"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_THERM_CONDUCT); // "Thermal Conductivity"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_THERM_CAPACI);  // "Thermal Capacity"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_FLUIDX);  // "Fluid Thermal Expansion Coefficient"
  assert(insertedExcludedValueType.second);

  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_DUMMY);  // "Dummy"
  assert(insertedExcludedValueType.second);

  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_KRAD);  // "Krad"
  assert(insertedExcludedValueType.second);

  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_KTAN);  // "Ktan"
  assert(insertedExcludedValueType.second);

  // non-linear

  if (materialModel == MM_CAMCLAY)
  {
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_CAPSHAPE);  // "Cap Shape Factor"
  assert(insertedExcludedValueType.second);
  insertedExcludedValueType = excludedValueTypes.insert(IDT_VALUETYPE_TENSILE_STRETCH); // "Tensile stretch"
  assert(insertedExcludedValueType.second);
  }
  else if (materialModel == MM_MOHRCOULOMB)
  {
  }
  else
  {
  }

  return excludedValueTypes;
}

CFilterValueTypes::TFilterSet CFilterValueTypes::fillAllLinearValueTypes()
{
  TFilterSet allLinearValueTypes;
  std::pair <TFilterSet::iterator, bool> insertedLinearValueType;

  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNGMODULUS_NORM);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNGMODULUS_TRANS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSONRATIO_NORM);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSONRATIO_TRANS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNGS_MODULUS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_YOUNG_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSONS_RATIO);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_POISSON_DECOMP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_BULKSTIFFNESS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_SHEARMODULUS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_DYNUNISTIFFNESS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_DYNSHEARSTIFFNESS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_VELOCITYP);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_VELOCITYS);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_ELASCOMPRES);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_THOMSEN_EPSILON);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_THOMSEN_GAMMA);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_THOMSEN_DELTA);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_LAYER_NORMAL_INCL);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_LAYER_NORMAL_AZI);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_LATRATIO_MIN);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_LATRATIO_MAX);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_AZIMUTH);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM1);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM2);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM3);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM4);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM5);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM6);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM7);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM8);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM9);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM10);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM11);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM12);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM13);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM14);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM15);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM16);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM17);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM18);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM19);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM20);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_RIGI_PARAM21);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_UNDRAINEDYOUNG);
  assert(insertedLinearValueType.second);
  insertedLinearValueType = allLinearValueTypes.insert(IDT_VALUETYPE_UNDRAINEDPOISSON);
  assert(insertedLinearValueType.second);

  addLinearModelValueTypes(allLinearValueTypes);

  return allLinearValueTypes;
}

namespace
{

const QString MATERIAL = "linear material";

} // anonymous namespace

void CFilterValueTypes::addLinearModelValueTypes(
  TFilterSet& allLinearValueTypes)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  ml::TMaterialCreatorRefPtr materialCreator = f->getMatCreator(MM_LINEAR);
  assert(materialCreator);

  ml::TMaterialPtr material = materialCreator->OnCreateMaterial();

  materialCreator->Create(MATERIAL, *material);

  CLibraryMaterial* libraryMaterial =
      dynamic_cast <CLibraryMaterial*> (material);

  for (size_t p = 0; (libraryMaterial != 0) &&
      (p < libraryMaterial->ParameterSize()); ++p)
  {
      allLinearValueTypes.insert(libraryMaterial->Parameter(p).ValueTypeID());
  }

  materialCreator->Destroy(material);
}

} // namespace cora
