#if !defined(AFX_GEOMECATTRITEMP_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_)
#define AFX_GEOMECATTRITEMP_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//The attributes template generlize the validation of an 
//object.
//##ModelId=3BC55D650219

template <class T>
  class CGeomecAttriTemp;

#include "AttributeTempl.h"
#include "BoundarySpecific.h"

template<class T>
class CGeomecAttriTemp : public CAttributesTemplate<T>
{
public:

	// Constructor / Destructor  ....
	//##ModelId=3BC55D650238
	CGeomecAttriTemp(unsigned int uIDTemplate, T& source, CWnd* pParent = NULL)
		:CAttributesTemplate<T>(uIDTemplate, source, pParent)
	{
	}

protected:
  friend void BoundarySpecificDDX_Point <T> (CGeomecAttriTemp <T> * dialog,
    CDataExchange* pDX, int nID_X, int nID_Y, int nID_Z, geo::IPoint &point,
    int nID_UNIT1, int nID_UNIT2, int nID_UNIT3);

	void DDX_Point( CDataExchange* pDX,
					int nID_X,
					int nID_Y,
					int nID_Z,
					geo::IPoint &point,
					int nID_UNIT1,
					int nID_UNIT2 = 0,
					int nID_UNIT3 = 0);
};



template<class T>
void CGeomecAttriTemp<T>::DDX_Point(CDataExchange* pDX,
									   int nID_X,
									   int nID_Y,
									   int nID_Z,
									   geo::IPoint &point,
									   int nID_UNIT1,
									   int nID_UNIT2,
									   int nID_UNIT3)
{
	CLengthQuantity q;
	double x, y, z;
	
	if(!pDX->m_bSaveAndValidate)
	{
		// Update the units ....
		GetDlgItem(nID_UNIT1)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
		if(nID_UNIT2 != 0)
			GetDlgItem(nID_UNIT2)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));
		if(nID_UNIT3 != 0)
			GetDlgItem(nID_UNIT3)->SetWindowText(CString(q.UnitName(UnitNode().Unit()).c_str()));

		// Set the members
		x = q.Convert(point.X(), UnitNode().Unit(), CQuantity::SI_UNIT);
		y = q.Convert(point.Y(), UnitNode().Unit(), CQuantity::SI_UNIT);
		
		if(nID_Z != 0)
			z = q.Convert(point.Z(), UnitNode().Unit(), CQuantity::SI_UNIT);
	}

	// Do the data exchange ....
	DDX_Text(pDX, nID_X, x);
	DDX_Text(pDX, nID_Y, y);
	if(nID_Z != 0)
		DDX_Text(pDX, nID_Z, z);

	if(pDX->m_bSaveAndValidate)
	{
		point.X(q.Convert(x, CQuantity::SI_UNIT, UnitNode().Unit()));
		point.Y(q.Convert(y, CQuantity::SI_UNIT, UnitNode().Unit()));
		if(nID_Z != 0)
			point.Z(q.Convert(z, CQuantity::SI_UNIT, UnitNode().Unit()));
	}
}

		
#endif // !defined(AFX_GEOMECATTRITEMP_H__4DB112CC_5590_40A2_83EE_E97FB334E8EB__INCLUDED_)
