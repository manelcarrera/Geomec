
#include <QFile>

#include "GeomecCompactionDianaRunner.h"
#include "MeshBase.h"
#include "DianaRunController.h"
#include "Pressure.h"
#include "LineInterface.h"
#include "GeomecUtils.h"

#include "lbfl.h"
#include "lbcx.h"
#include "InterfaceElement.h"
#include "GlobalConstants.h"
#include "DepletionStage.h"
#include "IPlatform.h"
#include "DCLoadCase.h"
#include "BranchState.h"
#include "BoundaryBase.h"
#include "FormationBase.h"
#include "DCPressureLoad.h"
#include "DCTemperatureLoad.h"
#include "HorizonBase.h"
#include "FaultParametersNode.h"

CGeomecCompactionDianaRunner::CGeomecCompactionDianaRunner(CMeshBase &meshbase,
                                                           CModelBase &model,
                                                           CDianaRunController& controller)
: CGeomecDianaRunnerBase(meshbase, model, controller)
{
}

bool CGeomecCompactionDianaRunner::OnlyNewLoads(const CDepletionStage& startstage) const
{
  if(startstage.Initial())
  return false;

  // check that there are no materials connected to formations for this stage
  TFormationBaseEntry& form_entry = (TFormationBaseEntry&)*Model().GraphEntry(MD_BASE_FORMATION);
  const TFormationBaseEntry::TNodeSet& stFormNodes = form_entry.EntryNodes();
  for(TFormationBaseEntry::TNodeSet::const_iterator it = stFormNodes.begin(); it != stFormNodes.end(); ++it)
  {
  const CFormationBase& form = **it;
  assert(form.HasMaterial(startstage)); // must be a branch start stage
  const CMaterialServer* pMatServer = form.ConnectedMaterial(startstage);
  assert(pMatServer != 0);
  if(pMatServer->LibraryMaterial() != 0)
      return false; // this formation has a new material defined for this stage
  }

  // check that there are no materials defined for faults for this stage
  THorizonBaseEntry& hor_entry = (THorizonBaseEntry&)*Model().GraphEntry(MD_BASE_HORIZON);
  const THorizonBaseEntry::TNodeSet& stHorNodes = hor_entry.EntryNodes();
  for(THorizonBaseEntry::TNodeSet::const_iterator it = stHorNodes.begin(); it != stHorNodes.end(); ++it)
  {
  const CHorizonBase& hor = **it;
  if(hor.Slip() && hor.SlipType() == CHorizonBase::USER)
  {
      assert(hor.hasFaultParameters(startstage));
      const CFaultParametersNode& params = hor.FaultParameters(startstage);
      if(!params.CohesionSameAsPrevious() ||
         !params.FrictionSameAsPrevious() ||
         !params.DStiffSameAsPrevious()   ||
          params.DistributedCohesion()    ||
          params.DistributedFriction())
      {
    return false; // this fault has a new material defined for this stage
      }
  }
  }

  return true;
}

