// wellpath.h: interface for the WellPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPATH_H__91487968_C15C_45A6_A7FD_7F617E9F33D2__INCLUDED_)
#define AFX_WELLPATH_H__91487968_C15C_45A6_A7FD_7F617E9F33D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*******************************************************************
//              
//  FILE:       wellpath.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellPath
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//              
//*******************************************************************

// Includes

#include "WellSchemeInclude.h"
#include "WellPointList.h"
#include "WellSectionList.h"
#include "WellDefinitionPointList.h"

#include <qdatetime.h> 
//#include <qmemarray.h> 
//#include <qvaluevector.h>
#include <QMap>

#include "DocumentBase.h"
#include "IModelObject.h"
#include "StreamVersion.h"
#include "CommonWellPath.h"

namespace well {

class CWellPoint;

class CWellDateList : public QObject
{
  Q_OBJECT;

public:

  typedef QList<QDate*>::iterator Iterator;

  //constructors
  CWellDateList();
  virtual ~CWellDateList();
  CWellDateList& operator=(const CWellDateList& rhs);
  CWellDateList(const CWellDateList& rhs);

  QDate* At(int index);
  const QDate* At(int index) const;

  //iterators
  Iterator begin() const;
  Iterator end() const;
  Iterator GetIteratorAt(QDate* pDate) const;

  int AddDate(QDate *pDate);
  bool RemoveDate(int nIndex);
  int Size() const;

  QDate *First() const;
  QDate *Next() const;

  void AutoDelete(bool bAutoDelete);
  void Clear();


private:

  CWellDateList* This() const;
  QList<QDate*> m_lstDates;
  QList<QDate*>::iterator m_iterator;
  virtual int compareItems( QDate* item1, QDate* item2 );
  bool m_bAutoDelete;
  
};



//Dummy document for creating wellpaths with default constructor
class CWellPathBaseDocument : public CDocumentBase
{
public:
  CWellPathBaseDocument(){}
  virtual ~CWellPathBaseDocument(){}

  //CDocumentBase interface
  virtual void createContainers(){}
  virtual QString documentType() const{return "CWellPathBaseDocument"; }
  virtual CStreamVersion documentVersion() const {	return CStreamVersion(0,0,0);	}	
};







class CWellPathBase : public CCommonWellPath 
{

  Q_OBJECT;

friend class CWellPoint;

//
//signals:
//	void DefPointsChanged();
//	void WellNameChanged();
//	void HoleNameChanged();
//	void LocationChanged();
//	void MeanGroundOrSeabedLevelChanged();
//	void MeanSeaLevelChanged();
//
//	//for progress
//	void StartLongJob(const QString& description, long NrOfSteps);
//	void DuringLongJob(long Step);
//	void FinishedLongJob();
//protected:
//	virtual void EmitDefPointsChanged();
//	virtual void EmitWellNameChanged();
//	virtual void EmitHoleNameChanged();
//	virtual void EmitLocationChanged();
//	virtual void EmitMeanGroundOrSeabedLevelChanged();
//	virtual void EmitMeanSeaLevelChanged();
//	virtual void EmitStartLongJob(const QString& description, long NrOfSteps);
//	virtual void EmitDuringLongJob(long Step);
//	virtual void EmitFinishedLongJob();
public:
  enum LOCATION
  {
    ONSHORE,
    PLATFORM,
    SUBSEA
  };

public slots:
  void SetDescription(QString);
  void SetWellType(QString);
  void SetFieldName(QString);
  void SetLocationName(QString);
  void SetWellName(QString);
  void SetCountry(QString);
  void SetSpudDate(const QDate&);
  void SetBirthDate(const QDate&);
  void SetLocation(LOCATION location);

  void SetCustomerName(QString str);
  void SetCustomerContact(QString str);
  void SetJobNumber(QString str);

private slots:
  void OnQuantityChanged(const CDoubleQuantity& quantity);


public:
  static void GetBetweenPosition( const CWellPoint& p1,
                  const CWellPoint& p2,
                  const double& tmd,
                  double& eas,
                  double& nor,
                  double& tvd,
                  double& azi,
                  double& inc);


  LOCATION GetLocation() const;
  

  //construc
  CWellPathBase();
  virtual ~CWellPathBase();
  CWellPathBase(const CWellPathBase& right);

  CWellPathBase(const QString &new_text); 

  virtual void onCreate();

  bool Defined() const;

  //pointlist operations/atributes
  bool AutoDeletePointLists() const;
  void AutoDeletePointLists(bool autodel);
  void InsertWellPointList(CWellPointList* pList,int listID);
  CWellPointList& GetWellPointList(int listID);
  const CWellPointList& GetWellPointList(int listID) const;
  void RemoveWellPointList(int listID);
  void RemoveAllWellPointLists();
  QList<int> GetAllPointListID() const;
  bool IsWellPointList(int listID) const;
  int GetUniqueWellPointListID() const;

  void ChangeGlobalPos(const double& northing,
             const double& easting,
             const double& tvd);


  CLengthQuantity& MeanGroundOrSeabedLevel(); //always
  CLengthQuantity& MeanSeaLevel();	//PLATFORM and SUBSEA only
  double HeightOfWaterColumn() const;
  double DeltaTMD() const;
  double DeltaTVD() const;


