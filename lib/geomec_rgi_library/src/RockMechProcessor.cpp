
#include <stdexcept>
#include <algorithm>

#include <QDir>

#include "RockMechProcessor.h"
#include "GeomecUtils.h"
#include "RGInterface.h"
#include "LoadGridFromRGI.h"
#include "FemAppEntryTypes.h"
#include "MaterialEntry.h"
#include "ModelBase.h"
#include "FormationBase.h"
#include "ITetrahedron.h"
#include "IHexahedron.h"
#include "LoadPropertyBridge.h"
#include "SavePropertyBridge.h"
#include "IProgressFactory.h"
#include "IPlatform.h"
#include "AnalysisLogger.h"
#include "RGSync.h"
#include "IInterfaceElement.h"
#include "GeomecSurfaces2RGI.h"
#include "DianaRunController.h"
#include "SaveModelConsole.h"
#include "LoadPropertyMaterialType.h"
#include "MaterialHelperFactory.h"
#include "BuildTensor.h"
#include "PointSet.h"
#include "NodalValueSet.h"
#include "Result.h"
#include "DepletionStage.h"
#include "MeshResultTree.h"
#include "MaterialResultTree.h"
#include "ResultTree.h"
#include "DerivedResult.h"
#include "Global.h"
#include "PropertyMetaData.h"

namespace GeomecRGI
{

CRockMechProcessor::CRockMechProcessor(const QString &strFileName, const QString& strOutputDir, QString strTempDir)
: m_model(nullptr),
  m_strFileName(strFileName),
  m_strOutputDir(strOutputDir),
  m_bQuit(false),
  m_drc(0)
, m_isExistingModel(false)
{
  // remove trailing slash from output dir
  if(!m_strOutputDir.isEmpty() && (m_strOutputDir.right(1) == "\\" || m_strOutputDir.right(1) == "/"))
    m_strOutputDir = m_strOutputDir.left(m_strOutputDir.length() - 1);

  // remove trailing slash from temp dir
  if(!strTempDir.isEmpty() && (strTempDir.right(1) == "\\" || strTempDir.right(1) == "/"))
    strTempDir = strTempDir.left(strTempDir.length() - 1);

  if (!strTempDir.isEmpty())
    ForceGeomecTempPathExt(CTempPath::TEMP_GENERAL, strTempDir);

  SetupQuantities();
}

void CRockMechProcessor::SetModel(CModelBase& model, bool isExistingModel)
{
  m_model = &model;
  m_isExistingModel = isExistingModel;
}

namespace
{

const QString TENSOR_IS_NOT_COMPLETE =
  "Not all components of the residual strain tensor have been loaded, "
  "the tensor will be ignored.";

} // anonymous namespace

#pragma message("WARNING: executeRockMechCommands is deprecated; see WI 116602")
/*
Functionality of executeRockMechCommands is now handled by InterfaceRGI and the main loop of geomec_shell.

There is a grace period so that both geomec_shell and geomec_rgi can be tested (regression).
Changes to executeRockMechCommands should be avoided, but if that is not possible, they should be
implemented here and in geomec_shell.
*/
void CRockMechProcessor::executeRockMechCommands_DEPRECATED(int& nStage,
  const std::string& title, IDianaXWrapper* dianaXWrapper,
  const QString& getPathName, bool& bRet, ISaveModel& saveModel,
  IRetrieveDianaFileNames& retrieveDianaFileNames, bool& bSaveModel, bool validate)
{
  bSaveModel = false;

  bool bHaveCalculation = false;
  bool bHibernating     = false;

  try
  {
    RGSync sync(RGSync::Follower, m_strFileName.toStdString());

    while (!m_bQuit)
    {
      bool bHibernate = false;

      sync.lock();

      // Introduce an extra block to enforce the destruction of the
      // RGInterface instance and its release of resources.

      {
        RGInterface rgi(m_strFileName.toStdString());

        /*
          A loaded property is immediately destroyed after use (after
          execution of 'LoadProperty'). The loading of the 6 properties
          'propElementResidualStrainLoading_*' will require the persistence of
          the created object, hence the initial declaration below.
          As a side-effect the created object can be checked for completeness.
          When more tensors have to be supported the current implementation
          has to be revisited.
        */

        CBuildTensor buildTensor;

        if (validate)
        {
          bRet = CheckModel(rgi);
        }

        size_t nCommands = rgi.getNumCommands();
        size_t i;

        if (nCommands == 0)
        {
          QString l;
          l = QString("The RGI project '%1' does not contain any commands "
            "('%2' commands)").arg(m_strFileName).arg(nCommands);
          AddLogLine(l, &rgi, false, true);
          m_bQuit = true;
        }

        bool commandHibernateOrQuitEncountered = false;

        // check for quit command so we now what to do
        // when an exception is caught
        for(i = 0; i < nCommands; ++i)
        {
          GMCommand gmc(rgi.getCommandN(i));

          if(gmc.getType() == typeCommandHibernate)
          {
            commandHibernateOrQuitEncountered = true;
            bHibernating = true;
            break;
          }

          if(gmc.getType() == typeCommandQuit)
          {
            commandHibernateOrQuitEncountered = true;
            m_bQuit = true;
            break;
          }
        }

        if (!commandHibernateOrQuitEncountered)
        {
          QString l;
          l = QString("The RGI project '%1' does not contain either the "
            "'typeCommandHibernate' or the 'typeCommandQuit' command, the "
            "'typeCommandQuit' command is used as the default last command").
            arg(m_strFileName);
          AddLogLine(l, &rgi, false, true);
          m_bQuit = true;
        }

        bool bQuit = false;
        for(i = 0; i < nCommands && !bQuit; ++i)
        {
          GMCommand gmc(rgi.getCommandN(i));
          switch(gmc.getType())
          {
          case typeCommandLoadGrid:
            if (bRet)
            {
              bRet = LoadGrid(rgi);
              bSaveModel = true;
            }
            break;
          case typeCommandSaveGrid:
            if(bRet)
              bRet = SaveGrid(rgi);
            break;
          case typeCommandLoadProperty:
            if(bRet)
            {
              RGProperty property = gmc.getProperty();
              bRet = LoadProperty(rgi, property);
              bSaveModel = true;
            }
            break;
          case typeCommandSaveProperty:
            if(bRet)
            {
              RGProperty property = gmc.getProperty();
              bRet = SaveProperty(rgi, property);
            }
            break;
          case typeCommandCalculate:
            if(bRet)
            {
              if (!buildTensor.isTensorComplete())
              {
                QString message = TENSOR_IS_NOT_COMPLETE;

                AddLogLine(message, &rgi, false, false);
              }

              if (bHibernating)
              {
                if (!bHaveCalculation)
                  bRet = CalculateFirst(getPathName, dianaXWrapper, title, rgi, saveModel,
                    retrieveDianaFileNames);
                else
                  bRet = CalculateNext(getPathName, dianaXWrapper, title, rgi, saveModel,
                    retrieveDianaFileNames);

                bHaveCalculation = true;
                bSaveModel = true;
              }
              else
              {
                bRet = Calculate(getPathName, dianaXWrapper, title, rgi, saveModel,
                  retrieveDianaFileNames);
                bSaveModel = true;
              }
            }
            break;
          case typeCommandDefineInitialDate:
            if(bRet)
            {
              bRet = DefineInitialDate(rgi, gmc.getDatePrm());
              bSaveModel = true;
            }
            break;
          case typeCommandHibernate:
            bHibernate = true;
            break;
          case typeCommandQuit:
            assert(m_bQuit);
            bQuit = true;

            if (bHibernating && bHaveCalculation)
              m_drc->CleanUpAfterRun(getPathName, dianaXWrapper, title, saveModel, retrieveDianaFileNames);

            break;
          case typeCommandLast:
            assert(false);
            break;
          default:
            assert(false);
            break;
          }
        }

        if(rgi.getDepletionStagesNumber() > 0)
          nStage = rgi.getCurrentDepletionStage().getDepletionStage();

        rgi.dumpModel();
      }

      // in hibernation mode we need to do saving on quit here, as it seems that dynamo kills our thread after the lock is released
      if (m_bQuit && bHibernating && bSaveModel)
      {
        QString fileName = getSaveModelName(getPathName, nStage, bRet, true);
        saveModel.saveModel(fileName, false);

        bSaveModel = false;
      }

      sync.release();
      
      if (bHibernate)
      {
        sync.waitFor(-1);
      }
    }

  }

  catch(RGInterface::Exception& e)
  {
    QString l;
    l = QString("Caught interface error '%1'").arg(e.what());
    AddLogLine(l);
    bRet = false;
    bSaveModel = false;
  }

  catch(std::runtime_error e)
  {
    QString l;
    l = QString("Caught interface error '%1'").arg(e.what());
    AddLogLine(l);
    bRet = false;
    bSaveModel = false;
  }

}

bool CRockMechProcessor::QuitCommandReceived() const
{
  return m_bQuit;
}

bool CRockMechProcessor::isExistingModel() const
{
  return m_isExistingModel;
}

// static

CMaterialEntry* CRockMechProcessor::createMaterial(CModelBase& modelBase,
  const QString& materialName, int materialModel)
{
  CMaterialEntry* materialEntry =
    dynamic_cast <CMaterialEntry*> (modelBase.GraphEntry(MD_ROCK_MATERIAL));
  ml::CMaterialLibrary& materialLibrary = materialEntry->MaterialLibrary();

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  ml::TMaterialCreatorRefPtr materialCreator = f->getMatCreator(materialModel);

  QString name(!materialName.isEmpty() ? materialName : materialCreator->MaterialModelName() + " (rgi)"); 

  materialLibrary.AddMaterial(name, *materialCreator);

  return materialEntry;
}

void CRockMechProcessor::connectMaterials2Formations(CModelBase& modelBase,
  const std::vector <IMaterial*> materials)
{
  TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <TFormationBaseEntry*> (
      modelBase.GraphEntry(MD_BASE_FORMATION));
  size_t material = 0;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
    formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); ++formation,
      ++material)
  {
    connectMaterial2Formation(modelBase, materials[material], *formation);
  }
}

