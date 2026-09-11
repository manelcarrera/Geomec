#ifndef _cora_ParameterNonMeshedSurface_h_
#define _cora_ParameterNonMeshedSurface_h_

class CSurfaceBase;

#include "ParameterLimit.h"

namespace cora
{

class CParameterNonMeshedSurface : public CParameterLimit
{
  public:
  CParameterNonMeshedSurface(const QString& name, const geo::CValue& minimum,
      const geo::CValue& maximum, double mean, unsigned int valueTypeID,
      int depletionStage, CSurfaceBase* surfaceBase);
  virtual ~CParameterNonMeshedSurface();

  virtual int depletionStage() const;
  virtual unsigned int valueTypeID() const;
  virtual const QString& name() const;

  virtual CGraphNode* object() const;

  virtual double min() const;
  virtual double max() const;
  virtual double mean() const;

  virtual std::ostream& operator () (std::ostream& os) const;

  private:
  CParameterNonMeshedSurface(const CParameterNonMeshedSurface& rhs);
  CParameterNonMeshedSurface& operator = (
      const CParameterNonMeshedSurface& rhs);

  QString m_name;
  geo::CValue m_minimum;
  geo::CValue m_maximum;
  double m_mean;
  unsigned int m_valueTypeID;
  int m_depletionStage;
  CSurfaceBase* m_surfaceBase;
};

} // namespace cora

#endif  // _cora_ParameterNonMeshedSurface_h_
