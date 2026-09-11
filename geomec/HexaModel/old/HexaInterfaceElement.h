#ifndef _HEXAINTERFACEELEMENT_H_
#define _HEXAINTERFACEELEMENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHexaInterfaceElement;
class CHexaInterfaceElementSet : public geo::IElementSet {
  typedef std::map<geo::IPoint *, int> TPointMap;
  TPointMap m_mpPoint;
  std::vector<CHexaInterfaceElement *> m_vcElement;
  std::vector<geo::IPoint *> m_vcPoint;

protected:
  virtual geo::IPoint &PointAt(int nIndex);

public:
  CHexaInterfaceElementSet();

  int RegisterElement(CHexaInterfaceElement &element);

  int RegisterPoint(geo::IPoint &point);

  void Clear();

  virtual const geo::IPoint &Point(int nIndex) const;
  virtual int PointSize() const;

  virtual const geo::IElement &Element(int nIndex) const;
  virtual int ElementSize() const;
  virtual std::vector<int> ElementsAt(const geo::IPoint &point) const;

  // get the node indices connected to the given element
  virtual std::vector<int> Nodes(const geo::IElement &element) const;
  virtual void AssertValid() const;
};

class CHexaInterfaceElement : public geo::IBody {
  CHexaInterfaceElementSet &m_element_set;
  mutable std::vector<geo::IBodyFace *> m_vcFaces;
  int m_points[8];
  const geo::CHexahedron *m_pUpperBody;
  const geo::CHexahedron *m_pLowerBody;
  int m_nIndex;

public:
  // Construction
  CHexaInterfaceElement(CHexaInterfaceElementSet &element_set, const std::vector<geo::IPoint *> &points,
                        const geo::CHexahedron &upperBody, const geo::CHexahedron &lowerBody);

  CHexaInterfaceElement(CHexaInterfaceElementSet &element_set, geo::IPoint &p0, geo::IPoint &p1, geo::IPoint &p2,
                        geo::IPoint &p3, geo::IPoint &p4, geo::IPoint &p5, geo::IPoint &p6, geo::IPoint &p7,
                        const geo::CHexahedron &upperBody, const geo::CHexahedron &lowerBody);

  virtual ~CHexaInterfaceElement();

  // Indexing system
  virtual const geo::IElementSet *IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;

  CHexaInterfaceElement &operator=(const CHexaInterfaceElement &rhs);
  bool operator==(const CHexaInterfaceElement &rhs);

  virtual const geo::IPoint &Point(const int nIndex) const;
  virtual void Point(const int nIndex, const geo::IPoint &pt);
  virtual int NrOfPoints() const;

  virtual const geo::IBodyFace &Face(const int nIndex) const;
  virtual int NrOfFaces() const;
  void ClearFaces();
  void CreateFaces() const;
  virtual double Volume() const;

  virtual bool Contains(const geo::IPoint &point, bool bIncludeEdge) const;
  virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;

  virtual void SetUpperBody(const geo::CHexahedron &body);
  virtual void SetLowerBody(const geo::CHexahedron &body);
  virtual const geo::CHexahedron &GetUpperBody() const;
  virtual const geo::CHexahedron &GetLowerBody() const;

  virtual std::set<geo::CPoint> Intersection(const geo::IPlane &plane) const;
};

typedef std::vector<CHexaInterfaceElement> TInterfaceElement;
typedef TInterfaceElement::iterator TInterfaceElementIt;

#endif /* _HEXAINTERFACEELEMENT_H_*/
