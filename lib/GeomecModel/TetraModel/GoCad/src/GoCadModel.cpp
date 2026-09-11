// GoCadModel.cpp: implementation of the CGoCadModel class.
//
//////////////////////////////////////////////////////////////////////

#include "GoCadModel.h"
#include "TetraSubHorizon.h"
#include "TetraEntryTypes.h"
#include "TetraMesh.h"
#include "BoundaryBase.h"
#include "HexaModel.h"
#include "HexaHorizon.h"
#include "HexaFormation.h"
#include "TetraSurface.h"
#include "HexaEntryTypes.h"
#include "TetraSubBoundary.h"
#include "PointSet.h"
#include "TetraFormation.h"
#include "CrossSection.h"
#include "HomogenizationBox.h"
#include "ResultTree.h"
#include "DerivedResult.h"

#include "NewArchiveStdStringStream.h"
#include "BaseEntryTypes.h"
#include "resourceIDS.h"
#include "GeomecStringTable.h"
#include "Surface.h"
#include "RotatedSystem.h"
#include "CoordinateSet.h"

#include <fstream>

#include "GocadElementSet.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"

#ifdef SKUA_NEW
#include "SKUAParseData.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGoCadModel::CGoCadModel(CAnalysisLogger& logger, const CVersionManager& versionManager)
 : CTetraModel(logger, versionManager)
{
}

CGoCadModel::~CGoCadModel()
{
}

bool CGoCadModel::CanImportMesh() const
{
  return (GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes().size() == 0);
}

#ifdef SKUA_NEW
void CGoCadModel::ImportMesh(const QString& fileName,
  bool (*selectGocadSolidDlg)(
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  std::vector<int>& vcSolids))
#else
void CGoCadModel::ImportMesh(const QString& fileName,
  bool(*selectGocadSolidDlg)(
  const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
  std::vector<int>& vcSolids))
#endif
{
  CTetraMesh& mesh = dynamic_cast<CTetraMesh&>(Mesh());
  mesh.ImportMeshFromGoCadFile(fileName, selectGocadSolidDlg);
}

CGoCadModel::ModelType CGoCadModel::modelType() const
{
  return eGoCadModel;
}

long CGoCadModel::OnCountSteps(const CHexaModel& model) const
{
  long lRet = model.GraphEntry(MD_ROCK_MATERIAL)->GraphEntryNodes().size();

  // Calculate pointsets
  TPointSetEntry::TNodeSet stPsNode = ((TPointSetEntry*)model.GraphEntry(MD_BASE_POINTSET))->EntryNodes();
  for (TPointSetEntry::TNodeSet::iterator ps_it = stPsNode.begin(); ps_it != stPsNode.end(); ps_it++)
  {
    // Is it an generic volume
    if(dynamic_cast<const CElementSet*>(*ps_it) || dynamic_cast<const CPointSet*>(*ps_it))
      lRet += 2 * (*ps_it)->SavedItems();
  }

  // Calculate horizons
  CHexaFormationEntry *pEntry = (CHexaFormationEntry*)model.GraphEntry(MD_HEXA_FORMATION);
  assert(pEntry);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(CHexaFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    CHexaFormation& formation = **it;
    
    // Begin with top ...
    if(formation.UpperFormation() == 0)
      lRet += formation.UpperHorizon().BodyFaceSize();
    lRet += formation.LowerHorizon().BodyFaceSize();
  }

  lRet += model.GraphEntry(MD_BASE_XSECTION)->GraphEntryNodes().size();
  lRet += model.GraphEntry(MD_BASE_WELLPATH)->GraphEntryNodes().size();
  lRet += model.GraphEntry(MD_BASE_HOMO_BOX)->GraphEntryNodes().size();
  return lRet;
}

