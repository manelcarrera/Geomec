// DianaRunner.cpp: implementation of the CGeomecDianaRunnerBase class.
//
//////////////////////////////////////////////////////////////////////

#include <QDir>
#include <QFile>
#include <sstream>

#include "DepletionStage.h"
#include "GeomecDianaRunner.h"

#include "MeshBase.h"
#include "FormationBase.h"
#include "TetraMesh.h"
#include "GlobalInitialStressNode.h"
#include "TetraModel.h"
#include "InterfaceElement.h"
#include "DianaRunController.h"
#include "FileCopier.h"
#include "VolumetricStrainLoad.h"
#include "StrainLoad.h"
#include "MaterialFractureAperture.h"
#include "TetraBoundary.h"
#include "GeomecUtils.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"

#include "lbfl.h"
#include "lbcx.h"
#include "lbgc.h"
#include "NumRange.h"

#include "GlobalMessage.h"
#include "TetraHorizonBase.h"
#include "ElementGroup.h"
#include "BodyTriangle.h"
#include "ISupport.h"
#include "AnalysisLogger.h"
#include "DCPrescribedDisplacement.h"
#include "DCBodyFaceLoad.h"
#include "DCConcentrationLoad.h"
#include "DCStrainLoad.h"
#include "DCInitialStressLoad.h"
#include "GlobalConstants.h"
#include "IPlatform.h"
#include "Environment.h"
#include "ISettings.h"
#include "GlobalMessage.h"
#include "VectorValueSet.h"
#include "HexaModel.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "Temperature.h"
#include "FaultPressure.h"
#include "FaultParametersNode.h"
#include "LibraryMaterial.h"
#include "ValueMapper.h"
#include "IParallelInitializationCallback.h"
#include "HexaEntryTypes.h"
#include "hexameshregion.h"
#include "ifaceelmmat.h"
#include "FilosFatalHandler.h"

#include "Printer.h"

#ifndef WIN32
#define _stat stat
#endif  // WIN32

#define BS 2048

namespace
{
	Printer* printer = Printer::instance(Printer::RunR);
}

const geo::IPoint &CGeomecDianaRunnerBase::CEdgeLine::Point(int nIndex) const
{
	assert(nIndex == 0 || nIndex == 1);
	if(nIndex) 
		return *m_pt2;
	return *m_pt1;
}


CGeomecDianaRunnerBase::CElementSetInfo::CElementSetInfo(ftn_int_t matidx, ftn_int_t geoidx, const CFormationBase *pFormation, ftn_int_t eltidx) : matidx(matidx), geoidx(geoidx), pFormation(pFormation), eltidx(eltidx)
{
  assert(eltidx > 0);
}

bool CGeomecDianaRunnerBase::CElementSetInfo::operator<(const CElementSetInfo& rhs) const
{
  if (matidx != rhs.matidx)
    return matidx < rhs.matidx;

  if (geoidx != rhs.geoidx)
    return geoidx < rhs.geoidx;

  if (pFormation != rhs.pFormation)
    return pFormation < rhs.pFormation;

  return eltidx < rhs.eltidx;
}

CGeomecDianaRunnerBase::CGroupInfo::CGroupInfo(const CFormationBase *pFormation, ftn_int_t eltsetidx) : pFormation(pFormation), eltsetidx(eltsetidx)
{
  assert(eltsetidx > 0);
}

bool CGeomecDianaRunnerBase::CGroupInfo::operator<(const CGroupInfo& rhs) const
{
  if (pFormation != rhs.pFormation)
    return pFormation < rhs.pFormation;

  return eltsetidx < rhs.eltsetidx;
}

CGeomecDianaRunnerBase::CDistributedElementSetInfo::CDistributedElementSetInfo(const CFormationBase *pFormation, ftn_int_t eltidx, const dia::IElementProperty *pMaterial, const dia::IElementProperty *pGeometry) : pFormation(pFormation), eltidx(eltidx)
{
  assert(eltidx > 0);
  assert(pMaterial);
  pProperty[0] = pMaterial;
  pProperty[1] = pGeometry;
}

