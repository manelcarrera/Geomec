#ifndef _ValueType_Delegate_h_
#define _ValueType_Delegate_h_

class CValueType;

#include "IValueComposite_Delegate.h"
#include "ValueType.h"

class CValueType_Delegate : public IValueComposite_Delegate
{
public:
  CValueType_Delegate(CValueType* valueType);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanDestroy() const;
  virtual bool Destroy();

  // Export

  virtual bool CanExport() const;
  virtual void Export();

private:
  CValueType_Delegate(const CValueType_Delegate& rhs);
  CValueType_Delegate& operator = (const CValueType_Delegate& rhs);

  CValueType* m_valueType;

  REGISTER_DELEGATE(CValueType, CValueType_Delegate);
};

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uValueNameId, unsigned int uExportId>
  class CValueTypeTemp_Delegate : public CValueType_Delegate
{
public:
  CValueTypeTemp_Delegate(
    CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportId> *
      valueTypeTemp);

private:
  CValueTypeTemp_Delegate(const CValueTypeTemp_Delegate& rhs);
  CValueTypeTemp_Delegate& operator = (const CValueTypeTemp_Delegate& rhs);

  CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportId> * m_valueTypeTemp;

  typedef CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportId>
    CValueTypeTempTemplate;
  typedef CValueTypeTemp_Delegate <uTypeId, uIconId, uValueNameId, uExportId>
    CValueTypeTemp_DelegateTemplate;

  REGISTER_DELEGATE(CValueTypeTempTemplate, CValueTypeTemp_DelegateTemplate);
};

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uValueNameId, unsigned int uExportId>
  CValueTypeTemp_Delegate <uTypeId, uIconId, uValueNameId, uExportId> ::
    CValueTypeTemp_Delegate(
      CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportId> *
        valueTypeTemp)
: CValueType_Delegate(valueTypeTemp)
, m_valueTypeTemp(valueTypeTemp)
{
  ACTIVATE_TEMPLATE_DELEGATE(CValueTypeTempTemplate,
    CValueTypeTemp_DelegateTemplate);
}

#endif  // _ValueType_Delegate_h_
