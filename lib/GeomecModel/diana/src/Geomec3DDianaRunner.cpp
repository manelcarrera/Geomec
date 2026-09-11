// Geomec3DDianaRunner.cpp: implementation of the CGeomec3DDianaRunner class.
//
//////////////////////////////////////////////////////////////////////

#include "Geomec3DDianaRunner.h"

#include "3dmodel.h"

#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraBoundary.h"
#include "BoundaryInterfaceMaterial.h"
#include "InterfaceElement.h"
#include "DianaRunController.h"
#include "SurfaceDesc.h"

#include "lbfl.h"
#include "MeshBase.h"
#include "3DHorizon.h"
#include "DepletionStage.h"
#include "TetraHorizonBase.h"
#include "DCPressureLoad.h"
#include "FaultPressure.h"
#include "DCWeightLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeomec3DDianaRunner::CGeomec3DDianaRunner(CMeshBase &meshbase,
                       C3DModel &model,
                       CDianaRunController& controller)
: CGeomecCompactionDianaRunner(meshbase, model, controller)
{
}

CGeomec3DDianaRunner::~CGeomec3DDianaRunner()
{
}

const dia::IMaterial &CGeomec3DDianaRunner::Material(const geo::IElement &element) const
{
  const geo::CInterfaceElement *pElemen = dynamic_cast<const geo::CInterfaceElement*>(&element);
  if(pElemen)
  {
  // make sure it's not a fault interface element
  const CHorizonBase* pFault = MeshBase().SlipHorizon(*pElemen);
  if(!pFault)
  {
      const CInterfaceBoundary* pIfaceBoundary = dynamic_cast<const CInterfaceBoundary*>(&Model().Boundary());
      if(pIfaceBoundary && pIfaceBoundary->CreateInterfaces())
      {
      assert(pElemen->BackFace() == pElemen->FrontFace());
      return pIfaceBoundary->InterfaceMaterial(*pElemen);
      }
  }
  }

  return CGeomecCompactionDianaRunner::Material(element);
}

bool CGeomec3DDianaRunner::OnWriteMiscelaneous()
{
  if(!CGeomecCompactionDianaRunner::OnWriteMiscelaneous())
  return false;

  CTetraMesh* pTetMesh = dynamic_cast<CTetraMesh*>(&Model().Mesh());
  if(pTetMesh)
  {
  PushDir();

  ChangeDir("/GEOMEC");

  geo::CVector vecNormal;
  geo::CVector vecTangent;
  pTetMesh->GetModelOrientation(vecNormal, vecTangent);

  ftn_double_t direct[3];
  direct[0] = (ftn_double_t)vecNormal.X();
  direct[1] = (ftn_double_t)vecNormal.Y();
  direct[2] = (ftn_double_t)0.0;

  PutItemLength("DIRECT", direct, 3);

  PopDir();
  }

  return true;
}

int CGeomec3DDianaRunner::LoadsProgressSize() const
{
  int sz = 0;

  if(InitialLoadCaseActive())
  {
    // topload
    const C3DHorizon &top = dynamic_cast<const C3DHorizon &> (Model().Boundary().GetTopHorizon());
    sz += top.BodyFaceSize();
  }

  // pressure and initial stresses
  sz += (Controller().EndStage().Index() - Controller().StartStage().Index() + 1) * Mesh().ElementSize();

  const CTetraModel *pModel = dynamic_cast<const CTetraModel *> (&Model());

  if(pModel)
  {
    // we are dealing with a tetrahedron model, we will generate boundary loads
    const CTetraBoundary &boundary = dynamic_cast<const CTetraBoundary &> (pModel->Boundary());

    // side surfaces
  int i;
    for(i = 0; i < boundary.SideSurfaceSize(); i++)
    {
      const geo::CSurfaceDesc &surfdesc = boundary.SideSurfaceDesc(i);
      sz += surfdesc.TetSurfaceSize();
    }

    // bottom surface
    const CTetraHorizonBase *pBottom = boundary.BottomHorizon();
    assert(pBottom != 0);

    for(i = 0; i < pBottom->OutputSurfaceSize(); i++)
    {
      const geo::CSurfaceDesc &surfdesc = pBottom->OutputSurface(i);
      sz += surfdesc.TetSurfaceSize();
    }

    // top surface
    const CTetraHorizonBase* pTop = boundary.TopHorizon();
    assert(pTop != 0);

    for(i = 0; i < pTop->OutputSurfaceSize(); ++i)
    {
      const geo::CSurfaceDesc& surfdesc = pTop->OutputSurface(i);
      sz += surfdesc.TetSurfaceSize();
    }
  }

  return sz;
}

