#ifndef NEWWELLSECTION_H
#define NEWWELLSECTION_H

#include "NewWellPoint.h"
#include <string>

class CNewWellPathBase;

class CWellPathBase;
class CWellPoint;
class CNewWellSection;

class INewWellSection
{
protected:
  INewWellSection(const CNewWellPathBase& WellPath);

public:
  virtual ~INewWellSection();
  
  //ISection Interface
  virtual const CNewWellPoint& Top() const=0;
  virtual const CNewWellPoint& Bottom() const=0;
  virtual std::string GetDescription() const=0;

  const CNewWellPathBase& WellPath() const {return *m_pNewWellPath;}
  CNewWellPathBase& WellPath()
  {return *const_cast<CNewWellPathBase*>(m_pNewWellPath);}
  
  bool Contains(const CNewWellPoint& point,bool bIncludeEdge=true) const;
  bool Contains(const double& tmd,bool bIncludeEdge=true) const;

  //virtual bool Defined() const;

  //operators
  //bool operator==(const IWellSection &rhs) const;
  //bool operator!=(const IWellSection &rhs) const;

  //bool operator <(const IWellSection &rhs) const;
  //bool operator >(const IWellSection &rhs) const;

  //return a list with points witch are inside this section
  //Autodelete of new list is false
  //CWellPointList FilterContainingPoints(const CWellPointList& list,bool bIncludeEdge =true) const;

  //return a list with points witch are not inside this section
  //Autodelete of new list is false
  //CWellPointList FilterNonContainingPoints(const CWellPointList& list,bool bIncludeEdge =true)const;

  //double DeltaTVD() const;
  //double DeltaTMD() const;

  //divide the section in a number of intervals, the distance is the distance from the top and bottom,
  //add the points to the point list (NrOfIntervals >= 1 and distance >=0)
  //void AddIntervalPoints(CWellPointList& lst, int NrOfIntervals,const double& distance, const IWellPointFactory* pFactory=0) const;

  //returns the max min of the bounding box (approx. algorithm)
  //void GetBoundingBox(geo::IPoint& max,geo::IPoint& min) const;

  //gets the max/min TVD
  //void GetMaxMinTVD(double& maxTVD,double& minTVD) const;

protected:
  const CNewWellPathBase* m_pNewWellPath;
};

class CNewWellSection : public INewWellSection
{
public:
  CNewWellSection
  ( CNewWellPathBase& pWellPath 
  , const double &topTMD
  , const double &bottomTMD
  );

  virtual ~CNewWellSection();

  void SetDescription(const std::string& description);
  
	
  //IWellSection interface
  virtual const CNewWellPoint & Top() const;
  virtual const CNewWellPoint & Bottom() const;
  virtual std::string GetDescription() const;
  //virtual const CNewWellPathBase &WellPath() const; 

private:
  CNewWellPoint m_Top;
  CNewWellPoint m_Bottom;
  std::string m_Description;
};

#endif // NEWWELLSECTION_H
