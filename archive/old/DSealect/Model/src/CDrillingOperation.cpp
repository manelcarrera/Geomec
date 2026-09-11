// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CDrillingOperation.h"


//##ModelId=3C0DDDAE0015
CDrillingOperation::CDrillingOperation()
:
m_qBoreholeDiameter		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qMudDensityAtSurface	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qMudCompressibility	(1.0, 0.0, DOUBLE_MAX_, true, false),
m_qOperationBottomTMD	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qBoreholeDiameter.Invalidate();
	m_qMudDensityAtSurface.Invalidate();
	m_qMudCompressibility.Invalidate();
	m_qOperationBottomTMD.Invalidate();
	
}



//From: Drilling Operation
//Label: Borehole diameter
//Descr: Diameter of the borehole

//##ModelId=3C0DDDAE0020
CSmallLengthQuantity *CDrillingOperation::BoreholeDiameter()
{
	return &m_qBoreholeDiameter;
}

const CSmallLengthQuantity *CDrillingOperation::BoreholeDiameter() const
{
	return &m_qBoreholeDiameter;
}

//##ModelId=3C8D9DFF00DC
double CDrillingOperation::BoreholeRadius() const
{
	return m_qBoreholeDiameter.Value()/2.0;
}

//From: Drilling Operation . 
//Label: Mud density at surface
//Descr: Mud density at surface
//##ModelId=3C0DDDAE0021
CDensityQuantity *CDrillingOperation::MudDensityAtSurface() 
{
	return &m_qMudDensityAtSurface;
}

const CDensityQuantity *CDrillingOperation::MudDensityAtSurface() const
{
	return &m_qMudDensityAtSurface;
}


//From: Drilling Operation . 
//Label: Mud Compressability
//Descr: Mud Compressability
//##ModelId=3C0DDDAE0022
CCompressibilityQuantity *CDrillingOperation::MudCompressibility()
{
	return &m_qMudCompressibility;
}

const CCompressibilityQuantity *CDrillingOperation::MudCompressibility() const
{
	return &m_qMudCompressibility;
}

//Bottom TMD of operation. In this version there is only 
//one drilling operation, so the operation bottom TMD is 
//equal to the model bottom TMD.
//##ModelId=3C0DDDAE0023
CLengthQuantity *CDrillingOperation::OperationBottomTMD()
{
	return &m_qOperationBottomTMD;
}

const CLengthQuantity *CDrillingOperation::OperationBottomTMD() const
{
	return &m_qOperationBottomTMD;
}


//##ModelId=3C0DDDAE0024
CCasingOperation *CDrillingOperation::CasingOperation()
{
	return &m_CasingOperation;			
}

const CCasingOperation *CDrillingOperation::CasingOperation() const
{
	return &m_CasingOperation;			
}

//##ModelId=3C0DDDAE002F
CCementingOperation *CDrillingOperation::CementingOperation()
{
	return &m_CementingOperation;
}

const CCementingOperation *CDrillingOperation::CementingOperation() const
{
	return &m_CementingOperation;
}

//##ModelId=3C0DDDAE0030
CCementScenariosTable *CDrillingOperation::CementScenariosTable()
{
	return &m_CementScenariosTable;
}

const CCementScenariosTable *CDrillingOperation::CementScenariosTable() const
{
	return &m_CementScenariosTable;
}

//##ModelId=3CA0118C0290
bool CDrillingOperation::CheckComplete() const
{
	if(m_qBoreholeDiameter.Undefined())
		return false;

	if(m_qMudCompressibility.Undefined())
		return false;

	if(m_qMudDensityAtSurface.Undefined())
		return false;

	//if(m_qOperationBottomTMD.Undefined())
	//	return false;

	if(!CasingOperation()->CheckComplete())
		return false;

	if(!CementingOperation()->CheckComplete())
		return false;

	if(!CementScenariosTable()->CheckComplete())
		return false;

	return true;

}
