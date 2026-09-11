#ifndef _MATERIALMODELMOHRCOULOMB_H_
#define _MATERIALMODELMOHRCOULOMB_H_

#include "mlMatParam.h"
#include "MaterialCreator.h"
#include "LibraryMaterial.h"

class CMaterialMohrCoulombCreatorDEPRECATED : public CMaterialCreator
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
    virtual IPQPlotter* Clone() const;
    virtual void GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const;
  };
};

class CMaterialMohrCoulombCreatorV2 : public CMaterialCreator
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
    virtual IPQPlotter* Clone() const;
    virtual void GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const;
  };
};

typedef CMaterialMohrCoulombCreatorV2 CMaterialMohrCoulombCreator;


class CMaterialMohrCoulombFrictionCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialMohrCoulombFrictionCheckStrategy* Clone() const { return new CMaterialMohrCoulombFrictionCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialMohrCoulombDilatationCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialMohrCoulombDilatationCheckStrategy* Clone() const { return new CMaterialMohrCoulombDilatationCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

#endif // _MATERIALMODELMOHRCOULOMB_H_
