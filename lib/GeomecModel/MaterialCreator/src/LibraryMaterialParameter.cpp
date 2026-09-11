
#include "LibraryMaterialParameter.h"
#include "LibraryMaterial.h"
#include "ValueTypeFactory.h"

CLibraryMaterialParameter::CLibraryMaterialParameter(const QString& strName,
                                                     double dValue,
                                                     CLibraryMaterial& parentMat,
                                                     const CUnitType& unitConversion,
                                                     unsigned int uiValueTypeID,
                                                     const CCheckStrategy* pCheckStrategy,
                                                     const CSetStrategy* pSetStrategy,
                                                     const QString& strCalibrationPath,
                                                     bool bFixedCalibrationParam)
: ml::CMatParam(strName, dValue, parentMat, unitConversion, pCheckStrategy, pSetStrategy),
  m_strCalibrationPath(strCalibrationPath),
  m_bFixedCalibrationParam(bFixedCalibrationParam),
  m_bCurrentlyFixed(false),
  m_uiValueTypeID(uiValueTypeID)
{
}

CLibraryMaterialParameter::CLibraryMaterialParameter(CLibraryMaterial& parentMat)
: ml::CMatParam(parentMat)
{
}

CLibraryMaterialParameter::CLibraryMaterialParameter(const CLibraryMaterialParameter& rhs, ml::CMaterial& parentMat)
: ml::CMatParam(rhs, parentMat)
{
  *this = rhs;
}

CLibraryMaterialParameter::~CLibraryMaterialParameter()
{
}

CLibraryMaterialParameter* CLibraryMaterialParameter::Clone(ml::CMaterial& parentMat) const
{
  return new CLibraryMaterialParameter(*this, parentMat);
}

CLibraryMaterialParameter& CLibraryMaterialParameter::operator=(const CLibraryMaterialParameter& rhs)
{
  m_strCalibrationPath     = rhs.m_strCalibrationPath;
  m_bFixedCalibrationParam = rhs.m_bFixedCalibrationParam;
  m_bCurrentlyFixed        = rhs.m_bCurrentlyFixed;
  m_uiValueTypeID          = rhs.m_uiValueTypeID;

  return *this;
}

bool CLibraryMaterialParameter::operator==(const CLibraryMaterialParameter& rhs) const
{
  return (
  m_strCalibrationPath     == rhs.m_strCalibrationPath     &&
  m_bFixedCalibrationParam == rhs.m_bFixedCalibrationParam &&
  m_bCurrentlyFixed        == rhs.m_bCurrentlyFixed        &&
  m_uiValueTypeID          == rhs.m_uiValueTypeID);
}

bool CLibraryMaterialParameter::operator!=(const CLibraryMaterialParameter& rhs) const
{
  return !operator==(rhs);
}

const QString& CLibraryMaterialParameter::CalibrationPath() const
{
  return m_strCalibrationPath;
}

bool CLibraryMaterialParameter::IsFixedCalibrationParameter() const
{
  return m_bFixedCalibrationParam;
}

bool CLibraryMaterialParameter::IsCurrentlyFixed() const
{
  return m_bCurrentlyFixed;
}

void CLibraryMaterialParameter::CurrentlyFixed(bool bFixed)
{
  m_bCurrentlyFixed = bFixed;
}

unsigned int CLibraryMaterialParameter::ValueTypeID() const
{
  return m_uiValueTypeID;
}

unsigned int CLibraryMaterialParameter::IconID() const
{
  return CValueTypeFactory::instance()->ValueTypeIconID(m_uiValueTypeID);
}
