// PointSet.h: interface for the CPointSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTSET_H__8F9C1EB0_6EFE_4CA0_8F9F_381FE985F716__INCLUDED_)
#define AFX_POINTSET_H__8F9C1EB0_6EFE_4CA0_8F9F_381FE985F716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExportFormat.h"
#include "IPointSet.h"
#include "ValueComponent.h"
#include "ValueType.h"

namespace geo {
class CConvexHull;
class CConvexHull_2D;
class CCylindricHull;
} // namespace geo

#include "NearestNeighbour.h"

// class CDistributedQuantity;
// ##ModelId=3C74E3EB0010
class CPointSet : public IPointSet {
public:
  class CCoordinateType : public CValueType {
  public:
    class CCoordinateComponent : public CValueComponent {
    public:
      CCoordinateComponent(const QString &sName, CCoordinateType &value_type, int uComponentIndex);
      CCoordinateComponent(const QString &sName, CCoordinateType &value_type, CNodalValueSet &value_set,
                           int uComponentIndex);
      virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
      virtual double FieldFactor() const;
      virtual QString ExportLabel() const;
      double ValueAt(int nIndex, CQuantity::UNIT unit) const;
      virtual bool IsInRange(const IValueSet & /*value_set*/) const { return true; }
      virtual geo::CValue RangeMin(const CQuantity::UNIT /*unit*/ = CQuantity::SI_UNIT) const { return geo::CValue(); }
      virtual geo::CValue RangeMax(const CQuantity::UNIT /*unit*/ = CQuantity::SI_UNIT) const { return geo::CValue(); }
    };

  private:
    mutable geo::CNearestNeighbour m_data;
    CPointSet &m_point_set;
    bool m_bDestroy;
    void Update() const;

  public:
    CCoordinateType(CPointSet &point_set, IPointSet::DIMENSION dim, bool bCreateValueSet);
    ~CCoordinateType();
    IPointSet::DIMENSION Dimension() const;
    void Dimension(IPointSet::DIMENSION dim);

    int PointSize() const;
    const geo::IPoint &Point(int nIndex) const;

    void SetPoint(int nIndex, const geo::IPoint &pt);

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;

    bool PushBack(const geo::IPoint &point);

    void InvertZ();

    std::vector<int> NearestNeighbourIndex(const geo::IPoint &point) const;
    geo::CNearestNeighbour &NearestNeighbour();
    const geo::CNearestNeighbour &NearestNeighbour() const;
    virtual bool CanDestroy() const;
    void DestroyEnable(bool bCanDestroy);
    void Invalidate();
    virtual void PrepareMapping();
  };

  void pointSetType(TPointSetType pointSetType, bool bChangeHull = false);
  virtual TPointSetType pointSetType() const { return m_pointSetType; }

  CCoordinateType *m_pValueType;
  std::vector<const geo::CElementPoint *> m_vcElementPoints; // remember element points if provided
                                                             // Container for vertices

