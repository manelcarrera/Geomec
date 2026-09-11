// WellPointList.h: interface for the WellPointList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPOINTLIST_H__87F2D02F_ED4A_4F62_B8DA_FA4672293403__INCLUDED_)
#define AFX_WELLPOINTLIST_H__87F2D02F_ED4A_4F62_B8DA_FA4672293403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellPoint.h"
#include "WellSchemeInclude.h"

//*******************************************************************
//
//  FILE:       WellPointList.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellPointList
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//
//*******************************************************************

namespace well {

class CWellPathBase;

// ##ModelId=3F7AA15603B1
class CWellPointList : public QObject // , private QList<CWellPoint*>
{

  Q_OBJECT;

public:
  typedef QList<CWellPoint *>::iterator Iterator;
  typedef QList<CWellPoint *>::const_iterator ConstIterator;

  // constructors
  CWellPointList();
  CWellPointList(const CWellPathBase &pWellPath, bool bAutoDelete = true, bool bAllowDuplicateTMD = false);
  virtual ~CWellPointList();
  CWellPointList &operator=(const CWellPointList &rhs);
  CWellPointList(const CWellPointList &rhs);

  QString GetDescription() const;

  CWellPoint *First() const;
  CWellPoint *Last() const;
  CWellPoint *At(int index);
  const CWellPoint *At(int index) const;

  // for these insert/remove functions, duplicates are based on EqualPointExist(uses compareItem) !!!
  // not the pointers
  void InsertNonDuplicatePoints(const CWellPointList &list);
  void RemoveDuplicatePoints(const CWellPointList &list);
  void RemoveNonDuplicatePoints(const CWellPointList &list);
  void RemoveUnDefinedPoints();

  bool AutoDelete() const;
  void AutoDelete(bool autodelete);

  bool AllowDuplicateTMD() const;
  void AllowDuplicateTMD(bool allowduplicate);

  // if duplicate(uses compareItem) returns -1
  int AddPoint(const CWellPoint *point);

  // uses pointer equalty
  int GetIndex(const CWellPoint *point) const; // return -1 if not exists
  bool Exist(const CWellPoint *point) const;

  // look if a equal point exists (uses compareItem)
  bool EqualPointExist(const CWellPoint &point) const;
  bool Exist(const double &tmd) const; // same as above

  // state
  bool AllPointsDefined() const;

  // list operations
  int NrOfPoints() const;

  // iterators
  Iterator begin() const;
  Iterator end() const;
  Iterator GetIteratorAt(const CWellPoint *point) const;

  const CWellPathBase &WellPath() const;
  // CWellPathBase& WellPath();
  void WellPath(CWellPathBase *pWellPath);

  geo::CPoint MaxPoint() const;
  geo::CPoint MinPoint() const;

  QList<CWellPoint *> GetUpperLower(const double &TMD) const;

  void clear();

public slots:

  void SetDescription(QString description);
  void RemovePoint(int index);
  void RemovePoint(const CWellPoint *point);
  void RemoveAllPoints();

protected:
  mutable geo::CPoint m_MaxPoint;
  mutable geo::CPoint m_MinPoint;
  void CalculateMaxMin() const;
  const CWellPathBase *m_pWellPath;
  QString m_Description;

private:
  CWellPointList *This() const;
  QList<CWellPoint *> m_lstPoints;
  bool m_bAutoDelete;
  int compareItems(CWellPoint *item1, CWellPoint *item2);
  bool m_bAllowDuplicateTMD;
};

} // namespace well

#endif // !defined(AFX_WELLPOINTLIST_H__87F2D02F_ED4A_4F62_B8DA_FA4672293403__INCLUDED_)
