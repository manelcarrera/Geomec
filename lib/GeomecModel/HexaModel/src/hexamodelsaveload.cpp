// HexaModel.cpp: implementation of the CHexaModel class.
//
//////////////////////////////////////////////////////////////////////

#include "AnalysisPoint.h"
#include "BaseEntryTypes.h"
#include "BoundaryBase.h"
#include "ColorScaleEntry.h"
#include "CrossSection.h"
#include "DerivedResult.h"
#include "FemAppEntryTypes.h"
#include "GeoSurface.h"
#include "GlobalInitialStressNode.h"
#include "GlobalPressure.h"
#include "GroupNode.h"
#include "HexaBoundary.h"
#include "HexaEntryTypes.h"
#include "HexaHorizon.h"
#include "HexaMesh.h"
#include "HexaModel.h"
#include "HomogenizationBox.h"
#include "ResultTree.h"
#include "StreamVersion.h"
#include "hexameshregion.h"

#include "NewArchiveStdStringStream.h"
#include "NewWellPath.h"
#include "VersionNumbers.h"
#include "WellPath.h"

bool CHexaModel::LoadHexa1(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));
  m_bIsMeshed = false; // Set to true if formations are read.

  // Set mesh region
  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry *)(GraphEntry(MD_HEXA_MESH_ZONE));
  pEntry->Main().SetMainGrid(1.0, 1.0);

  // Load boundary
  Boundary().LoadStream(stream, version, prog);

  if (CStreamVersion(3, 0, 33) < version) {
    // Load analysis points
    CAnalysisPointEntry &point_entry = dynamic_cast<CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
    point_entry.LoadStream(*this, stream, version, prog);
  }

  // Load the point sets
  LoadPointSets(stream, version, prog);

  // Load initial stress node and pressure
  GlobalInitialStress().LoadStream(stream, version, prog);
  GlobalPressure().LoadStream(stream, version, prog);

  // Load surfaces
  LoadSurfaces(stream, version, prog);
  /*
  CSurfaceEntry& surface_entry = dynamic_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  surface_entry.LoadStream(*this, stream, version, prog);
  */

  // Load Horizons
  CHexaHorizonEntry *pHorizonEntry = (CHexaHorizonEntry *)GraphEntry(MD_HEXA_HORIZON);
  pHorizonEntry->LoadStream(*this, stream, version, prog);

  // Load rock materials
  LoadRockMaterials(stream, version, prog);
  /*
  CMaterialEntry& rock_material_entry = dynamic_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
  rock_material_entry.LoadStream(*this, stream, version, prog);
  */

  if (CStreamVersion(3, 0, 31) < version) {
    if (version < CStreamVersion(3, 8, 2)) {
      // Load casing grade
      CMaterialEntry casing_grade_entry(0, 0, "", *this);
      casing_grade_entry.LoadStream(*this, stream, version, prog);
    }
  }

  // Load Formations
  CHexaFormationEntry &formation_entry = dynamic_cast<CHexaFormationEntry &>(*GraphEntry(MD_HEXA_FORMATION));
  formation_entry.LoadStream(*this, stream, version, prog);

  // Load MeshRegions
  LoadMeshRegions(stream, version, prog);

  if (version < CStreamVersion(3, 0, 42)) {
    // Load wellpaths
    CWellPathEntry &well_entry = dynamic_cast<CWellPathEntry &>(*GraphEntry(MD_BASE_WELLPATH));
    well_entry.LoadStream(*this, stream, version, prog);

    CWellPathEntry::TNodeSet well_paths = well_entry.EntryNodes();
    CWellPathEntry::TNodeSet::iterator it;
    for (it = well_paths.begin(); it != well_paths.end(); it++) {
      CWellPath *path = (*it);
      if (version < CStreamVersion(3, 0, 46) && path->DefPointList().NrOfPoints() == 0)
        path->destroy();
    }
  }

  if ((version < CStreamVersion(4, 1, 32)) || (version == CStreamVersion(4, 1, 33))) {
    CWellPathEntry &well_entry = dynamic_cast<CWellPathEntry &>(*GraphEntry(MD_BASE_WELLPATH));
    CNewArchiveStdStringStream stream1(std::fstream::in | std::fstream::out | std::fstream::binary);
    well_entry.SaveStream(stream1, prog);
    stream1.SetPosition(0);
    CNewWellPathEntry &new_well_entry = dynamic_cast<CNewWellPathEntry &>(*GraphEntry(MD_NEW_WELLPATH));
    CStreamVersion ver(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
    new_well_entry.LoadStream(*this, stream1, ver, prog);
  }

  // Load CrossSections
  CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry *)GraphEntry(MD_BASE_XSECTION);
  pXSecEntry->LoadStream(*this, stream, version, prog);

  // Load the scene
  pCSEntry->LoadStream(stream, version, prog);

  return true;
}

