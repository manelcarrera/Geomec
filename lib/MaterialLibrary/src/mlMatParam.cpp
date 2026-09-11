#include "mlMatParam.h"
#include "mlMaterial.h"

#include <cassert>
#include <cmath>

namespace ml {

CMatParam::CMatParam(const QString& strName,
                     double dValue,
                     CMaterial& parentMat,
                     const CUnitType& unitConversion,
                     const CCheckStrategy* pCheckStrategy,
                     const CSetStrategy* pSetStrategy)
: m_strName(strName),
  m_dValue(dValue),
  m_dPreviousValue(dValue),
  m_parentMat(parentMat),
  m_pUnitType(&unitConversion),
  m_pCheckStrategy(pCheckStrategy),
  m_pSetStrategy(pSetStrategy)
{
}

CMatParam::CMatParam(CMaterial& parentMat)
: m_parentMat(parentMat),
  m_pUnitType(0),
  m_pCheckStrategy(0),
  m_pSetStrategy(0)
{
}

CMatParam::CMatParam(const CMatParam& rhs, CMaterial& parentMat)
: m_parentMat(parentMat)
{
  *this = rhs;
}

CMatParam::~CMatParam()
{
  //delete m_pCheckStrategy;
  //delete m_pSetStrategy;
  //delete m_pUnitType;
}

CMatParam* CMatParam::Clone(CMaterial& parentMat) const
{
  return new CMatParam(*this, parentMat);
}

CMatParam& CMatParam::operator=(const CMatParam& rhs)
{
  m_strName = rhs.m_strName;
  m_dValue = rhs.m_dValue;
  m_dPreviousValue = rhs.m_dPreviousValue;
  m_pUnitType = rhs.m_pUnitType; // ? rhs.m_pUnitType->Clone() : 0;
  m_pCheckStrategy = rhs.m_pCheckStrategy; // ? rhs.m_pCheckStrategy->Clone() : 0;
  m_pSetStrategy = rhs.m_pSetStrategy; // ? rhs.m_pSetStrategy->Clone() : 0;

  return *this;
}

bool CMatParam::operator==(const CMatParam& rhs) const
{
  return (
    m_strName == rhs.m_strName &&
    fabs(m_dValue - rhs.m_dValue) < std::max(1e-20, fabs(m_dValue * 1e-4)));
}

bool CMatParam::operator!=(const CMatParam& rhs) const
{
  return !operator==(rhs);
}

const QString& CMatParam::Name() const
{
  return m_strName;
}

const char *CMatParam::UnitName(int nUnitDef) const
{
  return m_pUnitType->UnitName(nUnitDef);
}

double CMatParam::Value() const
{
  return m_dValue;
}

double CMatParam::ValueToUserUnit(int nUnitDef) const
{
  assert(m_pUnitType);
  return m_pUnitType->ToUserUnit(m_dValue, nUnitDef);
}

bool CMatParam::Value(double dValue)
{
  QString strErrorMsg;

  return Value(dValue, strErrorMsg);
}

bool CMatParam::Value(double dValue, QString& strErrorMsg)
{
  if(!CheckValue(dValue, strErrorMsg))
    return false;

  bool bRet = false;

  double dEps = std::max(1e-20, fabs(m_dValue * 1e-4));
  if(fabs(m_dValue - dValue) > dEps)
  {
    ForceValue(dValue, strErrorMsg);
    bRet = true;
  }

  return bRet;
}

bool CMatParam::ValueFromUserUnit(double dValue, int nUnitDef)
{
  QString strErrorMsg;

  return ValueFromUserUnit(dValue, nUnitDef, strErrorMsg);
}

bool CMatParam::ValueFromUserUnit(double dValue, int nUnitDef, QString& strErrorMsg)
{
  dValue = m_pUnitType->FromUserUnit(dValue, nUnitDef);

  if (!CheckValue(dValue, strErrorMsg))
    return false;

  bool bRet = Value(dValue, strErrorMsg);

  return bRet;
}

bool CMatParam::ForceValue(double dValue, QString& /*strErrorMsg*/)
{
  if(m_pSetStrategy)
    (*m_pSetStrategy)(dValue, *this);
  else
    InternalSetValue(dValue);

  ParentMaterial().Finalize();

  return true;
}

double CMatParam::PreviousValue() const
{
  return m_dPreviousValue;
}

bool CMatParam::CheckDomainValue(double dValue, QString& strErrorMsg) const
{
  if (m_pCheckStrategy && m_pCheckStrategy->IsDomainCheck())
    return CheckValue(dValue, strErrorMsg);
  return true;
}

bool CMatParam::CheckValue(double dValue) const
{
  QString strErrorMsg;

  return CheckValue(dValue, strErrorMsg);
}

bool CMatParam::CheckValue(double dValue, QString& strErrorMsg) const
{
  if(m_pCheckStrategy && !(*m_pCheckStrategy)(dValue, *this, strErrorMsg))
    return false;

  return true;
}

bool CMatParam::CheckValueFromUserUnit(double dValue, int nUnitDef) const
{
  QString strErrorMsg;

  return CheckValueFromUserUnit(dValue, nUnitDef, strErrorMsg);
}

bool CMatParam::CheckValueFromUserUnit(double dValue, int nUnitDef, QString& strErrorMsg) const
{
  dValue = m_pUnitType->FromUserUnit(dValue, nUnitDef);

  if (m_pCheckStrategy && !(*m_pCheckStrategy)(dValue, *this, strErrorMsg, 0))
    return false;

  return true;
}

const CMaterial& CMatParam::ParentMaterial() const
{
  return m_parentMat;
}

CMaterial& CMatParam::ParentMaterial()
{
  return m_parentMat;
}

const CMatParam::CUnitType& CMatParam::UnitConversion() const
{
  assert(m_pUnitType);
  return *m_pUnitType;
}

void CMatParam::LoadValue(double dValue)
{
  InternalSetValue(dValue);
}

void CMatParam::InternalSetValue(double dValue)
{
  m_dPreviousValue = m_dValue;
  m_dValue = dValue;
}


///// CMatParam::CUnitType
CMatParam::CUnitType::CUnitType()
{
}

CMatParam::CUnitType::~CUnitType()
{
}

} // namespace ml