// GocadElementSet.h: interface for the CGocadElementSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADELEMENTSET_H__EDEE9A44_278B_462a_923C_9E029AC519F1__INCLUDED_)
#define AFX_GOCADELEMENTSET_H__EDEE9A44_278B_462a_923C_9E029AC519F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ElementSet.h"
#include "GeomecModelVisitor.h"
#ifdef SKUA_NEW
namespace gm_skua
{
struct SKUAParseData;
}
#else
#include "GocadData.h"
#endif


class CGocadElementSet : public CElementSet
{
public:
  typedef std::vector <geo::CPoint> TPointVec;
  typedef std::vector <int> TElement;
  typedef std::vector <TElement> TElementVec;

#ifdef SKUA_NEW
  CGocadElementSet(const QString& sName, eElementType type,
    const TPointVec& vcPoint, const TElementVec& vcElement, CFemAppModel& model,
    std::vector<const gm_skua::SKUAParseData *>& solids);
#else
  CGocadElementSet(const QString& sName, eElementType type,
    const TPointVec& vcPoint, const TElementVec& vcElement, CFemAppModel& model,
    std::vector <QSharedPointer <CGocadData::CTSolid> >& solids);
#endif
  CGocadElementSet(CFemAppModel& model);
  virtual ~CGocadElementSet();

  bool canImportInGocadModel() const;
  bool importInGocadModel();
#ifdef SKUA_NEW
  bool importInGocadModel(bool (*selectGocadSolidDlg)(
    const std::vector<const gm_skua::SKUAParseData *>& solids,
    std::vector<int>& vcSolids));
#else
  bool importInGocadModel(bool(*selectGocadSolidDlg)(
    const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
    std::vector<int>& vcSolids)); 
#endif

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version,
    TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitGocadElementSet);

private:
  CGocadElementSet(const CGocadElementSet& rhs);
  CGocadElementSet& operator = (const CGocadElementSet& rhs);

#ifdef SKUA_NEW
  std::vector<const gm_skua::SKUAParseData *> m_Solids;
#else
  std::vector <QSharedPointer <CGocadData::CTSolid> > m_Solids;
#endif
};

#endif // !defined(AFX_GOCADELEMENTSET_H__EDEE9A44_278B_462a_923C_9E029AC519F1__INCLUDED_)
