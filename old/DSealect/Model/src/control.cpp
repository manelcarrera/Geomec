// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "control.h"


//##ModelId=3C0DDDAB0253
CControl::CControl()
:
m_bBalancedCompletion		(true),
m_bUnderBalancedCompletion	(false),
m_bOverBalancedCompletion	(false),
m_bDefaultMeshRefinement	(true),
m_bConversion				(false),
m_bFracturing				(false),
m_bEvacuation				(false),
m_bShutIn					(false)
{
	m_eBalancedType				= Balanced;
	WellType(OilProducer);
		
	m_iRockRefinement			= 2;
	m_iCasingRefinement			= 3;
	m_iCementRefinement			= 5;
	m_iOutputLevel				= 1;
	
}

void CControl::BalancedType(CControl::eBalancedType newval)
{
	m_eBalancedType=newval;
}

//##ModelId=3C0DDDAB0313
CControl::eBalancedType CControl::BalancedType() const
{
	return m_eBalancedType;
}


//From: Mesh refinement
//Label: 'radio button'
//Descr: true if user not allowed to tune mesh 
//refinement, if false: use advanced tuning of mesh 
//refinement
//##ModelId=3C0DDDAB031F
bool CControl::DefaultMeshRefinement() const
{
	return m_bDefaultMeshRefinement;	
}

//##ModelId=3C0DDDAB0321
void CControl::DefaultMeshRefinement(bool bDefaultMeshRefinement)
{
	m_bDefaultMeshRefinement= bDefaultMeshRefinement;
	if(m_bDefaultMeshRefinement)
	{
		CasingRefinement(3);
		CementRefinement(5);
		RockRefinement(2);
	}
}

//From: Mesh refinement
//Label: Rock refinement level
//Descr: refinement indicator for rock meshing
//##ModelId=3C0DDDAB032D
long CControl::RockRefinement() const
{
	return m_iRockRefinement;	
}

//##ModelId=3C0DDDAB032F
void CControl::RockRefinement(long iRockRefinement)
{
	m_iRockRefinement= iRockRefinement;
}

//From: Mesh refinement
//Label: casing refinement level
//Descr: number of elements in casing thickness
//##ModelId=3C0DDDAB0331
long CControl::CasingRefinement() const
{
	return m_iCasingRefinement;	
}

//##ModelId=3C0DDDAB033E
void CControl::CasingRefinement(long iCasingRefinement)
{
	m_iCasingRefinement= iCasingRefinement;
}

//From: Mesh refinement
//Label: cement refinement level
//Descr: number of elements in cement thichkness
//##ModelId=3C0DDDAB034C
long CControl::CementRefinement() const
{
	return m_iCementRefinement;	
}

//##ModelId=3C0DDDAB035D
void CControl::CementRefinement(long iCementRefinement)
{
	m_iCementRefinement= iCementRefinement;
}


//From: Output selection
//Label: Select output level
//Descr: choose 1 of the three optionbuttons to define 
//the level
//##ModelId=3C0DDDAB039D
long CControl::OutputLevel() const
{
	return m_iOutputLevel;	
}

//##ModelId=3C0DDDAB039F
void CControl::OutputLevel(long iOutputLevel)
{
	m_iOutputLevel= iOutputLevel;
}

//From: Completion&Testing
//Label: Well type
//Descr: well type in following options: oil producer, 
//gas producer, gas storage, water injector, steam 
//injector, thermal well, and water source.
//##ModelId=3C0DDDAB03DA
CControl::eWellType CControl::WellType() const
{
	return m_eWellType;	
}

void CControl::WellType(CControl::eWellType WellType)
{
	m_eWellType= WellType;
}

//From: Production&Injection
//Label: Conversion to injector/productor
//Descr:If true: access to conversion parameters
//##ModelId=3C0DDDAC0003
bool CControl::Conversion() const
{
	return m_bConversion;	
}

//##ModelId=3C0DDDAC0005
void CControl::Conversion(bool bConversion)
{
	m_bConversion= bConversion;
}

//From: extreme Operations
//Dependent parameter WELTYP and a table of available 
//extreme operations per well type.
//Label: Fracturing/high rate killing
//Descr: true if well is submitted to fracturing or 
//high-rate killing
//##ModelId=3C0DDDAC0010
bool CControl::Fracturing() const
{
	return m_bFracturing;	
}

