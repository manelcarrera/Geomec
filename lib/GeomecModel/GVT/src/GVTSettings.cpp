
#include <memory>

#ifndef WIN32
#include <spawn.h>
#endif // WIN32

#include "BaseEntryTypes.h"
#include "FieldFactor.h"
#include "FormationBase.h"
#include "GVTAnalysis.h"
#include "GVTInputWriter.h"
#include "GVTSeismicGridDefinition.h"
#include "GVTSettings.h"
#include "GeomecUtils.h"
#include "Global.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "ISettings.h"
#include "MeshSampler.h"
#include "ModelBase.h"
#include "NodalValueSet.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "ValueTypeFactory.h"

#include <fstream>
#include <sstream>

// CGVTSettings ***********************************************************
// ************************************************************************
CGVTSettings::CGVTSettings(const CGVTSettings &rhs)
    : CStorageNode(rhs), m_nCurrentGrid(rhs.m_nCurrentGrid), m_Model(rhs.m_Model),
      m_strOutputPrefix(rhs.m_strOutputPrefix), m_strOutputDirectory(rhs.m_strOutputDirectory), m_bdVp(rhs.m_bdVp),
      m_bdt(rhs.m_bdt), m_bResultsChanged(rhs.m_bResultsChanged), m_bImportBack(rhs.m_bImportBack), m_data(rhs.m_Model),
      m_bForceNewPointSets(rhs.m_bForceNewPointSets), m_GVTVelocityModelFile(rhs.m_GVTVelocityModelFile) {
  // Copy constructor used for having a working copy in the CGVTSettingDlg
  // So don't link with CResultRegister

  // make deep copies of the containers
  for (size_t i = 0; i < rhs.m_SeismicGrids.size(); ++i) {
    m_SeismicGrids.push_back(new CGVTSeismicGridDefinition(*rhs.m_SeismicGrids[i]));
  }
  std::map<CFormationBase *, CGVTVelocityModel *>::const_iterator it;
  for (it = rhs.m_mpFormation2Model.begin(); it != rhs.m_mpFormation2Model.end(); ++it) {
    m_mpFormation2Model.insert(
        std::map<CFormationBase *, CGVTVelocityModel *>::value_type(it->first, new CGVTVelocityModel(*it->second)));
  }

  m_vcTimeLapses = rhs.m_vcTimeLapses;
}

CGVTSettings::CGVTSettings(CModelBase &model)
    : CStorageNode(model), m_nCurrentGrid(-1), m_Model(model), m_strOutputPrefix("Run1_"), m_strOutputDirectory(""),
      m_bdVp(true), m_bdt(true), m_bResultsChanged(false), m_bImportBack(true), m_data(model),
      m_bForceNewPointSets(false), m_GVTVelocityModelFile(GVT::CGVTVelocityModelFile::velocityFromFile, m_Model) {
  LinkTo(model.ResultRegister());
  LinkTo(*model.GraphEntry(MD_BASE_FORMATION));
}

CGVTSettings &CGVTSettings::operator=(const CGVTSettings &rhs) {
  CStorageNode::operator=(rhs);
  m_nCurrentGrid = rhs.m_nCurrentGrid;
  m_strOutputPrefix = rhs.m_strOutputPrefix;
  m_strOutputDirectory = rhs.m_strOutputDirectory;
  m_bdVp = rhs.m_bdVp;
  m_bdt = rhs.m_bdt;
  m_bImportBack = rhs.m_bImportBack;
  m_bForceNewPointSets = rhs.m_bForceNewPointSets;
  m_data = rhs.m_data;

  // NOTE: don't set m_bResultsChanged

  // Clear the memory, we're going to make new copies
  ClearMemory();
  m_SeismicGrids.clear();
  m_mpFormation2Model.clear();
  // make deep copies of the containers
  for (size_t i = 0; i < rhs.m_SeismicGrids.size(); ++i) {
    m_SeismicGrids.push_back(new CGVTSeismicGridDefinition(*rhs.m_SeismicGrids[i]));
  }
  std::map<CFormationBase *, CGVTVelocityModel *>::const_iterator it;
  for (it = rhs.m_mpFormation2Model.begin(); it != rhs.m_mpFormation2Model.end(); ++it) {
    m_mpFormation2Model.insert(
        std::map<CFormationBase *, CGVTVelocityModel *>::value_type(it->first, new CGVTVelocityModel(*it->second)));
  }
  m_vcTimeLapses = rhs.m_vcTimeLapses;
  m_GVTVelocityModelFile = rhs.m_GVTVelocityModelFile;
  InvalidateStorage();

  m_bResultsChanged = true; // force building new sets
  return *this;
}

