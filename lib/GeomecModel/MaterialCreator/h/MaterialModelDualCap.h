#ifndef _MATERIALMODELDUALCAP_H_
#define _MATERIALMODELDUALCAP_H_

#include "MaterialModelCamClay.h"
#include "MaterialCreator.h"

class CMaterialDualCapCreator : public CMaterialCreator
{
public:
  class CPQPlotter : public CMaterialCamClayCreator::CPQPlotter
  {
  private:
    virtual IPQPlotter* Clone() const;
    virtual void GetSecondaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const;
  };
};

class CMaterialDualCapPreconsolidationCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialDualCapPreconsolidationCheckStrategy* Clone() const { return new CMaterialDualCapPreconsolidationCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialDualCapSecondaryPreconCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialDualCapSecondaryPreconCheckStrategy* Clone() const { return new CMaterialDualCapSecondaryPreconCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialDualCapYoungSetStrategy : public CMaterialCamClayYoungSetStrategy
{
public:
  //virtual CMaterialDualCapYoungSetStrategy* Clone() const { return new CMaterialDualCapYoungSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialDualCapPoissonSetStrategy : public CMaterialCamClayPoissonSetStrategy
{
public:
  //virtual CMaterialDualCapPoissonSetStrategy* Clone() const { return new CMaterialDualCapPoissonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialDualCapPorositySetStrategy : public CMaterialCamClayPorositySetStrategy
{
public:
  //virtual CMaterialDualCapPorositySetStrategy* Clone() const { return new CMaterialDualCapPorositySetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialDualCapPreconsolidationSetStrategy : public CMaterialCamClayPreconsolidationSetStrategy
{
public:
  //virtual CMaterialDualCapPreconsolidationSetStrategy* Clone() const { return new CMaterialDualCapPreconsolidationSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialDualCapElasticHardeningSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialDualCapElasticHardeningSetStrategy* Clone() const { return new CMaterialDualCapElasticHardeningSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

#endif // _MATERIALMODELDUALCAP_H_