bool CHexaModel::LoadHexa2(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  m_pMesh->InvalidateMesh();

  // if we're importing an older model for zoom-in we need to trick the mesh
  // so it believes it shouldn't generate boundary interfaces, otherwise
  // the resultregister will not load properly
  CHexaBoundary &boundary = static_cast<CHexaBoundary &>(Boundary());
  if (version < CStreamVersion(4, 1, 2) && parentModel() != 0 && !parentModel()->Loading()) {
    assert(boundary.CreateInterfaces()); // should have been set by constructor
    boundary.CreateInterfaces(false);
  }

  m_pMesh->LoadStream(stream, version, prog);

  // Load the result server
  ResultRegister().LoadStream(stream, version, prog);

  if (version < CStreamVersion(4, 1, 2) && parentModel() != 0 && !parentModel()->Loading()) {
    assert(!boundary.CreateInterfaces());
    boundary.CreateInterfaces(true);
  }

  // From 3,0,42 we load wells here
  if ((CStreamVersion(3, 0, 41) < version) &&
      ((version < CStreamVersion(4, 1, 32)) || (version == CStreamVersion(4, 1, 33)))) {
    // Load wellpaths
    CWellPathEntry &well_entry = dynamic_cast<CWellPathEntry &>(*GraphEntry(MD_BASE_WELLPATH));
    well_entry.LoadStream(*this, stream, version, prog);

    CWellPathEntry::TNodeSet well_paths = well_entry.EntryNodes();
    CWellPathEntry::TNodeSet::iterator it;
    for (it = well_paths.begin(); it != well_paths.end(); it++) {
      CWellPath *path = (*it);
      if (version < CStreamVersion(3, 0, 46) && path->DefPointList().NrOfPoints() == 0)
        delete path;
    }
  }

  if ((version < CStreamVersion(4, 1, 32)) || (version == CStreamVersion(4, 1, 33))) {
    CWellPathEntry &well_entry = dynamic_cast<CWellPathEntry &>(*GraphEntry(MD_BASE_WELLPATH));
    CNewArchiveStdStringStream stream1(std::fstream::in | std::fstream::out | std::fstream::binary);

    NumberModels();

    well_entry.SaveStream(stream1, prog);
    stream1.SetPosition(0);
    CNewWellPathEntry &new_well_entry = dynamic_cast<CNewWellPathEntry &>(*GraphEntry(MD_NEW_WELLPATH));
    CStreamVersion ver(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
    new_well_entry.LoadStream(*this, stream1, ver, prog);
  }

  if (version >= CStreamVersion(3, 7, 23)) {
    // wjrx mantis 3327
    // Load newwellpaths
    CNewWellPathEntry &well_entry = dynamic_cast<CNewWellPathEntry &>(*GraphEntry(MD_NEW_WELLPATH));
    well_entry.LoadStream(*this, stream, version, prog);
  }

  if (version > CStreamVersion(4, 1, 75)) {
    CGroupNodeEntry &groupnode_entry = static_cast<CGroupNodeEntry &>(*GraphEntry(MD_BASE_GROUPNODE));
    groupnode_entry.LoadStream(*this, stream, version, prog);
  }

  // Derived results (versioning is done inside)
  if (CStreamVersion(3, 0, 6) < version) {
    CDerivedResultGroup &resultgroup = ResultTree().DerivedResults();
    resultgroup.LoadStream(stream, version, prog);
  }

  if (CStreamVersion(3, 0, 50) < version) {
    CHomoBoxEntry &homobox_entry = dynamic_cast<CHomoBoxEntry &>(*GraphEntry(MD_BASE_HOMO_BOX));
    homobox_entry.LoadStream(*this, stream, version, prog);
  }

  return true;
}

bool CHexaModel::LoadHexa3(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  C3DModel::OnLoad(stream, version, prog);

  return true;
}

bool CHexaModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  return (LoadHexa1(stream, version, prog) && LoadHexa2(stream, version, prog) && LoadHexa3(stream, version, prog));
}

