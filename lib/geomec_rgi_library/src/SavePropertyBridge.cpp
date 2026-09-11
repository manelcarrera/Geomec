
#include "SavePropertyBridge.h"
#include "RGProperty.h"
#include "SavePropertyVolumetricStrain.h"
#include "SavePropertyFluidPressure.h"
#include "SavePropertyTemperature.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

#include "SavePropertyEffStress.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

#include "SavePropertyFormation.h"
#include "ITensorGroup.h"
#include "SavePropertyFracDens.h"
#include "SavePropertyPermMultFrac.h"
#include "SavePropertyPorosity.h"
#include "SavePropertyBodyIndex.h"
#include "SavePropertyTemplate.h"
#include "ValueTypes.h"
#include "SavePropertyTemplateWithFactor.h"
#include "SavePropertyTemplateDegrees2Radials.h"
#include "SavePropertyMaterialType.h"
#include "SavePropertyTotalStress.h"
#include "SavePropertyTotalStrain.h"
#include "SavePropertyIsReservoir.h"


namespace
{

typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_RHOB>
  TSavePropertyDensity;
typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_POISSONS_RATIO>
  TSavePropertyPoisson;
typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_UNDRAINEDPOISSON>
  TSavePropertyPoissonUndrained;
typedef GeomecRGI::CSavePropertyTemplateWithFactor <IDT_VALUETYPE_SHEARMODULUS>
  TSavePropertyShearModulus;

typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_YOUNGS_MODULUS>
  TSavePropertyYoung;
typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_YOUNGMODULUS_NORM>
  TSavePropertyYoungNormal;
typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_YOUNGMODULUS_TRANS>
  TSavePropertyYoungPlane;
typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_UNDRAINEDYOUNG>
  TSavePropertyYoungUndrained;

typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_POISSONRATIO_TRANS>
  TSavePropertyPoissonPlane;
typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_POISSONRATIO_NORM>
  TSavePropertyPoissonNormal;

typedef GeomecRGI::CSavePropertyTemplateDegrees2Radials
  <IDT_VALUETYPE_LAYER_NORMAL_INCL>
  TSavePropertyOrientation_Dip;
typedef GeomecRGI::CSavePropertyTemplateDegrees2Radials
  <IDT_VALUETYPE_LAYER_NORMAL_AZI>
  TSavePropertyOrientation_Azimuth;

typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS>
  TSavePropertyShearNormal;

typedef GeomecRGI::CSavePropertyTemplateWithFactor
  <IDT_VALUETYPE_GRAINSTIFFNESS>
  TLoadPropertyGrainBulkModulus;

typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_NRMSTRAIN>
  TSavePropertyStrainLoadingNormal;
typedef GeomecRGI::CSavePropertyTemplate <IDT_VALUETYPE_LATSTRAIN>
  TSavePropertyStrainLoadingPlane;

QSharedPointer <GeomecRGI::CSavePropertyBase> selectSaveProperty(
  const RGProperty& rgProperty)
{
  QSharedPointer <GeomecRGI::CSavePropertyBase> savePropertyBase;

  switch (rgProperty.getType())
  {
  case RGPropertyType::propElementVolumetricStrain:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyVolumetricStrain(rgProperty));
      break;
  case RGPropertyType::propElementFluidPressure:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFluidPressure(rgProperty));
      break;
  case RGPropertyType::propElementTemperature:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTemperature(rgProperty));
      break;
  case RGPropertyType::propElementEffStress_NN:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_XX));
      break;
  case RGPropertyType::propElementEffStress_EE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_YY));
      break;
  case RGPropertyType::propElementEffStress_DD:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZZ));
      break;
  case RGPropertyType::propElementEffStress_NE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_XY));
      break;
  case RGPropertyType::propElementEffStress_ND:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZX));
      break;
  case RGPropertyType::propElementEffStress_ED:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyEffStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_YZ));
      break;
  case RGPropertyType::propElementFormation:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFormation(rgProperty));
      break;
  case RGPropertyType::propFracDens_1_N:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::HIGH,
          GeomecRGI::CSavePropertyFracDens::X));
      break;
  case RGPropertyType::propFracDens_1_E:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::HIGH,
          GeomecRGI::CSavePropertyFracDens::Y));
      break;
  case RGPropertyType::propFracDens_1_D:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::HIGH,
          GeomecRGI::CSavePropertyFracDens::Z));
      break;
  case RGPropertyType::propFracDens_2_N:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::INTERMEDIATE,
          GeomecRGI::CSavePropertyFracDens::X));
      break;
  case RGPropertyType::propFracDens_2_E:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::INTERMEDIATE,
          GeomecRGI::CSavePropertyFracDens::Y));
      break;
  case RGPropertyType::propFracDens_2_D:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::INTERMEDIATE,
          GeomecRGI::CSavePropertyFracDens::Z));
      break;
  case RGPropertyType::propFracDens_3_N:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::LOW,
          GeomecRGI::CSavePropertyFracDens::X));
      break;
  case RGPropertyType::propFracDens_3_E:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::LOW,
          GeomecRGI::CSavePropertyFracDens::Y));
      break;
  case RGPropertyType::propFracDens_3_D:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyFracDens(rgProperty,
          GeomecRGI::CSavePropertyFracDens::LOW,
          GeomecRGI::CSavePropertyFracDens::Z));
      break;
  case RGPropertyType::propPermMultFrac_1:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyPermMultFrac(rgProperty,
          GeomecRGI::CSavePropertyPermMultFrac::HIGH));
      break;
  case RGPropertyType::propPermMultFrac_2:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyPermMultFrac(rgProperty,
          GeomecRGI::CSavePropertyPermMultFrac::INTERMEDIATE));
      break;
  case RGPropertyType::propPermMultFrac_3:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyPermMultFrac(rgProperty,
          GeomecRGI::CSavePropertyPermMultFrac::LOW));
      break;
  case RGPropertyType::propElementYoung:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyYoung(rgProperty, 1e6));
      break;
  case RGPropertyType::propElementPoisson:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyPoisson(rgProperty));
      break;
  case RGPropertyType::propElementPorosity:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyPorosity(rgProperty));
      break;
  case RGPropertyType::propSaturatedRockDensity:
      return QSharedPointer<GeomecRGI::CSavePropertyBase> (
    new TSavePropertyDensity(rgProperty));
      break;
  case RGPropertyType::propElementShearModulus:
      return QSharedPointer<GeomecRGI::CSavePropertyBase> (
    new TSavePropertyShearModulus(rgProperty, 1e6));
      break;
  case RGPropertyType::propBodyIndex:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyBodyIndex(rgProperty));
      break;
  case RGPropertyType::propElementOrientation_Dip:
      return QSharedPointer<GeomecRGI::CSavePropertyBase> (
    new TSavePropertyOrientation_Dip(rgProperty));
      break;
  case RGPropertyType::propElementOrientation_Azimuth:
      return QSharedPointer<GeomecRGI::CSavePropertyBase> (
    new TSavePropertyOrientation_Azimuth(rgProperty));
      break;
  case RGPropertyType::propElementYoungNormal:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyYoungNormal(rgProperty, 1e6));
      break;
  case RGPropertyType::propElementYoungPlane:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyYoungPlane(rgProperty, 1e6));
      break;
  case RGPropertyType::propElementPoissonPlane:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyPoissonPlane(rgProperty));
      break;
  case RGPropertyType::propElementPoissonNormal:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyPoissonNormal(rgProperty));
      break;
  case RGPropertyType::propElementShearNormal:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyShearNormal(rgProperty, 1e6));
      break;
  case RGPropertyType::propElementMaterialType:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyMaterialType(rgProperty));
      break;
  case RGPropertyType::propElementGrainBulkModulus:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TLoadPropertyGrainBulkModulus(rgProperty, 1e6));
      break;
  case RGPropertyType::propElementStrainLoadingNormal:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyStrainLoadingNormal(rgProperty));
      break;
  case RGPropertyType::propElementStrainLoadingPlane:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new TSavePropertyStrainLoadingPlane(rgProperty));
      break;
  case RGPropertyType::propElementTotalStress_NN:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_XX));
      break;
  case RGPropertyType::propElementTotalStress_EE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_YY));
      break;
  case RGPropertyType::propElementTotalStress_DD:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZZ));
      break;
  case RGPropertyType::propElementTotalStress_NE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_XY));
      break;
  case RGPropertyType::propElementTotalStress_ND:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZX));
      break;
  case RGPropertyType::propElementTotalStress_ED:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStress(rgProperty,
          ITensorGroup::CComponentComposite::TC_YZ));
      break;
  case RGPropertyType::propElementTotalStrain_NN:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_XX));
      break;
  case RGPropertyType::propElementTotalStrain_EE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_YY));
      break;
  case RGPropertyType::propElementTotalStrain_DD:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZZ));
      break;
  case RGPropertyType::propElementTotalStrain_NE:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_XY));
      break;
  case RGPropertyType::propElementTotalStrain_ND:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_ZX));
      break;
  case RGPropertyType::propElementTotalStrain_ED:
      return QSharedPointer <GeomecRGI::CSavePropertyBase> (
    new GeomecRGI::CSavePropertyTotalStrain(rgProperty,
          ITensorGroup::CComponentComposite::TC_YZ));
      break;
  case RGPropertyType::propElementYoungUndrained:
    return QSharedPointer <GeomecRGI::CSavePropertyBase>(
      new TSavePropertyYoungUndrained(rgProperty, 1e6));
    break;
  case RGPropertyType::propElementPoissonUndrained:
    return QSharedPointer <GeomecRGI::CSavePropertyBase>(
      new TSavePropertyPoissonUndrained(rgProperty));
    break;
  case RGPropertyType::propElementIsReservoir:
    return QSharedPointer <GeomecRGI::CSavePropertyBase>(
      new GeomecRGI::CSavePropertyIsReservoir(rgProperty));
    break;
  case RGPropertyType::propLast:
      throw QString("The saving of property propLast is currently "
    "not implemented");
      break;
  default:
      QString errorMessage = QString("The saving of property %1 is currently "
    "not implemented").arg(rgProperty.toString().c_str());
      throw errorMessage;
      break;
  }
}

} // anonymous namespace

namespace GeomecRGI
{

CSavePropertyBridge::CSavePropertyBridge(const RGProperty& rgProperty)
: m_SavePropertyBase(selectSaveProperty(rgProperty))
{
}

CSavePropertyBridge::~CSavePropertyBridge()
{
}

bool CSavePropertyBridge::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  return m_SavePropertyBase->saveProperty(rgi, modelBase, rmp);
}

} // namespace GeomecRGI
