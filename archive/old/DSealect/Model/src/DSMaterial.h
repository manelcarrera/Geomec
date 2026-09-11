// DSMaterial.h: interface for the CDSMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSMATERIAL_H__BDA8C876_AA88_4754_B618_FD1CDA99CBFF__INCLUDED_)
#define AFX_DSMATERIAL_H__BDA8C876_AA88_4754_B618_FD1CDA99CBFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IComponent.h"

#define MATERIAL_NAME "MaterialName"
#define HARDENING_IDS_OFFSET 4000



//##ModelId=3C60D563030E
enum eHardeningType
{
	//##ModelId=3C60D563032D
	None=0,
	//##ModelId=3C60D563032E
	LinearHardening,
	//##ModelId=3C60D563032F
	LinearSoftening,
	//##ModelId=3C60D5630330
	ParabolicSoftening
};

//##ModelId=3C60D563034C
class CDSMaterial  :public IComponent
{
private:
	//##ModelId=3C60D563035D
	CString	m_Name;

public:
	//##ModelId=3C60D563035E
	static CString GetHardeningName(eHardeningType ht);

	//##ModelId=3C60D563036D
	CDSMaterial();
	//##ModelId=3C60D563036E
	virtual ~CDSMaterial();
	
	//##ModelId=3C60D5630370
	virtual void Name(CString name);
	//##ModelId=3C60D5630373
	virtual CString Name() const;

};

#endif // !defined(AFX_DSMATERIAL_H__BDA8C876_AA88_4754_B618_FD1CDA99CBFF__INCLUDED_)
