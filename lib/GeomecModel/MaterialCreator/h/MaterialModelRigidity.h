#ifndef _MATERIALMODELRIGIDITY_H_
#define _MATERIALMODELRIGIDITY_H_

#include "MaterialCreator.h"

class CMaterialRigidityCreatorDEPRECATED : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return false; }
};

class CMaterialRigidityCreatorV2 : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return false; }
  virtual bool CanCalibratePlastic() const { return false; }
};

typedef CMaterialRigidityCreatorV2 CMaterialRigidityCreator;

#endif // _MATERIALMODELRIGIDITY_H_
