// MaterialRigidity.h: interface for the CMaterialRigidity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALRIGIDITY_H__35E7E0F8_BEB2_423B_8ECE_FB5FCC3F444A__INCLUDED_)
#define AFX_MATERIALRIGIDITY_H__35E7E0F8_BEB2_423B_8ECE_FB5FCC3F444A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMaterialRock.h"
#include "MaterialModelRigidity.h"
#include "Materials.h"

class CMaterialRigidity : public IMaterialRock
{
public:
	CMaterialRigidity(CMaterialEntry &entry, CLibraryMaterial& libmat);
	virtual ~CMaterialRigidity();

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const;
  virtual long MaterialModelFilter() const { return MLFO_RIGIDITY; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialRigidityCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialRigidity);
};

#endif // !defined(AFX_MATERIALRIGIDITY_H__35E7E0F8_BEB2_423B_8ECE_FB5FCC3F444A__INCLUDED_)
