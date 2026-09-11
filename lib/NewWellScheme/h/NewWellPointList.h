#ifndef _NewWellPointList_h_
#define _NewWellPointList_h_

#include <QObject>

class CNewWellPoint;
class CNewWellPathBase;

#include "Point.h"

class CNewWellPointList : public QObject {
public:
  typedef QList<CNewWellPoint *>::iterator Iterator;
  typedef QList<CNewWellPoint *>::const_iterator ConstIterator;

  CNewWellPointList();
  CNewWellPointList(const CNewWellPathBase &pWellPath, bool bAutoDelete = true, bool bAllowDuplicateTMD = false);

  CNewWellPointList(const CNewWellPointList &rhs);
  CNewWellPointList &operator=(CNewWellPointList rhs);

  virtual ~CNewWellPointList();

  QString GetDescription() const;

  CNewWellPoint *First() const;
  CNewWellPoint *Last() const;

  CNewWellPoint *At(int index);
  const CNewWellPoint *At(int index) const;

  // for these insert/remove functions, duplicates are based on
  // EqualPointExist (uses compareItem) !!! not the pointers
  void InsertNonDuplicatePoints(const CNewWellPointList &list);
  void RemoveDuplicatePoints(const CNewWellPointList &list);
  void RemoveNonDuplicatePoints(const CNewWellPointList &list);
  void RemoveUnDefinedPoints();

  bool AutoDelete() const;
  void AutoDelete(bool autodelete);

  bool AllowDuplicateTMD() const;
  void AllowDuplicateTMD(bool allowduplicate);

  // if duplicate(uses compareItem) returns -1
  int AddPoint(const CNewWellPoint *point);

  // uses pointer equalty
  int GetIndex(const CNewWellPoint *point) const; // return -1 if not exists
  bool Exist(const CNewWellPoint *point) const;

  // look if a equal point exists (uses compareItem)
  bool EqualPointExist(const CNewWellPoint &point) const;
  bool Exist(const double &tmd) const; // same as above

  // state
  bool AllPointsDefined() const;

  // list operations
  int NrOfPoints() const;

  // iterators
  Iterator begin() const;
  Iterator end() const;
  Iterator GetIteratorAt(const CNewWellPoint *point) const;

  const CNewWellPathBase &WellPath() const;
  void WellPath(CNewWellPathBase *pWellPath);

  geo::CPoint MaxPoint() const;
  geo::CPoint MinPoint() const;

  QList<CNewWellPoint *> GetUpperLower(const double &TMD) const;

  void clear();

  void SetDescription(QString description);
  void RemovePoint(int index);
  void RemovePoint(const CNewWellPoint *point);
  void RemoveAllPoints();

protected:
  void CalculateMaxMin() const;

  mutable geo::CPoint m_MaxPoint;
  mutable geo::CPoint m_MinPoint;

  const CNewWellPathBase *m_pWellPath;
  QString m_Description;

private:
  int compareItems(CNewWellPoint *item1, CNewWellPoint *item2);
  CNewWellPointList *This() const;
  void swap(CNewWellPointList &lhs, CNewWellPointList &rhs);

  QList<CNewWellPoint *> m_lstPoints;
  bool m_bAutoDelete;
  bool m_bAllowDuplicateTMD;
};

#endif // _NewWellPointList_h_
