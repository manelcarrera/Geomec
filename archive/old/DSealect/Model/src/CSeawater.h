// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSEAWATER_3B68F73E035C_INCLUDED
#define _INC_CSEAWATER_3B68F73E035C_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAA0197
class CSeawater 
: public IComponent
{
private:
		
	//##ModelId=3CAAA656017B
	CLengthQuantity m_qWaterDepth;
	//##ModelId=3CAAA656018A
	CDensityQuantity m_qWaterdensity;

public:
	//##ModelId=3CA0118700AB
	virtual bool CheckComplete() const;
	//##ModelId=3C0DDDAA01E5
	CSeawater();

	//From: Formation Definition
	//Label: Sea bed TVD
	//Descr: True vertical depth of sea bed measured from 
	//derrick floor.
	//##ModelId=3C0DDDAA01E6
	CLengthQuantity *WaterDepth();
	const CLengthQuantity *WaterDepth() const;

  CLengthQuantity SeaBedTVD() const;
	
	//From: 
	//Label: 
	//Descr:
	//##ModelId=3C0DDDAA01E7
	CDensityQuantity *Waterdensity();
	
};

#endif /* _INC_CSEAWATER_3B68F73E035C_INCLUDED */
