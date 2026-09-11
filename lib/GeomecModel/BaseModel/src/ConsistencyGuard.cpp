#include "ConsistencyGuard.h"
#include "HDF5File.h"
#include "HDF5StorageInterface.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "ValidateModel.h"
#include "BaseEntryTypes.h"
#include "ExportResultDataEntry.h"
#include "ExportResultData.h"
#include "ExportResults.h"

#include <QString>
#include <QDir>
#include <cassert>


//#include "QUtil.h"
//#include "Xml.h"
//#include "WellCasingModel.h"
//#include "WellZoomInModel.h"


CConsistencyGuardOptions::CConsistencyGuardOptions()
  : consistencyGuardUsage(CGU_ALWAYS)
  , propagateResultClearing(PRC_ALWAYS)
  , propagateWellpaths(PWP_ALWAYS)
  , movePointsets(MPS_ALWAYS)
  , autoResultExport(ARE_ALWAYS)
  , bLoaded(false)
{
}

bool CConsistencyGuardOptions::operator==(const CConsistencyGuardOptions& rhs) const
{
  return !operator!=(rhs);
}

bool CConsistencyGuardOptions::operator!=(const CConsistencyGuardOptions& rhs) const
{
  return consistencyGuardUsage != rhs.consistencyGuardUsage
  || propagateResultClearing != rhs.propagateResultClearing
  || propagateWellpaths != rhs.propagateWellpaths
  || movePointsets != rhs.movePointsets
  || autoResultExport != rhs.autoResultExport;
}

// we maintain our own versioning
void CConsistencyGuardOptions::LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& /*version*/, CStorageNode::TPROGRESS& /*prg*/)
{
  int optionsVersion;
  stream >> optionsVersion;

  int temp; // we only load first time
  stream >> temp; if (!bLoaded) consistencyGuardUsage = (TConsistencyGuardUsageType)temp;
  stream >> temp; if (!bLoaded) propagateResultClearing = (TPropagateResultClearingType)temp;
  stream >> temp; if (!bLoaded) propagateWellpaths = (TPropagateWellpathType)temp;
  stream >> temp; if (!bLoaded) movePointsets = (TMovePointsetType)temp;
  stream >> temp; if (!bLoaded) autoResultExport = (TAutoResultExportType)temp;

  bLoaded = true;
}

void CConsistencyGuardOptions::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& /*prg*/)
{
  int optionsVersion = 0;
  stream << optionsVersion;

  stream << (int)consistencyGuardUsage;
  stream << (int)propagateResultClearing;
  stream << (int)propagateWellpaths;
  stream << (int)movePointsets;
  stream << (int)autoResultExport;
}




CConsistencyGuard::CConsistencyGuard(CModelBase& rootModel)
  : m_rootModel(&rootModel)
  , m_currentModel(&rootModel)
  , m_storageFile(0)
  , m_nAttaching(0)
  , m_nChildLoading(0)
  , m_nDeleting(0)
  , m_status( UNKNOWN )
{
  m_models.reserve(8);
  m_models.push_back(&rootModel);
}

CConsistencyGuard::~CConsistencyGuard()
{
  ClearHDF5Interfaces();
}

void CConsistencyGuard::AttachChildModelStart(CModelBase& model)
{
  assert(model.parentModel());
  assert(model.GetConsistencyGuard() == this);
  assert(&model.RootModel() == m_rootModel);

  m_models.push_back(&model);
  ++m_nAttaching;
  m_status = UNKNOWN;
}

void CConsistencyGuard::AttachChildModelFinish(CModelBase& model)
{
  assert(model.parentModel());
  assert(model.GetConsistencyGuard() == this);
  assert(&model.RootModel() == m_rootModel);
  assert(m_nAttaching > 0);

  --m_nAttaching;
}

void CConsistencyGuard::DetachChildModel(CModelBase& model)
{
  assert(model.parentModel());
  assert(model.GetConsistencyGuard() == this);
  assert(&model.RootModel() == m_rootModel);

  std::vector<CModelBase *>::iterator it = std::find(m_models.begin() + 1, m_models.end(), &model);
  if (it != m_models.end())
  m_models.erase(it);
}

void CConsistencyGuard::SwitchToModel(CModelBase& model)
{
  assert(model.GetConsistencyGuard() == this);
  assert(&model.RootModel() == m_rootModel);

  m_currentModel = &model;
  m_status = UNKNOWN;
}

void CConsistencyGuard::ChildIsLoading(bool loading)
{
  if (loading)
  ++m_nChildLoading;
  else
  --m_nChildLoading;
  assert(m_nChildLoading >= 0);
}