namespace {

bool copyMaterialLibraryValue(CLibraryMaterial *oldMatLib, CLibraryMaterial *newMatLib, int fromValueType, int toValueType_1, int toValueType_2 = -1)
{
  CLibraryMaterialParameter *oldParam, *newParam;
          
  if ((oldParam = oldMatLib->ParameterByValueTypeID(fromValueType)))
  {
    if (!newMatLib->ParameterByValueTypeID(fromValueType))
    {
       double oldValue = oldParam->Value();

       if ((newParam = newMatLib->ParameterByValueTypeID(toValueType_1)))
         newParam->Value(oldValue);

       if (toValueType_2 > -1 && (newParam = newMatLib->ParameterByValueTypeID(toValueType_2)))
         newParam->Value(oldValue);
    }

    return true;
  }

  return false;
}

}

void CRockMechProcessor::connectMaterial2Formation(CModelBase& modelBase,
  IMaterial* material, CFormationBase* formationBase)
{
  if (!formationBase) // formation may be 0 when we were handling interface elements
    return;

  CDepletionStageEntry* depletionStageEntry =
    dynamic_cast <CDepletionStageEntry*> (
      modelBase.GraphEntry(MD_BASE_DEPLETION_STAGE));

  for (CDepletionStageEntry::iterator stage = depletionStageEntry->begin();
    stage != depletionStageEntry->end(); ++stage)
  {
    CMaterialServer* materialServer = formationBase->ConnectedMaterial(*stage);

    if (materialServer != 0)
    {
      if (materialServer->LibraryMaterial() != 0)
      {
        if (materialServer->MaterialModel() != material->MaterialModel())
        {
          QString name = material->Name();
          material->CloneValues(*materialServer->LibraryMaterial());

          material->Name(name);

          CLibraryMaterial *oldMatLib = &materialServer->LibraryMaterial()->LibraryMaterial();
          CLibraryMaterial *newMatLib = &material->LibraryMaterial();

          copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNGS_MODULUS, IDT_VALUETYPE_YOUNGMODULUS_NORM, IDT_VALUETYPE_YOUNGMODULUS_TRANS);
          
          if (!copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNG_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP))
            copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_YOUNGS_MODULUS, IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
          
          copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSONS_RATIO, IDT_VALUETYPE_POISSONRATIO_NORM, IDT_VALUETYPE_POISSONRATIO_TRANS);
          
          if (!copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSON_DECOMP, IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP))
            copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_POISSONS_RATIO, IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
          
          copyMaterialLibraryValue(oldMatLib, newMatLib, IDT_VALUETYPE_SHEARMODULUS, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);
        }

        formationBase->ConnectedMaterial(*stage)->UnLink(
          *(formationBase->ConnectedMaterial(*stage)->LibraryMaterial()));
      }

      formationBase->ConnectedMaterial(*stage)->LinkTo(*material);
      assert(formationBase->ConnectedMaterial(*stage)->IsLinkedTo(*material));
    }
  }
}

// private

