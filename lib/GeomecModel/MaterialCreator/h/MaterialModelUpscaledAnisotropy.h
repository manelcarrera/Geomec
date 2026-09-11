#ifndef _MATERIALMODELUPSCALEDANISOTROPY_H_
#define _MATERIALMODELUPSCALEDANISOTROPY_H_

#include "MaterialCreator.h"

class CMaterialUpscaledAnisotropyCreatorDEPRECATED : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return true; }
};

class CMaterialUpscaledAnisotropyCreatorV2 : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return true; }
};

typedef CMaterialUpscaledAnisotropyCreatorV2 CMaterialUpscaledAnisotropyCreator;

#endif // _MATERIALMODELUPSCALEDANISOTROPY_H_
