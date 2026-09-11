#ifndef _MATERIALMODELFRACTUREAPERTURE_H_
#define _MATERIALMODELFRACTUREAPERTURE_H_

#include "MaterialCreator.h"

class CMaterialFractureApertureCreatorDEPRECATED : public CMaterialCreator
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

class CMaterialFractureApertureCreatorV2 : public CMaterialCreator
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

typedef CMaterialFractureApertureCreatorV2 CMaterialFractureApertureCreator;


#endif // _MATERIALMODELFRACTUREAPERTURE_H_
