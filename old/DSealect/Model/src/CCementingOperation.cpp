// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CCementingOperation.h"

//##ModelId=3C0DDDAE031D
CCementingOperation::CCementingOperation()
:
m_qTopOfCementTMD			(1.0, 0.0, DOUBLE_MAX_, true, false),
m_qPressureFromFluidsAbove	(1.0, 0.0, DOUBLE_MAX_, true, false),
m_qTopOfTailCement			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qLeadCementStressGradient	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qTailCementStressGradient	(1.0, 0.0, DOUBLE_MAX_, false, false),

m_bLeadTailCements(true)
{
	m_qTopOfCementTMD.Invalidate();
	m_qPressureFromFluidsAbove.Invalidate();
	m_qTopOfTailCement.Invalidate();
	m_qLeadCementStressGradient.Invalidate();
	m_qTailCementStressGradient.Invalidate();
	
}


//From: Cementing Operation
//Label: Top of cement TMD (TVD is calculated 
//automatically)
//Descr: TMD at top of cement in annulus
//##ModelId=3C0DDDAE031E
CLengthQuantity *CCementingOperation::TopOfCementTMD()
{
	return &m_qTopOfCementTMD;
}



//From: Cementing Operation
//Label: Pressure from fluids above
//Descr: Pressure on top of cements from fluids above
//##ModelId=3C0DDDAE031F
CSinglePressure *CCementingOperation::PressureFromFluidsAbove()
{
	return &m_qPressureFromFluidsAbove;
}


//From: Cementing Operation
//Label: 'radio Button'
//Descr: True id lead/tail cement interface lies in model
//##ModelId=3C0DDDAE032D
bool CCementingOperation::LeadTailCements() const
{
	return m_bLeadTailCements;	
}

//##ModelId=3C0DDDAE032F
void CCementingOperation::LeadTailCements(bool bLeadTailCements)
{
	m_bLeadTailCements= bLeadTailCements;
}

//From: Cementing Operation
//Label: top of tail cement TVD
//Descr: TMD of lead/tail cement interface
//##ModelId=3C0DDDAE033D
CLengthQuantity *CCementingOperation::TopOfTailCement()
{
	return &m_qTopOfTailCement;
}


//From: Cementing Operation
//Label: (Lead) Cement stress gradient
//Descr: (Lead) Cement stress gradient
//##ModelId=3C0DDDAE033E
CStressGradientQuantity *CCementingOperation::LeadCementStressGradient()
{
	return &m_qLeadCementStressGradient;
}


//From: Cementing Operation
//Label: Tail cement stress gradient
//Descr: Tail cement bottom hole average stress gradient 
//(in case bLeadTailCement = true)
//##ModelId=3C0DDDAE033F
CStressGradientQuantity *CCementingOperation::TailCementStressGradient()
{
	return &m_qTailCementStressGradient;
}

//##ModelId=3C3040AD0201
bool CCementingOperation::CheckComplete() const
{
	if (m_qLeadCementStressGradient.Undefined())
	{
		return false;
	}
	if (m_qPressureFromFluidsAbove.Undefined())
	{
		return false;
	}
	if (m_qTopOfCementTMD.Undefined())
	{
		return false;
	}
	
	if(m_bLeadTailCements)
	{
		if (m_qTailCementStressGradient.Undefined())
		{
			return false;
		}
		if (m_qTopOfTailCement.Undefined())
		{
			return false;
		}
	}
	return true;
	
}
