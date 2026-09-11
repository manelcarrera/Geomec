// 2DVertexContainer.h: interface for the C2DVertexContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DVERTEXCONTAINER_H__6A57333A_E562_475F_8DA2_DBCDA7CEE330__INCLUDED_)
#define AFX_2DVERTEXCONTAINER_H__6A57333A_E562_475F_8DA2_DBCDA7CEE330__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelContainer.h"
#include "ipolyline.h"

class C2DVertex;
class C2DSegment;
class C2DModel;

class C2DPolyLine : public IModelObject
{
public:
  // The geometry polyline class ...
  class CPolyLine : public geo::IPolyLine
  {
    typedef std::vector<C2DSegment *> TSegmentVec;
    friend class C2DPolyLine;
    C2DPolyLine& m_polyline;
    TSegmentVec m_vcSegment;
  protected:
    virtual geo::IPoint &PointAt(int nIndex);

  public:
    // Construction / Destruction
    CPolyLine(C2DPolyLine& polyline);
    virtual ~CPolyLine();

    void PushBack(C2DSegment& segment);
    void Insert(int nIndex, C2DSegment& segment);
    void Remove(C2DSegment& segment);
    // int	-> index (-1 when not found)
    // bool -> true (first, second); false(second, first)
    std::pair<int, bool> SegmentIndex(const C2DSegment& segment) const;

    virtual const geo::IPoint &Point(int nIndex) const;
    const C2DVertex& Vertex(int nIndex) const;
    C2DVertex& Vertex(int nIndex);
    virtual int PointSize() const;

    virtual const geo::ILine &Line(int nIndex) const;
    const C2DSegment& Segment(int nIndex) const;
    C2DSegment& Segment(int nIndex);
    virtual int LineSize() const;

    virtual void Swap(int nIndex1, int nIndex2);

    virtual void AssertValid() const;
    virtual std::vector<int> Nodes(const geo::IElement &element) const;
  };

  class CPointInsert
  {
    C2DPolyLine & m_polyline;
    geo::CPoint m_point;
  public:
    CPointInsert(C2DPolyLine& polyline, const geo::IPoint& point);
    bool CanInsert() const;
    void Insert();
  };

private:
  CPolyLine* m_pPolyLine;
public:
  C2DPolyLine(IModelObject& parent);
  C2DPolyLine(const char* name, IModelObject& parent); 
  virtual ~C2DPolyLine();
  
  const CPolyLine& PolyLine() const;
  CPolyLine& PolyLine();

  virtual int geometrySize() const;
  virtual const geo::IObject& geometry(int nIndex) const;

  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();

/*
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode& node, UINT uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
*/
  // Mesh node and mesh elements. NodeSize and EdgeSize are zero when 
  // mesh not valid
  int NodeSize() const;
  int NodeIndex(int nIndex) const;
  const geo::IPoint& Node(int nIndex) const;

  bool canDestroy() const { return true; }
/*	
  int EdgeSize() const;
  const C2DMesher::CEdgeSurrogate& Edge(int nIndex) const;
  void ClearEdges();
/*
  virtual BOOL Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
*/
};

typedef CModelContainer<C2DPolyLine> T2DPolyLineContainer;


#endif // !defined(AFX_2DVERTEXCONTAINER_H__6A57333A_E562_475F_8DA2_DBCDA7CEE330__INCLUDED_)
