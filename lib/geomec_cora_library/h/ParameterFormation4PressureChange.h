#ifndef _cora_ParameterFormation4PressureChange_h_
#define _cora_ParameterFormation4PressureChange_h_

#include "ParameterFormation.h"

namespace cora
{

class CParameterFormation4PressureChange : public CParameterFormation
{
  public:
    CParameterFormation4PressureChange(const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CFormationBase* formationBase, int depletionStage,
      unsigned int valueTypeID);
    virtual ~CParameterFormation4PressureChange();

    virtual unsigned int valueTypeID() const;

  private:
    CParameterFormation4PressureChange(
      const CParameterFormation4PressureChange& rhs);
    CParameterFormation4PressureChange& operator = (
      CParameterFormation4PressureChange rhs);

    unsigned int m_valueTypeID;
};

} // namespace cora

#endif  // _cora_ParameterFormation4PressureChange_h_
