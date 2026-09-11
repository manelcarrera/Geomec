
#include "GeomecMixtureDianaRunner.h"
#include "BoundaryInterfaceMaterial.h"
#include "DianaRunController.h"
#include "FaultPressure.h"
#include "FormationBase.h"
#include "HorizonBase.h"
#include "InterfaceElement.h"
#include "LineInterface.h"
#include "MeshBase.h"
#include "Pressure.h"
#include "TetraBoundary.h"

#include "BoundaryBase.h"
#include "DCLoadCase.h"
#include "DCPrescribedPressurePotential.h"
#include "DCTemperatureLoad.h"
#include "DCTimeLoadTable.h"
#include "DCWeightLoad.h"
#include "DepletionStage.h"
#include "ElementGroup.h"
#include "lbcx.h"
#include "lbfl.h"

#include "GlobalConstants.h"

CGeomecMixtureDianaRunner::CGeomecMixtureDianaRunner(CMeshBase &meshbase, CModelBase &model,
                                                     CDianaRunController &controller)
    : CGeomecDianaRunnerBase(meshbase, model, controller) {}

bool CGeomecMixtureDianaRunner::AllowLinearCalc() const { return false; }

bool CGeomecMixtureDianaRunner::OnApplyLoads(dia::CLoadManager &manager) {
  m_progress->StatusMessage("Applying loads");

  TStageMap mpStages;
  ProcessStages(manager, mpStages);

  TStageMap::iterator it1 = mpStages.find(&Model().InitialDepletionStage());
  assert(it1 != mpStages.end());
  dia::CLoadCase &lcase1 = *it1->second;

  if (lcase1.Active()) {
    if (!IsZoomInModel()) {
      // topload
      const CHorizonBase &top = Model().Boundary().GetTopHorizon();
      CreateTopLoad3D(top, lcase1);
    }

    // create the WEIGHT load
    new dia::CWeightLoad(lcase1, Model().Gravity().Value(), geo::CVector::Zaxis);
  }

  if (InitialStressesDefined())
    PerformInitialStressCalculation(false);

  int i;
  for (i = 0; i < Mesh().NodeSize(); ++i) {
    const geo::INode &node = Mesh().Node(i);
    int idx = node.Index();

    // priority: first faults, then reservoirs, finally boundary
    CModelBase::TNodeElementsMap::iterator itn;
    bool bValid = false;

    itn = m_mpDepletingFaultNodes.find(idx);
    if (itn != m_mpDepletingFaultNodes.end())
      bValid = true;

    if (!bValid) {
      itn = m_mpDepletingFormationNodes.find(idx);
      if (itn != m_mpDepletingFormationNodes.end())
        bValid = true;
    }

    if (!bValid) {
      itn = m_mpBoundaryNodes.find(idx);
      if (itn != m_mpBoundaryNodes.end())
        bValid = true;
    }

    if (bValid) {
      geo::CValue valInitialPressure = Pressure(node, itn->second, Model().InitialDepletionStage());
      if (valInitialPressure.Valid()) {
        double dInitialPressure = valInitialPressure.Value();
        assert(m_stFixedNodes.find(node.Index()) != m_stFixedNodes.end());

        for (TStageMap::iterator it = mpStages.begin(); it != mpStages.end(); ++it) {
          const CDepletionStage &stage = *it->first;
          dia::CLoadCase &lcase2 = *it->second;

          if (lcase2.Active()) {
            if (stage.Initial()) {
              if (fabs(dInitialPressure) > EPS)
                new dia::CPrescribedPressurePotential(node, lcase2, dInitialPressure * 1e6);
            } else {
              geo::CValue valPressure = Pressure(node, itn->second, stage);
              if (valPressure.Valid()) {
                double dDeltaPressure = valPressure.Value() - dInitialPressure;
                if (fabs(dDeltaPressure) > EPS)
                  new dia::CPrescribedPressurePotential(node, lcase2, dDeltaPressure * 1e6);
              }
            }
          }
        }
      }
    }

    m_progress->Step();
  }

  for (i = 0; i < Mesh().ElementSize(); ++i) {
    const geo::IElement &elm = Mesh().Element(i);
    IValueDomainScalar::TValueVec vcInitialTemperatures;
    const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement *>(&elm);
    if (!pIface) {
      const CFormationBase &formation = *MeshBase().Formation(elm);
      vcInitialTemperatures =
          formation.EffectiveTemperature(Model().InitialDepletionStage()).Component().ScalarData().ValueElement(elm);
    }

    for (TStageMap::iterator it = mpStages.begin(); it != mpStages.end(); ++it) {
      const CDepletionStage &stage = *it->first;
      dia::CLoadCase &lcase = *it->second;

      if (lcase.Active()) {
        if (stage.Initial()) {
          if (!pIface) {
            if (!PerformInitialStressCalculation()) {
              const CFormationBase &formation = *MeshBase().Formation(elm);
              IValueDomainScalar::TValueVec vcInitialPressures =
                  formation.Pressure(stage).Component().ScalarData().ValueElement(elm);
              if (!WriteZoomInInitialStresses(elm, lcase, vcInitialPressures))
                return false;
            }

            WriteElementInitialLoads(stage, elm, lcase, vcInitialTemperatures);
          }
        } else {
          CreateStrainLoad(elm, stage, lcase);

          if (!pIface)
            WriteElementStageLoads(stage, elm, lcase, vcInitialTemperatures);
        }
      }
    }

    m_progress->Step();
  }

  WriteModelSpecificLoads(mpStages);

  WriteBoundaryLoads(mpStages);

  return true;
}