void CGeomecCompactionDianaRunner::WriteNonlinCommands(const CDepletionStage& startstage, const CDepletionStage& endstage, int iStartS)
{
  const CModelBase *pModel = dynamic_cast<const CModelBase *> (&MeshBase().Model());

  if(pModel->LargeDeformations()) PutCharItem("GEOTYP", "UPDATE");

  WriteConvergenceCommands();

  PushDir();

    ChangeDir("NONLIN");

  SetActive("CLNRES", (Controller().CleanupOldResults(iStartS) ? FTN_TRUE : FTN_FALSE));

  if(PerformInitialStressCalculation())
  {
      /*ftn_bool_t zoomin =*/ PerformInitialStressCalculation() /*? FTN_FALSE : FTN_TRUE*/;
      SetActive("ZOOMIN", Model().DefaultDianaZoominSetting() ? FTN_TRUE : FTN_FALSE);

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
  }
  else
      SetActive("ZOOMIN", FTN_TRUE);

    ftn_bool_t antime = FTN_TRUE;
    PutItem("ANTIME", &antime);

  // fault initialisation
  PutCharItem("FAULT", FaultInitCommand(Model().FaultInit()).toStdString().c_str());

    if(XistIndexed("STAGE", 0))
      RemoveIndexedItem("STAGE", 0);

    assert(!XistIndexed("STAGE", 0));
    const CDepletionStage *pStage = &startstage;

    if(!pStage->Initial())
    {
      ftn_double_t ttime = (ftn_double_t)((pModel->InitialDepletionStage().Time().SecondsTo(pStage->Previous().Time())) / SECONDS_PER_MONTH);
      PutItem("TTIME", &ttime);
    }

    PushDir();

      while(pStage)
      {
        ftn_int_t idxstage = (ftn_int_t)(pStage->Index() + 1);

        PushDir();
          assert(!XistIndexed("STAGE/", &idxstage));
          ChangeIndexedDir("STAGE/", &idxstage);
          WriteNonlinDepletionStageParameters(pStage, true);
        PopDir();

    if(Controller().WriteOutputStreamFile())
    {
          PushDir();
            ChangeIndexedDir("OUTPUT/", &idxstage);
            PutCharItem("DEVICE","STREAM");
          PopDir();
    }

        if(pStage->Last() || pStage == &endstage)
          pStage = 0;
        else
          pStage = &pStage->Next();
      }

      if(iStartS >= 0)
      {
    ftn_int_t starts = ftn_int_t(iStartS + 1);
    PutItem("STARTS", &starts);

    const CDepletionStage& sstage = Model().DepletionStageEntry().StageByIndex(iStartS);
    if(OnlyNewLoads(sstage))
          SetActive("NWLOON", FTN_TRUE);

      }

    PopDir();

  PopDir();
}

int CGeomecCompactionDianaRunner::CommandsProgressSize() const
{
  return 1;
}

int CGeomecCompactionDianaRunner::TyingsProgressSize() const
{
  if(WriteFullModel())
    return CDianaRunner::TyingsProgressSize();

  return 0;
}

bool CGeomecCompactionDianaRunner::OnEndPostprocessWriteInputFiles(const std::string& sComfileName, const std::string& sDatfileName)
{
  if(!CGeomecDianaRunnerBase::OnEndPostprocessWriteInputFiles(sComfileName, sDatfileName))
  return false;

  if(Controller().AnalysisType().AnalysisType() == CAnalysisType::AT_NONLIN && (Controller().WriteInputFiles() || Controller().WriteQuadDat()))
  {
  // insert FILOS copy commands into the commands file
  FILE* fpin;
  FILE* fpout;

  bool bCopied = false;

  fpin = fopen(sComfileName.c_str(), "r");
  if(fpin)
  {
      char* tmpcomfile = vDiStrsave(GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION).toStdString().c_str(), "\\geomeccomXXXXXX", 0);
      mktemp(tmpcomfile);
      fpout = fopen(tmpcomfile, "w");
      if(fpout)
      {
    const CDepletionStage* pStage = &Model().InitialDepletionStage();
    bool bFirstInputSeen = false;
    char buf[1024];
    while(fgets(buf, 1024, fpin))
    {
          if(strncmp(buf, "*INPUT", 6) == 0)
          {
      if(bFirstInputSeen)
      {
              // insert Filos copy command if this is a branch
              if(pStage->OutputType() == CDepletionStage::BRANCH)
              {
        assert(!pStage->Initial());
        std::string sFFBranchName = pStage->ModifiedFileName(sComfileName);

        // replace file extension
        std::string::size_type index = sFFBranchName.rfind('.');
        if(index != std::string::npos)
        {
                  int len = sFFBranchName.length() - index;
                  sFFBranchName.replace(index, len, ".ff");
        }

        // remove path
        index = sFFBranchName.rfind('\\');
        if(index == std::string::npos)
                  index = sFFBranchName.rfind('/');
        if(index != std::string::npos)
                  sFFBranchName = sFFBranchName.substr(index + 1);

        fputs("*FILOS\n", fpout);
        fprintf(fpout, " COPY FILE=\"%s\"\n", sFFBranchName.c_str());
              }
      }
      else
      {
              bFirstInputSeen = true;
      }

      if(!pStage->Last())
              pStage = &pStage->GetNextBranchStage(pStage->Next());
          }

          fputs(buf, fpout);
    }

    bCopied = true;

    fclose(fpout);
      }
      else
      {
    IPlatform* platform = IPlatform::instance();
    QString traceString = QString("Could not open tempfile %1\n").arg(tmpcomfile);

    platform->trace(traceString);
      }

      fclose(fpin);

      if(bCopied)
      {
    QFile::remove(sComfileName.c_str());
    QFile::copy(tmpcomfile, sComfileName.c_str());
    unlink(tmpcomfile);
      }

      DiFree(tmpcomfile, "CGeomecCompactionDianaRunner::OnEndPostprocessWriteInputFiles");
  }
  else
  {
      IPlatform* platform = IPlatform::instance();
      QString traceString = QString("Could not open %1 for modification\n").arg(sComfileName.c_str());

      platform->trace(traceString);
  }
  }

  return true;
}

