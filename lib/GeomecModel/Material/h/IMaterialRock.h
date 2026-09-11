// IMaterialRock.h: interface for the IMaterialRock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMATERIALROCK_H__073683B7_7298_42CA_B8AA_2C3F8AF8F64A__INCLUDED_)
#define AFX_IMATERIALROCK_H__073683B7_7298_42CA_B8AA_2C3F8AF8F64A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMaterial.h"

class IMaterialRock : public IMaterial
{
public:
	IMaterialRock(CMaterialEntry &entry, CLibraryMaterial& libmat);
	virtual unsigned int IconId() const;

  bool WriteTransverseIsotropicValues(const CFFMaterial& ffmat, dia::IDianaRunner& diarunner) const;

  // Interface for dia::IElementProperty -- implementation present
  int WriteFilosTransverseIsotropicParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  bool WriteFilosTransverseIsotropicParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  void WriteFilosTransverseIsotropicParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *&values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitIMaterialRock);
};

#endif // !defined(AFX_IMATERIALROCK_H__073683B7_7298_42CA_B8AA_2C3F8AF8F64A__INCLUDED_)
