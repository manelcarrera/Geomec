// CasingMaterial.cpp: implementation of the CCasingMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CasingMaterial.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

/////////////////////////////////////
// Current version of casing material
/////////////////////////////////////
//##ModelId=3CA0119900F9
CVersionInfo CCasingMaterial::CurrentVersion()
{
	return CVersionInfo(1,0);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D56F03C9
CCasingMaterial::CCasingMaterial()
:
m_CasingOuterDiameter		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_CasingInnerDiameter		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_YoungsModulus				(0.2, 0.0, DOUBLE_MAX_,	true, false),
m_PoissonRatio				(0.2, 0.0, 0.5,			false, false),
m_VolumetricSpecificHeat	(1.0, 0.0, DOUBLE_MAX_, true, false),
m_ThermalConductivity		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_ThermalExpansion			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_WeightPerUnitLength		(1.0, 0.0, DOUBLE_MAX_, true, false),
m_YieldStrength				(1.0, 0.0, DOUBLE_MAX_, true, false)

{
	m_bPlasticity=false;
	m_CasingOuterDiameter.Invalidate();
	m_CasingInnerDiameter.Invalidate();
	m_YoungsModulus.Invalidate();
	m_PoissonRatio.Invalidate();
	m_VolumetricSpecificHeat.Invalidate();
	m_ThermalConductivity.Invalidate();
	m_ThermalExpansion.Invalidate();
	m_WeightPerUnitLength.Invalidate();
	m_YieldStrength.Invalidate();
}

//##ModelId=3C60D56F03CA
CCasingMaterial::~CCasingMaterial()
{

}


//##ModelId=3C60D56F03DA
CSmallLengthQuantity	*CCasingMaterial::CasingOuterDiameter()	
{
	return &m_CasingOuterDiameter;
}


//##ModelId=3C60D56F03DB
CSmallLengthQuantity	*CCasingMaterial::CasingInnerDiameter()	
{
	return &m_CasingInnerDiameter;
}	

//##ModelId=3C8D9E110282
double CCasingMaterial::CasingInnerRadius()
{
	return m_CasingInnerDiameter.Value()/2.0;
}

//##ModelId=3C8D9E110215
double CCasingMaterial::CasingOuterRadius()
{
	return m_CasingOuterDiameter.Value()/2.0;
}

//##ModelId=3C60D56F03DC
CGigaSinglePressureQuantity *CCasingMaterial::YoungsModulus()
{
	return &m_YoungsModulus;
}

//##ModelId=3C60D56F03DD
CNoneQuantity *CCasingMaterial::PoissonRatio()
{
	return &m_PoissonRatio;
}

//##ModelId=3C60D56F03DE
CVolumetricSpecificHeatQuantity	*CCasingMaterial::VolumetricSpecificHeat()
{
	return &m_VolumetricSpecificHeat;
}

//##ModelId=3C60D5700000
CThermalConductivityQuantity *CCasingMaterial::ThermalConductivity()
{
	return &m_ThermalConductivity;
}

//##ModelId=3C60D5700001
CThermalExpansionQuantity *CCasingMaterial::ThermalExpansion()
{
	return &m_ThermalExpansion;
}

//##ModelId=3C60D5700002
CWeightPerUnitLengthQuantity	*CCasingMaterial::WeightPerUnitLength()
{
	return &m_WeightPerUnitLength;
}

//##ModelId=3C60D5700003
CSinglePressure	*CCasingMaterial::YieldStrength()
{
	return &m_YieldStrength;
}



//##ModelId=3C60D5700004
void CCasingMaterial::Plasticity(bool plasticity)
{
	m_bPlasticity=plasticity;
}

//##ModelId=3C60D570002F
bool CCasingMaterial::Plasticity() 
{
	return m_bPlasticity;
}

//##ModelId=3C60D5700030
bool CCasingMaterial::CheckComplete() const
{
	if(Name().IsEmpty())
		return false;

	if(m_CasingOuterDiameter.Undefined())
		return false;

	if(m_CasingInnerDiameter.Undefined())
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

	if(m_WeightPerUnitLength.Undefined())
		return false;

	if(m_bPlasticity)
	{
		if(m_YieldStrength.Undefined())
			return false;
	}

	return true;

}