void CGeomec3DDianaRunner::WriteInterfaceInitialLoads(const CDepletionStage &/*stage*/, const geo::CInterfaceElement &iface, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures)
{
  assert(iface.FrontFace());
  assert(iface.BackFace());

  // get interface's initial pressures
  // interfaces have 8 initial pressures, but they are equal per corner
  // for each corner the maximum of the pressures of the two bodies is taken
  assert(iface.NrOfNodes() == iface.FrontFace()->NrOfPoints() + iface.BackFace()->NrOfPoints());
  std::vector<double> vcPressures(2 * iface.FrontFace()->NrOfPoints());

  bool bWriteThem = false;
  bool bWriteMultiple = false;

  for(int j = 0; j < iface.Front().NrOfPoints(); j++)
  {
    // make Pascal values
    vcPressures[j] = vcInitialPressures[j].Value() * 1e6;
    if(fabs(vcPressures[j]) > MIN_PRESSU_LOAD_VAL)
      bWriteThem = true;
    if(j && fabs(vcPressures[j] - vcPressures[j-1]) > MIN_PRESSU_LOAD_VAL)
      bWriteMultiple = true;
  }

  if(bWriteThem)
  {
    if(bWriteMultiple)
    {
      // 09022004: wed:
      // interface elements now have 8 values instead of 4, but each pair is equal, so copy
      for(int i = 0; i < iface.Front().NrOfPoints(); i++) 
        vcPressures[i+iface.Front().NrOfPoints()] = vcPressures[i];
      new dia::CPressureLoad(lcase, vcPressures, iface);
    }
    else
    {
      new dia::CPressureLoad(lcase, vcPressures[0], iface);
    }
  }
}

void CGeomec3DDianaRunner::WriteInterfaceStageLoads(const CDepletionStage &stage, const geo::CInterfaceElement &iface, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures)
{
  const CHorizonBase* fault = Model().Mesh().SlipHorizon(iface);
  if(fault)
  {
    const CFaultPressure& faultpressure = fault->Pressure(stage);

    IValueDomainScalar::TValueVec vcFaultPressures = faultpressure.Component().ScalarData().ValueElement(iface);

    std::vector<double> vcPressures(2 * iface.Front().NrOfPoints());

    bool bWriteThem = false;
    bool bWriteMultiple = false;

    for(int j = 0; j < iface.Front().NrOfPoints(); j++)
    {
      vcPressures[j] = vcFaultPressures[j].Value();

      vcPressures[j] -= vcInitialPressures[j].Value();
      vcPressures[j] *= 1e6;
      if(fabs(vcPressures[j]) > MIN_PRESSU_LOAD_VAL)
        bWriteThem = true;
      if(j && fabs(vcPressures[j] - vcPressures[j-1]) > MIN_PRESSU_LOAD_VAL)
        bWriteMultiple = true;
    }

    if(bWriteThem)
    {
      if(bWriteMultiple)
      {
        // 09022004: wed:
        // interface elements now get 8 values, but they are equal per corner node pair
        // copy the values
        for(int i = 0; i < iface.Front().NrOfPoints(); i++) vcPressures[i+iface.Front().NrOfPoints()] = vcPressures[i];
        new dia::CPressureLoad(lcase, vcPressures, iface);
      }
      else
      {
        new dia::CPressureLoad(lcase, vcPressures[0], iface);
      }
    }
  }
}

//void CGeomec3DDianaRunner::CombineSurface(geo::CSurface *pSurface,const geo::CSurfaceDesc &surfacedesc) const
//{
//	for(int i = 0; i < surfacedesc.TetSurfaceSize(); i++)
//	{
//		const geo::CTetSurface &surface = surfacedesc.TetSurface(i);
//		for(int j = 0; j < surface.FaceSize(); j++)
//		{
//			pSurface->AddFace(surface.Face(j));
//		}
//	}
//}

