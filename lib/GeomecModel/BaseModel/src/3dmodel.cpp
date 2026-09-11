// 3dmodel.cpp: implementation of the C3DModel class.
//
//////////////////////////////////////////////////////////////////////

#include "3dmodel.h"

#include "BaseEntryTypes.h"
#include "CrossSection.h"
#include "WellPath.h"
#include "NewWellPath.h" // wjrx mantis 3185
#include "HomogenizationBox.h"
#include "Geomec3DDianaRunner.h"
#include "DianaRunController.h"
#include "GeomecHeatFlowDianaRunner.h"
#include "3DHorizon.h"
#include "MeasuredTopDisplacementsNode.h"
#include "GeomecMixtureDianaRunner.h"
#include "GeomecContainmentMixtureDianaRunner.h"
#include "Upscaling.h"
#include "GeoSurface.h"
#include "StreamVersion.h"
#include "BoundaryBase.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"
#include "InterfaceBoundary.h"
#include "NonMeshedSurface.h"
#include "GroupNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DModel::C3DModel(CAnalysisLogger& logger, const CVersionManager& versionManager)
: CModelBase(logger, versionManager),
  m_pTopDisplacementsNode(0)
{

}

C3DModel::~C3DModel()
{
  if (m_pTopDisplacementsNode != 0)
  {
  delete m_pTopDisplacementsNode;
  }
}

bool C3DModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog)
{
  if(CStreamVersion(3, 0, 114) < version)
  {
  m_pTopDisplacementsNode->LoadStream(stream, version, prog);
  }

  if(version >= CStreamVersion(3, 7, 1))
  {
    CUpscalingEntry &upscalingentry = static_cast<CUpscalingEntry&>(*GraphEntry(MD_BASE_UPSCALING));
  upscalingentry.LoadStream(stream, version, prog);
  }

  if(version >= CStreamVersion(4, 1, 21))
  {
  CNonMeshedSurfaceEntry& nmsEntry = static_cast<CNonMeshedSurfaceEntry&>(*GraphEntry(MD_BASE_NONMESHEDSURFACE));
  nmsEntry.LoadStream(*this, stream, version, prog);
  }

  CInterfaceBoundary* pBoundary = dynamic_cast<CInterfaceBoundary*>(&Boundary());
  if(pBoundary && parentModel() && !pBoundary->CreateInterfaces())
  {
  // force creation of interfaces for imported models that are used for zoomin
  pBoundary->CreateInterfaces(true);

  // and force use of default K values...
  pBoundary->InterfaceDefinition().IntMatType(CBoundaryInterfaceDef::K);
  pBoundary->InterfaceDefinition().KradTop(0.001, CDoubleQuantity::SI_UNIT);
  pBoundary->InterfaceDefinition().KradBottom(100., CDoubleQuantity::SI_UNIT);
  pBoundary->InterfaceDefinition().KradSides(100., CDoubleQuantity::SI_UNIT);
  pBoundary->InterfaceDefinition().KtanTop(0.001, CDoubleQuantity::SI_UNIT);
  pBoundary->InterfaceDefinition().KtanBottom(0.001, CDoubleQuantity::SI_UNIT);
  pBoundary->InterfaceDefinition().KtanSides(0.001, CDoubleQuantity::SI_UNIT);

  if(Mesh().Locked())
      Mesh().InvalidateAtUnLock();
  }

  return true;
}

bool C3DModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog)
{
  m_pTopDisplacementsNode->SaveStream(stream, prog);

  CUpscalingEntry &upscalingentry = static_cast<CUpscalingEntry&>(*GraphEntry(MD_BASE_UPSCALING));
  upscalingentry.SaveStream(stream, prog);

  CNonMeshedSurfaceEntry& nmsEntry = static_cast<CNonMeshedSurfaceEntry&>(*GraphEntry(MD_BASE_NONMESHEDSURFACE));
  nmsEntry.SaveStream(stream, prog);

  return true;
}

long C3DModel::SavedItems() const
{
  const CUpscalingEntry &upscalingentry = static_cast<const CUpscalingEntry&>(*GraphEntry(MD_BASE_UPSCALING));
  const CNonMeshedSurfaceEntry& nmsEntry = static_cast<const CNonMeshedSurfaceEntry&>(*GraphEntry(MD_BASE_NONMESHEDSURFACE));
  return m_pTopDisplacementsNode->SavedItems() + upscalingentry.SavedItems() + nmsEntry.SavedItems() + CModelBase::SavedItems();
}

void C3DModel::createContainers()
{
  CModelBase::createContainers();

  // We have cross sections, wells and surfaces in 3D
  new CCrossSectionEntry(*this);
  new CWellPathEntry(*this);
  new CNewWellPathEntry(*this); // wjrx mantis 3185
  new CGroupNodeEntry(*this);
  new CSurfaceEntry(*this);
  new CHomoBoxEntry(*this);
  new CUpscalingEntry(*this);

#if 1
  if(!parentModel())
  new CZoominModelEntry(*this);
#endif

  new CNonMeshedSurfaceEntry(*this);
}

