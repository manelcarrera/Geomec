// FistExport.h: interface for the CFistExport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FISTEXPORT_H__513EB9F0_8CE0_49F8_9F16_AEC37801D2E3__INCLUDED_)
#define AFX_FISTEXPORT_H__513EB9F0_8CE0_49F8_9F16_AEC37801D2E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "xmlbase.h"
#include "mfStressTensor.h"
#include "NewWellPoint.h"

class CModelBase;
class CNewWellPath;
class QDate;
class CFormationBase;
class CGeoWellPoint;
class CFormationSection;
class CFistExport : public CXMLBase
{
  typedef std::map<const CFormationBase*, int> TFormationNameMap;

  class CLayer
  {
  public:
  CLayer(CFistExport& parent,
           QDomElement xmlparent,
           const well::CWellPoint& wellpoint,
           double dStartAHD,
           double dEndAHD,
           const CModelBase& model,
           TFormationNameMap& mpFormationNames);

  const well::CWellPoint& WellPoint() const;
  const CFormationBase& Formation() const;

  double StartAHD() const;
  double EndAHD() const;

  const QString& Name() const;

  private:
  void FindFormationAndElement();
  void CreateLayerDirectory(QDomElement xmlparent);

  private:
  CFistExport& m_parent;
  const well::CWellPoint& m_wellpoint;
  const CFormationBase* m_pFormation;
  const geo::IElement* m_pElement;
  const CModelBase& m_model;
  TFormationNameMap& m_mpFormationNames;
  double m_dStartAHD;
  double m_dEndAHD;
  QString m_strName;
  }; //CLayer

  class CNewLayer //CNewWellPath wjrx mantis 3564
  {
  public:
  CNewLayer(CFistExport& parent,
           QDomElement xmlparent,
           const CNewWellPoint& wellpoint,
           double dStartAHD,
           double dEndAHD,
           const CModelBase& model,
           TFormationNameMap& mpFormationNames);

  const CNewWellPoint& WellPoint() const;
  const CFormationBase& Formation() const;

  double StartAHD() const;
  double EndAHD() const;

  const QString& Name() const;

  private:
  void FindFormationAndElement();
  void CreateLayerDirectory(QDomElement xmlparent);

  private:
  CFistExport& m_parent;
  const CNewWellPoint m_wellpoint;
  const CFormationBase* m_pFormation;
  const geo::IElement* m_pElement;
  const CModelBase& m_model;
  TFormationNameMap& m_mpFormationNames;
  double m_dStartAHD;
  double m_dEndAHD;
  QString m_strName;
  }; //CNewLayer

  typedef std::map<const well::CWellPoint*, CLayer> TLayerMap;
  typedef std::map<const CNewWellPoint, CNewLayer> TNewLayerMap;
  TLayerMap m_mpLayers;
  TNewLayerMap m_mpNewLayers;
  bool m_bNewWellPaths;

  const CModelBase *m_pModel;
  const CNewWellPath *m_pNewWellPath;
  std::list<const CNewWellPath *> m_NewWellList;
  void Init();
  void InitForNewWellPath();
  void CreateCaseDirectory();
  void CreateWellDirectory(const CNewWellPath *pPath);

  QPair<double, double> FindProducingInterval(int nIndex, const CNewWellPath *pPath) const;
  //CStressTensor TotalStress(const CGeoWellPoint *pPoint, const QDate &date) const;
  CStressTensor TotalStress(const geo::IPoint *pPoint, const QDate &date) const;

public:
  CFistExport
  ( const CModelBase *pModel
  , const CNewWellPath *pNewWellPath= 0
  , bool bNewWellPaths=false
  );
  CFistExport(const CModelBase *pModel, const CNewWellPath *pWellPath);
  virtual ~CFistExport();

};

#endif // !defined(AFX_FISTEXPORT_H__513EB9F0_8CE0_49F8_9F16_AEC37801D2E3__INCLUDED_)
