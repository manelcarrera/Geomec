// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <set>

#include "CDataModel.h"
#include "CFormationTopography.h"
#include "CDependenciesManager.h"


//##ModelId=3CAC3FE50179
CFormationTopography::CFormationTopography(CDataModel* pDataModel)
:
m_qElevationTVD(1.0,0.0,DOUBLE_MAX_,true,false),
m_bOffshore(true)
{
	m_pDataModel=pDataModel;
	m_FormationSet.clear();
	m_qElevationTVD.Invalidate();
	m_pOverburden = new COverburden(m_pDataModel);
}

//##ModelId=3CAC3FE5017B
CFormationTopography::~CFormationTopography()
{
	delete m_pOverburden;
}

//##ModelId=3CAC3FE501B7
double CFormationTopography::GetVertStressOBBottom() const
{
	// NOTE: Requires a correct return value to compile.
	return 0;
}


//##ModelId=3CAC3FE501B8
double CFormationTopography::GetTempOBBottom() const
{
	// NOTE: Requires a correct return value to compile.
	return 0;
}

//##ModelId=3CAC3FE501B9
bool CFormationTopography::AddFormation(CString Name, double Depth, CString &strError) 
{
	if (Depth<= Overburden()->OverburdenBottomTVD()->Value())
	{
		strError= CString("Formation bottom TVD > Overburden bottom TVD");
		return false;
	}
	
	Name.TrimLeft();
	Name.TrimRight();
	if(Name.IsEmpty())
	{
		strError= CString("Empty Formation name not allowed.");
		return false;
	}

	FormationSetInserter inserter;
	CFormation form(Name,Depth, m_pDataModel);
	long iSize = m_FormationSet.size();
	inserter=m_FormationSet.insert(form);

	if(inserter.second)
	{
		m_qBottomOfTopFormation=*GetFormationAt(0)->BottomTVD();
	}
	else
	{
		strError=CString("No TVD duplication allowed.");
	}

	//ASSERT(insert.second);
	return inserter.second;
}

//##ModelId=3CAC3FE501E6
bool CFormationTopography::RemoveFormation(CFormation *formation)
{
	FormationSetIt it;
	bool  bRemove=false;

	for(it=m_FormationSet.begin();it != m_FormationSet.end();it++)
	{
		if(formation==&(*it))
		{
			m_FormationSet.erase(it);
			bRemove=true;
			break;
		}
	}

	if(bRemove)
	{
		if(NrOfFormations())
		{
			m_qBottomOfTopFormation=*GetFormationAt(0)->BottomTVD();
		}
		else
		{
			m_qBottomOfTopFormation.Invalidate();
		}
		return true;
	}

	return false;
	
}


//##ModelId=3CAC3FE501D7
bool CFormationTopography::RemoveFormation(long index)
{
	FormationSetIt it;
	bool  bRemove=false;
	long i=0;
	for(it=m_FormationSet.begin();it != m_FormationSet.end();it++)
	{
		if(i==index)
		{
			m_FormationSet.erase(it);
			bRemove=true;
			break;
		}
		i++;
	}
	
	if(bRemove)
	{
		if(NrOfFormations())
		{
			m_qBottomOfTopFormation=*GetFormationAt(0)->BottomTVD();
		}
		else
		{
			m_qBottomOfTopFormation.Invalidate();
		}
		return true;
	}
		
	return false;
}

//##ModelId=3CAC3FE501F6
long CFormationTopography::NrOfFormations() const
{
	return m_FormationSet.size();
}

//##ModelId=3CAC3FE501F7
bool CFormationTopography::CheckComplete() const
{
	for(int i = 0 ;i<NrOfFormations();i++)
	{	
		if(!GetFormationAt(i)->CheckComplete())
			return false;
	}
	
	if(!Overburden()->CheckComplete())
		return false;

	return true;
}

//##ModelId=3CAC3FE501F8
CString CFormationTopography::GetInfo()
{
	// NOTE: Requires a correct return value to compile.
	return CString("FormationTopography");
}


//##ModelId=3CAC3FE50188
CFormation* CFormationTopography::GetFormationAt(long index)
{
	FormationSetIt it;
	
	long i=0;
	for(it=m_FormationSet.begin();it != m_FormationSet.end();it++)
	{
		if(i==index)
		{
			return &(*it);
		}
		i++;
	}

	assert(false);
	return 0;
}

const CFormation* CFormationTopography::GetFormationAt(long index) const
{
  return (const_cast<CFormationTopography*>(this))->GetFormationAt(index);
}

//##ModelId=3CAC3FE5018A
CFormation* CFormationTopography::GetFormationAtDepth(double dDepth)
{
	FormationSetIt it;

	if (dDepth < Overburden()->OverburdenBottomTVD()->Value())
	{
		return 0;
	}

	if(dDepth == Overburden()->OverburdenBottomTVD()->Value())
	{
		return GetFirstFormation();
	}
	
	
	for(it=m_FormationSet.begin();it != m_FormationSet.end();it++)
	{
		if ((*it).BottomTVD()->Value()>=dDepth)
		{
			return &(*it);
		}
	}
	return 0;
}

const CFormation* CFormationTopography::GetFormationAtDepth(double dDepth) const
{
  return (const_cast<CFormationTopography*>(this))->GetFormationAtDepth(dDepth);
}


//##ModelId=3CAC3FE50205
bool CFormationTopography::RemoveAllFormations()
{
	m_FormationSet.clear();
	m_qBottomOfTopFormation.Invalidate();	
	return true;

}

//##ModelId=3CAC3FE50206
long CFormationTopography::GetFormationIndex(const CFormation *pFormation) const
{

	for(long i=0; i<NrOfFormations();i++)
	{
		if (pFormation== GetFormationAt(i))
			return i;
	}
	return -1;

}

//##ModelId=3CAC3FE50208
bool CFormationTopography::CanAddFormations() const
{
	if (ElevationTVD()->Undefined())
	{
		return false;
	}
	if ((Seawater()->SeaBedTVD().Undefined()) && (Offshore()))
	{
		return false;
	}
	if (Overburden()->OverburdenBottomTVD()->Undefined())
	{
		return false;
	}
	return true;
}


//##ModelId=3CAC3FE50199
CFormation* CFormationTopography::GetFirstFormation()
{
	ASSERT(NrOfFormations());
	return GetFormationAt(0);
}

const CFormation* CFormationTopography::GetFirstFormation() const
{
	ASSERT(NrOfFormations());
	return GetFormationAt(0);
}

//##ModelId=3CAC3FE5019A
CFormation* CFormationTopography::GetLastFormation()
{
	ASSERT(NrOfFormations());
	return GetFormationAt(NrOfFormations()-1);
}

const CFormation* CFormationTopography::GetLastFormation() const
{
	ASSERT(NrOfFormations());
	return GetFormationAt(NrOfFormations()-1);
}