void C3DModel::createDefaults()
{
  CModelBase::createDefaults();

  m_pTopDisplacementsNode = new CMeasuredTopDisplacementsNode("Measured top displacements", *this);
}

void C3DModel::OnCloseModel()
{
  VERIFY(DeleteEntry(MD_BASE_NONMESHEDSURFACE));

  if(!parentModel())
  DeleteEntry(MD_BASE_ZOOMIN_MODEL);

  if(parentModel())
  {
  // unlink the surfaces from the opengl node entry, otherwise they will be deleted
  CSurfaceEntry* pEntry = (CSurfaceEntry*)GraphEntry(MD_BASE_SURFACE);
  const CSurfaceEntry::TNodeSet& stNodes = pEntry->EntryNodes();
  for(CSurfaceEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if(GraphEntry(MD_BASE_OPENGL_NODE)->IsLinkedTo(**it))
    GraphEntry(MD_BASE_OPENGL_NODE)->UnLink(**it);
  }
  }

  VERIFY(DeleteEntry(MD_BASE_UPSCALING));
  VERIFY(DeleteEntry(MD_BASE_WELLPATH));
  VERIFY(DeleteEntry(MD_NEW_WELLPATH)); //wjrx mantis 3185
  VERIFY(DeleteEntry(MD_BASE_GROUPNODE));
  VERIFY(DeleteEntry(MD_BASE_SURFACE));
  VERIFY(DeleteEntry(MD_BASE_XSECTION));
  VERIFY(DeleteEntry(MD_BASE_HOMO_BOX));


  CModelBase::OnCloseModel();
}

CGeomecDianaRunnerBase* C3DModel::OnCreateDianaRunner(CDianaRunController& controller)
{
  switch(controller.AnalysisType().AnalysisType())
  {
  case CAnalysisType::AT_HEAT:					return new CGeomecHeatFlowDianaRunner(Mesh(), *this, controller);
  case CAnalysisType::AT_MIXTURE:				return new CGeomecMixtureDianaRunner(Mesh(), *this, controller);
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:   return new CGeomecContainmentMixtureDianaRunner(Mesh(), *this, controller);

  case CAnalysisType::AT_LINEAR:
  case CAnalysisType::AT_NONLIN:				
  default:										return new CGeomec3DDianaRunner(Mesh(), *this, controller);
  }
}

void C3DModel::CollectNodesFromBodyFace(TNodeElementsMap& mpNodeElements, const geo::IFace& face) const
{
  const geo::IBody* pBody = face.Parent();
  assert(pBody != 0);
  int j;
  for(j = 0; j < face.NrOfNodes(); ++j)
  {
  TNodeElementsMap::iterator it = mpNodeElements.insert(TNodeElementsMap::value_type(face.Node(j).Index(), std::set<int>())).first;
  it->second.insert(pBody->Index());
  }
}

void C3DModel::CollectBoundaryNodesFromHorizon(TNodeElementsMap& mpNodeElements, const C3DHorizon& hor) const
{
  int i;
  for(i = 0; i < hor.BodyFaceSize(); ++i)
  {
  const geo::IFace& face = hor.BodyFace(i);
  CollectNodesFromBodyFace(mpNodeElements, face);
  }
}

void C3DModel::CollectBoundaryNodes(TNodeElementsMap& mpNodeElements) const
{
  CollectBoundaryNodesFromHorizon(mpNodeElements, static_cast<const C3DHorizon&>(Boundary().GetTopHorizon()));
  CollectBoundaryNodesFromHorizon(mpNodeElements, static_cast<const C3DHorizon&>(Boundary().GetBottomHorizon()));

  std::vector<const geo::ISurface*> vcSideSurfaces = Boundary().GetSideMeshSurfaces();
  for(size_t i = 0; i < vcSideSurfaces.size(); ++i)
  {
  const geo::ISurface& surface = *vcSideSurfaces[i];
  int j;
  for(j = 0; j < surface.FaceSize(); ++j)
      CollectNodesFromBodyFace(mpNodeElements, surface.Face(j));
  }
}

const CMeasuredTopDisplacementsNode& C3DModel::MeasuredTopDisplacementsNode() const
{
  assert(m_pTopDisplacementsNode);
  return *m_pTopDisplacementsNode;
}

CMeasuredTopDisplacementsNode& C3DModel::MeasuredTopDisplacementsNode()
{
  assert(m_pTopDisplacementsNode);
  return *m_pTopDisplacementsNode;
}

C3DModel::ModelType C3DModel::modelType() const
{
  return e3DModel;
}
