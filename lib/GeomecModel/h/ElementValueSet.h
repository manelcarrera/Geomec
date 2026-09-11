// ElementValueSet.h: interface for the CElementValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTVALUESET_H__038C1A16_C5EB_4563_8A26_905BC7B2001D__INCLUDED_)
#define AFX_ELEMENTVALUESET_H__038C1A16_C5EB_4563_8A26_905BC7B2001D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ElementSet.h"

#include "ValueSet.h"

class IElementSet;

#include "GeomecIValueSet.h"

class CElementValueSet : public IValueSet {
  geo::CValueSet m_value_set;

public:
  typedef std::map<int, double> TElementAverageVec;

private:
  IElementSet *m_pElementSet;

  mutable geo::CValue m_dMin; // Cache min
  mutable geo::CValue m_dMax; // Cache max
  mutable bool m_bDirty;      // Dirty flag

  void CalculateProperties() const;
  void CalculateAverage();

  TValueVec LoadValueVec(TSTREAM &stream, const CStreamVersion &version);
  void SaveValueVec(const TValueVec &data, TSTREAM &stream);
  TValueVec ValueElement(int nIndex, geo::IParallelInitializationCallback *cb) const;

public:
  // Construction
  CElementValueSet(IElementSet &set);
  CElementValueSet(const QString &sName, CQuantity::UNIT unit, IElementSet &set);
  CElementValueSet(const CElementValueSet &rhs);
  virtual ~CElementValueSet();

  virtual CElementValueSet *clone();

  void Clear(); // Clear valueset

  // Compare and assignment
  bool operator==(const CElementValueSet &rhs) const;
  CElementValueSet &operator=(const CElementValueSet &rhs);

  // Elementset access
  const IElementSet &ElementSet() const;
  IElementSet &ElementSet();

  // Tree visualisation
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;

  int ValueSize() const;
  TValueVec ValueAt(int nIndex, geo::IParallelInitializationCallback *cb) const;

  // Min, max enz
  virtual TValue Min() const;
  virtual TValue Max() const;

  // Present Point
  virtual TValue ValuePoint(const geo::IPoint &point, geo::IParallelInitializationCallback *cb) const;

  // Present Element
  virtual TValueVec ValueElement(const geo::IElement &element, geo::IParallelInitializationCallback *cb) const;

  int PushBack(const double &value);
  int PushBack(const TValueVec &value);
  int PushBack(const std::vector<double> &value);

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  const geo::CValueSet &getValueSet() const;
  geo::CValueSet &getValueSet();

  ACCEPT_GEOMECMODELVISITORS(VisitElementValueSet);
};

#endif // !defined(AFX_ELEMENTVALUESET_H__038C1A16_C5EB_4563_8A26_905BC7B2001D__INCLUDED_)
