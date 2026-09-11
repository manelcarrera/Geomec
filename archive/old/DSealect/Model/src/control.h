// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCONTROL_3B67E3610216_INCLUDED
#define _INC_CCONTROL_3B67E3610216_INCLUDED



#include "IComponent.h"



//##ModelId=3C0DDDAB0243
class CControl 
: public IComponent
{
public:
	//##ModelId=3C0DDDAC006E
	enum eBalancedType
	{
		//##ModelId=3C0DDDAC007F
		UnderBalanced,
		//##ModelId=3C0DDDAC0080
		OverBalanced,
		//##ModelId=3C0DDDAC008D
		Balanced
	};


	//##ModelId=3C0DDDAC009D
	enum eWellType
	{
		//##ModelId=3C0DDDAC00AC
		OilProducer=1000,
		//##ModelId=3C0DDDAC00AD
		GasProducer,
		//##ModelId=3C0DDDAC00AE
		GasStorage,
		//##ModelId=3C0DDDAC00BC
		WaterInjector,
		//##ModelId=3C0DDDAC00BD
		SteamInjector,
		//##ModelId=3C0DDDAC00BE
		ThermalWell,
		//##ModelId=3C0DDDAC00BF
		WaterSource,
	};

	//##ModelId=3C0DDDAB0253
	CControl();

private:

	//##ModelId=3C0DDDAB0254
	bool m_bDefaultMeshRefinement;
	//##ModelId=3C0DDDAB0273
	bool m_bConversion;
	//##ModelId=3C0DDDAB0274
	bool m_bFracturing;
	//##ModelId=3C0DDDAB0275
	bool m_bEvacuation;
	//##ModelId=3C0DDDAB0281
	bool m_bBalancedCompletion;
	//##ModelId=3C0DDDAB0282
	bool m_bUnderBalancedCompletion;
	//##ModelId=3C0DDDAB0291
	bool m_bOverBalancedCompletion;

	//##ModelId=3DDA0C4401D8
	bool m_bShutIn;

	//##ModelId=3C0DDDAB0292
	long m_iRockRefinement;
	//##ModelId=3C0DDDAB0293
	long m_iCasingRefinement;
	//##ModelId=3C0DDDAB0294
	long m_iCementRefinement;
		
	//##ModelId=3C0DDDAB02B0
	long m_iOutputLevel;
	
	//##ModelId=3CAAA65702D3
	eBalancedType    m_eBalancedType;
	//##ModelId=3CAAA65702E2
	eWellType        m_eWellType;
	
	
public:
	//##ModelId=3DDA0C4401E8
	bool ProductionConversionAllowed();

	//##ModelId=3DDA0C4401E9
	bool InjectionConversionAllowed();

	//##ModelId=3C8D9DF9011B
	bool IsInjection();
	//##ModelId=3C8D9DF90198
	bool IsProduction();
	
	//##ModelId=3C0DDDAB0313
	eBalancedType BalancedType() const;
	//##ModelId=3C0DDDAB031D
	void BalancedType(eBalancedType newval);


	//From: Mesh refinement
	//Label: 'radio button'
	//Descr: true if user not allowed to tune mesh 
	//refinement, if false: use advanced tuning of mesh 
	//refinement
	//##ModelId=3C0DDDAB031F
	bool DefaultMeshRefinement() const;
	//##ModelId=3C0DDDAB0321
	void DefaultMeshRefinement(bool bDefaultMeshRefinement);

	
	//##ModelId=3DDA0C4401F7
	bool ShutIn() const;
	//##ModelId=3DDA0C4401F9
	void ShutIn(bool bShutIn);

	//From: Mesh refinement
	//Label: Rock refinement level
	//Descr: refinement indicator for rock meshing
	//##ModelId=3C0DDDAB032D
	long RockRefinement() const;
	//##ModelId=3C0DDDAB032F
	void RockRefinement(long iRockRefinement);

	//From: Mesh refinement
	//Label: casing refinement level
	//Descr: number of elements in casing thickness
	//##ModelId=3C0DDDAB0331
	long CasingRefinement() const;
	//##ModelId=3C0DDDAB033E
	void CasingRefinement(long iCasingRefinement);

	//From: Mesh refinement
	//Label: cement refinement level
	//Descr: number of elements in cement thichkness
	//##ModelId=3C0DDDAB034C
	long CementRefinement() const;
	//##ModelId=3C0DDDAB035D
	void CementRefinement(long iCementRefinement);
	
	//From: Output selection
	//Label: Select output level
	//Descr: choose 1 of the three optionbuttons to define 
	//the level
	//##ModelId=3C0DDDAB039D
	long OutputLevel() const;
	//##ModelId=3C0DDDAB039F
	void OutputLevel(long iOutputLevel);

	//From: Completion&Testing
	//Label: Well type
	//Descr: well type in following options: oil producer, 
	//gas producer, gas storage, water injector, steam 
	//injector, thermal well, and water source.
	//##ModelId=3C0DDDAB03DA
	eWellType WellType() const;
	//##ModelId=3C0DDDAC0001
	void WellType(eWellType welltype);

	//From: Production&Injection
	//Label: Conversion to injector/productor
	//Descr:If true: access to conversion parameters
	//##ModelId=3C0DDDAC0003
	bool Conversion() const;
	//##ModelId=3C0DDDAC0005
	void Conversion(bool bConversion);

	//From: extreme Operations
	//Dependent parameter WELTYP and a table of available 
	//extreme operations per well type.
	//Label: Fracturing/high rate killing
	//Descr: true if well is submitted to fracturing or 
	//high-rate killing
	//##ModelId=3C0DDDAC0010
	bool Fracturing() const;
	//##ModelId=3C0DDDAC0012
	void Fracturing(bool bFracturing);

	//From: Extreme operations
	//Dependent parameter WELTYP and a table of available 
	//extreme operations per well type.
	//Label: Evacuation/unloading
	//Descr: true if well submitted to evacuation or unloading
	//##ModelId=3C0DDDAC0020
	bool Evacuation() const;
	//##ModelId=3C0DDDAC002F
	void Evacuation(bool bEvacuation);

	//From: Completion& Testing
	//Label: Balanced completion
	//Descr: true if balanced completion
	//##ModelId=3C0DDDAC0031
	bool BalancedCompletion() const;
	//##ModelId=3C0DDDAC0033
	void BalancedCompletion(bool bBalancedCompletion);

	//From: Completion& Testing
	//Label: underbalanced completion
	//Descr: true if pressure decrease in casing
	//##ModelId=3C0DDDAC003F
	bool UnderBalancedCompletion() const;
	//##ModelId=3C0DDDAC004F
	void UnderBalancedCompletion(bool bUnderBalancedCompletion);

	//From: Completion& Testing
	//Label: overbalanced completion
	//Descr: true if pressure increase in completion
	//##ModelId=3C0DDDAC0051
	bool OverBalancedCompletion() const;
	//##ModelId=3C0DDDAC0053
	void OverBalancedCompletion(bool bOverBalancedCompletion);

	//##ModelId=3DDA0C440217
	bool DoCompletion();

};

#endif /* _INC_CCONTROL_3B67E3610216_INCLUDED */
