// CasingMaterial.h: interface for the CCasingMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATERIAL_H__65F72ACA_996E_4ACD_9AF1_02E8E5B54BA7__INCLUDED_)
#define AFX_CASINGMATERIAL_H__65F72ACA_996E_4ACD_9AF1_02E8E5B54BA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VersionInfo.h"
#include "IComponent.h"
#include "DSMaterial.h"


//##ModelId=3C60D56F0271
class CCasingMaterial : public CDSMaterial 
{

private:
	//##ModelId=3CAAA66003BD
	CSmallLengthQuantity			m_CasingOuterDiameter;
	
	//##ModelId=3CAAA66003CD
	CSmallLengthQuantity			m_CasingInnerDiameter;
	//##ModelId=3CAAA6610004
	CGigaSinglePressureQuantity		m_YoungsModulus;
	//##ModelId=3CAAA6610013
	CNoneQuantity					m_PoissonRatio;
	//##ModelId=3CAAA6610033
	CVolumetricSpecificHeatQuantity	m_VolumetricSpecificHeat;
	//##ModelId=3CAAA6610042
	CThermalConductivityQuantity	m_ThermalConductivity;
	//##ModelId=3CAAA6610062
	CThermalExpansionQuantity		m_ThermalExpansion;
	//##ModelId=3CAAA6610071
	CWeightPerUnitLengthQuantity	m_WeightPerUnitLength;
	//##ModelId=3CAAA6610090
	CSinglePressure					m_YieldStrength;
	
	//##ModelId=3C60D56F038B
	bool m_bPlasticity;
	

public:
	//##ModelId=3CA0119900F9
	static CVersionInfo CurrentVersion();

	//##ModelId=3C60D56F03C9
	CCasingMaterial();
	//##ModelId=3C60D56F03CA
	virtual ~CCasingMaterial();

	//##ModelId=3C60D56F03DA
	CSmallLengthQuantity			*CasingOuterDiameter();
	//##ModelId=3C8D9E110215
	double CasingOuterRadius();

	//##ModelId=3C60D56F03DB
	CSmallLengthQuantity			*CasingInnerDiameter();
	//##ModelId=3C8D9E110282
	double CasingInnerRadius();	

	//##ModelId=3C60D56F03DC
	CGigaSinglePressureQuantity		*YoungsModulus();
	//##ModelId=3C60D56F03DD
	CNoneQuantity					*PoissonRatio();
	//##ModelId=3C60D56F03DE
	CVolumetricSpecificHeatQuantity	*VolumetricSpecificHeat();
	//##ModelId=3C60D5700000
	CThermalConductivityQuantity	*ThermalConductivity();
	//##ModelId=3C60D5700001
	CThermalExpansionQuantity		*ThermalExpansion();
	//##ModelId=3C60D5700002
	CWeightPerUnitLengthQuantity	*WeightPerUnitLength();
	//##ModelId=3C60D5700003
	CSinglePressure					*YieldStrength();


	//##ModelId=3C60D5700004
	void Plasticity(bool plasticity);
	//##ModelId=3C60D570002F
	bool Plasticity();

	//IComponent interface
	//##ModelId=3C60D5700030
	virtual bool CheckComplete() const;
};

#endif // !defined(AFX_CASINGMATERIAL_H__65F72ACA_996E_4ACD_9AF1_02E8E5B54BA7__INCLUDED_)