bool CGeomecDianaRunnerBase::CDistributedElementSetInfo::operator<(const CDistributedElementSetInfo& rhs) const
{
  if (pFormation != rhs.pFormation)
    return pFormation < rhs.pFormation;

  if (pProperty[0]->Type() != rhs.pProperty[0]->Type())
    return pProperty[0]->Type() < rhs.pProperty[0]->Type();

  if (pProperty[0]->isInterfaceElementMaterial())
  {
    assert(dynamic_cast<const CInterfaceElementMaterial *>(pProperty[0]));
    assert(dynamic_cast<const CInterfaceElementMaterial *>(rhs.pProperty[0]));

    const CInterfaceElementMaterial *lhs_mat = static_cast<const CInterfaceElementMaterial *>(pProperty[0]);
    const CInterfaceElementMaterial *rhs_mat = static_cast<const CInterfaceElementMaterial *>(rhs.pProperty[0]);

    if (!lhs_mat->sameSlip(*rhs_mat))
      return *lhs_mat < *rhs_mat;
  }

  if (pProperty[1] != rhs.pProperty[1])
    return pProperty[1] < rhs.pProperty[1]; // note that we're only interested in 0 and 0-zero division

  return eltidx < rhs.eltidx;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static void FilosFatal(int /*erno*/, const char *errm, const char* /*proc*/, const char *args)
{
	char buf[2048];

	snprintf(buf, 2047, errm, args);

	QString str;
	str = QString("Fatal Filos error: %1").arg(buf);
	_m()->msg(str);
	
  FilosFatalHandler::instance()->Strategy().Execute(buf);
}

CGeomecDianaRunnerBase::CGeomecDianaRunnerBase(CMeshBase &meshbase,
											   CModelBase &model,
											   CDianaRunController& controller)
:	CDianaRunner(meshbase.Mesh()), 
    m_MeshBase(meshbase), 
	m_Model(model),
	m_controller(controller),
	m_bAnalysisSuccess(false),
  m_AllowCleanUpAfterRun(true),
	m_bPerformInitialStressCalculation(true),
  m_bBehaveLinearly(false),
  m_bBehaveLinearlyDirty(true)
{
	m_pFilosFatal = SetFilosFatalFunc(FilosFatal);
}

CGeomecDianaRunnerBase::~CGeomecDianaRunnerBase()
{
	SetFilosFatalFunc(m_pFilosFatal);

  // delete allocated element properties
  for(size_t i = 0; i < m_vcAllocatedElementProperties.size(); ++i)
    delete m_vcAllocatedElementProperties[i];
}

bool CGeomecDianaRunnerBase::WriteFullModel() const
{
	// we need to write the full model if we're starting at initial
	return (m_controller.StartStage().Initial() != FALSE);
}

const CDianaRunController& CGeomecDianaRunnerBase::Controller() const
{
	return m_controller;
}

void CGeomecDianaRunnerBase::SetLocation(const std::string& strLocation)
{
	m_strLocation = strLocation;
}

void CGeomecDianaRunnerBase::SetFileName(const std::string& strFileName)
{
	m_strFileName = strFileName;
}

bool CGeomecDianaRunnerBase::HasTemperatureValue(const geo::IElement& element) const
{
	// not for interface elements
	if(dynamic_cast<const geo::CInterfaceElement *> (&element)) return false;

	// otherwise, yes
	return true;
}

bool CGeomecDianaRunnerBase::HasConcentrationValue(const geo::IElement& element) const
{
	// not for interface elements
	if(dynamic_cast<const geo::CInterfaceElement *> (&element)) return false;

  const CFormationBase* pFormation = Model().Mesh().Formation(element);
  assert(pFormation);

  const CModelBase& model = Model();
  bool bVolumetric = false;
  for(CDepletionStageEntry::const_iterator it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
  {
    if(!it->Initial() && pFormation->Strain(*it).IsVolumetric())
    {
      bVolumetric = true;
      break;
    }
  }

  return bVolumetric;
}

bool CGeomecDianaRunnerBase::WriteBoundaryLoadsForSurfaceDesc(
	TStageMap &mpStages,
	const geo::CSurfaceDesc& surfdesc,
	const CPressureSupportNode &pnode)
{
  std::vector<const geo::ISurface*> vcSurfaces(surfdesc.TetSurfaceSize());
  for(int i = 0; i < surfdesc.TetSurfaceSize(); ++i)
    vcSurfaces[i] = &surfdesc.TetSurface(i);

  return WriteBoundaryLoadsForSurfaces(mpStages, vcSurfaces, pnode);
}

bool CGeomecDianaRunnerBase::WriteTetraBoundaryLoads(TStageMap& mpStages)
{
  assert(dynamic_cast<const CTetraModel*>(&Model()));
  const CTetraModel& model = static_cast<const CTetraModel&>(Model());
	const CTetraBoundary &boundary = static_cast<const CTetraBoundary &> (model.Boundary());
	const CPressureSupportNode &pnode = boundary.PressureSupportNode();

	// side surfaces
  int i;
	for(i = 0; i < boundary.SideSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = boundary.SideSurfaceDesc(i);
		if (!WriteBoundaryLoadsForSurfaceDesc(mpStages, surfdesc, pnode))
			return false;
	}

	// bottom surface
	const CTetraHorizonBase *pBottom = boundary.BottomHorizon();
	assert(pBottom != 0);

	for(i = 0; i < pBottom->OutputSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = pBottom->OutputSurface(i);
		if (!WriteBoundaryLoadsForSurfaceDesc(mpStages, surfdesc, pnode))
			return false;
	}

	// top surface
	const CTetraHorizonBase *pTop = boundary.TopHorizon();
	assert(pTop != 0);

	for(i = 0; i < pTop->OutputSurfaceSize(); i++)
	{
		const geo::CSurfaceDesc &surfdesc = pTop->OutputSurface(i);
		if (!WriteBoundaryLoadsForSurfaceDesc(mpStages, surfdesc, pnode))
			return false;
	}

  return true;
}

bool CGeomecDianaRunnerBase::WriteHexaHorizonBoundaryLoads(
	const C3DHorizon& hor,
    TStageMap& mpStages,
    const CPressureSupportNode& pnode)
{
  std::vector<const geo::ISurface*> vcSurfaces(hor.MeshedSurfaceSize());

  for(int i = 0; i < hor.MeshedSurfaceSize(); ++i)
    vcSurfaces[i] = &hor.MeshedSurface(i);

  if (!WriteBoundaryLoadsForSurfaces(mpStages, vcSurfaces, pnode))
    return false;

  return true;
}

bool CGeomecDianaRunnerBase::WriteHexaBoundaryLoads(TStageMap& mpStages)
{
  assert(dynamic_cast<const CHexaModel*>(&Model()));

  const CInterfaceBoundary& boundary = static_cast<const CInterfaceBoundary&>(Model().Boundary());
	const CPressureSupportNode &pnode = boundary.PressureSupportNode();

  // side surfaces
  std::vector<const geo::ISurface*> vcSideSurfaces = boundary.GetSideMeshSurfaces();
  for(size_t i = 0; i < vcSideSurfaces.size(); ++i)
  {
    std::vector<const geo::ISurface*> vcSurfaces(1);
    vcSurfaces[0] = vcSideSurfaces[i];
	if (!WriteBoundaryLoadsForSurfaces(mpStages, vcSurfaces, pnode))
      return false;
  }

  // bottom
  if (!WriteHexaHorizonBoundaryLoads(boundary.GetBottomHorizon(), mpStages, pnode))
    return false;

  // top
  if (!WriteHexaHorizonBoundaryLoads(boundary.GetTopHorizon(), mpStages, pnode))
    return false;

  return true;
}

bool CGeomecDianaRunnerBase::WriteBoundaryLoads(TStageMap &mpStages)
{
  assert(dynamic_cast<const CInterfaceBoundary*>(&Model().Boundary()));
  const CInterfaceBoundary& boundary = static_cast<const CInterfaceBoundary&>(Model().Boundary());
	if(boundary.CreateInterfaces())
	{
		return WriteInterfaceBoundaryLoads(mpStages, boundary, Model().Mesh());
	}

	m_mpProcessedNodes.clear();

	const CTetraModel *pTetraModel = dynamic_cast<const CTetraModel *> (&Model());

	if(pTetraModel)
		return WriteTetraBoundaryLoads(mpStages);

	return WriteHexaBoundaryLoads(mpStages);
}

IValueDomainScalar::TValueVec CGeomecDianaRunnerBase::ElementTemperatures(const geo::IElement& element, const CDepletionStage& stage) const
{
	// get the formation
	const CFormationBase* pFormation = Model().Mesh().Formation(element);
  assert(pFormation);

	// get the temperature values for the element
	return pFormation->EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
}

IValueDomainScalar::TValueVec CGeomecDianaRunnerBase::ElementConcentrations(const geo::IElement& element, const CDepletionStage& stage) const
{
  // get the formation
  const CFormationBase* pFormation = Model().Mesh().Formation(element);
  assert(pFormation);

  IValueDomainScalar::TValueVec vcValues;

  // get the concentration values for the element
  if (pFormation->Strain(stage).IsVolumetric())
  {
    vcValues = pFormation->Strain(stage).Component().ScalarData().ValueElement(element);
    if (vcValues.size() > 0 && vcValues[0].Valid())
      return vcValues;
  }

  vcValues.resize(element.NrOfPoints(), 0);
  return vcValues;
}

CGeomecDianaRunnerBase::TSolver CGeomecDianaRunnerBase::Solver() const
{
  return ITERATIVE;
}

bool CGeomecDianaRunnerBase::WriteInterfaceBoundaryLoads(
    TStageMap& mpStages,
    const CInterfaceBoundary& boundary,
    const CMeshBase& mesh)
{
  // see whether there are distributed boundary loads
	const CPressureSupportNode &pnode = boundary.PressureSupportNode();
	const CDisplacementSupportNode &dnode = boundary.DisplacementSupportNode();

  // always use distributions if there's no parent model
  bool bUserLoads = (Model().parentModel() == 0);

  if(!bUserLoads)
  {
    for(TStageMap::const_iterator it = mpStages.begin(); it != mpStages.end(); ++it)
    {
      if(pnode.Distributed(*it->first) ||
         dnode.Distributed(*it->first))
      {
        bUserLoads = true;
        break;
      }
    }
  }

  if(bUserLoads)
	return WriteBoundaryLoadsUsingDistributions(mpStages, boundary, mesh);

  return WriteBoundaryDisplacementLoadsFromParentModel(mpStages, boundary, mesh)
	  && WriteBoundaryStressLoadsFromParentModel(mpStages, boundary, mesh);
}

bool CGeomecDianaRunnerBase::WriteBoundaryStressLoadsFromParentModel(
    TStageMap& mpStages,
    const CInterfaceBoundary& boundary,
    const CMeshBase& /*mesh*/)
{
  assert(Model().parentModel());

  // find initial stage

  TStageMap::iterator initialIterator = mpStages.end();

  for(TStageMap::iterator it = mpStages.begin(); it != mpStages.end(); ++it)
  {
    const CDepletionStage& stage = *it->first;

    if (stage.IsMarkedAsInitial())
    {
      assert(initialIterator == mpStages.end());
      initialIterator = it;
#ifndef _DEBUG
      break;
#endif
    }
  }

  assert(initialIterator != mpStages.end());
  
  const CDepletionStage& parentInitialStage = Model().ParentEquivalentDepletionStage(*initialIterator->first);

  CStressTensorValueSet initialStress = Model().parentModel()->ResultRegister().TotalStress(parentInitialStage,
    Model().ParentLinearResults(Controller().AnalysisType()) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);




  for(TStageMap::iterator it = mpStages.begin(); it != mpStages.end(); ++it)
  {
    const CDepletionStage& stage = *it->first;
    dia::CLoadCase& lcase = *it->second;
    const CDepletionStage& parentStage = Model().ParentEquivalentDepletionStage(stage);

    CStressTensorValueSet stress = Model().parentModel()->ResultRegister().TotalStress(parentStage,
      Model().ParentLinearResults(Controller().AnalysisType()) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    // pressures
    assert(boundary.InterfaceElements());
    for(int i = 0; i < boundary.InterfaceElements()->ElementSize(); ++i)
    {
      const geo::IElement& elm = boundary.InterfaceElements()->Element(i);
      assert(dynamic_cast<const geo::CInterfaceElement*>(&elm));
      const geo::CInterfaceElement& iface = static_cast<const geo::CInterfaceElement&>(elm);
      assert(iface.FrontFace());
      const geo::IBody* pParent = 0;
      int nBodyFaceIndex = 0;
      const geo::CBodyTriangle* pBodyTriangle = dynamic_cast<const geo::CBodyTriangle*>(iface.FrontFace());
      if(pBodyTriangle)
      {
        pParent = pBodyTriangle->Parent();
        nBodyFaceIndex = pBodyTriangle->Index();
      }

      if(!pParent)
      {
        assert(dynamic_cast<const geo::CBodyQuadrilateral*>(iface.FrontFace()));
        const geo::CBodyQuadrilateral& bodyquad = static_cast<const geo::CBodyQuadrilateral&>(*iface.FrontFace());
        pParent = bodyquad.Parent();
        nBodyFaceIndex = bodyquad.Index();
      }

      assert(pParent);
      const geo::IFace& bodyface = *iface.FrontFace();

      geo::CVector vecPressureDirection;
      std::vector<double> vcPressureValues(bodyface.NrOfNodes());

      for(int n = 0; n < bodyface.NrOfNodes(); ++n)
      {
        const geo::INode& node = bodyface.Node(n);

        CStressTensor stresstensor = stress.ValuePoint(node, 0);

        if (it != initialIterator) // in D1 and higher we need to write the difference with D0
        {
          stresstensor = stresstensor - initialStress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);
        }

        geo::CVector vecPressure = -(stresstensor.Matrix() * bodyface.Normal().UnitVector());

        vecPressure = vecPressure * 1e6;
        vcPressureValues[n] = vecPressure.Length();
        if(vecPressureDirection.Empty())
          vecPressureDirection = vecPressure.UnitVector();
      }

      assert(!vecPressureDirection.Empty());
      new dia::CBodyFaceLoad(lcase, vcPressureValues, vecPressureDirection, *pParent, nBodyFaceIndex);
    }
  }

  return true;
}

bool CGeomecDianaRunnerBase::WriteBoundaryDisplacementLoadsFromParentModel(
    TStageMap& mpStages,
    const CInterfaceBoundary& boundary,
    const CMeshBase& mesh)
{
  // Note: according to Diana all D1+ loads in the zoomin models should be written as difference with D0
  // according to Shell, however, the displacements appear to be fine
  // so for now we leave this as it is, and if we get a bug-report about it: solution analogous to WriteBoundaryStressLoadsFromParentModel

  assert(Model().parentModel());

  for(TStageMap::iterator it = mpStages.begin(); it != mpStages.end(); ++it)
  {
    const CDepletionStage& stage = *it->first;
    dia::CLoadCase& lcase = *it->second;
    const CDepletionStage& parentStage = Model().ParentEquivalentDepletionStage(stage);

    CVectorValueSet displa = Model().parentModel()->ResultRegister().Displacement(parentStage,
      Model().ParentLinearResults(Controller().AnalysisType()) ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

    // displacements
    for(int i = 0; i < boundary.InterfaceNodeSize(); ++i)
    {
      const geo::INode& bnode = boundary.InterfaceNode(i);

      // get the support
      assert(mesh.Mesh().HasTranslationSupport(bnode.Index()));
      const geo::CTranslationSupport& support = mesh.Mesh().TranslationSupport(bnode.Index());

      geo::CVector vecDispla;

      std::vector<int> vcElementIndex = Model().parentModel()->Mesh().Mesh().ElementsAt(bnode);
      for(size_t j = 0; j < vcElementIndex.size(); ++j)
      {
        const geo::IElement& elm = Model().parentModel()->Mesh().Mesh().Element(vcElementIndex[j]);
        vecDispla = GetDisplacementVector(displa, elm, bnode);
        if(!vecDispla.Empty())
          break;
      }

      if(!vecDispla.Empty())
      {
        // create the load
        double dDisplaLength = vecDispla.Length();
        if(dDisplaLength > EPS)
          new dia::CPrescribedDisplacement(lcase, dDisplaLength, vecDispla.UnitVector(), support);
      }
    }

  }

  return true;
}

bool CGeomecDianaRunnerBase::WriteBoundaryLoadsUsingDistributions(
	TStageMap &mpStages, 
	const CInterfaceBoundary &boundary, 
	const CMeshBase &mesh)
{
	const CPressureSupportNode &pnode = boundary.PressureSupportNode();
	const CDisplacementSupportNode &dnode = boundary.DisplacementSupportNode();

	bool bInitialDistributionDISP = dnode.Distributed(*(*mpStages.begin()).first);
	bool bInitialDistributionSTRESS = pnode.Distributed(*(*mpStages.begin()).first);
	TStageMap::iterator it;
	//for(int i = 0; i < boundary.InterfaceElements().size(); i++)
  int i;
	for(i = 0; i < boundary.InterfaceNodeSize(); i++)
	{
		//const geo::IElement &element = mesh.Mesh().Element(boundary.InterfaceElements()[i]);
		int iNode = boundary.InterfaceNode(i).Index();
		
		for(it = mpStages.begin(); it != mpStages.end(); it++)
		{
			// first the displacements:
			if(dnode.Distributed(*(*it).first))
			{ // a distribution was dropped on the node for this stage
				CreatePrescDisplLoad(dnode, iNode, mesh.Mesh(), *(*it).first, *(*it).second, true, bInitialDistributionDISP);
			}
			else
			{
				CreatePrescDisplLoad(dnode, iNode, mesh.Mesh(), *(*it).first, *(*it).second, false, bInitialDistributionDISP);
			}
		}
	}

	assert(boundary.InterfaceElements());
	for(i = 0; i < boundary.InterfaceElements()->ElementSize(); i++)
	{
		//const geo::IElement &element = mesh.Mesh().Element(boundary.InterfaceElements()[i]);
		const geo::IElement &element = boundary.InterfaceElements()->Element(i);
		for(it = mpStages.begin(); it != mpStages.end(); it++)
		{
			// next the stresses
			if(pnode.Distributed(*(*it).first))
			{
				CreateBodyFaceLoad(pnode, element, *(*it).first, *(*it).second, true, bInitialDistributionSTRESS); 
			}
			else
			{
				CreateBodyFaceLoad(pnode, element, *(*it).first, *(*it).second, false, bInitialDistributionSTRESS); 
			}
		}

	}

	return true;
}

bool CGeomecDianaRunnerBase::WriteBoundaryLoadsForSurfaces(
	TStageMap &mpStages,
	const std::vector<const geo::ISurface*>& vcSurface,
	const CPressureSupportNode &pnode)
{
  bool retval = true;

  if(!vcSurface.empty())
  {
    const CPressureSupportNode::CFaceSupportDef &facedef =
      pnode.FaceSupportDef(CPressureSupportNode::CSurfaceDef(*vcSurface[0]));

	  switch(facedef.SupportType())
	  {
	  case CPressureSupportNode::CFaceSupportDef::ST_NORMAL:
	  case CPressureSupportNode::CFaceSupportDef::ST_FULL:
	  case CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL:
    case CPressureSupportNode::CFaceSupportDef::ST_VERTICAL:
			  retval = WritePrescribedDisplacements(mpStages, vcSurface, facedef.SupportType());
		  break;
	  case CPressureSupportNode::CFaceSupportDef::ST_STRESS:
      {
        for(size_t i = 0; i < vcSurface.size(); i++)
        {
	        const geo::ISurface &surface = *vcSurface[i];
  			  WriteBoundaryPressure(mpStages, surface, facedef);
        }
      }
		  break;
	  case CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED:
		  // do nothing
		  break;
	  default:
		  assert(FALSE);
	  }

	  m_progress->Step();
  }

	return retval;
}

void CGeomecDianaRunnerBase::CreatePrescDisplLoad(const CDisplacementSupportNode &node, 
												int nNode,
												const geo::IMesh &mesh,
												const CDepletionStage &stage,
												dia::CLoadCase &lc,
												bool bDistrAvailable,
												bool bDistrAvInitial)
{
	geo::CVector disp;
	if(stage.Initial())
		DisplacementVector(node, nNode, mesh, stage, bDistrAvailable, disp);
	else
	{
		geo::CVector disp_init;
		DisplacementVector(node, nNode, mesh, stage.InitialStage(), bDistrAvInitial, disp_init);
		DisplacementVector(node, nNode, mesh, stage, bDistrAvailable, disp);

    if(!disp_init.Empty())
		  disp = disp - disp_init;
	}

  if(disp != geo::CVector::NullVector)
  {
  	const geo::CTranslationSupport &s = mesh.TranslationSupport(nNode);
    new dia::CPrescribedDisplacement(lc, disp.Length(), disp.UnitVector(), s);
  }
}

void CGeomecDianaRunnerBase::CreateBodyFaceLoad(const CPressureSupportNode &node,
											  const geo::IElement &element,
											  const CDepletionStage &stage,
											  dia::CLoadCase &lc,
											  bool bDistrAvailable,
											  bool bDistrAvInitial)
{
	// Element from boundary
	const geo::CInterfaceElement &bie = dynamic_cast<const geo::CInterfaceElement&>(element);
	assert(&bie);
	assert(bie.FrontFace() == bie.BackFace());

  const geo::IFace& face = *bie.FrontFace();
	const geo::CBodyTriangle* pBodyTriangle = dynamic_cast<const geo::CBodyTriangle*>(&face);
  const geo::CBodyQuadrilateral* pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral*>(&face);
  assert(pBodyTriangle || pBodyQuad);

  const geo::IBody* pBody;
  int nFaceIndex;
  if(pBodyTriangle)
  {
    pBody = pBodyTriangle->Parent();
    nFaceIndex = pBodyTriangle->Index();
  }
  else
  {
    pBody = pBodyQuad->Parent();
    nFaceIndex = pBodyQuad->Index();
  }

  std::vector<geo::CVector> vcPressure;
	std::vector<double> Xvals, Yvals, Zvals;
	
	if(stage.Initial())
	{
		StressVectors(node, element, stage, face, bDistrAvailable, vcPressure);
	}
	else
	{
    std::vector<geo::CVector> vcIniPressure;
		StressVectors(node, element, stage.InitialStage(), face, bDistrAvInitial, vcIniPressure);
		StressVectors(node, element, stage, face, bDistrAvailable, vcPressure);

    assert(vcIniPressure.size() == vcPressure.size());
    for(size_t i = 0; i < vcPressure.size(); ++i)
      vcPressure[i] -= vcIniPressure[i];
	}

  for(size_t i = 0; i < vcPressure.size(); ++i)
  {
    Xvals.push_back(vcPressure[i].X());
    Yvals.push_back(vcPressure[i].Y());
    Zvals.push_back(vcPressure[i].Z());
  }
	
	new dia::CBodyFaceLoad(lc, Xvals, geo::CVector::Xaxis, *pBody, nFaceIndex);
	new dia::CBodyFaceLoad(lc, Yvals, geo::CVector::Yaxis, *pBody, nFaceIndex);
	new dia::CBodyFaceLoad(lc, Zvals, geo::CVector::Zaxis, *pBody, nFaceIndex);
}

void CGeomecDianaRunnerBase::StressVectors(const CPressureSupportNode &node,
										const geo::IElement &/*element*/, 
										const CDepletionStage &stage,
										const geo::IFace &face,
										bool bDistrAvailable,
                    std::vector<geo::CVector>& vcPressure)
{
  vcPressure.resize(face.NrOfNodes());
  for(int i = 0; i < face.NrOfNodes(); ++i)
  {
	  if(bDistrAvailable)
      vcPressure[i] = node.StressFromDistribution(face.Node(i), stage, face.NormalInPoint(face.Node(i)));
	  else
      vcPressure[i] = geo::CVector::NullVector;
  }
}

void CGeomecDianaRunnerBase::DisplacementVector(const CDisplacementSupportNode &node,
											  int nNode,
											  const geo::IMesh &mesh,
											  const CDepletionStage &stage,
											  bool bDistrAvailable,
											  geo::CVector &displacement)
{
	if(bDistrAvailable)
		displacement = node.DisplacementFromDistribution(mesh.Point(nNode), stage);
	else
		displacement = geo::CVector::NullVector;
}

bool CGeomecDianaRunnerBase::WritePrescribedDisplacements(
		TStageMap &mpStages,
		const std::vector<const geo::ISurface*>& vcSurface,
		CPressureSupportNode::CFaceSupportDef::TSupportType /*suptype*/)
{
	// must have at least one depletion stage
	if(mpStages.size() < 2) return true;

	// should be dealing with a tetramodel
	const CInterfaceBoundary &boundary = static_cast<const CInterfaceBoundary &> (Model().Boundary());
	const CDisplacementSupportNode &dnode = boundary.DisplacementSupportNode();

	CComboSurface surface(vcSurface);


//	for(int i = 0; i < surfacedesc.TetSurfaceSize(); i++)
//	{
//		const geo::CTetSurface &surface = surfacedesc.TetSurface(i);
		TSupportDefMap mpSupportDef;

		//for(int j = 0; j < surface.PointSize(); j++)
		for(int j = 0; j < surface.PointSize(); j++)
		{
			// should be mesh nodes (output surface)
			const geo::INode &pt = dynamic_cast<const geo::INode &> (surface.Point(j));
			const CDisplacementSupportNode::CNodalSupportDef *pSupDef = dnode.NodalSupportDef(pt);
			if(pSupDef)
			{
				// this point is one of the corner nodes the user specified
				VERIFY(mpSupportDef.insert(std::make_pair(&pt, pSupDef)).second);
			}
		}

		const CDepletionStage* pStage = &Controller().StartStage();
    if(pStage->Initial())
    {
      assert(!pStage->Last());
      pStage = &pStage->Next();
    }

		do
		{
			TStageMap::iterator it = mpStages.find(pStage);
			assert(it != mpStages.end());
			// see whether we have to interpolate (i.e. if there are nodes where
			// an explicit vector has been supplied), otherwise we will get all displacement
			// vectors from the global strain tensor
			bool bMustInterpolate = false;

			for(int j = 0; j < surface.PointSize(); j++)
			{
				const geo::INode &pt = dynamic_cast<const geo::INode &> (surface.Point(j));
				TSupportDefMap::iterator itSupDef = mpSupportDef.find(&pt);

				if(itSupDef != mpSupportDef.end())
				{
					const CDisplacementSupportNode::CNodalSupportDef *pSupDef = itSupDef->second;

					// see whether we can still do without interpolating
					bMustInterpolate = bMustInterpolate || 
						(pSupDef->SupportDef(*it->first).SupportType() != CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_GLOBAL);
				}
			}

			geo::CMatrix MatPreMult = CreateGlobalTensorPreMultMatrix(dnode, *it->first);

			if(bMustInterpolate || MatPreMult.Empty())
			{
        if (!WritePrescribedDisplacementsFromInterpolation(Mesh(), surface, dnode, *it->first, *it->second, MatPreMult, mpSupportDef))
          return false;
			}
			else
			{
				assert(MatPreMult.ColumnSize() == 3 && MatPreMult.RowSize() == 3);
				WritePrescribedDisplacementsFromGlobalTensor(Mesh(), surface, dnode, *it->first, *it->second, MatPreMult);
			}

			if(pStage->Last() || pStage == &Controller().EndStage())
				pStage = 0;
			else
				pStage = &pStage->Next();

		} while(pStage);
	//}
    return true;
}

geo::CMatrix CGeomecDianaRunnerBase::CreateGlobalTensorPreMultMatrix(
		const CDisplacementSupportNode &dnode,
		const CDepletionStage &stage)
{
  return dnode.CreateGlobalTensorPreMultMatrix(stage);
}

void CGeomecDianaRunnerBase::WritePrescribedDisplacementForSupport(
		dia::CLoadCase& lcase,
		const geo::IVector& vecDispla,
		const geo::CTranslationSupport& support)
{
	dia::CPrescribedDisplacement* pPrescr = 0;
	if(vecDispla.Length() > EPS)
	{
		geo::CVector vecDisplaProj = support.Projection(vecDispla);

		if(vecDisplaProj.Length() > EPS)
		{
			pPrescr = new dia::CPrescribedDisplacement(lcase, vecDisplaProj.Length(), vecDisplaProj, support);
		}
	}

	if(!pPrescr)
	{
		// write dummy (i.e. zero length) prescribed displacement
//		new dia::CPrescribedDisplacement(lcase, 0, support.Direction(0), support);
	}
}

void CGeomecDianaRunnerBase::WritePrescribedDisplacementsFromGlobalTensor(
		const geo::IMesh &mesh,
		const geo::ISurface &surface,
		const CDisplacementSupportNode &dnode,
		const CDepletionStage &stage,
		dia::CLoadCase &lcase,
		const geo::IMatrix &MatPreMult)
{
	for(int i = 0; i < surface.PointSize(); i++)
	{
		const geo::INode &pt = dynamic_cast<const geo::INode &> (surface.Point(i));

		if(mesh.HasTranslationSupport(pt.Index()))
		{
			if(m_mpProcessedNodes.find(CStagePointCombi(stage, pt)) == m_mpProcessedNodes.end())
			{
				geo::CVector vecDispla =
					CDisplacementSupportNode::DisplacementFromStrainTensor(pt, dnode.ReferencePoint(), MatPreMult);

				m_mpProcessedNodes.insert(std::make_pair(CStagePointCombi(stage, pt),
					CDisplacementSupportNode::CNodalSupportDef::CSupportDef()));

				const geo::CTranslationSupport &support = mesh.TranslationSupport(pt.Index());
				WritePrescribedDisplacementForSupport(lcase, vecDispla, support);
			}
		}
	}
}

bool CGeomecDianaRunnerBase::WritePrescribedDisplacementsFromInterpolation(
		const geo::IMesh &mesh,
		const geo::ISurface &surface,
		const CDisplacementSupportNode &dnode,
		const CDepletionStage &stage,
		dia::CLoadCase &lcase,
		const geo::IMatrix &MatPreMult,
		TSupportDefMap &mpSupportDef)
{
	if(mpSupportDef.size() == 0)
		return true;
	// get all sorted edge nodes from the surface
	std::vector<std::vector<const geo::IPoint *> > vcAllEdges = surface.SortedEdgePoints();

//	for(size_t current_edge = 0; current_edge < vcTempAllEdges.size(); current_edge++)
//	{
//		std::vector<const geo::IPoint *> &vcEdge = vcTempAllEdges[current_edge];
//
//		// ****************************
//		// First we test if any of the points in this edge can be found in the support points map.
//		// If none of the points in this edge is supported, we assume that this edge is an edge inside an edge.
//		// In that case the edge will be ignored from now on.
//		for(size_t l = 0; l < vcEdge.size(); l++)
//		{
//			if(mpSupportDef.find((geo::INode *)vcEdge[l]) != mpSupportDef.end())
//			{
//				vcAllEdges.push_back(vcEdge);
//				break;
//			}
//		}
//		// ****************************
//	}

  if (vcAllEdges.size() == 0)
  {
    if (dynamic_cast<CHexaModel *>(&Model()))
    {
      // we found no edge, possibly due to tyings; we try different epsilons (if need be) until we get one edge (we assume that we don't go from NO edge to several rings)
      double epsilon[] = { 0.0001, 0.001, 0.01 };

      for (int i = 0; i < 3; ++i)
      {
        vcAllEdges = surface.SortedEdgePoints(true, epsilon[i]);
        if (vcAllEdges.size() == 1)
          break;
      }
    }
    if (vcAllEdges.size() != 1)
    {
      _m()->error("Error: unable to detect surface edge. Please check your mesh regions.", MB_ICONEXCLAMATION);
      return false;
    }
  }

	for(size_t current_edge = 0; current_edge < vcAllEdges.size(); current_edge++)
	{

		std::vector<const geo::IPoint *> &vcEdge = vcAllEdges[current_edge];

		bool bBreak = false;
		
		
		for(int i = 0; i < vcEdge.size(); i++)
		{
			const geo::INode &pt = dynamic_cast<const geo::INode &> (*vcEdge[i]);
			if(m_mpProcessedNodes.find(CStagePointCombi(stage, pt)) == m_mpProcessedNodes.end())
			{
				TSupportDefMap::iterator it = mpSupportDef.find(&pt);
				if(it == mpSupportDef.end())
				{
					if(mpSupportDef.size() > 0)
					{
						// we must interpolate for this node
						int iPrev;
						int iNext;
						int iLast;
						double dDistPrev = 0;
						double dDistNext = 0;

						// find the previous user-specified node
						iPrev = i;
						for(;;)
						{
							iLast = iPrev;
							if(--iPrev < 0) iPrev = vcEdge.size() - 1;
							dDistPrev += vcEdge[iPrev]->Distance(*vcEdge[iLast]);
							if(mpSupportDef.find((geo::INode *)vcEdge[iPrev]) != mpSupportDef.end() ||
								iPrev == i)
							{
								// if iPrev == i the current edge does not have any definition points
								// assume it is an inner ring, and let the procedure that handles internal points
								// (below) deal with it, which means that we're ignoring it here.
								if(iPrev == i)
									bBreak = true;
								break;
							}
						}

						if(bBreak) break;

						// find the next user-specified node
						iNext = i;
						for(;;)
						{
							iLast = iNext;
							if(++iNext >= vcEdge.size()) iNext = 0;
							dDistNext += vcEdge[iNext]->Distance(*vcEdge[iLast]);
							if(mpSupportDef.find((geo::INode *)vcEdge[iNext]) != mpSupportDef.end()) break;
						}

						assert(iPrev != i);
						assert(iNext != i);
						assert(dDistPrev > EPS);
						assert(dDistNext > EPS);

						TSupportDefMap::iterator itPrev = mpSupportDef.find((geo::INode *) vcEdge[iPrev]);
						assert(itPrev != mpSupportDef.end());

						TSupportDefMap::iterator itNext = mpSupportDef.find((geo::INode *) vcEdge[iNext]);
						assert(itNext != mpSupportDef.end());

						CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdefPrev = itPrev->second->SupportDef(stage);
						CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdefNext = itNext->second->SupportDef(stage);

						CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef =
							supdefPrev.Interpolate(supdefNext, dDistPrev, dDistNext, pt, MatPreMult, dnode.ReferencePoint());

						const geo::IVector &vecDispla = supdef.LocalVector();
						m_mpProcessedNodes.insert(std::make_pair(CStagePointCombi(stage, pt), supdef));
						const geo::CTranslationSupport &support = mesh.TranslationSupport(pt.Index());
						WritePrescribedDisplacementForSupport(lcase, vecDispla, support);
					}
					else
					{
						m_mpProcessedNodes.insert(std::make_pair(CStagePointCombi(stage, pt),
							CDisplacementSupportNode::CNodalSupportDef::CSupportDef()));
					}
				}
				else
				{
					// use the user-specified value for this node
					CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef = it->second->SupportDef(stage);

					geo::CVector vecDispla = supdef.Vector(pt, MatPreMult, dnode.ReferencePoint());
					m_mpProcessedNodes.insert(std::make_pair(CStagePointCombi(stage, pt), supdef));
					const geo::CTranslationSupport &support = mesh.TranslationSupport(pt.Index());
					WritePrescribedDisplacementForSupport(lcase, vecDispla, support);
				}
			}
		}

		if(bBreak) continue;
	}

	// we are left with the nodes in the middle of the surface

	// test that all edge nodes have been processed and are present in the map
#ifdef _DEBUG
	for(size_t jDbg = 0; jDbg < vcAllEdges.size(); jDbg++)
	{
		std::vector<const geo::IPoint *> &vcEdge = vcAllEdges[jDbg];

		bool bMatch = false;
		for(size_t l = 0; l < vcEdge.size(); l++)
		{
			if(mpSupportDef.find((geo::INode *)vcEdge[l]) != mpSupportDef.end())
			{
				bMatch = true;
				break;
			}
		}
		if(!bMatch)
			continue;

		for(size_t iDbg = 0; iDbg < vcEdge.size(); iDbg++)
			assert(m_mpProcessedNodes.find(CStagePointCombi(stage, *vcEdge[iDbg])) != m_mpProcessedNodes.end());
	}
#endif

	// find a plane through the surface to do the interpolation on
	// the plane is defined by the surface's average normal
	// and either the X-, Y- or Z-axis, whichever has the smallest
	// cosine of the angle with the surface's average normal (dotproduct)
  geo::CVector vecAverageNormal = surface.AverageNormal();
  if(vecAverageNormal == geo::CVector::NullVector)
    vecAverageNormal = geo::CVector::Xaxis; // probably a side surface that goes around the entire model (single side surface)
	geo::CVector vecSurfaceNormal = vecAverageNormal.UnitVector();
	const geo::IVector *pOtherVec = &geo::CVector::Xaxis;
	double dDotSmallest = vecSurfaceNormal.DotProduct(*pOtherVec);

	double dDot = vecSurfaceNormal.DotProduct(geo::CVector::Yaxis);
	if(fabs(dDot) < fabs(dDotSmallest))
	{
		dDotSmallest = dDot;
		pOtherVec = &geo::CVector::Yaxis;
	}

	dDot = vecSurfaceNormal.DotProduct(geo::CVector::Zaxis);
	if(fabs(dDot) < fabs(dDotSmallest)) pOtherVec = &geo::CVector::Zaxis;

  std::map<const geo::IPoint *, int> mpIndices = surface.SubSurfaceIndices(vcAllEdges);

	// now walk over all surface points and find a proper value for them
	for(int i = 0; i < surface.PointSize(); i++)
	{
		const geo::INode &pt = dynamic_cast<const geo::INode &> (surface.Point(i));

		if(mesh.HasTranslationSupport(pt.Index()))
		{
			// make sure it's not an edge point
			if(m_mpProcessedNodes.find(CStagePointCombi(stage, pt)) == m_mpProcessedNodes.end())
			{
				std::map<const geo::IPoint *, int>::iterator itmap1 = mpIndices.find(&pt);
				assert(itmap1 != mpIndices.end());
				assert(itmap1->second >= 0 && itmap1->second < vcAllEdges.size());

				std::vector<const geo::IPoint *> &vcEdge = vcAllEdges[itmap1->second];

				// create a plane through this point spanned by the two vectors
				geo::CPlane plane(pt, vecSurfaceNormal, *pOtherVec);

				// store the intersection points with the plane here
				geo::CCoordinateMap<geo::CPoint, CEdgeLine> mpIntersectionPoints;

				// now walk over the edge lines and see if they intersect with the plane
				size_t j, k;
				for(j = vcEdge.size() - 1, k = 0; k < vcEdge.size(); j = k++)
				{
					const geo::IPoint *pt1 = vcEdge[j];
					const geo::IPoint *pt2 = vcEdge[k];
					CEdgeLine l(*pt1, *pt2);

					geo::CPoint ptIntersect = plane.RealIntersection(l, true, true);
					if(!ptIntersect.Empty()) mpIntersectionPoints.insert(std::make_pair(ptIntersect, l));
				}

				if(mpIntersectionPoints.size() > 2)
				{
					// the plane cut through multiple edges (i.e. more than two),
					// so find the two nearest, but on either side of the point

					// find the two appropriate points

					// this map will contain vector from the point (pt) to each intersection
					// with the edge intersection points
					std::map<geo::IVector *, const geo::IPoint *> mpVectors;

					// find the one with the smallest length and put a pointer in pvecSmallest
					// this vector then represents the nearest intersection point
					double dSmallest = DBL_MAX;
					geo::IVector *pvecSmallest = 0;
					for(geo::CCoordinateMap<geo::CPoint, CEdgeLine>::iterator it = mpIntersectionPoints.begin(); it != mpIntersectionPoints.end(); it++)
					{
						std::map<geo::IVector *, const geo::IPoint *>::iterator itm = mpVectors.insert(std::map<geo::IVector *, const geo::IPoint *>::value_type(new geo::CVector(pt, it->first), &(it->first))).first;
						double d = pt.SquareDistance(it->first);
						if(mpVectors.size() == 1 || d < dSmallest)
						{
							dSmallest = d;
							pvecSmallest = itm->first;
						}
					}

					assert(pvecSmallest != 0);
					assert(mpVectors.size() == mpIntersectionPoints.size());

					// now find the next shortest vector, but it also has to be pointing 'the other way'
					// this is a bit tricky in 3D, so we use the dot-product on the first shortest vector
					// (pvecSmallest) and see if it is negative (which then thus defines 'the other way')
					bool bInit = true;
					geo::IVector *pvecSmallestInverse = 0;
					for(std::map<geo::IVector *, const geo::IPoint *>::iterator itm = mpVectors.begin(); itm != mpVectors.end(); itm++)
					{
						if(itm->first != pvecSmallest && itm->first->DotProduct(*pvecSmallest) < 0)
						{
							double dLength = itm->first->SquareLength();
							if(bInit)
							{
								bInit = false;
								dSmallest = dLength;
								pvecSmallestInverse = itm->first;
							}
							else
							{
								if(dLength < dSmallest)
								{
									dSmallest = dLength;
									pvecSmallestInverse = itm->first;
								}
							}
						}
					}

          // wedx 19092012: in some rare cases (e.g. mantis #3649) there is no vector pointing 'the other way'
          // so simply take the smallest
					for(std::map<geo::IVector *, const geo::IPoint *>::iterator itm = mpVectors.begin(); itm != mpVectors.end(); itm++)
					{
						if(itm->first != pvecSmallest)
						{
							double dLength = itm->first->SquareLength();
							if(bInit)
							{
								bInit = false;
								dSmallest = dLength;
								pvecSmallestInverse = itm->first;
							}
							else
							{
								if(dLength < dSmallest)
								{
									dSmallest = dLength;
									pvecSmallestInverse = itm->first;
								}
							}
						}
					}

					assert(pvecSmallestInverse != 0);

					// the two vectors (pvecSmallest and pvecSmallestInverse) represent the two points
					// so we can remove all other points from the map
					// this vector will contain the key points we need to delete from the map
					std::vector<const geo::IPoint *> vcDelete;
					std::map<geo::IVector *, const geo::IPoint *>::iterator itfind;
					itfind = mpVectors.find(pvecSmallest);
					assert(itfind != mpVectors.end());
					const geo::IPoint *pLeave1 = itfind->second;
					itfind = mpVectors.find(pvecSmallestInverse);
					assert(itfind != mpVectors.end());
					const geo::IPoint *pLeave2 = itfind->second;
          geo::CCoordinateMap<geo::CPoint, CEdgeLine>::iterator itmap2;
					for(itmap2 = mpIntersectionPoints.begin(); itmap2 != mpIntersectionPoints.end(); itmap2++)
					{
						if(*pLeave1 != itmap2->first && *pLeave2 != itmap2->first) vcDelete.push_back(&(itmap2->first));
					}

					assert(!vcDelete.empty());
					assert(vcDelete.size() == (mpIntersectionPoints.size() - 2));
					for(size_t i2 = 0; i2 < vcDelete.size(); i2++)
					{
						itmap2 = mpIntersectionPoints.find(*vcDelete[i2]);
						assert(itmap2 != mpIntersectionPoints.end());
						mpIntersectionPoints.erase(itmap2);
					}
				}

        if(!mpIntersectionPoints.empty())
        {
				  CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef;
          bool bValidFound = true;

				  if(mpIntersectionPoints.size() == 1)
				  {
					  // this can happen in very specific cases, e.g. when we are dealing with
					  // a side surface that goes completely around the model (sort of ring)
					  // we will simply use the value from the single point
					  // (sort of nearest neighbour)
					  geo::CCoordinateMap<geo::CPoint, CEdgeLine>::iterator it = mpIntersectionPoints.begin();
					  const geo::IPoint &pt2 = it->second.First();
					  TStagePointSupportMap::iterator its = m_mpProcessedNodes.find(CStagePointCombi(stage, pt2));
            if(its == m_mpProcessedNodes.end())
              its = m_mpProcessedNodes.find(CStagePointCombi(stage, it->second.Second()));

					  if(its == m_mpProcessedNodes.end())
              bValidFound = false;
            else
					    supdef = its->second;
				  }
				  else
				  {
					  // interpolate supportdefs for the two newly found points

					  // first point
					  geo::CCoordinateMap<geo::CPoint, CEdgeLine>::iterator it = mpIntersectionPoints.begin();
					  const geo::IPoint &pt11 = it->second.First();
					  const geo::IPoint &pt12 = it->second.Second();
					  const geo::IPoint &pt1 = it->first;

            try
            {
					    CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef1 =
						    InterpolateSupportDef(pt1, pt11, pt12, MatPreMult, stage, dnode);

					    // second point
					    it++;
					    const geo::IPoint &pt21 = it->second.First();
					    const geo::IPoint &pt22 = it->second.Second();
					    const geo::IPoint &pt2 = it->first;

					    CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef2 =
						    InterpolateSupportDef(pt2, pt21, pt22, MatPreMult, stage, dnode);

					    // interpolate supportdef for the actual point from the two interpolated points
					    double dDist1 = pt.Distance(pt1);
					    double dDist2 = pt.Distance(pt2);

					    supdef = supdef1.Interpolate(supdef2, dDist1, dDist2, pt, MatPreMult, dnode.ReferencePoint());
            }
            catch(std::exception&)
            {
              bValidFound = false;
            }
				  }

          if(bValidFound)
          {
				    // and create the load
				    geo::CVector vecDispla = supdef.Vector(pt, MatPreMult, dnode.ReferencePoint());
				    m_mpProcessedNodes.insert(std::make_pair(CStagePointCombi(stage, pt), supdef));
				    const geo::CTranslationSupport &support = mesh.TranslationSupport(pt.Index());
				    WritePrescribedDisplacementForSupport(lcase, vecDispla, support);
          }
        }
			}
		}
	}

  return true;
}

CDisplacementSupportNode::CNodalSupportDef::CSupportDef CGeomecDianaRunnerBase::InterpolateSupportDef(
		const geo::IPoint &pt,
		const geo::IPoint &pt1,
		const geo::IPoint &pt2,
		const geo::IMatrix &MatPreMult,
		const CDepletionStage &stage,
		const CDisplacementSupportNode &dnode)
{
	TStagePointSupportMap::iterator it = m_mpProcessedNodes.find(CStagePointCombi(stage, pt1));
	if(it == m_mpProcessedNodes.end())
    throw std::exception();

	CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef1 = it->second;

	it = m_mpProcessedNodes.find(CStagePointCombi(stage, pt2));
	if(it == m_mpProcessedNodes.end())
    throw std::exception();

	CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef2 = it->second;

	// interpolate
	double dist1 = pt1.Distance(pt1);
	double dist2 = pt1.Distance(pt2);
	CDisplacementSupportNode::CNodalSupportDef::CSupportDef supdef =
		supdef1.Interpolate(supdef2, dist1, dist2, pt, MatPreMult, dnode.ReferencePoint());

	return supdef;
}

geo::CVector CGeomecDianaRunnerBase::PressureVector(
		const geo::IFace &face,
		const CDepletionStage &stage,
		const geo::ISurface &surface)
{
	// should be dealing with a tetramodel
	const CInterfaceBoundary &boundary = static_cast<const CInterfaceBoundary &> (Model().Boundary());
	const CPressureSupportNode &pnode = boundary.PressureSupportNode();

	const CPressureSupportNode::CFaceSupportDef &facedef =
		pnode.FaceSupportDef(CPressureSupportNode::CSurfaceDef(surface));

	assert(facedef.SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS);
	const CPressureSupportNode::CFaceSupportDef::CSupportDef &supdef = facedef.SupportDef(stage);

	geo::CVector ret;

	switch(supdef.PressureType())
	{
	case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_UNLOADED:
		ret = geo::CVector(0, 0, 0);
		break;
	case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL:
		ret = supdef.LocalVector();
		break;
	case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_GLOBAL:
		assert(pnode.GlobalTensorDefined(stage));
		ret = -(pnode.GlobalTensor(stage).Matrix() * face.Normal().UnitVector());
		break;
	default:
		assert(FALSE);
		break;
	}

	return ret;
}

void CGeomecDianaRunnerBase::WriteBoundaryPressureEntry(
		const CDepletionStage &stage,
		dia::CLoadCase &lcase,
		const geo::ISurface &surface)
{
	for(int i = 0; i < surface.FaceSize(); i++)
	{
		const geo::CBodyTriangle &face = dynamic_cast<const geo::CBodyTriangle &> (surface.Face(i));
		geo::CVector vecPressure = PressureVector(face, stage, surface);
		if(vecPressure.Length() > EPS)
			new dia::CBodyFaceLoad(lcase, vecPressure.Length() * 1e6, vecPressure.UnitVector(), *face.Parent(), face.Index());
	}
}

void CGeomecDianaRunnerBase::WriteBoundaryPressure(
		TStageMap &mpStages,
		const geo::ISurface &surface,
		const CPressureSupportNode::CFaceSupportDef &facedef)
{
	assert(facedef.SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS);


//	assert(mpStages.size() > 1);
//	assert(!Model().InitialDepletionStage().Last());
//	const CDepletionStage *pStage = &Model().InitialDepletionStage();
	const CDepletionStage* pStage = &Controller().StartStage();
	while(pStage)
	{
		TStageMap::iterator it = mpStages.find(pStage);
		assert(it != mpStages.end());
		dia::CLoadCase &lcase = *it->second;

		if(!pStage->Initial())
		{
			const CPressureSupportNode::CFaceSupportDef::CSupportDef &supdef = facedef.SupportDef(*pStage);

			switch(supdef.PressureType())
			{
			case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_UNLOADED:
				// do nothing
				break;
			case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL:
			case CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_GLOBAL:
				WriteBoundaryPressureEntry(*pStage, lcase, surface);
				break;
			default:
				assert(FALSE);
				break;
			}
		}

		if(pStage->Last() || pStage == &Controller().EndStage())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

std::vector <double>
  CGeomecDianaRunnerBase::recalculateElementConcentrations(
    bool& bValid, bool& bWrite, bool& bEqual,
    IValueDomainScalar::TValueVec& vcValues,
    const geo::IElement& elm, const CDepletionStage& stage) const
{
  std::vector<double> vcElementValues(vcValues.size());
  for(size_t i = 0; i < vcValues.size(); ++i)
  {
    if(vcValues[i].Valid())
    {
      vcElementValues[i] = -vcValues[i].Value();
    }
    else
    {
      bValid = false;
      break;
    }
  }

  if (bValid)
  {
    bValid = calculateVolumetricStrain(vcElementValues, elm, stage);
  }

  if (bValid)
  {
    for (size_t i = 0; i < vcValues.size(); ++i)
    {
      vcElementValues[i] = vcElementValues[i] / 3;

      if(fabs(vcElementValues[i]) > 1e-15)
        bWrite = true;
      if(i)
      {
        if(fabs(vcElementValues[i] - vcElementValues[i-1]) > 1e-15)
          bEqual = false;
      }
    }
  }

  return vcElementValues;
}

bool CGeomecDianaRunnerBase::OnInitialize(const std::string& title,
  IRetrieveDianaFileNames& retrieveDianaFileNames)
{
  if(Model().parentModel() && !Model().CheckParentResults(Controller().AnalysisType()))
    return false;

  if (!Model().CheckZoominSecant())
    return false;

  if (!Model().CheckCasingContainedInWell())
    return false;

  CAnalysisLogger& logger = Model().Logger();
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("'*** OnInitialize at' dd MMM yyyy',' hh':'mm '***'");
  logger.AddLine(strTime.toStdString(), true);

  Controller().RemoveRedundantLoads();

  // Diana requires that the major axis directions are created first
  AddDirection(geo::CVector::Xaxis);
  AddDirection(geo::CVector::Yaxis);
  AddDirection(geo::CVector::Zaxis);

  return Controller().OnQueryInputFileNames(title, m_sComFile, m_sDatFile, retrieveDianaFileNames);
}

int CGeomecDianaRunnerBase::MeshProgressSize() const
{
  if(WriteFullModel())
    return CDianaRunner::MeshProgressSize();

  return 0;
}

bool CGeomecDianaRunnerBase::OnWriteMesh()
{
	if(!WriteFullModel())
  {
    RemoveIndexedItem("/MATERI", 0);
    RemoveIndexedItem("/INPUT/MATERI", 0);
    RemoveIndexedItem("/ELMSET", 0);

    if (UseLegacyCodeForMaterialProperties())
    {
      // clear /MATERI, /INPUT/MATERI
      // so new values for the material parameters can be written

      TElementSetInfoVector elementset_info;
      elementset_info.reserve(Mesh().ElementSize());

      int i;
      for (i = 0; i < Mesh().ElementSize(); ++i)
      {
        const geo::IElement& elm = Mesh().Element(i);
        const dia::IMaterial& mat = Material(elm);
        const dia::IElementGeometry* geo = ElementGeometry(elm);
        ftn_int_t matidx = (ftn_int_t)AddElementProperty(mat, 0);
        ftn_int_t geoidx = geo ? (ftn_int_t)AddElementProperty(*geo, 1) : 0;
        const CFormationBase *pFormation = MeshBase().Formation(elm);

        ftn_int_t elmidx = (ftn_int_t)(i + 1);

        elementset_info.push_back(CElementSetInfo(matidx, geoidx, pFormation, elmidx));

        PushDir();
        assert(XistIndexed("/ELEMEN/", &elmidx));
        ChangeIndexedDir("/ELEMEN/", &elmidx);
        RemoveIndexedItem("MATERI", 0);
        PutItem("MATERI", &matidx);
        PopDir();
      }

	  OnWriteElementSets(elementset_info);
    }
    else if (Mesh().ElementSize() > 0)
    {
      RemoveIndexedItem("/GEPLST/MATERI", 0);
      RemoveIndexedItem("/GEPLST/GEOMET", 0);

      std::vector<CDistributedElementSetInfo> distributed_info;

      distributed_info.reserve(Mesh().ElementSize());

      PushDir();

      for (int i = 0; i < Mesh().ElementSize(); i++)
      {
        const geo::IElement &elm = Mesh().Element(i);

        distributed_info.push_back(CDistributedElementSetInfo(MeshBase().Formation(elm), elm.Index() + 1, &Material(elm), ElementGeometry(elm)));

        ftn_int_t elmidx = (ftn_int_t)(i + 1);
        assert(XistIndexed("/ELEMEN/", &elmidx));
        ChangeIndexedDir("/ELEMEN/", &elmidx);
        RemoveIndexedItem("MATERI", 0);

        m_progress->Step();
      }

      PopDir();

	  return OnWriteElementInformation(distributed_info);
    }

    return true;
  }

  return CDianaRunner::OnWriteMesh();
}

bool CGeomecDianaRunnerBase::UseLegacyCodeForMaterialProperties() const
{
  CValueMapper *vm = CValueMapper::instance();
  return !vm->DianaDistributedMapping();
}

int CGeomecDianaRunnerBase::GroupsProgressSize() const
{
	if((m_controller.WriteInputFiles() || m_controller.WriteQuadDat()) && WriteFullModel())
		return CDianaRunner::GroupsProgressSize();

	return 0;
}

bool CGeomecDianaRunnerBase::OnWriteElementGroups(TGroupInfoVector& group_info)
{
  m_mpFormationElmsetNames.clear(); // !! if this routine is ever overridden, make sure to handle this properly !! for now we assume we build it from scratch here

	if((m_controller.WriteInputFiles() || m_controller.WriteQuadDat()) && WriteFullModel())
  {
	  // get the dimension of /GROUPS, override may have written groups already...
	  int offset = 0;
	  if(XistIndexed("/GROUPS", 0)) offset = Inquire("/GROUPS", "DIM");

	  m_progress->StatusMessage("Writing element groups");

    PushDir();

    std::sort(group_info.begin(), group_info.end());

    size_t idx_cur = 0;
    size_t idx_max = group_info.size();

    while (idx_cur < idx_max)
    {
      size_t idx_start = idx_cur;

      const CFormationBase *pFormation = group_info[idx_start].pFormation;

      ++idx_cur;

      while (idx_cur < idx_max && pFormation == group_info[idx_cur].pFormation)
        ++idx_cur;

      size_t NrOfSets = idx_cur - idx_start;

			ftn_int_t idx = (ftn_int_t) (++offset);
			ChangeIndexedDir("/GROUPS/", &idx);

      std::stringstream str;
      str << idx << " " << pFormation->Name().toStdString();
			char name[20];
			snprintf(name, 20, str.str().c_str());
			PutCharItem("NAME", name);

      // make sure we only write each element set once
      ftn_int_t eltsetidx = -1; 

      std::stringstream stream;

      for (size_t j = 0; j < NrOfSets; ++j)
      {
        if (group_info[idx_start + j].eltsetidx != eltsetidx)
        {
          eltsetidx = group_info[idx_start + j].eltsetidx;
          stream << "ELMSET" << eltsetidx << " ";
        }
      }

      m_mpFormationElmsetNames.insert(std::make_pair(pFormation, QString(stream.str().c_str())));

      NumRange range(stream.str().c_str());

      if (!range.isEmpty())
      {
        TFtnIntVec vcValues;
        range.filosEncode(vcValues);

        ftn_int_t *data = new ftn_int_t[vcValues.size()];
        for (size_t i = 0; i < vcValues.size(); ++i)
          data[i] = vcValues[i];

        PutIndexedItemLength("INFO", 0, data, (int)vcValues.size());
      
        delete[] data;
      }

			PutCharItem("TYPE", "ELEMEN"); // indicates element sets
	  }

	  PopDir();
  }
  else // fill the map
  {
    std::sort(group_info.begin(), group_info.end());

    size_t idx_cur = 0;
    size_t idx_max = group_info.size();

    while (idx_cur < idx_max)
    {
      size_t idx_start = idx_cur;

      const CFormationBase *pFormation = group_info[idx_start].pFormation;

      ++idx_cur;

      while (idx_cur < idx_max && pFormation == group_info[idx_cur].pFormation)
        ++idx_cur;

      size_t NrOfSets = idx_cur - idx_start;

      // make sure we only write each element set once
      ftn_int_t eltsetidx = -1;

      std::stringstream stream;

      for (size_t j = 0; j < NrOfSets; ++j)
      {
        if (group_info[idx_start + j].eltsetidx != eltsetidx)
        {
          eltsetidx = group_info[idx_start + j].eltsetidx;
          stream << "ELMSET" << eltsetidx << " ";
        }
      }

      m_mpFormationElmsetNames.insert(std::make_pair(pFormation, QString(stream.str().c_str())));
    }
  }

	return true;
}


bool CGeomecDianaRunnerBase::OnWriteElementSets(TElementSetInfoVector& elementset_info)
{
  if (elementset_info.size() == 0)
    return true;

  m_progress->AddSteps(elementset_info.size());
	m_progress->StatusMessage("Writing element sets");

	// get the dimension of /ELMSETS, override may have written groups already...
	int offset = 0;
	if(XistIndexed("/ELMSET", 0)) offset = Inquire("/ELMSET", "DIM");

  std::sort(elementset_info.begin(), elementset_info.end());

  size_t idx_cur = 0;
  size_t idx_max = elementset_info.size();

  TGroupInfoVector group_info;
  group_info.reserve(100); // arbitrary heuristic, we have no idea yet

	PushDir();

  while (idx_cur < idx_max)
  {
    size_t idx_start = idx_cur;

    ftn_int_t matidx = elementset_info[idx_start].matidx;
    ftn_int_t geoidx = elementset_info[idx_start].geoidx;
    const CFormationBase *pFormation = elementset_info[idx_start].pFormation;

    ++idx_cur;

    while (idx_cur < idx_max && matidx == elementset_info[idx_cur].matidx && geoidx == elementset_info[idx_cur].geoidx && pFormation == elementset_info[idx_cur].pFormation)
      ++idx_cur;

    size_t NrOfElements = idx_cur - idx_start;

	  ftn_int_t idx = (ftn_int_t) (++offset);
	  ChangeIndexedDir("/ELMSET/", &idx);

    if (pFormation)
      group_info.push_back(CGroupInfo(pFormation, idx));

	  std::stringstream stream;
	  stream << "ELMSET" << offset;

  	std::string strGroup = stream.str();
		char name[20];
	 	snprintf(name, 20, strGroup.c_str());
	  PutCharItem("NAME", name);

    ftn_int_t *Elements = (ftn_int_t *)DiMalloc(NrOfElements * sizeof(ftn_int_t), "IDianaRunner::OnWriteElementSets");

    for (size_t j = 0; j < NrOfElements; ++j)
    {
      Elements[j] = elementset_info[idx_start + j].eltidx;
    }

		PutItemLength("ELEMEN", Elements, NrOfElements);
		    
    DiFree(Elements, "IDianaRunner::OnWriteElementSets");

    if (matidx > 0)
    {
      PutItem("MATERI", &matidx);
    }

    if (geoidx > 0)
    {
      PutItem("GEOMET", &geoidx);
		}

    m_progress->Step();
	}
	
  PopDir();

	return OnWriteElementGroups(group_info);
}


static const char* element_property_name[] = { "MATERI", "GEOMET", "DATA" };

bool compareMaterialProperties(const dia::IElementProperty *lhs, const dia::IElementProperty *rhs)
{
  if (lhs->Type() != rhs->Type())
    return false;

  if (lhs->isInterfaceElementMaterial())
  {
    assert(dynamic_cast<const CInterfaceElementMaterial *>(lhs));
    assert(dynamic_cast<const CInterfaceElementMaterial *>(rhs));
    return static_cast<const CInterfaceElementMaterial *>(lhs)->sameSlip(*static_cast<const CInterfaceElementMaterial *>(rhs));
  }

  return true;
}

bool CGeomecDianaRunnerBase::OnWriteElementInformation(TDistributedElementSetInfoVector& distributed_info)
{
  const int M = 0; // material property
  const int G = 1; // geometry property

  size_t idx_cur = 0;
  size_t idx_max = distributed_info.size();

  m_progress->AddSteps(distributed_info.size());
  m_progress->StatusMessage("Writing material properties");

  std::sort(distributed_info.begin(), distributed_info.end());

  int elmset_offset = 0;
  if (XistIndexed("/ELMSET", 0)) elmset_offset = Inquire("/ELMSET", "DIM");

  /* Not needed, indexed by material/geomet index
  int lst_offset[2] = { 0, 0 };
  if (XistIndexed("/GEPLST/MATERI", 0)) lst_offset[M] = Inquire("/GEPLST/MATERI", "DIM");
  if (XistIndexed("/GEPLST/GEOMET", 0)) lst_offset[G] = Inquire("/GEPLST/GEOMET", "DIM");
  */

  TGroupInfoVector group_info;
  group_info.reserve(100); // arbitrary heuristic, we have no idea yet

  PushDir();

  while (idx_cur < idx_max)
  {
    size_t idx_start = idx_cur;

    const CFormationBase *pFormation = distributed_info[idx_start].pFormation;
    const dia::IElementProperty *pProperty[2] = { distributed_info[idx_start].pProperty[M], distributed_info[idx_start].pProperty[G] };

    ++idx_cur;

    while (idx_cur < idx_max && pFormation == distributed_info[idx_cur].pFormation && compareMaterialProperties(pProperty[M], distributed_info[idx_cur].pProperty[M]) && ((pProperty[G] == 0 && distributed_info[idx_cur].pProperty[G] == 0) || (pProperty[G] != 0 && distributed_info[idx_cur].pProperty[G] != 0)))
      ++idx_cur;

    size_t NrOfElements = idx_cur - idx_start;

    ftn_int_t *Elements = (ftn_int_t *)DiMalloc(NrOfElements * sizeof(ftn_int_t), "IDianaRunner::OnWriteElementInformation");

    for (size_t j = 0; j < NrOfElements; ++j)
    {
      Elements[j] = distributed_info[idx_start + j].eltidx;
    }
    // freed further below after we write elmset

    // assume first element has constant values
    // currently we write all distributed values, but this can be optimized by careful selection of the "constant"
    // but we are hindered by that our selection of elements needs to hold for all distributed properties

    int propidx[2] = { 0, 0 };
    int nSize[2] = { pProperty[M]->WriteFilosParamSize(*this), pProperty[G] ? pProperty[G]->WriteFilosParamSize(*this) : 0 };
    int nDistributedSize[2] = { 0, 0 };
    std::vector<bool> bDistributed[2];
    bDistributed[M].resize(nSize[M], false);
    bDistributed[G].resize(nSize[G], false);


    double *valueCache[2] = { 0, 0 };

    for (int i = 0; i < 2; ++i)
    {
      if (nSize[i] == 0)
        continue;

      valueCache[i] = new double[NrOfElements * nSize[i]];

      for (size_t j = 0; j < NrOfElements; ++j)
        distributed_info[idx_start + j].pProperty[i]->WriteFilosParamValues(*this, valueCache[i] + nSize[i] * j); // STRIDE 1
        //distributed_info[idx_start + j].pProperty[i]->WriteFilosParamValues(*this, valueCache[i] + j, NrOfElements); // STRIDE #Elts
      // one of the above methods will likely be faster: measure
      // if we change this, also change the STRIDE statements below
      // nothing measured yet, but we start with the one that uses the memcpy
      // update: change to stride 1 due to different order in Diana precluding us from using memcpy; now we go for locality

      for (int j = 0; j < nSize[i]; ++j)
      {
        for (size_t k = 1; k < NrOfElements; ++k)
        {
          if (*(valueCache[i] + j) != *(valueCache[i] + k * nSize[i] + j)) // STRIDE 1
          //if (*(valueCache[i] + j * NrOfElements) != *(valueCache[i] + j * NrOfElements + k)) // STRIDE #Elts
          {
            bDistributed[i][j] = true;
            ++nDistributedSize[i];
            break;
          }
        }
      }

      propidx[i] = AddElementProperty(*pProperty[i], i, true);
    }

    for (size_t j = 0; j < NrOfElements; ++j) // We also need to write the indices to the elements (a bit superfluous)
    {
      ftn_int_t idx = (ftn_int_t)Elements[j];
      ChangeIndexedDir("/ELEMEN/", &idx);
      for (int i = 0; i < 2; ++i)
      {
        if (propidx[i] > 0)
        {
          PutItem(element_property_name[i], &propidx[i]);
        }
      }
    }

    int propStart = nDistributedSize[M] > 0 ? M : G;
    int propSize = nDistributedSize[G] > 0 ? G + 1 : M + 1;
    int paramSize[2] = { nDistributedSize[M], nDistributedSize[G] };

    for (int i = propStart; i < propSize; ++i)
    {
      ChangeDir("/GEPLST/");

      ftn_int_t lst_idx = (ftn_int_t)(propidx[i]);
      ChangeIndexedDir(element_property_name[i], &lst_idx);

      PutItemLength("ELEMEN", Elements, NrOfElements);

      // we need an array of strings, a la argv, but the strings themselves are maximal size 10
      ftn_char_t *parameptr = (ftn_char_t *)DiMalloc(paramSize[i] * sizeof(ftn_char_t), "IDianaRunner::OnWriteElementInformation");
      ftn_char_t  parameval = (ftn_char_t)DiCalloc(paramSize[i], sizeof(char) * 10, "IDianaRunner::OnWriteElementInformation");
      for (int j = 0; j < paramSize[i]; ++j)
        parameptr[j] = parameval + j * 10;

      ftn_double_t *values = (ftn_double_t *)DiMalloc(paramSize[i] * NrOfElements * sizeof(ftn_double_t), "IDianaRunner::OnWriteElementInformation");

      int dis_idx = 0;
      int dis_max = 0;

      for (int j = 0; j < nSize[i]; ++j)
        if (bDistributed[i][j])
          ++dis_max;
          
      for (int j = 0; j < nSize[i]; ++j)
      {
        if (bDistributed[i][j])
        {
          pProperty[i]->WriteFilosParamName(*this, j, parameptr[dis_idx]);

          //for (int k = 0; k < NrOfElements; ++k) // CAUTION: this needs to be checked with final Diana implementation, but order should be param0 for all elts, param1 for all elts, etc
            //values[dis_idx * NrOfElements + k] = *(valueCache[i] + k * nSize[i] + j); // STRIDE 1 (including the for-loop, of course)
          //memcpy(values + dis_idx * NrOfElements, valueCache[i] + j * NrOfElements, NrOfElements * sizeof(double)); // STRIDE #Elts

          // Bug 179704: order is elt0 all params, elt1 all params, etc
          for (int k = 0; k < NrOfElements; ++k)
            *(values + k * dis_max + dis_idx) = *(valueCache[i] + k * nSize[i] + j);

          ++dis_idx;
        }
      }

      ftn_int_t parame_idx = 0;
      PutIndexedCharItemLength("PARAME", &parame_idx, parameptr, paramSize[i]);
      PutItemLength("VALUES", values, paramSize[i] * NrOfElements);

      DiFree(parameptr, "IDianaRunner::OnWriteElementInformation");
      DiFree(parameval, "IDianaRunner::OnWriteElementInformation");
      DiFree(values, "IDianaRunner::OnWriteElementInformation");
    }

    for (int i = 0; i < 2; ++i)
      if (valueCache[i])
        delete[] valueCache[i];

    // write elmset
    ftn_int_t elmset_idx = (ftn_int_t)(++elmset_offset);
    ChangeIndexedDir("/ELMSET/", &elmset_idx);

    if (pFormation)
      group_info.push_back(CGroupInfo(pFormation, elmset_idx));

    std::stringstream stream;
    stream << "ELMSET" << elmset_offset;

    std::string strGroup = stream.str();
    char name[20];
    snprintf(name, 20, strGroup.c_str());
    PutCharItem("NAME", name);

    PutItemLength("ELEMEN", Elements, NrOfElements);

    DiFree(Elements, "IDianaRunner::OnWriteElementInformation");

    for (int i = 0; i < 2; ++i)
    {
      if (propidx[i] > 0)
      {
        PutItem(element_property_name[i], &propidx[i]);
      }
    }

    m_progress->Step();
  }

  PopDir();

  return OnWriteElementGroups(group_info);
}

bool CGeomecDianaRunnerBase::WriteElements()
{
	assert(!XistIndexed("/ELEMEN", 0));

	m_progress->StatusMessage("Writing elements");

	for(int j = 0; j < 3; j++) {
		m_mpElementProperty[j].clear();
	}

  if (UseLegacyCodeForMaterialProperties())
  {
    TElementSetInfoVector elementset_info;
    elementset_info.reserve(Mesh().ElementSize());

    PushDir();
    // write elements
    for (int i = 0; i < Mesh().ElementSize(); i++)
    {
      ftn_int_t idx = (ftn_int_t)(i + 1);
      ChangeIndexedDir("/ELEMEN/", &idx);

      const geo::IElement &elm = Mesh().Element(i);

      // nodes
      int nnod = elm.NrOfNodes();
      ftn_int_t *nodenrs = (ftn_int_t *)DiMalloc(nnod * sizeof(ftn_int_t), "IDianaRunner::OnWriteMesh");

      int j;
      for (j = 0; j < nnod; j++) nodenrs[j] = (ftn_int_t)(elm.PointIndex(j) + 1);
      PutItemLength("NODES", nodenrs, nnod);
      DiFree(nodenrs, "IDianaRunner::WriteElements");

      // get material, geometry, data
      const dia::IElementProperty* property[3];
      property[0] = &Material(elm);
      property[1] = ElementGeometry(elm);
      property[2] = ElementData(elm);

      ftn_int_t property_idx[3];

      // write element properties
      for (j = 0; j < 3; j++) {
        property_idx[j] = 0;
        if (property[j]) {
          property_idx[j] = (ftn_int_t)AddElementProperty(*property[j], j);
          PutItem(element_property_name[j], &property_idx[j]);
        }
      }

      elementset_info.push_back(CElementSetInfo(property_idx[0], property_idx[1], MeshBase().Formation(elm), elm.Index() + 1));

      // element type
      assert(!elm.Type().empty());
      char *type = DiStrsave(elm.Type().c_str());
      PutCharItem("TYPE", type);
      DiFree(type, "IDianaRunner::WriteElements");

      m_progress->Step();
    }
    PopDir();

	OnWriteElementSets(elementset_info);

  }
  else if (Mesh().ElementSize() > 0) // just to be correct; we shouldn't be here without elements, but if it were to happen, we'd crash
  {
    std::vector<CDistributedElementSetInfo> distributed_info;

    distributed_info.reserve(Mesh().ElementSize());

    PushDir();

    for (int i = 0; i < Mesh().ElementSize(); i++)
    {
      const geo::IElement &elm = Mesh().Element(i);

      distributed_info.push_back(CDistributedElementSetInfo(MeshBase().Formation(elm), elm.Index() + 1, &Material(elm), ElementGeometry(elm)));

      ftn_int_t idx = (ftn_int_t)(i + 1);
      ChangeIndexedDir("/ELEMEN/", &idx);

      // nodes
      int nnod = elm.NrOfNodes();
      ftn_int_t *nodenrs = (ftn_int_t *)DiMalloc(nnod * sizeof(ftn_int_t), "IDianaRunner::WriteElements");

      int j;
      for (j = 0; j < nnod; j++) nodenrs[j] = (ftn_int_t)(elm.PointIndex(j) + 1);
      PutItemLength("NODES", nodenrs, nnod);
      DiFree(nodenrs, "IDianaRunner::WriteElements");

      // element type
      assert(!elm.Type().empty());
      char *type = DiStrsave(elm.Type().c_str());
      PutCharItem("TYPE", type);
      DiFree(type, "IDianaRunner::WriteElements");

      m_progress->Step();
    }

    PopDir();

	return OnWriteElementInformation(distributed_info);
  }

	return true;
}

bool CGeomecDianaRunnerBase::InitialLoadCaseActive() const
{
	return &m_controller.StartStage() == &Model().InitialDepletionStage();
}

void CGeomecDianaRunnerBase::WriteSolveCommands()
{
  // assuming we're in /cmnds/seginf()/geomec
	PushDir();
	ChangeDir("SOLVE");

  switch (Solver())
  {
  case ITERATIVE:
    /* Fall-through */
  default:
    ChangeDir("ITERAT");
    break;
  case PARALLEL_DIRECT_SPARSE:
    ChangeDir("PARDIS");
    break;
  }

  ftn_int_t iThreads = 0; // 0 is used when not using the parallel solver
  if(Model().UseParallelSolver())
    iThreads = (ftn_int_t)Model().NumSolverThreads();

  assert(iThreads >= 0 && iThreads <= Model().MaxNumSolverThreads());

  PutItem("MAXTHR", &iThreads);

	PopDir();

  if (Model().UseStressSmootheningOnDSF() && Model().HasDoubleSidedFaults())
    SetActive("AVINTF", FTN_TRUE);
}

bool CGeomecDianaRunnerBase::AllowLinearCalc() const
{
  return Model().AllowLinearCalc();
}

bool CGeomecDianaRunnerBase::BehavesLinearly(const CDepletionStage& stage) const
{
  if (m_bBehaveLinearlyDirty)
  {
    m_bBehaveLinearlyDirty = false;
    m_bBehaveLinearly = false;

    if (!AllowLinearCalc())
      return false;

  	THorizonBaseEntry::TEntryNodeSet stHorizons = Model().GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();
	  for(THorizonBaseEntry::TEntryNodeSet::iterator it = stHorizons.begin(); it != stHorizons.end(); ++it)
	  {
		  CHorizonBase& hor = static_cast<CHorizonBase&>(**it);
      if (hor.Slip())
      {
        if (hor.SlipType() != CHorizonBase::STICK && hor.SlipType() != CHorizonBase::SLIP)
          return false;
      }
  	}

    TFormationBaseEntry::TEntryNodeSet stFormations = Model().GraphEntry(MD_BASE_FORMATION)->GraphEntryNodes();
    for (TFormationBaseEntry::TEntryNodeSet::iterator it = stFormations.begin(); it != stFormations.end(); ++it)
    {
      CFormationBase& frm = static_cast<CFormationBase&>(**it);
      if (!frm.Material(stage).LibraryMaterial()->LibraryMaterial().BehavesLinearly())
        return false;
    }

    m_bBehaveLinearly = true;
  }
  
  return m_bBehaveLinearly;
}

void CGeomecDianaRunnerBase::WriteDepletionStageStepSizesAndTime(const CDepletionStage& stage)
{
//  assert(!stage.Initial());
  if(!stage.Initial())
  {
	  double secs = stage.Previous().Time().SecondsTo(stage.Time());
	  ftn_double_t dtime = (ftn_double_t) (secs / SECONDS_PER_MONTH);
	  PutItem("DTIME", &dtime);
  }

  if (BehavesLinearly(stage))
  {
    if (stage.Initial())
    {
      CAnalysisLogger& logger = Model().Logger();
      QString strLog = QString("*** Found linear behavior, overriding default step parameters with stepsize 1");
      logger.AddLine(strLog.toStdString(), true);
    }

    ftn_double_t* dStepSize = (ftn_double_t*)DiMalloc(1 * sizeof(double), "CGeomecDianaRunnerBase::WriteDepletionStageStepSizesAndTime");
    dStepSize[0] = 1;
    PutItemLength("STPSIZ", dStepSize, 1);
    DiFree(dStepSize, "CGeomecDianaRunnerBase::WriteDepletionStageStepSizesAndTime");
  }
  else if(stage.AutomaticStepSizes())
  {
    SetActive("AUTOST", FTN_TRUE);
    ftn_double_t dMaxSiz = (ftn_double_t) stage.MaxAutomaticStepSize();
    PutItem("MAXSIZ", &dMaxSiz);
  }
  else
  {
    // variable names according to description in mantis #2723
    int n = stage.NLSteps();
    const double& x = stage.NLStepScalingFactor();
    double dDividor = 0;
    int i;
    for(i = 1; i < n; ++i)
      dDividor += i;

    double z = n * (1. / x - 1.) / dDividor;

    ftn_double_t* dStepSize = (ftn_double_t*)DiMalloc(n * sizeof(double), "CGeomecDianaRunnerBase::WriteDepletionStageStepSizesAndTime");
    dStepSize[0] = x / n;

    for(i = 1; i < n; ++i)
      dStepSize[i] = x * (1. + i * z) / n;

#ifdef _DEBUG
    double dDbg = 0;
    for(i = 0; i < n; ++i)
      dDbg += dStepSize[i];
    assert(fabs(dDbg - 1.) < EPS);
#endif
    PutItemLength("STPSIZ", dStepSize, n);
    DiFree(dStepSize, "CGeomecDianaRunnerBase::WriteDepletionStageStepSizesAndTime");
  }
}

double CGeomecDianaRunnerBase::calculateBulkModulus(
  const CFFMaterial* cffMaterial) const
{
  double youngModulusN =
    cffMaterial->ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM);
  double youngModulusT =
    cffMaterial->ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS);
  double bulkModulus = 0;

  if ((youngModulusN != 0) && (youngModulusT != 0))
  {
    double poissonRatioN =
      cffMaterial->ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
    double poissonRatioT =
      cffMaterial->ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);

    bulkModulus =
      (2 - (2 * poissonRatioT) - (2 * poissonRatioN)) / youngModulusT;
    bulkModulus += (1 - (2 * poissonRatioN)) / youngModulusN;

    if (bulkModulus != 0)
    {
      bulkModulus = 1 / bulkModulus;
    }
  }

  return bulkModulus;
}

double CGeomecDianaRunnerBase::calculateBulkModulus(
  const geo::IElement& elm) const
{
  const dia::IMaterial& material = Material(elm);
  const CFFMaterial* cffMaterial =
    dynamic_cast <const CFFMaterial*> (&material);
  double bulkModulus = 0;

  if (cffMaterial)
  {
    if (cffMaterial->IsParameter(IDT_VALUETYPE_BULKSTIFFNESS))
    {
      bulkModulus =
        cffMaterial->ParameterValue(IDT_VALUETYPE_BULKSTIFFNESS);
    }
    else if (isAnisotropic(cffMaterial))
    {
      bulkModulus = calculateBulkModulus(cffMaterial);
    }
  }

  return bulkModulus;
}

bool CGeomecDianaRunnerBase::calculateVolumetricStrain(
  std::vector <double>& vcElementValues, const geo::IElement& elm,
  const CDepletionStage& stage) const
{
  const CFormationBase &formationBase = *MeshBase().Formation(elm);
  const CPressure& initialPressure =
    formationBase.Pressure(stage.InitialStage());
  const CFractureMatrixPressure& initialFractureMatrixPressure =
    formationBase.FractureMatrixPressure(stage.InitialStage());
  const CPressure& pressure = formationBase.Pressure(stage);
  const CFractureMatrixPressure& fractureMatrixPressure = formationBase.FractureMatrixPressure(stage);
  bool valid = true;

  if (!stage.IsMarkedAsInitial() &&
    initialPressure.Component().Defined() &&
    initialFractureMatrixPressure.Component().Defined() &&
    pressure.Component().Defined() && fractureMatrixPressure.Component().Defined())
  {
    bool bCalculate = isAnisotropic(elm);

    double bulkModulus = 0;
    if (bCalculate)
    {
      bulkModulus = calculateBulkModulus(elm);
      bCalculate  = (bulkModulus != 0);
    }

    if (bCalculate)
    {
      IValueDomainScalar::TValueVec initialPressureValues =
        initialPressure.Component().ScalarData().ValueElement(elm);
      IValueDomainScalar::TValueVec initialFractureMatrixPressureValues =
        initialFractureMatrixPressure.Component().ScalarData().ValueElement(elm);
      IValueDomainScalar::TValueVec pressureValues =
        pressure.Component().ScalarData().ValueElement(elm);
      IValueDomainScalar::TValueVec fractureMatrixPressureValues =
        fractureMatrixPressure.Component().ScalarData().ValueElement(elm);

      assert(initialPressureValues.size() == elm.NrOfNodes());
      assert(initialFractureMatrixPressureValues.size() == elm.NrOfNodes());
      assert(pressureValues.size() == elm.NrOfNodes());
      assert(fractureMatrixPressureValues.size() == elm.NrOfNodes());

      for (size_t s = 0; s < vcElementValues.size(); ++s)
      {
        if (initialFractureMatrixPressureValues[s].Valid() &&
          initialPressureValues[s].Valid() &&
          fractureMatrixPressureValues[s].Valid() && pressureValues[s].Valid())
        {
          vcElementValues[s] +=
            ((fractureMatrixPressureValues[s].Value() - pressureValues[s].Value()) -
            (initialFractureMatrixPressureValues[s].Value() -
              initialPressureValues[s].Value())) / bulkModulus;
        }
        else
        {
          valid = false;
          break;
        }
      }
    }
  }

  return valid;
}

void CGeomecDianaRunnerBase::_CreateVolumetricStrainLoad(const geo::IElement& elm, const CDepletionStage& stage, dia::CLoadCase& lcase)
{
  const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
  if(!pInterface)
  {
    const CStrainLoad &strain = StrainLoad(elm, stage);

    if(strain.IsVolumetric() && strain.Component().Defined())
    {
      IValueDomainScalar::TValueVec vcValues = strain.Component().ScalarData().ValueElement(elm);
      if(vcValues.size() == elm.NrOfNodes())
      {
        std::vector<double> vcElementValues(vcValues.size());
        bool bValid = true;
        bool bWrite = false;
        bool bEqual = true;

        vcElementValues = recalculateElementConcentrations(
          bValid, bWrite, bEqual, vcValues, elm, stage);

        if(bValid && bWrite)
        {
          if(bEqual)
            new dia::CConcentrationLoad(lcase, vcElementValues[0], elm);
          else
            new dia::CConcentrationLoad(lcase, vcElementValues, elm);
        }
      }
    }
  }
}


void CGeomecDianaRunnerBase::_CreatePlanarStrainLoad(const geo::IElement& elm, const CDepletionStage& stage, dia::CLoadCase& lcase)
{
  const geo::IInterfaceElement* pInterface = dynamic_cast<const geo::IInterfaceElement*>(&elm);
  if(!pInterface)
  {

    const CStrainLoad &strain = StrainLoad(elm, stage);

    std::vector<CTensor> vcTensors = strain.Component().TensorData()->ValueElement(elm);

    bool bIsStrainTensor = strain.EncounteredFullTensor();

    if (vcTensors.size() == elm.NrOfNodes())
    {
      bool bValid = !vcTensors[0].Empty();
      bool bSame  = true;

      if (!bValid)
        return;

      // use 1e-15 as epsilon, just as in Volumetric Strains
      bool bWrite = fabs(vcTensors[0].XX()) > 1e-15
        || fabs(vcTensors[0].YY()) > 1e-15
        || fabs(vcTensors[0].ZZ()) > 1e-15
        || fabs(vcTensors[0].XY()) > 1e-15
        || fabs(vcTensors[0].YZ()) > 1e-15
        || fabs(vcTensors[0].XZ()) > 1e-15;

      for (size_t i = 1; i < vcTensors.size(); ++i)
      {
        if (vcTensors[i].Empty())
        {
          bValid = false;
          return;
        }
        bWrite = bWrite
          || fabs(vcTensors[i].XX()) > 1e-15
          || fabs(vcTensors[i].YY()) > 1e-15
          || fabs(vcTensors[i].ZZ()) > 1e-15
          || fabs(vcTensors[i].XY()) > 1e-15
          || fabs(vcTensors[i].YZ()) > 1e-15
          || fabs(vcTensors[i].XZ()) > 1e-15;
        if (fabs(vcTensors[0].XX() - vcTensors[i].XX()) > 1e-15)
        {
          bSame = false;
          break;
        }
        if (fabs(vcTensors[0].YY() - vcTensors[i].YY()) > 1e-15)
        {
          bSame = false;
          break;
        }
        if (fabs(vcTensors[0].ZZ() - vcTensors[i].ZZ()) > 1e-15)
        {
          bSame = false;
          break;
        }
        if (fabs(vcTensors[0].XY() - vcTensors[i].XY()) > 1e-15)
        {
          bSame = false;
          break;
        }
        if (fabs(vcTensors[0].YZ() - vcTensors[i].YZ()) > 1e-15)
        {
          bSame = false;
          break;
        }
        if (fabs(vcTensors[0].XZ() - vcTensors[i].XZ()) > 1e-15)
        {
          bSame = false;
          break;
        }
      }

      if (!bWrite)
        return;

      if (bSame)
        vcTensors.resize(1);

      // rotation of the normal/lateral tensors is handled by Diana; see DCStrainLoad.cpp

      if (bValid && bWrite)
      {
        if (bSame)
          new dia::CStrainLoad(lcase, vcTensors[0], elm, bIsStrainTensor);
        else
          new dia::CStrainLoad(lcase, vcTensors, elm, bIsStrainTensor);
      }
    }

  }
}

void CGeomecDianaRunnerBase::CreateStrainLoad(const geo::IElement& elm, const CDepletionStage& stage, dia::CLoadCase& lcase)
{
  // We support both the old VolumetricStrain and the new one for now, for comparisons and tests
  // In which case the Volumetric Strain should either be linked to Volumetric Strains or Strains in the control tree

  if (dynamic_cast<const geo::IInterfaceElement*>(&elm)) // nothing to do for interface elements
    return;

//  if (!StrainLoad(elm, stage).IsVolumetric())
//    _CreateVolumetricStrainLoadDEPRECATED(elm, stage, lcase);
//  else
	  _CreateVolumetricStrainLoad(elm, stage, lcase);
	_CreatePlanarStrainLoad(elm, stage, lcase);
}


const CStrainLoad& CGeomecDianaRunnerBase::StrainLoad(const geo::IElement& element, const CDepletionStage& stage) const
{
	const CFormationBase &formation = *MeshBase().Formation(element);
  return const_cast<CFormationBase&>(formation).Strain(stage);
}

const dia::IElementGeometry* CGeomecDianaRunnerBase::ElementGeometry(const geo::IElement &element) const
{
  const dia::IMaterial& mat = Material(element);
  const CFFMaterial* pffmat = dynamic_cast<const CFFMaterial*>(&mat);
  if(pffmat)
  {
    const IMaterial* pMat = dynamic_cast<const IMaterial*>(&pffmat->Material());
    if(pMat)
    {
      dia::IElementGeometry* pGeom = 0;

      if(pMat->MaterialModel() == MM_UPSCALED_ANISOTROPY || pMat->MaterialModel() == MM_ANISOTROPIC_CAMCLAY || pMat->MaterialModel() == MM_FRACTURE_APERTURE || pMat->MaterialModel() == MM_FRACTURE_APERTURE2)
      {
        assert(pffmat->IsParameter(IDT_VALUETYPE_LAYER_NORMAL_INCL));
        assert(pffmat->IsParameter(IDT_VALUETYPE_LAYER_NORMAL_AZI));
 
        pGeom = new CUpscalingAnisotropyElementGeometry(pffmat->ParameterValue(IDT_VALUETYPE_LAYER_NORMAL_INCL),
                                                        pffmat->ParameterValue(IDT_VALUETYPE_LAYER_NORMAL_AZI));
      }

      if(pGeom)
      {
        m_vcAllocatedElementProperties.push_back(pGeom);
        return pGeom;
      }
    }
  }

  return CDianaRunner::ElementGeometry(element);
}

void CGeomecDianaRunnerBase::WriteNonlinDepletionStageParameters(const CDepletionStage *pStage, bool bHasSave)
{
	ftn_int_t idum;
	char *sdum;
	ftn_double_t ddum;

//	if(!pStage->Initial())
    WriteDepletionStageStepSizesAndTime(*pStage);

	idum = (ftn_int_t) pStage->NLIterationSteps();
	PutItem("MITER", &idum);

	sdum = DiStrsave(pStage->IterationSchemeTag(pStage->NLIterationScheme()).c_str());
	PutCharItem("SCHEME", sdum);
	DiFree(sdum, "CGeomecDianaRunnerBase::WriteNonlinDepletionStageParameters");

	ddum = (ftn_double_t) pStage->NLConvNormVal();
	PutItem("NORM", &ddum);

	sdum = DiStrsave(pStage->ConvergenceTypeTag(pStage->NLConvergenceType()).c_str());
	PutCharItem("NORMTP", sdum);
	DiFree(sdum, "CGeomecDianaRunnerBase::WriteNonlinDepletionStageParameters");

  if(bHasSave)
  {
	  PushDir();

		  ChangeDir("SAVE");

	  PopDir();

	  if(pStage->OutputType() == CDepletionStage::NONE)
		  SetActive("SAVE", FTN_FALSE);
  }
}

bool CGeomecDianaRunnerBase::CreateTopLoad3D(
	const CHorizonBase &top, 
	dia::CLoadCase &lcase )
{
	const C3DHorizon &top3d = dynamic_cast<const C3DHorizon &> (top);
	
	for(int i = 0; i < top3d.BodyFaceSize(); i++)
	{
		const geo::IFace &face = top3d.BodyFace(i);
		const geo::CBodyTriangle *pTriangle = dynamic_cast<const geo::CBodyTriangle*> (&face);
		const geo::CBodyQuadrilateral *pQuad = dynamic_cast<const geo::CBodyQuadrilateral*> (&face);

		assert(pTriangle || pQuad);

		if(pTriangle)
		{
			const geo::IBody &body = *pTriangle->Parent();
			std::vector<double> vcValues(pTriangle->NrOfPoints());
			bool bWriteThem = false;
			bool bWriteMultiple = false;
			for(int j = 0; j < pTriangle->NrOfPoints(); j++)
			{
				vcValues[j] = -1*Model().GlobalInitialStress().TotalVerticalStressAtNode(body, pTriangle->BodyNode(j)) * 1e6;
				if(fabs(vcValues[j]) > MIN_PRESSU_LOAD_VAL)
					bWriteThem = true;
				if(j && fabs(vcValues[j] - vcValues[j-1]) > MIN_PRESSU_LOAD_VAL)
					bWriteMultiple = true;
			}

			if(bWriteThem)
			{
				if(bWriteMultiple)
					new dia::CBodyFaceLoad(lcase, vcValues, body, pTriangle->Index());
				else
					new dia::CBodyFaceLoad(lcase, vcValues[0], body, pTriangle->Index());
			}
		}
		else
		{
			const geo::IBody &body = *pQuad->Parent();
			std::vector<double> vcValues(pQuad->NrOfPoints());
			bool bWriteThem = false;
			bool bWriteMultiple = false;
			for(int j = 0; j < pQuad->NrOfPoints(); j++)
			{
				vcValues[j] = -1*Model().GlobalInitialStress().TotalVerticalStressAtNode(body, pQuad->BodyNode(j)) * 1e6;
				if(fabs(vcValues[j]) > MIN_PRESSU_LOAD_VAL)
					bWriteThem = true;
				if(j && fabs(vcValues[j] - vcValues[j-1]) > MIN_PRESSU_LOAD_VAL)
					bWriteMultiple = true;
			}

			if(bWriteThem)
			{
				if(bWriteMultiple)
					new dia::CBodyFaceLoad(lcase, vcValues, body, pQuad->Index());
				else
					new dia::CBodyFaceLoad(lcase, vcValues[0], body, pQuad->Index());
			}
		}

		m_progress->Step();
	
	}

	return true;
}

bool CGeomecDianaRunnerBase::IsZoomInModel() const
{
  return (Model().parentModel() != 0);
}

bool CGeomecDianaRunnerBase::WriteZoomInInitialStresses(const geo::IElement &element, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures)
{
	const CInterfaceBoundary* pBound = dynamic_cast<const CInterfaceBoundary*>(&Model().Boundary());

  if(pBound)
  {
	  const CPressureSupportNode& psn = pBound->PressureSupportNode();
	  const CValueType* pIniStresses = psn.DistriValues(Model().InitialDepletionStage());
	  if(pIniStresses)
	  {
		  assert(pIniStresses->ComponentSize() == 6); // stress tensor
      size_t iNode;
		  int iComp;
		  std::vector<CTensor> vcTensors;
		  bool bWriteThem = false;
		  bool bWriteMultiple = false;
		  assert(vcInitialPressures.size() == element.NrOfNodes());

      std::vector<geo::CValue> components(6 * element.NrOfNodes());

      bool bSomeValid = false;
      bool bAllValid  = true;

		  for(iComp = 0; iComp < 6 && bAllValid; ++iComp)
		  {
        IValueDomainScalar::TValueVec vcValues = pIniStresses->Component(iComp).ScalarData().ValueElement(element);
        for(iNode = 0; iNode < element.NrOfNodes(); ++iNode)
        {
    		  //double components[6];
          geo::CValue val = vcValues[iNode];

          if(val.Valid())
          {
            bSomeValid = true;

			      if(iComp < 3) // normal
				      components[6*iNode + iComp] = -(val.Value() - vcInitialPressures[iNode].Value()) * 1e6; // total -> effective && MPa -> Pa
			      else          // shear
				      components[6*iNode + iComp] = -val.Value() * 1e6; // MPa -> Pa
          }
          else
          {
            bAllValid = false;
            break;
          }
        }
      }

      if (bSomeValid && !bAllValid)
      {
        _m()->error("Not all initial stress tensors on the boundary are valid. Please check your input.", MB_ICONEXCLAMATION);
        return false;
      }

      for(iNode = 0; iNode < element.NrOfNodes(); ++iNode)
      {
        for(iComp = 0; iComp < 6; ++iComp)
        {
          if(!components[6*iNode + iComp].Valid())
          {
            bWriteThem = false;
            break;
          }

          if(fabs(components[6*iNode + iComp].Value()) > MIN_PRESTR_LOAD_VAL)
            bWriteThem = true;
        }

			  if(bWriteThem && iNode)
			  {
				  if((components[6*iNode    ].Valid() && fabs(components[6*iNode    ].Value() - vcTensors[iNode - 1].XX()) > MIN_PRESTR_LOAD_VAL) ||
				     (components[6*iNode + 1].Valid() && fabs(components[6*iNode + 1].Value() - vcTensors[iNode - 1].YY()) > MIN_PRESTR_LOAD_VAL) ||
				     (components[6*iNode + 2].Valid() && fabs(components[6*iNode + 2].Value() - vcTensors[iNode - 1].ZZ()) > MIN_PRESTR_LOAD_VAL) ||
				     (components[6*iNode + 3].Valid() && fabs(components[6*iNode + 3].Value() - vcTensors[iNode - 1].XY()) > MIN_PRESTR_LOAD_VAL) ||
				     (components[6*iNode + 4].Valid() && fabs(components[6*iNode + 4].Value() - vcTensors[iNode - 1].YZ()) > MIN_PRESTR_LOAD_VAL) ||
				     (components[6*iNode + 5].Valid() && fabs(components[6*iNode + 5].Value() - vcTensors[iNode - 1].XZ()) > MIN_PRESTR_LOAD_VAL))
				  {
					  bWriteMultiple = true;
				  }
			  }

        if(bWriteThem)
        {
  			  vcTensors.push_back(CStressTensor(components[6*iNode    ].Value(),
                                            components[6*iNode + 1].Value(),
                                            components[6*iNode + 2].Value(),
                                            components[6*iNode + 3].Value(),
                                            components[6*iNode + 4].Value(),
                                            components[6*iNode + 5].Value()));
        }
        else
        {
          break;
        }
      }

      if(bWriteThem && vcTensors.size() == element.NrOfNodes())
      {
			  if(bWriteMultiple)
				  new dia::CInitialStressLoad(lcase, vcTensors, element);
			  else
				  new dia::CInitialStressLoad(lcase, vcTensors[0], element);
      }
	  }
    else if(Model().parentModel() != 0)
    {
      // get initial stresses from parent model
		  std::vector<CTensor> vcTensors(element.NrOfNodes());
      const CDepletionStage& parentStage = Model().ParentEquivalentDepletionStage(Model().InitialDepletionStage());

      CStressTensorValueSet stress = Model().parentModel()->ResultRegister().EffectiveStress(parentStage,
        Model().ParentLinearResults(Controller().AnalysisType()) ?
          CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN, false);

      for(int i = 0; i < element.NrOfNodes(); ++i)
      {
        const geo::INode& node = element.Node(i);
        vcTensors[i] = stress.ValuePoint(node, geo::IParallelInitializationCallback::Sequential);
        if(vcTensors[i].Empty())
        {
          _m()->msg(QObject::tr(
            "Node %1 (%2,%3,%4) seems to reside outside the parent model, analysis cannot be performed")
              .arg(node.Index() + 1).arg(node.X()).arg(node.Y()).arg(node.Z()));
          return false;
        }
      }

      bool bWriteThem = false;
      bool bWriteMultiple = false;
      for(size_t i = 0; i < vcTensors.size(); ++i)
      {
        vcTensors[i] *= -1e6;
        if(fabs(vcTensors[i].XX()) > MIN_PRESTR_LOAD_VAL ||
           fabs(vcTensors[i].YY()) > MIN_PRESTR_LOAD_VAL ||
           fabs(vcTensors[i].ZZ()) > MIN_PRESTR_LOAD_VAL ||
           fabs(vcTensors[i].XY()) > MIN_PRESTR_LOAD_VAL ||
           fabs(vcTensors[i].YZ()) > MIN_PRESTR_LOAD_VAL ||
           fabs(vcTensors[i].XZ()) > MIN_PRESTR_LOAD_VAL)
        {
          bWriteThem = true;
        }

        if(i && (
          fabs(vcTensors[i].XX() - vcTensors[i-1].XX()) > MIN_PRESTR_LOAD_VAL ||
          fabs(vcTensors[i].YY() - vcTensors[i-1].YY()) > MIN_PRESTR_LOAD_VAL ||
          fabs(vcTensors[i].ZZ() - vcTensors[i-1].ZZ()) > MIN_PRESTR_LOAD_VAL ||
          fabs(vcTensors[i].XY() - vcTensors[i-1].XY()) > MIN_PRESTR_LOAD_VAL ||
          fabs(vcTensors[i].YZ() - vcTensors[i-1].YZ()) > MIN_PRESTR_LOAD_VAL ||
          fabs(vcTensors[i].XZ() - vcTensors[i-1].XZ()) > MIN_PRESTR_LOAD_VAL))
        {
          bWriteMultiple = true;
        }
      }

      if(bWriteThem)
      {
        if(bWriteMultiple)
          new dia::CInitialStressLoad(lcase, vcTensors[0], element);
        else
          new dia::CInitialStressLoad(lcase, vcTensors, element);
      }
    }
	  else
	  {
		  // no available initial stress, so perform the calculation
		  PerformInitialStressCalculation(true);
	  }
  }

  return true;
}

void CGeomecDianaRunnerBase::PerformInitialStressCalculation(bool b)
{
	m_bPerformInitialStressCalculation = b;
}

bool CGeomecDianaRunnerBase::PerformInitialStressCalculation() const
{
	return m_bPerformInitialStressCalculation;
}

bool CGeomecDianaRunnerBase::InitialStressesDefined() const
{
  if(Model().parentModel() != 0)
    return true;

  const CInterfaceBoundary* pBound = dynamic_cast<const CInterfaceBoundary*>(&Model().Boundary());
  if(pBound)
  {
    const CDepletionStage& inistage = Model().InitialDepletionStage();
    const CSupportDepletion* pSupDep = pBound->PressureSupportNode().SupportDepletion(inistage);
    if(pSupDep)
      return (pSupDep->ValueType() != 0);
  }

  return false;
}

bool CGeomecDianaRunnerBase::ExportCommandFileWithDefaults() const
{
  return Model().ExportCommandFilesWithDefaults();
}

geo::CVector CGeomecDianaRunnerBase::GetDisplacementVector(const CVectorValueSet& displa, const geo::IElement& elm, const geo::IPoint& ptSample) const
{
  const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&elm);
  if(pIface)
  {
    if(pIface->FrontFace())
      return GetDisplacementVector(displa, *pIface->FrontFace(), ptSample);

    assert(pIface->BackFace());
    return GetDisplacementVector(displa, *pIface->BackFace(), ptSample);
  }

  CVectorValueSet::TValueVec values(elm.NrOfNodes());
  displa.ElementValues(values, elm.Index(), geo::IParallelInitializationCallback::Sequential);

  bool bValid = true;
  for(size_t j = 0; j < values.size(); j++)
  {
    if(!displa.IsValid(values[j]))
    {
      bValid = false;
      break;
    }
  }

  geo::CVector vecDispla;
  if(bValid)
  {
    geo::IElement::TDoubleVec vcShape = elm.ShapeFunction(elm.WorldToIso(ptSample));
    assert(vcShape.size() == values.size());

    vecDispla = values[0] * vcShape[0];
    for(size_t j = 1; j < values.size(); j++)
      vecDispla += values[j] * vcShape[j];
  }

  return vecDispla;
}

void CGeomecDianaRunnerBase::WriteConvergenceCommands()
{
  SetActive("CHECK", Model().ConvergenceChecks() || !Model().AllowLinearCalc() ? FTN_TRUE : FTN_FALSE);

  if(!AllowLinearCalc())
  {
    PushDir();
      ChangeDir("CHECK");
      ChangeDir("INACC");
      SetActive("LINELM", FTN_FALSE);
    PopDir();
  }

  ftn_double_t val;

  if(Model().ConvergenceChecks())
  {
    PushDir();
      
      ChangeDir("CHECK");

      PushDir();
        ChangeDir("ANGLE");
        val = ftn_double_t(Model().ConvergenceMinimumAngleValue());
        PutItem("MINANG", &val);
        SetActive("LINELM", Model().ConvergenceMinimumAngleCheck() ? FTN_TRUE : FTN_FALSE);
      PopDir();

      PushDir();
        ChangeDir("EDGES");
        val = ftn_double_t(Model().ConvergenceMinimumRatioValue());
        PutItem("MINRAT", &val);
        SetActive("LINELM", Model().ConvergenceMinimumRatioCheck() ? FTN_TRUE : FTN_FALSE);
      PopDir();

      PushDir();
        ChangeDir("VOLUME");
        if (CheckConvergenceMinimumVolume())
          val = ftn_double_t(Model().ConvergenceMinimumVolumeValue());
        else
          val = 1E-5; // in zoom-in we do have small elements, and Diana will list all smaller than the volume given (typically 1m^3), even though it will ignore the check -- we give it a smaller volume to get rid of (most of) those messages
        PutItem("MINVOL", &val);
        SetActive("LINELM", CheckConvergenceMinimumVolume() && Model().ConvergenceMinimumVolumeCheck() ? FTN_TRUE : FTN_FALSE);
      PopDir();

    PopDir();
  }
}

bool CGeomecDianaRunnerBase::CheckConvergenceMinimumVolume() const
{
  return true; // default to true, wellpath zoom-in models will return false (they have smaller elements)
}

std::string CGeomecDianaRunnerBase::GroupName(const geo::IElementSet& group, int nIndex)
{
  // formations
  TFormationBaseEntry& entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stFormations = entry.EntryNodes();
  for(TFormationBaseEntry::TNodeSet::iterator it = stFormations.begin(); it != stFormations.end(); ++it)
  {
    const CFormationBase& formation = **it;
    for(int i = 0; i < formation.ElementSetSize(); ++i)
    {
      const IFormationElementSet& form_elset = formation.ElementSet(i);
      const geo::IElementSet& elm_set = form_elset.ElementSet();
      if(&elm_set == &group)
      {
        std::stringstream str;
        str << (nIndex + 1) << " " << formation.Name().toStdString();
        return str.str();
      }
    }
  }

  // faults
  THorizonBaseEntry& hor_entry = (THorizonBaseEntry&)*Model().GraphEntry(MD_BASE_HORIZON);
  THorizonBaseEntry::TNodeSet stHorizons = hor_entry.EntryNodes();
  for(THorizonBaseEntry::TNodeSet::iterator it = stHorizons.begin(); it != stHorizons.end(); ++it)
  {
    const CHorizonBase& horizon = **it;
    if(horizon.Slip())
    {
      const geo::CElementGroup& el_group = *horizon.InterfaceElementGroup();
      if(&el_group == &group)
      {
        std::stringstream str;
        str << (nIndex + 1) << " " << horizon.Name().toStdString();
        return str.str();
      }
    }
  }

  assert(false);
  return std::string();
}

QString CGeomecDianaRunnerBase::DianaExecutable()
{
	return CEnvironment::instance()->get( 
		m_controller.WriteInputFiles() ?
			CEnvironment::InputExeName :
			CEnvironment::DianaExeName );
}

void CGeomecDianaRunnerBase::PreExecution()
{
	printer->info("pre execution");

	char *pEnvStop = DiGetenv("GEOMEC_BREAKATANALYSIS");
	if(pEnvStop && (*pEnvStop == 'y' || *pEnvStop == 'Y'))
		_m()->msg("Debug stop, click OK to continue...");

  CAnalysisLogger& logger = Model().Logger();
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("'*** PreExecution at' dd MMM yyyy',' hh':'mm '***'");
  logger.AddLine(strTime.toStdString(), true);
}

void CGeomecDianaRunnerBase::PostExecution(bool bCalcResult)
{
	printer->info("post execution");

	char *pEnvStop = DiGetenv("GEOMEC_BREAKAFTERANALYSIS");
	if(pEnvStop && (*pEnvStop == 'y' || *pEnvStop == 'Y'))
		_m()->msg("Debug stop, click OK to continue...");

  CAnalysisLogger& logger = Model().Logger();
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("'*** PostExecution at' dd MMM yyyy',' hh':'mm '***'");
  logger.AddLine(strTime.toStdString(), true);

	m_bAnalysisSuccess = bCalcResult;
}

void CGeomecDianaRunnerBase::AnalysisSuccess(bool bAnalysisSuccess)
{
  m_bAnalysisSuccess = bAnalysisSuccess;
}

bool CGeomecDianaRunnerBase::LoadPartialResults()
{
  return Model().ResultRegister().ReadFilosResults(FF().c_str());
}

void CGeomecDianaRunnerBase::OnError(eError id, const std::string& msg2)
{	
	if( id == Running || id == RunAnalysis )
	{
    QString msg = getStringTableEntry(id == Running ?
      IDP_ERRORRUNNINGANALYSIS :	// error starting kernel
      IDP_ERRORDURINGANALYSIS);   // error during calculation

    if (!msg2.empty())
      msg += QString("\nAdditional information: %1").arg(msg2.c_str());

    _m()->error(msg);
	}
	else if (id == CreateSubDir)
	{
		printer->error("create subdir");

		QString msg("Unable to create a working directory in the temp directory '%1'. Is the temp directory writable?");
		msg = msg.arg(msg2.c_str());
		_m()->error(msg);
	}
	else
		dia::IDianaRunner::OnError( id, msg2 );
}

bool CGeomecDianaRunnerBase::OnEndAnalysis(
	const QString& getPathName,
	ISaveModel& saveModel)
{
	printer->info("on end analysis");

	bool bRet = true;

  CAnalysisLogger& logger = Model().Logger();
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("'*** OnEndAnalysis at' dd MMM yyyy',' hh':'mm '***'");
  logger.AddLine(strTime.toStdString(), true);

	if(!m_controller.WriteInputFiles() && !m_controller.WriteQuadDat())
	{
		Model().ResultRegister().ReadFilosResults(
			m_controller.StartStage(),
			m_controller.EndStage(),
			m_controller.AnalysisType());

		bRet =	!m_AllowCleanUpAfterRun || 
				m_controller.OnWriteBranchFiles(
					getPathName, 
					m_bAnalysisSuccess,
					saveModel);
	}

	if(m_controller.WriteQuadDat() && !m_sQuadDatFileDir.empty())
	{
		// copy the resulting .dat file
		assert(!m_sQuadDatFileName.empty());

		// we need to move the generated .dat file to the desired location
		FILE *fpin = fopen(m_sQuadDatFileName.c_str(), "r");
		if(!fpin) 
			return false;

		FILE *fpout = fopen(m_sDatFile.c_str(), "w");
		if(!fpout)
		{
			fclose(fpin);
			return false;
		}

		char buf[BS];

		while(fgets(buf, BS - 1, fpin)) 
			fputs(buf, fpout);

		fclose(fpin);
		fclose(fpout);

		unlink(m_sQuadDatFileName.c_str());
	}

	return bRet;
}

bool CGeomecDianaRunnerBase::OnEndWriteCommands(const std::string &title)
{
	printer->info("end write commands");

	if(m_controller.WriteQuadDat())
	{
		// append input reader
		int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
		PushDir();
		ChangeDir("/CMNDS");

		CEnvironment* env = CEnvironment::instance();
		QString strInputExeName = env->get( CEnvironment::InputExeName );

		strInputExeName = strInputExeName.toUpper();
		if(strInputExeName.right(4).compare(".EXE") == 0)
			strInputExeName = strInputExeName.left(strInputExeName.length() - 4);

		//ftn_int_t idx = 20;
		ftn_int_t idx = Inquire("SEGINF", "DIM") + 10; // next
		ChangeIndexedDir("SEGINF/", &idx);
		PutCharItem("MODULE", "AP");
		PutCharItem("SEGMEN", strInputExeName.toStdString().c_str());

		ChangeDir("INPUT");
		ChangeDir("REMAKE");

		char *filename = DiStrsave(m_sDatFile.c_str());
		char *pfname = strrchr(filename, '\\');
		if(!pfname) pfname = strrchr(filename, '/');

		if(pfname)
		{
			*pfname = 0;
			pfname++;
			m_sQuadDatFileDir = filename;
			m_sQuadDatFileName = pfname;
		}
		else
		{
			m_sQuadDatFileName = filename;
			m_sQuadDatFileDir = std::string();
		}

		PutCharItem("FILE", m_sQuadDatFileName.c_str());

		DiFree(filename, "CGeomecDianaRunnerBase::OnEndWriteCommands");

    WriteTableCommands();

		PopDir();
		SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);
	}

	PushDir();
		ChangeDir("/JOBINF");
		PutCharItem("BASENM", title.c_str());
    if (m_controller.AnalysisType() == CAnalysisType::AT_MIXTURE ||
        m_controller.AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
    {
      ftn_int_t ernoin = 1;
      PutItem("ERNOIN", &ernoin);
    }
	PopDir();

	return CDianaRunner::OnEndWriteCommands(title);
}

void CGeomecDianaRunnerBase::WriteTableCommands()
{
	printer->info("write table commands");

  const CDepletionStage& stage = m_controller.StartStage();
  if(stage.Initial())
  {
    ftn_int_t idx = 1;

    PushDir();

    PushDir();
	  ChangeIndexedDir("TABLE/", &idx);
    SetActive("COORDI", FTN_TRUE);
	  PutCharItem("FMT", "%23.15E");
    PopDir();

    ++idx;
    ChangeIndexedDir("TABLE/", &idx);

    SetActive("BOUNDA", FTN_TRUE);
    SetActive("CONCEN", FTN_TRUE);
    SetActive("COOLPI", FTN_TRUE);
    SetActive("DATA", FTN_TRUE);
    SetActive("EQUAL", FTN_TRUE);
    SetActive("ESTIMA", FTN_TRUE);
    SetActive("FIXPOT", FTN_TRUE);
    SetActive("FIXTEM", FTN_TRUE);
    SetActive("FREQLO", FTN_TRUE);
    SetActive("GEOMET", FTN_TRUE);
    SetActive("HUMIDI", FTN_TRUE);
    SetActive("INIVAR", FTN_TRUE);
    SetActive("LOADS", FTN_TRUE);
    SetActive("MATERI", FTN_TRUE);
    SetActive("MATURI", FTN_TRUE);
    SetActive("PRESS2", FTN_TRUE);
    SetActive("PRESSU", FTN_TRUE);
    SetActive("SUPPOR", FTN_TRUE);
    SetActive("TARGET", FTN_TRUE);
    SetActive("TEMPER", FTN_TRUE);
    SetActive("TIMEBO", FTN_TRUE);
    SetActive("TIMELO", FTN_TRUE);
    SetActive("TYINGS", FTN_TRUE);
    SetActive("UNITS", FTN_TRUE);
    SetActive("VOIDS", FTN_TRUE);
    SetActive("DIRECT", FTN_TRUE);
    SetActive("ELEMEN", FTN_TRUE);
    SetActive("MODEL", FTN_TRUE);
    SetActive("GROUPS", FTN_TRUE);
    SetActive("REINFO", FTN_TRUE);
    SetActive("FILOS", FTN_TRUE);
    SetActive("GEPLST", FTN_TRUE);  // Requires Diana >= 2016-07-21

    PopDir();
  }
  else
  {
    // see if any of the following changes in this stage:
    // - formation pressure
    bool bFormationPressure = false;
    // - formation temperature
    bool bFormationTemperature = false;
    // - formation volumetric strain
    bool bFormationVolStrain = false;
    // - formation material parameters
    bool bFormationMaterial = false;
    // - fault pressure
    bool bFaultPressure = false;
    // - fault material parameters
    bool bFaultMaterial = false;

    const TFormationBaseEntry& formation_entry = static_cast<const TFormationBaseEntry&>(*Model().GraphEntry(MD_BASE_FORMATION));
    const TFormationBaseEntry::TNodeSet& stFormations = formation_entry.EntryNodes();
    for(TFormationBaseEntry::TNodeSet::const_iterator it = stFormations.begin(); it != stFormations.end(); ++it)
    {
      const CFormationBase& formation = **it;
      if(formation.Pressure(stage).Type() != CPressure::REPEAT_MODE ||
         formation.Pressure(stage).DistributedSize() > 0)
      {
        bFormationPressure = true;
      }

      // always write calculated temperatures...
      if(&formation.EffectiveTemperature(stage) != &formation.UserTemperature(stage) ||
         formation.UserTemperature(stage).Type() != CPressure::REPEAT_MODE ||
         formation.UserTemperature(stage).DistributedSize() > 0)
      {
        bFormationTemperature = true;
      }

      if(!stage.Previous().Initial() ||
         formation.Strain(stage).DistributedSize() > 0 ||
         formation.Strain(stage).IsVolumetric())
      {
        bFormationVolStrain = true;
      }

      if(formation.ConnectedMaterial(stage)->LibraryMaterial() != 0)
      {
        bFormationMaterial = true;
      }
    }

    const THorizonBaseEntry& horizon_entry = static_cast<const THorizonBaseEntry&>(*Model().GraphEntry(MD_BASE_HORIZON));
    const THorizonBaseEntry::TNodeSet& stHorizons = horizon_entry.EntryNodes();
    for(THorizonBaseEntry::TNodeSet::const_iterator it = stHorizons.begin(); it != stHorizons.end(); ++it)
    {
      const CHorizonBase& horizon = **it;
      if(horizon.Slip())
      {
        if(horizon.Pressure(stage).Mode() == CFaultPressure::MT_MAXIMUM ||
           horizon.Pressure(stage).Mode() == CFaultPressure::MT_MINIMUM)
        {
          bFaultPressure = bFormationPressure;
        }
        else if(horizon.Pressure(stage).Mode() == CFaultPressure::MT_GRADIENT)
        {
          bFaultPressure = true;
        }

        const CFaultParametersNode& fpn = horizon.FaultParameters(stage);
        if(!fpn.CohesionSameAsPrevious() ||
           !fpn.FrictionSameAsPrevious() ||
           !fpn.DStiffSameAsPrevious())
        {
          bFaultMaterial = true;
        }
      }
    }

    PushDir();
    ChangeDir("TABLE");
    SetActive("DIRECT", FTN_TRUE);

    if(bFormationPressure ||
       bFormationTemperature ||
       bFormationVolStrain ||
       bFormationMaterial ||
       bFaultPressure ||
       bFaultMaterial)
    {
      if(bFormationPressure || bFaultPressure)
        SetActive("LOADS", FTN_TRUE);

      if(bFormationTemperature)
        SetActive("TEMPER", FTN_TRUE);

      if(bFormationVolStrain)
        SetActive("CONCEN", FTN_TRUE);

      if(bFormationMaterial || bFaultMaterial)
      {
        SetActive("MATERI", FTN_TRUE);

        // must write ELEMEN as well as indices may have changed...
        SetActive("ELEMEN", FTN_TRUE);
      }
    }

    PopDir();
  }
}

bool CGeomecDianaRunnerBase::OnEndPreprocessWriteInputFiles()
{
	printer->info("end pre-process write input files");
  PushDir();

  ChangeDir("/CMNDS");

  ftn_int_t idx = 10;
	ChangeIndexedDir("SEGINF/", &idx);

  ChangeDir("INPUT");

  idx = 1;
  ChangeIndexedDir("REMAKE/", &idx);

  WriteTableCommands();

  PopDir();

  return true;
}

bool CGeomecDianaRunnerBase::OnEndPostprocessWriteInputFiles(const std::string& /*sComfileName*/, const std::string& sDatfileName)
{
	printer->info("end post-process write input files");

  CTetraMesh* pTetMesh = dynamic_cast<CTetraMesh*>(&Model().Mesh());
  if(pTetMesh)
  {
    geo::CVector vecNormal;
    geo::CVector vecTangent;

    pTetMesh->GetModelOrientation(vecNormal, vecTangent);

    FILE* fpin;
    FILE* fpout;

    bool bCopied = false;

    fpin = fopen(sDatfileName.c_str(), "r");
    if(fpin)
    {
      char* tmpdatfile = vDiStrsave(GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION).toStdString().c_str(), "\\geomecdatXXXXXX", 0);
      mktemp(tmpdatfile);
      fpout = fopen(tmpdatfile, "w");
      if(fpout)
      {
        // write the FILOS statement for model orientation to the dat file
        fprintf(fpout, "'FILOS'\n");
        fprintf(fpout, "/GEOMEC/DIRECT %g %g 0\n\n", vecNormal.X(), vecNormal.Y());

        // append the rest of the dat file
        char buf[1024];
        while(fgets(buf, 1024, fpin))
        {
          fputs(buf, fpout);
        }

        bCopied = true;

        fclose(fpout);
      }
      else
      {
        IPlatform* platform = IPlatform::instance();
        QString traceString = QString("Could not open temp file %1\n").arg(tmpdatfile);

        platform->trace(traceString);
      }

      fclose(fpin);

      if(bCopied)
      {
        QFile::remove(sDatfileName.c_str());
        QFile::copy(tmpdatfile, sDatfileName.c_str());
        unlink(tmpdatfile);
      }

      DiFree(tmpdatfile, "CGeomecCompactionDianaRunner::OnEndPostprocessWriteInputFiles");
    }
    else
    {
      IPlatform* platform = IPlatform::instance();
      QString traceString = QString("Could not open %1 for modification\n").arg(sDatfileName.c_str());

      platform->trace(traceString);
    }
  }

  return true;
}

