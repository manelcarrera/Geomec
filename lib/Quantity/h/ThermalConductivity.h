// ThermalConductivity.h: interface for the CThermalConductivityQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THERMALCONDUCTIVITY_H__1CF90F35_EE51_4667_86E2_976F545C3068__INCLUDED_)
#define AFX_THERMALCONDUCTIVITY_H__1CF90F35_EE51_4667_86E2_976F545C3068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CThermalConductivityQuantity : public CDoubleQuantity {
public:
  CThermalConductivityQuantity(const CThermalConductivityQuantity &rhs);
  virtual ~CThermalConductivityQuantity();

  CThermalConductivityQuantity();

  CThermalConductivityQuantity(const double &value, const UNIT unit = SI_UNIT);

  CThermalConductivityQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CThermalConductivityQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                               bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_THERMALCONDUCTIVITY_H__1CF90F35_EE51_4667_86E2_976F545C3068__INCLUDED_)