//##ModelId=3C0DDDAC0012
void CControl::Fracturing(bool bFracturing)
{
	m_bFracturing= bFracturing;
}

//From: Extreme operations
//Dependent parameter WELTYP and a table of available 
//extreme operations per well type.
//Label: Evacuation/unloading
//Descr: true if well submitted to evacuation or unloading
//##ModelId=3C0DDDAC0020
bool CControl::Evacuation() const
{
	return m_bEvacuation;	
}

//##ModelId=3C0DDDAC002F
void CControl::Evacuation(bool bEvacuation)
{
	m_bEvacuation= bEvacuation;
}

//##ModelId=3DDA0C4401F7
bool CControl::ShutIn() const
{
	return m_bShutIn;
}

//##ModelId=3DDA0C4401F9
void CControl::ShutIn(bool bShutIn)
{
	m_bShutIn = bShutIn;
}

//From: Completion& Testing
//Label: Balanced completion
//Descr: true if balanced completion
//##ModelId=3C0DDDAC0031
bool CControl::BalancedCompletion() const
{
	return m_bBalancedCompletion;	
}

//##ModelId=3C0DDDAC0033
void CControl::BalancedCompletion(bool bBalancedCompletion)
{
	m_bBalancedCompletion= bBalancedCompletion;
	if(m_bBalancedCompletion)
	{
		m_bUnderBalancedCompletion=false;
		m_bOverBalancedCompletion=false;
	}
}

//From: Completion& Testing
//Label: underbalanced completion
//Descr: true if pressure decrease in casing
//##ModelId=3C0DDDAC003F
bool CControl::UnderBalancedCompletion() const
{
	return m_bUnderBalancedCompletion;	
}

//##ModelId=3C0DDDAC004F
void CControl::UnderBalancedCompletion(bool bUnderBalancedCompletion)
{
	m_bUnderBalancedCompletion= bUnderBalancedCompletion;
}

//From: Completion& Testing
//Label: overbalanced completion
//Descr: true if pressure increase in completion
//##ModelId=3C0DDDAC0051
bool CControl::OverBalancedCompletion() const
{
	return m_bOverBalancedCompletion;	
}

//##ModelId=3C0DDDAC0053
void CControl::OverBalancedCompletion(bool bOverBalancedCompletion)
{
	m_bOverBalancedCompletion= bOverBalancedCompletion;
}

//##ModelId=3DDA0C440217
bool CControl::DoCompletion()
{
	return !(m_bShutIn && m_bBalancedCompletion);
}

//##ModelId=3C8D9DF90198
bool CControl::IsProduction()
{
	switch(WellType())
	{
		case CControl::WaterInjector:
			return Conversion();
						
		case CControl::OilProducer:
		case CControl::WaterSource:
		case CControl::GasStorage:
		case CControl::GasProducer:
		case CControl::ThermalWell:
			return true;
		
		case CControl::SteamInjector:
			return false;
	}
	assert(false);
	return false;
}

//##ModelId=3C8D9DF9011B
bool CControl::IsInjection()
{
	switch(WellType())
	{
		case CControl::OilProducer:
		case CControl::WaterSource:
			return Conversion();
		
		case CControl::WaterInjector:
		case CControl::GasStorage:
		case CControl::SteamInjector:
			return true;

		case CControl::GasProducer:
		case CControl::ThermalWell:
			return false;
	}
	assert(false);
	return false;
}

//##ModelId=3C8D9DF9008E
bool CControl::InjectionConversionAllowed()
{
	switch(WellType())
	{

		case CControl::OilProducer:
		case CControl::WaterSource:
			return true;
		
		case CControl::GasStorage:
		case CControl::GasProducer:
		case CControl::ThermalWell:
		case CControl::SteamInjector:
		case CControl::WaterInjector:
			return false;
	}
	assert(false);
	return false;

}

//##ModelId=3DDA0C4401E8
bool CControl::ProductionConversionAllowed()
{
	switch(WellType())
	{

		case CControl::WaterInjector:
			return true;
		
		case CControl::GasStorage:
		case CControl::GasProducer:
		case CControl::ThermalWell:
		case CControl::SteamInjector:
		case CControl::OilProducer:
		case CControl::WaterSource:
			return false;
	}
	assert(false);
	return false;

}