  mutable geo::CConvexHull *m_pHull_3D;
  mutable geo::CConvexHull_2D *m_pHull_2D;
  mutable geo::CCylindricHull *m_pHull_Cylindric;
  mutable geo::CPtrArray<geo::CLine> m_display_hull;
  mutable geo::CPtrArray<geo::IObject> m_display_cylindrichull;
  bool HasHull() const;
  bool CreateHull(CPointSetEntry::CreateHullCallback cb = 0) const;
  void DestroyHull();
  void LoadPointSetData(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

private:
  TPointSetType m_pointSetType;
  std::vector<CValueType *> SharedValueTypes(const CPointSet &source) const;
  void GetValueTypesNoAveraging(CPointSet &target, const CPointSet &ptset, std::vector<CValueType *> vcTypes) const;

  geo::CCylindricHull *CreateCylindricHull(bool &bCanceled) const;
  geo::CConvexHull_2D *Create2DHull(bool &bCanceled, int nDisabledDim) const;
  geo::CConvexHull *Create3DHull(bool &bCanceled) const;

private:
  mutable geo::CPoint m_bbox_min;
  mutable geo::CPoint m_bbox_max;

  double m_ResizeHull;
  void BBox(geo::IPoint &min, geo::IPoint &max) const;                          // mantis 2401 wjrx
  bool InBBox(const geo::IPoint &pt, geo::IPoint &min, geo::IPoint &max) const; // mantis 2401 wjrx
  bool DoSample(CPointSet &sampledPointSet, double boxSize_north, double boxSize_east, double boxSize_depth,
                IProgressBase &progress);

public:
  void ResizeHullValue(double d);
  double ResizeHullValue() const { return m_ResizeHull; }

  typedef std::vector<double> TLineVec;

  // Add a Coordinate point to the PointSet
  int PushBack(const std::vector<double> &vcRow, bool bConvertValue = false);
  int PushBack(const std::vector<geo::CValue> &vcRow, bool bConvertValue = false);
  int PushBack(const geo::IPoint &point, const std::vector<double> &vcRow, bool bConvertValue = false);

  // Consruction ...
  CPointSet(CFemAppModel &model);

  CPointSet(const QString &sName, CFemAppModel &model, int nValueSetSize, DIMENSION dim,
            bool bCreateValueSetForCoordinates = true, bool bLinkToEntry = true);
  CPointSet(const QString &strName, CFemAppModel &model, const std::vector<TLineVec> vcData, const DIMENSION dim,
            bool bCreateValueSetForCoordinates = true);
  CPointSet(const CPointSet &rhs);
  virtual ~CPointSet();

  // Point Access
  virtual const geo::IPoint &PointAt(int nIndex) const;
  virtual std::vector<int> PointAt(const geo::IPoint &pt) const;
  void SetPoint(int nIndex, const geo::IPoint &pt);
  virtual int PointSize() const;
  virtual bool PointInConvexHull(const geo::IPoint &pt) const;

  // ##ModelId=3C74E3EB011C
  virtual const geo::IObject &DisplayList(int nIndex) const;
  virtual int DisplayListSize() const;

  // ##ModelId=3C74E3EB012D
  virtual void OnNewNeighbour(const CGraphNode &node);

  // ##ModelId=3C74E3EB0138
  bool operator==(const CPointSet &rhs) const;
  CPointSet &operator=(const CPointSet &rhs);
  // Dimension handling ...
  virtual DIMENSION Dimension() const;
  void Dimension(IPointSet::DIMENSION dim);

  virtual void OnCoordinateUnitChanged(CQuantity::UNIT old_unit, CQuantity::UNIT new_unit);

  // Function for destroying vertex sets ...
  virtual bool CanDestroy() const;

  // ValueType for coordinate
  CCoordinateType &Coordinates();
  const CCoordinateType &Coordinates() const;

  // Drag and drop properties
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  // Unit
  virtual void CoordinateUnit(CQuantity::UNIT unit);
  virtual CQuantity::UNIT CoordinateUnit(void) { return IPointSet::CoordinateUnit(); }

  // Our Axis invertor ....
  virtual void ZAxis(Z_AXIS axis);

  // Derived functions ...
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;

  bool EqualPoints(const CPointSet &set) const;
#ifdef _DEBUG
  virtual void AssertValid() const;
#endif //_DEBUG

  // Save and load
  virtual bool Empty() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  // merging of pointsets, ptset is merged with this, resulting in target
  void Merge(CPointSet &target, const CPointSet &ptset) const;

  virtual void Sample(const QString &name, const double blockSizeNorth, const double blockSizeEast,
                      const double blockSizeDepth);
  virtual void Export(const IExportFormat::CExportArg &arg, const QString &fileName);

  virtual bool PrepareMapping(const geo::IElementSet *);

  ACCEPT_GEOMECMODELVISITORS(VisitPointSet);

private:
  mutable bool m_bTriedCreatingHull;
};

#endif // !defined(AFX_POINTSET_H__8F9C1EB0_6EFE_4CA0_8F9F_381FE985F716__INCLUDED_)