long CHexaModel::SavedItems() const {
  long lRet = C3DModel::SavedItems();

  lRet += Boundary().SavedItems(); // Boundary
  const CAnalysisPointEntry &point_entry =
      dynamic_cast<const CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  lRet += point_entry.SavedItems();
  lRet += SavedItemsPointSets();              // PointSets
  lRet += GlobalInitialStress().SavedItems(); // Global initial stress
  lRet += GlobalPressure().SavedItems();      // Global pressure
  lRet += SavedItemsSurfaces();
  /*
  const CSurfaceEntry& surface_entry = dynamic_cast<const CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  lRet += surface_entry.SavedItems();
  */
  CHexaHorizonEntry *pEntry = (CHexaHorizonEntry *)GraphEntry(MD_HEXA_HORIZON);
  lRet += pEntry->SavedItems(); // Horizons
  lRet += SavedItemsRockMaterials();
  /*
  const CMaterialEntry& rock_material_entry = dynamic_cast<const CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
  lRet += rock_material_entry.SavedItems();
  */
  const CHexaFormationEntry &formation_entry =
      dynamic_cast<const CHexaFormationEntry &>(*GraphEntry(MD_HEXA_FORMATION));
  lRet += formation_entry.SavedItems(); // Formations
  lRet += SavedItemsMeshRegions();      // Meshregions
  const CNewWellPathEntry &newwell_entry = dynamic_cast<const CNewWellPathEntry &>(*GraphEntry(MD_NEW_WELLPATH));
  lRet += newwell_entry.SavedItems();
  const CGroupNodeEntry &groupnode_entry = static_cast<const CGroupNodeEntry &>(*GraphEntry(MD_BASE_GROUPNODE));
  lRet += groupnode_entry.SavedItems();

  CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry *)GraphEntry(MD_BASE_XSECTION);
  lRet += pXSecEntry->SavedItems(); // XSec
  const CColorScaleEntry *pCSEntry = dynamic_cast<const CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));
  lRet += pCSEntry->SavedItems(); // Colorscales
  lRet += m_pMesh->SavedItems();
  lRet += ResultRegister().SavedItems();
  const CDerivedResultGroup &derivedresults = ResultTree().DerivedResults();
  lRet += derivedresults.SavedItems();

  return lRet;
}

