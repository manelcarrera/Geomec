// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include <set>
#include "CAnalysis.h"

#include "CBatchTable.h"
#include "CDataModel.h"

//##ModelId=3CA0119C0187
CAnalysis::CAnalysis(CDataModel* pDataModel) 
{
	m_pDataModel = pDataModel;
	m_pDrillingScheme = pDataModel->DrillingScheme();
}

//##ModelId=3C60D57002CF
CAnalysis::~CAnalysis()
{
	
}

//##ModelId=3C3040AF00E8
bool CAnalysis::AddAnalysisPoint(const double& TMD,CString &strError)
{
	if(!m_pDrillingScheme->OverburdenBottomTMD()->Undefined())
	{
	  if(TMD < m_pDrillingScheme->OverburdenBottomTMD()->Value())
	  {
		  strError= CString("Analysis point TMD >= Overburden bottom TMD");
		  return false;
	  }
	}
	
	AnalysisPointSetInserter insert;
	CAnalysisPoint ap(TMD, m_pDataModel);
	insert = m_AnalysisPointSet.insert(ap);

	if(!insert.second)
	{
		strError = CString("No TMD duplication allowed.");
	}
	else
	{
		m_pDataModel->BatchTable()->Invalidate();
	}

	return insert.second;

}

//##ModelId=3C2043F80149
bool CAnalysis::RemoveAnalysisPoint(CAnalysisPoint* analysispoint)
{
	AnalysisPointSetIt it;
	bool  bRemove = false;
	for(it = m_AnalysisPointSet.begin();it != m_AnalysisPointSet.end();it++)
	{
		if(analysispoint==&(*it))
		{
			m_AnalysisPointSet.erase(it);
			bRemove = true;
			break;
		}
	}
	m_pDataModel->BatchTable()->Invalidate();

	return bRemove;

}

//##ModelId=3C0DDDAF0350
bool CAnalysis::RemoveAnalysisPoint(long index)
{

	AnalysisPointSetIt it;
	
	long i=0;
	for(it = m_AnalysisPointSet.begin();it != m_AnalysisPointSet.end();it++)
	{
		if(i == index)
		{
			m_AnalysisPointSet.erase(it);
			return true;
		}
		i++;
	}
	m_pDataModel->BatchTable()->Invalidate();
	return false;
	
}

//##ModelId=3C2043F7035C
CAnalysisPoint* CAnalysis::GetAnalysisPointAt(int index)
{
	AnalysisPointSetIt it;
	
	long i=0;
	for(it = m_AnalysisPointSet.begin();it != m_AnalysisPointSet.end();it++)
	{
		if(i == index)
		{
			return &(*it);
		}
		i++;
	}

	assert(false);
	return 0;
}

const CAnalysisPoint* CAnalysis::GetAnalysisPointAt(int index) const
{
  return (const_cast<CAnalysis*>(this))->GetAnalysisPointAt(index);
}

//##ModelId=3C0DDDAF035D
long CAnalysis::NrOfAnalysisPoints() const
{
	return m_AnalysisPointSet.size();
}

//##ModelId=3C2043F80271
long CAnalysis::GetAnalysisPointIndex(CAnalysisPoint *pAnalysisPoint) const
{

	for(long i = 0; i<NrOfAnalysisPoints();i++)
	{
		if (pAnalysisPoint == GetAnalysisPointAt(i))
			return i;
	}
	return -1;

}

//##ModelId=3C60D5700291
bool CAnalysis::RemoveAllAnalysisPoints()
{
	m_AnalysisPointSet.clear();
	m_pDataModel->BatchTable()->Invalidate();
	return true;
}


//##ModelId=3CA0119C00F9
bool CAnalysis::CheckComplete() const
{
	if(NrOfAnalysisPoints()<1)
		return false;
	
	for(int i=0 ;i< NrOfAnalysisPoints();i++)
	{
		if(!GetAnalysisPointAt(i)->CheckComplete())
			return false;
	}

	return true;

}