bool CGeomecMixtureDianaRunner::OnWriteCommands() {
  m_progress->StatusMessage("Writing commands...");

  if (!CGeomecDianaRunnerBase::OnWriteCommands())
    return false;

  PutCharItem("MODULE", "AP");
  PutCharItem("SEGMEN", "GM42");

  PushDir();

  ChangeDir("GEOMEC");
  executeCommandInGeomec();

  WriteSolveCommands();

  if (!OnWriteElementOrder(Controller().WriteQuadDat()))
    return false;

  WriteConvergenceCommands();

  ChangeDir("MIXTURE");

  if (PerformInitialStressCalculation()) {
    /*ftn_bool_t zoomin =*/PerformInitialStressCalculation() /*? FTN_FALSE : FTN_TRUE*/;
    SetActive("ZOOMIN", FTN_FALSE);

    PushDir();

    // initial stress
    ChangeDir("INISTR");

    PushDir();

    ChangeDir("ITERAT");
    ftn_int_t maxite = Model().IniStressMaxIterations();
    PutItem("MAXITE", &maxite);

    ChangeDir("CONVER");
    ChangeDir("FORCE");
    ftn_double_t tolcon = Model().IniStressConvCriterion();
    PutItem("TOLCON", &tolcon);

    PopDir();

    PopDir();
  } else
    SetActive("ZOOMIN", FTN_TRUE);

  if (Controller().WriteOutputStreamFile()) {
    PushDir();
    ChangeDir("OUTPUT");
    PutCharItem("DEVICE", "STREAM");
    PopDir();
  }

  ftn_bool_t antime = FTN_TRUE;
  PutItem("ANTIME", &antime);

  if (XistIndexed("STAGE", 0))
    RemoveIndexedItem("STAGE", 0);

  assert(!XistIndexed("STAGE", 0));

  const CDepletionStage *pStage = &Model().InitialDepletionStage();
  const CDepletionStage *pNextStage = &pStage->Next();

  if (!pStage->Initial()) {
    ftn_double_t ttime = (ftn_double_t)((Model().InitialDepletionStage().Time().SecondsTo(pStage->Previous().Time())) /
                                        SECONDS_PER_MONTH);
    PutItem("TTIME", &ttime);
  }

  PushDir();
  ftn_int_t idx = 1;
  ChangeIndexedDir("STAGE/", &idx);
  ftn_double_t dtime = 0;
  PutItem("DTIME", &dtime);
  WriteNonlinDepletionStageParameters(pStage, false);
  m_progress->Step();
  PopDir();

  while (pNextStage) {
    ++idx;

    PushDir();

    ChangeIndexedDir("STAGE/", &idx);
    if (!pNextStage->Initial())
      WriteDepletionStageStepSizesAndTime(*pNextStage);

    WriteNonlinDepletionStageParameters(pNextStage, false);
    m_progress->Step();

    PopDir();

    pStage = pNextStage;
    if (pNextStage->Last())
      pNextStage = 0;
    else
      pNextStage = &pNextStage->Next();
  }

  PopDir();
  MakeDefaultTree(".", 0);
  return true;
}

bool CGeomecMixtureDianaRunner::OnWriteSupports() {
  if (WriteFullModel()) {
    CGeomecDianaRunnerBase::OnWriteSupports();

    m_progress->StatusMessage("Writing pressure supports...");

    // boundary conditions
    assert(!m_stFixedNodes.empty());

    std::set<int>::iterator it;
    for (it = m_stFixedNodes.begin(); it != m_stFixedNodes.end(); ++it) {
      // use empty vector for direction
      WriteSupport(*it, "PR", geo::CVector());

      m_progress->Step();
    }
  }

  return true;
}

