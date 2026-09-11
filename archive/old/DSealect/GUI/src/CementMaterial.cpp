// CementMaterial.cpp: implementation of the CCementMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CementMaterial.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

/////////////////////////////////////
// Current version of Cement material
/////////////////////////////////////
//##ModelId=3CA0118B0222
CVersionInfo CCementMaterial::CurrentVersion()
{
	return CVersionInfo(1,0);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D5680168
CCementMaterial::CCementMaterial()
:
m_YoungsModulus				(1.0, 0.0, DOUBLE_MAX_, false, false),
m_PoissonRatio				(0.2, 0.0, 0.5,			true, false),
m_VolumetricSpecificHeat	(1.0, 0.0, DOUBLE_MAX_, true, false),
m_ThermalConductivity		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_ThermalExpansion			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_TensileStrength			(1.0, 0.0, DOUBLE_MAX_, true, false),
m_TensFractureEnergy		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_Cohesion					(1.0, 0.0, DOUBLE_MAX_, true, false),
m_FrictionAngle				(1.0, 0.0, 90.0,		true, true),
m_CompFractureEnergy		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_CompHardeningGradient		(1.0, 0.0, DOUBLE_MAX_, true, false),
m_VolumetricShrinkage		(0.01, -1.0, 1.0,	false, false),
m_HydrationHeat				(1.0, 0.0, DOUBLE_MAX_, true, false)
{
	m_HardeningType=LinearHardening;
	m_YoungsModulus.Invalidate();
	m_PoissonRatio.Invalidate();
	m_VolumetricSpecificHeat.Invalidate();
	m_ThermalConductivity.Invalidate();
	m_ThermalExpansion.Invalidate();
	m_TensileStrength.Invalidate();
	m_TensFractureEnergy.Invalidate();
	m_Cohesion.Invalidate();
	m_FrictionAngle.Invalidate();
	m_CompFractureEnergy.Invalidate();
	m_CompHardeningGradient.Invalidate();
	m_VolumetricShrinkage.Invalidate();
	m_HydrationHeat.Invalidate();
}

//##ModelId=3C60D5680169
CCementMaterial::~CCementMaterial()
{

}

//##ModelId=3C60D568016B
CGigaSinglePressureQuantity *CCementMaterial::YoungsModulus()
{
	return &m_YoungsModulus;
}

//##ModelId=3C60D568016C
CNoneQuantity *CCementMaterial::PoissonRatio()
{
	return &m_PoissonRatio;
}

//##ModelId=3C60D5680177
CVolumetricSpecificHeatQuantity	*CCementMaterial::VolumetricSpecificHeat()
{
	return &m_VolumetricSpecificHeat;
}

//##ModelId=3C60D5680178
CThermalConductivityQuantity *CCementMaterial::ThermalConductivity()
{
	return &m_ThermalConductivity;
}

//##ModelId=3C60D5680179
CThermalExpansionQuantity *CCementMaterial::ThermalExpansion()
{
	return &m_ThermalExpansion;
}

//##ModelId=3C60D568017A
CSinglePressure	*CCementMaterial::TensileStrength()
{
	return &m_TensileStrength;
}

//##ModelId=3C60D568017B
CFractureEnergyQuantity	*CCementMaterial::TensFractureEnergy()
{
	return &m_TensFractureEnergy;
}

//##ModelId=3C60D5680187
CSinglePressure	*CCementMaterial::Cohesion()
{
	return &m_Cohesion;
}

//##ModelId=3C60D5680188
CAngleQuantity	*CCementMaterial::FrictionAngle()
{
	return &m_FrictionAngle;
}

//##ModelId=3C60D5680197
void CCementMaterial::HardeningType(eHardeningType newval)
{
	m_HardeningType=newval;
}

//##ModelId=3C60D56801C6
eHardeningType CCementMaterial::HardeningType() const
{
	return m_HardeningType;
}



//##ModelId=3C60D5680189
CFractureEnergyQuantity	*CCementMaterial::CompFractureEnergy()
{
	return &m_CompFractureEnergy;
}


//##ModelId=3C60D568018A
CGigaSinglePressureQuantity	*CCementMaterial::CompHardeningGradient()
{
	return &m_CompHardeningGradient;
}

//##ModelId=3C60D568018B
CPercentageQuantity	*CCementMaterial::VolumetricShrinkage()
{
	return &m_VolumetricShrinkage;
}

//##ModelId=3C60D568018C
CVolumetricHeatQuantity	*CCementMaterial::HydrationHeat()
{
	return &m_HydrationHeat;
}

//##ModelId=3C60D56801C8
bool CCementMaterial::CheckComplete() const
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

	if(m_TensileStrength.Undefined())
		return false;

	if(m_TensFractureEnergy.Undefined())
		return false;
	
	if(m_Cohesion.Undefined())
		return false;

	if(m_FrictionAngle.Undefined())
		return false;

	if(m_HardeningType==LinearHardening)
	{
		if(m_CompHardeningGradient.Undefined())
			return false;
	}
	else if(m_HardeningType==None)
	{
	}
	else
	{
		if(m_CompFractureEnergy.Undefined())
			return false;
	}

	if(m_VolumetricShrinkage.Undefined())
		return false;

	if(m_HydrationHeat.Undefined())
		return false;
	

	return true;
}
