#ifndef _MATERIALMODELMCFRICTIONHARD1_H_
#define _MATERIALMODELMCFRICTIONHARD1_H_

#include "MaterialModelMohrCoulomb.h"

class CMaterialModelMCFrictionHard1CreatorDEPRECATED : public CMaterialMohrCoulombCreatorDEPRECATED {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
};

class CMaterialModelMCFrictionHard1CreatorV2 : public CMaterialMohrCoulombCreatorV2 {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
};

typedef CMaterialModelMCFrictionHard1CreatorV2 CMaterialModelMCFrictionHard1Creator;

#endif // _MATERIALMODELMCFRICTIONHARD1_H_