void CGoCadModel::OnConvert(const CHexaModel& model, TPROGRESS& progress)
{
  // Number value composites
  const_cast<CHexaModel&>(model).NumberValueComposites();

  // Number opengl nodes
  const_cast<CHexaModel&>(model).NumberOpenGLNodes();

  progress.AddSteps(OnCountSteps(model));
  ProjectInfo(model.ProjectInfo() + " Converted from hexahedron to tetrahedron");
  Gravity(model.Gravity().Value());
  ElementOrder(model.ElementOrder());

  CopyDepletionStages(model.DepletionStageEntry(), DepletionStageEntry(), progress);

  // Convert point sets and create valuetype mapper
  progress.StatusMessage("Converting point sets ...");

  TPointSetMap psMap;

  ConvertPointSets(model, progress, psMap);

  CopyInitialStressAndPressure(model);
  CopyTemperature(model);

  progress.StatusMessage("Converting materials ...");
  CopyMaterial(model, progress);
  progress.StatusMessage("Converting horizons ...");
  ConvertSurfaces(model, progress);
  progress.StatusMessage("Converting cross sections ...");
  ConvertCrossSections(model, progress);
  progress.StatusMessage("Converting well paths ...");
  ConvertWellPaths(model, progress, psMap);
  progress.StatusMessage("Converting homogenization boxes ...");
  ConvertHomoBoxes(model, progress);

  ConvertDerivedResults(model, progress);

  EnablePointSets();
}

void CGoCadModel::AddPoints(const CHexaFormation& formation, TLineMap& line_map)
{
  for(TLineMap::iterator it = line_map.begin(); it != line_map.end(); it++)
  {
    std::set<double>::iterator m = it->second.end();
    m--;
    double dMax = *m;
    m--;
    double dMin = *m;
    assert(dMin < dMax);
    for(int i = 1; i < formation.Elements(); i++)
      it->second.insert(dMin + ((dMax - dMin) * i) / formation.Elements());
  }
}

geo::ISurface* CGoCadModel::YSurface(const CRotatedSystem& rs, const TLineMap& Ymap, double YPos)
{
  // Create surface
  geo::CSurface *pSurface = new geo::CSurface();

  // Init routine
  TLineMap::const_iterator it_first = Ymap.begin();
  TLineMap::const_iterator it_second = Ymap.begin();
  assert(it_second != Ymap.end());
  it_second++;

  while(it_second != Ymap.end())
  {
    assert(it_second->second.size() == it_first->second.size());
    // Walke over the sets	
    std::set<double>::const_iterator it_first_z_first = it_first->second.begin(); 
    std::set<double>::const_iterator it_first_z_second = it_first->second.begin();
    it_first_z_second++;
    std::set<double>::const_iterator it_second_z_first = it_second->second.begin(); 
    std::set<double>::const_iterator it_second_z_second = it_second->second.begin();
    it_second_z_second++;
    while(it_second_z_second != it_second->second.end())
    {
      // Create two triangles
      pSurface->AddTriangle(rs.ToGlobal(geo::CPoint(it_first->first, YPos, *it_first_z_first)),
                      rs.ToGlobal(geo::CPoint(it_second->first, YPos, *it_second_z_first)),
                      rs.ToGlobal(geo::CPoint(it_second->first, YPos, *it_second_z_second)));
      pSurface->AddTriangle(rs.ToGlobal(geo::CPoint(it_first->first, YPos, *it_first_z_first)),
                      rs.ToGlobal(geo::CPoint(it_second->first, YPos, *it_second_z_second)),
                      rs.ToGlobal(geo::CPoint(it_first->first, YPos, *it_first_z_second)));
      // Update iterators
      it_first_z_first++;
      it_first_z_second++;
      it_second_z_first++;
      it_second_z_second++;
    }
    
    it_first++;
    it_second++;
  }

  return pSurface;
}

