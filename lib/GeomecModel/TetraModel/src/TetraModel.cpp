// TetraModel.cpp: implementation of the CTetraModel class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraEntryTypes.h"
#include "TetraModel.h"
#include "CrossSection.h"
#include "TetraBoundary.h"
#include "TetraFormation.h"
#include "TetraSubBoundary.h"
#include "TetraSuperHorizon.h"
#include "GlobalInitialStressNode.h"
#include "GlobalPressure.h"
#include "TetraMesh.h"
#include "ResultTree.h"
#include "DerivedResult.h"
#include "AnalysisPoint.h"
#include "KeyFile.h"
#include "HomogenizationBox.h"
#include "ColorScaleEntry.h"
#include "resourceIDI.h"
#include "TetraSubHorizon.h"
#include "resourceIDS.h"
#include "3DSupportNode.h"
#include "DisplacementSupportNode.h"
#include "PressureSupportNode.h"
#include "GroupNode.h"

#include "WellPath.h"
#include "NewWellPath.h"
#include "NewArchiveStdStringStream.h"
#include "VersionNumbers.h"

#include "MeshRefinementModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3B6905BF01F4
CTetraModel::CTetraModel(CAnalysisLogger& logger, const CVersionManager& versionManager)
:C3DModel(logger, versionManager), m_pMesh(0), m_pSubBoundary(0)
{
	m_mesh_refinement_data = new CMeshRefinementModel();
}

//##ModelId=3B6905BF01F6
CTetraModel::~CTetraModel()
{
	delete m_mesh_refinement_data;
}

//##ModelId=3B6905BF01C5
unsigned int CTetraModel::ModelIconId() const
{
	return staticModelIconId();
}

//##ModelId=3B6905BF01D5
QString CTetraModel::documentType() const 
{
	return staticDocumentType();
}

unsigned int CTetraModel::staticModelIconId()
{
  return IDI_TETRA_MODEL;
}

QString CTetraModel::staticDocumentType()
{
  return "Tetrahedron";
}

void CTetraModel::createContainers()
{
	C3DModel::createContainers();
	// Create entries for sub model
	new CTetraSubHorizonEntry(MD_TETRA_SUB_HORIZON, IDI_HORIZONS, IDS_TREE_HORIZONS, *this);	// Horizons
	new CTetraSubHorizonEntry(MD_TETRA_SUB_FAULT, IDI_FAULT, IDS_TREE_FAULTS_FRACTURES, *this);			// Faults

	// Create entries for super model
	new CTetraSuperHorizonEntry(*this);

	new CTetraFormationEntry(*this);
//	InsertEntry(*(new CTetraSubBoundary()));
}

void CTetraModel::createDefaults()
{
	// Create mesh first
	m_pSubBoundary = new CTetraSubBoundary(*this);

	// Create boundary first
	m_pBoundary = new CTetraBoundary(*m_pSubBoundary);

	m_pMesh = new CTetraMesh(*this);
	C3DModel::createDefaults();
}


void CTetraModel::OnCloseModel()
{
	VERIFY(DeleteEntry(MD_TETRA_SUPER_HORIZON));
	VERIFY(DeleteEntry(MD_TETRA_SUB_HORIZON));
	VERIFY(DeleteEntry(MD_TETRA_SUB_FAULT));

	VERIFY(DeleteEntry(MD_TETRA_FORMATION));

	delete m_pSubBoundary;
	m_pSubBoundary = 0;

	C3DModel::OnCloseModel();

	// Mesh deletion is done by the framework
}

//=============================================================================

bool CTetraModel::IsResult() const
{
	return ResultRegister().ResultsAvailable();
}

void CTetraModel::Calculate()
{
}

CMeshBase& CTetraModel::Mesh()
{
	return *m_pMesh;
}

const CMeshBase& CTetraModel::Mesh() const
{
	return *m_pMesh;
}

