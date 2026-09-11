#ifndef _cora_ParameterFormation_h_
#define _cora_ParameterFormation_h_

class CFormationBase;

#include "ParameterLimit.h"

namespace cora
{

class CParameterFormation : public CParameterLimit
{
  public:
    CParameterFormation(const QString& name, const geo::CValue& minimum,
      const geo::CValue& maximum, double mean, CFormationBase* formationBase,
      int depletionStage);
    virtual ~CParameterFormation();

    virtual int depletionStage() const;
    virtual const QString& name() const;

    virtual CGraphNode* object() const;

    virtual double min() const;
    virtual double max() const;
    virtual double mean() const;

    virtual std::ostream& operator () (std::ostream& os) const;

  private:
    CParameterFormation(const CParameterFormation& rhs);
    CParameterFormation& operator = (const CParameterFormation& rhs);

    QString m_name;
    geo::CValue m_minimum;
    geo::CValue m_maximum;
    double m_mean;
    CFormationBase* m_formationBase;
    int m_depletionStage;
};

} // namespace cora

#endif  // _cora_ParameterFormation_h_