CGVTSettings::~CGVTSettings(void) { ClearMemory(); }

void CGVTSettings::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint /*uHint*/) {
  const CResultRegister *rg = dynamic_cast<const CResultRegister *>(&node);
  const TFormationBaseEntry *fbe = dynamic_cast<const TFormationBaseEntry *>(&node);

  if (rg)
    m_bResultsChanged = true;
  else if (fbe) {
    // any formations deleted?
    std::map<CFormationBase *, CGVTVelocityModel *>::iterator it;
    for (it = m_mpFormation2Model.begin(); it != m_mpFormation2Model.end(); ++it) {
      if (fbe->EntryNodes().find(it->first) == fbe->EntryNodes().end()) {
        delete it->second;
        m_mpFormation2Model.erase(it);
        break;
      }
    }
    // any new formations?
    TFormationBaseEntry::TNodeSet stNode = fbe->EntryNodes();
    TFormationBaseEntry::TNodeSet::const_iterator it_form;
    for (it_form = stNode.begin(); it_form != stNode.end(); ++it_form) {
      if (m_mpFormation2Model.find(*it_form) == m_mpFormation2Model.end()) {
        m_mpFormation2Model.insert(
            std::map<CFormationBase *, CGVTVelocityModel *>::value_type(*it_form, new CGVTVelocityModel(RFactor)));
      }
    }
  }
}

void CGVTSettings::ClearMemory() {
  for (size_t i = 0; i < m_SeismicGrids.size(); ++i) {
    delete m_SeismicGrids[i];
  }

  std::map<CFormationBase *, CGVTVelocityModel *>::iterator it;
  for (it = m_mpFormation2Model.begin(); it != m_mpFormation2Model.end(); ++it) {
    delete it->second;
  }
}

int CGVTSettings::NoOfGrids() const { return m_SeismicGrids.size(); }

CGVTSeismicGridDefinition *CGVTSettings::NewGrid() {
  QString name;
  name = QString("grid%1").arg(m_SeismicGrids.size());
  CGVTSeismicGridDefinition *grid = new CGVTSeismicGridDefinition(name);
  m_SeismicGrids.push_back(grid);
  return grid;
}

void CGVTSettings::RemoveGrid(CGVTSeismicGridDefinition *grid) {
  std::vector<CGVTSeismicGridDefinition *>::iterator it;

  for (it = m_SeismicGrids.begin(); it != m_SeismicGrids.end(); ++it) {
    if (*it == grid) {
      m_SeismicGrids.erase(it);
      delete grid;
      break;
    }
  }
}

const CGVTSeismicGridDefinition &CGVTSettings::Grid(int nIndex) const {
  assert(nIndex >= 0 && nIndex < m_SeismicGrids.size());
  return *m_SeismicGrids[nIndex];
}

CGVTSeismicGridDefinition &CGVTSettings::Grid(int nIndex) {
  assert(nIndex >= 0 && nIndex < m_SeismicGrids.size());
  return *m_SeismicGrids[nIndex];
}

void CGVTSettings::SetVelModel(CFormationBase *formation, eVelocityModel model) {
  std::map<CFormationBase *, CGVTVelocityModel *>::iterator it;
  it = m_mpFormation2Model.find(formation);
  assert(it != m_mpFormation2Model.end());
  delete it->second;
  it->second = new CGVTVelocityModel(model);
}