bool CTetraModel::LoadTetra1(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  if(CStreamVersion(3, 0, 69) > version) // from version 3.0.69 the pointsets are loaded before the boundary
  {
    // Depletion stages and model params are already loaded by CModelBase
    Boundary().LoadStream(stream,version,prog);
  }
  else
  {
    // Load the point sets
    LoadPointSets(stream,version, prog);
  }

  assert(Mesh().IsLinkedTo(Boundary()));

  if(CStreamVersion(3, 0, 33) < version)
  {
    // Load analysis points
    CAnalysisPointEntry& point_entry = dynamic_cast<CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
    point_entry.LoadStream(*this, stream, version, prog);
  }

  if(CStreamVersion(3, 0, 69) > version) // from version 3.0.69 the pointsets are loaded before the boundary
  {
    // Load the point sets
    LoadPointSets(stream,version, prog);
  }
  else
  {
    // Depletion stages and model params are already loaded by CModelBase
    Boundary().LoadStream(stream,version,prog);
  }

  if(CStreamVersion(3, 0, 16) < version)
  {
    // Load initial stress node and pressure
    GlobalInitialStress().LoadStream(stream, version, prog);
    GlobalPressure().LoadStream(stream, version, prog);
  }

  // Load surfaces
  LoadSurfaces(stream, version, prog);
  /*
  CSurfaceEntry& surface_entry = dynamic_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  surface_entry.LoadStream(*this, stream, version, prog);
  */

  // Load faults
  CTetraSubHorizonEntry& fault_entry = dynamic_cast<CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_FAULT));
  fault_entry.LoadStream(*this, stream, version, prog);

  const CKeyFile keyfile;

#ifdef TETRA_FAULTS_KEYFILE
  if(!keyfile.Unlocked() && fault_entry.EntryNodes().size() > 0)
  {
    _m()->msg(IDP_FAULTSLOCKED);
    return false;
  }
#endif

  // Save horizons
  CTetraSubHorizonEntry& horizon_entry = dynamic_cast<CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_HORIZON));
  horizon_entry.LoadStream(*this, stream, version, prog);

  // Save sub boundary
  SubBoundary().LoadStream(stream, version, prog);

  // Load Horizons of super model
  CTetraSuperHorizonEntry& s_horizon_entry = dynamic_cast<CTetraSuperHorizonEntry&>(*GraphEntry(MD_TETRA_SUPER_HORIZON));
  s_horizon_entry.LoadStream(*this, stream, version, prog);

#ifdef TETRA_SUPERMODEL_KEYFILE
  if(!keyfile.Unlocked() && s_horizon_entry.EntryNodes().size() > 0)
  {
    _m()->msg(IDP_SUPERMODELLOCKED);
    return false;
  }
#endif


#ifdef TETRA_FAULTS_KEYFILE
  if(!keyfile.Unlocked())
  {
    CTetraSuperHorizonEntry::TNodeSet nodes = s_horizon_entry.EntryNodes();
    bool bShowMessage = true;
    for(CTetraSuperHorizonEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
      if((*it)->Slip())
      {
        if(bShowMessage)
        {
          _m()->msg(IDP_SLIPPINGHORIZONSLOCKED);
          bShowMessage = false;
        }
        (*it)->Slip(false);
      }
    }
  }
