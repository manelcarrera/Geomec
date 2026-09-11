#ifndef _INTERFACEBOUNDARY_H_
#define _INTERFACEBOUNDARY_H_

#include "BoundaryBase.h"
#include "BoundaryInterfaceDef.h"
#include "3DHorizon.h"
#include "GeomecModelVisitor.h"

class CBoundaryInterfaceMaterial;
class CDisplacementSupportNode;
class CPressureSupportNode;

namespace geo {
  class CInterfaceElement;
  class CElementGroup;
}

class CInterfaceBoundary : public CBoundaryBase
{
public:
  CInterfaceBoundary(const geo::IPoint& ptMin, 
                     const geo::IPoint& ptMax,
                     CFemAppModel& model,	  
                     BOUNDARY_STATE state = DEFAULT_DEFINED);

  CInterfaceBoundary(const CInterfaceBoundary &rhs);
  ~CInterfaceBoundary();
  bool operator==(const CInterfaceBoundary& rhs) const;
  CInterfaceBoundary& operator=(const CInterfaceBoundary& rhs);

  virtual bool CreateInterfaces() const;	
  void CreateInterfaces(bool bVal);
  const CBoundaryInterfaceDef& InterfaceDefinition() const;
  CBoundaryInterfaceDef& InterfaceDefinition();
  void ToggleInterfaces();

  // Interface elements
  virtual int InterfaceNodeSize() const;
  virtual const geo::INode& InterfaceNode( int nIndex ) const;

  const geo::CElementGroup *InterfaceElements() const;
  void AddInterfaceElement( geo::CInterfaceElement& interface_element );

  virtual bool HasInterfaces() const;
  bool IsBoundaryInterface(const geo::CInterfaceElement& iface) const;

  virtual const CBoundaryInterfaceMaterial& InterfaceMaterial(const geo::CInterfaceElement& iface) const;

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  virtual const C3DHorizon& GetTopHorizon() const = 0;
  virtual const C3DHorizon& GetBottomHorizon() const = 0;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;

  const CDisplacementSupportNode &DisplacementSupportNode() const { return *m_pDisplacementSupportNode; }
  const CPressureSupportNode &PressureSupportNode() const { return *m_pPressureSupportNode; }

  ACCEPT_GEOMECMODELVISITORS(VisitInterfaceBoundary);

protected:
  void LoadSupportNodes(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  void LoadPre412Stream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

private:
  // Values for the interface, if required. Are initiated with large value. 1E6 = default
  bool m_bCreateInterfaces;
  
  mutable std::vector<const geo::INode*> m_vcInterfaceNode; 
  geo::CElementGroup *m_pInterfaceElements;

  CBoundaryInterfaceDef* m_pBoundaryInterfaceDef;

  typedef std::map<const geo::CInterfaceElement*, CBoundaryInterfaceDef::TBoundarySurface> TInterfaceElement2BoundarySurfaceMap;
  mutable TInterfaceElement2BoundarySurfaceMap m_mpInterfaceElement2BoundarySurface;

  typedef std::map<const geo::IFace*, CBoundaryInterfaceDef::TBoundarySurface> TFace2BoundarySurfaceMap;

  CDisplacementSupportNode *m_pDisplacementSupportNode;
  CPressureSupportNode *m_pPressureSupportNode;

private:
  void LoadProperties(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  void CreateInterfaceElement2BoundarySurfaceMap() const;
  void CreateChildren();
};

#endif // _INTERFACEBOUNDARY_H_
