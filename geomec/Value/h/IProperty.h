// IProperty.h: interface for the IProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROPERTY_H__1AAA0899_3BC8_4D11_A9FF_D63F079CE4BE__INCLUDED_)
#define AFX_IPROPERTY_H__1AAA0899_3BC8_4D11_A9FF_D63F079CE4BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueTypeFactory.h"

class IProperty {
  CGraphModel &m_model;
  CPointSet *m_pPointSet;
  std::vector<CValueType *> m_vcValueType;

  typedef std::pair<unsigned int, unsigned int> TValueType;
  typedef std::vector<unsigned int> TComponentVec;
  typedef std::map<CString, TValueType> TValueTypeMap;
  typedef std::map<CString, TComponentVec> TComponentMap;
  TValueTypeMap m_mpValueType;
  TValueTypeMap m_mpComponent;

  typedef std::map<unsigned int, std::vector<TComponentVec>> THeaderTagMap;
  THeaderTagMap m_mpHeader;

  // Definition of the header map
  virtual BOOL DefineValueType(unsigned int uHeaderID, unsigned int uValueTypeID, unsigned int uMode);
  virtual BOOL DefineComponent(unsigned int uHeaderID, unsigned int uValueTypeID, unsigned int uComponentID,
                               unsigned int uMode);

public:
  // Construction
  IProperty(CGraphModel &model);
  virtual ~IProperty();

  // Definition of Value Types
  size_t ValueTypeSize() const;
  const CValueType &ValueType(size_t nIndex) const;
  CValueType &ValueType(size_t nIndex);
  BOOL AddValueType(CValueType &value_type);

  // Create value type
  BOOL CreateValueType(const CString &sHeaderTag);
  const CString &HeaderTag(CValueComponent &component) const;
};

#endif // !defined(AFX_IPROPERTY_H__1AAA0899_3BC8_4D11_A9FF_D63F079CE4BE__INCLUDED_)