bool CRockMechProcessor::LoadGrid(RGInterface& rgi)
{
  CLoadGridFromRGI loadGridFromRGI(*m_model, rgi, *this);
  bool loadGrid = loadGridFromRGI();

  CMaterialEntry* materialEntry =
    createMaterial(*m_model, "linear rgi", MM_LINEAR);
  CGraphEntry::TEntryNodeSet entryNodeSet = materialEntry->GraphEntryNodes();

  assert(entryNodeSet.size() == 1);
  assert(materialEntry->MaterialSize() == 1);
  assert(materialEntry->MaterialLibrary().MaterialSize() == 1);

  IMaterial* material = dynamic_cast <IMaterial*> (*(entryNodeSet.begin()));

  assert(material != 0);

  TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <TFormationBaseEntry*> (
      m_model->GraphEntry(MD_BASE_FORMATION));
  std::vector <IMaterial*>
    materials(formationBaseEntry->EntryNodes().size(), material);

  connectMaterials2Formations(*m_model, materials);

  std::vector <CFormationBase*> formations = loadGridFromRGI.getGeomecFormations();

  RGProperty rgProperty(RGPropertyType::propElementMaterialType);
  CLoadPropertyMaterialType
    loadPropertyMaterialType(rgProperty, rgi, *m_model, *this);

  return loadPropertyMaterialType.loadProperty() && loadGrid;
}

bool CRockMechProcessor::SaveGrid(RGInterface& rgi) const
{
  if(rgi.getNumNodes() != 0)
  {
    AddLogLine("The RESCUE model is not empty (read-only)", &rgi, false, true);
    return false;
  }

  AddLogLine("Saving grid", &rgi, true);
  if(!m_model->Mesh().IsMesh())
  {
    AddLogLine("No mesh available in model", &rgi, false, true);
    return false;
  }

  const geo::IMesh& mesh = m_model->Mesh().Mesh();

  QString l;
  l = QString("Adding %1 nodes").arg(mesh.NodeSize());
  AddLogLine(l, &rgi, true);
  int i;
  for(i = 0; i < mesh.NodeSize(); ++i)
  {
    const geo::INode& node = mesh.Node(i);
    rgi.addNode(RGNode(node.X(), node.Y(), node.Z()));
  }

  l = QString("Adding %1 elements").arg(mesh.ElementSize());
  AddLogLine(l, &rgi, true);
  for(i = 0; i < mesh.ElementSize(); ++i)
  {
    const geo::IElement& elm = mesh.Element(i);
    RGElementType::Type elmtype = GetRGElementType(elm, rgi);
    if(elmtype == RGElementType::typeElementLast)
      return false;
    std::vector<RGNodeId> vcNodes(elm.NrOfNodes());
    int j;
    for(j = 0; j < elm.NrOfNodes(); ++j)
      vcNodes[j] = elm.Node(j).Index();
    rgi.addElement(RGElement(elmtype, vcNodes));
  }

  l = QString("Adding surfaces");
  AddLogLine(l, &rgi, true);

  CGeomecSurfaces2RGI geomecSurfaces2RGI(rgi, *this, *m_model);

  geomecSurfaces2RGI.saveSurfaces();

  AddLogLine("Done saving grid", &rgi, true);
  return true;
}

RGElementType::Type CRockMechProcessor::GetRGElementType(const geo::IElement& elm, RGInterface& rgi) const
{
  if(dynamic_cast<const geo::ITetrahedron*>(&elm))
    return RGElementType::typeTE12L;
  if(dynamic_cast<const geo::IHexahedron*>(&elm))
    return RGElementType::typeHX24L;
  if(dynamic_cast<const geo::IInterfaceElement*>(&elm))
  {
    if(elm.NrOfNodes() == 6)
      return RGElementType::typeT18IF;
    if(elm.NrOfNodes() == 8)
      return RGElementType::typeQ24IF;
  }

  QString l;
  l = QString("Invalid elementtype in mesh, index = %1, nr of nodes = %2, typeid(elm).name() gives '%3'").
           arg(elm.Index()).arg(elm.NrOfNodes()).arg(typeid(elm).name());
  AddLogLine(l, &rgi, false, true);
  assert(FALSE);
  return RGElementType::typeElementLast;
}

bool CRockMechProcessor::LoadProperty(RGInterface& rgi,
  const RGProperty& rgProperty)
{
  QString l = QString("Loading property %1").arg(PropertyName(rgProperty));

  AddLogLine(l, &rgi, true);

  try
  {
    CLoadPropertyBridge loadPropertyBridge(rgProperty, rgi, *m_model, *this);
    bool ok = EnsureDepletionStageAvailable(rgi) &&
      loadPropertyBridge.loadProperty();

    if (!ok)
    {
      l = QString("A mismatch has occurred loading property %1").
        arg(PropertyName(rgProperty));
      AddLogLine(l, &rgi, true, true);
    }

    return ok;
  }

  catch (const QString& message)
  {
    AddLogLine(message, &rgi, false, true);

    return false;
  }

  assert(false);  // Shouldn't reach this point!

  return true;
}

bool CRockMechProcessor::SaveProperty(RGInterface& rgi,
  const RGProperty& rgProperty)
{
  QString l = QString("Saving property %1").arg(PropertyName(rgProperty));

  AddLogLine(l, &rgi, true);

  try
  {
    CSavePropertyBridge savePropertyBridge(rgProperty);
    bool ok = EnsureDepletionStageAvailable(rgi) &&
      savePropertyBridge.saveProperty(rgi, *m_model, *this);

    if (!ok)
    {
      l = QString("A mismatch has occurred saving property %1").
        arg(PropertyName(rgProperty));
      AddLogLine(l, &rgi, true, true);
    }

    return ok;
  }

  catch (const QString& message)
  {
    AddLogLine(message, &rgi, false, true);

    return false;
  }

  assert(false);  // Shouldn't reach this point!

  return true;
}

