// HomogenizationBox.h: interface for the CHomogenizationBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOMOGENIZATIONBOX_H__207DF3A0_F5CF_41A5_A17D_F790F6264F6D__INCLUDED_)
#define AFX_HOMOGENIZATIONBOX_H__207DF3A0_F5CF_41A5_A17D_F790F6264F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IDianaXWrapper;

#include "IScalingBox.h"
#include "GeomecModelVisitor.h"

class CModelBase;
class CMaterialServer;
class IMaterialRock;
class CElementSet;
class CElementValueSet;
class IResult;
class IResultComponent;
class CDepletionStage;

class CValueType;
class CPointSet;
class CAnalysisType;
class ISaveModel;
class IRetrieveDianaFileNames;

namespace geo {
  class CBlockMesh;
}

#include "ivaluecomposite.h"

class CHomogenizationBox : public IScalingBox  
{
private:
	//geo::CBlockMesh m_mesh;
	geo::CBlockMesh *m_mesh;
	geo::CBlockMesh	*m_display_mesh; // is basically the same as m_mesh but without the springelements
	CPointSet *m_mesh_ptset; // this is a pointset representing the m_mesh, each point represents a center of an element

	std::vector<int> m_vcRightSprings; // indices to the node of the springelements in m_mesh
	std::vector<int> m_vcTopSprings;   // indices to the node of the springelements in m_mesh
	std::vector<int> m_vcBackSprings;  // indices to the node of the springelements in m_mesh
	
	typedef CSingleCommandTemplate<CHomogenizationBox> THomoBoxCommand;
	
	const geo::CBlockMesh &DisplayMesh() const { return *m_display_mesh; }

	void assignMesh(geo::CBlockMesh& mesh, const CHomogenizationBox& hbox) const;

public:
	CElementSet* CaptureElementSet(CModelBase& target_model) const;
	
	
	CHomogenizationBox(CFemAppModel &model);
	CHomogenizationBox(CFemAppModel &model, const QString &strName);
	CHomogenizationBox(const CHomogenizationBox &rhs);
	virtual ~CHomogenizationBox();

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool CanConnectItem(const CGraphNode& item) const;
	virtual bool ConnectItem(const CGraphNode& item);
	virtual bool Destroy();

	virtual int DisplayListSize() const;
	virtual const geo::IObject &DisplayList(int nIndex) const;

	virtual const QString &Name() const;
	virtual void Name(const QString &strName);

	const geo::CBlockMesh &Mesh() const {return *m_mesh;}
//	void SetMesh(const geo::IPoint &center, double &edge_length, int mesh_division);
	void SetMesh(const geo::IPoint& center, const double& sizeX, const double& sizeY, const double& sizeZ,
				 int divX, int divY, int divZ, const double& azimuth);

	virtual CHomogenizationBox& operator=(const CHomogenizationBox &rhs);
	bool operator==(const CHomogenizationBox &rhs) const;

//	geo::CValue Value(const geo::IElement &element,IQuantityDouble::UNIT unit,bool bPoisson) const;

	bool InsideModel() const;

	CPointSet *PointSet() { return m_mesh_ptset; }
	
	const std::vector<int> &RightSprings() const { return m_vcRightSprings; }
	const std::vector<int> &BackSprings() const { return m_vcBackSprings; }
	const std::vector<int> &TopSprings() const { return m_vcTopSprings; }

	//wjrx improved zoom-in: after creation of zoom in model, 
	// directly display cut 'surafces' menu
	// We need public methods for this
	//
	bool CanCutSurafceInDatastoreWithBox() const;

	void CreatePointSet();

  ACCEPT_GEOMECMODELVISITORS(VisitHomogenizationBox);

private:
	void SortOutTyings(std::map<int, std::set<int> > &connections) const;
	void SetupSpringElements();
	void SetupSupports();
	void SetupTyings(const std::map<int,std::set<int> > &connections);
public:
	void SetUpCalculationGeometry();
};

class CHomoBoxEntry : public CStorageNodeEntry<CHomogenizationBox>
{
public:
	typedef CSingleCommandTemplate<CHomoBoxEntry> THomoBoxEntryCommand;
	
	CHomoBoxEntry(CModelBase &model);

  ACCEPT_GEOMECMODELVISITORS(VisitHomoBoxEntry);
};

#endif // !defined(AFX_HOMOGENIZATIONBOX_H__207DF3A0_F5CF_41A5_A17D_F790F6264F6D__INCLUDED_)
