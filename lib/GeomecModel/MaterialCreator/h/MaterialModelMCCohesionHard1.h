#ifndef _MATERIALMODELMCCOHESIONHARD1_H_
#define _MATERIALMODELMCCOHESIONHARD1_H_

#include "MaterialModelMohrCoulomb.h"

class CMaterialModelMCCohesionHard1CreatorDEPRECATED : public CMaterialMohrCoulombCreatorDEPRECATED {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
};

class CMaterialModelMCCohesionHard1CreatorV2 : public CMaterialMohrCoulombCreatorV2 {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
};

typedef CMaterialModelMCCohesionHard1CreatorV2 CMaterialModelMCCohesionHard1Creator;

#endif // _MATERIALMODELMCCOHESIONHARD1_H_