bool CRockMechProcessor::CalculateFirst(const QString& getPathName,
  IDianaXWrapper* dianaXWrapper, const std::string &title, RGInterface& rgi,
  ISaveModel& saveModel, IRetrieveDianaFileNames& retrieveDianaFileNames)
{
  if(rgi.getCurrentDepletionStage().getDepletionStage() == 0)
  {
    AddLogLine("Cannot calculate the initial stage", &rgi, false, true);
    return false;
  }

  if(!EnsureDepletionStageAvailable(rgi)) // add it if no properties were loaded
    return false;

  // setup branch state
  CDepletionStage* pStage = &m_model->DepletionStageEntry().LastStage();
  while (pStage && !pStage->Initial())
  {
    pStage->setOutputType(CDepletionStage::PHASE);
    pStage = &pStage->Previous();
  }
  
  pStage = &m_model->DepletionStageEntry().LastStage();
  while(pStage && !pStage->IsPhaseStartStage())
  {
    if(pStage->Initial())
      pStage = 0;
    else
      pStage = &pStage->Previous();
  }

  assert(pStage);
  if(!pStage)
  {
    AddLogLine("Unable to determine first depletion stage for analysis", &rgi, false, true);
    return false;
  }

  if(pStage->Initial())
    m_model->BranchState().ClearBranch();

  AddLogLine("Starting GEOMEC non-linear analysis", &rgi);
  bool bRet;
  try
  {
	  m_strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
	  m_drc = new CDianaRunController (*m_model, CAnalysisType::AT_NONLIN, false, false, false, m_strTempPath);

	  dia::IDianaRunner::RunParams p = { getPathName, title, m_drc->tmp().toStdString(), dianaXWrapper, &saveModel, &retrieveDianaFileNames, nullptr, false, _g->dsa() };
	  m_drc->params(p);

	  bRet = m_drc->run(CDianaRunController::First);
  }
  catch(CProgressCancel* p)
  {
    delete p;
    bRet = false;
    AddLogLine("GEOMEC analysis cancelled", &rgi, false, true);
  }

  AddLogLine("GEOMEC non-linear analysis ended", &rgi);

  return bRet;
}

bool CRockMechProcessor::CalculateNext(const QString& getPathName,
  IDianaXWrapper* dianaXWrapper, const std::string &title, RGInterface& rgi,
  ISaveModel& saveModel, IRetrieveDianaFileNames& retrieveDianaFileNames)
{

  AddLogLine("Starting GEOMEC non-linear analysis", &rgi);
  bool bRet;
  try
  {
	  bRet = m_drc->run(CDianaRunController::Next);
  }
  catch(CProgressCancel* p)
  {
    delete p;
    bRet = false;
    AddLogLine("GEOMEC analysis cancelled", &rgi, false, true);
  }

  AddLogLine("GEOMEC non-linear analysis ended", &rgi);

  return bRet;
}


bool CRockMechProcessor::Calculate(const QString& getPathName,
  IDianaXWrapper* dianaXWrapper, const std::string &title, RGInterface& rgi,
  ISaveModel& saveModel, IRetrieveDianaFileNames& retrieveDianaFileNames)
{
  if(rgi.getCurrentDepletionStage().getDepletionStage() == 0)
  {
    AddLogLine("Cannot calculate the initial stage", &rgi, false, true);
    return false;
  }

  if(!EnsureDepletionStageAvailable(rgi)) // add it if no properties were loaded
    return false;

  // setup branch state
  CDepletionStage* pStage = &m_model->DepletionStageEntry().LastStage();
  while(pStage && !pStage->IsPhaseStartStage())
  {
    if(pStage->Initial())
      pStage = 0;
    else
      pStage = &pStage->Previous();
  }

  assert(pStage);
  if(!pStage)
  {
    AddLogLine("Unable to determine first depletion stage for analysis", &rgi, false, true);
    return false;
  }

  if(pStage->Initial())
    m_model->BranchState().ClearBranch();

  AddLogLine("Starting GEOMEC non-linear analysis", &rgi);
  bool bRet;
  try
  {
    bRet = m_model->WriteFilosModel(getPathName, dianaXWrapper, title,
      CAnalysisType::AT_NONLIN, false, false, saveModel, retrieveDianaFileNames);
  }
  catch(CProgressCancel* p)
  {
    delete p;
    bRet = false;
    AddLogLine("GEOMEC analysis cancelled", &rgi, false, true);
  }

  AddLogLine("GEOMEC non-linear analysis ended", &rgi);

  return bRet;
}

bool CRockMechProcessor::DefineInitialDate(RGInterface& rgi, const RGDate& date)
{
  QString l;
  l = QString("Define Initial Date, year = %1, month = %2, day = %3").arg(date.getYear()).arg(date.getMonth()).arg(date.getDay());
  AddLogLine(l, &rgi, true);

  if(rgi.getCurrentDepletionStage().getDepletionStage() != 0)
  {
    AddLogLine("DefineInitialDate command can only be issued for the initial stage", &rgi, false, true);
    return false;
  }

  CDepletionStage& stage = m_model->InitialDepletionStage();
  stage.Time(QDateTime(QDate(date.getYear(), date.getMonth(), date.getDay())));

  return true;
}

bool CRockMechProcessor::EnsureDepletionStageAvailable(RGInterface& rgi)
{
  const RGDepletionStage& rgStage = rgi.getCurrentDepletionStage();
  int idx = rgStage.getDepletionStage();
  int nStagesInModel = m_model->DepletionStageEntry().EntryNodes().size();
  if(idx > nStagesInModel)
  {
    QString l;
    l = QString("Number of stages in model = %1, current depletion stage = %2. It seems at least one stage was skipped...").
             arg(nStagesInModel).arg(idx);
    AddLogLine(l, &rgi, false, true);
    return false;
  }

  CDepletionStage* pStage;
  CDepletionStage* pLastStage = 0;

  if(idx == nStagesInModel)
  {
    // add this stage...
    QString strName;
    strName = QString("Depletion Stage %1").arg(idx);
    pLastStage = &m_model->DepletionStageEntry().LastStage();
    pStage = new CDepletionStage(strName,*pLastStage);
  }
  else
  {
    pStage = &m_model->DepletionStageEntry().StageByIndex(idx);
    if(!pStage->Initial())
      pLastStage = &pStage->Previous();
  }

  if(pLastStage)
  {
    if(!pLastStage->Initial())
    {
      m_model->BranchState().SetBranchStage(pStage->Previous());
      // copy non-linear parameters from the previous stage (but not from initial)
      pStage->SetNonLinParams(pLastStage->AutomaticStepSizes(),
                              pLastStage->MaxAutomaticStepSize(),
                              pLastStage->NLSteps(),
                              pLastStage->NLStepScalingFactor(),
                              pLastStage->NLConvNormVal(),
                              pLastStage->NLIterationSteps(),
                              pLastStage->NLConvergenceType(),
                              pLastStage->NLIterationScheme());
    }

    int nSeconds = rgStage.getElapsedSecs();
    QString l;
    l = QString("Elapsed number of seconds for depletion stage %1 is %2").arg(pStage->Index()).arg(nSeconds);
    AddLogLine(l, &rgi, true);

    if(nSeconds <= 0)
    {
      AddLogLine("Elapsed number of seconds must be greater than 0", &rgi, false, true);
      return false;
    }

    QDateTime qDateTime(pLastStage->Time());
    qDateTime = qDateTime.addSecs(nSeconds);
    pStage->Time(qDateTime);
//    __int64 iElapse = pLastStage->Time().SecondsTo(pStage->Time());
//    assert(iElapse == nSeconds);
  }

  assert(m_model->DepletionStageEntry().EntryNodes().size() >= idx + 1);
  m_model->DepletionStageEntry().LastStage().setOutputType(CDepletionStage::BRANCH);

  return true;
}

