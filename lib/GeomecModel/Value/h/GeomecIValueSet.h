// GeomecIValueSet.h: interface for the IValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVALUESET_H__52F18128_FADA_43C9_ADAA_599F2FFD3BFC__INCLUDED_)
#define AFX_IVALUESET_H__52F18128_FADA_43C9_ADAA_599F2FFD3BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecTime.h"
#include "SingleQuantity.h"
#include "StorageNode.h"
#include "Value.h"
#include "ivaluecomponent.h"

// The IValueSet is the base class for value containers attached to point- or element
// set. There are three types of valuesets. NodalV

class IPointSet;
class CValueComponent;

#include "GeomecModelVisitor.h"
#include "SingleQuantity.h"

class IValueSet : public CStorageNode {
  IPointSet *m_pPointSet;        // Reference to corresponding pointset
  CValueComponent *m_pComponent; // Attached component
  CQuantity::UNIT m_unit;        // Current unit
  CGeomecTime m_Time;

public:
  typedef geo::CValue TValue;
  typedef IValueDomainScalar::TValueVec TValueVec;

  // Construction
  IValueSet(IPointSet &set);
  IValueSet(const QString &sName, CQuantity::UNIT unit, IPointSet &set);
  IValueSet(const IValueSet &rhs);
  virtual ~IValueSet();

  virtual IValueSet *clone() = 0;

  // PointSet Access
  const IPointSet &PointSet() const;
  IPointSet &PointSet();

  // Compare and assignment
  bool operator==(const IValueSet &rhs) const;
  IValueSet &operator=(const IValueSet &rhs);

  void FileIndex(int iIndex);

  // Value component
  const CValueComponent *Component() const;
  CValueComponent *Component();

  // Graph Notification
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &item);

  // Time
  inline CGeomecTime &Time() { return m_Time; }
  inline const CGeomecTime &Time() const { return m_Time; }
  inline void Time(int year, int month) { m_Time.Set(year, month); }

  // Min, max enz
  virtual TValue Min() const = 0;
  virtual TValue Max() const = 0;

  // Present Point and Element for the sets
  virtual TValue ValuePoint(const geo::IPoint &pt, geo::IParallelInitializationCallback *cb) const = 0;
  virtual TValueVec ValueElement(const geo::IElement &element, geo::IParallelInitializationCallback *cb) const = 0;

  CQuantity::UNIT Unit() const;
  void Unit(CQuantity::UNIT unit);

  // Save and load
  virtual bool Empty() const;

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  ACCEPT_GEOMECMODELVISITORS(VisitIValueSet);
};

#endif // !defined(AFX_IVALUESET_H__52F18128_FADA_43C9_ADAA_599F2FFD3BFC__INCLUDED_)
