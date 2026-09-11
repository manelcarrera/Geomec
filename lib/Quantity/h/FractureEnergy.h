// FractureEnergy.h: interface for the CFractureEnergyQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRACTUREENERGY_H__216934EB_0468_419E_B80C_F21484B3CFE4__INCLUDED_)
#define AFX_FRACTUREENERGY_H__216934EB_0468_419E_B80C_F21484B3CFE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CFractureEnergyQuantity : public CDoubleQuantity {
public:
  CFractureEnergyQuantity(const CFractureEnergyQuantity &rhs);
  CFractureEnergyQuantity();
  virtual ~CFractureEnergyQuantity();

  CFractureEnergyQuantity(const double &value, const UNIT unit = SI_UNIT);

  CFractureEnergyQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CFractureEnergyQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                          bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_FRACTUREENERGY_H__216934EB_0468_419E_B80C_F21484B3CFE4__INCLUDED_)
