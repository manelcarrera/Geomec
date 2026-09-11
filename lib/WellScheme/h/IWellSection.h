// IWellSection.h: interface for the IWellSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IWELLSECTION_H__56DDB276_C6D0_4995_A1A1_93F4145D96E4__INCLUDED_)
#define AFX_IWELLSECTION_H__56DDB276_C6D0_4995_A1A1_93F4145D96E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellSchemeInclude.h"
#include "WellPoint.h"

//*******************************************************************
//              
//  FILE:       WellSection.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  IWellSection
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//              
//*******************************************************************

namespace well {

class CWellPathBase;
class CWellPoint;
class CWellPointList;

class IWellSection  : public QObject
{

	Q_OBJECT;

public:
	virtual ~IWellSection();

	//ISection Interface
	virtual const CWellPoint& Top() const=0;
	virtual const CWellPoint& Bottom() const=0;
	virtual QString GetDescription() const=0;

	const CWellPathBase& WellPath() const;
	CWellPathBase& WellPath();
	
	bool Contains(const CWellPoint& point,bool bIncludeEdge=true) const;
	bool Contains(const double& tmd,bool bIncludeEdge=true) const;

	virtual bool Defined() const;

	//operators
	bool operator==(const IWellSection &rhs) const;
	bool operator!=(const IWellSection &rhs) const;

	bool operator <(const IWellSection &rhs) const;
	bool operator >(const IWellSection &rhs) const;

	//return a list with points witch are inside this section
	//Autodelete of new list is false
	CWellPointList FilterContainingPoints(const CWellPointList& list,bool bIncludeEdge =true) const;

	//return a list with points witch are not inside this section
	//Autodelete of new list is false
	CWellPointList FilterNonContainingPoints(const CWellPointList& list,bool bIncludeEdge =true)const;

	double DeltaTVD() const;
	double DeltaTMD() const;

	//divide the section in a number of intervals, the distance is the distance from the top and bottom,
	//add the points to the point list (NrOfIntervals >= 1 and distance >=0)
	void AddIntervalPoints(CWellPointList& lst, int NrOfIntervals,const double& distance, const IWellPointFactory* pFactory=0) const;

	//returns the max min of the bounding box (approx. algorithm)
	void GetBoundingBox(geo::IPoint& max,geo::IPoint& min) const;

	//gets the max/min TVD
	void GetMaxMinTVD(double& maxTVD,double& minTVD) const;

signals:
	void OnDestroy(const well::IWellSection& section);

protected:
	IWellSection(const CWellPathBase& WellPath);
	
	const CWellPathBase* m_pWellPath;
};
}

#endif // !defined(AFX_IWELLSECTION_H__56DDB276_C6D0_4995_A1A1_93F4145D96E4__INCLUDED_)
