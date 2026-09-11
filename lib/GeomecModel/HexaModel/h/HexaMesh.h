// HexaMesh.h: interface for the CHexaMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXAMESH_H__8E59A0C1_6F38_4751_B8CC_7028EDA779BA__INCLUDED_)
#define AFX_HEXAMESH_H__8E59A0C1_6F38_4751_B8CC_7028EDA779BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshBase.h"

class CHexaModel;
class CHexaFormation;
class CHexaHorizon;
class CHexaMeshRegionBase;
class CHexaMesher;
class IHexaMesher;

namespace hmm
{
class CHexaMetaMesh;
}

namespace geo {
  class CInterfaceElement;
}

#include "IMesh3D.h"
#include "BodyGroup.h"
#include "CoordinateMap.h"
#include "CoordinateSet.h"
#include "GeomecModelVisitor.h"

#define TOP		0
#define BOTTOM	1
#define FRONT	2
#define BACK	3
#define LEFT	4
#define RIGHT	5

namespace geo {
  class CNode;
}

class CHexaMesh : public CMeshBase 
{
  friend class CHexaMesher;
  friend class CHexaMesherNG;
  friend class hmm::CHexaMetaMesh;

	typedef std::map<const geo::CInterfaceElement*, CHexaHorizon*> TInterfaceMap;
	TInterfaceMap m_mpInterfaces;

  IHexaMesher *m_pMesher;
	geo::CMesh3D m_mesh;
	bool m_bReMesh;

  typedef std::map<const geo::INode*, const geo::INode*> TBoundaryNodeMap;
  TBoundaryNodeMap m_mpMeshNode2BoundaryNode;
  TBoundaryNodeMap m_mpBoundaryNode2MeshNode;

	// Internal functions
	CHexaFormation &GetTopFormation() const;
	CFormationBase *InternalNextFormation(const CFormationBase *pFormation) const;
	void CreateSupport();
	void CreateSupport(IProgressBase& progress);

	// Save and load functions
	void SaveSlipHorizon(const CHexaHorizon& horizon, TSTREAM& stream, TPROGRESS& progress);
	void LoadSlipHorizon(CHexaHorizon& horizon, TSTREAM& stream, TPROGRESS& progress);
	void LoadSideSurface(geo::CBodyGroup::CSideSurface& side_surface, bool bTop, TSTREAM& stream, TPROGRESS& progress);
	void SaveBodyGroup(const geo::CBodyGroup& body_group, TSTREAM& stream, TPROGRESS& progress);	
	void LoadBodyGroup(geo::CBodyGroup& body_group, TSTREAM& stream, CStreamVersion &version, TPROGRESS& progress);
	void old_load_stream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

  typedef std::map<int, geo::CCoordinateSet<geo::CVector> > TSupportMap;
  void CreateStandardSupports(const geo::ISurface& surface, TSupportMap& mpSupportMap, IProgressBase& progress);
  void CreateBoundaryInterfaceElements(const geo::ISurface& surface, TSupportMap& mpSupportMap, IProgressBase& progress);
  void CreateBoundaryInterfaceElement(const geo::IFace& face, TSupportMap& mpSupportMap, IProgressBase& progress);


public:
	// construction
	CHexaMesh(CHexaModel& model);
	virtual ~CHexaMesh();

	// GraphNode
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;

	// Element set
	virtual DIMENSION Dimension() const;

	// Intial volume
	virtual const geo::IMesh& Mesh() const;
	virtual geo::IMesh& Mesh();

	// Horizon look up
	virtual const CHorizonBase* SlipHorizon(const geo::IElement &interface_element) const;

	// Mesher
	//virtual bool OnGridLine(const geo::IPoint& pt1, const geo::IPoint& pt2) const;
	virtual bool CanCreateMesh() const;
	virtual void CreateMesh();
	virtual void InvalidateMesh();
	virtual bool IsMesh() const;

  std::string CurrentMesher() const;
  void SwitchMesher(std::string type);

  virtual void InvalidateSupports();
  virtual void GenerateSupports();

	// Stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

	virtual long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaMesh);
};

#endif // !defined(AFX_HEXAMESH_H__8E59A0C1_6F38_4751_B8CC_7028EDA779BA__INCLUDED_)
