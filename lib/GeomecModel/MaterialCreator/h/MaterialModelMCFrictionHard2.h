#ifndef _MATERIALMODELMCFRICTIONHARD2_H_
#define _MATERIALMODELMCFRICTIONHARD2_H_

#include "MaterialModelMCFrictionHard1.h"

class CMaterialModelMCFrictionHard2CreatorDEPRECATED : public CMaterialModelMCFrictionHard1CreatorDEPRECATED
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

class CMaterialModelMCFrictionHard2CreatorV2 : public CMaterialModelMCFrictionHard1CreatorV2
{
protected:
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
};

typedef CMaterialModelMCFrictionHard2CreatorV2 CMaterialModelMCFrictionHard2Creator;


#endif // _MATERIALMODELMCFRICTIONHARD2_H_