bool CGeomecCompactionDianaRunner::OnWriteTyings()
{
  if(WriteFullModel())
    return CDianaRunner::OnWriteTyings();

  return true;
}

bool CGeomecCompactionDianaRunner::OnWriteSupports()
{
  if(WriteFullModel())
    return CDianaRunner::OnWriteSupports();

  return true;
}

bool CGeomecCompactionDianaRunner::OnWriteCommands()
{
  bool bRet;
  
  if (!CGeomecDianaRunnerBase::OnWriteCommands())
  return false;

  if(Controller().AnalysisType().AnalysisType() == CAnalysisType::AT_NONLIN && (Controller().WriteInputFiles() || Controller().WriteQuadDat()))
  bRet = Controller().WriteCommands();
  else
  bRet = DoWriteCommands(Controller().StartStage().InitialStage(), Controller().EndStage(), Controller().StartStage().Index());

  m_progress->Step();

  return bRet;
}

bool CGeomecCompactionDianaRunner::DoWriteCommands(
  const CDepletionStage& startstage, 
  const CDepletionStage& endstage, 
  int iStartS)
{
  m_progress->StatusMessage("Writing commands");

  PutCharItem("MODULE", "AP");
  PutCharItem("SEGMEN", "GM42");

  PushDir();

  ChangeDir("GEOMEC");
  executeCommandInGeomec();
  const CModelBase *pModel = static_cast<const CModelBase *> (&MeshBase().Model());

  bool bSaltStressInit = pModel->SaltInitRelax();
  ftn_bool_t SSI = bSaltStressInit ? FTN_TRUE : FTN_FALSE;

  if(Controller().WriteQuadDat())
  {
  if (!OnWriteElementOrder(true))
      return false;
  }
  else
  {
  if (!OnWriteElementOrder(false))
      return false;

    switch(Controller().AnalysisType().AnalysisType())
    {
  case CAnalysisType::AT_LINEAR:
      {
    WriteSolveCommands();
        // back in /cmnds/seginf/geomec

        PushDir();

          ChangeDir("LINSTA");

          // write depletion stage times
          int nStages = Model().DepletionStageEntry().EntryNodes().size();
          ftn_double_t* pTimes = (ftn_double_t*)DiMalloc(nStages * sizeof(ftn_double_t), "CGeomecCompactionDianaRunner::DoWriteCommands");
          const CDepletionStage& inistage = Model().InitialDepletionStage();
          CDepletionStageEntry::const_iterator it;
          int i;
          for(it = Model().DepletionStageEntry().begin(), i = 0; it != Model().DepletionStageEntry().end(); ++it, ++i)
          {
      const CDepletionStage& stage = *it;
      assert(i < nStages);
      pTimes[i] = inistage.Time().SecondsTo(stage.Time()) / SECONDS_PER_MONTH;
          }

          PutItemLength("TIMES", pTimes, nStages);
          DiFree(pTimes, "CGeomecCompactionDianaRunner::DoWriteCommands");

          if(PerformInitialStressCalculation())
          {
            SetActive("ZOOMIN", Model().DefaultDianaZoominSetting() ? FTN_TRUE : FTN_FALSE);

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
              // back in /cmnds/seginf/geomec/linsta/inistr

              ChangeDir("RESULT");

            PopDir();
            // back in /cmnds/seginf/geomec/linsta
          }
          else
          {
            SetActive("ZOOMIN", FTN_TRUE);
          }

          // see if we need to do full analysis (not just initial stresses)
          if(!Model().LinstaOnlyInitialStress())
          {
            PushDir();
            ChangeDir("STAGES");
            ChangeDir("RESULT");
            PopDir();
          }

          if(Controller().WriteOutputStreamFile())
          {
            PushDir();
              ChangeDir( "OUTPUT" );
              PutCharItem("DEVICE","STREAM");
            PopDir();
            // back in /cmnds/seginf/geomec/linsta
          }

        PopDir();
        // back in /cmnds/seginf/geomec

      }
      break;
  case CAnalysisType::AT_NONLIN:
      {
        WriteNonlinCommands(startstage, endstage, iStartS);
    WriteSolveCommands();
      }
      break;
    default:
      assert(FALSE);
    }
  }

  PutItem("SALTFL", &SSI);

  PopDir();

  MakeDefaultTree(".", 0);

  return true;
}