bool CGeomecDianaRunnerBase::OnWriteCommands()
{
  SetActive("/JOBINF/IS3D", FTN_TRUE);

  return true;
}

bool CGeomecDianaRunnerBase::OnWriteMiscelaneous()
{
  ftn_int_t nMaxWarn = 3; // one more than we actually want (2), TFS #78876
  PutItem("/JOBINF/MWARNI", &nMaxWarn);

  return CDianaRunner::OnWriteMiscelaneous();
}

bool CGeomecDianaRunnerBase::OnWriteElementOrder(bool forceQuadratic)
{
  if (forceQuadratic)
  {
    PutCharItem("ORDELM", "QUADRA");
  }
  else
  {
		const CModelBase *pModel = static_cast<const CModelBase *>(&MeshBase().Model());
    assert(pModel);
    const TFormationBaseEntry* pFormationEntry = static_cast<const TFormationBaseEntry*>(pModel->GraphEntry(MD_BASE_FORMATION));
    assert(pFormationEntry);
    const TFormationBaseEntry::TNodeSet& stNodes = pFormationEntry->EntryNodes();

    std::vector<const CFormationBase *> vcGlobal;
    std::vector<const CFormationBase *> vcLinear;
    std::vector<const CFormationBase *> vcQuadra;

    CElementOrder::TElementOrder nOrder = pModel->ElementOrder();

    bool bAllow = true;

    if (dynamic_cast<const CHexaModel *>(pModel))
    {
      const CHexaMeshRegionEntry *pEntry = dynamic_cast<const CHexaMeshRegionEntry *>(pModel->GraphEntry(MD_HEXA_MESH_ZONE));
      if (pEntry)
      {
        const CHexaMeshRegionEntry::TNodeSet& stRegions = pEntry->EntryNodes();

        if (stRegions.size() > 1)
          bAllow = false;
      }
    }

    vcGlobal.reserve(stNodes.size());
    vcLinear.reserve(stNodes.size());
    vcQuadra.reserve(stNodes.size());

    size_t nQuadratic = 0;

    for (TFormationBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
    {
      if ((*it)->inheritElementOrderFromModel())
        vcGlobal.push_back(*it);
      else
        switch ((*it)->ElementOrder())
        {
          case CElementOrder::EO_LINEAR:
            vcLinear.push_back(*it);
            break;
          case CElementOrder::EO_QUADRATIC:
            ++nQuadratic;
            /* Fall-through */
          case CElementOrder::EO_INIQUAD:
            vcQuadra.push_back(*it);
            break;
        }
    }

    if (stNodes.size() != vcGlobal.size()) // we may have conversions
    {
      switch (nOrder)
      {
      case CElementOrder::EO_LINEAR:
        for (std::vector<const CFormationBase *>::const_iterator it = vcGlobal.begin(); it != vcGlobal.end(); ++it)
          vcLinear.push_back(*it);
        break;
      case CElementOrder::EO_QUADRATIC:
        nQuadratic += vcGlobal.size();
        /* Fall-through */
      case CElementOrder::EO_INIQUAD:
        for (std::vector<const CFormationBase *>::const_iterator it = vcGlobal.begin(); it != vcGlobal.end(); ++it)
          vcQuadra.push_back(*it);
      }

      if (stNodes.size() == vcLinear.size())
        nOrder = CElementOrder::EO_LINEAR;
      else if (bAllow)
      {
        if (nQuadratic == 0)
        {
          nOrder = CElementOrder::EO_INIQUAD;
        }
        else if (nQuadratic == vcQuadra.size())
        {
          nOrder = CElementOrder::EO_QUADRATIC;
        }
        else
        {
          // we can only support one

          QString msg("Found both 'Quadratic' and 'Quadratic initially, then linear' formation settings; ");

          switch (nOrder)
          {
          case CElementOrder::EO_LINEAR: // pick largest set
            if (nQuadratic >= vcQuadra.size() / 2)
            {
              nOrder = CElementOrder::EO_QUADRATIC;
              msg += "ignoring the latter";
            }
            else
            {
              nOrder = CElementOrder::EO_INIQUAD;
              msg += "ignoring the former";
            }
            break;
          case CElementOrder::EO_QUADRATIC:
            msg += "using global setting ('Quadratic')";
            break;
          case CElementOrder::EO_INIQUAD:
            msg += "using global setting ('Quadratic initially, then linear')";
            break;
          }

          const_cast<CAnalysisLogger&>(pModel->Logger()).AddLine(msg.toStdString());
        }
      }
      else
      {
        assert(!bAllow);
        if (stNodes.size() != vcQuadra.size())
        {
          _m()->error("Hexamodel with submeshregions does not support quadratic elements per formation; please adjust your input");
          return false;
        }
      }
    }

		switch(nOrder)
		{
    case CElementOrder::EO_LINEAR:
      PutCharItem("ORDELM", "LINEAR");
			break;
    case CElementOrder::EO_QUADRATIC:
      PutCharItem("ORDELM", "QUADRA");
			break;
    case CElementOrder::EO_INIQUAD:
      PutCharItem("ORDELM", "QUALIN");
      break;
		default:
			assert(FALSE);
		}

    if (vcQuadra.size() > 0 && vcQuadra.size() < stNodes.size())
    {
      QString elmsets;
      for (std::vector<const CFormationBase *>::const_iterator it = vcQuadra.begin(); it != vcQuadra.end(); ++it)
      {
        elmsets += m_mpFormationElmsetNames[*it];
      }
      QStringList l = elmsets.split(' ');

      ftn_char_t *elmsetptr = (ftn_char_t *)DiMalloc(l.size() * sizeof(ftn_char_t), "IDianaRunner::OnWriteElementOrder");
      ftn_char_t  elmsetval = (ftn_char_t)DiMalloc(l.size() * 25 * sizeof(ftn_char_t), "IDianaRunner::OnWriteElementOrder");

      for (int j = 0; j < l.size(); ++j)
      {
        elmsetptr[j] = elmsetval + j * 10;
        strncpy(elmsetptr[j], l[j].toStdString().c_str(), 25);
      }

      ftn_int_t idx = 0;
      PutIndexedCharItemLength("QUASET", &idx, elmsetptr, l.size());

      DiFree(elmsetptr, "IDianaRunner::OnWriteElementOrder");
      DiFree(elmsetval, "IDianaRunner::OnWriteElementOrder");
    }
  }
  return true;
}

void CGeomecDianaRunnerBase::GenerateTimePoints(dia::IValueTable& table) const
{
  CDepletionStageEntry::iterator it;
  for(it = Model().DepletionStageEntry().begin(); it != Model().DepletionStageEntry().end(); ++it)
  {
		double secs = Model().InitialDepletionStage().Time().SecondsTo(it->Time());

    // time in months
		double dtime = secs / SECONDS_PER_MONTH;
    table.AddTime(dtime);

    // don't go beyond end stage
    if(&(*it) == &Controller().EndStage())
      break;
  }
}

void CGeomecDianaRunnerBase::GenerateMoreTimePoints(dia::IValueTable& table) const
{
  int oldSize = table.Size();

  int i;
  CDepletionStageEntry::iterator it;
  for(i = 0, it = Model().DepletionStageEntry().begin(); it != Model().DepletionStageEntry().end(); ++it, ++i)
  {
    if (i >= oldSize)
    {
		  double secs = Model().InitialDepletionStage().Time().SecondsTo(it->Time());

      // time in months
		  double dtime = secs / SECONDS_PER_MONTH;
      table.AddTime(dtime);
    }

    // don't go beyond end stage
    if(&(*it) == &Controller().EndStage())
      break;
  }

  table.CleanUp(false);
}

// For a temper table, create an instance of the derived class of ITemperatureTable on the heap
// The runner will delete this pointer. Override this in derived classes if you don't want the
// temp table. Just return 0 in that case.
dia::ITemperatureTable *CGeomecDianaRunnerBase::OnCreateTemperatureTable() const
{
  return 0;
/*
	CTemperatureTable* pTable = new CTemperatureTable(*this, Model());
  GenerateTimePoints(*pTable);

  int i;
  CDepletionStageEntry::iterator it;
  for(i = 0, it = Model().DepletionStageEntry().begin(); i < pTable->Size() && it != Model().DepletionStageEntry().end(); ++it, ++i)
    pTable->RegisterTemperaturePoint(pTable->Time(i), *it);

	return pTable;
*/
}

void CGeomecDianaRunnerBase::OnExtendTemperatureTable(dia::ITemperatureTable* /*pTable*/) const
{
/*
  CTemperatureTable* pTemperatureTable = static_cast<CTemperatureTable*>(pTable);

  int oldSize = pTable->Size();
  GenerateMoreTimePoints(*pTable);

  int i;
  CDepletionStageEntry::iterator it;
  for(i = 0, it = Model().DepletionStageEntry().begin(); i < pTable->Size() && it != Model().DepletionStageEntry().end(); ++it, ++i)
    if (i >= oldSize)
      pTemperatureTable->RegisterTemperaturePoint(pTable->Time(i), *it);
*/
}

dia::IConcentrationTable* CGeomecDianaRunnerBase::OnCreateConcentrationTable() const
{
  CConcentrationTable* pTable = new CConcentrationTable(*this, Model());
  GenerateTimePoints(*pTable);

  int i;
  CDepletionStageEntry::iterator it;
  for(i = 0, it = Model().DepletionStageEntry().begin(); i < pTable->Size() && it != Model().DepletionStageEntry().end(); ++it, ++i)
    pTable->RegisterConcentrationPoint(pTable->Time(i), *it);

	return pTable;
}

void CGeomecDianaRunnerBase::OnExtendConcentrationTable(dia::IConcentrationTable* pTable) const
{
  CConcentrationTable* pConcentrationTable = static_cast<CConcentrationTable*>(pTable);

  int oldSize = pTable->Size();
  GenerateMoreTimePoints(*pTable);

  int i;
  CDepletionStageEntry::iterator it;
  for(i = 0, it = Model().DepletionStageEntry().begin(); i < pTable->Size() && it != Model().DepletionStageEntry().end(); ++it, ++i)
    if (i >= oldSize)
      pConcentrationTable->RegisterConcentrationPoint(pTable->Time(i), *it);
}


// Return true if you want to export .com and .dat files
// instead of actually running the analysis.
// The analysis will not be performed, but input
// will be run to generate the data file.
bool CGeomecDianaRunnerBase::WriteInputFiles(std::string &comfilename, std::string &datfilename) const
{
	if(m_controller.WriteInputFiles())
	{
		assert(!m_sComFile.empty());
		assert(!m_sDatFile.empty());
		comfilename = m_sComFile;
		datfilename = m_sDatFile;
		return true;
	}

	return false;
}

// return material for the given element
const dia::IMaterial &CGeomecDianaRunnerBase::Material(const geo::IElement &element) const
{
	const dia::IMaterial *pInterfaceMaterial = InterfaceMaterial(element);
	if(pInterfaceMaterial) return *pInterfaceMaterial;

	const CFormationBase &formation = *MeshBase().Formation(element);
	assert(&formation != 0);
	return formation.Material(Controller().StartStage()).Material(element);
}

bool CGeomecDianaRunnerBase::CanCleanupAfterRun() const
{
	bool res = m_AllowCleanUpAfterRun && (&m_controller.EndStage() == &m_controller.EndStage().LastStage() || !m_bAnalysisSuccess);

	printer->debug("can cleanup : (allowed && last || !success) : allowed:%s, last stage :%s, ana_success:%s", 
		m_AllowCleanUpAfterRun ? "yes":"no", 
		&m_controller.EndStage() == &m_controller.EndStage().LastStage() ? "yes":"no",
		m_bAnalysisSuccess ? "yes":"no" );
	
	return res;
}

void CGeomecDianaRunnerBase::AllowCleanUpAfterRun(bool allow)
{
  m_AllowCleanUpAfterRun = allow;
}

void CGeomecDianaRunnerBase::ForceCleanUp(const std::string &subdir)
{
  Cleanup(subdir);
}

// return an existing path if you want to use that instead of a generated one
std::string CGeomecDianaRunnerBase::UserDefinedLocation() const
{
	return m_strLocation;
}

// allows user filos file (e.g. for restart)
//  strFilosFile - set the name of the file
//  bInitFilos   - set to false if filos file should not be initialized
bool CGeomecDianaRunnerBase::UserDefinedFilosFile(const QString& getPathName,
	std::string& strFilosFile, bool& bInitFilos) const
{
	bInitFilos = &m_controller.StartStage() == &m_controller.StartStage().InitialStage();

	if(!bInitFilos)
	{
		if(m_strFileName.empty())
		{
			// initial run, but from a branch stage
			assert(!getPathName.isEmpty()); // model should have been saved at least once

			QString strDocBase;
			int iDot = getPathName.lastIndexOf('.');
			if(iDot >= 0)
				strDocBase = getPathName.left(iDot); // excluding dot
			else
				strDocBase = getPathName; // no dot found

			QString strFilosPath = strDocBase + ".ff";

#ifdef _WIN32
      struct _stat64 buf;
			if(_stat64(strFilosPath.toStdString().c_str(), &buf) != 0 || !(buf.st_mode & S_IFREG))
#else
			struct _stat buf;
			if(_stat(strFilosPath.toStdString().c_str(), &buf) != 0 || !(buf.st_mode & S_IFREG))
#endif
			{
        if(!m_controller.StartStage().Initial() && !m_controller.StartStage().Previous().Initial())
        {
          // try appending stage index
          QString strAppend;
          strAppend = QString("_D%1").arg(m_controller.StartStage().Previous().Index());
          strFilosPath = strDocBase + strAppend + ".ff";
        }
      }

#ifdef _WIN32
			if(_stat64(strFilosPath.toStdString().c_str(), &buf) != 0 || !(buf.st_mode & S_IFREG))
#else
			if(_stat(strFilosPath.toStdString().c_str(), &buf) != 0 || !(buf.st_mode & S_IFREG))
#endif
			{
				QString msg;
				msg = QString("Could not find branch database '%1'\n\nThe analysis can not be performed").arg(strDocBase + ".ff");
				_m()->error(msg);
				return false;
			}

			// copy the file
			QString strFilosFileName = strFilosPath;
			int idxSlash = strFilosFileName.lastIndexOf(QDir::separator());
			if(idxSlash >= 0)
				strFilosFileName = strFilosFileName.right(strFilosFileName.length() - idxSlash - 1);

			ftn_int_t status = TestFilosFileStatus(strFilosPath.toStdString().c_str());
			if(status != 0)
			{
				QString msg;
				msg = QString("The file '%1' is not a valid calculation database\n\nThe analysis can not be performed").arg(strFilosPath);
				_m()->error(msg);
				return false;
			}

			CFileCopier fcopier(strFilosPath.toStdString().c_str(), (QString(FFDIR().c_str()) + QDir::separator() + strFilosFileName).toStdString().c_str(),
				"Preparing calculation database");
			if(!fcopier.Copy())
			{
				_m()->error("Unable to prepare the calculation database\n\nThe analysis can not be performed");
				return false;
			}

			strFilosFile = strFilosFileName.toStdString();
		}
		else
		{
			// continuation
			strFilosFile = m_strFileName;
		}
	}

	return true;
}

CMeshBase &CGeomecDianaRunnerBase::MeshBase() const
{
	return m_MeshBase;
}

CModelBase &CGeomecDianaRunnerBase::Model() const
{
	return m_Model;
}

bool CGeomecDianaRunnerBase::isAnisotropic(const CFFMaterial* cffMaterial) const
{
  return (cffMaterial->IsParameter(IDT_VALUETYPE_YOUNGMODULUS_NORM) &&
    cffMaterial->IsParameter(IDT_VALUETYPE_YOUNGMODULUS_TRANS) &&
    cffMaterial->IsParameter(IDT_VALUETYPE_POISSONRATIO_NORM) &&
    cffMaterial->IsParameter(IDT_VALUETYPE_POISSONRATIO_TRANS));
}

bool CGeomecDianaRunnerBase::isAnisotropic(const geo::IElement& element) const
{
  // for CFFMaterial (equivalent to having formation) we can just check the library
	const CFormationBase *formation = MeshBase().Formation(element);

  if (formation)
  {
  	const IMaterialRock *library = formation->Material(Controller().StartStage()).LibraryMaterial();

    return library &&
      library->IsParameter(IDT_VALUETYPE_YOUNGMODULUS_NORM) &&
      library->IsParameter(IDT_VALUETYPE_YOUNGMODULUS_TRANS) &&
      library->IsParameter(IDT_VALUETYPE_POISSONRATIO_NORM) &&
      library->IsParameter(IDT_VALUETYPE_POISSONRATIO_TRANS);
  }

  return false;
}

////////////////////////////////////////
// class CTemperatureTable
////////////////////////////////////////

CGeomecDianaRunnerBase::CTemperatureTable::CTemperatureTable(const CGeomecDianaRunnerBase &runner, const CModelBase &model)
: dia::ITemperatureTable(runner),
  m_runner(const_cast<CGeomecDianaRunnerBase*>(&runner)),
  m_model(model)
{
}

void CGeomecDianaRunnerBase::CTemperatureTable::RegisterTemperaturePoint(const dia::IValueTable::CTimePoint& tpoint, const CDepletionStage& dstage)
{
	VERIFY(m_mpTimePoint2DeplStage.insert(TTimePoint2DeplStageMap::value_type(&tpoint, &dstage)).second);
}

void CGeomecDianaRunnerBase::CTemperatureTable::SetRunner(dia::IDianaRunner *runner)
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(runner));
  m_runner = static_cast<CGeomecDianaRunnerBase*>(runner);
}

