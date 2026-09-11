#ifndef _NONMESHEDSURFACEPARAMETERSNODE_H_
#define _NONMESHEDSURFACEPARAMETERSNODE_H_

#include "StorageNode.h"

#include "AngleQuantity.h"
#include "CohesionQuantity.h"
#include "GeomecModelVisitor.h"

class CNonMeshedSurface;
class CValueType;

namespace geo {
class IPoint;
class IElement;
} // namespace geo

class CNonMeshedSurfaceParametersNode : public CStorageNode {
public:
  CNonMeshedSurfaceParametersNode(CNonMeshedSurface &surface);
  CNonMeshedSurfaceParametersNode(const CNonMeshedSurfaceParametersNode &rhs);

  CNonMeshedSurfaceParametersNode &operator=(const CNonMeshedSurfaceParametersNode &rhs);
  bool operator==(const CNonMeshedSurfaceParametersNode &rhs) const;

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint = Default);

  virtual bool CanConnectItem(const CGraphNode &item) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  // Constant settings for cohesion and friction
  const CCohesionQuantity &Cohesion() const;
  const CAngleQuantity &FrictionAngle() const;

  CCohesionQuantity &Cohesion();
  CAngleQuantity &FrictionAngle();

  // Distributed settings for cohesion and friction
  bool DistributedCohesion() const;
  bool DistributedFriction() const;
  bool CohesionExtrapolated() const;
  bool FrictionExtrapolated() const;
  void CohesionExtrapolated(bool b);
  void FrictionExtrapolated(bool b);

  double Cohesion(const geo::IPoint &point, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  std::vector<double> Cohesion(const geo::IElement &element, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  double FrictionAngle(const geo::IPoint &point, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  std::vector<double> FrictionAngle(const geo::IElement &element,
                                    const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  const CNonMeshedSurface &Surface() const;

  ACCEPT_GEOMECMODELVISITORS(VisitNonMeshedSurfaceParametersNode);

private:
  CNonMeshedSurface &m_surface;
  bool m_bCohesionExtrapolated;
  bool m_bFrictionExtrapolated;
  CCohesionQuantity m_Cohesion;
  CAngleQuantity m_FrictionAngle;
  const CValueType *m_pValueTypeCohesion;
  const CValueType *m_pValueTypeFriction;
};

#endif // _NONMESHEDSURFACEPARAMETERSNODE_H_