geo::ISurface* CGoCadModel::XSurface(const CRotatedSystem& rs, const TLineMap& Xmap, double XPos)
{
  // Create surface
  geo::CSurface *pSurface = new geo::CSurface();

  // Init routine
  TLineMap::const_iterator it_first = Xmap.begin();
  TLineMap::const_iterator it_second = Xmap.begin();
  it_second++;

  while(it_second != Xmap.end())
  {
    assert(it_second->second.size() == it_first->second.size());
    // Walke over the sets	
    std::set<double>::const_iterator it_first_z_first = it_first->second.begin(); 
    std::set<double>::const_iterator it_first_z_second = it_first->second.begin();
    it_first_z_second++;
    std::set<double>::const_iterator it_second_z_first = it_second->second.begin(); 
    std::set<double>::const_iterator it_second_z_second = it_second->second.begin();
    it_second_z_second++;
    while(it_second_z_second != it_second->second.end())
    {
      // Create two triangles
      pSurface->AddTriangle(rs.ToGlobal(geo::CPoint(XPos, it_first->first, *it_first_z_first)),
                      rs.ToGlobal(geo::CPoint(XPos, it_second->first, *it_second_z_first)),
                      rs.ToGlobal(geo::CPoint(XPos, it_second->first, *it_second_z_second)));
      pSurface->AddTriangle(rs.ToGlobal(geo::CPoint(XPos, it_first->first, *it_first_z_first)),
                            rs.ToGlobal(geo::CPoint(XPos, it_second->first, *it_second_z_second)),
                            rs.ToGlobal(geo::CPoint(XPos, it_first->first, *it_first_z_second)));
      // Update iterators
      it_first_z_first++;
      it_first_z_second++;
      it_second_z_first++;
      it_second_z_second++;
    }
    
    it_first++;
    it_second++;
  }

  return pSurface;
}

CTetraSubHorizon* CGoCadModel::ConvertHorizon(const CHexaHorizon& horizon,
                      TLineMap& Ymax,
                      TLineMap& Ymin,
                      TLineMap& Xmax,
                      TLineMap& Xmin,
                      TPROGRESS& progress)
{
  const CHexaModel& hexa_model = dynamic_cast<const CHexaModel&>(horizon.Model());
  const CBoundaryBase& boundary = hexa_model.Boundary();
  CRotatedSystem rs(boundary.BoxAzimuth(), boundary.Box().MidPoint());

  geo::CPoint ptMax = rs.LocalMax(boundary.Box());
  geo::CPoint ptMin = rs.LocalMin(boundary.Box());

  // Walk over faces collect points for horizons
  typedef geo::CCoordinateSet<const geo::IPoint*> TPointSet;
  //typedef std::set<const geo::IPoint*, geo::ICoordinate::CCoordinateLess> TPointSet;
  TPointSet stPoint;
  for(int nFace = 0; nFace < horizon.BodyFaceSize(); nFace++)
  {
    // Walk over points
    const geo::IFace& face = horizon.BodyFace(nFace);
    for(int nPoint = 0; nPoint < face.NrOfPoints(); nPoint++)
    {
      geo::CPoint pt(rs.ToLocal(face.Point(nPoint)));
      if(fabs(pt.Y() - ptMax.Y()) < EPS)
      {
        TLineMap::iterator it = Ymax.insert(TLineMap::value_type(pt.X(), std::set<double>())).first;
        it->second.insert(face.Point(nPoint).Z());
      }
      if(fabs(pt.Y() - ptMin.Y()) < EPS)
      {
        TLineMap::iterator it = Ymin.insert(TLineMap::value_type(pt.X(), std::set<double>())).first;
        it->second.insert(face.Point(nPoint).Z());
      }
      if(fabs(pt.X() - ptMax.X()) < EPS)
      {
        TLineMap::iterator it = Xmax.insert(TLineMap::value_type(pt.Y(), std::set<double>())).first;
        it->second.insert(face.Point(nPoint).Z());
      }
      if(fabs(pt.X() - ptMin.X()) < EPS)
      {
        TLineMap::iterator it = Xmin.insert(TLineMap::value_type(pt.Y(), std::set<double>())).first;
        it->second.insert(face.Point(nPoint).Z());
      }
      stPoint.insert(&face.Point(nPoint));
    }

    progress.Step();
  }

  // Create surface from points and create a horizon
  geo::CPtrArray<geo::IPoint> vcPoint;
  for(TPointSet::iterator it = stPoint.begin(); it != stPoint.end(); it++)
    vcPoint.PushBack((geo::IPoint&)**it);

  CTetraSurface *pTetraSurface = new CTetraSurface(horizon.Name().toStdString().c_str(), vcPoint, *this);
  pTetraSurface->Color(horizon.Color());
  CTetraSubHorizon *pHorizon = new CTetraSubHorizon(*this);
  CopyHorizon( horizon, *pHorizon);
  pHorizon->LinkTo(Mesh());
  if(pHorizon->Slip())
    pHorizon->create(GraphEntry(MD_TETRA_SUB_FAULT));
  else
    pHorizon->create(GraphEntry(MD_TETRA_SUB_HORIZON));
  pHorizon->LinkTo(*GraphEntry(MD_TETRA_SUB_ALL));
  pHorizon->LinkTo(*pTetraSurface);
  
  return pHorizon;
}

