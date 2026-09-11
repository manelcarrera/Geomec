
#include "GeomecHeatFlowDianaRunner.h"
#include "InterfaceElement.h"
#include "MeshBase.h"
#include "HorizonBase.h"
#include "DianaRunController.h"
#include "Temperature.h"
#include "TetraBoundary.h"
#include "BoundaryInterfaceMaterial.h"

#include "lbfl.h"
#include "lbcx.h"
#include "DepletionStage.h"
#include "DCFixedPotential.h"
#include "DCBoundaCase.h"
#include "DCNodalTemperature.h"
#include "FormationBase.h"
#include "DCTimeBoundaTable.h"

#include "GlobalConstants.h"

CGeomecHeatFlowDianaRunner::CGeomecHeatFlowDianaRunner(CMeshBase &meshbase,
						                                           CModelBase &model,
						                                           CDianaRunController& controller)
: CGeomecDianaRunnerBase(meshbase, model, controller)
{
}

bool CGeomecHeatFlowDianaRunner::AllowLinearCalc() const
{
  return false;
}

bool CGeomecHeatFlowDianaRunner::OnApplyLoads(dia::CLoadManager &manager)
{
  m_progress->StatusMessage("Writing loads...");

  TStageMap mpStages;
  ProcessStages(manager, mpStages);

  m_progress->StatusMessage("Writing loads... nodal temperatures");

  // nodal temperatures
  // first process the boundary nodes, skip the depleting formation nodes which override them later
  CreateNodalTemperatures(mpStages, m_mpBoundaryNodes, &m_mpDepletingFormationNodes);
  CreateNodalTemperatures(mpStages, m_mpDepletingFormationNodes, 0);

  m_progress->StatusMessage("Writing loads... time-boundary tables");

  // time-bounda table for each case
  CreateTimeBoundaTables(mpStages);

  WriteModelSpecificLoads(mpStages);

  return true;
}

bool CGeomecHeatFlowDianaRunner::OnWriteCommands()
{
  m_progress->StatusMessage("Writing commands...");

  if (!CGeomecDianaRunnerBase::OnWriteCommands())
    return false;

	PutCharItem("MODULE", "AP");
	PutCharItem("SEGMEN", "GM42");

	PushDir();

	ChangeDir("GEOMEC");
  SetActive("HX24L1", FTN_TRUE);

  WriteSolveCommands();

	if (!OnWriteElementOrder(Controller().WriteQuadDat()))
    return false;

  ChangeDir("HEAT");

  if(Controller().WriteOutputStreamFile())
  {
	  PushDir();
		  ChangeDir("OUTPUT");
		  PutCharItem("DEVICE","STREAM");
	  PopDir();
  }

  const CDepletionStage* pStage = &Model().InitialDepletionStage();
  const CDepletionStage* pNextStage = &pStage->Next();

  PushDir();
  ftn_int_t idx = 1;
  ChangeIndexedDir("STAGE/", &idx);
  ftn_double_t dtime = 0;
  PutItem("DTIME", &dtime);
  m_progress->Step();
  PopDir();

  while(pNextStage)
  {
    ++idx;

    PushDir();
    ChangeIndexedDir("STAGE/", &idx);
    WriteDepletionStageStepSizesAndTime(*pNextStage);
    PopDir();

    m_progress->Step();

    pStage = pNextStage;
    if(pNextStage->Last())
      pNextStage = 0;
    else
      pNextStage = &pNextStage->Next();
  }

  PopDir();
	MakeDefaultTree(".", 0);
  return true;
}

bool CGeomecHeatFlowDianaRunner::OnWriteSupports()
{
  m_progress->StatusMessage("Writing fixed potentials...");

  // heat flow boundary conditions (FIXTEM)
  assert(!m_stFixedNodes.empty());

  std::set<int>::iterator it;
  for(it = m_stFixedNodes.begin(); it != m_stFixedNodes.end(); ++it)
  {
    FixedPotentialNodeSet().InsertNode(Model().Mesh().Mesh().Node(*it));
    m_progress->Step();
  }

  return true;
}

