// AnalysisPoint.h: interface for the CAnalysisPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSISPOINT_H__7015C340_E5D1_4D6D_888E_81864C8160BC__INCLUDED_)
#define AFX_ANALYSISPOINT_H__7015C340_E5D1_4D6D_888E_81864C8160BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CModelBase;

#include "GeomecModelVisitor.h"
#include "colornode.h"

class CAnalysisPoint : public CColorNode {
protected:
  geo::CPoint m_point;

public:
  // Construction / Destruction
  CAnalysisPoint(CFemAppModel &model);
  CAnalysisPoint(const QString &sName, const geo::IPoint &point, CModelBase &model);
  CAnalysisPoint(unsigned int uName, const geo::IPoint &point, CModelBase &model);
  CAnalysisPoint(const CAnalysisPoint &rhs);
  virtual ~CAnalysisPoint();

  bool operator==(const CAnalysisPoint &rhs);
  CAnalysisPoint &operator=(const CAnalysisPoint &rhs);

  void DefaultName(const geo::IPoint &point);

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;
  virtual unsigned int IconId() const;
  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;

  const geo::IPoint &Point() const;
  void Point(const geo::IPoint &point);
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  ACCEPT_GEOMECMODELVISITORS(VisitAnalysisPoint);
};

class CAnalysisPointEntry : public CStorageNodeEntry<CAnalysisPoint> {
public:
  CAnalysisPointEntry(CModelBase &model);

  ACCEPT_GEOMECMODELVISITORS(VisitAnalysisPointEntry);
};

#endif // !defined(AFX_ANALYSISPOINT_H__7015C340_E5D1_4D6D_888E_81864C8160BC__INCLUDED_)