void CConsistencyGuard::IsDeleting(bool deleting)
{
  if (deleting)
  ++m_nDeleting;
  else
  --m_nDeleting;
  assert(m_nDeleting >= 0);
}


CConsistencyGuard::TStatus CConsistencyGuard::Status(CModelBase& model) const
{
  if (&model != m_currentModel)
  return BROKEN;

  return m_status;
}

bool CConsistencyGuard::CanCalculate(CModelBase& model, CAnalysisType::TAnalysisType /*anyType*/)
{
  TStatus status = Status(model);
  if (status == UNKNOWN)
  {
  Validate(model);
  status = Status(model);
  }

  if (status == OK || status == NEED_CLEANUP)
  return true;

  return false;
}

/*CConsistencyGuard::eModel CConsistencyGuard::current_model_type()
{
  if( dynamic_cast<CWellCasingModel*>( m_currentModel ) )
    return Casing;

  if( dynamic_cast<CWellZoomInModel*>( m_currentModel ) )
    return ZoomIn;

  return Main;
}

bool CConsistencyGuard::is_model_in_scope( eScope scope )
{
  static const std::string GEOMEC_CONFIG_FILE = QUtil::native_sepatators( std::string( getenv("APPDATA") ) + "/Geomec/geomec.xml");

  static unsigned char CASING_MODEL	= 0x01; // 0000 0001
  static unsigned char ZOOM_IN_MODEL	= 0x02;	// 0000 0010
  static unsigned char MAIN_MODEL		= 0x04; // 0000 0100

  //dont't need to be verified as all the aparmeters are optional
  CXml gm_cfg( GEOMEC_CONFIG_FILE, {} , {"deformation","xsec_improvement"} ); //FIXME: already defined at 'ChangelogDlg' (main app)

  std::string key = scope == Deformation ? "deformation" : "xsec_improvement";

  int val = gm_cfg.exist( key ) ? atoi( gm_cfg.value( key ).c_str() ) : CASING_MODEL;

  eModel type_ = current_model_type();

  unsigned char filter = type_ == Casing ? CASING_MODEL : type_ == ZoomIn ? ZOOM_IN_MODEL : MAIN_MODEL;

  //unsigned char res = val & filter; //FIXME: Just to test

  return (val & filter) == filter;
}*/

void CConsistencyGuard::NotifyChange(CModelBase& model)
{
  if (&model == m_currentModel)
  {
  TStatus status = Status(model);

  if (status != BROKEN)
      m_status = UNKNOWN;
  }
}

void CConsistencyGuard::Validate(CModelBase& model, bool extensive)
{
  CleanUp(model); // simple

  TStatus status = Status(model);
  if (status == BROKEN)
  return;

  if (m_modelErrors.size() > 0)
  {
  m_status = BROKEN;
  return;
  }

  if (!model.Mesh().CanInvalidateMesh())
  {
  m_status = INVALID;
  return;
  }

  if (!model.CanCalculate())
  {
  m_status = INVALID;
  return;
  }

  if (extensive)
  {
  CValidateModel validateModel(&model);
  if (!validateModel.checkModel(false))
  {
      m_status = INVALID;
      return;
  }
  }

  // we should check here whether there is need for an extensive cleanup, but for now we set it to OK
  m_status = OK;
}

void CConsistencyGuard::CleanUp(CModelBase& /*model*/, bool extensive)
{
  // Move 3D hull to Wellpath (when applicable) is done during hull creation
  
  CopyAllWellpaths();

  if (extensive)
  {
  // remove empty formations / faults
  }
}

void CConsistencyGuard::GetErrors(QStringList& errors)
{
  errors.clear();

  for (TModelErrors::iterator it = m_modelErrors.begin(); it != m_modelErrors.end(); ++it)
  {
  int index = it->first->Index();
  for (TModelErrorEvents::iterator event = it->second.begin(); event != it->second.end(); ++event)
  {
      switch (event->first)
      {
      case WRONG_CLEARANCE_ATTEMPT:
    errors.append(QString("Model %1: a child model tried to delete results (%2 attempts)").arg(index).arg(event->second));
    break;
      }
  }
  }
}

void CConsistencyGuard::ClearErrors()
{
  m_modelErrors.clear();
  m_status = UNKNOWN;
}


CConsistencyGuardOptions CConsistencyGuard::Options() const
{
  return m_options;
}

void CConsistencyGuard::Options(CConsistencyGuardOptions& options)
{
  m_options = options;
}

void CConsistencyGuard::LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& prg)
{
  m_options.LoadStream(stream, version, prg);
}

void CConsistencyGuard::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg)
{
  m_options.SaveStream(stream, prg);
}


