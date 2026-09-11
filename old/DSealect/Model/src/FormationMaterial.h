// FormationMaterial.h: interface for the CFormationMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONMATERIAL_H__44C1A080_5740_42B3_BC21_72C195D752BD__INCLUDED_)
#define AFX_FORMATIONMATERIAL_H__44C1A080_5740_42B3_BC21_72C195D752BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VersionInfo.h"
#include "IComponent.h"
#include "DSMaterial.h"

//##ModelId=3C60D56300AC
class CFormationMaterial : public CDSMaterial 
{
public:
	//##ModelId=3CA011860157
	static CVersionInfo CurrentVersion();

private:
	//##ModelId=3CAAA65502B3
	CGigaSinglePressureQuantity		m_YoungsModulus;
	//##ModelId=3CAAA65502D3
	CNoneQuantity					m_PoissonRatio;
	//##ModelId=3CAAA65502E2
	CVolumetricSpecificHeatQuantity	m_VolumetricSpecificHeat;
	//##ModelId=3CAAA6550300
	CThermalConductivityQuantity	m_ThermalConductivity;
	//##ModelId=3CAAA6550311
	CThermalExpansionQuantity		m_ThermalExpansion;

	//##ModelId=3CAAA6550321
	CSinglePressure					m_Cohesion;
	//##ModelId=3CAAA6550340
	CAngleQuantity					m_FrictionAngle;
	//##ModelId=3CAAA655035F
	eHardeningType					m_HardeningType;

	//##ModelId=3CAAA655037E
	CFractureEnergyQuantity			m_FractureEnergy;
	//##ModelId=3CAAA655039E
	CGigaSinglePressureQuantity		m_HardeningGradient;

  CDensityQuantity m_Density;
	
	//##ModelId=3C60D5630252
	BOOL							m_Plasticity;
public:
	//##ModelId=3C60D5630281
	CFormationMaterial();
	//##ModelId=3C60D5630282
	virtual ~CFormationMaterial();

	//##ModelId=3C60D5630291
	CGigaSinglePressureQuantity		*YoungsModulus();
	//##ModelId=3C60D5630292
	CNoneQuantity					*PoissonRatio();
	//##ModelId=3C60D5630293
	CVolumetricSpecificHeatQuantity	*VolumetricSpecificHeat();
	//##ModelId=3C60D5630294
	CThermalConductivityQuantity	*ThermalConductivity();
	//##ModelId=3C60D5630295
	CThermalExpansionQuantity		*ThermalExpansion();

	//##ModelId=3C60D5630296
	CSinglePressure					*Cohesion();
	//##ModelId=3C60D56302A0
	CAngleQuantity					*FrictionAngle();

	//##ModelId=3C60D56302A1
	CFractureEnergyQuantity			*FractureEnergy();
	//##ModelId=3C60D56302A2
	CGigaSinglePressureQuantity		*HardeningGradient();

  CDensityQuantity* Density();
  const CDensityQuantity* Density() const;

	//##ModelId=3C60D56302A3
	void HardeningType(eHardeningType newval);
	//##ModelId=3C60D56302CF
	eHardeningType HardeningType() const;

	//##ModelId=3C60D56302D1
	BOOL	Plasticity() const;
	//##ModelId=3C60D56302D3
	void	Plasticity(BOOL newval);

	//IComponent interface
	//##ModelId=3C60D56302FE
	virtual bool CheckComplete() const;
};

#endif // !defined(AFX_FORMATIONMATERIAL_H__44C1A080_5740_42B3_BC21_72C195D752BD__INCLUDED_)
