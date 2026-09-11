#ifndef _MATERIALMODELUNDRAINED_H_
#define _MATERIALMODELUNDRAINED_H_

#include "MaterialCreator.h"
#include "MaterialModelLinear.h"

class CMaterialUndrainedCreatorDEPRECATED : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return false; }
};

class CMaterialUndrainedCreatorV2 : public CMaterialCreator
{
protected:
  virtual void OnCreateLibrary(CLibraryMaterial& m);
  virtual void OnCreateLibraryParameters(CLibraryMaterial& m);
  virtual int MaterialModel();
  virtual QString MaterialModelName();
  virtual QString CalibrationPath() const;
  virtual bool CanCalibrateElastic() const { return true; }
  virtual bool CanCalibratePlastic() const { return false; }
};

typedef CMaterialUndrainedCreatorV2 CMaterialUndrainedCreator;


class CMaterialUndrainedYoungSetStrategy : public CMaterialLinearYoungSetStrategy
{
public:
  //virtual CMaterialUndrainedYoungSetStrategy* Clone() const { return new CMaterialUndrainedYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedPoissonSetStrategy : public CMaterialLinearPoissonSetStrategy
{
public:
  //virtual CMaterialUndrainedPoissonSetStrategy* Clone() const { return new CMaterialUndrainedPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedUndrainedYoungSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedUndrainedYoungSetStrategy* Clone() const { return new CMaterialUndrainedUndrainedYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedUndrainedPoissonSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedUndrainedPoissonSetStrategy* Clone() const { return new CMaterialUndrainedUndrainedPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy* Clone() const { return new CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedPorositySetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedPorositySetStrategy* Clone() const { return new CMaterialUndrainedPorositySetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedGrainStiffnessSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedGrainStiffnessSetStrategy* Clone() const { return new CMaterialUndrainedGrainStiffnessSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedFluidBulkModulusSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedFluidBulkModulusSetStrategy* Clone() const { return new CMaterialUndrainedFluidBulkModulusSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy* Clone() const { return new CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy* Clone() const { return new CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

#endif // _MATERIALMODELUNDRAINED_H_
