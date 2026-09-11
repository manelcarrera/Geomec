#include "ParameterFormation4Material.h"
#include "LibraryMaterialParameter.h"

namespace cora {

CParameterFormation4Material::CParameterFormation4Material(const QString &name, const geo::CValue &minimum,
                                                           const geo::CValue &maximum, double mean,
                                                           CFormationBase *formationBase, int depletionStage,
                                                           CLibraryMaterialParameter &libraryMaterialParameter)
    : CParameterFormation(name, minimum, maximum, mean, formationBase, depletionStage),
      m_libraryMaterialParameter(libraryMaterialParameter) {}

CParameterFormation4Material::~CParameterFormation4Material() {}

unsigned int CParameterFormation4Material::valueTypeID() const { return m_libraryMaterialParameter.ValueTypeID(); }

} // namespace cora