void CRockMechProcessor::SetupQuantities()
{
  m_quantities.insert(std::make_pair(" -", Quantity("None", false, 1)));
  m_quantities.insert(std::make_pair("-", Quantity("None", false, 1)));
  m_quantities.insert(std::make_pair("1/C", Quantity("ThermalExpansion", false, 1)));
  m_quantities.insert(std::make_pair("1/MPa", Quantity("Compressibility", true, 1e-6)));
  m_quantities.insert(std::make_pair("1/m", Quantity("Unknown", false, 1)));
  m_quantities.insert(std::make_pair("1/s", Quantity("Unknown", false, 1)));
  m_quantities.insert(std::make_pair("C", Quantity("Temperature", false, 1)));
  m_quantities.insert(std::make_pair("C/m", Quantity("TemperatureGradient", false, 1)));
  m_quantities.insert(std::make_pair("Degree", Quantity("Angle", false, 1)));
  m_quantities.insert(std::make_pair("J/(kg,K)", Quantity("None", false, 1)));
  m_quantities.insert(std::make_pair("MJ/m2", Quantity("FractureEnergy", true, 1e6)));
  m_quantities.insert(std::make_pair("MJ/m3", Quantity("VolumetricHeat", true, 1e6)));
  m_quantities.insert(std::make_pair("MPa", Quantity("Pressure", true, 1e6)));
  m_quantities.insert(std::make_pair("MPa/m", Quantity("PressureGradient", true, 1e6)));
  m_quantities.insert(std::make_pair("W/(m,K)", Quantity("ThermalConductivity", false, 1)));
  m_quantities.insert(std::make_pair("cP", Quantity("Unknown", false, 1)));
  m_quantities.insert(std::make_pair("kg/m3", Quantity("Density", false, 1)));
  m_quantities.insert(std::make_pair("m", Quantity("Length", false, 1)));
  m_quantities.insert(std::make_pair("m/s", Quantity("Velocity", false, 1)));
  m_quantities.insert(std::make_pair("mD", Quantity("Unknown", false, 1)));
}

void CRockMechProcessor::ListMonitorableProperties(RGInterface& rgi)
{
  std::map<std::string, const IValueComponentBase *> properties;
  std::vector<RGGeneralProperty> RGProps;

  CollectMonitorablePropertiesFromModel(properties, false);
  TransformMonitorableProperties(properties, RGProps);

  rgi.SetAvailableMonitorableProperties(RGProps);
}

bool CRockMechProcessor::LoadMonitorPointSets(RGInterface& rgi)
{
  RGMonitorPointSets monPointSets = rgi.GetSelectedMonitoringPointSets();

  size_t size = monPointSets.GetNumPointSets();

  m_monitorPointSets.resize(size, nullptr);
  m_monitorPointSetProps.resize(size);

  for (size_t i = 0; i < size; ++i)
  {
    const RGPointSet& monPointSet = monPointSets.GetPointSet(i);
    std::vector<RGGeneralProperty> RGProps = monPointSets.GetProperties(i);

    m_monitorPointSetProps[i].reserve(RGProps.size());

    CPointSet *ps = new CPointSet(monPointSet.GetName().c_str(), *m_model, 0, CPointSet::DIM_3D);

    size_t numPoints = monPointSet.GetNumPoints();
    std::vector<double> point(3);
    for (size_t n = 0; n < numPoints; ++n)
    {
      monPointSet.GetNthPoint(n, point[0], point[1], point[2]);
      ps->PushBack(point);
    }

    ps->reParent(m_model->GraphEntry(MD_BASE_POINTSET));
    m_monitorPointSets[i] = ps;

    for (size_t j = 0; j < RGProps.size(); ++j)
    {
      m_monitorPointSetProps[i].push_back(RGProps[j].GetProperty());
    }
  }

  return true;
}


bool CRockMechProcessor::EvaluateMonitorValues(RGInterface& rgi)
{
  std::vector<QString> vectorComponents{ "N", "E", "V", "-", "-", "-" };
  std::vector<QString> tensorComponents{ "NN", "EE", "VV", "NE", "EV", "NV" };
  std::vector<QString> *pComponents = &vectorComponents;

  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  RGMonitorPointSets monPointSets = rgi.GetSelectedMonitoringPointSets();
  RGMonitorValues monValues(rgi.getCurrentDepletionStage(), monPointSets);

  std::map<std::string, const IValueComponentBase *> properties;
  std::map<std::string, PropertyMetaData> metaData;

  CollectMonitorablePropertiesFromModel(properties, true);
  PropertyMetaData::Setup(properties, metaData);

  QRegExp regexDepStage("_D[0-9]+$");

  for (size_t i = 0; i < m_monitorPointSets.size(); ++i)
  {
    std::map<std::string, int> composites;

    CPointSet *ps = m_monitorPointSets[i];

    std::vector<std::string> RGProps = m_monitorPointSetProps[i];

    size_t numPoints = ps->PointSize();
    size_t numProps = RGProps.size();

    for (size_t j = 0; j < numProps; ++j)
    {
      std::map<std::string, const IValueComponentBase *>::iterator prop = properties.find(RGProps[j]);

      if (prop == properties.end())
      {
        AddLogLine(QString("PointSet '%1': Property '%2' not found for depletion stage %3").arg(ps->Name(), RGProps[j].c_str(), QString::number(rgi.getCurrentDepletionStage().getDepletionStage())));
        continue;
      }

      std::map<std::string, PropertyMetaData>::iterator meta = metaData.find(RGProps[j]);

      if (meta == metaData.end() || meta->second.typeName.empty() || meta->second.componentIndex < 0)
      {
        AddLogLine(QString("PointSet '%1': Cannot associate property '%2' with a Geomec type").arg(ps->Name(), RGProps[j].c_str()));
        continue;
      }

      Quantity quant = GetQuantity(prop->second->UnitName().toStdString());

      std::pair<std::map<std::string, int>::iterator, bool> res = composites.insert(std::make_pair(meta->second.propertyName, 0));

      int idx = -1;

      if (res.second)
      {
        CValueType *composite = factory->BuildValueType(*ps, meta->second.typeName.c_str(), QString("%1_D%2").arg(meta->second.propertyName.c_str()).arg(rgi.getCurrentDepletionStage().getDepletionStage()));

        if (!composite)
        {
          AddLogLine(QString("PointSet '%1': Cannot create Geomec type '%2' for property '%3'").arg(ps->Name(), meta->second.typeName.c_str(), RGProps[j].c_str()));
          continue;
        }

        idx = ps->AddNodalValueSet();

        ps->NodalValueSet(idx).LinkTo(composite->Component());

        switch (meta->second.typeType)
        {
        case PropertyMetaData::SCALAR:
          break;
        case PropertyMetaData::DUAL_SCALAR:
          break;
        case PropertyMetaData::VECTOR:
          ps->NodalValueSet(idx).Name(vectorComponents[0]);

          for (int c = 1; c < 3; ++c)
          {
            ps->AddNodalValueSet();
            ps->NodalValueSet(idx + c).LinkTo(composite->Component(c));
            ps->NodalValueSet(idx + c).Name(vectorComponents[c]);
          }
          break;
        case PropertyMetaData::TENSOR_VECTOR:
          ps->NodalValueSet(idx).Name(vectorComponents[0]);

          for (int c = 1; c < 6; ++c)
          {
            ps->AddNodalValueSet();
            ps->NodalValueSet(idx + c).LinkTo(composite->Component(c));
            ps->NodalValueSet(idx + c).Name(vectorComponents[c]);
          }
          break;
        case PropertyMetaData::TENSOR:
          ps->NodalValueSet(idx).Name(tensorComponents[0]);

          for (int c = 1; c < 6; ++c)
          {
            ps->AddNodalValueSet();
            ps->NodalValueSet(idx + c).LinkTo(composite->Component(c));
            ps->NodalValueSet(idx + c).Name(tensorComponents[c]);
          }
          break;
        }

        res.first->second = idx;
      }
      else
      {
        idx = res.first->second;
      }

      if (idx >= 0)
      {
        idx += meta->second.componentIndex;
      }

      for (size_t k = 0; k < numPoints; ++k)
      {
        geo::CValue value = prop->second->ScalarData().ValuePoint(ps->PointAt(k));

        if (value.Valid())
        {
          if (idx >= 0)
            ps->NodalValueSet(idx).Value(k, value.Value());
          monValues.Set(i, k, j, quant.Convert(value.Value()));
        }
      }
    }
  }

  rgi.SetMonitorValues(monValues);

  return true;
}