bool CHexaModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));

  // Number value composites
  NumberValueComposites();

  // Number opengl nodes
  NumberOpenGLNodes();

  // Save boundary
  Boundary().SaveStream(stream, progress);

  // Save analysis points
  CAnalysisPointEntry &point_entry = dynamic_cast<CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  point_entry.SaveStream(stream, progress);

  // Save pointset
  SavePointSets(stream, progress);

  GlobalInitialStress().SaveStream(stream, progress);
  GlobalPressure().SaveStream(stream, progress);

  // Save surfaces
  SaveSurfaces(stream, progress);
  /*
  CSurfaceEntry& surface_entry = dynamic_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  surface_entry.SaveStream(stream, progress);
  */

  // Save horizons
  CHexaHorizonEntry *pEntry = (CHexaHorizonEntry *)GraphEntry(MD_HEXA_HORIZON);
  pEntry->SaveStream(stream, progress);

  // Save rock material
  SaveRockMaterials(stream, progress);
  /*
  CMaterialEntry& rock_material_entry = dynamic_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
  rock_material_entry.SaveStream(stream, progress);
  */

  // Save formations
  CHexaFormationEntry &formation_entry = dynamic_cast<CHexaFormationEntry &>(*GraphEntry(MD_HEXA_FORMATION));
  formation_entry.SaveStream(stream, progress);

  // Save the meshregions
  SaveMeshRegions(stream, progress);

  // Save cross sections
  CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry *)GraphEntry(MD_BASE_XSECTION);
  pXSecEntry->SaveStream(stream, progress);

  // Save color scales
  pCSEntry->SaveStream(stream, progress);

  // Save mesh
  m_pMesh->SaveStream(stream, progress);
  ResultRegister().SaveStream(stream, progress);

  // wjrx mantis 3327
  // Save new wellpaths
  //
  CNewWellPathEntry &newwell_entry = dynamic_cast<CNewWellPathEntry &>(*GraphEntry(MD_NEW_WELLPATH));
  newwell_entry.SaveStream(stream, progress);

  CGroupNodeEntry &groupnode_entry = static_cast<CGroupNodeEntry &>(*GraphEntry(MD_BASE_GROUPNODE));
  groupnode_entry.SaveStream(stream, progress);

  // Saving the user defined result
  CDerivedResultGroup &derivedresults = ResultTree().DerivedResults();
  derivedresults.SaveStream(stream, progress);

  // Save scalingboxes
  CHomoBoxEntry &homobox_entry = dynamic_cast<CHomoBoxEntry &>(*GraphEntry(MD_BASE_HOMO_BOX));
  homobox_entry.SaveStream(stream, progress);

  C3DModel::OnSave(stream, progress);

  return true;
}

long CHexaModel::SavedItemsMeshRegions() const {
  long nRet = 0;

  // Save Mesh Regions
  assert(GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = ((CHexaMeshRegionEntry *)GraphEntry(MD_HEXA_MESH_ZONE))->EntryNodes();

  for (CHexaMeshRegionEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    nRet += (*it)->SavedItems();
  }

  return nRet;
}

void CHexaModel::SaveMeshRegions(CStorageNode::TSTREAM &stream, TPROGRESS &progress) {
  // Save Mesh Regions
  assert(GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = ((CHexaMeshRegionEntry *)GraphEntry(MD_HEXA_MESH_ZONE))->EntryNodes();
  int nRegionSize = stNode.size();

  // Save size first and then main
  stream << nRegionSize;
  ((CHexaMeshRegionEntry *)GraphEntry(MD_HEXA_MESH_ZONE))->Main().SaveStream(stream, progress);

  for (CHexaMeshRegionEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    if (dynamic_cast<CHexaSubMeshRegion *>(*it))
      (*it)->SaveStream(stream, progress);
  }
}

void CHexaModel::LoadMeshRegions(CStorageNode::TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Load MeshParams
  // there may already be one meshregion. Search and destroy!!!
  CGraphEntry::TEntryNodeSet meshregions = GraphEntry(MD_HEXA_MESH_ZONE)->GraphEntryNodes();
  for (CGraphEntry::TEntryNodeSet::iterator iter = meshregions.begin(); iter != meshregions.end(); iter++) {
    delete *iter;
  }

  // Read size first
  int nRegionSize;
  stream >> nRegionSize;

  // Load main region first
  CHexaMainMeshRegion *pMain = new CHexaMainMeshRegion(*this);
  pMain->LoadStream(stream, version, progress);

  // Read sub regions
  for (int i = 1; i < nRegionSize; i++) {
    CHexaSubMeshRegion *pRegion = new CHexaSubMeshRegion(*this);
    pRegion->LoadStream(stream, version, progress);
  }
}
