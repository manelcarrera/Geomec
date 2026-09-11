// CementMaterial.h: interface for the CCementMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTMATERIAL_H__35CC3C4C_A211_4C7A_977B_C0FDDDB42F52__INCLUDED_)
#define AFX_CEMENTMATERIAL_H__35CC3C4C_A211_4C7A_977B_C0FDDDB42F52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VersionInfo.h"
#include "IComponent.h"
#include "DSMaterial.h"


//##ModelId=3C60D567034C
class CCementMaterial  : public CDSMaterial 
{
public:
	//##ModelId=3CA0118B0222
	static CVersionInfo CurrentVersion();

private:
	//##ModelId=3CAAA65A0023
	CGigaSinglePressureQuantity		m_YoungsModulus;
	//##ModelId=3CAAA65A0042
	CNoneQuantity					m_PoissonRatio;
	//##ModelId=3CAAA65A0052
	CVolumetricSpecificHeatQuantity	m_VolumetricSpecificHeat;
	//##ModelId=3CAAA65A0071
	CThermalConductivityQuantity	m_ThermalConductivity;
	//##ModelId=3CAAA65A0081
	CThermalExpansionQuantity		m_ThermalExpansion;
	//##ModelId=3CAAA65A00A0
	CSinglePressure					m_TensileStrength;
	//##ModelId=3CAAA65A00B0
	CFractureEnergyQuantity			m_TensFractureEnergy;

	//##ModelId=3CAAA65A00CF
	CSinglePressure					m_Cohesion;
	//##ModelId=3CAAA65A00EE
	CAngleQuantity					m_FrictionAngle;
	//##ModelId=3CAAA65A00FE
	eHardeningType					m_HardeningType;

	//##ModelId=3CAAA65A013C
	CFractureEnergyQuantity			m_CompFractureEnergy;
	//##ModelId=3CAAA65A015C
	CGigaSinglePressureQuantity		m_CompHardeningGradient;
	//##ModelId=3CAAA65A016B
	CPercentageQuantity				m_VolumetricShrinkage;
	//##ModelId=3CAAA65A018A
	CVolumetricHeatQuantity					m_HydrationHeat;

public:
	//##ModelId=3C60D5680168
	CCementMaterial();
	//##ModelId=3C60D5680169
	virtual ~CCementMaterial();

	//##ModelId=3C60D568016B
	CGigaSinglePressureQuantity		*YoungsModulus();
	//##ModelId=3C60D568016C
	CNoneQuantity					*PoissonRatio();
	//##ModelId=3C60D5680177
	CVolumetricSpecificHeatQuantity	*VolumetricSpecificHeat();
	//##ModelId=3C60D5680178
	CThermalConductivityQuantity	*ThermalConductivity();
	//##ModelId=3C60D5680179
	CThermalExpansionQuantity		*ThermalExpansion();
	//##ModelId=3C60D568017A
	CSinglePressure					*TensileStrength();
	//##ModelId=3C60D568017B
	CFractureEnergyQuantity			*TensFractureEnergy();

	//##ModelId=3C60D5680187
	CSinglePressure					*Cohesion();
	//##ModelId=3C60D5680188
	CAngleQuantity					*FrictionAngle();
	
	//##ModelId=3C60D5680189
	CFractureEnergyQuantity			*CompFractureEnergy();
	//##ModelId=3C60D568018A
	CGigaSinglePressureQuantity		*CompHardeningGradient();
	//##ModelId=3C60D568018B
	CPercentageQuantity				*VolumetricShrinkage();
	//##ModelId=3C60D568018C
	CVolumetricHeatQuantity					*HydrationHeat();


	//##ModelId=3C60D5680197
	void HardeningType(eHardeningType newval);
	//##ModelId=3C60D56801C6
	eHardeningType HardeningType() const;
	
	// IComponent interface
	//##ModelId=3C60D56801C8
	virtual bool CheckComplete() const;

};


#endif // !defined(AFX_CEMENTMATERIAL_H__35CC3C4C_A211_4C7A_977B_C0FDDDB42F52__INCLUDED_)
