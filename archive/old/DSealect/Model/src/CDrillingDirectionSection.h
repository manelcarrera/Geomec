// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDRILLINGDIRECTIONSECTION_3B667C4B0046_INCLUDED
#define _INC_CDRILLINGDIRECTIONSECTION_3B667C4B0046_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAE005E
class CDrillingDirectionSection 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65B0071
	CAngleQuantity m_qWellInclination;
	//##ModelId=3CAAA65B0081
	CAngleQuantity m_qAzimuthWithNorth;
	//##ModelId=3CAAA65B0090
  CLengthQuantity m_qDrillingSectionTopTMD;
	CLengthQuantity m_qDrillingSectionBottomTMD;			
	
public:
	//##ModelId=3CA0118C03D8
	virtual bool CheckComplete() const;

	//##ModelId=3C0DDDAE009D
	CDrillingDirectionSection();

	//From: Drilling Operation
	//label: Well Inclination
	//Descr: Well Inclination
	//##ModelId=3C0DDDAE009E
	CAngleQuantity *WellInclination();
	
	//From: Drilling Operation
	//label: Azimuth with North
	//descr:Azimuth with North
	//##ModelId=3C0DDDAE009F
	CAngleQuantity *AzimuthWithNorth(); 
	
	//Top and bottom of the DrillingSection.
	//##ModelId=3C0DDDAE00A0
  CLengthQuantity* DrillingSectionTopTMD();
	CLengthQuantity *DrillingSectionBottomTMD();
	
	
private:
	
};

#endif /* _INC_CDRILLINGDIRECTIONSECTION_3B667C4B0046_INCLUDED */
