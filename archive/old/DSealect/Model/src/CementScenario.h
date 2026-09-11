// CementScenario.h: interface for the CCementScenarios class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTSCENARIO_H__34BAC094_2E37_4F4C_B1F2_F5764FF6EC44__INCLUDED_)
#define AFX_CEMENTSCENARIO_H__34BAC094_2E37_4F4C_B1F2_F5764FF6EC44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//##ModelId=3C2043F2002F
class CCementScenario  
{
private:
	//##ModelId=3C2043F2003F
	CString m_TailMaterialName;
	//##ModelId=3C2043F20040
	CString m_HeadMaterialName;

public:
	//##ModelId=3C2043F20041
	CString TailMaterialName();
	//##ModelId=3C2043F20042
	void    TailMaterialName(CString newname);

	//##ModelId=3C2043F20050
	CString HeadMaterialName();
	//##ModelId=3C2043F20051
	void    HeadMaterialName(CString newname);

	//##ModelId=3C2043F20053
	CCementScenario();
	//##ModelId=3C2043F2005E
	virtual ~CCementScenario();

};

#endif // !defined(AFX_CEMENTSCENARIO_H__34BAC094_2E37_4F4C_B1F2_F5764FF6EC44__INCLUDED_)