#endif

  // Load rock materials
  LoadRockMaterials(stream, version, prog);
  /*
  CMaterialEntry& rock_material_entry = dynamic_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
  rock_material_entry.LoadStream(*this, stream, version, prog);
  */

  if(CStreamVersion(3,0,31) < version)
  {
    if(version < CStreamVersion(3, 8, 2))
    {
      // Load casing grade
      CMaterialEntry entry(0, 0, "", *this);
      entry.LoadStream(*this, stream, version, prog);
    }
  }

  // Development version (10 - 02 - 2003)
  assert(Mesh().IsLinkedTo(Boundary()));
  if(CStreamVersion(3, 0, 1) < version)
  {
    // Load Formations 
    CTetraFormationEntry& formation_entry = dynamic_cast<CTetraFormationEntry&>(*GraphEntry(MD_TETRA_FORMATION));
    formation_entry.LoadStream(*this, stream, version, prog);

    if(version < CStreamVersion(3, 0, 42))
    {
      // Wellpaths
      CWellPathEntry& well_entry = dynamic_cast<CWellPathEntry&>(*GraphEntry(MD_BASE_WELLPATH));
      well_entry.LoadStream(*this, stream, version, prog);

      CWellPathEntry::TNodeSet well_paths = well_entry.EntryNodes();
      CWellPathEntry::TNodeSet::iterator it;
      for(it = well_paths.begin(); it != well_paths.end(); it++)
      {
        CWellPath *path = (*it);
        if(version < CStreamVersion(3, 0, 46) && path->DefPointList().NrOfPoints() == 0)
        delete path;
      }

    }

    // Load CrossSections
    CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry*)GraphEntry(MD_BASE_XSECTION);
    pXSecEntry->LoadStream(*this, stream, version, prog);

    // Load scenes
    assert(Mesh().IsLinkedTo(Boundary()));
    Mesh().InvalidateMesh();
    Mesh().Lock();
    Mesh().LoadStream(stream,version,prog);
    assert(Mesh().IsLinkedTo(Boundary()));

    // Load the color scales
    if(version > CStreamVersion(3, 0, 63))
    {
      CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));	
      pCSEntry->LoadStream(stream, version, prog);
    }

    // Load the result server
    ResultRegister().LoadStream(stream, version, prog);

    if ((CStreamVersion(3, 0, 41) < version) &&
      ((version < CStreamVersion(4, 1, 32)) ||
        (version == CStreamVersion(4, 1, 33))))
    {
      // Wellpaths
      CWellPathEntry& well_entry = dynamic_cast<CWellPathEntry&>(*GraphEntry(MD_BASE_WELLPATH));
      well_entry.LoadStream(*this, stream, version, prog);

      CWellPathEntry::TNodeSet well_paths = well_entry.EntryNodes();
      CWellPathEntry::TNodeSet::iterator it;
      for(it = well_paths.begin(); it != well_paths.end(); it++)
      {
        CWellPath *path = (*it);
        if (version < CStreamVersion(3, 0, 46) && path->DefPointList().NrOfPoints() == 0)
        {
          delete path;
        }
      }
    }

    if ((version < CStreamVersion(4, 1, 32)) ||
      (version == CStreamVersion(4, 1, 33)))
    {
      CWellPathEntry& well_entry =
        dynamic_cast<CWellPathEntry&>(*GraphEntry(MD_BASE_WELLPATH));
      CNewArchiveStdStringStream
        stream1(std::fstream::in | std::fstream::out | std::fstream::binary);

      NumberModels();

      well_entry.SaveStream(stream1, prog);
      stream1.SetPosition(0);
      CNewWellPathEntry& new_well_entry=
        dynamic_cast<CNewWellPathEntry&>(*GraphEntry(MD_NEW_WELLPATH));
      CStreamVersion ver(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
      new_well_entry.LoadStream(*this, stream1, ver, prog);
    }

    if(version >= CStreamVersion(3, 7, 23) )
    {
      // wjrx mantis 3327
      // Load newwellpaths
      CNewWellPathEntry& well_entry=
      dynamic_cast<CNewWellPathEntry&>(*GraphEntry(MD_NEW_WELLPATH));
      well_entry.LoadStream(*this, stream, version, prog);
    }

    if (version > CStreamVersion(4, 1, 75))
    {
      CGroupNodeEntry& groupnode_entry = static_cast<CGroupNodeEntry&>(*GraphEntry(MD_BASE_GROUPNODE));
      groupnode_entry.LoadStream(*this, stream, version, prog);
    }

  }

  // Derived results (versioning is done inside)
  if(CStreamVersion(3,0,6) < version)
  {
    CDerivedResultGroup& resultgroup = ResultTree().DerivedResults();
    resultgroup.LoadStream(stream, version, prog);
  }

  return true;
}

bool CTetraModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog)
{
  return (
    LoadTetra1(stream, version, prog) &&
    LoadTetra2(stream, version, prog));
}

