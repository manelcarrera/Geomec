// StressGradOrDensity.h: interface for the CStressGradOrDensity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_StressGradOrDensity_H__ED4B0135_95C7_4960_9AE8_FC63BC9B2864__INCLUDED_)
#define AFX_StressGradOrDensity_H__ED4B0135_95C7_4960_9AE8_FC63BC9B2864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CStressGradOrDensity : public CDoubleQuantity {
public:
  enum TYPE { GRAD, DENS };

  CStressGradOrDensity();
  virtual ~CStressGradOrDensity();

  CStressGradOrDensity(const CStressGradOrDensity &rhs);

  CStressGradOrDensity(const double &value, const UNIT unit = SI_UNIT);

  CStressGradOrDensity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CStressGradOrDensity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                       bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;

  TYPE GetType() const;
  void SetType(TYPE type);

  double GetGradient(const UNIT unit = SI_UNIT) const;
  double GetDensity(const UNIT unit = SI_UNIT) const;

  void SetGradient(const double &value, const UNIT unit = SI_UNIT);
  void SetDensity(const double &value, const UNIT unit = SI_UNIT);

  virtual void restore(IModelStream &stream);
  virtual void store(IModelStream &stream) const;

private:
  void Init();
  TYPE m_Type;
};

#endif // !defined(AFX_StressGradOrDensity_H__ED4B0135_95C7_4960_9AE8_FC63BC9B2864__INCLUDED_)
