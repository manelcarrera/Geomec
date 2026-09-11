// IPointSet.h: interface for the IPointSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOINTSET_H__95BFBCA9_1723_488D_A948_23A4E7628DC5__INCLUDED_)
#define AFX_IPOINTSET_H__95BFBCA9_1723_488D_A948_23A4E7628DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The IPointSet interfaces represents a set of points in 2D or 3D space. A NodalValueSet could
// be attached to the pointset so a value is defined for every point. The NodalValueSet is
// derived from IValueSet. The user can build properties like pressure, stresses etc. from the
// IValueSets by attaching the IValueSet to CValueTypes. The valuetype defines a unit and a
// unit conversion factor for IValueSet set. The RpnValueSet can also attached to the IPointSet

class IUnitConversion;
class CRpnValueSet;
class CNodalValueSet;
class IValueSet;
class CValueType;
class CModelBase;
class IValueComponentBase;

#include "CacheManager.h"
#include "GeomecModelVisitor.h"
#include "SingleQuantity.h"
#include "colornode.h"

class IPointSet : public CColorNode {
  friend class CNodalValueSet;
  friend class CRpnValueSet;

public:
  enum DIMENSION { DIM_1D = 1, DIM_2D = 2, DIM_3D = 3 };
  enum Z_AXIS { Z_UP, Z_DOWN };
  typedef std::vector<const IValueComponentBase *> TDataVec;

private:
  typedef std::map<std::string, IUnitConversion *> TUnitConversionMap;
  typedef std::vector<CNodalValueSet *> TNodalValueSetVec;
  typedef std::vector<CRpnValueSet *> TRpnValueSetVec;
  TUnitConversionMap m_mpUnitConversion;
  TNodalValueSetVec m_vcNodalValueSet;
  TRpnValueSetVec m_vcRpnValueSet;
  CQuantity::UNIT m_value_unit;
  Z_AXIS m_zaxis;
  QString m_guid;

protected:
  CQuantity::UNIT m_coordinate_unit;
  void CoordinateUnitEx(CQuantity::UNIT unit); // Doesn't overide
public:
  IPointSet(const QString &sName, CFemAppModel &model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit);
  IPointSet(const IPointSet &rhs, CModelBase &model, TPROGRESS &progress);
  IPointSet(CFemAppModel &model);
  IPointSet(const IPointSet &rhs);
  virtual ~IPointSet();

  virtual bool HasElements() const { return false; }

  // Compare and assignment
  bool operator==(const IPointSet &rhs) const;
  IPointSet &operator=(const IPointSet &rhs);

  // Unit conversion
  virtual size_t UnitConversionSize() const;
  virtual void AddUnitConversion(const std::string &name, IUnitConversion *conversion);
  virtual void RemoveUnitConversion(const std::string &name);
  virtual void RemoveAllUnitConversions();
  virtual bool AnyUnitConversionDefined() const;
  virtual const IUnitConversion *UnitConversion(const std::string &name) const;

  // Point Access
  virtual int PointSize() const = 0;
  virtual const geo::IPoint &PointAt(int nIndex) const = 0;
  virtual std::vector<int> PointAt(const geo::IPoint &pt) const = 0;
  virtual bool PointInConvexHull(const geo::IPoint &pt) const = 0;
  virtual bool ElementInConvexHull(const geo::IElement &element) const;

  // ValueSet
  virtual int ValueSetSize() const;
  virtual const IValueSet &ValueSet(int nIndex) const;
  virtual IValueSet &ValueSet(int nIndex);

  // NodalValueSet
  virtual int NodalValueSetSize() const;
  virtual const CNodalValueSet &NodalValueSet(int nIndex) const;
  virtual CNodalValueSet &NodalValueSet(int nIndex);
  int AddNodalValueSet();

  // RpnValueSet
  virtual int RpnValueSetSize() const;
  virtual const CRpnValueSet &RpnValueSet(int nIndex) const;
  virtual CRpnValueSet &RpnValueSet(int nIndex);

  // Function to enum the value types ...
  std::vector<CValueType *> ValueTypes() const;

  // Graph Notification
  virtual void OnNeighbourDeleted(const CGraphNode &item);

  // Unit for coordinates
  virtual void CoordinateUnit(CQuantity::UNIT unit);
  virtual void CoordinateInverseUnit(CQuantity::UNIT unit);
  CQuantity::UNIT CoordinateUnit() const;

  virtual void ValueUnit(CQuantity::UNIT unit);
  CQuantity::UNIT ValueUnit(bool bForceStoredValue = false) const;

  virtual void OnCoordinateUnitChanged(CQuantity::UNIT old_unit, CQuantity::UNIT new_unit) = 0;

  // Dimension of pointset
  virtual DIMENSION Dimension() const { return DIM_1D; }

  // Direction of the axis
  Z_AXIS ZAxis() const;
  virtual void ZAxis(Z_AXIS axis);

  // Saving and loading
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  bool Overlap(const IPointSet &point_set) const;

  // Connectivity
  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;

  enum TPointSetType { POINTSET = 0, WELLPATH = 1, INPUT = 2, TIME_DEPTH };

  // wellpath point set // waij TFS 92430
  virtual TPointSetType pointSetType() const { return POINTSET; }

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  bool isDrawable() const;

  bool isUsed() const;

  ACCEPT_GEOMECMODELVISITORS(VisitIPointSet);
};

class CPointSetEntry : public CGraphEntryTemp<IPointSet> {
  CCacheManager m_cache_manager;

public:
  typedef std::set<IPointSet *, CGraphNode::CPtrLess> TNodeSet;
  typedef std::set<IPointSet *, CGraphNode::CLess> TSortedNodeSet;
  typedef std::map<int, IPointSet *> TIndexMap;
  typedef std::map<int, std::set<CGraphNode *>> TDelayedLinkMap;
  typedef IPointSet value_type;

  CPointSetEntry(int nEntryId, unsigned int uIconId, const QString &strName, CFemAppModel &model)
      : CGraphEntryTemp<IPointSet>(nEntryId, uIconId, strName, model), m_hullCB(0) {}

  CPointSetEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model)
      : CGraphEntryTemp<IPointSet>(nEntryId, uIconId, uNameId, model), m_hullCB(0) {}

  void DestroyCaches();

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint = Default);

  typedef void (*CreateHullCallback)();
  void SetCreateHullCallback(CreateHullCallback cb);

  virtual bool CanCreateAllHulls() const;
  void CreateAllHulls();

  bool CanCopyAllWellpaths() const;
  void CopyAllWellpaths();

  bool CanDeleteAllUnusedPointsets() const;
  void DeleteAllUnusedPointsets();

  ACCEPT_GEOMECMODELVISITORS(VisitPointSetEntry);

private:
  CreateHullCallback m_hullCB;
};

// Define a entry type ...
typedef CPointSetEntry TPointSetEntry;

#endif // !defined(AFX_IPOINTSET_H__95BFBCA9_1723_488D_A948_23A4E7628DC5__INCLUDED_)
