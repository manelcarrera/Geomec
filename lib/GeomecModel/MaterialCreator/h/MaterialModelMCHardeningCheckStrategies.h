#ifndef _MATERIALMODELMCHARDENINGCHECKSTRATEGIES_H_
#define _MATERIALMODELMCHARDENINGCHECKSTRATEGIES_H_

#include "mlMatParam.h"

class CMaterialMCHardEquivalentPlasticStrain1CheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialMCHardEquivalentPlasticStrain1CheckStrategy* Clone() const { return new
  // CMaterialMCHardEquivalentPlasticStrain1CheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialMCHardEquivalentPlasticStrain2CheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialMCHardEquivalentPlasticStrain2CheckStrategy* Clone() const { return new
  // CMaterialMCHardEquivalentPlasticStrain2CheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialMCHardEquivalentPlasticStrain3CheckStrategy : public ml::CMatParam::CCheckStrategy {
public:
  // virtual CMaterialMCHardEquivalentPlasticStrain3CheckStrategy* Clone() const { return new
  // CMaterialMCHardEquivalentPlasticStrain3CheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg, int nUnitDef = 0) const;
};

#endif // _MATERIALMODELMCHARDENINGCHECKSTRATEGIES_H_