QString CGeomecCompactionDianaRunner::FaultInitCommand(CModelBase::TFaultInit nFaultInit)
{
  switch(nFaultInit)
  {
  case CModelBase::FI_MOD_COHESION:
  return "STKCOH";
  case CModelBase::FI_MOD_FLUIDPRESSURE:
  return "STKPRS";
  case CModelBase::FI_FULLSLIP:
  break;
  }

  assert(nFaultInit == CModelBase::FI_FULLSLIP);
  return "SLIP";
}

int CGeomecCompactionDianaRunner::SupportProgressSize() const
{
  // generate the supports now
  // also generate the supports when the mesh is not actually written
  // to the database (!WriteFullModel()), because we need them
  // with prescribed displacement loads

  if (MeshBase().getGenerateSupports())
  {
  MeshBase().GenerateSupports();
  }

  if(WriteFullModel())
  {
    return CDianaRunner::SupportProgressSize();
  }

  return 0;
}

bool CGeomecCompactionDianaRunner::ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages)
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
    dia::CLoadCase &lcase = manager.NewLoadCase();
    lcase.SetActive(bActive);

    VERIFY(mpStages.insert(std::make_pair(pStage, &lcase)).second);

    // stop when last stage or controller's end stage
    if(pStage->Last() || pStage == &Controller().EndStage())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  return true;
}

bool CGeomecCompactionDianaRunner::OnModifyInputFileNames(std::string& sComfileName, std::string& sDatfileName) const
{
  return Controller().OnModifyInputFileNames(sComfileName, sDatfileName);
}

std::string CGeomecCompactionDianaRunner::InitialComFileText() const
{
  if(Controller().AnalysisType().AnalysisType() == CAnalysisType::AT_NONLIN && (Controller().WriteInputFiles() || Controller().WriteQuadDat()))
  {
  if(&Model().BranchState().ActiveStage() == &Model().BranchState().ActiveStage().InitialStage())
      return "*FILOS\n INITIA\n";
  return "";
  }

  return CGeomecDianaRunnerBase::InitialComFileText();
}

