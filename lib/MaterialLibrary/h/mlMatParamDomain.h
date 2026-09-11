#ifndef _MLMATPARAMDOMAIN_H_
#define _MLMATPARAMDOMAIN_H_

#include "mlMatParam.h"

namespace ml {

class CMatParam;

class CMatParamDomain : public CMatParam::CCheckStrategy
{
public:
  CMatParamDomain(bool bMinIsInf, double dMin, bool bIncludeMin, bool bMaxIsInf, double dMax, bool bIncludeMax);
  CMatParamDomain(const CMatParamDomain& rhs);
  CMatParamDomain* Clone() const;
  virtual bool operator()(double dValue, const CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
  virtual bool IsDomainCheck() const;

#ifdef TEST_MATERIALS
  bool operator==(const CMatParamDomain& rhs) const
  {
  return m_bMinIsInf == rhs.m_bMinIsInf
      && m_dMin == rhs.m_dMin
      && m_bIncludeMin == rhs.m_bIncludeMin
      && m_bMaxIsInf == rhs.m_bMaxIsInf
      && m_dMax == rhs.m_dMax
      && m_bIncludeMax == rhs.m_bIncludeMax;
  }
#endif

private:
  bool m_bMinIsInf;
  double m_dMin;
  bool m_bIncludeMin;
  bool m_bMaxIsInf;
  double m_dMax;
  bool m_bIncludeMax;
};

} // namespace ml

#define MP_GT(dMin)          ml::CMatParamDomain(false, dMin, false, true, 0, false)
#define MP_GE(dMin)          ml::CMatParamDomain(false, dMin, true, true, 0, false)
#define MP_LT(dMax)          ml::CMatParamDomain(true, 0, false, false, dMax, false)
#define MP_LE(dMax)          ml::CMatParamDomain(true, 0, false, false, dMax, true)
#define MP_GT_LT(dMin, dMax) ml::CMatParamDomain(false, dMin, false, false, dMax, false)
#define MP_GT_LE(dMin, dMax) ml::CMatParamDomain(false, dMin, false, false, dMax, true)
#define MP_GE_LT(dMin, dMax) ml::CMatParamDomain(false, dMin, true, false, dMax, false)
#define MP_GE_LE(dMin, dMax) ml::CMatParamDomain(false, dMin, true, false, dMax, true)

#endif // _MLMATPARAMDOMAIN_H_
