// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDRILLINGOPERATION_3B667C4B0056_INCLUDED
#define _INC_CDRILLINGOPERATION_3B667C4B0056_INCLUDED



#include "IComponent.h"
#include "CCasingOperation.h"
#include "CCementingOperation.h"
#include "CCementScenariosTable.h"

//##ModelId=3C0DDDAD037B
class CDrillingOperation 
: public IComponent
{
private:
		
	//##ModelId=3CAAA65A0330
	CSmallLengthQuantity m_qBoreholeDiameter;
	//##ModelId=3CAAA65A0350
	CDensityQuantity m_qMudDensityAtSurface;
	//##ModelId=3CAAA65A035F
	CCompressibilityQuantity m_qMudCompressibility;
	//##ModelId=3CAAA65A037E
	CLengthQuantity m_qOperationBottomTMD;
	//##ModelId=3CAAA65A038E
	CCasingOperation m_CasingOperation;
	//##ModelId=3CAAA65A03BD
	CCementingOperation m_CementingOperation;
	//##ModelId=3CAAA65B0004
	CCementScenariosTable m_CementScenariosTable;

public:
	//##ModelId=3CA0118C0290
	virtual bool CheckComplete() const;
	//##ModelId=3C0DDDAE0015
	CDrillingOperation();

	//From: Drilling Operation
	//Label: Borehole diameter
	//Descr: Diameter of the borehole
	
	//##ModelId=3C0DDDAE0020
	CSmallLengthQuantity *BoreholeDiameter();
	const CSmallLengthQuantity *BoreholeDiameter() const;
	
	//##ModelId=3C8D9DFF00DC
	double BoreholeRadius() const;

	//From: Drilling Operation . 
	//Label: Mud density at surface
	//Descr: Mud density at surface
	//##ModelId=3C0DDDAE0021
	CDensityQuantity *MudDensityAtSurface();
	const CDensityQuantity *MudDensityAtSurface() const;

	//From: Drilling Operation . 
	//Label: Mud Compressability
	//Descr: Mud Compressability
	//##ModelId=3C0DDDAE0022
	CCompressibilityQuantity *MudCompressibility();
	const CCompressibilityQuantity *MudCompressibility() const;

	//Bottom TMD of operation. In this version there is only 
	//one drilling operation, so the operation bottom TMD is 
	//equal to the model bottom TMD.
	//##ModelId=3C0DDDAE0023
	CLengthQuantity *OperationBottomTMD();
	const CLengthQuantity *OperationBottomTMD() const;

	//##ModelId=3C0DDDAE0024
	CCasingOperation *CasingOperation();
	const CCasingOperation *CasingOperation() const;

	//##ModelId=3C0DDDAE002F
	CCementingOperation *CementingOperation();
	const CCementingOperation *CementingOperation() const;

	//##ModelId=3C0DDDAE0030
	CCementScenariosTable *CementScenariosTable();
	const CCementScenariosTable *CementScenariosTable() const;

	
private:
	

};

#endif /* _INC_CDRILLINGOPERATION_3B667C4B0056_INCLUDED */
