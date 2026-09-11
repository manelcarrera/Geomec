#include "WellCasingContainmentMixtureDianaRunner.h"
#include "MaterialMappingCache.h"

CWellCasingMixtureContainmentDianaRunner::CWellCasingMixtureContainmentDianaRunner(CWellCasingModel &model,
                                                                                   CDianaRunController &controller)
    : CWellCasingMixtureDianaRunner(model, controller) {
  CMaterialMappingCache::instance()->NoCaching(true);
}

CWellCasingMixtureContainmentDianaRunner::~CWellCasingMixtureContainmentDianaRunner() {
  CMaterialMappingCache::instance()->NoCaching(false);
}

const dia::IMaterial &CWellCasingMixtureContainmentDianaRunner::Material(const geo::IElement &element) const {
  const dia::IMaterial &mat = CGeomecMixtureDianaRunner::Material(element);

  ModifyMaterialForContainment(mat, element);

  return mat;
}