const CGVTVelocityModel *CGVTSettings::GetVelModel(CFormationBase *formation) const {
  std::map<CFormationBase *, CGVTVelocityModel *>::const_iterator it;
  it = m_mpFormation2Model.find(formation);
  assert(it != m_mpFormation2Model.end());
  return it->second;
}

void CGVTSettings::AddTimeLapse(CDepletionStage *time1, CDepletionStage *time2) {
  m_vcTimeLapses.push_back(std::make_pair(time1, time2));
}

void CGVTSettings::RemoveTimeLapse(int nIndex) {
  assert(nIndex >= 0 && nIndex < m_vcTimeLapses.size());
  m_vcTimeLapses.erase(m_vcTimeLapses.begin() + nIndex);
}

void CGVTSettings::SetTimeLapse(int nIndex, CDepletionStage *time1, CDepletionStage *time2, TTimeLapseType /*TLT*/) {
  assert(nIndex >= 0 && nIndex < m_vcTimeLapses.size());
  m_vcTimeLapses[nIndex].first = time1;
  m_vcTimeLapses[nIndex].second = time2;
}

QString CGVTSettings::ListObjectName() const { return "Scenario "; }

const std::vector<std::pair<CDepletionStage *, CDepletionStage *>> &CGVTSettings::TimeLapses() const {
  return m_vcTimeLapses;
}

void CGVTSettings::RemoveGrid(int nIndex) {
  assert(nIndex >= 0);
  assert(nIndex < m_SeismicGrids.size());
  if (m_SeismicGrids.size() == 0)
    return;
  delete m_SeismicGrids[nIndex];
  m_SeismicGrids.erase(m_SeismicGrids.begin() + nIndex);
  if (m_SeismicGrids.size() == 0)
    CurrentGrid(-1);
  else
    CurrentGrid(m_SeismicGrids.size() - 1);
}

std::map<CFormationBase *, CGVTVelocityModel *> &CGVTSettings::FormationToVelModelMp() {
  if (m_mpFormation2Model.size() == 0) {
    TFormationBaseEntry &entry = (TFormationBaseEntry &)*m_Model.GraphEntry(MD_BASE_FORMATION);
    TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();
    for (TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++) {
      // default set velocity model to R factor
      m_mpFormation2Model.insert(
          std::map<CFormationBase *, CGVTVelocityModel *>::value_type(*it, new CGVTVelocityModel(RFactor)));
    }
  }
  assert(m_mpFormation2Model.size() > 0);
  return m_mpFormation2Model;
}

bool CGVTSettings::GridReady() const {
  if (m_nCurrentGrid == -1)
    return false;

  CGVTSeismicGridDefinition *gridDef = m_SeismicGrids[m_nCurrentGrid];
  return gridDef->GridIsValid();
}

bool CGVTSettings::TimeLapseReady() const { return m_vcTimeLapses.size() > 0; }

bool CGVTSettings::VelocityModelFileReady() const { return m_GVTVelocityModelFile.isVelocityModelFileReady(); }

bool CGVTSettings::AnalysisIsGo() const {
  if (!GridReady() || !TimeLapseReady() || !VelocityModelFileReady())
    return false;
  return true;
}

namespace {

const QString FAILURE_MESSAGE = "The execution of '%1' failed, "
                                "would You like to see the log-file?";
const QString SEPARATOR = "\\";
const QString GVT_LOG_FILE = "gvt.log";
const QString NOTEPAD = "notepad.exe";

} // anonymous namespace

bool CGVTSettings::PrepareCalculation(IProgressBase &progress) {
  for (CGVTDataTypes::TTimeLapses::iterator it = m_vcTimeLapses.begin(); it != m_vcTimeLapses.end(); ++it) {
    if (it->second->Initial()) {
      _m()->msg("Timelapses can't end with the initial stage. Please correct your settings.");
      return false;
    }
  }

  m_data.Build(progress, *m_SeismicGrids[m_nCurrentGrid], m_mpFormation2Model, m_vcTimeLapses, DeltaVp(), Deltat(),
               m_bForceNewPointSets, m_bResultsChanged);

  m_bResultsChanged = false;

  return true;
}

