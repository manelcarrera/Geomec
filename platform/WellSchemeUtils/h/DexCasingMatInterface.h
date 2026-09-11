// DexCasingMatInterface.h: interface for the CDexCasingMatInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEXCASINGMATINTERFACE_H__1B939B8A_3AD9_43F8_9E3E_A8422B64F92D__INCLUDED_)
#define AFX_DEXCASINGMATINTERFACE_H__1B939B8A_3AD9_43F8_9E3E_A8422B64F92D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "idexinterface.h"

class AFX_EXT_CLASS CDexCasingMatInterface : public IDexInterface {
  struct SCasingGrades {
    double young_modulus;
    double poisson_ratio;
    double thermal_exp;
  };

  QMap<QString, SCasingGrades *> m_GradesMap;

public:
  struct SCasingDefinition {
    QString casing_type;
    double outer_diameter;
    double inner_diameter;
    double young_modulus;
    double poisson_ratio;
    double thermal_exp;
    double weight;
    double yield_strength;
    double vol_spec_heat;
    double thermal_conduct;
    double eccentricity;
    SCasingDefinition() {
      outer_diameter = 0.0;
      inner_diameter = 0.0;
      young_modulus = 0.0;
      poisson_ratio = 0.0;
      thermal_exp = 0.0;
      weight = 0.0;
      yield_strength = 0.0;
      vol_spec_heat = 4;    // MJ/m3*C
      thermal_conduct = 15; // W/m*C
      eccentricity = 0.0;
    }
  };

  CDexCasingMatInterface();
  virtual ~CDexCasingMatInterface();
  BOOL ImportCasingDefinition(QString path_name, QString &strError, QList<SCasingDefinition *> &casing_definitions);

private:
  BOOL ImportCasingGrades();
};

#endif // !defined(AFX_DEXCASINGMATINTERFACE_H__1B939B8A_3AD9_43F8_9E3E_A8422B64F92D__INCLUDED_)