bool CTetraModel::LoadTetra2(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog)
{
	if(CStreamVersion(3, 0, 50) < version)
	{
		CHomoBoxEntry &homobox_entry = dynamic_cast<CHomoBoxEntry&>(*GraphEntry(MD_BASE_HOMO_BOX));
		homobox_entry.LoadStream(*this, stream, version, prog);		
	}

	if( version > CStreamVersion(4, 1, 68) )
		m_mesh_refinement_data->from_stream( stream );


  C3DModel::OnLoad(stream, version, prog);

	assert(Mesh().IsLinkedTo(Boundary()));
	return true;
}

long CTetraModel::SavedItems() const
{
	long lRet = C3DModel::SavedItems();

	lRet += Boundary().SavedItems();
	const CAnalysisPointEntry& point_entry = dynamic_cast<const CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
	lRet += point_entry.SavedItems();
	lRet += SavedItemsPointSets();
	lRet += GlobalInitialStress().SavedItems();
	lRet += GlobalPressure().SavedItems();
  lRet += SavedItemsSurfaces();
  /*
	const CSurfaceEntry& surface_entry = dynamic_cast<const CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
	lRet += surface_entry.SavedItems();
  */
	const CTetraSubHorizonEntry& fault_entry = dynamic_cast<const CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_FAULT));
	lRet += fault_entry.SavedItems();
	const CTetraSubHorizonEntry& horizon_entry = dynamic_cast<const CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_HORIZON));
	lRet += horizon_entry.SavedItems();
	lRet += SubBoundary().SavedItems();
	const CTetraSuperHorizonEntry& s_horizon_entry = dynamic_cast<const CTetraSuperHorizonEntry&>(*GraphEntry(MD_TETRA_SUPER_HORIZON));
	lRet += s_horizon_entry.SavedItems();
  lRet += SavedItemsRockMaterials();
  /*
	const CMaterialEntry& rock_material_entry = dynamic_cast<const CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
	lRet += rock_material_entry.SavedItems();
  */
	const CTetraFormationEntry& formation_entry = dynamic_cast<const CTetraFormationEntry&>(*GraphEntry(MD_TETRA_FORMATION));
	lRet += formation_entry.SavedItems();
	const CNewWellPathEntry& newwell_entry = dynamic_cast<const CNewWellPathEntry&>(*GraphEntry(MD_NEW_WELLPATH));
	lRet += newwell_entry.SavedItems();
  const CGroupNodeEntry& groupnode_entry = static_cast<const CGroupNodeEntry&>(*GraphEntry(MD_BASE_GROUPNODE));
  lRet += groupnode_entry.SavedItems();
	const CCrossSectionEntry& xsec_entry = dynamic_cast<const CCrossSectionEntry&>(*GraphEntry(MD_BASE_XSECTION));
	lRet += xsec_entry.SavedItems();
	const CColorScaleEntry *pCSEntry = dynamic_cast<const CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));
	lRet += pCSEntry->SavedItems();	

	lRet += Mesh().SavedItems();

	lRet += ResultRegister().SavedItems();

	// Load the derived results
  const CDerivedResultGroup& derivedresults = ResultTree().DerivedResults();
	lRet += derivedresults.SavedItems();

	const CHomoBoxEntry &homobox_entry = dynamic_cast<const CHomoBoxEntry&>(*GraphEntry(MD_BASE_HOMO_BOX));
	lRet += homobox_entry.SavedItems();

	return lRet;
}