CGVTSettings::eCalculatorType CGVTSettings::CalculatorType() {
#ifdef _DEBUG
  return INTERNAL;
#else
#if 0
  ISettings* registry = ISettings::instance();
  if (registry->getProfileInt("Settings", "UnlockGVT"))
  return INTERNAL;
  else
  return EXTERNAL;
#else
  return INTERNAL;
#endif
#endif
}

bool CGVTSettings::CanCalculate() {
  if (CalculatorType() == EXTERNAL) {
#ifdef _WIN32
    QString appPath = GetAppPath();
    // appPath = appPath.left(appPath.indexOf("\\Geomec.exe"));
    appPath = appPath + "\\GVT\\GVT.exe";

    return FileExists(appPath);
#else
    return false;
#endif
  }

  assert(CalculatorType() == INTERNAL);
  return true;
}

void CGVTSettings::Calculate() {
  std::auto_ptr<IProgressBase> progress(_g->prog()->create(eProgress::Dual, "GVT Analysis", false, 1));

  progress->StatusMessage("Setting Up Analysis");

  if (!PrepareCalculation(*progress))
    return;

  progress->StatusMessage("Starting Analysis...");

  switch (CalculatorType()) {
  case INTERNAL:
    InternalCalculation(*progress);
    break;
  case EXTERNAL:
    ExternalCalculation(*progress);
    break;
  default:
    assert(false);
  }
}

void CGVTSettings::InternalCalculation(IProgressBase &progress) {
  CGVTAnalysis calculator;

  calculator.Calculate(progress, m_data);

  m_data.BuildOutputPointSet(progress, *m_SeismicGrids[m_nCurrentGrid]);
}

// External calculation is going to be deprecated; therefore not moving this to CGVTAnalysis
void CGVTSettings::ExternalCalculation(IProgressBase & /* progress */) {
  CGVTInputWriter gvtInput(*this);
  QString gvtInputPath;
  gvtInputPath = QString("%1\\gvtinput.gvt").arg(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL));
  if (!gvtInput.WriteInput(gvtInputPath.toStdString().c_str())) {
    _m()->msg(gvtInput.Error());
  }

  // start the GVT job
  QString commandline;
  commandline = QString("-p \"%1\" -v \"%2\"").arg(gvtInputPath).arg(VelocityModelFile());
  if (!OutputDirectory().length() == 0) {
    commandline.append(" -o \"");
    commandline.append(OutputDirectory());
    commandline.append("\"");
  }

  QString appPath = GetAppPath();
  // appPath = appPath.left(appPath.indexOf("\\Geomec.exe"));
  appPath = appPath + "\\GVT\\GVT.exe";

#ifdef WIN32
  int returnValue = _spawnl(_P_WAIT, appPath.toStdString().c_str(), commandline.toStdString().c_str(), NULL);

  if (returnValue != 0) {
    QString message = QString(FAILURE_MESSAGE).arg(appPath);

    if (_m()->msg(message, MB_YESNO) == IDYES) {
      QString path, fileName;

      SplitPathAndFileName(gvtInputPath, path, fileName);

      QString logFile = path + SEPARATOR + GVT_LOG_FILE;

      _spawnlp(_P_NOWAIT, NOTEPAD.toStdString().c_str(), NOTEPAD.toStdString().c_str(), logFile.toStdString().c_str(),
               NULL);
    }
  }
#else  // !WIN32
  pid_t pid;
  char *argv[2];

  argv[0] = const_cast<char *>(commandline.toStdString().c_str());
  argv[1] = NULL;

  posix_spawn(&pid, appPath.toStdString().c_str(), NULL, NULL, argv, NULL);
#endif // WIN32
}