void CGoCadModel::ConvertHomoBoxes(const CHexaModel &model, TPROGRESS& progress)
{
  assert(GraphEntry(MD_BASE_HOMO_BOX));
  CHomoBoxEntry::TNodeSet stNode = ((CHomoBoxEntry*)model.GraphEntry(MD_BASE_HOMO_BOX))->EntryNodes();
  CHomoBoxEntry::TNodeSet::iterator it;
  for(it = stNode.begin(); it != stNode.end(); it++)
  {
    CHomogenizationBox *pBox = new CHomogenizationBox(*this);
    CopyHomogenizationBox(**it, *pBox);
    progress.Step();
  }
}

void CGoCadModel::ConvertDerivedResults(const CHexaModel& model, TPROGRESS& progress)
{
  const CDerivedResultGroup& derivedresults = model.ResultTree().DerivedResults();
  int i;
  for(i = 0; i < derivedresults.ChildSize(); ++i)
  {
    // Save derived result in buffer
    CNewArchiveStdStringStream mem_file(std::fstream::in | std::fstream::out | std::fstream::binary);
  assert(dynamic_cast<const CDerivedResult*>(&derivedresults.Child(i)));
  CDerivedResult& result = const_cast<CDerivedResult&>(static_cast<const CDerivedResult&>(derivedresults.Child(i)));
    result.SaveStream(mem_file, progress);
    mem_file.SetPosition(0);
    // Create derived result on new model
    CDerivedResult* pTarget = new CDerivedResult(ResultTree().DerivedResults());
    CStreamVersion version = currentVersion();
    pTarget->LoadStream(mem_file, version, progress);
  }
  ResultTree().DerivedResults().AttachToModel();
}

void CGoCadModel::ConvertPointSets(const CHexaModel& model, TPROGRESS& progress, TPointSetMap& psMap)
{
  assert(GraphEntry(MD_BASE_POINTSET));
  TPointSetEntry::TNodeSet stNode = ((TPointSetEntry*)model.GraphEntry(MD_BASE_POINTSET))->EntryNodes();
  for (TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    IPointSet *pTarget = 0;
    // Is it an generic volume
    CElementSet *pVolume = dynamic_cast<CElementSet*>(*it);
    if(pVolume)
    {
      // Save volume in buffer
      CNewArchiveStdStringStream mem_file(std::fstream::in | std::fstream::out | std::fstream::binary);
      pVolume->SaveStream(mem_file, progress);
      mem_file.SetPosition(0);
      // Create volume on new model
      pTarget = new CElementSet(*this);
      CStreamVersion version = currentVersion();
      pTarget->LoadStream(mem_file, version, progress);

      psMap[pVolume] = pTarget;
    }

    CPointSet* pPointSet = dynamic_cast<CPointSet*>(*it);
    if(pPointSet)
    {
      // Save pointset in buffer
      CNewArchiveStdStringStream mem_file(std::fstream::in | std::fstream::out | std::fstream::binary);
      pPointSet->SaveStream(mem_file, progress);
      mem_file.SetPosition(0);
      // Create point set on new model
      CStreamVersion version = currentVersion();
      if (pPointSet->pointSetType() == IPointSet::INPUT)
    pTarget = new CNewWellPathInput(*this);
      else
    pTarget = new CPointSet(*this);
      pTarget->LoadStream(mem_file, version, progress);
  
      psMap[pPointSet] = pTarget;
  }
  }
}