bool CGeomecMixtureDianaRunner::OnWriteTyings() {
  if (WriteFullModel()) {
    // let the base class write the structural tyings
    if (!CGeomecDianaRunnerBase::OnWriteTyings())
      return false;

    m_progress->StatusMessage("Writing pressure tyings");
    std::set<int> stTiedSlaves;
    for (int i = 0; i < Mesh().TyingSize(); i++) {
      const geo::CTying &tying = Mesh().Tying(i);

      if (m_stFixedNodes.find(tying.SlaveIndex()) == m_stFixedNodes.end() &&
          stTiedSlaves.find(tying.SlaveIndex()) == stTiedSlaves.end()) {
        WriteTying(tying, "PR", false);
        stTiedSlaves.insert(tying.SlaveIndex());
      }
      m_progress->Step();
    }
  }

  return true;
}

int CGeomecMixtureDianaRunner::LoadsProgressSize() const { return Mesh().NodeSize() + Mesh().ElementSize(); }

int CGeomecMixtureDianaRunner::CommandsProgressSize() const { return Model().NrOfDepletionStages(); }

int CGeomecMixtureDianaRunner::SupportProgressSize() const {
  if (!WriteFullModel())
    return 0;

  if (MeshBase().getGenerateSupports()) {
    MeshBase().GenerateSupports();
  }

  if (m_mpBoundaryNodes.empty() && m_mpDepletingFormationNodes.empty())
    FetchFixedPotentialNodes();

  assert(m_stFixedNodes.empty());
  CModelBase::TNodeElementsMap::const_iterator it;

  for (it = m_mpBoundaryNodes.begin(); it != m_mpBoundaryNodes.end(); ++it)
    m_stFixedNodes.insert(it->first);

  for (it = m_mpDepletingFormationNodes.begin(); it != m_mpDepletingFormationNodes.end(); ++it)
    m_stFixedNodes.insert(it->first);

  for (it = m_mpDepletingFaultNodes.begin(); it != m_mpDepletingFaultNodes.end(); ++it)
    m_stFixedNodes.insert(it->first);

  return m_stFixedNodes.size() + CGeomecDianaRunnerBase::SupportProgressSize();
}

int CGeomecMixtureDianaRunner::TyingsProgressSize() const {
  if (!WriteFullModel())
    return 0;

  return CGeomecDianaRunnerBase::TyingsProgressSize();
}

const dia::IMaterial *CGeomecMixtureDianaRunner::InterfaceMaterial(const geo::IElement &element) const {
  if (IsInterfaceElement(element)) {
    const CHorizonBase *pFault = Model().Mesh().SlipHorizon(element);
    if (pFault)
      return &pFault->InterfaceMaterial(element, Controller().StartStage());
    else {
      // boundary interfaces
      assert(dynamic_cast<const CInterfaceBoundary *>(&Model().Boundary()));
      const CInterfaceBoundary &bound = static_cast<const CInterfaceBoundary &>(Model().Boundary());
      if (bound.CreateInterfaces()) {
        const geo::CInterfaceElement *pElemen = static_cast<const geo::CInterfaceElement *>(&element);
        assert(pElemen->BackFace() == pElemen->FrontFace());
        return &bound.InterfaceMaterial(*pElemen);
      }
    }
  }

  return 0;
}

void CGeomecMixtureDianaRunner::ElementPressures(const geo::IElement &elm, const CDepletionStage &stage,
                                                 IValueDomainScalar::TValueVec &vcValues) const {
  if (IsInterfaceElement(elm)) {
    const CHorizonBase *pFault = Model().Mesh().SlipHorizon(elm);
    assert(pFault != 0);
    assert(pFault->Slip());
    vcValues = pFault->Pressure(stage).Component().ScalarData().ValueElement(elm);
  } else {
    const CFormationBase &formation = *Model().Mesh().Formation(elm);
    vcValues = formation.Pressure(stage).Component().ScalarData().ValueElement(elm);
  }
}

void CGeomecMixtureDianaRunner::WriteModelSpecificLoads(const TStageMap & /*mpStages*/) {}

