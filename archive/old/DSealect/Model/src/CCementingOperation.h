// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCEMENTINGOPERATION_3B667C4B00BC_INCLUDED
#define _INC_CCEMENTINGOPERATION_3B667C4B00BC_INCLUDED



#include "IComponent.h"

//##ModelId=3C0DDDAE0281
class CCementingOperation 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65F00CF
	CLengthQuantity m_qTopOfCementTMD;
	//##ModelId=3CAAA65F00EE
	CSinglePressure m_qPressureFromFluidsAbove;
	//##ModelId=3CAAA65F010D
	CLengthQuantity m_qTopOfTailCement;
	//##ModelId=3CAAA65F011D
	CStressGradientQuantity m_qLeadCementStressGradient;
	//##ModelId=3CAAA65F013C
	CStressGradientQuantity m_qTailCementStressGradient;

	//##ModelId=3C0DDDAE030E
	bool m_bLeadTailCements;

	
public:
	//##ModelId=3C3040AD0201
	bool CheckComplete() const;
	//##ModelId=3C0DDDAE031D
	CCementingOperation();

	//From: Cementing Operation
	//Label: Top of cement TMD (TVD is calculated 
	//automatically)
	//Descr: TMD at top of cement in annulus
	//##ModelId=3C0DDDAE031E
	CLengthQuantity *TopOfCementTMD();
	
	//From: Cementing Operation
	//Label: Pressure from fluids above
	//Descr: Pressure on top of cements from fluids above
	//##ModelId=3C0DDDAE031F
	CSinglePressure *PressureFromFluidsAbove();

	//From: Cementing Operation
	//Label: 'radio Button'
	//Descr: True id lead/tail cement interface lies in model
	//##ModelId=3C0DDDAE032D
	bool LeadTailCements() const;
	//##ModelId=3C0DDDAE032F
	void LeadTailCements(bool bLeadTailCements);
	
	//From: Cementing Operation
	//Label: top of tail cement TVD
	//Descr: TMD of lead/tail cement interface
	//##ModelId=3C0DDDAE033D
	CLengthQuantity* TopOfTailCement();
	
	//From: Cementing Operation
	//Label: (Lead) Cement stress gradient
	//Descr: (Lead) Cement stress gradient
	//##ModelId=3C0DDDAE033E
	CStressGradientQuantity *LeadCementStressGradient();
	
	//From: Cementing Operation
	//Label: Tail cement stress gradient
	//Descr: Tail cement bottom hole average stress gradient 
	//(in case bLeadTailCement = true)
	//##ModelId=3C0DDDAE033F
	CStressGradientQuantity *TailCementStressGradient();
	
};

#endif /* _INC_CCEMENTINGOPERATION_3B667C4B00BC_INCLUDED */
