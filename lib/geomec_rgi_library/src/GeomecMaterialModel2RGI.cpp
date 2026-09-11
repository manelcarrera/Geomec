
#include "GeomecMaterialModel2RGI.h"
#include "BaseEntryTypes.h"
#include "FormationBase.h"
#include "Materials.h"
#include "ModelBase.h"
#include "RGMaterialType.h"

namespace {

std::vector<CGraphNode *> getFormations(const CModelBase &modelBase) {
  const CGraphEntry *graphEntry = modelBase.GraphEntry(MD_BASE_FORMATION);
  CGraphEntry::TEntryNodeSet entryNodeSet = graphEntry->GraphEntryNodes();
  std::vector<CGraphNode *> formations;

  for (CGraphEntry::TEntryNodeSet::const_iterator entryNode = entryNodeSet.begin(); entryNode != entryNodeSet.end();
       ++entryNode) {
    formations.push_back(*entryNode);
  }

  return formations;
}

const int UNKNOWN = -1;

std::vector<int> getMaterialModels(const std::vector<CGraphNode *> &formations) {
  // The typedef 'enum _MaterialModel' does not contain an 'unkown'/'undefined'
  // value, hence the initialization with the local magic constant 'UNKNOWN'.

  std::vector<int> materialModels(formations.size(), UNKNOWN);

  try {
    for (size_t f = 0; f < formations.size(); ++f) {
      materialModels[f] = dynamic_cast<CFormationBase *>(formations[f])->ActiveMaterial().MaterialModel();
    }
  }

  catch (...) {
    assert(false);
  }

  return materialModels;
}

std::vector<int> mapMaterialModel2MaterialType(const std::vector<int> &materialModels) {
  std::vector<int> materialTypes(materialModels.size(), RGMaterialType::UNKNOWN);

  for (size_t m = 0; m < materialModels.size(); ++m) {
    switch (materialModels[m]) {
    case MM_LINEAR:
      materialTypes[m] = RGMaterialType::LINEAR;
      break;
    case MM_CAMCLAY:
      materialTypes[m] = RGMaterialType::CAMCLAY;
      break;
    case MM_MOHRCOULOMB:
      materialTypes[m] = RGMaterialType::MOHRCOULOMB;
      break;
    case MM_MODIFIEDMOHRCOULOMB:
      materialTypes[m] = RGMaterialType::MODIFIEDMOHRCOULOMB;
      break;
    case MM_CREEP:
      materialTypes[m] = RGMaterialType::CREEP;
      break;
    case MM_UNDRAINED:
      materialTypes[m] = RGMaterialType::UNDRAINED;
      break;
    case MM_MC_COHESION_HARD1:
      materialTypes[m] = RGMaterialType::MC_COHESION_HARD1;
      break;
    case MM_MC_COHESION_HARD2:
      materialTypes[m] = RGMaterialType::MC_COHESION_HARD2;
      break;
    case MM_MC_COHESION_HARD3:
      materialTypes[m] = RGMaterialType::MC_COHESION_HARD3;
      break;
    case MM_MC_FRICTION_HARD1:
      materialTypes[m] = RGMaterialType::MC_FRICTION_HARD1;
      break;
    case MM_MC_FRICTION_HARD2:
      materialTypes[m] = RGMaterialType::MC_FRICTION_HARD2;
      break;
    case MM_MC_FRICTION_HARD3:
      materialTypes[m] = RGMaterialType::MC_FRICTION_HARD3;
      break;
    case MM_RIGIDITY:
      materialTypes[m] = RGMaterialType::RIGIDITY;
      break;
    case MM_DUALCAP:
      materialTypes[m] = RGMaterialType::UNKNOWN;
      break;
    case MM_DUALCAP_LINELA:
      materialTypes[m] = RGMaterialType::DUALCAP_LINELA;
      break;
    case MM_FRACTURE_ANISOTROPY:
      materialTypes[m] = RGMaterialType::FRACTURE_ANISOTROPY;
      break;
    case MM_UPSCALED_ANISOTROPY:
      materialTypes[m] = RGMaterialType::UPSCALED_ANISOTROPY;
      break;
    case MM_FRACTURE_APERTURE:
      materialTypes[m] = RGMaterialType::FRACTURE_APERTURE;
      break;
    case MM_ANISOTROPIC_CAMCLAY:
      materialTypes[m] = RGMaterialType::ANISOTROPIC_CAMCLAY;
      break;
    case MM_FRACTURE_APERTURE2:
      materialTypes[m] = RGMaterialType::FRACTURE_APERTURE2;
      break;
    case MM_CAMCLAY_CREEP:
      materialTypes[m] = RGMaterialType::UNKNOWN;
      break;
    default:
      assert(false);
      materialTypes[m] = RGMaterialType::UNKNOWN;
      break;
    }
  }

  return materialTypes;
}

std::vector<int> getMaterialTypes(const std::vector<CGraphNode *> &formations, const std::vector<int> &materialModels,
                                  const CModelBase &modelBase) {
  std::vector<int> mappedMaterialTypes = mapMaterialModel2MaterialType(materialModels);
  std::vector<int> materialTypes(modelBase.Mesh().Mesh().ElementSize(), RGMaterialType::UNKNOWN);

  for (int e = 0; e < modelBase.Mesh().Mesh().ElementSize(); ++e) {
    const geo::IElement &element = modelBase.Mesh().Mesh().Element(e);
    const CFormationBase *formationBase = modelBase.Mesh().Formation(element);
    std::vector<CGraphNode *>::const_iterator formation =
        std::find(formations.begin(), formations.end(), formationBase);

    assert(formation != formations.end());

    materialTypes[e] = mappedMaterialTypes[std::distance(formations.begin(), formation)];
  }

  return materialTypes;
}

} // anonymous namespace

namespace GeomecRGI {

CGeomecMaterialModel2RGI::CGeomecMaterialModel2RGI(RGInterface & /*rgi*/, const CModelBase &modelBase)
    : m_modelBase(modelBase), m_formations(getFormations(m_modelBase)),
      m_materialModels(getMaterialModels(m_formations)),
      m_materialTypes(::getMaterialTypes(m_formations, m_materialModels, m_modelBase)) {}

CGeomecMaterialModel2RGI::~CGeomecMaterialModel2RGI() {}

std::vector<int> CGeomecMaterialModel2RGI::getMaterialTypes() const { return m_materialTypes; }

} // namespace GeomecRGI