  const CLengthQuantity& GlobalNorthing() const;
  const CLengthQuantity& GlobalEasting() const;
  const CLengthQuantity& GlobalTVD() const;


  //SectionList operations/attributes
  bool AutoDeleteSectionLists() const;
  void AutoDeleteSectionLists(bool autodel);
  void InsertWellSectionList(CWellSectionList* pList,int listID);
  CWellSectionList& GetWellSectionList(int listID);
  const CWellSectionList& GetWellSectionList(int listID) const;
  void RemoveWellSectionList(int listID);
  void RemoveAllWellSectionLists();

  
  double GetDogLegSeverity(double tmd) const;
  QString GetDescription() const;
  QString GetWellName() const;
  QString GetFieldName() const;
  QString GetWellType() const;
  QString GetLocationName() const;
  QString GetCountry() const;

  QString GetCustomerName() const;
  QString GetJobNumber() const;
  QString GetCustomerContact() const;

  const QDate& GetSpudDate() const;
  const QDate& GetBirthDate() const;

  double TMD2TVD(double tmd) const;  //unambigus (asserts if not on wellpath)
  double TVD2TMD(double tvd) const;  //is ambigus return first one (asserts if not on wellpath)

  //if you clear the defpointlist by calling RemoveAllPoints direct from DefPointList()
  //no DefPointsChanged() signal is emitted !! (see InvalidateDefPointList());
  virtual const CWellDefinitionPointList& DefPointList() const;
  virtual CWellDefinitionPointList& DefPointList();

  //clears the Defpoints and emit DefPointsChanged() signal.
  void InvalidateDefPointList();

  //clears the Defpoints and emit DefPointsChanged() signal.
  void HardCopyDefPointList(const CWellPathBase& org_wellpath);

  //initialization of the definition points with arrays (SI_UNIT)
  bool InitFromArray_tmd_azi_inc(
                      double global_northing,
                      double global_easting,
                      double global_depth,
                      double start_northing,
                      double start_easting,
                      double start_depth,
                      QVector<double>& tmd,
                      QVector<double>& azi,
                      QVector<double>& inc);

  //initialization of the definition points with arrays uses numerical approx (SI_UNIT)
  bool InitFromArray_tmd_tvd_azi(
                        double global_northing,
                     		double global_easting,
                        double global_depth,
                        double start_northing,
                     		double start_easting,
                        double start_inc,
                        QVector<double>& tmd,
                        QVector<double>& tvd,
                        QVector<double>& azi);

  //initialization of a vertical wellpath (SI_UNIT)
  void InitVertical(	double start_northing,
            double start_easting,
            double start_depth,
            double start_tmd,
            double end_depth);

  bool InitFromPointArray(const QVector<geo::CPoint>& vcPoint);

  //initialization of the definition points with mores well report file (SI_UNIT)
  bool InitFromMWRFile(QString sFileName);

  //dumps the coordinates of the definition points to a file(for debug and testing)
  void DumpToFile(QString sFileName) const;

  //intersection with geometrical objects
  void CalculateIntersections(const geo::IMesh& mesh, CWellPointList& list) const;
  void GetIntersectionPointsWithPlane(const geo::CPlane& plane, CWellPointList& list) const;

  //Attributes
  virtual bool IsVertical() const; 
  bool IsSnakeWell() const;
  bool IsDeviated() const; //== not vertical
  bool IsStraight() const; //not curved 

  void GetBoundingBox(geo::IPoint& max,geo::IPoint& min) const;

  int AddDate(QDate *pDate);
  QDate *GetDate(int nIndex);

  void GetUpLowDefPoints(double tmd, CWellPoint **pUpper, CWellPoint **pLower) const;



protected:
  CWellDefinitionPointList *m_pDefPointList;
  QString m_WellName;
  QString m_Description;
  QString m_WellType;
  QString m_FieldName;
  QString m_LocationName;
  QString m_Country;

  QString m_CustName;
  QString m_JobNumber;
  QString m_CustContact;

  QDate m_SpudDate;
  QDate m_BirthDate;

  CLengthQuantity m_MeanGroundOrSeabedLevel; //always
  CLengthQuantity m_MeanSeaLevel;	//PLATFORM and SUBSEA only


  //global position
  CLengthQuantity m_GlobalNorthing;
  CLengthQuantity m_GlobalEasting;
  CLengthQuantity m_GlobalTVD;

  
  


private:
  QMap<int,CWellPointList*> m_PointListMap;
  QMap<int,CWellSectionList*> m_SectionListMap;
  CWellPathBase* This() const; //no design fault, just handy!!!

  //initialization of a well_point
  bool InitWellPoint(CWellPoint& Point) const;
  bool m_AutoDeletePointLists;
  bool m_AutoDeleteSectionLists;
  LOCATION m_Location;

  // dates of importance
  CWellDateList m_DateList;
  void		  Init();

//	void DoGetInterSectionsWithBodyGroup(const geo::CBodyGroup& bodies,CWellPointList& list);	
};

}


#endif // !defined(AFX_WELLPATH_H__91487968_C15C_45A6_A7FD_7F617E9F33D2__INCLUDED_)
