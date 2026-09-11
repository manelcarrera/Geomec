// 2DCompartment.h: interface for the C2DBody class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DCOMPARTMENT_H__ACAF06E2_53D7_4D30_B362_BD4E45D636D6__INCLUDED_)
#define AFX_2DCOMPARTMENT_H__ACAF06E2_53D7_4D30_B362_BD4E45D636D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "ipolygon.h"

class CRegion;
class C2DVertex;
class C2DSegment;
class C2DBody : public IModelObject
{
public:
  class CPolygon : public geo::IPolygon
  {
    std::vector<const C2DVertex*> m_vcVertex;
  public:
    CPolygon();
    
    virtual int NrOfPoints() const;
    virtual const geo::IPoint &Point(int nIndex) const;
    const C2DVertex& Vertex(int nIndex) const;
    virtual void Point(int nIndex, const geo::IPoint &pt);

    int SegmentSize() const;
    const C2DSegment& Segment(int nIndex) const;

    // Clear and pushback
    void Clear();
    void PushBack(const C2DVertex& point);
    virtual int Order() const { return 1; }
  };
private:
  geo::IElementSet *m_pElementSet;
  CPolygon m_polygon;
  void CalculateIdentifier();
  double m_dElementSize;
  bool m_bElementSize;
  double m_dMaxElementSize;
  std::set<IModelObject*>	m_identifier;
public:
  typedef std::vector<int> TIndexVec;
  typedef std::map<int, C2DVertex*> TVertexMap;

  // Construction ...
  C2DBody(IModelObject& parent);
  C2DBody(IModelObject& parent, const TIndexVec& vcVertex, const TVertexMap& mpVertex);
  C2DBody(const C2DBody& rhs);

  bool operator==(const C2DBody& rhs) const;
  C2DBody& operator=(const C2DBody& rhs);

  int Resemblance(const std::set<IModelObject*>& identifier) const;

  std::set<IModelObject*> Indentifier() const {	return m_identifier;}
  void Indentifier(const std::set<IModelObject*>& identifier) { m_identifier = identifier; }

/*
  virtual UINT IconId() const;
  virtual UINT TypeId() const { return 0; }
  virtual CString TypeName() const { return "Compartments"; }
*/
  void SetElementSet(geo::IElementSet &elementset);
  void ClearElementSet();

  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();

  virtual int geometrySize() const;
  virtual const geo::IObject& geometry(int nIndex) const;
/*
  virtual bool MouseRelease(TFrame& frame, TButtonState state, TButtonState button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);
  virtual bool Properties();
*/
  const CPolygon& Polygon() const;
  void SetPolygon(const std::vector<const C2DVertex*>& vcVertex);

  bool ElementSizeEnabled() const;
  void ElementSize(const double& dSize);
  void DisableElementSize();
  const double& ElementSize() const;
  const double& MaxElementSize() const;
  void CalculateElementSize();

  void OnCoarsen();
  void OnRefine();
  bool CanCoarsen() const;
  bool CanRefine() const;
/*
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
*/
};

typedef CModelContainer<C2DBody> T2DBodyContainer;


#endif // !defined(AFX_2DCOMPARTMENT_H__ACAF06E2_53D7_4D30_B362_BD4E45D636D6__INCLUDED_)
