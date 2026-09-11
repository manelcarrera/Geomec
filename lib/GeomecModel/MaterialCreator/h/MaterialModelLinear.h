#ifndef _MATERIALMODELLINEAR_H_
#define _MATERIALMODELLINEAR_H_

#include "MaterialCreator.h"
#include "mlMatParam.h"

class CMaterialLinearCreatorDEPRECATED : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return false; }
};

class CMaterialLinearCreatorV2 : public CMaterialCreator {
protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return false; }
};

typedef CMaterialLinearCreatorV2 CMaterialLinearCreator;

class CMaterialLinearYoungSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearYoungSetStrategy* Clone() const { return new CMaterialLinearYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearPoissonSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearPoissonSetStrategy* Clone() const { return new CMaterialLinearPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearCMESetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearCMESetStrategy* Clone() const { return new CMaterialLinearCMESetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearBulkStiffCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearBulkStiffCheckStrategy* Clone() const { return new CMaterialLinearBulkStiffCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearBulkStiffSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearBulkStiffSetStrategy* Clone() const { return new CMaterialLinearBulkStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearShearStiffCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearShearStiffCheckStrategy* Clone() const { return new CMaterialLinearShearStiffCheckStrategy;
  // }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearShearStiffSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearShearStiffSetStrategy* Clone() const { return new CMaterialLinearShearStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearDynUniCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearDynUniCheckStrategy* Clone() const { return new CMaterialLinearDynUniCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearDynUniSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearDynUniSetStrategy* Clone() const { return new CMaterialLinearDynUniSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearDynShearCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearDynShearCheckStrategy* Clone() const { return new CMaterialLinearDynShearCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearDynShearSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearDynShearSetStrategy* Clone() const { return new CMaterialLinearDynShearSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearRhoBSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearRhoBSetStrategy* Clone() const { return new CMaterialLinearRhoBSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearVelocityPCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearVelocityPCheckStrategy* Clone() const { return new CMaterialLinearVelocityPCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearVelocityPSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearVelocityPSetStrategy* Clone() const { return new CMaterialLinearVelocityPSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearVelocitySCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearVelocitySCheckStrategy* Clone() const { return new CMaterialLinearVelocitySCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearVelocitySSetStrategy : public ml::CMatParam::CSetStrategy {
public:
  // virtual CMaterialLinearVelocitySSetStrategy* Clone() const { return new CMaterialLinearVelocitySSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam &param) const;
};

class CMaterialLinearLatRatioMinCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearLatRatioMinCheckStrategy* Clone() const { return new
  // CMaterialLinearLatRatioMinCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialLinearLatRatioMaxCheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialLinearLatRatioMaxCheckStrategy* Clone() const { return new
  // CMaterialLinearLatRatioMaxCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

#endif // _MATERIALMODELLINEAR_H_
