#ifndef _NONMESHEDSURFACE_H_
#define _NONMESHEDSURFACE_H_

#include "GeomecModelVisitor.h"
#include "Result.h"
#include "TriSurface.h"
#include "colornode.h"

class CSurfaceBase;
class CModelBase;
class CGraphNode;
class CNonMeshedSurfaceParametersNode;
class CNonMeshedSurfacePressure;

class CNonMeshedSurface : public CColorNode {
public:
  class CGeoSurface : public geo::ISurface {
  public:
    CGeoSurface(const geo::CTriSurface &surface, const CNonMeshedSurface &parent);
    virtual ~CGeoSurface();

    virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint &p) const;
    virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint &p) const;
    virtual const geo::CTriangle &Face(int nIndex) const;
    virtual int FaceSize() const;
    virtual const geo::CPoint &Point(int nIndex) const;
    virtual void Clear();
    virtual int PointSize() const;
    virtual std::vector<int> Nodes(const geo::IElement &element) const;

    const CNonMeshedSurface &Parent() const;

  protected:
    virtual geo::CPoint &PointAt(int nIndex);

  private:
    std::vector<geo::CTriangle *> m_vcTriangle;
    std::vector<geo::CPoint> m_vcPoint;
    typedef std::map<const geo::IPoint *, geo::CPtrArray<geo::IFace>> TPointToFacesMap;
    TPointToFacesMap m_mpPointToFaces;
    const CNonMeshedSurface &m_parent;
  };

public:
  CNonMeshedSurface(CFemAppModel &model);
  CNonMeshedSurface(CSurfaceBase &surface);

  virtual ~CNonMeshedSurface();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual QString TypeName() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;

  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  const CNonMeshedSurfaceParametersNode &ParametersNode() const;
  const CNonMeshedSurfacePressure &Pressure() const;

  int FaceSize() const;
  const geo::IFace &Face(int nIndex) const;

  ACCEPT_GEOMECMODELVISITORS(VisitNonMeshedSurface);

protected:
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

private:
  const CSurfaceBase *m_pSurface;
  mutable CGeoSurface *m_pGeoSurface;
  CNonMeshedSurfaceParametersNode *m_pParametersNode;
  CNonMeshedSurfacePressure *m_pPressure;
};

class CNonMeshedSurfaceEntry : public CStorageNodeEntry<CNonMeshedSurface> {
public:
  CNonMeshedSurfaceEntry(CFemAppModel &model);

  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;

  ACCEPT_GEOMECMODELVISITORS(VisitNonMeshedSurfaceEntry);
};

#endif // _NONMESHEDSURFACE_H_
