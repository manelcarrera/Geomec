#ifndef _MATERIALMODELFRACTUREAPERTURE2_H_
#define _MATERIALMODELFRACTUREAPERTURE2_H_

#include "MaterialCreator.h"

class CMaterialFractureAperture2CreatorDEPRECATED : public CMaterialCreator
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

class CMaterialFractureAperture2CreatorV2 : public CMaterialCreator
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

typedef CMaterialFractureAperture2CreatorV2 CMaterialFractureAperture2Creator;


#endif // _MATERIALMODELFRACTUREAPERTURE2_H_