void CGeomecMixtureDianaRunner::ModifyMaterialForContainment(const dia::IMaterial &mat,
                                                             const geo::IElement &element) const {
  const CFFMaterial *pffmat = dynamic_cast<const CFFMaterial *>(&mat);
  if (pffmat) {
    const CFormationBase *pFormation = MeshBase().Formation(element);
    if (pFormation && !pFormation->Depleting()) {
      // need to modify, get rid of the const
      CFFMaterial &fm = const_cast<CFFMaterial &>(*pffmat);

      if (fm.IsParameter(IDT_VALUETYPE_PERMEA))
        fm.SetParameterValue(IDT_VALUETYPE_PERMEA, 1000);

      if (fm.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS) && fm.IsParameter(IDT_VALUETYPE_BULKSTIFFNESS))
        fm.SetParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS, 1.02 * fm.ParameterValue(IDT_VALUETYPE_BULKSTIFFNESS));

      if (fm.IsParameter(IDT_VALUETYPE_POROSITY))
        fm.SetParameterValue(IDT_VALUETYPE_POROSITY, 0.01);
    }
  }
}

void CGeomecMixtureDianaRunner::FetchFixedPotentialNodes() const {
  assert(m_mpBoundaryNodes.empty());
  assert(m_mpDepletingFormationNodes.empty());
  assert(m_mpDepletingFaultNodes.empty());

  Model().CollectBoundaryNodes(m_mpBoundaryNodes);
  Model().CollectDepletingFormationNodes(m_mpDepletingFormationNodes, CAnalysisType::AT_MIXTURE);

  const THorizonBaseEntry &entry = (const THorizonBaseEntry &)*Model().GraphEntry(MD_BASE_HORIZON);
  const THorizonBaseEntry::TNodeSet &stNodes = entry.EntryNodes();
  THorizonBaseEntry::TNodeSet::const_iterator it;
  for (it = stNodes.begin(); it != stNodes.end(); ++it) {
    const CHorizonBase &horizon = **it;
    if (horizon.Slip()) {
      bool bUserPressure = false;

      const CDepletionStage *pStage = &Model().InitialDepletionStage();
      while (pStage) {
        const CFaultPressure &faultpressure = horizon.Pressure(*pStage);

        if (faultpressure.Mode() == CFaultPressure::MT_GRADIENT || faultpressure.DistributedSize() > 0) {
          bUserPressure = true;
          break;
        }

        if (pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }

      if (bUserPressure) {
        // the fault is part of the flow boundary, add the nodes
        const geo::CElementGroup *pGroup = horizon.InterfaceElementGroup();
        assert(pGroup != 0);

        int i;
        for (i = 0; i < pGroup->ElementSize(); ++i) {
          const geo::IElement &elm = pGroup->Element(i);
          int n;
          for (n = 0; n < elm.NrOfNodes(); ++n) {
            CModelBase::TNodeElementsMap::iterator ite =
                m_mpDepletingFaultNodes
                    .insert(CModelBase::TNodeElementsMap::value_type(elm.Node(n).Index(), std::set<int>()))
                    .first;
            ite->second.insert(elm.Index());
          }
        }
      }
    }
  }
}

void CGeomecMixtureDianaRunner::ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages) {
  // the loadcase map must contain cases from the start to make sure the indexes are ok
  const CDepletionStage *pStage = &Model().InitialDepletionStage();
  bool bActive = false;

  while (pStage) {
    // switch on active stage from start stage
    if (pStage == &Controller().StartStage())
      bActive = true;

    // build up loadcase map
    dia::CLoadCase &lcase = manager.NewLoadCase();
    lcase.SetActive(bActive);

    VERIFY(mpStages.insert(std::make_pair(pStage, &lcase)).second);

    // stop when last stage or controller's end stage
    if (pStage->Last() || pStage == &Controller().EndStage())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CGeomecMixtureDianaRunner::CreateTimeLoadTables(const TStageMap &mpStages) {
  // get the depletion stage (relative) times, in months
  std::vector<double> vcStageTimes;
  const CDepletionStage *pStage = &Model().InitialDepletionStage();
  const CDepletionStage &iniStage = *pStage;

  while (pStage) {
    double dDiffSecs = iniStage.Time().SecondsTo(pStage->Time());
    vcStageTimes.push_back(dDiffSecs / SECONDS_PER_MONTH);

    m_progress->Step();

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  pStage = &iniStage;

  // skip initial stage (no table needed)
  if (pStage)
    pStage = &pStage->Next();

  while (pStage) {
    dia::CTimeLoadTable *pTable = new dia::CTimeLoadTable(*mpStages.find(pStage)->second);
    int idx = pStage->Index();
    int i;
    for (i = 0; i < vcStageTimes.size(); ++i) {
      pTable->Insert(vcStageTimes[i], (i == idx ? 1.0 : 0.0));
      m_progress->Step();
    }

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

geo::CValue CGeomecMixtureDianaRunner::Pressure(const geo::INode &node, const std::set<int> &stAttachedElements,
                                                const CDepletionStage &stage) const {
  std::set<int>::const_iterator it;
  double dValue = 0;
  int iValues = 0;
  for (it = stAttachedElements.begin(); it != stAttachedElements.end(); ++it) {
    const geo::IElement &elm = Model().Mesh().Mesh().Element(*it);
    IValueDomainScalar::TValueVec vcValues;
    ElementPressures(elm, stage, vcValues);

    assert(vcValues.size() == elm.NrOfNodes());

    int i;
    bool bFound = false;
    for (i = 0; i < elm.NrOfNodes(); ++i) {
      if (&elm.Node(i) == &node) {
        if (vcValues[i].Valid()) {
          dValue += vcValues[i].Value();
          ++iValues;
        }
        bFound = true;
        break;
      }
    }
    if (!bFound) {
      // do a position-based comparison, happens for some 2D fault elements due to the attached 'surrogate' elements
      for (i = 0; i < elm.NrOfNodes(); ++i) {
        if (elm.Node(i) == node) {
          if (vcValues[i].Valid()) {
            dValue += vcValues[i].Value();
            ++iValues;
          }
          bFound = true;
          break;
        }
      }
    }

    assert(bFound);
  }

  if (iValues > 0) {
    dValue /= iValues;
    return geo::CValue(dValue);
  }

  return geo::CValue();
}

bool CGeomecMixtureDianaRunner::IsInterfaceElement(const geo::IElement &elm) const {
  return (dynamic_cast<const geo::CInterfaceElement *>(&elm) != 0 ||
          dynamic_cast<const geo::CLineInterface *>(&elm) != 0);
}

void CGeomecMixtureDianaRunner::WriteElementInitialLoads(const CDepletionStage & /*stage*/,
                                                         const geo::IElement &element, dia::CLoadCase &lcase,
                                                         const IValueDomainScalar::TValueVec &vcInitialTemperatures) {
  bool bWriteMultiple = false;

  std::vector<double> vcTemperatures(vcInitialTemperatures.size());
  for (size_t j = 0; j < vcInitialTemperatures.size(); ++j) {
    vcTemperatures[j] = vcInitialTemperatures[j].Value() + 273.15; // convert to Kelvin
    if (j && fabs(vcTemperatures[j] - vcTemperatures[j - 1]) > MIN_TEMPER_LOAD_VAL)
      bWriteMultiple = true;
  }

  if (bWriteMultiple)
    new dia::CTemperatureLoad(lcase, vcTemperatures, element);
  else
    new dia::CTemperatureLoad(lcase, vcTemperatures[0], element);
}

void CGeomecMixtureDianaRunner::FetchStageTemperatures(const geo::IElement &element, const CDepletionStage &stage,
                                                       IValueDomainScalar::TValueVec &vcTemperatures) {
  const CFormationBase &formation = *MeshBase().Formation(element);

  // depletion stage pressures
  vcTemperatures = formation.EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
}

void CGeomecMixtureDianaRunner::WriteElementStageLoads(const CDepletionStage &stage, const geo::IElement &element,
                                                       dia::CLoadCase &lcase,
                                                       const IValueDomainScalar::TValueVec &vcInitialTemperatures) {
  // depletion stage pressures
  IValueDomainScalar::TValueVec vcValues;

  bool bWriteThem = false;
  bool bWriteMultiple = false;

  FetchStageTemperatures(element, stage, vcValues);

  std::vector<double> vcTemperatures(vcValues.size());
  for (size_t j = 0; j < vcValues.size(); ++j) {
    vcTemperatures[j] = vcValues[j].Value() - vcInitialTemperatures[j].Value();
    if (fabs(vcTemperatures[j]) > MIN_TEMPER_LOAD_VAL)
      bWriteThem = true;
    if (j && fabs(vcTemperatures[j] - vcTemperatures[j - 1]) > MIN_TEMPER_LOAD_VAL)
      bWriteMultiple = true;
  }

  if (bWriteThem) {
    if (bWriteMultiple)
      new dia::CTemperatureLoad(lcase, vcTemperatures, element);
    else
      new dia::CTemperatureLoad(lcase, vcTemperatures[0], element);
  }
}
