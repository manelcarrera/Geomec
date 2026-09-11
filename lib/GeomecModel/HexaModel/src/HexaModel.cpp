// HexaModel.cpp: implementation of the CHexaModel class.
//
//////////////////////////////////////////////////////////////////////

#include "HexaEntryTypes.h"
#include "HexaModel.h"
#include "HexaHorizon.h"
#include "HexaFormation.h"
#include "HexaBoundary.h"
#include "hexameshregion.h"
#include "HexaMesh.h"
#include "resourceIDI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C74E3F10197
CHexaModel::CHexaModel(CAnalysisLogger& logger, const CVersionManager& versionManager)
:C3DModel(logger, versionManager), m_pMesh(0)
{
}

//##ModelId=3BAF4D1E0007
CHexaModel::~CHexaModel()
{
//	assert(m_pMesh == 0);
}

//##ModelId=3BAF4D1E0009
unsigned int CHexaModel::ModelIconId() const
{
  return staticModelIconId();
}

//##ModelId=3BAF4D1E0013
QString CHexaModel::documentType() const
{
  return staticDocumentType();
}

unsigned int CHexaModel::staticModelIconId()
{
  return IDI_HORIZON_MODEL;
}

QString CHexaModel::staticDocumentType()
{
  return "Hexahedron";
}


void CHexaModel::createContainers()
{
  C3DModel::createContainers();
  new CHexaMeshRegionEntry(*this);
  new CHexaHorizonEntry(*this); 
  new CHexaFormationEntry(*this);
}

void CHexaModel::createDefaults()
{	
  m_pBoundary = new CHexaBoundary(geo::CPoint(0,0,0), geo::CPoint(100, 100, 100), *this);
  // Create the mesh before the resultserver is created in CModelBase. The boundary needs to be in place for the mesh
  m_pMesh = new CHexaMesh(*this);

  C3DModel::createDefaults();

  // Create the main region ...
  new CHexaMainMeshRegion(*this, 1000, 1000);
}

//##ModelId=3C74E3F101C5
void CHexaModel::OnCloseModel()
{
  // First we delete the geo objects
  VERIFY(DeleteEntry(MD_HEXA_FORMATION));
  VERIFY(DeleteEntry(MD_HEXA_HORIZON));
  VERIFY(DeleteEntry(MD_HEXA_MESH_ZONE));

  // Delete the base model
  C3DModel::OnCloseModel();

  // Mesh is deleted automaticly by the frame work
}

//##ModelId=3C74E3F101C7
bool CHexaModel::CreateHorizon(const CSurfaceBase& s)
{
  CHexaHorizonEntry* pEntry = (CHexaHorizonEntry*)(GraphEntry(MD_HEXA_HORIZON));
  pEntry->ConnectItem(s);
  return true;
}

//=============================================================================

bool CHexaModel::IsResult() const
{
  return ResultRegister().ResultsAvailable();
}

void CHexaModel::Calculate()
{
  // TODO : Call the functions of the ResultServer to calculate.
}

CMeshBase& CHexaModel::Mesh()
{
  return *m_pMesh;
}

const CMeshBase& CHexaModel::Mesh() const
{
  return *m_pMesh;
}

CHexaModel::ModelType CHexaModel::modelType() const
{
  return eHexaModel;
}

void CHexaModel::CreateDensityPoints(const geo::IElement &/*element*/, CPointSet &/*pointset*/)
{
/*
  const geo::CHexahedron &body = dynamic_cast<const geo::CHexahedron &> (element);

  double dStressDiff = GlobalInitialStress().TotalVerticalStressAtNode(body, 4, true) -
    GlobalInitialStress().TotalVerticalStressAtNode(body, 0, true);

  double dDepth = body.Point(4).Z() - body.Point(0).Z();

  // Push back density in pointset
  std::vector<double> vcDensity(1);

  if(fabs(dDepth) > EPS) vcDensity[0] = ((dStressDiff / dDepth) * 1.e6 / Gravity().Value());
  else                   vcDensity[0] = 0;
  
  pointset.PushBack(geo::CPoint(body.Point(0).X(), body.Point(0).Y(), body.Point(0).Z() + (dDepth / 2)), vcDensity);
*/
}