bool CTetraModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress)
{
	// Number value composites
	NumberValueComposites();

	// Number opengl nodes
	NumberOpenGLNodes();
/*
	// Save boundary
	Boundary().SaveStream(stream,progress);
*/
	// Save pointset
	SavePointSets(stream, progress);

	// Save analysis points
	CAnalysisPointEntry& point_entry = dynamic_cast<CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
	point_entry.SaveStream(stream, progress);
/*
	// Save pointset
	SavePointSets(stream, progress);
*/
	// Save boundary
	Boundary().SaveStream(stream,progress);

	GlobalInitialStress().SaveStream(stream, progress);
	GlobalPressure().SaveStream(stream, progress);

	// Save surfaces
  SaveSurfaces(stream, progress);
  /*
  CSurfaceEntry& surface_entry = dynamic_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  surface_entry.SaveStream(stream, progress);
  */

	// Save faults
	CTetraSubHorizonEntry& fault_entry = dynamic_cast<CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_FAULT));
	fault_entry.SaveStream(stream, progress);

	// Save horizons
	CTetraSubHorizonEntry& horizon_entry = dynamic_cast<CTetraSubHorizonEntry&>(*GraphEntry(MD_TETRA_SUB_HORIZON));
	horizon_entry.SaveStream(stream, progress);

	// Save sub boundary
	SubBoundary().SaveStream(stream, progress);

	// Save horizons
	CTetraSuperHorizonEntry& s_horizon_entry = dynamic_cast<CTetraSuperHorizonEntry&>(*GraphEntry(MD_TETRA_SUPER_HORIZON));
	s_horizon_entry.SaveStream(stream, progress);

	// Save rock material
  SaveRockMaterials(stream, progress);
  /*
	CMaterialEntry& rock_material_entry = dynamic_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
	rock_material_entry.SaveStream(stream, progress);
  */

	// Save formations
	CTetraFormationEntry& formation_entry = dynamic_cast<CTetraFormationEntry&>(*GraphEntry(MD_TETRA_FORMATION));
	formation_entry.SaveStream(stream, progress);

	// Save cross sections
	CCrossSectionEntry& xsec_entry = dynamic_cast<CCrossSectionEntry&>(*GraphEntry(MD_BASE_XSECTION));
	xsec_entry.SaveStream(stream, progress);

	Mesh().SaveStream(stream,progress);

	// Save the color scales
	CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));	
	pCSEntry->SaveStream(stream, progress);

	ResultRegister().SaveStream(stream,progress);

	// wjrx mantis 3327
	// Save new wellpaths
	//
	CNewWellPathEntry& newwell_entry = dynamic_cast<CNewWellPathEntry&>(*GraphEntry(MD_NEW_WELLPATH));
	newwell_entry.SaveStream(stream, progress);

  CGroupNodeEntry& groupnode_entry = static_cast<CGroupNodeEntry&>(*GraphEntry(MD_BASE_GROUPNODE));
  groupnode_entry.SaveStream(stream, progress);

	// Save the derived results
  CDerivedResultGroup& derivedresults = ResultTree().DerivedResults();
  derivedresults.SaveStream(stream, progress);

	// Save scalingboxes
	CHomoBoxEntry &homobox_entry = dynamic_cast<CHomoBoxEntry&>(*GraphEntry(MD_BASE_HOMO_BOX));
	homobox_entry.SaveStream(stream, progress);

	m_mesh_refinement_data->to_stream( stream );

  C3DModel::OnSave(stream, progress);

	return true;
}

CTetraModel::ModelType CTetraModel::modelType() const
{
  return eTetraModel;
}

const CTetraSubBoundary& CTetraModel::SubBoundary() const
{
	return *m_pSubBoundary;
}

CTetraSubBoundary& CTetraModel::SubBoundary()
{
	return *m_pSubBoundary;
}

bool CTetraModel::OnModelSpecificAnalysisCheck(const CAnalysisType& /*antype*/, bool /*bWriteInputFiles*/, bool /*bWriteQuadDat*/)
{
  const CTetraBoundary& boundary = static_cast<const CTetraBoundary&>(Boundary());

  if(boundary.CreateInterfaces() && boundary.InterfaceElements() &&
     (!CheckZoomInBoundaryLoads(boundary.DisplacementSupportNode()) || !CheckZoomInBoundaryLoads(boundary.PressureSupportNode())))
  {
    if(_m()->msg(IDP_ZOOMINNOBOUNDARYLOADS, MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
      return false;
  }

  return true;
}

bool CTetraModel::CheckZoomInBoundaryLoads(const C3DSupportNode& supportnode) const
{
  const CDepletionStageEntry& entry = static_cast<const CDepletionStageEntry&>(*GraphEntry(MD_BASE_DEPLETION_STAGE));
  CDepletionStageEntry::const_iterator it;
  for(it = entry.begin(); it != entry.end(); ++it)
  {
    if(supportnode.DistriValues(*it) != 0)
      return true;
  }

  return false;
}
