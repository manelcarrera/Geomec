#ifndef _ValueVector_Delegate_h_
#define _ValueVector_Delegate_h_

#include "ValueType_Delegate.h"
#include "ValueVector.h"
#include "ValueComponent_Delegate.h"

template <unsigned int uComponentName, unsigned int uComponentIndex>
  class CVectorComponentTemp_Delegate :
  public CComponentTemp_Delegate <uComponentName, uComponentIndex>
{
public:
  CVectorComponentTemp_Delegate(
  CVectorComponentTemp <uComponentName, uComponentIndex> *
      vectorComponentTemp);

private:
  CVectorComponentTemp_Delegate(const CVectorComponentTemp_Delegate& rhs);
  CVectorComponentTemp_Delegate& operator = (
  const CVectorComponentTemp_Delegate& rhs);

  CVectorComponentTemp <uComponentName, uComponentIndex> *
  m_vectorComponentTemp;

  typedef CVectorComponentTemp <uComponentName, uComponentIndex>
  CVectorComponentTempTemplate;
  typedef CVectorComponentTemp_Delegate <uComponentName, uComponentIndex>
  CVectorComponentTemp_DelegateTemplate;

  REGISTER_DELEGATE(CVectorComponentTempTemplate,
  CVectorComponentTemp_DelegateTemplate);
};

template <unsigned int uComponentName, unsigned int uComponentIndex>
  CVectorComponentTemp_Delegate <uComponentName, uComponentIndex> ::
  CVectorComponentTemp_Delegate(
      CVectorComponentTemp <uComponentName, uComponentIndex> *
    vectorComponentTemp)
: CComponentTemp_Delegate <uComponentName, uComponentIndex> (
  vectorComponentTemp)
, m_vectorComponentTemp(vectorComponentTemp)
{
  ACTIVATE_TEMPLATE_DELEGATE(CVectorComponentTempTemplate,
  CVectorComponentTemp_DelegateTemplate);
}

template <unsigned int uTypeId, unsigned int uExport, unsigned int uIconId, unsigned int uValueNameId>
  class CValueVectorTemp_Delegate :
  public CValueTypeTemp_Delegate <uTypeId, uIconId, uValueNameId, uExport>
{
public:
  CValueVectorTemp_Delegate(
  CValueVectorTemp <uTypeId, uExport, uIconId, uValueNameId> *
      valueVectorTemp);

private:
  CValueVectorTemp_Delegate(const CValueVectorTemp_Delegate& rhs);
  CValueVectorTemp_Delegate& operator = (const CValueVectorTemp_Delegate& rhs);

  CValueVectorTemp <uTypeId, uExport, uIconId, uValueNameId> *
  m_valueVectorTemp;

  typedef CValueVectorTemp <uTypeId, uExport, uIconId, uValueNameId>
  CValueVectorTempTemplate;
  typedef CValueVectorTemp_Delegate <uTypeId, uExport, uIconId, uValueNameId>
  CValueVectorTemp_DelegateTemplate;

  REGISTER_DELEGATE(CValueVectorTempTemplate,
  CValueVectorTemp_DelegateTemplate);
};

template <unsigned int uTypeId, unsigned int uExport, unsigned int uIconId, unsigned int uValueNameId>
  CValueVectorTemp_Delegate <uTypeId, uExport, uIconId, uValueNameId> ::
  CValueVectorTemp_Delegate(
      CValueVectorTemp <uTypeId, uExport, uIconId, uValueNameId> *
    valueVectorTemp)
: CValueTypeTemp_Delegate <uTypeId, uIconId, uValueNameId, uExport> (
  valueVectorTemp)
, m_valueVectorTemp(valueVectorTemp)
{
  ACTIVATE_TEMPLATE_DELEGATE(CValueVectorTempTemplate,
  CValueVectorTemp_DelegateTemplate);
}

class TDisplacementVector_Delegate :
  public CValueVectorTemp_Delegate <IDT_VALUETYPE_DISPLACEMENT,
  IDS_ET_DISPLACEMENT, IDI_VALUETYPE_DISPLACEMENT, IDS_VALUENAME_DISP_VECTOR>
{
public:
  TDisplacementVector_Delegate(TDisplacementVector* displacementVector);

private:
  TDisplacementVector_Delegate(const TDisplacementVector_Delegate& rhs);
  TDisplacementVector_Delegate& operator = (
  const TDisplacementVector_Delegate& rhs);

  TDisplacementVector* m_displacementVector;

  REGISTER_DELEGATE(TDisplacementVector, TDisplacementVector_Delegate);
};

#endif  // _ValueVector_Delegate_h_
