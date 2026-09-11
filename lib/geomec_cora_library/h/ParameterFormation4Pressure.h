#ifndef _cora_ParameterFormation4Pressure_h_
#define _cora_ParameterFormation4Pressure_h_

#include "ParameterFormation.h"

namespace cora
{

class CParameterFormation4Pressure : public CParameterFormation
{
  public:
    CParameterFormation4Pressure(const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CFormationBase* formationBase, int depletionStage,
      unsigned int valueTypeID);
    virtual ~CParameterFormation4Pressure();

    virtual unsigned int valueTypeID() const;

  private:
    CParameterFormation4Pressure(const CParameterFormation4Pressure& rhs);
    CParameterFormation4Pressure& operator = (CParameterFormation4Pressure rhs);

    unsigned int m_valueTypeID;
};

} // namespace cora

#endif  // _cora_ParameterFormation4Pressure_h_
