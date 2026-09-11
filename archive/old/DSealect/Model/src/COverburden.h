// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_COVERBURDEN_3B6671FA01E5_INCLUDED
#define _INC_COVERBURDEN_3B6671FA01E5_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAA035C
class COverburden 
: public IComponent
{
private:
	//##ModelId=3C8D9DF70272
	CDataModel *m_pDataModel;
	//##ModelId=3CAAA656036F
	CTemperatureGradientQuantity m_qOBGeothermalGradient;
	//##ModelId=3CAAA656038E
	CDensityQuantity m_qOBAverageDensity;
	//##ModelId=3CAAA65603AD
	CTemperatureQuantity m_qSurfaceTemperature;
	//##ModelId=3CAAA65603CD
	CLengthQuantity m_qOverburdenBottomTVD;
	
public:
	//##ModelId=3C2043EB030E
	bool CheckComplete() const;
	//##ModelId=3C0DDDAA03C9
	COverburden(CDataModel *pDataModel);

	//From: Insitu Stresses
	//Label: geothermal gradient
	//descr: geothermal gradient, accessible if Formation = 
	//overburden
	//##ModelId=3C0DDDAA03CA
	CTemperatureGradientQuantity *OBGeothermalGradient();
	const CTemperatureGradientQuantity *OBGeothermalGradient() const;
	
	//From: Insitu Stresses
	//Label: geothermal gradient
	//descr: Average vertical stress gradient in overburden, 
	//accessible if Formation = overburden
	//##ModelId=3C0DDDAA03D9
	CDensityQuantity *OBAverageDensity(); 
	const CDensityQuantity *OBAverageDensity() const;

  // uses density profile if available
  double GetAverageDensity() const;
		
	//From; Insitu Stresses
	//label; Surface Undisturbed Temperature
	//descr: Onshore: Surface Undisturbed Temperature
	//Offshore: Seabed Undisturbed Temperature,
	//accessible if Formation = Overburden
	//##ModelId=3C0DDDAA03DB
	CTemperatureQuantity *SurfaceTemperature();
	const CTemperatureQuantity *SurfaceTemperature() const;
	
	//From: Formation Definition
	//label: Overburden bottom TVD
	//descr: Overburden bottom TVD
	//##ModelId=3C0DDDAA03DC
	CLengthQuantity *OverburdenBottomTVD(); 
	const CLengthQuantity *OverburdenBottomTVD() const; 

  double TopTVD() const;
	
	//##ModelId=3C8D9DF703CA
	double GetVertStressAtBottom() const;
	//##ModelId=3C8D9DF8005F
	double GetGeoThermalTemperatureAtBottom() const;
};

#endif /* _INC_COVERBURDEN_3B6671FA01E5_INCLUDED */
