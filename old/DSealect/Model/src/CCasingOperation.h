// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCASINGOPERATION_3B667C4B00A3_INCLUDED
#define _INC_CCASINGOPERATION_3B667C4B00A3_INCLUDED



#include "IComponent.h"



class MaterialData;

//##ModelId=3C0DDDAF003F
class CCasingOperation 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65F0265
	CLengthQuantity m_qGuideShoeTMD;
	//##ModelId=3CAAA65F0284
	CPercentageQuantity m_qPercentageEccentricity;

	//##ModelId=3CAAA65F0294
	CForceQuantity m_qHookLoad;

	//##ModelId=3CAAA65F02B3
	CStressGradientQuantity m_qDisplacementFluidStressGradient;
	//##ModelId=3CAAA65F02C3
	CCompressibilityQuantity m_qDispFluidCompressibility;
	//##ModelId=3CAAA65F02E2
	CLengthQuantity m_qWorkStringPackersTMD;
	//##ModelId=3CAAA65F0301
	CStressGradientQuantity m_qNewFluidStressGradient;
	//##ModelId=3CAAA65F0311
	CSinglePressure m_qNewFluidOverpressure;
	//##ModelId=3CAAA65F0330
	CCompressibilityQuantity m_qNewFluidCompressibility;
	
	//##ModelId=3C0DDDAF0100
	bool m_bWorkStringUsed;
	//##ModelId=3C0DDDAF010A
	bool m_bNewFluidInCasWSAnnulus;
	//##ModelId=3C0DDDAF011A
	bool m_bAdvancedAxialCasingStresses;

	//##ModelId=3C60D56F003F
	CString m_strCasingMaterialName;

public:
	//##ModelId=3CA01196033C
	virtual bool CheckComplete() const;

	//##ModelId=3CA0119603C9
	bool CheckCompleteCasingStresses() const;

	//##ModelId=3C0DDDAF0129
	CCasingOperation();

	//##ModelId=3C0DDDAF012A
	CString m_sCasingType;

	//From: Casing Operation
	//Label: Guide shoe depth along hole
	//Descr: True measure depth of guide shoe
	//##ModelId=3C0DDDAF0139
	CLengthQuantity* GuideShoeTMD();
	
	//From: CasingOperation
	//Label: Percentage Eccentricity
	//Descr: Percentage of Casing Eccentricity
	//##ModelId=3C0DDDAF013A
	CPercentageQuantity* PercentageEccentricity();
	

	//From: Casing stresses
	//Label: 'radio button'
	//Descr: True if casing initial axial stresses are 
	//assessed using an extra input
	//##ModelId=3C0DDDAF0149
	bool AdvancedAxialCasingStresses() const;
	//##ModelId=3C0DDDAF014B
	void AdvancedAxialCasingStresses(bool bAdvancedAxialCasingStresses);
	
	//From: Casing stresses
	//Label: hook load
	//Descr: vertical tensile stress in casing measured at 
	//the hook during WOC only if 
	//bAdvancedAxialCasingStresses = true
	//##ModelId=3C0DDDAF0158
	CForceQuantity* HookLoad();
	
	//From: casing Stresses
	//Label: 'radio button'
	//Descr: false if no workstring is used
	//##ModelId=3C0DDDAF0159
	bool WorkStringUsed() const;
	//##ModelId=3C0DDDAF015B
	void WorkStringUsed(bool bWorkStringUsed);
	
	//From: Casing Stresses
	//Label: Displacement fluid stress gradient
	//Descr: Density of displacement fluid at surface
	//##ModelId=3C0DDDAF0168
	CStressGradientQuantity* DisplacementFluidStressGradient();
	
	//From: Casing Stresses
	//Label: Displacement fluid compressability
	//Descr: Displacement fluid compressability
	//##ModelId=3C0DDDAF0169
	CCompressibilityQuantity* DispFluidCompressibility();
	

	//From: casing stresses
	//Label: Work string packers TMD
	//Descr: TMD of work string packers
	//Accessible if bWorkStringUsed = true
	//##ModelId=3C0DDDAF016A
	CLengthQuantity  *WorkStringPackersTMD();
	
	//From: casing stresses
	//Label: New Fluid Stress Gradient
	//Descr: New Fluid Stress Gradient
	//Accessible if bWorkStringUsed = true
	//##ModelId=3C0DDDAF016B
	CStressGradientQuantity *NewFluidStressGradient();
	
	//From: casing stresses
	//Label: New Fluid overpressure
	//Descr: New Fluid overpressure
	//Accessible if bWorkStringUsed = true
	//##ModelId=3C0DDDAF016C
	CSinglePressure *NewFluidOverpressure();
	
	//From: casing stresses
	//Label: New Fluid Compressibility
	//Descr: New Fluid Compressibility
	//Accessible if bWorkStringUsed = true
	//##ModelId=3C0DDDAF0178
	CCompressibilityQuantity *NewFluidCompressibility();
	
	
	//From: Casing Stresses
	//Label: option button: New Fluid
	//Descr: TRUE if optNewFluid is set, FALSE if optMud is 
	//set
	//##ModelId=3C0DDDAF017A
	bool NewFluidInCasWSAnnulus() const;
	//##ModelId=3C0DDDAF017C
	void NewFluidInCasWSAnnulus(bool bNewFluidInCasWSAnnulus);

	//##ModelId=3C60D56F00DB
	CString CasingMaterialName() const
	{
		return m_strCasingMaterialName;
	}

	//##ModelId=3C60D56F011B
	void CasingMaterialName(CString strCasingMaterialName)
	{
		m_strCasingMaterialName=strCasingMaterialName;
	}

	
};

#endif /* _INC_CCASINGOPERATION_3B667C4B00A3_INCLUDED */
