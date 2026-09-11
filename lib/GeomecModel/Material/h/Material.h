// Material.h: interface for the CMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIAL_H__050CDC72_BE27_45BE_BD73_04352D1F5C18__INCLUDED_)
#define AFX_MATERIAL_H__050CDC72_BE27_45BE_BD73_04352D1F5C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IMaterialBase;

#include "IDCElementProperty.h"

class CFFMaterial : public dia::IMaterial {
public:
  typedef std::map<unsigned int, double> TValueMap;

  CFFMaterial(IMaterialBase &Material, const TValueMap &value_map);
  ~CFFMaterial();

  void SetParameterValue(unsigned int uValueTypeID, double dValue);

  const IMaterialBase &Material() const;

  bool IsParameter(unsigned int ValueTypeID) const;
  double ParameterValue(unsigned int ValueTypeID) const;
  bool IsDistributedParameter(unsigned int ValueTypeID) const;

  bool operator<(const dia::IMaterial &rhs) const;
  virtual bool WriteFilos(dia::IDianaRunner &diarunner) const;

  virtual int Type() const;
  virtual int WriteFilosParamSize(dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(dia::IDianaRunner &diarunner, double *values, int stride = 1) const;

private:
  IMaterialBase &m_Material;
  TValueMap m_mpValue;
};

#endif // !defined(AFX_MATERIAL_H__050CDC72_BE27_45BE_BD73_04352D1F5C18__INCLUDED_)