namespace
{

struct CollectResults : IGeomecModelVisitor
{
  const CDepletionStage *depStage;

  std::map<std::string, const IValueComponentBase *>& properties;

  CollectResults(std::map<std::string, const IValueComponentBase *>& properties, const CDepletionStage *depStage)
    : properties(properties)
    , depStage(depStage)
  {
  }

  virtual bool VisitDefault(CGraphNode& /*node*/) { return false; }
  virtual bool VisitMeshResultTree(CMeshResultTree& tree)
  {
    for (size_t i = 0; i < tree.childSize(); ++i)
      tree.childAt(i).Accept(*this);
    return true;
  }
  virtual bool VisitResultGroup(CResultGroup& group)
  {
    for (size_t i = 0; i < group.ChildSize(); ++i)
      group.Child(i).Accept(*this);
    return true;
  }
  virtual bool VisitIResult(IResult& result)
  {
    int componentSize = result.ComponentSize();

    if (componentSize == 5)
      componentSize -= 2;
    else if (componentSize == 7)
      --componentSize;

    if (depStage)
    {
      for (size_t i = 0; i < componentSize; ++i)
      {
        result.ResultComponent(*depStage, CAnalysisType(CAnalysisType::AT_NONLIN), 0, i)->Accept(*this);
      }
    }
    else
    {
      for (size_t i = 0; i < componentSize; ++i)
        result.Component(i).Accept(*this);
    }
    return true;
  }
  virtual bool VisitIValueComposite(IValueComposite& composite)
  {
    int componentSize = composite.ComponentSize();

    if (componentSize == 5)
      componentSize -= 2;
    else if (componentSize == 7)
      --componentSize;

    for (size_t i = 0; i < componentSize; ++i)
      composite.Component(i).Accept(*this);
    return true;
  }

  virtual bool VisitIResultComponent(IResultComponent& component)
  {
    if (component.Type() == IValueComponentBase::SCALAR && component.AnalysisType().AnalysisType() == CAnalysisType::AT_NONLIN 
      && component.CanComputeOnPoints() && (depStage == nullptr || depStage->Index() == component.Stage().Index()))
    {
      const IResult& result = static_cast<const IResult&>(component.Parent());

      std::string exportLabel = result.ExportLabel(component.ComponentIndex()).toStdString();

      if (exportLabel.empty())
      {
        const CMaterialResult *mat = dynamic_cast<const CMaterialResult *>(&result);
        if (mat)
          exportLabel = mat->StoredExportLabel().toStdString();
      }

      if (!exportLabel.empty())
      {
        std::transform(exportLabel.begin(), exportLabel.end(), exportLabel.begin(), [](char c) {
          return c == ' ' ? '_' : c;
        });
        properties.insert(std::make_pair(exportLabel, &component));
      }
    }
    return true;
  }

  virtual bool VisitIValueComponentBase(IValueComponentBase& component)
  {
    if (component.Type() == IValueComponentBase::SCALAR && component.CanComputeOnPoints())
    {
      std::string exportLabel = component.ExportLabel().toStdString();
      properties.insert(std::make_pair(exportLabel, &component));
    }
    return true;
  }
};

} // namespace


void CRockMechProcessor::CollectMonitorablePropertiesFromModel(std::map<std::string, const IValueComponentBase *>& properties, bool withDepletionStages) const
{
  CollectResults collector(properties, withDepletionStages ? &m_model->LastDepletionStage() : nullptr);

  m_model->MeshResultTree().Accept(collector);
  m_model->MaterialResultTree().Accept(collector);
  m_model->ResultTree().Accept(collector);
}

void CRockMechProcessor::TransformMonitorableProperties(const std::map<std::string, const IValueComponentBase *>& properties, std::vector<RGGeneralProperty>& rgProps) const
{
  rgProps.reserve(100);

  for (std::map<std::string, const IValueComponentBase *>::const_iterator it = properties.begin(); it != properties.end(); ++it)
  {
    std::string name = it->first;
    std::string quantity = GetQuantity(it->second->UnitName().toStdString()).name;

    rgProps.emplace_back(RGGeneralProperty(name, quantity, RGSupport::Node));
  }
}

CRockMechProcessor::Quantity CRockMechProcessor::GetQuantity(const std::string& unit) const
{
  std::map<std::string, Quantity>::const_iterator q = m_quantities.find(unit);
  if (q != m_quantities.end())
    return q->second;

  return Quantity("Unknown", false, 1);
}


CAnalysisLogger& CRockMechProcessor::Logger() const
{
  return const_cast<CAnalysisLogger&>(m_model->Logger());
}

// private

