#ifndef _MATERIALMODELCAMCLAYCREEP_H_
#define _MATERIALMODELCAMCLAYCREEP_H_

#include "LibraryMaterial.h"
#include "MaterialCreator.h"
#include "mlMatParam.h"

#include "MaterialModelLinear.h"

class CMaterialCamClayCreepCreatorDEPRECATED : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter *PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }

public:
  class CPQPlotter : public IPQPlotter {
  protected:
    void GetYieldCurve(const CLibraryMaterial &mat, double dPrecon, double dPMin, double dPMax,
                       CStressStrainArray &values) const;

  private:
    virtual IPQPlotter *Clone() const;
    virtual void GetPrimaryYieldCurve(const CLibraryMaterial &mat, double dPMin, double dPMax,
                                      CStressStrainArray &values) const;
    double GetCapShapeQ(double m2, double beta2, double p, double a) const;
  };
};

class CMaterialCamClayCreepCreatorV2 : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual IPQPlotter *PQPlotter() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return true; }

public:
  class CPQPlotter : public IPQPlotter {
  protected:
    void GetYieldCurve(const CLibraryMaterial &mat, double dPrecon, double dPMin, double dPMax,
                       CStressStrainArray &values) const;

  private:
    virtual IPQPlotter *Clone() const;
    virtual void GetPrimaryYieldCurve(const CLibraryMaterial &mat, double dPMin, double dPMax,
                                      CStressStrainArray &values) const;
    double GetCapShapeQ(double m2, double beta2, double p, double a) const;
  };
};

typedef CMaterialCamClayCreepCreatorV2 CMaterialCamClayCreepCreator;

class CMaterialCamClayCreepYoungSetStrategy : public CMaterialLinearYoungSetStrategy {
public:
  // virtual CMaterialCamClayCreepYoungSetStrategy* Clone() const { return new CMaterialCamClayCreepYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepPoissonSetStrategy : public CMaterialLinearPoissonSetStrategy {
public:
  // virtual CMaterialCamClayCreepPoissonSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepBulkStiffSetStrategy : public CMaterialLinearBulkStiffSetStrategy {
public:
  // virtual CMaterialCamClayCreepBulkStiffSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepBulkStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepShearStiffSetStrategy : public CMaterialLinearShearStiffSetStrategy {
public:
  // virtual CMaterialCamClayCreepShearStiffSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepShearStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepDynUniSetStrategy : public CMaterialLinearDynUniSetStrategy {
public:
  // virtual CMaterialCamClayCreepDynUniSetStrategy* Clone() const { return new CMaterialCamClayCreepDynUniSetStrategy;
  // }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepDynShearSetStrategy : public CMaterialLinearDynShearSetStrategy {
public:
  // virtual CMaterialCamClayCreepDynShearSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepDynShearSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepVelocityPSetStrategy : public CMaterialLinearVelocityPSetStrategy {
public:
  // virtual CMaterialCamClayCreepVelocityPSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepVelocityPSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepVelocitySSetStrategy : public CMaterialLinearVelocitySSetStrategy {
public:
  // virtual CMaterialCamClayCreepVelocitySSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepVelocitySSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepCMESetStrategy : public CMaterialLinearCMESetStrategy {
public:
  // virtual CMaterialCamClayCreepCMESetStrategy* Clone() const { return new CMaterialCamClayCreepCMESetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepPorositySetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCreepPorositySetStrategy* Clone() const { return new
  // CMaterialCamClayCreepPorositySetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepFrictionSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCreepFrictionSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepFrictionSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepHardeningSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCreepHardeningSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepHardeningSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepPreconsolidationSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  virtual CMaterialCamClayCreepPreconsolidationSetStrategy *Clone() const {
    return new CMaterialCamClayCreepPreconsolidationSetStrategy;
  }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepCapShapSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCreepCapShapSetStrategy* Clone() const { return new
  // CMaterialCamClayCreepCapShapSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCreepCMPCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialCamClayCreepCMPCheckStrategy* Clone() const { return new CMaterialCamClayCreepCMPCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialCamClayCreepCMPSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCreepCMPSetStrategy* Clone() const { return new CMaterialCamClayCreepCMPSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

#endif // _MATERIALMODELCAMCLAYCREEP_H_