bool CGeomecDianaRunnerBase::CTemperatureTable::HasValue(const geo::IElement &element) const
{
  return m_runner->HasTemperatureValue(element);
}

void CGeomecDianaRunnerBase::CTemperatureTable::ValueAt(const geo::IElement &element, const dia::IValueTable::CTimePoint &tpoint, std::vector<double>& vcValues) const
{
	assert(!dynamic_cast<const geo::CInterfaceElement *> (&element));
	// get the depletion stage
	TTimePoint2DeplStageMap::const_iterator it = m_mpTimePoint2DeplStage.find(&tpoint);
	assert(it != m_mpTimePoint2DeplStage.end());
	const CDepletionStage& dstage = *it->second;

  IValueDomainScalar::TValueVec vcTempValues = m_runner->ElementTemperatures(element, dstage);

  // should have the right number of values in the vector
	assert(vcTempValues.size() == element.NrOfNodes());
	vcValues.resize(vcTempValues.size());

	for(size_t i = 0; i < vcTempValues.size(); ++i)
		vcValues[i] = vcTempValues[i].Value() + 273.15; // convert to Kelvin
}

////////////////////////////////////////
// class CConcentrationTable
////////////////////////////////////////

CGeomecDianaRunnerBase::CConcentrationTable::CConcentrationTable(const CGeomecDianaRunnerBase &runner, const CModelBase &model)
: dia::IConcentrationTable(runner),
  m_runner(const_cast<CGeomecDianaRunnerBase*>(&runner)),
  m_model(model)
{
}