namespace
{

const QString DATE_TIME_FORMAT = "dd-MM-yyyy hh:mm:ss ";

QString getDateTimeStamp()
{
  QDateTime dateTimeStamp = QDateTime::currentDateTime();

  return dateTimeStamp.toString(DATE_TIME_FORMAT);
}

} // anonymous namespace

void CRockMechProcessor::AddLogLine(const QString& str, RGInterface* pRGI, bool bVerbose, bool bFatal) const
{
  IPlatform* platform = IPlatform::instance();
  QString traceString = getDateTimeStamp() + QString("%1\n").arg(str);

  platform->trace(traceString);
  if (m_model)
    Logger().AddLine(std::string("RockMechProcessor: ") + str.toStdString(), bVerbose);

  if (bFatal)
  {
    if (pRGI)
      pRGI->addErrorMsg(str.toStdString());
    else
      m_fatalErrors.push_back(str);
  }
}

bool CRockMechProcessor::HandleFatalErrors(RGInterface* pRGI)
{
  bool retval = m_fatalErrors.size() > 0;
  for (std::vector<QString>::iterator it = m_fatalErrors.begin(); it != m_fatalErrors.end(); ++it)
  {
    pRGI->addErrorMsg(it->toStdString());
  }
  m_fatalErrors.clear();
  return retval;
}

QString CRockMechProcessor::PropertyName(const RGProperty& property) const
{
  if(!property.isValid())
    return "<invalid>";

  switch(property.getType())
  {
  case RGPropertyType::propElementFluidPressure:
  case RGPropertyType::propElementDeltaFluidPressure:
    return "ElementFluidPressure";
  case RGPropertyType::propElementFluidPressureFrac:
  case RGPropertyType::propElementDeltaFluidPressureFrac:
    return "ElementFluidPressureFracture";
  case RGPropertyType::propElementTemperature:
  case RGPropertyType::propElementDeltaTemperature:
    return "ElementTemperature";
  case RGPropertyType::propElementVolumetricStrain:
    return "ElementVolumetricStrain";
  case RGPropertyType::propElementFormation:
    return "ElementFormation";
  case RGPropertyType::propElementYoung:
    return "ElementYoung";
  case RGPropertyType::propElementPoisson:
    return "ElementPoisson";
  case RGPropertyType::propElementPorosity:
    return "ElementPorosity";
  case RGPropertyType::propSaturatedRockDensity:
    return "SaturatedRockDensity";
  case RGPropertyType::propFracDens_1_N:
    return "Fracture density 1 (highest), Northing component of direction vector";
  case RGPropertyType::propFracDens_1_E:
    return "Fracture density 1 (highest), Easting component of direction vector";
  case RGPropertyType::propFracDens_1_D:
    return "Fracture density 1 (highest), Depth component of direction vector";
  case RGPropertyType::propFracDens_2_N:
    return "Fracture density 2 (intermediate), Northing component of direction vector";
  case RGPropertyType::propFracDens_2_E:
    return "Fracture density 2 (intermediate), Easting component of direction vector";
  case RGPropertyType::propFracDens_2_D:
    return "Fracture density 2 (intermediate), Depth component of direction vector";
  case RGPropertyType::propFracDens_3_N:
    return "Fracture density 3 (lowest), Northing component of direction vector";
  case RGPropertyType::propFracDens_3_E:
    return "Fracture density 3 (lowest), Easting component of direction vector";
  case RGPropertyType::propFracDens_3_D:
    return "Fracture density 3 (lowest), Depth component of direction vector";
  case RGPropertyType::propPermMultFrac_1:
    return "Permeability multiplier in fracture direction 1 (highest)";
  case RGPropertyType::propPermMultFrac_2:
    return "Permeability multiplier in fracture direction 2 (intermediate)";
  case RGPropertyType::propPermMultFrac_3:
    return "Permeability multiplier in fracture direction 3 (lowest)";
  case RGPropertyType::propDeltaDesorptionStrain:
    return "DeltaDesorptionStrain";
  case RGPropertyType::propBodyIndex:
    return "BodyIndex";
  case RGPropertyType::propElementShearModulus:
    return "ElementShearModulus";
  case RGPropertyType::propElementOrientation_Dip:
    return "ElementOrientation_Dip";
  case RGPropertyType::propElementOrientation_Azimuth:
    return "ElementOrientation_Azimuth";
  case RGPropertyType::propElementYoungNormal:
    return "ElementYoungNormal";
  case RGPropertyType::propElementYoungPlane:
    return "ElementYoungPlane";
  case RGPropertyType::propElementPoissonPlane:
    return "ElementPoissonPlane";
  case RGPropertyType::propElementPoissonNormal:
    return "ElementPoissonNormal";
  case RGPropertyType::propElementShearNormal:
    return "ElementShearNormal";
  case RGPropertyType::propElementEffStress_NN:
    return "ElementEffStress_NN";
  case RGPropertyType::propElementEffStress_EE:
    return "ElementEffStress_EE";
  case RGPropertyType::propElementEffStress_DD:
    return "ElementEffStress_DD";
  case RGPropertyType::propElementEffStress_NE:
    return "ElementEffStress_NE";
  case RGPropertyType::propElementEffStress_ND:
    return "ElementEffStress_ND";
  case RGPropertyType::propElementEffStress_ED:
    return "ElementEffStress_ED";
  case RGPropertyType::propElementTemperatureFrac:
    return "ElementTemperatureFrac";
  case RGPropertyType::propElementVolumetricStrainFrac:
    return "ElementVolumetricStrainFrac";
  case RGPropertyType::propElementDeltaTemperatureFrac:
    return "ElementDeltaTemperatureFrac";
  case RGPropertyType::propElementMaterialType:
    return "ElementMaterialType";
  case RGPropertyType::propElementGrainBulkModulus:
    return "ElementGrainBulkModulus";
  case RGPropertyType::propElementStrainLoadingNormal:
    return "ElementStrainLoadingNormal";
  case RGPropertyType::propElementStrainLoadingPlane:
    return "ElementStrainLoadingPlane";
  case RGPropertyType::propElementTotalStress_NN:
    return "ElementTotalStress_NN";
  case RGPropertyType::propElementTotalStress_EE:
    return "ElementTotalStress_EE";
  case RGPropertyType::propElementTotalStress_DD:
    return "ElementTotalStress_DD";
  case RGPropertyType::propElementTotalStress_NE:
    return "ElementTotalStress_NE";
  case RGPropertyType::propElementTotalStress_ND:
    return "ElementTotalStress_ND";
  case RGPropertyType::propElementTotalStress_ED:
    return "ElementTotalStress_ED";
  case RGPropertyType::propElementTotalStrain_NN:
    return "ElementTotalStrain_NN";
  case RGPropertyType::propElementTotalStrain_EE:
    return "ElementTotalStrain_EE";
  case RGPropertyType::propElementTotalStrain_DD:
    return "ElementTotalStrain_DD";
  case RGPropertyType::propElementTotalStrain_NE:
    return "ElementTotalStrain_NE";
  case RGPropertyType::propElementTotalStrain_ND:
    return "ElementTotalStrain_ND";
  case RGPropertyType::propElementTotalStrain_ED:
    return "ElementTotalStrain_ED";
  case RGPropertyType::propElementResidualStrainLoading_NN:
    return "ElementResidualStrainLoading_NN";
  case RGPropertyType::propElementResidualStrainLoading_EE:
    return "ElementResidualStrainLoading_EE";
  case RGPropertyType::propElementResidualStrainLoading_DD:
    return "ElementResidualStrainLoading_DD";
  case RGPropertyType::propElementResidualStrainLoading_NE:
    return "ElementResidualStrainLoading_NE";
  case RGPropertyType::propElementResidualStrainLoading_ND:
    return "ElementResidualStrainLoading_ND";
  case RGPropertyType::propElementResidualStrainLoading_ED:
    return "ElementResidualStrainLoading_ED";
  case RGPropertyType::propElementYoungUndrained:
	  return "ElementYoungUndrained";
  case RGPropertyType::propElementPoissonUndrained:
	  return "ElementPoissonUndrained";
  case RGPropertyType::propElementIsReservoir:
	return "ElementIsReservoir";
  case RGPropertyType::propLast:
    assert(false);
    break;
  default:
    assert(false);
    break;
  }

  return "<invalid type>";
}

