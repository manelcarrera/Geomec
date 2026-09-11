// BoundaryInterfaceMaterial.h: interface for the CBoundaryInterfaceMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDARYINTERFACEMATERIAL_H__7AAC1786_1E70_4AD7_B5F7_E3332252CCB7__INCLUDED_)
#define AFX_BOUNDARYINTERFACEMATERIAL_H__7AAC1786_1E70_4AD7_B5F7_E3332252CCB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCElementProperty.h"

class CBoundaryInterfaceMaterial : public dia::IMaterial
{
	double m_dKrad;
	double m_dKtan;
public:
	CBoundaryInterfaceMaterial(const double &krad, const double &ktan);
	virtual ~CBoundaryInterfaceMaterial();

	// write the material assuming the current dir is index of /MATERI or /INPUT/MATERI
	virtual bool WriteFilos(dia::IDianaRunner& diarunner) const;

	virtual bool operator<(const dia::IMaterial &rhs) const;

  // Interface for IElementProperty
  virtual int Type() const;
  virtual int WriteFilosParamSize(dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  double Krad() const { return m_dKrad; }
  double Ktan() const { return m_dKtan; }
};

#endif // !defined(AFX_BOUNDARYINTERFACEMATERIAL_H__7AAC1786_1E70_4AD7_B5F7_E3332252CCB7__INCLUDED_)
