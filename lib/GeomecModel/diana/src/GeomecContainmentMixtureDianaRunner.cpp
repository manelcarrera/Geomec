#include "GeomecContainmentMixtureDianaRunner.h"
#include "MaterialMappingCache.h"

CGeomecContainmentMixtureDianaRunner::CGeomecContainmentMixtureDianaRunner(CMeshBase &meshbase, CModelBase &model,
                                                                           CDianaRunController &controller)
    : CGeomecMixtureDianaRunner(meshbase, model, controller) {
  CMaterialMappingCache::instance()->NoCaching(true);
}

CGeomecContainmentMixtureDianaRunner::~CGeomecContainmentMixtureDianaRunner() {
  CMaterialMappingCache::instance()->NoCaching(false);
}

const dia::IMaterial &CGeomecContainmentMixtureDianaRunner::Material(const geo::IElement &element) const {
  const dia::IMaterial &mat = CGeomecMixtureDianaRunner::Material(element);

  ModifyMaterialForContainment(mat, element);

  return mat;
}
