#ifndef _cora_ParameterFault_h_
#define _cora_ParameterFault_h_

class CHorizonBase;

#include "ParameterLimit.h"

namespace cora
{

class CParameterFault : public CParameterLimit
{
  public:
  CParameterFault(const QString& name, const geo::CValue& minimum,
      const geo::CValue& maximum, double mean, unsigned int valueTypeID,
      int depletionStage, CHorizonBase* horizonBase);
  virtual ~CParameterFault();

  virtual int depletionStage() const;
  virtual unsigned int valueTypeID() const;
  virtual const QString& name() const;

  virtual CGraphNode* object() const;

  virtual double min() const;
  virtual double max() const;
  virtual double mean() const;

  virtual std::ostream& operator () (std::ostream& os) const;

  private:
  CParameterFault(const CParameterFault& rhs);
  CParameterFault& operator = (CParameterFault rhs);

  QString m_name;
  geo::CValue m_minimum;
  geo::CValue m_maximum;
  double m_mean;
  unsigned int m_valueTypeID;
  int m_depletionStage;
  CHorizonBase* m_horizonBase;
};

} // namespace cora

#endif  // _cora_ParameterFault_h_
