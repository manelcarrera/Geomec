#ifndef _cora_ParameterFormation4Material_h_
#define _cora_ParameterFormation4Material_h_

class CFormationBase;
class CLibraryMaterialParameter;

#include "ParameterFormation.h"

namespace cora {

class CParameterFormation4Material : public CParameterFormation {
public:
  CParameterFormation4Material(const QString &name, const geo::CValue &minimum, const geo::CValue &maximum, double mean,
                               CFormationBase *formationBase, int depletionStage,
                               CLibraryMaterialParameter &libraryMaterialParameter);
  virtual ~CParameterFormation4Material();

  virtual unsigned int valueTypeID() const;

private:
  CParameterFormation4Material(const CParameterFormation4Material &rhs);
  CParameterFormation4Material &operator=(CParameterFormation4Material rhs);

  CLibraryMaterialParameter &m_libraryMaterialParameter;
};

} // namespace cora

#endif // _cora_ParameterFormation4Material_h_