void CGeomecDianaRunnerBase::CConcentrationTable::RegisterConcentrationPoint(const dia::IValueTable::CTimePoint& tpoint, const CDepletionStage& dstage)
{
	VERIFY(m_mpTimePoint2DeplStage.insert(TTimePoint2DeplStageMap::value_type(&tpoint, &dstage)).second);
}

void CGeomecDianaRunnerBase::CConcentrationTable::SetRunner(dia::IDianaRunner *runner)
{
  m_runner = static_cast<CGeomecDianaRunnerBase*>(runner);
}

bool CGeomecDianaRunnerBase::CConcentrationTable::HasValue(const geo::IElement &element) const
{
  return m_runner->HasConcentrationValue(element);
}

void CGeomecDianaRunnerBase::CConcentrationTable::ValueAt(const geo::IElement &element, const dia::IValueTable::CTimePoint &tpoint, std::vector<double>& vcValues) const
{
	assert(!dynamic_cast<const geo::CInterfaceElement *> (&element));
	// get the depletion stage
	TTimePoint2DeplStageMap::const_iterator it = m_mpTimePoint2DeplStage.find(&tpoint);
	assert(it != m_mpTimePoint2DeplStage.end());
	const CDepletionStage& dstage = *it->second;

  if(dstage.Initial())
  {
    vcValues.resize(element.NrOfNodes(), 0);
  }
  else
  {
    IValueDomainScalar::TValueVec vcConcenValues = m_runner->ElementConcentrations(element, dstage);
    bool bValid = true;
    bool bWrite = false;
    bool bEqual = true;
    std::vector <double> vcElementValues =
      m_runner->recalculateElementConcentrations(
        bValid, bWrite, bEqual, vcConcenValues, element, dstage);

	  // should have the right number of values in the vector
	  assert(vcElementValues.size() == element.NrOfNodes());
	  vcValues.resize(vcElementValues.size());

	  for(size_t i = 0; i < vcElementValues.size(); ++i)
		  vcValues[i] = vcElementValues[i];
  }
}

