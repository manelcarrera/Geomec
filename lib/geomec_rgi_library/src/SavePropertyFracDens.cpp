
#include "SavePropertyFracDens.h"
#include "FormationBase.h"
#include "MaterialFractureApertureBase.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "RGInterface.h"
#include "RGUtils.h"
#include "RockMechProcessor.h"

namespace GeomecRGI {

CSavePropertyFracDens::CSavePropertyFracDens(const RGProperty &rgProperty, TDensityDirection densityDirection,
                                             TVectorDirection vectorDirection)
    : CSavePropertyBase(rgProperty), m_DensityDirection(densityDirection), m_VectorDirection(vectorDirection) {}

CSavePropertyFracDens::~CSavePropertyFracDens() {}

bool CSavePropertyFracDens::saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp) {
  std::vector<double> values(modelBase.Mesh().Mesh().ElementSize(), RGUtils::nullReal());
  CDepletionStage &depletionStage =
      modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());
  bool valuesSeen = false;

  for (int i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &element = modelBase.Mesh().Mesh().Element(i);
    const CFormationBase *formationBase = modelBase.Mesh().Formation(element);

    if (formationBase) {
      try {
        const CFFMaterial &cffMaterial = formationBase->Material(depletionStage).Material(element);
        const CMaterialFractureApertureBase &materialFractureApertureBase =
            dynamic_cast<const CMaterialFractureApertureBase &>(cffMaterial.Material());
        geo::CVector vector = getVector(cffMaterial, materialFractureApertureBase);
        double length = getLength(cffMaterial);

        values[i] = getVectorComponent(vector, length);
        valuesSeen = true;
      }

      catch (const std::bad_cast &) {
      }
    }
  }

  rgi.saveProperty(m_RGProperty, values);

  if (!valuesSeen) {
    rmp.AddLogLine("No fracture aperture material has been found.");
  }

  return true;
}

// private

double CSavePropertyFracDens::getLength(const CFFMaterial &cffMaterial) const {
  switch (m_DensityDirection) {
  case HIGH:
    return cffMaterial.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS);
  case INTERMEDIATE:
    return cffMaterial.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS);
  case LOW:
    return cffMaterial.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS);
  default:
    assert(false);
  }

  return 0;
}

geo::CVector CSavePropertyFracDens::getVector(const CFFMaterial &cffMaterial,
                                              const CMaterialFractureApertureBase &materialFractureApertureBase) const {
  switch (m_DensityDirection) {
  case HIGH:
    return materialFractureApertureBase.HighDensityDirection(cffMaterial);
  case INTERMEDIATE:
    return materialFractureApertureBase.IntermediateDensityDirection(cffMaterial);
  case LOW:
    return materialFractureApertureBase.LowDensityDirection(cffMaterial);
  default:
    assert(false);
  }

  return geo::CVector();
}

double CSavePropertyFracDens::getVectorComponent(const geo::CVector &vector, double length) const {
  switch (m_VectorDirection) {
  case X:
    return vector.X() * length;
  case Y:
    return vector.Y() * length;
  case Z:
    return vector.Z() * length;
  default:
    assert(false);
  }

  return 0;
}

} // namespace GeomecRGI
