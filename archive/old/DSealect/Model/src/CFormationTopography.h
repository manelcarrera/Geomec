// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFORMATIONTOPOGRAPHY_3B657735012A_INCLUDED
#define _INC_CFORMATIONTOPOGRAPHY_3B657735012A_INCLUDED




#include "IComponent.h"
#include "CFormation.h"
#include "COverburden.h"
#include "CSeawater.h"
#include "Dependency.h"
#include <vector>


class CDataModel;
//##ModelId=3CAC3FE40206
class CFormationTopography 
: public IComponent
{
	//##ModelId=3CAC3FE50225
	typedef std::set<CFormation> FormationSet;
	//##ModelId=3CAC3FE50234
	typedef FormationSet::iterator FormationSetIt;
	//##ModelId=3CAC3FE50253
	typedef std::pair<FormationSetIt, bool>  FormationSetInserter;

	//##ModelId=3CAC3FE50080
	std::vector<CDependency*> m_DependencyVec;

private:
	//##ModelId=3CAC3FE5009F
	FormationSet m_FormationSet;
	
	//##ModelId=3CAC3FE500AF
	CDataModel* m_pDataModel;

	//##ModelId=3CAC3FE500DD
	CLengthQuantity m_qElevationTVD;
	//##ModelId=3CAC3FE500EC
	bool m_bOffshore;
	//##ModelId=3CAC3FE500FD
	COverburden *m_pOverburden;
	//##ModelId=3CAC3FE5012C
	CSeawater m_Seawater;
	//##ModelId=3CAC3FE5016A
	CLengthQuantity m_qBottomOfTopFormation;
	
public:
	//##ModelId=3CAC3FE50179
	CFormationTopography(CDataModel* pDataModel);
	//##ModelId=3CAC3FE5017B
	~CFormationTopography();

	//##ModelId=3CAC3FE50188
	CFormation* GetFormationAt(long index);
	const CFormation* GetFormationAt(long index) const;
	//##ModelId=3CAC3FE5018A
	CFormation* GetFormationAtDepth(double dDepth);
	const CFormation* GetFormationAtDepth(double dDepth) const;
	//##ModelId=3CAC3FE50199
	CFormation* GetFirstFormation();
	const CFormation* GetFirstFormation() const;
	//##ModelId=3CAC3FE5019A
	CFormation* GetLastFormation();
	const CFormation* GetLastFormation() const;
	//From: Formation definition . Onshore defintion
	//Label: Mean Ground Level if Onshore defintion
	//Label: Mean Sea Level if Offshore definition
	//Descr: True vertical depth of ground floor measured 
	//from derrick floor.
	//##ModelId=3CAC3FE5019B
	CLengthQuantity* ElevationTVD()
	{
		return &m_qElevationTVD;
	}

	const CLengthQuantity* ElevationTVD() const
	{
		return &m_qElevationTVD;
	}


	//##ModelId=3CAC3FE501A8
	COverburden* Overburden()
	{
		return m_pOverburden;
	}

	const COverburden* Overburden() const
	{
		return m_pOverburden;
	}

	//##ModelId=3CAC3FE501A9
	CSeawater* Seawater()
	{
		return &m_Seawater;
	}

	const CSeawater* Seawater() const
	{
		return &m_Seawater;
	}

	//Use CSeawater in case of Offshore
	//##ModelId=3CAC3FE501AA
	bool Offshore() const
	{
		return m_bOffshore;
	}

	//##ModelId=3CAC3FE501AB
	void Offshore(bool newvalue)
	{
		m_bOffshore=newvalue;
	}

	
	
	//Uses Information from Seawater (if Offshore) and 
	//Overburden to get this vertical stress.
	//##ModelId=3CAC3FE501B7
	double GetVertStressOBBottom() const;

	//Uses Overburden.
	//##ModelId=3CAC3FE501B8
	double GetTempOBBottom() const;

	//Returns TRUE if formation is added
	//##ModelId=3CAC3FE501B9
	bool AddFormation(CString Name, double Depth, CString &strError);


	//Returns TRUE is Formation is removed
	//##ModelId=3CAC3FE501D7
	bool RemoveFormation(long Index);
	//##ModelId=3CAC3FE501E6
	bool RemoveFormation(CFormation* Formation);

	//Returns the number of formations
	//##ModelId=3CAC3FE501F6
	long NrOfFormations() const;
	
	//##ModelId=3CAC3FE501F7
	bool CheckComplete() const;

	//##ModelId=3CAC3FE501F8
	CString GetInfo();
	
	//##ModelId=3CAC3FE50205
	bool RemoveAllFormations();
	//##ModelId=3CAC3FE50206
	long GetFormationIndex(const CFormation* pFormation) const;
	//##ModelId=3CAC3FE50208
	bool CanAddFormations() const;

};

#endif /* _INC_CFORMATIONTOPOGRAPHY_3B657735012A_INCLUDED */
