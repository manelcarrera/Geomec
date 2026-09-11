
#include <cctype>
#include <algorithm>

#include "UnitConversion.h"

IUnitConversion::~IUnitConversion()
{
}

IUnitConversion *IUnitConversion::Create
( const std::string &sJustAName
, const std::string &sUnitName)
{
  std::string sLowerName= sUnitName;
  std::transform(sLowerName.begin(), sLowerName.end(), sLowerName.begin(), tolower);

  return CLinearRationalUnitConversion::Create( sJustAName, sLowerName);
}

void IUnitConversion::Remove(IUnitConversion *conversion)
{
  if ( conversion ) 
  {
    delete conversion;
    conversion= NULL;
  }
}

CLinearRationalUnitConversion::CLinearRationalUnitConversion
( const std::string &sJustAName
, const double &divisor_factor
, const double &divisor_addition
, const double &denominator_factor
, const double &denominator_addition
)
: IUnitConversion(sJustAName)
, m_divisor_factor( divisor_factor )
, m_divisor_addition( divisor_addition )
, m_denominator_factor( denominator_factor )
, m_denominator_addition( denominator_addition )
{
}

CLinearRationalUnitConversion *CLinearRationalUnitConversion::Create
( const std::string &sJustAName
, const std::string &sUnitName)
{

  // to MPa
  if ( sUnitName == "kpa" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-3);
  if ( sUnitName == "bar" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-1);
  if (sUnitName == "kbar")
    return new CLinearRationalUnitConversion(sJustAName, 1e2);
  if (sUnitName == "pa")
    return new CLinearRationalUnitConversion(sJustAName, 1e-6);
  if ( sUnitName == "mpa" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "gpa" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "psi" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-6*6894.75728);
  if ( sUnitName == "kpsi" || sUnitName == "ksi" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-3*6894.75728);

  // to 1/MPa
  if ( sUnitName == "1/kpa" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "1/bar" )
    return new CLinearRationalUnitConversion(sJustAName, 1e1);
  if (sUnitName == "1/kbar")
    return new CLinearRationalUnitConversion(sJustAName, 1e-2);
  if (sUnitName == "1/pa")
    return new CLinearRationalUnitConversion(sJustAName, 1e6);
  if ( sUnitName == "1/mpa" )
    return new CLinearRationalUnitConversion(sJustAName, 1);
  if ( sUnitName == "1/gpa" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-3);
  if ( sUnitName == "1/psi" )
    return new CLinearRationalUnitConversion(sJustAName, 1e6*1/6894.75728);
  if ( sUnitName == "1/kpsi" || sUnitName == "1/ksi" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3/6894.75728);

  // to C
  if ( sUnitName == "c" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "k" )
    return new CLinearRationalUnitConversion(sJustAName, 1,-273.15);
  if ( sUnitName == "f" )
    return new CLinearRationalUnitConversion(sJustAName, 1/1.8,-32/1.8);

  // to kg/m3
  // http://www.unitconversion.org/unit_converter/density.html
  if ( sUnitName == "kg/m3" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "t/m3" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "ppg" || sUnitName == "lb/gal" ) //US
    return new CLinearRationalUnitConversion(sJustAName, 119.82648378907);
  if ( sUnitName == "g/cc" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if (sUnitName == "g/cm3")
    return new CLinearRationalUnitConversion(sJustAName, 1e3);

  // to m
  if ( sUnitName == "m" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "cm" )
    return new CLinearRationalUnitConversion(sJustAName, 1e-2);
  if ( sUnitName == "inch" )
    return new CLinearRationalUnitConversion(sJustAName, 0.0254);
  if ( sUnitName == "ft" )
    return new CLinearRationalUnitConversion(sJustAName, 12*0.0254);

  // to cP (centiPoise)
  // http://en.wikipedia.org/wiki/Viscosity
  if ( sUnitName == "cp" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "p" )
    return new CLinearRationalUnitConversion(sJustAName, 1e2);
  if ( sUnitName == "mpas" ) //Mega
    return new CLinearRationalUnitConversion(sJustAName, 1e9);
  if ( sUnitName == "pas" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);

  // to mD (milliDarcy)
  // http://en.wikipedia.org/wiki/Permeability_(earth_sciences)
  if ( sUnitName == "md" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "d" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "m2" )
    return new CLinearRationalUnitConversion(sJustAName, 1e15);

  // thermal expansion
  // to 1/C
  // http://www.engineeringtoolbox.com/linear-thermal-expansion-d_1379.html
  //
  if ( sUnitName == "1/c" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "1/k" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "1/f" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1,1/1.8,0);

  // heat capacity
  // to J/C
  //
  if ( sUnitName == "j/c" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "j/k" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "j/f" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1,1/1.8,0);
  if ( sUnitName == "cal/c" )
    return new CLinearRationalUnitConversion(sJustAName,  4.184);
  if ( sUnitName == "cal/k" )
    return new CLinearRationalUnitConversion(sJustAName,  4.184);
  if ( sUnitName == "cal/f" )
    return new CLinearRationalUnitConversion(sJustAName, 0,4.184,1/1.8,0);
  if ( sUnitName == "kj/c" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "kj/k" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "kj/f" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3,1/1.8,0);
  if ( sUnitName == "kcal/c" )
    return new CLinearRationalUnitConversion(sJustAName,  1e3*4.184);
  if ( sUnitName == "kcal/k" )
    return new CLinearRationalUnitConversion(sJustAName,  1e3*4.184);
  if ( sUnitName == "kcal/f" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3*4.184,1/1.8,0);

  // conductivity
  // to W/mC
  //
  if ( sUnitName == "w/mc" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "w/mk" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "w/mf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1,1/1.8,0);

  if ( sUnitName == "j/smc" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "j/smk" )
    return new CLinearRationalUnitConversion(sJustAName);
  if ( sUnitName == "j/smf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1,1/1.8,0);

  if ( sUnitName == "kj/smc" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "kj/smk" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3);
  if ( sUnitName == "kj/smf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3,1/1.8,0);

  if ( sUnitName == "cal/smc" ) // cal/s.m.C
    return new CLinearRationalUnitConversion(sJustAName, 4.184);
  if ( sUnitName == "cal/smk" )
    return new CLinearRationalUnitConversion(sJustAName, 4.184);
  if ( sUnitName == "cal/smf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,4.184,1/1.8,0);

  if ( sUnitName == "kcal/smc" ) // kcal/s.m.C
    return new CLinearRationalUnitConversion(sJustAName, 1e3*4.184);
  if ( sUnitName == "kcal/smk" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3*4.184);
  if ( sUnitName == "kcal/smf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3*4.184,1/1.8,0);

  if ( sUnitName == "w/ftc" ) // W/ft.C
    return new CLinearRationalUnitConversion(sJustAName, 1/(12*0.0254));
  if ( sUnitName == "w/ftk" )
    return new CLinearRationalUnitConversion(sJustAName, 1/(12*0.0254));
  if ( sUnitName == "w/ftf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1/(12*0.0254),1/1.8,0);

  if ( sUnitName == "j/sftc" ) // J/s.ft.C
    return new CLinearRationalUnitConversion(sJustAName, 1/(12*0.0254));
  if ( sUnitName == "j/sftk" )
    return new CLinearRationalUnitConversion(sJustAName, 1/(12*0.0254));
  if ( sUnitName == "j/sftf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1/(12*0.0254),1/1.8,0);

  if ( sUnitName == "kj/sftc" ) // kJ/s.ft.C
    return new CLinearRationalUnitConversion(sJustAName, 1e3/(12*0.0254));
  if ( sUnitName == "kj/sftk" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3/(12*0.0254));
  if ( sUnitName == "kj/sftf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3/(12*0.0254),1/1.8,0);

  if ( sUnitName == "cal/sftc" ) // cal/s.ft.C
    return new CLinearRationalUnitConversion(sJustAName, 4.184/(12*0.0254));
  if ( sUnitName == "cal/sftk" )
    return new CLinearRationalUnitConversion(sJustAName, 4.184/(12*0.0254));
  if ( sUnitName == "cal/sftf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,4.184/(12*0.0254),1/1.8,0);

  if ( sUnitName == "kcal/sftc" ) // kcal/s.ft.C
    return new CLinearRationalUnitConversion(sJustAName, 1e3*4.184/(12*0.0254));
  if ( sUnitName == "kcal/sftk" )
    return new CLinearRationalUnitConversion(sJustAName, 1e3*4.184/(12*0.0254));
  if ( sUnitName == "kcal/sftf" )
    return new CLinearRationalUnitConversion(sJustAName, 0,1e3*4.184/(12*0.0254),1/1.8,0);

  return NULL;
}

double CLinearRationalUnitConversion::ConvertToSi(const double &value) const
{
  return 
  ( m_divisor_factor * value + m_divisor_addition ) /
  ( m_denominator_factor * value + m_denominator_addition );
}