QString CRockMechProcessor::getSaveModelName(const QString& pathName, int nStage, bool bSuccess, bool bAppendStageNumber)
{
  QString sPath;
  QString sFileName;
  SplitPathAndFileName(pathName, sPath, sFileName);

  if(!m_strOutputDir.isEmpty())
    sPath = m_strOutputDir;

  int iDot = sFileName.lastIndexOf('.');
  QString sBaseName;
  QString sExt;
  if(iDot >= 0)
  {
    sBaseName = sFileName.left(iDot);
    sExt = sFileName.mid(iDot);
  }
  else
  {
    sBaseName = sFileName;
  }

  QString dirBase = sPath.isEmpty() ? "" : sPath + "/";
  QString strNewName;

  if(bSuccess)
  {
    strNewName = (dirBase + sBaseName + (bAppendStageNumber ? QString("_D%1").arg(nStage) : "") + sExt);
  }
  else
  {
    QString strNewNameMask = dirBase + sBaseName + ".rockmech.D%1.error" + sExt;
    strNewName = QString(strNewNameMask).arg(nStage);
  }

  if(!strNewName.isEmpty())
    strNewName = QDir::toNativeSeparators(strNewName);

  return strNewName;
}

/*
  // first rename the current model
  QString strPathName = m_model.Document().PathName();
  QString strDirName;
  QString strFileName;
  SplitPathAndFileName(strPathName, strDirName, strFileName);
  std::vector<QString> vcTokens = TokenizeString(strFileName, ".");
  QString strExtension;
  QString strBase;
  if(vcTokens.size() > 1)
  {
    strExtension = QString(".") + vcTokens[vcTokens.size() - 1];
    strBase += vcTokens[0];
    int i;
    for(size_t i = 1; i < vcTokens.size() - 1; ++i)
    {
      strBase += ".";
      strBase += vcTokens[i];
    }
  }
  else if(vcTokens.size() == 1)
  {
    strBase = vcTokens[0];
  }

  QString strNewNameMask;
  if(!strDirName.IsEmpty())
    strNewNameMask = strDirName + "\\";

  QString strNewName;
  if(bSuccess)
  {
    strNewNameMask += strBase + ".rockmech.D%d" + strExtension;
    strNewName.Format(strNewNameMask, nStage);

    // move the previous file
    BOOL bSuccess = MoveFile(strPathName, strNewName);
    if(bSuccess)
    {
      // save the new file
      CGeomecDoc& doc = static_cast<CGeomecDoc&>(m_model.Document());
      doc.SaveModel(m_model, strPathName);
    }
  }
  else
  {
    // save the model with 'error' in the name, so the log can be viewed
    strNewNameMask += strBase + ".rockmech.D%d.error" + strExtension;
    strNewName.Format(strNewNameMask, nStage);

    // save the file (leave the original untouched
    CGeomecDoc& doc = static_cast<CGeomecDoc&>(m_model.Document());
    doc.SaveModel(m_model, strNewName);
  }
*/

bool CRockMechProcessor::CheckModel(RGInterface& rgi) const
{
  const CGraphEntry* pHomoEntry = m_model->GraphEntry(MD_BASE_HOMO_BOX);
  if(pHomoEntry && !pHomoEntry->GraphEntryNodes().empty())
  {
    AddLogLine("This model contains at least one scaling box. Scaling boxes are not allowed with RockMech processing", &rgi, false, true);
    return false;
  }

	// first check if there is sufficient memory to read in all results after calculation
	if(!m_model->ResultRegister().SufficientFreeMemory())
	{
    AddLogLine("There may be insufficient memory to load all results after the analysis", &rgi);
	}

	// make sure there are no formations with pressure changes that use the undrained material model
	TFormationBaseEntry* pEntry = (TFormationBaseEntry*)m_model->GraphEntry(MD_BASE_FORMATION);
	assert(pEntry);
	TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
	std::vector<const CFormationBase *> vcWrongFormations;

	for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		const CFormationBase &formation = **it;
    if(formation.PressureChange() && formation.Material(m_model->InitialDepletionStage()).MaterialModel() == MM_UNDRAINED)
			vcWrongFormations.push_back(&formation);
	}

	if(vcWrongFormations.size() > 0)
	{
    AddLogLine("Encountered depleting formations with undrained material. This will be ignored.", &rgi);
		for(size_t i = 0; i < vcWrongFormations.size() && i < 10; ++i)
		{
      QString msg("   ");
      msg += vcWrongFormations[i]->Name();
      AddLogLine(msg, &rgi, true);
		}

    if(vcWrongFormations.size() > 10)
    {
      QString msg("   ");
      msg += "<more>";
      AddLogLine(msg, &rgi, true);
    }
	}

	if(!m_model->Mesh().IsMesh())
	{
		if(!m_model->Mesh().CanCreateMesh())
    {
      AddLogLine("Unable to generate a mesh for the model", &rgi, false, true);
      return false;
    }

		m_model->Mesh().CreateMesh();
	}

	QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
	if(strTempPath.isEmpty())
	{
    AddLogLine("There is no valid path for temporary files set in the GEOMEC environment", &rgi, false, true);
    return false;
	}

  return true;
}

} // namespace GeomecRGI
