// FormationVolume.h: interface for the FormationVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONVOLUME_H__6019DE30_404A_47EA_9D49_31302DC009FD__INCLUDED_)
#define AFX_FORMATIONVOLUME_H__6019DE30_404A_47EA_9D49_31302DC009FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoordinateMap.h"

class C3DFormation;

#include "FormationBase.h"
#include "GeomecModelVisitor.h"
#include "IParallelInitializationCallback.h"

// A formation volume points a CBodyGroup in the mesher. When the m_nGroupIndex == -1 the formation
// volume is undefined. The formation volume is still in the tree, but cannot displayed by OpenGL.
class CFormationVolume : public IFormationElementSet {
  geo::CBodyGroup *m_BodyGroup;

  friend class CValueMapper;
  friend class CThicknessParallelInitializationCallback;
  typedef struct {
    geo::CValue value;
    geo::CPoint ptTop;
    geo::CPoint ptBottom;
  } TThicknessValue;
  typedef geo::CCoordinateMap<geo::CPoint, TThicknessValue> TThicknessCache;
  mutable TThicknessCache m_mpThicknessCache;

  void AddToCache(TThicknessCache *cache) const;

  int BodyGroupIndex(const geo::IMesh &mesh, const geo::CBodyGroup &group) const;

public:
  // Construction ...
  CFormationVolume(C3DFormation &formation, geo::CBodyGroup &group, bool bDoLink = true);
  CFormationVolume(C3DFormation &formation);
  CFormationVolume(const CFormationVolume &rhs);
  bool operator==(const CFormationVolume &rhs) const;
  CFormationVolume &operator=(const CFormationVolume &rhs);

  // IconId's enz.
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  // Check the valid function before you check the volume or the elementset function function.
  bool Valid() const;
  const geo::CBodyGroup &Volume() const;
  geo::CBodyGroup &Volume();
  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();
  virtual int DisplayListSize() const;

  // The set function ... Reseting is done by the mesher ...
  bool Volume(const geo::CBodyGroup &group);

  // Element set functions
  virtual DIMENSION Dimension() const;

  // Check for the mesher ...
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual bool PointInConvexHull(const geo::IPoint &pt) const;

  virtual geo::CValue ThicknessAt(const geo::IPoint &pt, geo::IPoint &ptTop, geo::IPoint &ptBottom,
                                  geo::IParallelInitializationCallback *cb) const;

  void clearThicknessCache();

  ACCEPT_GEOMECMODELVISITORS(VisitFormationVolume);
};

#endif // !defined(AFX_FORMATIONVOLUME_H__6019DE30_404A_47EA_9D49_31302DC009FD__INCLUDED_)