void CConsistencyGuard::AttachHDF5File(CHDF5File *h5file, bool bClearInterfaces)
{
  if (m_storageFile)
  {
  delete m_storageFile;
  }

  m_storageFile = h5file;

  SyncModelHDF5(bClearInterfaces);
}

QString CConsistencyGuard::HDF5FileName() const
{
  if (m_storageFile)
  return m_storageFile->FileName();
  return QString();
}

CHDF5StorageInterface *CConsistencyGuard::HDF5StorageInterface(int modelIndex, int maxStages, int maxColumns) const
{
  CHDF5StorageInterface *HDF5Interface = 0;

  if (m_storageFile && modelIndex >= 0)
  {
  TStorageInterfaces::iterator it = m_storageInterfaces.find(modelIndex);

  if (it == m_storageInterfaces.end())
  {
      if (m_storageFile->DataSetExists(CHDF5File::ResultGroup(modelIndex)))
      {
    HDF5Interface = new CHDF5StorageInterface(*m_storageFile, modelIndex, maxStages, maxColumns);
    HDF5Interface->OwnedByGuard(true);
    m_storageInterfaces.insert(std::make_pair(modelIndex, HDF5Interface));
      }
  }
  else
      HDF5Interface = it->second;
  }

  return HDF5Interface;
}

void CConsistencyGuard::InvalidateHDF5StorageInterface(int modelIndex)
{
  if (m_rootModel->Loading())
  return;

  TStorageInterfaces::iterator it = m_storageInterfaces.find(modelIndex);

  if (it != m_storageInterfaces.end())
  {
  delete it->second;
  it->second = 0;
  }
  else if (m_storageFile && modelIndex >= 0)
  {
  m_storageInterfaces.insert(std::make_pair(modelIndex, static_cast<CHDF5StorageInterface *>(0)));
  }
}


void CConsistencyGuard::SyncModelHDF5(bool bClearInterfaces)
{
  if (bClearInterfaces)
  {
  // refresh Interfaces
  for (std::vector<CModelBase *>::iterator it = m_models.begin(); it != m_models.end(); ++it)
      (*it)->ResultRegister().RemoveCache(true);

  ClearHDF5Interfaces();

  for (std::vector<CModelBase *>::iterator it = m_models.begin(); it != m_models.end(); ++it)
      (*it)->ResultRegister().CreateCache();
  }
  else
  {
  for (TStorageInterfaces::iterator it = m_storageInterfaces.begin(); it != m_storageInterfaces.end(); ++it)
      if (it->second)
    it->second->AttachHDF5File(m_storageFile);
  }
}

void CConsistencyGuard::ClearHDF5Interfaces()
{
  for (TStorageInterfaces::iterator it = m_storageInterfaces.begin(); it != m_storageInterfaces.end(); ++it)
  delete it->second;
  m_storageInterfaces.clear();
}


bool CConsistencyGuard::isDescendant(CModelBase& model, CModelBase& ancestor, bool strict)
{
  if (&model == &ancestor)
  return !strict;

  CModelBase *parent = model.parentModel();

  while (parent)
  {
  if (parent == &ancestor)
      return true;
  parent = parent->parentModel();
  }

  return false;
}

void CConsistencyGuard::getChildren(CModelBase& model, std::vector<CModelBase *>& children)
{
  assert(m_models.size() > 0);
  children.reserve(m_models.size() - 1);
  for (std::vector<CModelBase *>::iterator descendant = m_models.begin() + 1; descendant != m_models.end(); ++descendant)
  if ((*descendant)->parentModel() == &model)
      children.push_back(*descendant);
}



void CConsistencyGuard::EnterResultClearing(CModelBase& model)
{
  m_resultClearingOriginators.push_back(&model);
  assert(AllowResultClearing(model));
}

void CConsistencyGuard::LeaveResultClearing(CModelBase& model)
{
  assert(!m_resultClearingOriginators.empty());
  assert(m_resultClearingOriginators.back() == &model);
  m_resultClearingOriginators.pop_back();
}

bool CConsistencyGuard::AllowResultClearing(CModelBase& model)
{
  if (m_resultClearingOriginators.empty())
  {
  assert(false); // shouldn't happen
  return false;
  }

  // we should only be clearing results from the current model as root, unless we're creating new models or loading them (because we don't switch there), or closing them
  return (m_rootModel->getDeleting() || m_nDeleting > 0 || m_nChildLoading > 0 || m_nAttaching > 0 || m_currentModel == m_resultClearingOriginators.front() || isDescendant(model, *m_currentModel)) && isDescendant(model, *m_resultClearingOriginators.front());
}