bool CGeomec3DDianaRunner::CreateTopLoad(const CHorizonBase &top, dia::CLoadCase &lcase)
{
  return CreateTopLoad3D(top, lcase);
}

bool CGeomec3DDianaRunner::FetchInitialPressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialPressures)
{
  bool bRet;

  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *> (&element);
  if(pIface)
  {
    const CHorizonBase* fault = Model().Mesh().SlipHorizon(*pIface);
    if(fault)
    {
      const CFaultPressure& faultpressure = fault->Pressure(stage);
      vcInitialPressures = faultpressure.Component().ScalarData().ValueElement(*pIface);
    }
    bRet = true;
  }
  else
  {
    bRet = FetchElementInitialPressures(stage, element, vcInitialPressures);
  }

  return bRet;
}

bool CGeomec3DDianaRunner::CreateInitialLoads(const geo::IElement &element,
                                              const CDepletionStage &stage,
                                              dia::CLoadCase &lcase,
                                              const IValueDomainScalar::TValueVec &vcInitialPressures,
                                              const IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *> (&element);
  const geo::IBody *pBody = dynamic_cast<const geo::IBody *> (&element);
  
  if(!pIface)
  {
    WriteElementInitialLoads(stage, *pBody, lcase, vcInitialPressures, vcInitialTemperatures);

    if(!PerformInitialStressCalculation())
    {
      if(!WriteZoomInInitialStresses(element, lcase, vcInitialPressures))
    return false;
    }
  }
  else
  {
    if(pIface->BackFace() != pIface->FrontFace()) { // for the boundary interface elements no initial loads are written
      WriteInterfaceInitialLoads(stage, *pIface, lcase, vcInitialPressures);
    }
  }

  m_progress->Step();
  

  return true;
}

bool CGeomec3DDianaRunner::CreateStageLoads(
  const geo::IElement &element, 
  const CDepletionStage &stage, dia::CLoadCase &lcase, 
  const IValueDomainScalar::TValueVec &vcInitialPressures, 
  const IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *> (&element);
  const geo::IBody *pBody = dynamic_cast<const geo::IBody *> (&element);
  
  if(!pIface)
  {
    WriteElementStageLoads(stage, *pBody, lcase, vcInitialPressures, vcInitialTemperatures);

    m_progress->Step();
  }
  else
  {
    if(pIface->BackFace() != pIface->FrontFace()) // for the boundary interface elements no stage loads are written
    {
      WriteInterfaceStageLoads(stage, *pIface, lcase, vcInitialPressures);
    }

    m_progress->Step();
  }

  return true;
}

void CGeomec3DDianaRunner::CreateWeightLoad(dia::CLoadCase &lcase, const double &gravity)
{
  new dia::CWeightLoad(lcase, gravity, geo::CVector::Zaxis);
}

// class CStagePointCombi
CGeomec3DDianaRunner::CStagePointCombi::CStagePointCombi(const CDepletionStage &stage, const geo::IPoint &pt)
: m_stage(stage), m_pt(pt)
{
}

bool CGeomec3DDianaRunner::CStagePointCombi::operator <(const CStagePointCombi &rhs) const
{
  // simple pointer comparison
  if(&m_stage < &rhs.m_stage) return true;
  if(&m_stage > &rhs.m_stage) return false;

  return (&m_pt < &rhs.m_pt);
}

const dia::IMaterial *CGeomec3DDianaRunner::InterfaceMaterial(const geo::IElement &element) const
{
  const geo::CInterfaceElement *pInterface = dynamic_cast<const geo::CInterfaceElement*> (&element);

  if( pInterface){
    const CHorizonBase* pFault = MeshBase().SlipHorizon(*pInterface);
    if(pFault)
      return &pFault->InterfaceMaterial(*pInterface, Controller().StartStage());
  }

  return 0;
}

bool CGeomec3DDianaRunner::IsZoomInModel() const
{
  const CInterfaceBoundary* pInterBound = dynamic_cast<const CInterfaceBoundary*>(&Model().Boundary());
  return pInterBound ? pInterBound->CreateInterfaces() : CGeomecDianaRunnerBase::IsZoomInModel();
}