bool CGeomecHeatFlowDianaRunner::OnWriteTyings()
{
	if(WriteFullModel())
  {
  	m_progress->StatusMessage("Writing temperature tyings");
    // we want unique tyings per node (structural tyings are one for each direction)
    typedef std::map<int, const geo::CTying*> TTyingMap; // from slave node to tying
    TTyingMap mpTyings;

    // supported nodes cannot be tied as well
    assert(!m_stFixedNodes.empty());

    int i;
    for(i = 0; i < Mesh().TyingSize(); ++i)
    {
      const geo::CTying& tying = Mesh().Tying(i);

      // only insert the tying when it's not a supported node
      if(m_stFixedNodes.find(tying.SlaveIndex()) == m_stFixedNodes.end())
        mpTyings.insert(TTyingMap::value_type(tying.SlaveIndex(), &tying));
    }
    TTyingMap::iterator it;
	  for(it = mpTyings.begin(), i = 0; it != mpTyings.end(); ++it, ++i)
	  {
		  const geo::CTying &tying = *it->second;
      WriteTying(tying, "TE", false);
		  m_progress->Step();
	  }
  }

  return true;
}

int CGeomecHeatFlowDianaRunner::LoadsProgressSize() const
{
  if(m_mpBoundaryNodes.empty() && m_mpDepletingFormationNodes.empty())
    FetchFixedPotentialNodes();

  int nStages = Model().NrOfDepletionStages();

  return m_mpBoundaryNodes.size() + m_mpDepletingFormationNodes.size() + nStages * nStages;
}

int CGeomecHeatFlowDianaRunner::CommandsProgressSize() const
{
  return Model().NrOfDepletionStages();
}

int CGeomecHeatFlowDianaRunner::SupportProgressSize() const
{
  if(m_mpBoundaryNodes.empty() && m_mpDepletingFormationNodes.empty())
    FetchFixedPotentialNodes();

  assert(m_stFixedNodes.empty());
  CModelBase::TNodeElementsMap::const_iterator it;

  for(it = m_mpBoundaryNodes.begin(); it != m_mpBoundaryNodes.end(); ++it)
    m_stFixedNodes.insert(it->first);

  for(it = m_mpDepletingFormationNodes.begin(); it != m_mpDepletingFormationNodes.end(); ++it)
    m_stFixedNodes.insert(it->first);

  return m_stFixedNodes.size();
}

int CGeomecHeatFlowDianaRunner::TyingsProgressSize() const
{
  if(!WriteFullModel())
    return 0;

  return CGeomecDianaRunnerBase::TyingsProgressSize();
}

const dia::IMaterial* CGeomecHeatFlowDianaRunner::InterfaceMaterial(const geo::IElement &element) const
{
	const geo::CInterfaceElement *pInterface = dynamic_cast<const geo::CInterfaceElement*> (&element);

	if(pInterface)
  {
		const CHorizonBase* pFault = MeshBase().SlipHorizon(element);
		if(pFault)
			return &pFault->InterfaceMaterial(element, Controller().StartStage());
    else
    {
      // boundary interfaces, must be a tetra model
	    const CTetraBoundary &bound = static_cast<const CTetraBoundary&>(Model().Boundary());
      if(bound.CreateInterfaces())
      {
        const geo::CInterfaceElement *pElemen = static_cast<const geo::CInterfaceElement*>(&element);
	      assert(pElemen->BackFace() == pElemen->FrontFace());
	      return &bound.InterfaceMaterial(*pElemen);
      }
    }
	}

	return 0;
}

void CGeomecHeatFlowDianaRunner::WriteModelSpecificLoads(const TStageMap& /*mpStages*/)
{
}

void CGeomecHeatFlowDianaRunner::FetchFixedPotentialNodes() const
{
  assert(m_mpBoundaryNodes.empty());
  assert(m_mpDepletingFormationNodes.empty());

  Model().CollectBoundaryNodes(m_mpBoundaryNodes);
  Model().CollectDepletingFormationNodes(m_mpDepletingFormationNodes, CAnalysisType::AT_HEAT);
}

