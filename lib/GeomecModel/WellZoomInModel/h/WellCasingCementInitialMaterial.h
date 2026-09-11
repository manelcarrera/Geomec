#ifndef _WELLCASINGCEMENTINITIALMATERIAL_H_
#define _WELLCASINGCEMENTINITIALMATERIAL_H_

class CFFMaterial;

#include "IDCElementProperty.h"

class CWellCasingCementInitialMaterial : public dia::IMaterial
{
public:
  CWellCasingCementInitialMaterial(const CFFMaterial& base);

  virtual bool operator<(const dia::IMaterial& rhs) const;
  virtual bool WriteFilos(dia::IDianaRunner& diarunner) const;

  // Interface for IElementProperty
  virtual int Type() const;
  virtual int WriteFilosParamSize(dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

private:
  const CFFMaterial& m_base;
};

#endif // _WELLCASINGCEMENTINITIALMATERIAL_H_
