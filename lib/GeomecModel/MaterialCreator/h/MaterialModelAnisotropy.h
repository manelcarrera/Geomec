#ifndef _MATERIALMODELANISOTROPY_H_
#define _MATERIALMODELANISOTROPY_H_

#include "mlMatParam.h"
#include "mlMaterial.h"

class CMaterialAnisotropyYoungNormSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyYoungNormSetStrategy* Clone() const { return new CMaterialAnisotropyYoungNormSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyYoungTransSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyYoungTransSetStrategy* Clone() const { return new CMaterialAnisotropyYoungTransSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyPoissonNormSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyPoissonNormSetStrategy* Clone() const { return new CMaterialAnisotropyPoissonNormSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyPoissonTransSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyPoissonTransSetStrategy* Clone() const { return new CMaterialAnisotropyPoissonTransSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyShearStiffSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyShearStiffSetStrategy* Clone() const { return new CMaterialAnisotropyShearStiffSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyVpCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialAnisotropyVpCheckStrategy* Clone() const { return new CMaterialAnisotropyVpCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialAnisotropyVpSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyVpSetStrategy* Clone() const { return new CMaterialAnisotropyVpSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyVsCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialAnisotropyVsCheckStrategy* Clone() const { return new CMaterialAnisotropyVsCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialAnisotropyVsSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyVsSetStrategy* Clone() const { return new CMaterialAnisotropyVsSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyThomsenEpsilonCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenEpsilonCheckStrategy* Clone() const { return new CMaterialAnisotropyThomsenEpsilonCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialAnisotropyThomsenEpsilonSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenEpsilonSetStrategy* Clone() const { return new CMaterialAnisotropyThomsenEpsilonSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyThomsenGammaCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenGammaCheckStrategy* Clone() const { return new CMaterialAnisotropyThomsenGammaCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialAnisotropyThomsenGammaSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenGammaSetStrategy* Clone() const { return new CMaterialAnisotropyThomsenGammaSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyThomsenDeltaCheckStrategy : public ml::CMatParam::CCheckStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenDeltaCheckStrategy* Clone() const { return new CMaterialAnisotropyThomsenDeltaCheckStrategy; }
  virtual bool operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const;
};

class CMaterialAnisotropyThomsenDeltaSetStrategy : public ml::CMatParam::CSetStrategy
{
public:
  //virtual CMaterialAnisotropyThomsenDeltaSetStrategy* Clone() const { return new CMaterialAnisotropyThomsenDeltaSetStrategy; }
  virtual void operator()(double dValue, ml::CMatParam& param) const;
};

class CMaterialAnisotropyCheckStrategy : public ml::CMaterial::CCheckStrategy
{
public:
  CMaterialAnisotropyCheckStrategy() {}
  virtual bool operator () (const ml::CMaterial& material) const;
private:
  CMaterialAnisotropyCheckStrategy(const CMaterialAnisotropyCheckStrategy& rhs);
  CMaterialAnisotropyCheckStrategy& operator = (
    const CMaterialAnisotropyCheckStrategy& rhs);
};

#endif // _MATERIALMODELANISOTROPY_H_