void CGeomecHeatFlowDianaRunner::ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages)
{
	// the loadcase map must contain cases from the start to make sure the indexes are ok
	const CDepletionStage *pStage = &Model().InitialDepletionStage();
	bool bActive = false;

	while(pStage)
	{
		// switch on active stage from start stage
		if(pStage == &Controller().StartStage())
			bActive = true;

		// build up loadcase map
		dia::CBoundaCase &bcase = manager.NewBoundaCase();
		bcase.SetActive(bActive);

		VERIFY(mpStages.insert(std::make_pair(pStage, &bcase)).second);

		// stop when last stage or controller's end stage
		if(pStage->Last() || pStage == &Controller().EndStage())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

void CGeomecHeatFlowDianaRunner::CreateNodalTemperatures(
	const TStageMap& mpStages,
    CModelBase::TNodeElementsMap& mpNodeElements,
    CModelBase::TNodeElementsMap* pmpNodeElementsExclude)
{
  CModelBase::TNodeElementsMap::iterator it;
  for(it = mpNodeElements.begin(); it != mpNodeElements.end(); ++it)
  {
    // skip from excluded map (if not NULL)
    if(!pmpNodeElementsExclude || pmpNodeElementsExclude->find(it->first) == pmpNodeElementsExclude->end())
    {
      TStageMap::const_iterator its;
      for(its = mpStages.begin(); its != mpStages.end(); ++its)
      {
        const CDepletionStage& stage = *its->first;
        dia::CBoundaCase& bcase = *its->second;

        geo::CValue valTemp = GetAverageTemperatureInNode(it->first, it->second, stage);
        if(valTemp.Valid())
          new dia::CNodalTemperature(bcase, valTemp.Value() + 273.15, Model().Mesh().Mesh().Node(it->first));
      }
    }

    m_progress->Step();
  }
}

geo::CValue CGeomecHeatFlowDianaRunner::GetAverageTemperatureInNode(int iNode, const std::set<int>& stElements, const CDepletionStage& stage) const
{
  // get the averaged temperature values for this node (from all attached elements)
  geo::CValue value; // initially invalid
  std::set<int>::const_iterator it;
  int nDividor = 0;
  for(it = stElements.begin(); it != stElements.end(); ++it)
  {
    const geo::IElement& elm = Model().Mesh().Mesh().Element(*it);
    const CFormationBase* pFormation = Model().Mesh().Formation(elm);
    if(pFormation)
    {
      IValueDomainScalar::TValueVec vcTemp = Model().Mesh().Formation(elm)->UserTemperature(stage).Component().ScalarData().ValueElement(elm);
      int localidx = -1;
      int j;
      for(j = 0; j < elm.NrOfNodes(); ++j)
      {
        if(elm.Node(j).Index() == iNode)
        {
          localidx = j;
          break;
        }
      }
      if(localidx == -1)
      {
        // for 2D try location based comparison
        const geo::INode& node = Model().Mesh().Mesh().Node(iNode);
        for(j = 0; j < elm.NrOfNodes(); ++j)
        {
          if(elm.Node(j) == node)
          {
            localidx = j;
            break;
          }
        }
      }
      assert(localidx >= 0 && localidx < elm.NrOfNodes());
      assert(localidx < vcTemp.size());
      if(vcTemp[localidx].Valid())
      {
        if(value.Valid())
          value += vcTemp[localidx].Value();
        else
          value = vcTemp[localidx].Value();
        ++nDividor;
      }
    }
  }

  if(value.Valid())
  {
    assert(nDividor > 0);
    value /= nDividor; // average
  }

  return value;
}

void CGeomecHeatFlowDianaRunner::CreateTimeBoundaTables(const TStageMap& mpStages)
{
  // get the depletion stage (relative) times, in months
  std::vector<double> vcStageTimes;
  const CDepletionStage* pStage = &Model().InitialDepletionStage();
  const CDepletionStage& iniStage = *pStage;

  while(pStage)
  {
    double dDiffSecs = iniStage.Time().SecondsTo(pStage->Time());
    vcStageTimes.push_back(dDiffSecs / SECONDS_PER_MONTH);

    m_progress->Step();

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  pStage = &iniStage;

  while(pStage)
  {
    dia::CTimeBoundaTable* pTable = new dia::CTimeBoundaTable(*mpStages.find(pStage)->second);
    int idx = pStage->Index();
    int i;
    for(i = 0; i < vcStageTimes.size(); ++i)
    {
      pTable->Insert(vcStageTimes[i], (i == idx ? 1.0 : 0.0));
      m_progress->Step();
    }

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}