bool CGeomecCompactionDianaRunner::OnApplyLoads(dia::CLoadManager &manager)
{
  TStageMap mpStages;

  m_progress->StatusMessage("Writing loads");

  if (!ProcessStages(manager, mpStages))
    return false;

  TStageMap::iterator it1 = mpStages.find(&Model().InitialDepletionStage());
  assert(it1 != mpStages.end());
  dia::CLoadCase& lcase1 = *it1->second;

  if(lcase1.Active())
  {
    // we shouldn't write a top load for zoom in models
    if(!IsZoomInModel())
  {
      // topload
      const CHorizonBase &top = Model().Boundary().GetTopHorizon();
      CreateTopLoad(top, lcase1);
  }

    // weight load
    CreateWeightLoad(lcase1, Model().Gravity().Value());
  }

  if(InitialStressesDefined())
    PerformInitialStressCalculation(false);

  for(int i = 0; i < Mesh().ElementSize(); i++)
  {
    const geo::IElement &element = Mesh().Element(i);
    IValueDomainScalar::TValueVec vcInitialPressures;
    IValueDomainScalar::TValueVec vcInitialTemperatures;

    FetchInitialPressures(element, Model().InitialDepletionStage(), vcInitialPressures);
  FetchInitialTemperatures(element, Model().InitialDepletionStage(), vcInitialTemperatures);

    // loop won't be in depletion stage's order, but that doesn't matter, loadcases
    //  have been defined in order

    for(TStageMap::iterator it2 = mpStages.begin(); it2 != mpStages.end(); ++it2)
    {
      const CDepletionStage& stage = *it2->first;
      dia::CLoadCase &lcase2 = *it2->second;

      if(lcase2.Active())
      {
        if(stage.Initial())
        {
          if (!CreateInitialLoads(element, stage, lcase2, vcInitialPressures, vcInitialTemperatures))
      return false;
        }
        else
        {
          if (!CreateStageLoads(element, stage, lcase2, vcInitialPressures, vcInitialTemperatures))
      return false;
          CreateStrainLoad(element, stage, lcase2);
        }
      }
    }
  }

  if (!WriteBoundaryLoads(mpStages))
    return false;

  return true;
}

bool CGeomecCompactionDianaRunner::FetchInitialTemperatures(const geo::IElement& element, const CDepletionStage& /*stage*/, IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  if(dynamic_cast<const geo::CInterfaceElement*>(&element) == 0 && dynamic_cast<const geo::CLineInterface*>(&element) == 0)
    FetchElementInitialTemperatures(Model().InitialDepletionStage(), element, vcInitialTemperatures);

  return true;
}

void CGeomecCompactionDianaRunner::FetchElementInitialTemperatures(const CDepletionStage& stage, const geo::IElement& element, IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  // lookup formation ...
  const CFormationBase& formation = *MeshBase().Formation(element);

  // ... and get the temperatures
  vcInitialTemperatures = formation.EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
}

bool CGeomecCompactionDianaRunner::FetchElementInitialPressures(const CDepletionStage& stage, const geo::IElement& element, IValueDomainScalar::TValueVec& vcInitialPressures)
{
  const CFormationBase &formation = *MeshBase().Formation(element);

  // initial pressures
  vcInitialPressures = formation.Pressure(stage).Component().ScalarData().ValueElement(element);

  return true;
}

