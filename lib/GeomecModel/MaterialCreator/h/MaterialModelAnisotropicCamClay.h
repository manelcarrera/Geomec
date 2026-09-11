#ifndef _MATERIALMODELANISOTROPICCAMCLAY_H_
#define _MATERIALMODELANISOTROPICCAMCLAY_H_

#include "MaterialCreator.h"
#include "MaterialModelAnisotropy.h"

class CMaterialAnisotropicCamClayCreatorDEPRECATED : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }
};

class CMaterialAnisotropicCamClayCreatorV2 : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }
};

typedef CMaterialAnisotropicCamClayCreatorV2 CMaterialAnisotropicCamClayCreator;


#endif // _MATERIALMODELANISOTROPICCAMCLAY_H_
