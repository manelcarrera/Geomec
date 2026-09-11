#ifndef _MATERIALMODELFRACTUREANISOTROPY_H_
#define _MATERIALMODELFRACTUREANISOTROPY_H_

#include "MaterialCreator.h"

class CMaterialFractureAnisotropyCreatorDEPRECATED : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return true; }
};

class CMaterialFractureAnisotropyCreatorV2 : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return true; }
};

typedef CMaterialFractureAnisotropyCreatorV2 CMaterialFractureAnisotropyCreator;


#endif // _MATERIALMODELFRACTUREANISOTROPY_H_