bool CGeomecDianaRunnerBase::CConcentrationTable::CompressTable() const
{
  // gm42 requires a full concentration table to be written
  return false;
}


/////

CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::CUpscalingAnisotropyElementGeometry(double dInclination, double dAzimuth)
: m_dInclination(dInclination),
  m_dAzimuth(dAzimuth)
{
}

bool CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::WriteFilos(IDianaRunner& /*diarunner*/) const
{
  // convert to radians
  double azi = m_dAzimuth * PI / 180;
  double inc = m_dInclination * PI / 180;

  // normal vector (normal to layers)
  geo::CVector vecNormal(cos(azi)*sin(inc), sin(azi)*sin(inc), -1.0 * cos(inc));

  // perpendicular vector in layer plane
  geo::CVector vecY(-sin(azi), cos(azi), 0);

  // X perpendicular to normal and Y
  geo::CVector vecX(vecNormal.CrossProduct(vecY));

  ftn_double_t xaxis[3];
  xaxis[0] = vecX.X();
  xaxis[1] = vecX.Y();
  xaxis[2] = vecX.Z();
  PutItemLength("XAXIS", xaxis, 3);

  ftn_double_t yaxis[3];
  yaxis[0] = vecY.X();
  yaxis[1] = vecY.Y();
  yaxis[2] = vecY.Z();
  PutItemLength("YAXIS", yaxis, 3);

  return true;
}

int CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::Type() const
{
  return 201;
}

int CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::WriteFilosParamSize(IDianaRunner& /*diarunner*/) const
{
  return 6;
}

bool CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::WriteFilosParamName(IDianaRunner& /*diarunner*/, int i, char *name) const
{
  if (i < 3)
  {
    QString xaxis = QString("XAXIS(%1)").arg(i + 1);
    strncpy(name, xaxis.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  if (i < 3)
  {
    QString yaxis = QString("YAXIS(%1)").arg(i + 1);
    strncpy(name, yaxis.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  return false;
}

void CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::WriteFilosParamValues(IDianaRunner& /*diarunner*/, double *values, int stride) const
{
  double azi = m_dAzimuth * PI / 180;
  double inc = m_dInclination * PI / 180;

  // normal vector (normal to layers)
  geo::CVector vecNormal(cos(azi)*sin(inc), sin(azi)*sin(inc), -1.0 * cos(inc));

  // perpendicular vector in layer plane
  geo::CVector vecY(-sin(azi), cos(azi), 0);

  // X perpendicular to normal and Y
  geo::CVector vecX(vecNormal.CrossProduct(vecY));

  *values = vecX.X(); // XAXIS(1)
  values += stride;

  *values = vecX.Y(); // XAXIS(2)
  values += stride;

  *values = vecX.Z(); // XAXIS(3)
  values += stride;


  *values = vecY.X(); // YAXIS(1)
  values += stride;

  *values = vecY.Y(); // YAXIS(2)
  values += stride;

  *values = vecY.Z(); // YAXIS(3)
  values += stride;
}

bool CGeomecDianaRunnerBase::CUpscalingAnisotropyElementGeometry::operator<(const IElementProperty &rhs) const
{
  const CUpscalingAnisotropyElementGeometry* pGeom = dynamic_cast<const CUpscalingAnisotropyElementGeometry*>(&rhs);
  if(pGeom)
  {
    if(m_dInclination < pGeom->m_dInclination)
      return true;
    if(m_dInclination > pGeom->m_dInclination)
      return false;

    return m_dAzimuth < pGeom->m_dAzimuth;
  }

  if(dynamic_cast<const CFractureApertureElementGeometry*>(&rhs))
    return true; // smaller

  return false; // always larger
}

/////

CGeomecDianaRunnerBase::CFractureApertureElementGeometry::CFractureApertureElementGeometry(const geo::IVector& vecHighDensity, const geo::IVector& vecLowDensity)
: m_vecHighDensity(vecHighDensity),
  m_vecLowDensity(vecLowDensity)
{
}

bool CGeomecDianaRunnerBase::CFractureApertureElementGeometry::WriteFilos(IDianaRunner& /*diarunner*/) const
{
  ftn_double_t xaxis[3];
  xaxis[0] = m_vecHighDensity.X();
  xaxis[1] = m_vecHighDensity.Y();
  xaxis[2] = -m_vecHighDensity.Z();
  PutItemLength("XAXIS", xaxis, 3);

  ftn_double_t yaxis[3];
  yaxis[0] = m_vecLowDensity.X();
  yaxis[1] = m_vecLowDensity.Y();
  yaxis[2] = -m_vecLowDensity.Z();
  PutItemLength("YAXIS", yaxis, 3);

  return true;
}

int CGeomecDianaRunnerBase::CFractureApertureElementGeometry::Type() const
{
  return 200;
}

int CGeomecDianaRunnerBase::CFractureApertureElementGeometry::WriteFilosParamSize(IDianaRunner& /*diarunner*/) const
{
  return 6;
}

bool CGeomecDianaRunnerBase::CFractureApertureElementGeometry::WriteFilosParamName(IDianaRunner& /*diarunner*/, int i, char *name) const
{
  if (i < 3)
  {
    QString xaxis = QString("XAXIS(%1)").arg(i + 1);
    strncpy(name, xaxis.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  if (i < 3)
  {
    QString yaxis = QString("YAXIS(%1)").arg(i + 1);
    strncpy(name, yaxis.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  return false;
}

void CGeomecDianaRunnerBase::CFractureApertureElementGeometry::WriteFilosParamValues(IDianaRunner& /*diarunner*/, double *values, int stride) const
{
  *values = m_vecHighDensity.X(); // XAXIS(1)
  values += stride;

  *values = m_vecHighDensity.Y(); // XAXIS(2)
  values += stride;

  *values = -m_vecHighDensity.Z(); // XAXIS(3)
  values += stride;


  *values = m_vecLowDensity.X(); // YAXIS(1)
  values += stride;

  *values = m_vecLowDensity.Y(); // YAXIS(2)
  values += stride;

  *values = -m_vecLowDensity.Z(); // YAXIS(3)
  values += stride;
}

bool CGeomecDianaRunnerBase::CFractureApertureElementGeometry::operator<(const IElementProperty& rhs) const
{
  const CFractureApertureElementGeometry* pGeom = dynamic_cast<const CFractureApertureElementGeometry*>(&rhs);
  if(pGeom)
  {
    if(m_vecHighDensity < pGeom->m_vecHighDensity)
      return true;
    if(m_vecHighDensity > pGeom->m_vecHighDensity)
      return false;

    return m_vecLowDensity < pGeom->m_vecLowDensity;
  }

  return false; //always larger
}

// ***************************************************************************************

CGeomecDianaRunnerBase::CComboSurface::CComboSurface(const std::vector<const geo::ISurface*>& vcSurface)
{
	for(size_t i = 0; i < vcSurface.size(); i++)
	{
		const geo::ISurface& surface = *vcSurface[i];
		for(int j = 0; j < surface.FaceSize(); j++)
			AddFace(surface.Face(j));
	}
}

CGeomecDianaRunnerBase::CComboSurface::~CComboSurface()
{
	// Destruct
}

void CGeomecDianaRunnerBase::CComboSurface::AddFace(const geo::IFace &face) 
{
	for(int i = 0; i < face.NrOfPoints(); i++)
	{
		if(m_mpPointIndices.find(&face.Point(i)) == m_mpPointIndices.end())
		{
      m_mpPointIndices.insert(std::map<const geo::IPoint*, size_t>::value_type(&face.Point(i), m_vcPoints.size()));
			m_vcPoints.push_back(&face.Point(i));
		}
	}

	m_vcFaces.push_back(&face);
}

/*virtual*/ const geo::CPtrArray <geo::IFace> CGeomecDianaRunnerBase::CComboSurface::FacesAt(const geo::IPoint &/*p*/) const
{
	assert(false);
	geo::CPtrArray<geo::IFace> ret;
	return ret;
}

/*virtual*/ const geo::CPtrArray <geo::IFace> CGeomecDianaRunnerBase::CComboSurface::FacesAtNode(const geo::IPoint &/*p*/) const
{
	assert(false);
	geo::CPtrArray<geo::IFace> ret;
	return ret;
}

/*virtual*/ const geo::IFace &CGeomecDianaRunnerBase::CComboSurface::Face(int nIndex) const
{
	assert(nIndex > -1 && nIndex < m_vcFaces.size());

	return *m_vcFaces[nIndex];
}

/*virtual*/ int CGeomecDianaRunnerBase::CComboSurface::FaceSize() const
{
	return m_vcFaces.size();
}

/*virtual*/ const geo::IPoint& CGeomecDianaRunnerBase::CComboSurface::Point(int nIndex) const
{
	assert(nIndex > -1 && nIndex < m_vcPoints.size());
	return *m_vcPoints[nIndex];
}

int  CGeomecDianaRunnerBase::CComboSurface::PointSize() const
{
	return m_vcPoints.size();
}

/*virtual*/ geo::IPoint &CGeomecDianaRunnerBase::CComboSurface::PointAt(int /*nIndex*/)
{
	assert(false);
	geo::CPoint *p = new geo::CPoint();
	return *p;
}

/*virtual*/ const geo::IElement &CGeomecDianaRunnerBase::CComboSurface::Element(int nIndex) const
{
	return *m_vcFaces[nIndex];
}

/*virtual*/ int CGeomecDianaRunnerBase::CComboSurface::ElementSize() const
{
	return m_vcFaces.size();
}

/*virtual*/ std::vector<int> CGeomecDianaRunnerBase::CComboSurface::Nodes(const geo::IElement &/*element*/) const
{
	assert(false);
	std::vector<int> ret;
	return ret;
}