void CConsistencyGuard::ResultClearingPropagate(CModelBase& model, TResultClearingType resultType, bool sendModified)
{
  if (m_options.propagateResultClearing == CConsistencyGuardOptions::PRC_NEVER)
  return;

  if (resultType == CLEAR_LINEAR || resultType == CLEAR_NONLIN || resultType == CLEAR_ALL)
  {
  std::vector<CModelBase *> children;
  getChildren(model, children);

  for (std::vector<CModelBase *>::iterator child = children.begin(); child != children.end(); ++child)
  {
      CModelBase::TParentResultsDef parentResultsDef = (*child)->ParentResultsDefinition();

      switch (resultType)
      {
      case CLEAR_LINEAR:
    if (parentResultsDef == CModelBase::PRD_EQUAL)
          (*child)->ResultRegister().ClearLinear(sendModified);
    else if (parentResultsDef == CModelBase::PRD_LINEAR)
          (*child)->ResultRegister().ClearAll();
    break;
      case CLEAR_NONLIN:
    if (parentResultsDef == CModelBase::PRD_EQUAL)
    {
          (*child)->ResultRegister().ClearNonLinear(sendModified);
          (*child)->ResultRegister().ClearHeat(sendModified);
          (*child)->ResultRegister().ClearMixture(sendModified);
          (*child)->ResultRegister().ClearMixtureContainment(sendModified);
    }
    else if (parentResultsDef == CModelBase::PRD_NONLIN)
          (*child)->ResultRegister().ClearAll();
    break;
      case CLEAR_HEAT:
      case CLEAR_MIXTURE:
      case CLEAR_MIXTURE_CONTAINMENT:
    /* These three don't propagate to the child models */
    break;
      case CLEAR_ALL:
    (*child)->ResultRegister().ClearAll();
    break;
      }
  }
  }
}

void CConsistencyGuard::AutoResultExport(CModelBase& model, CAnalysisType::TAnalysisType antype)
{
  if (!m_options.autoResultExport)
  return;

  CExportResultDataEntry *pExportResultDataEntry = dynamic_cast<CExportResultDataEntry *>(model.GraphEntry(MD_BASE_EXPORT_MACROS));

  if (pExportResultDataEntry)
  {
  CExportResultDataEntry::TNodeSet stNodes = pExportResultDataEntry->EntryNodes();

  // mask is a noun here
  bool bMaskLinear  = false;
  bool bMaskNonlin  = false;
  bool bMaskHeat    = false;
  bool bMaskMixture = false;
  bool bMaskContain = false;

  switch (antype)
  {
  case CAnalysisType::AT_LINEAR:
      bMaskLinear = true;
      break;
  case CAnalysisType::AT_NONLIN:
      bMaskNonlin = true;
      break;
  case CAnalysisType::AT_HEAT:
      bMaskHeat = true;
      break;
  case CAnalysisType::AT_MIXTURE:
      bMaskMixture = true;
      break;
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
      bMaskContain = true;
      break;
  }

  for (CExportResultDataEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if ((*it)->m_settings.m_export)
      {
    QDir dExportPath = (*it)->m_settings.m_folder;

    if (!dExportPath.exists())
          dExportPath.mkdir(".");

    if (!dExportPath.exists())
          continue;

    (*it)->bind();

    (*it)->m_bLinear = bMaskLinear && (*it)->m_bLinear;
    (*it)->m_bNonLinear = bMaskNonlin && (*it)->m_bNonLinear;
    (*it)->m_bHeat = bMaskHeat && (*it)->m_bHeat;
    (*it)->m_bMixture = bMaskMixture && (*it)->m_bMixture;
    (*it)->m_bMixtureContainment = bMaskContain && (*it)->m_bMixtureContainment;

    CExportResults exportResults(*(*it), model);

    QString sExportFile = (*it)->m_settings.m_file;

    exportResults.onExport(dExportPath.filePath(sExportFile), false, false);

    (*it)->unbind(false);
      }
  }
  }
}

bool CConsistencyGuard::AllowMovePointsets() const
{
  return m_options.movePointsets == CConsistencyGuardOptions::MPS_ALWAYS;
}

void CConsistencyGuard::CopyAllWellpaths()
{
  CPointSetEntry *entry = static_cast<CPointSetEntry *>(m_rootModel->GraphEntry(MD_BASE_POINTSET));
  entry->CopyAllWellpaths();
}


void CConsistencyGuard::NotifyModelError(CModelBase& model, TModelErrorType error)
{
  std::pair<TModelErrors::iterator, bool> retval_0 = m_modelErrors.insert(std::make_pair(&model, TModelErrorEvents()));
  std::pair<TModelErrorEvents::iterator, bool> retval_1 = retval_0.first->second.insert(std::make_pair(error, 0));
  ++retval_1.first->second;
}