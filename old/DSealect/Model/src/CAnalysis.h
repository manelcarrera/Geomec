// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CANALYSIS_3B669DD3006E_INCLUDED
#define _INC_CANALYSIS_3B669DD3006E_INCLUDED



#include "IComponent.h"
#include "CAnalysisPoint.h"
#include "CDrillingScheme.h"

#include <set>


//Operational regimes and Control are grouped in separate 
//components since they are handled on different leaves in 
//the tree-structure. In this way they will have their own 
//'CheckComplete' function etc.
//##ModelId=3C0DDDAF02FE
class CAnalysis 
: public IComponent
{
private:

	//##ModelId=3C2043F8036B
	typedef std::set<CAnalysisPoint> AnalysisPointSet;
	//##ModelId=3C2043F90010
	typedef AnalysisPointSet::iterator AnalysisPointSetIt;
	//##ModelId=3C2043F900AC
	typedef std::pair<AnalysisPointSetIt, bool>  AnalysisPointSetInserter;

	//##ModelId=3C2043F7030E
	AnalysisPointSet m_AnalysisPointSet;
	
	//##ModelId=3CA0119C00DB
	CDataModel* m_pDataModel;
	//##ModelId=3C3040AE0378
	CDrillingScheme* m_pDrillingScheme;
	
public:
	//##ModelId=3CA0119C00F9
	virtual bool CheckComplete() const;
	//##ModelId=3C60D5700291
	bool RemoveAllAnalysisPoints();
	
	//##ModelId=3CA0119C0187
	CAnalysis(CDataModel *pDataModel);

	//##ModelId=3C60D57002CF
	~CAnalysis();

	//##ModelId=3C2043F7035C
	CAnalysisPoint* GetAnalysisPointAt(int Index);
	const CAnalysisPoint* GetAnalysisPointAt(int Index) const;

	//Add an Analysis Point to the list.
	//##ModelId=3C3040AF00E8
	bool AddAnalysisPoint(const double& TMD,CString &strError);

	//Removes an Analysis Point from the list 
	//Returns TRUE if succeeded
	//##ModelId=3C0DDDAF0350
	bool RemoveAnalysisPoint(long Index);
	//##ModelId=3C2043F80149
	bool RemoveAnalysisPoint(CAnalysisPoint* analysispoint);

	//Returns the number of analysis points in the list
	//##ModelId=3C0DDDAF035D
	long NrOfAnalysisPoints() const;
	//##ModelId=3C2043F80271
	long GetAnalysisPointIndex(CAnalysisPoint *pAnalysisPoint) const;

};

#endif /* _INC_CANALYSIS_3B669DD3006E_INCLUDED */
