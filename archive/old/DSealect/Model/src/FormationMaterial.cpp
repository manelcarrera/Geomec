// FormationMaterial.cpp: implementation of the CFormationMaterial class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "FormationMaterial.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

/////////////////////////////////////
// Current version of formation material
/////////////////////////////////////
//##ModelId=3CA011860157
CVersionInfo CFormationMaterial::CurrentVersion()
{
	return CVersionInfo(1,0);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D5630281
CFormationMaterial::CFormationMaterial()
:
m_YoungsModulus				(1.0, 0.0, DOUBLE_MAX_, false, false),
m_PoissonRatio				(0.2, 0.0, 0.5,			true, false),
m_VolumetricSpecificHeat	(1.0, 0.0, DOUBLE_MAX_, true, false),
m_ThermalConductivity		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_ThermalExpansion			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_Cohesion					(1.0, 0.0, DOUBLE_MAX_, true, false),
m_FrictionAngle				(1.0, 0.0, 90.0,		true, false),
m_FractureEnergy			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_HardeningGradient			(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_HardeningType=LinearHardening;
	m_Plasticity=FALSE;

	m_YoungsModulus.Invalidate();
	m_PoissonRatio.Invalidate();
	m_VolumetricSpecificHeat.Invalidate();
	m_ThermalConductivity.Invalidate();
	m_ThermalExpansion.Invalidate();
	m_Cohesion.Invalidate();
	m_FrictionAngle.Invalidate();
	m_FractureEnergy.Invalidate();
	m_HardeningGradient.Invalidate();
}

//##ModelId=3C60D5630282
CFormationMaterial::~CFormationMaterial()
{

}

//##ModelId=3C60D5630291
CGigaSinglePressureQuantity *CFormationMaterial::YoungsModulus()
{
	return &m_YoungsModulus;
}

//##ModelId=3C60D5630292
CNoneQuantity *CFormationMaterial::PoissonRatio()
{
	return &m_PoissonRatio;
}

//##ModelId=3C60D5630293
CVolumetricSpecificHeatQuantity	*CFormationMaterial::VolumetricSpecificHeat()
{
	return &m_VolumetricSpecificHeat;
}

//##ModelId=3C60D5630294
CThermalConductivityQuantity *CFormationMaterial::ThermalConductivity()
{
	return &m_ThermalConductivity;
}

//##ModelId=3C60D5630295
CThermalExpansionQuantity *CFormationMaterial::ThermalExpansion()
{
	return &m_ThermalExpansion;
}



//##ModelId=3C60D5630296
CSinglePressure	*CFormationMaterial::Cohesion()
{
	return &m_Cohesion;
}

//##ModelId=3C60D56302A0
CAngleQuantity	*CFormationMaterial::FrictionAngle()
{
	return &m_FrictionAngle;
}

//##ModelId=3C60D56302A3
void CFormationMaterial::HardeningType(eHardeningType newval)
{
	m_HardeningType=newval;
}

//##ModelId=3C60D56302CF
eHardeningType CFormationMaterial::HardeningType() const
{
	return m_HardeningType;
}

//##ModelId=3C60D56302D1
BOOL CFormationMaterial::Plasticity() const
{
	return m_Plasticity;
}

//##ModelId=3C60D56302D3
void CFormationMaterial::Plasticity(BOOL newval)
{
	m_Plasticity=newval;
}


//##ModelId=3C60D56302A1
CFractureEnergyQuantity	*CFormationMaterial::FractureEnergy()
{
	return &m_FractureEnergy;
}


//##ModelId=3C60D56302A2
CGigaSinglePressureQuantity	*CFormationMaterial::HardeningGradient()
{
	return &m_HardeningGradient;
}

CDensityQuantity* CFormationMaterial::Density()
{
  return &m_Density;
}

const CDensityQuantity* CFormationMaterial::Density() const
{
  return &m_Density;
}

//##ModelId=3C60D56302FE
bool CFormationMaterial::CheckComplete() const
{

	if(Name().IsEmpty())
		return false;

	if(m_YoungsModulus.Undefined())
		return false;

	if(m_PoissonRatio.Undefined())
		return false;

	if(m_VolumetricSpecificHeat.Undefined())
		return false;

	if(m_ThermalConductivity.Undefined())
		return false;

	if(m_ThermalExpansion.Undefined())
		return false;

	if(m_YoungsModulus.Undefined())
		return false;


	if(m_Plasticity)
	{
		if(m_Cohesion.Undefined())
			return false;

		if(m_FrictionAngle.Undefined())
			return false;

		if(m_HardeningType==LinearHardening)
		{
			if(m_HardeningGradient.Undefined())
				return false;
		}
		else if(m_HardeningType==None)
		{
		}
		else
		{
			if(m_FractureEnergy.Undefined())
				return false;
		}

	}

	return true;
}

	
