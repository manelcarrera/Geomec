// CohesionQuantity.h: interface for the CCohesionQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COHESIONQUANTITY_H__FECE3B65_6718_45B1_80FC_BAC13D71802B__INCLUDED_)
#define AFX_COHESIONQUANTITY_H__FECE3B65_6718_45B1_80FC_BAC13D71802B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CCohesionQuantity : public CDoubleQuantity 
{
public:
  CCohesionQuantity();

  //##ModelId=3BE7AEB802A6
  CCohesionQuantity(const double &value , const UNIT unit = SI_UNIT);

  CCohesionQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CCohesionQuantity(   const double& value,
            const double &min ,
            const double &max,
            bool bIncludeMin=true,
            bool bIncludeMax=true,
            const UNIT unit= SI_UNIT);

  CCohesionQuantity(const CCohesionQuantity& rhs);


  //##ModelId=3BE7AEB802B6
  double Convert(const double &value, const UNIT out, const UNIT in) const;
  // Quantity interface ...
  //##ModelId=3BE7AEB802BC
  virtual std::string QuantityName() const;
  //##ModelId=3BE7AEB802C5
  virtual std::string UnitName(const UNIT unit) const;
};

#endif // !defined(AFX_COHESIONQUANTITY_H__FECE3B65_6718_45B1_80FC_BAC13D71802B__INCLUDED_)
