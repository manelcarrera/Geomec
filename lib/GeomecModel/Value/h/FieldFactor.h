// FieldFactor.h: interface for the CFieldFactor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELDFACTOR_H__020278DF_69FE_42E3_82D7_B865F0C1B14D__INCLUDED_)
#define AFX_FIELDFACTOR_H__020278DF_69FE_42E3_82D7_B865F0C1B14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// These factors are to calculate from SI to Field!! Multiply the SI value with the factor and a value in Field unit is
// the result.
#define FF_FACTOR_PRESSURE 145.038   // From MPa to psi
#define FF_FACTOR_STRESS 145.038     // From MPa to psi
#define FF_FACTOR_LENGTH 3.2808399   // From m to ft 061120 jbj Mantis #2322 based on official factor (imperial ft)
#define FF_FACTOR_VELOCITY 3.2808399 // From m to ft 061120 jbj Mantis #2322 based on official factor (imperial ft)
#define FF_FACTOR_DIMENSIONLESS 1.0
#define FF_FACTOR_PERTIME 1.0
#define FF_FACTOR_TIME 1.0
#define FF_FACTOR_DENSITY 0.008345405 // wjrx mantis 2522 conversion from kg/m3 to lb/gallon(=3.78L)
#define FF_FACTOR_PERTEMPER 1.0
#define FF_FACTOR_TEMPERATURE 1.0
#define FF_FACTOR_PERSTRESS 6.894744825494e-03      // From 1/MPa to 1/psi (1 / 145.038)
#define FF_FACTOR_STRESSGRADIENT 44.207582332804475 // From MPa/m to psi/ft (145.038/3.2808399)
#define FF_FACTOR_THERM_CONDUCT 0.3048              // W/(m,K) to W/(ft,K) (1/3.2808399)
#define FF_FACTOR_THERM_CAPACI 0.45359237           // From J/(kg,K) to J/(lb,K) (1/2.2046226218)
#define FF_FACTOR_PERLENGTH 0.3048
#define FF_FACTOR_PERMEA 1.0
#define FF_FACTOR_VISCOSITY 1.0

#endif // !defined(AFX_FIELDFACTOR_H__020278DF_69FE_42E3_82D7_B865F0C1B14D__INCLUDED_)
