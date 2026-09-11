// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CCasingOperation.h"

//##ModelId=3C0DDDAF0129
CCasingOperation::CCasingOperation()
:
m_qGuideShoeTMD						(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qPercentageEccentricity			(0.0, 0.0, 0.8, true, true),
m_qHookLoad							(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
m_qDisplacementFluidStressGradient	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qDispFluidCompressibility			(1.0, 0.0, DOUBLE_MAX_, true, false),
m_qWorkStringPackersTMD				(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qNewFluidStressGradient			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qNewFluidOverpressure				(1.0, 0.0, DOUBLE_MAX_, true, false),
m_qNewFluidCompressibility			(1.0, 0.0, DOUBLE_MAX_, true, false),

m_bWorkStringUsed				(false),
m_bNewFluidInCasWSAnnulus		(false),
m_bAdvancedAxialCasingStresses	(false)
{
	m_qGuideShoeTMD.Invalidate();
	m_qPercentageEccentricity.Invalidate();
	m_qHookLoad.Invalidate();							
	m_qDisplacementFluidStressGradient.Invalidate();	
	m_qDispFluidCompressibility.Invalidate();			
	m_qWorkStringPackersTMD.Invalidate();				
	m_qNewFluidStressGradient.Invalidate();			
	m_qNewFluidOverpressure.Invalidate();				
	m_qNewFluidCompressibility.Invalidate();			
	

}

//From: Casing Operation
//Label: Guide shoe depth along hole
//Descr: True measure depth of guide shoe
//##ModelId=3C0DDDAF0139
CLengthQuantity *CCasingOperation::GuideShoeTMD()
{
	return &m_qGuideShoeTMD;
}

//From: CasingOperation
//Label: Percentage Eccentricity
//Descr: Percentage of Casing Eccentricity
//##ModelId=3C0DDDAF013A
CPercentageQuantity *CCasingOperation::PercentageEccentricity()
{
	return &m_qPercentageEccentricity;
}


//From: Casing stresses
//Label: 'radio button'
//Descr: True if casing initial axial stresses are 
//assessed using an extra input
//##ModelId=3C0DDDAF0149
bool CCasingOperation::AdvancedAxialCasingStresses() const
{
	return m_bAdvancedAxialCasingStresses;	
}

//##ModelId=3C0DDDAF014B
void CCasingOperation::AdvancedAxialCasingStresses(bool bAdvancedAxialCasingStresses)
{
	m_bAdvancedAxialCasingStresses= bAdvancedAxialCasingStresses;
}

//From: Casing stresses
//Label: hook load
//Descr: vertical tensile stress in casing measured at 
//the hook during WOC only if 
//bAdvancedAxialCasingStresses = true
//##ModelId=3C0DDDAF0158
CForceQuantity *CCasingOperation::HookLoad()
{
	return &m_qHookLoad;
}


//From: casing Stresses
//Label: 'radio button'
//Descr: false if no workstring is used
//##ModelId=3C0DDDAF0159
bool CCasingOperation::WorkStringUsed() const
{
	return m_bWorkStringUsed;	
}

//##ModelId=3C0DDDAF015B
void CCasingOperation::WorkStringUsed(bool bWorkStringUsed)
{
	m_bWorkStringUsed= bWorkStringUsed;
}


//From: Casing Stresses
//Label: Displacement fluid stress gradient
//Descr: Density of displacement fluid at surface
//##ModelId=3C0DDDAF0168
CStressGradientQuantity *CCasingOperation::DisplacementFluidStressGradient()
{
	return &m_qDisplacementFluidStressGradient;
}


//From: Casing Stresses
//Label: Displacement fluid compressability
//Descr: Displacement fluid compressability
//##ModelId=3C0DDDAF0169
CCompressibilityQuantity *CCasingOperation::DispFluidCompressibility()
{
	return &m_qDispFluidCompressibility;
}

//From: casing stresses
//Label: Work string packers TMD
//Descr: TMD of work string packers
//Accessible if bWorkStringUsed = true
//##ModelId=3C0DDDAF016A
CLengthQuantity  *CCasingOperation::WorkStringPackersTMD()
{
	return &m_qWorkStringPackersTMD;
}


//From: casing stresses
//Label: New Fluid Stress Gradient
//Descr: New Fluid Stress Gradient
//Accessible if bWorkStringUsed = true
//##ModelId=3C0DDDAF016B
CStressGradientQuantity *CCasingOperation::NewFluidStressGradient()
{
	return &m_qNewFluidStressGradient;
}


//From: casing stresses
//Label: New Fluid overpressure
//Descr: New Fluid overpressure
//Accessible if bWorkStringUsed = true
//##ModelId=3C0DDDAF016C
CSinglePressure *CCasingOperation::NewFluidOverpressure()
{
	return &m_qNewFluidOverpressure;
}


//From: casing stresses
//Label: New Fluid Compressibility
//Descr: New Fluid Compressibility
//Accessible if bWorkStringUsed = true
//##ModelId=3C0DDDAF0178
CCompressibilityQuantity *CCasingOperation::NewFluidCompressibility()
{
	return &m_qNewFluidCompressibility;
}


//From: Casing Stresses
//Label: option button: New Fluid
//Descr: TRUE if optNewFluid is set, FALSE if optMud is 
//set
//##ModelId=3C0DDDAF017A
bool CCasingOperation::NewFluidInCasWSAnnulus() const
{
	return m_bNewFluidInCasWSAnnulus;	
}

//##ModelId=3C0DDDAF017C
void CCasingOperation::NewFluidInCasWSAnnulus(bool bNewFluidInCasWSAnnulus)
{
	m_bNewFluidInCasWSAnnulus= bNewFluidInCasWSAnnulus;
}

//##ModelId=3CA0119603C9
bool CCasingOperation::CheckCompleteCasingStresses() const
{
	if(AdvancedAxialCasingStresses())
	{
		if(m_qHookLoad.Undefined())
			return false;
	}

	if(m_qDispFluidCompressibility.Undefined())
		return false;
	if(m_qDisplacementFluidStressGradient.Undefined())
		return false;
	
	
	if(WorkStringUsed())
	{	
		if(m_qWorkStringPackersTMD.Undefined())
			return false;
	
		if(NewFluidInCasWSAnnulus())
		{
			if(m_qNewFluidCompressibility.Undefined())
				return false;
			if(m_qNewFluidOverpressure.Undefined())
				return false;
			if(m_qNewFluidStressGradient.Undefined())
				return false;
		}
	}
	return true;

}

//##ModelId=3CA01196033C
bool CCasingOperation::CheckComplete() const
{
	
	if(!CheckCompleteCasingStresses())
		return false;

	if(m_qGuideShoeTMD.Undefined())
		return false;
	
	if(m_qPercentageEccentricity.Undefined())
		return false;
	
	
	return true;
}