void CGoCadModel::ConvertSurfaces(const CHexaModel& model, TPROGRESS& progress)
{
  assert(model.IsMesh());
  typedef std::set<CHexaFormation*, CGraphNode::CLess> TFormationSet;
  TFormationSet stFormation;
  // Create formations set
  CHexaFormationEntry *pEntry = (CHexaFormationEntry*)model.GraphEntry(MD_HEXA_FORMATION);
  assert(pEntry);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(CHexaFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    VERIFY(stFormation.insert(*it).second);

  // Walk over formation and extract horizon surfaces and side surfaces
  TLineMap Ymax;
  TLineMap Ymin;
  TLineMap Xmax;
  TLineMap Xmin;

  CTetraSubHorizon *pUpper = 0;
  for(TFormationSet::iterator f = stFormation.begin(); f != stFormation.end(); f++)
  {
    CHexaFormation& formation = **f;
    assert((formation.UpperFormation() == 0) || (f != stFormation.begin()));
    
    // Begin with top ...
    if(formation.UpperFormation() == 0)
    {
      // We need to add the top surface to new model
      pUpper = ConvertHorizon(formation.UpperHorizon(), Ymax, Ymin, Xmax, Xmin, progress);
      SubBoundary().TopHorizon().LinkTo(*pUpper);
    }

    // Add other horizons
    CTetraSubHorizon* pLower = ConvertHorizon(formation.LowerHorizon(), Ymax, Ymin, Xmax, Xmin, progress);
    if(formation.LowerFormation() == 0)
      SubBoundary().BottomHorizon().LinkTo(*pLower);

    // Add formation
    assert(pUpper);
    ConvertFormation(formation, *pUpper, *pLower);
    pUpper = pLower;
  
    // Add points for formation depth
    AddPoints(formation, Ymax);
    AddPoints(formation, Ymin);
    AddPoints(formation, Xmax);
    AddPoints(formation, Xmin);
  }

  CRotatedSystem rs(model.Boundary().BoxAzimuth(), model.Boundary().Box().MidPoint());

  // Add boundary Ymax (Ksi_max)
  geo::ISurface* pSurface = YSurface(rs, Ymax, rs.LocalMax(model.Boundary().Box()).Y());
  CTetraSurface *pTetraSurface = new CTetraSurface("Ksi_max", *pSurface, *this);
  SubBoundary().LinkTo(*pTetraSurface);
  delete pSurface;

  // Add boundary Ymin (Ksi_min)
  pSurface = YSurface(rs, Ymin, rs.LocalMin(model.Boundary().Box()).Y());
  pTetraSurface = new CTetraSurface("Ksi_min", *pSurface, *this);
  SubBoundary().LinkTo(*pTetraSurface);
  delete pSurface;

  // Add boundary Xmax (Eta_max)
  pSurface = XSurface(rs, Xmax, rs.LocalMax(model.Boundary().Box()).X());
  pTetraSurface = new CTetraSurface("Eta_max", *pSurface, *this);
  SubBoundary().LinkTo(*pTetraSurface);
  delete pSurface;

  // Add boundary Xmin (Eta_min)
  pSurface = XSurface(rs, Xmin, rs.LocalMin(model.Boundary().Box()).X());
  pTetraSurface = new CTetraSurface("Eta_min", *pSurface, *this);
  SubBoundary().LinkTo(*pTetraSurface);
  delete pSurface;
}

void CGoCadModel::ConvertFormation(const CHexaFormation& formation, CTetraSubHorizon& upper, CTetraSubHorizon& lower)
{
  // Create formation and formation volume
  CTetraFormation* pFormation = new CTetraFormation(*this);
  CopyFormation(formation, *pFormation);
  pFormation->create(GraphEntry(MD_TETRA_FORMATION));

  CTetraFormationVolume* pVolume = new CTetraFormationVolume(*pFormation);
  /* From IFormationElementSet constructor */
  pVolume->reParent(pFormation);
//	CModelBase &pModel = dynamic_cast<CModelBase&>(pFormation->Model());
//	assert(&pModel);
//	pVolume->LinkTo(pModel.Mesh());
  /* ************************************* */

  pVolume->Color(formation.Color());

  // Create tag for identifying formation ...
  TGraphNodeSet stIdentifier;
  stIdentifier.insert(&upper.Surface(0));
  stIdentifier.insert(&lower.Surface(0));
  pVolume->Identifier(stIdentifier);
}

void CGoCadModel::ConvertCrossSections(const CHexaModel& model, TPROGRESS& progress)
{
  CCrossSectionEntry* pEntry = (CCrossSectionEntry*)model.GraphEntry(MD_BASE_XSECTION);
  assert(pEntry);
  CCrossSectionEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(CCrossSectionEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    // Convert xsection
    const CCrossSection& source = **it;
  new CCrossSection(source.Name().toStdString().c_str(), *this, source.basePoint(), source.normal());
    progress.Step();
  }
}


void CGoCadModel::ConvertWellPaths(const CHexaModel& model, TPROGRESS& progress, TPointSetMap& psMap)
{
  CNewWellPathEntry* pEntry = (CNewWellPathEntry*)model.GraphEntry(MD_NEW_WELLPATH);
  assert(pEntry);
  CNewWellPathEntry::TNodeSet stNode = pEntry->EntryNodes();
  for (CNewWellPathEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
  // Convert wellpath
  const CNewWellPath& source = **it;
  new CNewWellPath(*static_cast<const CNewWellPathInput *>(psMap[source.WellPathInput()]), *this);
  progress.Step();
  }
}



bool CGoCadModel::Load(const QString& /*sPath*/, CProgressDlg_MFC& /*dlg*/)
{
  assert(false);
  return false;
}
bool CGoCadModel::Save(const QString& /*sPath*/, CProgressDlg_MFC& /*dlg*/)
{
  assert(false);
  return false;
}

void CGoCadModel::createContainers()
{
  new CGoCadSubModelEntry(*this);			// Sub Model
  CTetraModel::createContainers();
}

void CGoCadModel::OnCloseModel()
{
  CTetraModel::OnCloseModel();
  VERIFY(DeleteEntry(MD_TETRA_SUB_ALL));
}


// Used in Import (Earth) Mesh from the main menu
// Should always exist, and will be the first child (see createContainers() above)
// so we just search for it, instead of adding a member pointing to the entry
CGoCadSubModelEntry *CGoCadModel::SubModelEntry()
{
  for (std::size_t i = 0; i < CGraphNode::childSize(); ++i)
  {
  if (dynamic_cast<CGoCadSubModelEntry *>(&CGraphNode::childAt(i)))
      return static_cast<CGoCadSubModelEntry *>(&CGraphNode::childAt(i));
  }
  return 0;
}

CGoCadSubModelEntry::CGoCadSubModelEntry(CTetraModel& model)
: TSubModelEntry(MD_TETRA_SUB_ALL, IDI_GOCAD, "GoCad Model", model)
{
}

bool CGoCadSubModelEntry::CanInsertSurfaceContainer() const
{
  CModelBase& model = (CModelBase&)Model();
  const CTetraMesh& mesh = dynamic_cast<const CTetraMesh&>(model.Mesh());
  return mesh.CanInvalidateMesh();
}

bool CGoCadSubModelEntry::ConnectItem(const CGraphNode& item)
{
  CGocadElementSet& gocadElementSet = const_cast <CGocadElementSet&> (
  dynamic_cast <const CGocadElementSet&> (item));

  return gocadElementSet.importInGocadModel();
}

bool CGoCadSubModelEntry::CanConnectItem(const CGraphNode& item) const
{
  try
  {
  const CGocadElementSet& gocadElementSet =
      dynamic_cast <const CGocadElementSet&> (item);

  return gocadElementSet.canImportInGocadModel();
  }

  catch (...)
  {
  }

  return TSubModelEntry::CanConnectItem(item);
}
