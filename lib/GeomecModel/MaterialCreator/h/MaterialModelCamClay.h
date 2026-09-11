#ifndef _MATERIALMODELCAMCLAY_H_
#define _MATERIALMODELCAMCLAY_H_

#include "LibraryMaterial.h"
#include "MaterialCreator.h"
#include "mlMatParam.h"

#include "MaterialModelLinear.h"

class CMaterialCamClayCreatorDEPRECATED : public CMaterialCreator {
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

class CMaterialCamClayCreatorV2 : public CMaterialCreator {
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

typedef CMaterialCamClayCreatorV2 CMaterialCamClayCreator;

class CMaterialCamClayYoungSetStrategy : public CMaterialLinearYoungSetStrategy {
public:
  // virtual CMaterialCamClayYoungSetStrategy* Clone() const { return new CMaterialCamClayYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayPoissonSetStrategy : public CMaterialLinearPoissonSetStrategy {
public:
  // virtual CMaterialCamClayPoissonSetStrategy* Clone() const { return new CMaterialCamClayPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayBulkStiffSetStrategy : public CMaterialLinearBulkStiffSetStrategy {
public:
  // virtual CMaterialCamClayBulkStiffSetStrategy* Clone() const { return new CMaterialCamClayBulkStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayShearStiffSetStrategy : public CMaterialLinearShearStiffSetStrategy {
public:
  // virtual CMaterialCamClayShearStiffSetStrategy* Clone() const { return new CMaterialCamClayShearStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayDynUniSetStrategy : public CMaterialLinearDynUniSetStrategy {
public:
  // virtual CMaterialCamClayDynUniSetStrategy* Clone() const { return new CMaterialCamClayDynUniSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayDynShearSetStrategy : public CMaterialLinearDynShearSetStrategy {
public:
  // virtual CMaterialCamClayDynShearSetStrategy* Clone() const { return new CMaterialCamClayDynShearSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayVelocityPSetStrategy : public CMaterialLinearVelocityPSetStrategy {
public:
  // virtual CMaterialCamClayVelocityPSetStrategy* Clone() const { return new CMaterialCamClayVelocityPSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayVelocitySSetStrategy : public CMaterialLinearVelocitySSetStrategy {
public:
  // virtual CMaterialCamClayVelocitySSetStrategy* Clone() const { return new CMaterialCamClayVelocitySSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCMESetStrategy : public CMaterialLinearCMESetStrategy {
public:
  // virtual CMaterialCamClayCMESetStrategy* Clone() const { return new CMaterialCamClayCMESetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayPorositySetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayPorositySetStrategy* Clone() const { return new CMaterialCamClayPorositySetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayFrictionSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayFrictionSetStrategy* Clone() const { return new CMaterialCamClayFrictionSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayHardeningSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayHardeningSetStrategy* Clone() const { return new CMaterialCamClayHardeningSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayPreconsolidationSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayPreconsolidationSetStrategy* Clone() const { return new
  // CMaterialCamClayPreconsolidationSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCapShapSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCapShapSetStrategy* Clone() const { return new CMaterialCamClayCapShapSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialCamClayCMPCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialCamClayCMPCheckStrategy* Clone() const { return new CMaterialCamClayCMPCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialCamClayCMPSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialCamClayCMPSetStrategy* Clone() const { return new CMaterialCamClayCMPSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

#endif // _MATERIALMODELCAMCLAY_H_
