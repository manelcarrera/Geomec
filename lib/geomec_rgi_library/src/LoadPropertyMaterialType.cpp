
#include "LoadPropertyMaterialType.h"
#include "BaseEntryTypes.h"
#include "GeomecFormationIDs2RGI.h"
#include "LibraryMaterial.h"
#include "MaterialEntry.h"
#include "Materials.h"
#include "ModelBase.h"
#include "RGIFormationIDs.h"
#include "RGInterface.h"
#include "RGMaterialType.h"
#include "RGUtils.h"
#include "RockMechProcessor.h"

namespace {

std::vector<int> loadMaterialTypes(RGInterface &rgi) {
  std::vector<int> materialTypes;

  rgi.loadProperty(RGPropertyType::propElementMaterialType, materialTypes);

  return materialTypes;
}

} // anonymous namespace

namespace GeomecRGI {

CLoadPropertyMaterialType::CLoadPropertyMaterialType(const RGProperty &rgProperty, RGInterface &rgi,
                                                     CModelBase &modelBase, CRockMechProcessor &rmp)
    : CLoadPropertyBase(rgProperty, rgi, modelBase, rmp), m_materialTypes(loadMaterialTypes(rgi)) {}

CLoadPropertyMaterialType::~CLoadPropertyMaterialType() {}

namespace {

const QString MODIFYING_PROPERTY = QObject::tr("Modifying property '%1' for an existing model");
const QString UNSUPPORTED_MATERIAL_TYPE = QObject::tr("Unsupported material type '%1'");

} // anonymous namespace

bool CLoadPropertyMaterialType::loadProperty() {
  // only process the material types when there are any

  if (m_materialTypes.size() > 0) {
    bool assigned = false;

    assigned = assignMaterialModel();

    if (assigned && m_RMP.isExistingModel()) {
      QString propertyName = m_RMP.PropertyName(this->m_RGProperty);
      QString l = QString(MODIFYING_PROPERTY).arg(propertyName);

      m_RMP.AddLogLine(l, &m_RGI, false, false);
    }

    return assigned;
  }

  return true;
}

// private

bool CLoadPropertyMaterialType::mapMaterialType2MaterialModel(std::vector<int> &materialModels,
                                                              const std::vector<int> &materialTypes) const {
  for (size_t m = 0; m < materialTypes.size(); ++m) {
    if (RGUtils::isNull(materialTypes[m]) || materialTypes[m] == INT_MIN) {
      materialModels[m] = -1;
      continue;
    }

    switch (materialTypes[m]) {
    case RGMaterialType::LINEAR:
      materialModels[m] = MM_LINEAR;
      break;
    case RGMaterialType::CAMCLAY:
      materialModels[m] = MM_CAMCLAY;
      break;
    case RGMaterialType::MOHRCOULOMB:
      materialModels[m] = MM_MOHRCOULOMB;
      break;
    case RGMaterialType::MODIFIEDMOHRCOULOMB:
      materialModels[m] = MM_MODIFIEDMOHRCOULOMB;
      break;
    case RGMaterialType::CREEP:
      materialModels[m] = MM_CREEP;
      break;
    case RGMaterialType::UNDRAINED:
      materialModels[m] = MM_UNDRAINED;
      break;
    case RGMaterialType::MC_COHESION_HARD1:
      materialModels[m] = MM_MC_COHESION_HARD1;
      break;
    case RGMaterialType::MC_COHESION_HARD2:
      materialModels[m] = MM_MC_COHESION_HARD2;
      break;
    case RGMaterialType::MC_COHESION_HARD3:
      materialModels[m] = MM_MC_COHESION_HARD3;
      break;
    case RGMaterialType::MC_FRICTION_HARD1:
      materialModels[m] = MM_MC_FRICTION_HARD1;
      break;
    case RGMaterialType::MC_FRICTION_HARD2:
      materialModels[m] = MM_MC_FRICTION_HARD2;
      break;
    case RGMaterialType::MC_FRICTION_HARD3:
      materialModels[m] = MM_MC_FRICTION_HARD3;
      break;
    case RGMaterialType::RIGIDITY:
      materialModels[m] = MM_RIGIDITY;
      break;
    case RGMaterialType::DUALCAP_LINELA:
      materialModels[m] = MM_DUALCAP_LINELA;
      break;
    case RGMaterialType::FRACTURE_ANISOTROPY:
      materialModels[m] = MM_FRACTURE_ANISOTROPY;
      break;
    case RGMaterialType::UPSCALED_ANISOTROPY:
      materialModels[m] = MM_UPSCALED_ANISOTROPY;
      break;
    case RGMaterialType::FRACTURE_APERTURE:
      materialModels[m] = MM_FRACTURE_APERTURE;
      break;
    case RGMaterialType::ANISOTROPIC_CAMCLAY:
      materialModels[m] = MM_ANISOTROPIC_CAMCLAY;
      break;
    case RGMaterialType::FRACTURE_APERTURE2:
      materialModels[m] = MM_FRACTURE_APERTURE2;
      break;
    case RGMaterialType::UNKNOWN:
    default:
      assert(false);

      QString l = QString(UNSUPPORTED_MATERIAL_TYPE).arg(materialTypes[m]);

      m_RMP.AddLogLine(l, &m_RGI, false, true);

      return false;
      break;
    }
  }

  return true;
}

namespace {

std::map<int, int> createFormation2MaterialMap(const std::vector<int> &formationIDs,
                                               const std::vector<int> &materialModels) {
  std::map<int, int> formation2Material;

  for (std::size_t e = 0; e < formationIDs.size(); ++e) {
    std::pair<std::map<int, int>::iterator, bool> inserted =
        formation2Material.insert(std::map<int, int>::value_type(formationIDs[e], materialModels[e]));

    if (!(inserted.second)) {
      assert((*(inserted.first)).second == materialModels[e]);
    }
  }

  return formation2Material;
}

} // anonymous namespace

bool CLoadPropertyMaterialType::assignMaterialModels() const {
  std::vector<int> materialModels(m_materialTypes.size());

  if (mapMaterialType2MaterialModel(materialModels, m_materialTypes)) {
    return assignMaterialModels(materialModels);
  } else {
    return false;
  }

  return true;
}

namespace {

const QString MATERIAL_MODEL_NOT_CREATED = QObject::tr("The material model could not be created");

} // anonymous namespace

bool CLoadPropertyMaterialType::assignMaterialModels(const std::vector<int> &materialModels) const {
  // CRGIFormationIDs rgiFormationIDs(m_RGI);
  std::vector<int> formationIDs; // = rgiFormationIDs.getRGIFormationIDs();

  CGeomecFormationIDs2RGI geomecFormationIDs2RGI(m_RGI, m_ModelBase);

  // if (formationIDs.empty())
  //{
  formationIDs = geomecFormationIDs2RGI.getFormationIDs();
  //}

  std::map<int, int> formation2Material = createFormation2MaterialMap(formationIDs, materialModels);

  for (std::map<int, int>::const_iterator materialModel = formation2Material.begin();
       materialModel != formation2Material.end(); ++materialModel) {
    if ((*materialModel).second == -1)
      continue;

    IMaterial *material = assignMaterialModel((*materialModel).second);

    if (material == 0) {
      QString l = QString(MATERIAL_MODEL_NOT_CREATED);

      m_RMP.AddLogLine(l, &m_RGI, false, true);

      return false;
    }

    CRockMechProcessor::connectMaterial2Formation(m_ModelBase, material,
                                                  geomecFormationIDs2RGI.getFormation((*materialModel).first));
  }

  return true;
}

namespace {

IMaterial *findMaterial(const CGraphEntry::TEntryNodeSet &entryNodeSet, int materialModel) {
  IMaterial *material = 0;

  for (CGraphEntry::TEntryNodeSet::const_iterator entryNode = entryNodeSet.begin(); entryNode != entryNodeSet.end();
       ++entryNode) {
    material = dynamic_cast<IMaterial *>(*entryNode);

    if (material->LibraryMaterial().MaterialModel() == materialModel) {
      break;
    }

    material = 0;
  }

  return material;
}

} // anonymous namespace

IMaterial *CLoadPropertyMaterialType::assignMaterialModel(int materialModel) const {
  CMaterialEntry *materialEntry = dynamic_cast<CMaterialEntry *>(m_ModelBase.GraphEntry(MD_ROCK_MATERIAL));
  CGraphEntry::TEntryNodeSet entryNodeSet = materialEntry->GraphEntryNodes();
  IMaterial *material = findMaterial(entryNodeSet, materialModel);

  // create a material only once

  if (material == 0) {
    materialEntry = CRockMechProcessor::createMaterial(m_ModelBase, "", materialModel);
    entryNodeSet = materialEntry->GraphEntryNodes();
  }

  material = findMaterial(entryNodeSet, materialModel);

  assert(material != 0);

  return material;
}

namespace {

const QString MATERIAL_MODEL_ALREADY_ASSIGNED = QObject::tr("A material type (model) is already assigned, "
                                                            "the previous assignment is ignored");

} // anonymous namespace

bool CLoadPropertyMaterialType::assignMaterialModel() const {
  // and only assign it once

  static bool materialModelIsAssigned = false;

  if (materialModelIsAssigned) {
    QString l = QString(MATERIAL_MODEL_ALREADY_ASSIGNED);

    m_RMP.AddLogLine(l, &m_RGI, false, true);
  }

  if (assignMaterialModels()) {
    materialModelIsAssigned = true;

    return true;
  }

  return false;
}

} // namespace GeomecRGI
