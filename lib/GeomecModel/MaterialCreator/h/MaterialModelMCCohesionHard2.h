#ifndef _MATERIALMODELMCCOHESIONHARD2_H_
#define _MATERIALMODELMCCOHESIONHARD2_H_

#include "MaterialModelMCCohesionHard1.h"

class CMaterialModelMCCohesionHard2CreatorDEPRECATED : public CMaterialModelMCCohesionHard1CreatorDEPRECATED
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

class CMaterialModelMCCohesionHard2CreatorV2 : public CMaterialModelMCCohesionHard1CreatorV2
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

typedef CMaterialModelMCCohesionHard2CreatorV2 CMaterialModelMCCohesionHard2Creator;


#endif // _MATERIALMODELMCCOHESIONHARD2_H_