void CGVTSettings::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  progress.StatusMessage("Reading GVT Settings");
  stream >> m_strOutputPrefix;
  stream >> m_strOutputDirectory;

  m_GVTVelocityModelFile.LoadStream(stream, version, progress);

  int temp;
  stream >> temp;
  m_bdVp = (temp == 1) ? true : false;
  stream >> temp;
  m_bdt = (temp == 1) ? true : false;
  stream >> temp;
  m_bImportBack = (temp == 1) ? true : false;

  stream >> m_nCurrentGrid;
  stream >> temp;
  for (int i = 0; i < temp; ++i) {
    CGVTSeismicGridDefinition *gridDef = new CGVTSeismicGridDefinition();
    gridDef->LoadStream(stream, version, progress);
    m_SeismicGrids.push_back(gridDef);
  }

  stream >> temp;
  for (int i = 0; i < temp; ++i) {
    int index;
    stream >> index;
    TFormationBaseEntry &form_entry = (TFormationBaseEntry &)(*m_Model.GraphEntry(MD_BASE_FORMATION));
    CFormationBase *fb = form_entry.FindIndex(index);

    int velModel;
    stream >> velModel;
    CGVTVelocityModel *vm = new CGVTVelocityModel((eVelocityModel)velModel);
    int paramSize;
    stream >> paramSize;
    for (int j = 0; j < paramSize; ++j) {
      QString name;
      double value;
      stream >> name;
      stream >> value;
      vm->SetParameter(name.toStdString().c_str(), value);
    }
    if (fb) // fix issue with older models where this map was not saved properly and formations might be missing (mantis
            // #3048)
    {
      std::pair<CGVTDataTypes::TFormationVelocityModels::iterator, bool> retval =
          m_mpFormation2Model.insert(std::map<CFormationBase *, CGVTVelocityModel *>::value_type(fb, vm));
      if (!retval.second) {
        *retval.first->second = *vm;
        delete vm;
      }
    }
  }

  stream >> temp;
  for (int i = 0; i < temp; ++i) {
    int idx1, idx2;
    stream >> idx1;
    stream >> idx2;
    TDepletionStageEntry &form_entry = (TDepletionStageEntry &)(*m_Model.GraphEntry(MD_BASE_DEPLETION_STAGE));
    CDepletionStage *ds1 = form_entry.FindIndex(idx1);
    CDepletionStage *ds2 = form_entry.FindIndex(idx2);
    AddTimeLapse(ds1, ds2);
  }
  if (version > CStreamVersion(3, 0, 121)) {
    stream >> temp;
    m_bResultsChanged = (temp == 1) ? true : false;
  }
}

void CGVTSettings::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  progress.StatusMessage("Writing GVT Settings");
  stream << OutputPrefix();
  stream << OutputDirectory();

  m_GVTVelocityModelFile.SaveStream(stream, progress);

  int n;
  n = m_bdVp ? 1 : 0;
  stream << n;

  n = m_bdt ? 1 : 0;
  stream << n;

  n = m_bImportBack ? 1 : 0;
  stream << n;

  stream << m_nCurrentGrid;
  stream << (int)m_SeismicGrids.size();
  for (size_t i = 0; i < m_SeismicGrids.size(); ++i) {
    m_SeismicGrids[i]->SaveStream(stream, progress);
  }

  stream << (int)m_mpFormation2Model.size();
  std::map<CFormationBase *, CGVTVelocityModel *>::iterator it;
  for (it = m_mpFormation2Model.begin(); it != m_mpFormation2Model.end(); ++it) {
    stream << it->first->Index();
    stream << it->second->ModelType();

    std::map<QString, double> &params = it->second->Parameters();
    stream << (int)params.size();
    std::map<QString, double>::const_iterator paramIt;
    for (paramIt = params.begin(); paramIt != params.end(); ++paramIt) {
      stream << paramIt->first;
      stream << paramIt->second;
    }
  }

  stream << (int)m_vcTimeLapses.size();
  for (size_t i = 0; i < m_vcTimeLapses.size(); ++i) {
    stream << m_vcTimeLapses[i].first->Index();
    stream << m_vcTimeLapses[i].second->Index();
  }

  n = m_bResultsChanged ? 1 : 0;
  stream << n;
}

int CGVTSettings::velocitySource() const { return m_GVTVelocityModelFile.velocityModelSourceNumerator(); }

void CGVTSettings::velocitySource(int velocitySource) {
  m_GVTVelocityModelFile.velocityModelSourceNumerator(velocitySource);
}