void CGeomecCompactionDianaRunner::WriteElementInitialLoads(const CDepletionStage &/*stage*/,
                              const geo::IElement &element,
                              dia::CLoadCase &lcase,
                              const IValueDomainScalar::TValueVec &vcInitialPressures,
                              const IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  // pressures
  std::vector<double> vcPressures(vcInitialPressures.size());

  bool bWriteThem = false;
  bool bWriteMultiple = false;

  for(size_t j = 0; j < vcInitialPressures.size(); ++j)
  {
    vcPressures[j] = vcInitialPressures[j].Value() * 1e6;
    if(fabs(vcPressures[j]) > MIN_PRESSU_LOAD_VAL)
      bWriteThem = true;
    if(j && fabs(vcPressures[j] - vcPressures[j-1]) > MIN_PRESSU_LOAD_VAL)
      bWriteMultiple = true;
  }

  if(bWriteThem)
  {
    if(bWriteMultiple)
      new dia::CPressureLoad(lcase, vcPressures, element);
    else
      new dia::CPressureLoad(lcase, vcPressures[0], element);
  }

  bWriteMultiple = false;

  std::vector<double> vcTemperatures(vcInitialTemperatures.size());
  for(size_t j = 0; j < vcInitialTemperatures.size(); ++j)
  {
  vcTemperatures[j] = vcInitialTemperatures[j].Value() + 273.15; // convert to Kelvin
  if(j && fabs(vcTemperatures[j] - vcTemperatures[j-1]) > MIN_TEMPER_LOAD_VAL)
      bWriteMultiple = true;
  }

  if(bWriteMultiple)
  new dia::CTemperatureLoad(lcase, vcTemperatures, element);
  else
  new dia::CTemperatureLoad(lcase, vcTemperatures[0], element);
}

void CGeomecCompactionDianaRunner::FetchStagePressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcPressures)
{
  const CFormationBase &formation = *MeshBase().Formation(element);

  // depletion stage pressures
  vcPressures = formation.Pressure(stage).Component().ScalarData().ValueElement(element);
}

void CGeomecCompactionDianaRunner::FetchStageTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcTemperatures)
{
  const CFormationBase &formation = *MeshBase().Formation(element);

  // depletion stage pressures
  vcTemperatures = formation.EffectiveTemperature(stage).Component().ScalarData().ValueElement(element);
}

void CGeomecCompactionDianaRunner::WriteElementStageLoads(const CDepletionStage &stage, const geo::IElement &element, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures)
{
  // depletion stage pressures
  IValueDomainScalar::TValueVec vcValues;
  FetchStagePressures(element, stage, vcValues);
  std::vector<double> vcPressures(vcValues.size());

  assert(vcInitialPressures.size() == vcValues.size());

  bool bWriteThem = false;
  bool bWriteMultiple = false;

  for(size_t j = 0; j < vcValues.size(); ++j)
  {
    vcPressures[j] = (vcValues[j].Value() - vcInitialPressures[j].Value()) * 1e6;
    if(fabs(vcPressures[j]) > MIN_PRESSU_LOAD_VAL)
      bWriteThem = true;
    if(j && fabs(vcPressures[j] - vcPressures[j-1]) > MIN_PRESSU_LOAD_VAL)
      bWriteMultiple = true;
  }

  if(bWriteThem)
  {
    if(bWriteMultiple)
      new dia::CPressureLoad(lcase, vcPressures, element);
    else
      new dia::CPressureLoad(lcase, vcPressures[0], element);
  }

  FetchStageTemperatures(element, stage, vcValues);
  bWriteMultiple = false;
  bWriteThem = false;

  std::vector<double> vcTemperatures(vcValues.size());
  for(size_t j = 0; j < vcValues.size(); ++j)
  {
  vcTemperatures[j] = vcValues[j].Value() - vcInitialTemperatures[j].Value();
  if(fabs(vcTemperatures[j]) > MIN_TEMPER_LOAD_VAL)
      bWriteThem = true;
  if(j && fabs(vcTemperatures[j] - vcTemperatures[j-1]) > MIN_TEMPER_LOAD_VAL)
      bWriteMultiple = true;
  }

  if(bWriteThem)
  {
  if(bWriteMultiple)
      new dia::CTemperatureLoad(lcase, vcTemperatures, element);
  else
      new dia::CTemperatureLoad(lcase, vcTemperatures[0], element);
  }
}
