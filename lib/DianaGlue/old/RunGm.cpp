#include "RunGm.h"

// RunStep:
//#include "DianaRunController.h"
#include "IDianaRunController.h"
//#include "ProgressCancel.h"
//#include "GeomecUtils.h"
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"

//#include "GeomecDoc.h"
#include "Classes.h"
//#include "ModelBase.h"
//#include "DepletionStage.h"
//#include "BranchState.h"


#include "Functions.h"
//#include "TempPath.h"

//AppVersion
//#include "version.h"
#include <QString>


namespace
{
  const QString APPLICATION = "geomec";
  //const QString VERSION = xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX " " xstr(GM_BUILD_STRING);
  const QString VERSION = "5.7.0";

  const QString AppVersion(){ return APPLICATION + " " + VERSION;	}
}


CRunGm::CRunGm()
{
  //m_doc = GetGeomecDoc();
  m_doc =  new CGeomecDoc();
  m_app_version = AppVersion();
  init( m_drc );
}

CRunGm::~CRunGm()
{
  delete m_drc;
  m_drc = nullptr;

  if( m_pDianaExecuter )
  {
    delete m_pDianaExecuter;
    m_pDianaExecuter = nullptr;
  }
  if( m_pSaveModel )
  {
    delete m_pSaveModel;
    m_pSaveModel = nullptr;
  }
  if( m_pRetrieveDianaFileNames )
  {
    delete m_pRetrieveDianaFileNames;
    m_pRetrieveDianaFileNames = nullptr;
  }
}

void CRunGm::init( IDianaRunController* drc )
{
  m_drc = drc;

  m_bHasRun = false;
  m_model = (CModelBase*)(m_doc->Model());
  m_temp_path = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
  
  // FIXME: MCR
  //m_pDianaRunController = new CDianaRunController(*m_model, CAnalysisType::AT_NONLIN, false, false, false, m_temp_path);
  const CAnalysisType* type = new CAnalysisType();

  //m_drc = new m_drc(*m_model, *type, false, false, false, m_temp_path);


  m_model_full_path = m_doc->GetPathName();
  QString fileName;
  SplitPathAndFileName(m_model_full_path, m_model_path, fileName);
  m_model_name = RemoveExtension(fileName);

  m_pDianaExecuter			= nullptr;
  m_pSaveModel				= nullptr;
  m_pRetrieveDianaFileNames	= nullptr;

  m_bHasRun					= false;
}

CRunGm *CRunGm::instance()
{
  static CRunGm _instance;
  return &_instance;
}
void CRunGm::setup_diana_run_env()
{
  m_pDianaExecuter			= new CDianaExecuter();
  m_pSaveModel				= new CSaveModelConsole(*m_model, m_app_version);
  m_pRetrieveDianaFileNames	= new CRetrieveDianaFileNamesConsole();
}

bool CRunGm::step()
{
  bool bRet = false;

  if( !m_pDianaExecuter )
    setup_diana_run_env();

  if( !m_bHasRun )
  {
    // setup branch state
    CDepletionStage* pStage = m_model->DepletionStageEntry()->LastStage(); //&m_model->DepletionStageEntry().LastStage();
    while (pStage && !pStage->Initial())
    {
      pStage->setOutputType(CDepletionStage::PHASE);
      pStage = pStage->Previous(); //&
    }
  
    pStage = m_model->DepletionStageEntry()->LastStage();
    while(pStage && !pStage->IsPhaseStartStage())
      pStage = pStage->Initial() ? 0 : pStage->Previous();

    assert(pStage);
    if( pStage )
    {
      if(pStage->Initial())
        m_model->BranchState()->ClearBranch();

      //if (log) log->AddLine("Starting GEOMEC non-linear analysis");
      try
      {
        m_temp_path = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
        
        // FIXME: MCR
        //m_pDianaRunController = new CDianaRunController(*m_model, CAnalysisType::AT_NONLIN, false, false, false, m_temp_path);
        const CAnalysisType* type = new CAnalysisType();


        //m_drc = new CDianaRunController(*m_model, *type, false, false, false, m_temp_path);

        bRet = m_drc->RunFirst(m_model_full_path, m_pDianaExecuter, m_model_name.toStdString(), *m_pSaveModel, *m_pRetrieveDianaFileNames);
      }
      catch(CProgressCancel* p)
      {
        delete p;
        bRet = false;
      }
      m_bHasRun = true;
    }
  }
  else
  {
    try
    {
      bRet = m_drc->RunNext(m_model_full_path, m_pDianaExecuter, m_model_name.toStdString(), *m_pSaveModel, *m_pRetrieveDianaFileNames);
    }
    catch(CProgressCancel* p)
    {
      delete p;
      bRet = false;
    }
  }

  

  return bRet;

}