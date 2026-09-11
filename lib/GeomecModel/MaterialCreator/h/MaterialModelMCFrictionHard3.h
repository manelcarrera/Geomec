#ifndef _MATERIALMODELMCFRICTIONHARD3_H_
#define _MATERIALMODELMCFRICTIONHARD3_H_

#include "MaterialModelMCFrictionHard2.h"

class CMaterialModelMCFrictionHard3CreatorDEPRECATED : public CMaterialModelMCFrictionHard2CreatorDEPRECATED {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

class CMaterialModelMCFrictionHard3CreatorV2 : public CMaterialModelMCFrictionHard2CreatorV2 {
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

typedef CMaterialModelMCFrictionHard3CreatorV2 CMaterialModelMCFrictionHard3Creator;

#endif // _MATERIALMODELMCFRICTIONHARD3_H_
