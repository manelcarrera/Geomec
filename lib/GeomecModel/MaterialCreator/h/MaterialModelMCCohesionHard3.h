#ifndef _MATERIALMODELMCCOHESIONHARD3_H_
#define _MATERIALMODELMCCOHESIONHARD3_H_

#include "MaterialModelMCCohesionHard2.h"

class CMaterialModelMCCohesionHard3CreatorDEPRECATED : public CMaterialModelMCCohesionHard2CreatorDEPRECATED
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

class CMaterialModelMCCohesionHard3CreatorV2 : public CMaterialModelMCCohesionHard2CreatorV2
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

typedef CMaterialModelMCCohesionHard3CreatorV2 CMaterialModelMCCohesionHard3Creator;


#endif // _MATERIALMODELMCCOHESIONHARD3_H_
