#ifndef _MATERIALMODELMODIFIEDMOHRCOULOMB_H_
#define _MATERIALMODELMODIFIEDMOHRCOULOMB_H_

#include "MaterialCreator.h"
#include "LibraryMaterial.h"

class CMaterialModifiedMohrCoulombCreatorDEPRECATED : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter* PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }

public:
  class CPQPlotter : public IPQPlotter
  {
  private:
  virtual IPQPlotter* Clone() const;
  virtual void GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const;
  };
};

class CMaterialModifiedMohrCoulombCreatorV2 : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter* PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }

public:
  class CPQPlotter : public IPQPlotter
  {
  private:
  virtual IPQPlotter* Clone() const;
  virtual void GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const;
  };
};

typedef CMaterialModifiedMohrCoulombCreatorV2 CMaterialModifiedMohrCoulombCreator;


#endif // _MATERIALMODELMODIFIEDMOHRCOULOMB_H_
