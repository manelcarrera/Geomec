
#include "LoadPropertyBridge.h"
#include "RGProperty.h"
#include "LoadPropertyFluidPressure.h"
#include "LoadPropertyDeltaFluidPressure.h"
#include "LoadPropertyFluidPressureFrac.h"
#include "LoadPropertyDeltaFluidPressureFrac.h"
#include "LoadPropertyTemperature.h"
#include "LoadPropertyDeltaTemperature.h"
#include "LoadPropertyFormation.h"
#include "LoadPropertyYoung.h"
#include "LoadPropertyPoisson.h"
#include "LoadPropertyBodyIndex.h"
#include "LoadPropertyShearModulus.h"
#include "LoadPropertyTemplate.h"
#include "LoadPropertyTemplateRadials2Degrees.h"
#include "LoadPropertyMaterialType.h"
#include "LoadPropertyTemplateWithFactor.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

#include "LoadPropertyResidualStrainLoading.h"

#ifdef _WIN32
#pragma warning(pop)
#endif


namespace
{

typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_VOLUMETRICSTRAIN>
  TLoadPropertyDeltaDesorptionStrain;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_POROSITY>
  TLoadPropertyPorosity;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_RHOB>
  TLoadPropertySaturatedRockDensity;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_VOLUMETRICSTRAIN>
  TLoadPropertyVolumetricStrain;
typedef GeomecRGI::CLoadPropertyTemplateWithFactor
  <IDT_VALUETYPE_YOUNGMODULUS_NORM>
    TLoadPropertyYoungNormal;
typedef GeomecRGI::CLoadPropertyTemplateWithFactor
  <IDT_VALUETYPE_YOUNGMODULUS_TRANS>
    TLoadPropertyYoungPlane;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_POISSONRATIO_NORM>
  TLoadPropertyPoissonNormal;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_POISSONRATIO_TRANS>
  TLoadPropertyPoissonPlane;

typedef GeomecRGI::CLoadPropertyTemplateRadials2Degrees
  <IDT_VALUETYPE_LAYER_NORMAL_INCL>
    TLoadPropertyOrientation_Dip;
typedef GeomecRGI::CLoadPropertyTemplateRadials2Degrees
  <IDT_VALUETYPE_LAYER_NORMAL_AZI>
    TLoadPropertyOrientation_Azimuth;

typedef GeomecRGI::CLoadPropertyTemplateWithFactor
  <IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS>
    TLoadPropertyShearNormal;

typedef GeomecRGI::CLoadPropertyTemplateWithFactor
  <IDT_VALUETYPE_GRAINSTIFFNESS>
    TLoadPropertyGrainBulkModulus;

typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_NRMSTRAIN>
  TLoadPropertyStrainLoadingNormal;
typedef GeomecRGI::CLoadPropertyTemplate <IDT_VALUETYPE_LATSTRAIN>
  TLoadPropertyStrainLoadingPlane;

typedef GeomecRGI::CLoadPropertyTemplateWithFactor<IDT_VALUETYPE_UNDRAINEDYOUNG>
  TLoadPropertyYoungUndrained;
typedef GeomecRGI::CLoadPropertyTemplate<IDT_VALUETYPE_UNDRAINEDPOISSON>
  TLoadPropertyPoissonUndrained;

QSharedPointer <GeomecRGI::CLoadPropertyBase> selectLoadProperty(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  GeomecRGI::CRockMechProcessor& rmp)
{
  QSharedPointer <GeomecRGI::CLoadPropertyBase> loadPropertyBase;

  switch (rgProperty.getType())
  {
    case RGPropertyType::propElementFluidPressure:
      return QSharedPointer <GeomecRGI::CLoadPropertyFluidPressure> (
        new GeomecRGI::CLoadPropertyFluidPressure(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propElementDeltaFluidPressure:
      return QSharedPointer <GeomecRGI::CLoadPropertyDeltaFluidPressure> (
        new GeomecRGI::CLoadPropertyDeltaFluidPressure(rgProperty, rgi,
          modelBase, rmp));
      break;
    case RGPropertyType::propElementFluidPressureFrac:
      return QSharedPointer <GeomecRGI::CLoadPropertyFluidPressureFrac> (
        new GeomecRGI::CLoadPropertyFluidPressureFrac(rgProperty, rgi,
          modelBase, rmp));
      break;
    case RGPropertyType::propElementDeltaFluidPressureFrac:
      return QSharedPointer <GeomecRGI::CLoadPropertyDeltaFluidPressureFrac> (
        new GeomecRGI::CLoadPropertyDeltaFluidPressureFrac(rgProperty, rgi,
          modelBase, rmp));
      break;
    case RGPropertyType::propElementTemperature:
      return QSharedPointer <GeomecRGI::CLoadPropertyTemperature> (
        new GeomecRGI::CLoadPropertyTemperature(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propElementDeltaTemperature:
      return QSharedPointer <GeomecRGI::CLoadPropertyDeltaTemperature> (
        new GeomecRGI::CLoadPropertyDeltaTemperature(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propElementFormation:
      return QSharedPointer <GeomecRGI::CLoadPropertyFormation> (
        new GeomecRGI::CLoadPropertyFormation(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementYoung:
      return QSharedPointer <GeomecRGI::CLoadPropertyYoung> (
        new GeomecRGI::CLoadPropertyYoung(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementPoisson:
      return QSharedPointer <GeomecRGI::CLoadPropertyPoisson> (
        new GeomecRGI::CLoadPropertyPoisson(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementPorosity:
      return QSharedPointer <TLoadPropertyPorosity> (
        new TLoadPropertyPorosity(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propSaturatedRockDensity:
      return QSharedPointer <TLoadPropertySaturatedRockDensity> (
        new TLoadPropertySaturatedRockDensity(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementVolumetricStrain:
      return QSharedPointer <TLoadPropertyVolumetricStrain> (
        new TLoadPropertyVolumetricStrain(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propDeltaDesorptionStrain:
      return QSharedPointer <TLoadPropertyDeltaDesorptionStrain> (
        new TLoadPropertyDeltaDesorptionStrain(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propBodyIndex:
      return QSharedPointer <GeomecRGI::CLoadPropertyBodyIndex> (
        new GeomecRGI::CLoadPropertyBodyIndex(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementShearModulus:
      return QSharedPointer <GeomecRGI::CLoadPropertyShearModulus> (
        new GeomecRGI::CLoadPropertyShearModulus(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propElementOrientation_Dip:
      return QSharedPointer <TLoadPropertyOrientation_Dip> (
        new TLoadPropertyOrientation_Dip(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementOrientation_Azimuth:
      return QSharedPointer <TLoadPropertyOrientation_Azimuth> (
        new TLoadPropertyOrientation_Azimuth(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementYoungNormal:
      return QSharedPointer <TLoadPropertyYoungNormal> (
        new TLoadPropertyYoungNormal(rgProperty, rgi, modelBase, rmp, 1e-6));
      break;
    case RGPropertyType::propElementYoungPlane:
      return QSharedPointer <TLoadPropertyYoungPlane> (
        new TLoadPropertyYoungPlane(rgProperty, rgi, modelBase, rmp, 1e-6));
      break;
    case RGPropertyType::propElementPoissonNormal:
      return QSharedPointer <TLoadPropertyPoissonNormal> (
        new TLoadPropertyPoissonNormal(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementPoissonPlane:
      return QSharedPointer <TLoadPropertyPoissonPlane> (
        new TLoadPropertyPoissonPlane(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementShearNormal:
      return QSharedPointer <TLoadPropertyShearNormal> (
        new TLoadPropertyShearNormal(rgProperty, rgi, modelBase, rmp, 1e-6));
      break;
    case RGPropertyType::propElementMaterialType:
      return QSharedPointer <GeomecRGI::CLoadPropertyMaterialType> (
        new GeomecRGI::CLoadPropertyMaterialType(rgProperty, rgi, modelBase,
          rmp));
      break;
    case RGPropertyType::propElementGrainBulkModulus:
      return QSharedPointer <TLoadPropertyGrainBulkModulus> (
        new TLoadPropertyGrainBulkModulus(rgProperty, rgi, modelBase, rmp,
          1e-6));
      break;
    case RGPropertyType::propElementStrainLoadingNormal:
      return QSharedPointer <TLoadPropertyStrainLoadingNormal> (
        new TLoadPropertyStrainLoadingNormal(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementStrainLoadingPlane:
      return QSharedPointer <TLoadPropertyStrainLoadingPlane> (
        new TLoadPropertyStrainLoadingPlane(rgProperty, rgi, modelBase, rmp));
      break;
    case RGPropertyType::propElementResidualStrainLoading_NN:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_XX));
      break;
    case RGPropertyType::propElementResidualStrainLoading_EE:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_YY));
      break;
    case RGPropertyType::propElementResidualStrainLoading_DD:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_ZZ));
      break;
    case RGPropertyType::propElementResidualStrainLoading_NE:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_XY));
      break;
    case RGPropertyType::propElementResidualStrainLoading_ND:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_ZX));
      break;
    case RGPropertyType::propElementResidualStrainLoading_ED:
      return QSharedPointer <GeomecRGI::CLoadPropertyBase> (
        new GeomecRGI::CLoadPropertyResidualStrainLoading(rgProperty, rgi,
          modelBase, rmp, ITensorGroup::CComponentComposite::TC_YZ));
      break;
	case RGPropertyType::propElementYoungUndrained:
		return QSharedPointer <TLoadPropertyYoungUndrained>(
			new TLoadPropertyYoungUndrained(rgProperty, rgi, modelBase, rmp, 1e-6));
		break;
	case RGPropertyType::propElementPoissonUndrained:
		return QSharedPointer <TLoadPropertyPoissonUndrained>(
			new TLoadPropertyPoissonUndrained(rgProperty, rgi, modelBase, rmp));
		break;
	case RGPropertyType::propLast:
      throw QString("The loading of property propLast is currently "
        "not implemented");
      break;
    default:
      QString errorMessage = QString("The loading of property %1 is currently "
        "not implemented").arg(rgProperty.toString().c_str());
      throw errorMessage;
      break;
  }
}

} // anonymous namespace

namespace GeomecRGI
{

CLoadPropertyBridge::CLoadPropertyBridge(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
: m_LoadPropertyBase(selectLoadProperty(rgProperty, rgi, modelBase, rmp))
{
}

CLoadPropertyBridge::~CLoadPropertyBridge()
{
}

bool CLoadPropertyBridge::loadProperty()
{
  return m_LoadPropertyBase->loadProperty();
}

} // namespace GeomecRGI
