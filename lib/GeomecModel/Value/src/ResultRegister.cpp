// ResultRegister.cpp: implementation of the ResultRegister class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "AnalysisType.h"
#include "ArchiveQDataStream.h"
#include "BranchState.h"
#include "ConsistencyGuard.h"
#include "DepletionStage.h"
#include "DianaStartUp.h"
#include "FormationBase.h"
#include "Global.h"
#include "HDF5File.h"
#include "HDF5StorageInterface.h"
#include "HorizonBase.h"
#include "IPlatform.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "InterfaceElement.h"
#include "LineInterface.h"
#include "MeshBase.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "ModelDiagnostics.h"
#include "ResultCache.h"
#include "ResultRegister.h"
#include "ScalarValueSet.h"
#include "StorageInterface.h"
#include "StreamVersion.h"
#include "VectorValueSet.h"
#include "WellCasingCement.h"
#include "WellCasingCementInterface.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"
#include "ifaceelmmat.h"
#include "lbcx.h"
#include "lbfl.h"
#include "resourceIDS.h"

#include "Printer.h"

#include "FilosFile.h" // close and open FF
#include "Global.h"

#define CHECKSUM_BUFFER_MAX 128

namespace {
Printer *printer = Printer::instance(Printer::RunRes);
}

static const int linear_plain_indices[] = {
    CI_DISPLA_X,  CI_DISPLA_Y,  CI_DISPLA_Z,  CI_POREPRES,  CI_STRESS_XX,   CI_STRESS_YY,
    CI_STRESS_ZZ, CI_STRESS_XY, CI_STRESS_YZ, CI_STRESS_XZ, CI_STRAIN_XX,   CI_STRAIN_YY,
    CI_STRAIN_ZZ, CI_STRAIN_XY, CI_STRAIN_YZ, CI_STRAIN_XZ, CI_TEMPERATURE, -1};

static const int nonlin_plain_indices[] = {CI_DISPLA_X,
                                           CI_DISPLA_Y,
                                           CI_DISPLA_Z,
                                           CI_POREPRES,
                                           CI_STRESS_XX,
                                           CI_STRESS_YY,
                                           CI_STRESS_ZZ,
                                           CI_STRESS_XY,
                                           CI_STRESS_YZ,
                                           CI_STRESS_XZ,
                                           CI_STRAIN_XX,
                                           CI_STRAIN_YY,
                                           CI_STRAIN_ZZ,
                                           CI_STRAIN_XY,
                                           CI_STRAIN_YZ,
                                           CI_STRAIN_XZ,
                                           CI_PSTRAIN_XX,
                                           CI_PSTRAIN_YY,
                                           CI_PSTRAIN_ZZ,
                                           CI_PSTRAIN_XY,
                                           CI_PSTRAIN_YZ,
                                           CI_PSTRAIN_XZ,
                                           CI_CSTRAIN_XX,
                                           CI_CSTRAIN_YY,
                                           CI_CSTRAIN_ZZ,
                                           CI_CSTRAIN_XY,
                                           CI_CSTRAIN_YZ,
                                           CI_CSTRAIN_XZ,
                                           CI_TEMPERATURE,
                                           CI_PRECON,
                                           -1};

static const int heat_plain_indices[] = {CI_TEMPERATURE, -1};

static const int mixture_plain_indices[] = {CI_DISPLA_X,
                                            CI_DISPLA_Y,
                                            CI_DISPLA_Z,
                                            CI_POREPRES,
                                            CI_STRESS_XX,
                                            CI_STRESS_YY,
                                            CI_STRESS_ZZ,
                                            CI_STRESS_XY,
                                            CI_STRESS_YZ,
                                            CI_STRESS_XZ,
                                            CI_STRAIN_XX,
                                            CI_STRAIN_YY,
                                            CI_STRAIN_ZZ,
                                            CI_STRAIN_XY,
                                            CI_STRAIN_YZ,
                                            CI_STRAIN_XZ,
                                            CI_PSTRAIN_XX,
                                            CI_PSTRAIN_YY,
                                            CI_PSTRAIN_ZZ,
                                            CI_PSTRAIN_XY,
                                            CI_PSTRAIN_YZ,
                                            CI_PSTRAIN_XZ,
                                            CI_CSTRAIN_XX,
                                            CI_CSTRAIN_YY,
                                            CI_CSTRAIN_ZZ,
                                            CI_CSTRAIN_XY,
                                            CI_CSTRAIN_YZ,
                                            CI_CSTRAIN_XZ,
                                            CI_TEMPERATURE,
                                            CI_PRECON,
                                            -1};

static const int linear_fault_indices[] = {
    CI_DISPLA_X,  CI_DISPLA_Y,  CI_DISPLA_Z,  CI_POREPRES, CI_STRESS_XX, CI_STRESS_YY, CI_STRESS_ZZ,
    CI_STRAIN_XX, CI_STRAIN_YY, CI_STRAIN_ZZ, -1};

static const int nonlin_fault_indices[] = {
    CI_DISPLA_X,  CI_DISPLA_Y,  CI_DISPLA_Z,  CI_POREPRES, CI_STRESS_XX, CI_STRESS_YY, CI_STRESS_ZZ,
    CI_STRAIN_XX, CI_STRAIN_YY, CI_STRAIN_ZZ, -1};

static const int heat_fault_indices[] = {CI_TEMPERATURE, -1};

static const int mixture_fault_indices[] = {
    CI_DISPLA_X,  CI_DISPLA_Y,  CI_DISPLA_Z,  CI_POREPRES, CI_STRESS_XX, CI_STRESS_YY, CI_STRESS_ZZ,
    CI_STRAIN_XX, CI_STRAIN_YY, CI_STRAIN_ZZ, -1};

#define NONE 0
#define PLAIN 1
#define FAULT 2

static const int linear_indices[] = {
    CI_DISPLA_X,  PLAIN | FAULT, CI_DISPLA_Y,    PLAIN | FAULT, CI_DISPLA_Z,  PLAIN | FAULT,
    CI_POREPRES,  PLAIN | FAULT, CI_STRESS_XX,   PLAIN | FAULT, CI_STRESS_YY, PLAIN | FAULT,
    CI_STRESS_ZZ, PLAIN | FAULT, CI_STRESS_XY,   PLAIN,         CI_STRESS_YZ, PLAIN,
    CI_STRESS_XZ, PLAIN,         CI_STRAIN_XX,   PLAIN | FAULT, CI_STRAIN_YY, PLAIN | FAULT,
    CI_STRAIN_ZZ, PLAIN | FAULT, CI_STRAIN_XY,   PLAIN,         CI_STRAIN_YZ, PLAIN,
    CI_STRAIN_XZ, PLAIN,         CI_TEMPERATURE, PLAIN,         -1,           NONE};

static const int nonlin_indices[] = {
    CI_DISPLA_X, PLAIN | FAULT, CI_DISPLA_Y, PLAIN | FAULT, CI_DISPLA_Z, PLAIN | FAULT, CI_POREPRES, PLAIN | FAULT,
    CI_STRESS_XX, PLAIN | FAULT, CI_STRESS_YY, PLAIN | FAULT, CI_STRESS_ZZ, PLAIN | FAULT, CI_STRESS_XY, PLAIN,
    CI_STRESS_YZ, PLAIN, CI_STRESS_XZ, PLAIN, CI_STRAIN_XX, PLAIN | FAULT, CI_STRAIN_YY, PLAIN | FAULT, CI_STRAIN_ZZ,
    PLAIN | FAULT, CI_STRAIN_XY, PLAIN, CI_STRAIN_YZ, PLAIN, CI_STRAIN_XZ, PLAIN, CI_PSTRAIN_XX, PLAIN, CI_PSTRAIN_YY,
    PLAIN, CI_PSTRAIN_ZZ, PLAIN, CI_PSTRAIN_XY, PLAIN, CI_PSTRAIN_YZ, PLAIN, CI_PSTRAIN_XZ, PLAIN,
    /* we never load or save creep strain
    CI_CSTRAIN_XX, PLAIN,
    CI_CSTRAIN_YY, PLAIN,
    CI_CSTRAIN_ZZ, PLAIN,
    CI_CSTRAIN_XY, PLAIN,
    CI_CSTRAIN_YZ, PLAIN,
    CI_CSTRAIN_XZ, PLAIN,
    */
    CI_TEMPERATURE, PLAIN, CI_PRECON, PLAIN, -1, NONE};

static const int heat_indices[] = {CI_TEMPERATURE, PLAIN | FAULT, -1, NONE};

static const int mixture_indices[] = {
    CI_DISPLA_X, PLAIN | FAULT, CI_DISPLA_Y, PLAIN | FAULT, CI_DISPLA_Z, PLAIN | FAULT, CI_POREPRES, PLAIN | FAULT,
    CI_STRESS_XX, PLAIN | FAULT, CI_STRESS_YY, PLAIN | FAULT, CI_STRESS_ZZ, PLAIN | FAULT, CI_STRESS_XY, PLAIN,
    CI_STRESS_YZ, PLAIN, CI_STRESS_XZ, PLAIN, CI_STRAIN_XX, PLAIN | FAULT, CI_STRAIN_YY, PLAIN | FAULT, CI_STRAIN_ZZ,
    PLAIN | FAULT, CI_STRAIN_XY, PLAIN, CI_STRAIN_YZ, PLAIN, CI_STRAIN_XZ, PLAIN, CI_PSTRAIN_XX, PLAIN, CI_PSTRAIN_YY,
    PLAIN, CI_PSTRAIN_ZZ, PLAIN, CI_PSTRAIN_XY, PLAIN, CI_PSTRAIN_YZ, PLAIN, CI_PSTRAIN_XZ, PLAIN,
    /* we never load or save creep strain
    CI_CSTRAIN_XX, PLAIN,
    CI_CSTRAIN_YY, PLAIN,
    CI_CSTRAIN_ZZ, PLAIN,
    CI_CSTRAIN_XY, PLAIN,
    CI_CSTRAIN_YZ, PLAIN,
    CI_CSTRAIN_XZ, PLAIN,
    */
    CI_TEMPERATURE, PLAIN, CI_PRECON, PLAIN, -1, NONE};

// we don't want to expose these values too much, so we keep the stringTable local
QString CResultRegister::localStringTable(int resultType) const {
  static std::map<int, QString> _stringTable;

  if (_stringTable.empty()) {
    _stringTable[CI_DISPLA_X] = "DisplacementX";
    _stringTable[CI_DISPLA_Y] = "DisplacementY";
    _stringTable[CI_DISPLA_Z] = "DisplacementZ";
    _stringTable[CI_POREPRES] = "PorePressure";
    _stringTable[CI_STRESS_XX] = "StressXX";
    _stringTable[CI_STRESS_YY] = "StressYY";
    _stringTable[CI_STRESS_ZZ] = "StressZZ";
    _stringTable[CI_STRESS_XY] = "StressXY";
    _stringTable[CI_STRESS_YZ] = "StressYZ";
    _stringTable[CI_STRESS_XZ] = "StressXZ";
    _stringTable[CI_STRAIN_XX] = "StrainXX";
    _stringTable[CI_STRAIN_YY] = "StrainYY";
    _stringTable[CI_STRAIN_ZZ] = "StrainZZ";
    _stringTable[CI_STRAIN_XY] = "StrainXY";
    _stringTable[CI_STRAIN_YZ] = "StrainYZ";
    _stringTable[CI_STRAIN_XZ] = "StrainXZ";
    _stringTable[CI_PSTRAIN_XX] = "PlasticStrainXX";
    _stringTable[CI_PSTRAIN_YY] = "PlasticStrainYY";
    _stringTable[CI_PSTRAIN_ZZ] = "PlasticStrainZZ";
    _stringTable[CI_PSTRAIN_XY] = "PlasticStrainXY";
    _stringTable[CI_PSTRAIN_YZ] = "PlasticStrainYZ";
    _stringTable[CI_PSTRAIN_XZ] = "PlasticStrainXZ";
    _stringTable[CI_CSTRAIN_XX] = "CreepStrainXX";
    _stringTable[CI_CSTRAIN_YY] = "CreepStrainYY";
    _stringTable[CI_CSTRAIN_ZZ] = "CreepStrainZZ";
    _stringTable[CI_CSTRAIN_XY] = "CreepStrainXY";
    _stringTable[CI_CSTRAIN_YZ] = "CreepStrainYZ";
    _stringTable[CI_CSTRAIN_XZ] = "CreepStrainXZ";
    _stringTable[CI_TEMPERATURE] = "Temperature";
    _stringTable[CI_PRECON] = "Preconsolidation";
  }
  std::map<int, QString>::iterator it = _stringTable.find(resultType);
  if (it != _stringTable.end())
    return it->second;

  return QString("Unknown(%1)").arg(resultType);
};

QString CResultRegister::localStringTableSet(std::set<int> resultTypes) const {
  QString retval;

  if (!resultTypes.empty()) {
    std::set<int>::iterator it = resultTypes.begin();

    retval = localStringTable(*it++);

    for (; it != resultTypes.end(); ++it)
      retval += ", " + localStringTable(*it);
  }

  return retval;
}

void CResultRegister::showChecksumErrors(std::set<int> stages, std::set<int> resultTypes) const {
  int minStage = INT_MAX;
  int maxStage = -1;

  for (std::set<int>::iterator it = stages.begin(); it != stages.end(); ++it) {
    if (*it < minStage)
      minStage = *it;
    if (*it > maxStage)
      maxStage = *it;
  }

  QString msg = "Failed checksum";

  if (minStage == maxStage)
    msg += QString(" for stage D%1").arg(minStage);
  else
    msg += QString("s between stages D%1 and D%2").arg(minStage).arg(maxStage);

  msg += " involving raw results " + localStringTableSet(resultTypes) + ", please check";

  _m()->msg(msg);
}

// memory sizes in Mb, only for 32 bits builds, otherwise considered 'infinite'
#if !defined(_WIN64) && !defined(LINUX_X86_64)
#define MEGA_BYTE (1024 * 1024)
#define MIN_CACHE_SIZE 64
#define MAX_PROCESS_SIZE 1400
#define MIN_MISC_SIZE 512
#endif // !defined(_WIN64) && !defined(LINUX_X86_64)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CResultRegister::CResultRegister(CMeshBase &mesh, CDepletionStageEntry &entry)
    : CStorageNode("", mesh.Model()), m_pCache(0),
      //  m_bLinear(false),
      //  m_bNonlinear(false),
      m_dp_entry(entry) {
  create(&mesh);
}

CResultRegister::~CResultRegister() {
  if (m_pCache)
    delete m_pCache;
}

// size of the process's memory in bytes
long CResultRegister::ProcessMemorySize() const {
  IPlatform *platform = IPlatform::instance();

  return platform->processMemorySize();
}

void CResultRegister::CreateCache() {
  if (!m_pCache && Mesh().IsMesh()) {
#if !defined(_WIN64) && !defined(LINUX_X86_64)
    long psize = ProcessMemorySize(); // in bytes
    psize /= MEGA_BYTE;               // in Mbytes

    long cachesize = MAX_PROCESS_SIZE - psize - MIN_MISC_SIZE; // available in Mb

    if (cachesize < MIN_CACHE_SIZE)
      cachesize = MIN_CACHE_SIZE; // use minimum of MIN_CACHE_SIZE Mb

    m_pCache = new CResultCache(static_cast<CModelBase &>(Model()), Mesh().Mesh(), 5 * NUM_COLUMNS * TimeStepSize(),
                                cachesize);
#else  // defined(_WIN64) || defined(LINUX_X86_64)
    if (static_cast<CModelBase &>(Model()).getEnableResultCache()) {
      m_pCache = new CResultCache(static_cast<CModelBase &>(Model()), Mesh().Mesh(), 5 * NUM_COLUMNS * TimeStepSize(),
                                  static_cast<CModelBase &>(Model()).getResultCacheSize());
    } else {
      m_pCache = new CResultCache(static_cast<CModelBase &>(Model()), Mesh().Mesh(), 5 * NUM_COLUMNS * TimeStepSize());
    }
#endif // !defined(_WIN64) && !defined(LINUX_X86_64)
  }
}

bool CResultRegister::SufficientFreeMemory() const {
#if !defined(_WIN64) && !defined(LINUX_X86_64)
  if (m_pCache)
    return true;

  long psize = ProcessMemorySize(); // in bytes
  psize /= MEGA_BYTE;               // in Mbytes

  // we want at least the current process size
  // added with the cache and some extra (miscellaneous stuff)
  return (psize + MIN_CACHE_SIZE + MIN_MISC_SIZE <= MAX_PROCESS_SIZE);
#else  // defined(_WIN64) || defined(LINUX_X86_64)
  return true;
#endif // !defined(_WIN64) && !defined(LINUX_X86_64)
}

bool CResultRegister::cacheExists() const { return (m_pCache != 0); }

CResultCache &CResultRegister::Cache() const {
  assert(m_pCache != 0);
  return *m_pCache;
}

int CResultRegister::ColumnNumber(const CAnalysisType &antype, int nTimeStep, int ResultType) const {
  int column = nTimeStep * NUM_COLUMNS + ResultType;

  int nStages = TimeStepSize();

  if (antype == CAnalysisType::AT_NONLIN) {
    // do nothing
  } else if (antype == CAnalysisType::AT_LINEAR) {
    // skip all non-linear results
    column += nStages * NUM_COLUMNS;
  } else if (antype == CAnalysisType::AT_HEAT) {
    // skip all non-linear and linear results
    column += 2 * nStages * NUM_COLUMNS;
  } else if (antype == CAnalysisType::AT_MIXTURE) {
    // skip all non-linear, linear and heat results
    column += 3 * nStages * NUM_COLUMNS;
  } else if (antype == CAnalysisType::AT_MIXTURE_CONTAINMENT) {
    // skip all non-linear, linear, heat and mixture results
    column += 4 * nStages * NUM_COLUMNS;
  } else {
    assert(false); // are there sufficient columns in the cacher (see CreateCache()) ?
  }

  return column;
}

void CResultRegister::Clear(const CAnalysisType &antype) {
  AvailableResultsSet(antype).clear();

  CModelBase &model = static_cast<CModelBase &>(Model());

  if (!model.RootModel().Loading()) {
    CConsistencyGuard *guard = model.GetConsistencyGuard();

    CHDF5StorageInterface *storageInterface = guard->HDF5StorageInterface(model.Index(), TimeStepSize(), NUM_COLUMNS);

    if (storageInterface)
      storageInterface->Invalidate(antype.AnalysisType());
  }
}

void CResultRegister::RemoveCache(bool bForUpdate) {
  assert(bForUpdate || !Linear() && !NonLinear() && !Heat());

  if (m_pCache)
    delete m_pCache;

  m_pCache = 0;

  if (!bForUpdate) {
    CModelBase &model = static_cast<CModelBase &>(Model());
    CConsistencyGuard *guard = model.GetConsistencyGuard();

    guard->InvalidateHDF5StorageInterface(model.Index());
  }
}

bool CResultRegister::SendModified(bool bSendModified) const {
  return bSendModified && !static_cast<const CModelBase &>(Model()).RootModel().Loading();
}

void CResultRegister::ClearLinear(bool bSendModified) {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {
    Clear(CAnalysisType::AT_LINEAR);

    assert(!Linear());

    if (!ResultsAvailable())
      RemoveCache();

    if (SendModified(bSendModified)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_LINEAR, bSendModified);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

void CResultRegister::ClearNonLinear(bool bSendModified) {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {

    const CModelBase &constModel = static_cast<const CModelBase &>(Model());
    if (constModel.BranchState().IsBranch()) {
      IPlatform *platform = IPlatform::instance();

      platform->trace("CResultRegister::ClearNonLinear called in branched state, "
                      "only clearing after branch stage\n");

      if (!constModel.BranchState().BranchStage().Last()) {
        const CDepletionStage *pStage = &constModel.BranchState().BranchStage().Next();
        do {

          AvailableResultsSet(CAnalysisType::AT_NONLIN).erase(pStage->Index());

          if (pStage->Last())
            pStage = 0;
          else
            pStage = &pStage->Next();

        } while (pStage);
      } else {
        // no change, don't send modified signal
        bSendModified = false;
      }
    } else {
      Clear(CAnalysisType::AT_NONLIN);

      assert(!NonLinear());

      if (!ResultsAvailable())
        RemoveCache();
    }

    if (SendModified(bSendModified)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_NONLIN, bSendModified);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

void CResultRegister::ClearHeat(bool bSendModified) {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {
    ClearLinear(false);
    ClearNonLinear(false);
    ClearMixture(false);
    Clear(CAnalysisType::AT_HEAT);

    assert(!Heat());

    if (!ResultsAvailable())
      RemoveCache();

    if (SendModified(bSendModified)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_HEAT, bSendModified);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

void CResultRegister::ClearMixture(bool bSendModified) {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {
    Clear(CAnalysisType::AT_MIXTURE);

    assert(!Mixture());

    if (!ResultsAvailable())
      RemoveCache();

    if (SendModified(bSendModified)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_MIXTURE, bSendModified);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

void CResultRegister::ClearMixtureContainment(bool bSendModified) {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {
    Clear(CAnalysisType::AT_MIXTURE_CONTAINMENT);

    assert(!MixtureContainment());

    if (!ResultsAvailable())
      RemoveCache();

    if (SendModified(bSendModified)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_MIXTURE_CONTAINMENT, bSendModified);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

/*!
The results are invalidated whenever there is a change in input parameters and / or in the mesh.
For example the results are invalidated when
-	The mesh invalidates
-	The global parameters for stress or initial pressure changes
-	The pressure for a formation changes
-	The material for a formation changes
-	The cohesion and / or friction angle parameter for a fault changes.
*/
void CResultRegister::ClearAll() {
  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  guard->EnterResultClearing(model);

  if (guard->AllowResultClearing(model)) {
    ClearLinear(false);
    ClearNonLinear(false);
    ClearHeat(false);
    ClearMixture(false);
    ClearMixtureContainment(false);

    if (SendModified(true)) {
      Modified();
      guard->NotifyChange(model);
    }

    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_ALL, true);
  } else {
    guard->NotifyModelError(model, CConsistencyGuard::WRONG_CLEARANCE_ATTEMPT);
  }

  guard->LeaveResultClearing(model);
}

bool CResultRegister::ResultsAvailable() const {
  return Linear() || NonLinear() || Heat() || Mixture() || MixtureContainment();
}

/*!
  Returns true when linear result are available
*/
bool CResultRegister::Linear() const {
  return !m_stAvailableLinearResults.empty();
  //	return m_bLinear;
}

/*!
  Returns true when non-linear result are available
*/
bool CResultRegister::NonLinear() const {
  return !m_stAvailableNonlinResults.empty();
  //	return m_bNonlinear;
}

bool CResultRegister::Heat() const { return !m_stAvailableHeatResults.empty(); }

bool CResultRegister::Mixture() const { return !m_stAvailableMixtureResults.empty(); }

bool CResultRegister::MixtureContainment() const { return !m_stAvailableMixtureContainmentResults.empty(); }

bool CResultRegister::ResultsAvailable(const CAnalysisType &antype) const {
  return !AvailableResultsSet(antype).empty();
}

const CMeshBase &CResultRegister::Mesh() const {
  assert(parent());
  return (CMeshBase &)*parent();
}

CMeshBase &CResultRegister::Mesh() {
  assert(parent());
  return (CMeshBase &)*parent();
}

const CDepletionStageEntry &CResultRegister::DepletionStageEntry() const { return m_dp_entry; }

CDepletionStageEntry &CResultRegister::DepletionStageEntry() { return m_dp_entry; }

bool CResultRegister::IsPlainElement(const geo::IElement &element) const {
  if (dynamic_cast<const geo::IBody *>(&element) != 0)
    return true;
  return false;
}

bool CResultRegister::IsFaultElement(const geo::IElement &element) const { return !IsPlainElement(element); }

void CResultRegister::ReadFilosElementNodeResult(const geo::IElement &element, int nNodeNr,
                                                 const CDepletionStage &stage, const CAnalysisType &antype) {
  // Read plastic strain
  CStrainTensor plastic_strain;
  CStrainTensor creep_strain;

  if (XistIndexed("EPSP", 0)) {
    ftn_double_t epsp[6];
    GetItemLength("EPSP", epsp, 6);
    plastic_strain.Set(-epsp[0], -epsp[1], -epsp[2], -epsp[3] * 0.5, -epsp[4] * 0.5, -epsp[5] * 0.5);
  } else {
    plastic_strain.Set(0, 0, 0, 0, 0, 0);
  }

  if (XistIndexed("EPSC", 0)) {
    ftn_double_t epsc[6];
    GetItemLength("EPSC", epsc, 6);
    creep_strain.Set(-epsc[0], -epsc[1], -epsc[2], -epsc[3] * 0.5, -epsc[4] * 0.5, -epsc[5] * 0.5);
  } else {
    creep_strain.Set(0, 0, 0, 0, 0, 0);
  }

  // wedx 03072007:
  // plastic strain is now the sum of the original plastic strain and the creep strain
  if (IsPlainElement(element)) {
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XX), nNodeNr,
                              plastic_strain.XX() + creep_strain.XX());
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_YY), nNodeNr,
                              plastic_strain.YY() + creep_strain.YY());
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_ZZ), nNodeNr,
                              plastic_strain.ZZ() + creep_strain.ZZ());
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XY), nNodeNr,
                              plastic_strain.XY() + creep_strain.XY());
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_YZ), nNodeNr,
                              plastic_strain.YZ() + creep_strain.YZ());
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XZ), nNodeNr,
                              plastic_strain.XZ() + creep_strain.XZ());
  }

  double dPorePres = 0;

  if (XistIndexed("PR", 0)) {
    ftn_double_t ddum;
    GetItem("PR", &ddum);
    dPorePres = (double)(ddum * 1e-6);
  }

  double dTemperature = 0;
  if (XistIndexed("TE", 0)) {
    ftn_double_t ddum;
    GetItem("TE", &ddum);
    dTemperature = (double)(ddum - 273.15); // convert to Celsius
  }

  double dPrecon = -1;
  if (XistIndexed("PC", 0)) {
    ftn_double_t ddum;
    GetItem("PC", &ddum);
    dPrecon = (double)(ddum * 1e-6);
  }

  // Read SIG and EPS
  int sz;
  ftn_double_t *valeps = 0;
  ftn_double_t *valsig = 0;

  if (XistIndexed("SIG", 0)) {
    sz = Inquire("SIG", "DIM");
    assert(sz == 2 || sz == 3 || sz == 6);

    assert(XistIndexed("EPS", 0));
    assert(Inquire("EPS", "DIM") == sz);

    valeps = (ftn_double_t *)DiMalloc(sz * sizeof(ftn_double_t), "CResultRegister::ReadFilosElementNodeResult");
    valsig = (ftn_double_t *)DiMalloc(sz * sizeof(ftn_double_t), "CResultRegister::ReadFilosElementNodeResult");

    GetItemLength("EPS", valeps, sz);
    GetItemLength("SIG", valsig, sz);
  } else {
    sz = 3;
    valeps = (ftn_double_t *)DiMalloc(sz * sizeof(ftn_double_t), "CResultRegister::ReadFilosElementNodeResult");
    valsig = (ftn_double_t *)DiMalloc(sz * sizeof(ftn_double_t), "CResultRegister::ReadFilosElementNodeResult");

    for (int i = 0; i < 3; i++) {
      valeps[i] = 0;
      valsig[i] = 0;
    }
  }

  for (int i = 0; i < sz; i++)
    valsig[i] *= 1e-6;

  if (sz == 2) {
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XX), nNodeNr, -valeps[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YY), nNodeNr, -valeps[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_ZZ), nNodeNr, 0);

    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XX), nNodeNr, -valsig[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YY), nNodeNr, -valsig[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_ZZ), nNodeNr, 0);
  } else if (sz == 3) {
    // interface result

    // the EPS values contain the relative displacement vector
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XX), nNodeNr, -valeps[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YY), nNodeNr, -valeps[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_ZZ), nNodeNr, -valeps[2]);

    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XX), nNodeNr, -valsig[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YY), nNodeNr, -valsig[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_ZZ), nNodeNr, -valsig[2]);
  } else {
    // body result

    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XX), nNodeNr, -valeps[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YY), nNodeNr, -valeps[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_ZZ), nNodeNr, -valeps[2]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XY), nNodeNr,
                              -valeps[3] * 0.5);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YZ), nNodeNr,
                              -valeps[4] * 0.5);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XZ), nNodeNr,
                              -valeps[5] * 0.5);

    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XX), nNodeNr, -valsig[0]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YY), nNodeNr, -valsig[1]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_ZZ), nNodeNr, -valsig[2]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XY), nNodeNr, -valsig[3]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YZ), nNodeNr, -valsig[4]);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XZ), nNodeNr, -valsig[5]);
  }

  // DiFree can handle NULL pointers
  DiFree(valeps, "CResultRegister::ReadFilosElementNodeResult");
  DiFree(valsig, "CResultRegister::ReadFilosElementNodeResult");

  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_POREPRES), nNodeNr, dPorePres);

  if (IsPlainElement(element)) {
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_TEMPERATURE), nNodeNr,
                              dTemperature);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PRECON), nNodeNr, dPrecon);
  }
}

// write default values to the register for elements or nodes that
// are not available in the Filos file
void CResultRegister::DefaultElementNodeResult(const geo::IElement &element, int nNodeNr, const CDepletionStage &stage,
                                               const CAnalysisType &antype) {
  // write zeros to the cache
  // pore pressure
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_POREPRES), nNodeNr, 0);

  // temperature
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_TEMPERATURE), nNodeNr, 0);

  // stresses
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XX), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YY), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_ZZ), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XY), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_YZ), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRESS_XZ), nNodeNr, 0);

  // strains
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XX), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YY), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_ZZ), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XY), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_YZ), nNodeNr, 0);
  Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_STRAIN_XZ), nNodeNr, 0);

  // plastic strains
  if (IsPlainElement(element)) {
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XX), nNodeNr, 0);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_YY), nNodeNr, 0);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_ZZ), nNodeNr, 0);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XY), nNodeNr, 0);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_YZ), nNodeNr, 0);
    Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_PSTRAIN_XZ), nNodeNr, 0);
  }
}

// get the element results and incorporate the nodal results
// for a given analysis type (linear/nonlin)
// and a given depletion stage
void CResultRegister::ReadFilosElementResults(const CDepletionStage &Stage, const CAnalysisType &antype,
                                              IProgressBase &progress) {
  int nElements = Mesh().Mesh().ElementSize();
  progress.AddSteps(nElements);

  for (ftn_int_t idx = 1; idx <= nElements; idx++) {
    int nElem = idx - 1;
    const geo::IElement &element = Mesh().Mesh().Element(nElem);

    if (XistIndexed("ELEMEN/", &idx)) {
      PushDir();

      ChangeIndexedDir("ELEMEN/", &idx);

      /*ftn_int_t szFilos =*/Inquire("POINT", "DIM");

      for (ftn_int_t idxpoint = 1; idxpoint <= element.NrOfPoints(); idxpoint++) {
        int nNode = idxpoint - 1;

        if (XistIndexed("POINT/", &idxpoint)) {
          PushDir();
          ChangeIndexedDir("POINT/", &idxpoint);
          ReadFilosElementNodeResult(element, nNode, Stage, antype);
          PopDir();
        } else
          DefaultElementNodeResult(element, nNode, Stage, antype);
      }

      PopDir();
    } else {
      for (int nNode = 0; nNode < element.NrOfPoints(); nNode++)
        DefaultElementNodeResult(element, nNode, Stage, antype);
    }

    ReadFilosNodeResults(element, Stage, antype);

    progress.Step();
  }
}

// Get displacement for a given depletion stage
void CResultRegister::ReadFilosNodeResults(const geo::IElement &element, const CDepletionStage &stage,
                                           const CAnalysisType &antype) {
  for (int i = 0; i < element.NrOfNodes(); i++) {
    ftn_int_t idx = (ftn_int_t)(element.PointIndex(i) + 1);

    if (XistIndexed("NODES", &idx)) {
      PushDir();

      ChangeIndexedDir("NODES", &idx);

      if (XistIndexed("DISPLA", 0)) {
        // read the values from filos
        ftn_double_t displa[3];
        GetItemLength("DISPLA", displa, 3);

        // and write them into the cache
        Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_DISPLA_X), i, displa[0]);
        Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_DISPLA_Y), i, displa[1]);
        Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_DISPLA_Z), i, displa[2]);
      }

      if (XistIndexed("TEMPER", 0)) {
        ftn_double_t temper;
        GetItem("TEMPER", &temper);

        Cache().RawCacher().Value(element.Index(), ColumnNumber(antype, stage.Index(), CI_TEMPERATURE), i,
                                  temper - 273.15); // convert to celsius
      }

      PopDir();
    }
  }
}

// get results from either LINSTA or NONLIN
void CResultRegister::ReadFilosTimeSteps(const CAnalysisType &antype, const CDepletionStage &startStage,
                                         const CDepletionStage &endStage, IProgressBase &progress) {
  printer->info("read filos steps : %s -> %s", startStage.Name().toStdString().c_str(),
                endStage.Name().toStdString().c_str());

  // Walk over depleting stages
  assert(XistIndexed("STEP", 0));

  PushDir();

  const CDepletionStage *pStage = &startStage;
  ftn_int_t idx = (ftn_int_t)(startStage.Index() + 1);

  do {

    if (XistIndexed("STEP/", &idx)) {
      PushDir();

      ChangeIndexedDir("STEP/", &idx);

      printer->debug("read filos steps : step:%d", idx);

      // Printer::instance()->debug("CResultRegister > ReadFilosTimeSteps > 7 : pStage:%p", pStage );

      // Update progress
      QString strJobName;
      QString stringTableEntry = getStringTableEntry(IDS_RETRIEVE_ELEMENT_RESULTS);
      strJobName = QString(stringTableEntry).arg(pStage->Name());
      progress.NextJob(strJobName);

      ReadFilosElementResults(*pStage, antype, progress);

      AvailableResultsSet(antype).insert(pStage->Index()); // mark as available

      PopDir();
    }

    ++idx;

    if (pStage->Last() || pStage == &endStage)
      pStage = 0;
    else
      pStage = &pStage->Next();

  } while (pStage != 0);

  //	assert(pStage == 0);

  PopDir();
}

unsigned int CResultRegister::TypeId() const { return 0; }

unsigned int CResultRegister::IconId() const { return 0; }

int CResultRegister::TimeStepSize() const { return DepletionStageEntry().EntryNodes().size(); }

/* Result Import ******************************************************************************* */
void CResultRegister::AddResultFromImport(int nResultType, int nElementIndex, int nNodeIndex, int nDepletionStage,
                                          const CAnalysisType &antype, double value) {
  assert(nResultType >= 0);
  assert(nResultType < NUM_COLUMNS);

  if (!m_pCache)
    CreateCache();

  // Add Result value
  AvailableResultsSet(antype).insert(nDepletionStage);

  if ((nResultType >= CI_STRAIN_XY && nResultType <= CI_STRAIN_XZ) ||
      (nResultType >= CI_PSTRAIN_XY && nResultType <= CI_PSTRAIN_XZ) ||
      (nResultType >= CI_CSTRAIN_XY && nResultType <= CI_CSTRAIN_XZ)) {
    value *= 0.5; // engineering strains to scientific strains
  }

  if (nResultType >= CI_CSTRAIN_XX && nResultType <= CI_CSTRAIN_XZ)
    IncrementCacheValue(nElementIndex, antype, nDepletionStage, CI_PSTRAIN_XX + (nResultType - CI_CSTRAIN_XX),
                        nNodeIndex, value);
  else
    Cache().RawCacher().Value(nElementIndex, ColumnNumber(antype, nDepletionStage, nResultType), nNodeIndex, value);

  Cache().InvalidateStorage();
}

/* ********************************************************************************************* */

bool CResultRegister::ReadFilosResults(const QString &filosFile) {
  printer->info("read filos results : %s", filosFile.toStdString().c_str());

  if (_m()->msg("WARNING: experimental feature, please make sure your model is saved. For expert use only.",
                MB_OKCANCEL) == IDCANCEL)
    return false;

  OpenFilos(filosFile.toStdString().c_str(), O_RDONLY);

  CAnalysisType antype;
  CDepletionStage *initialStage = 0;
  CDepletionStage *lastStage = 0;

  if (XistIndexed("/RESULT", 0)) {
    PushDir();

    ChangeDir("/RESULT");

    if (XistIndexed("LINSTA", 0) != 0) {
      antype = CAnalysisType::AT_LINEAR;
      ChangeDir("LINSTA");
    } else if (XistIndexed("MIXTUR", 0) != 0) {
      if (_m()->msg("Mixture results found; press Ok for Mixture Containment or cancel for normal Mixture",
                    MB_OKCANCEL) == IDCANCEL)
        antype = CAnalysisType::AT_MIXTURE;
      else
        antype = CAnalysisType::AT_MIXTURE_CONTAINMENT;
      ChangeDir("MIXTUR");
    } else if (XistIndexed("NONLIN", 0) != 0) {
      antype = CAnalysisType::AT_NONLIN;
      ChangeDir("NONLIN");
    } else if (XistIndexed("HEATTR", 0) != 0) {
      antype = CAnalysisType::AT_HEAT;
      ChangeDir("HEATTR");
    }

    TDepletionStageEntry *pEntry = (TDepletionStageEntry *)(Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
    TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
    TDepletionStageEntry::TNodeSet::const_iterator it;

    if (XistIndexed("STEP", 0)) {
      ChangeDir("STEP");

      ftn_int_t idx = InquireIndex(".");

      for (TDepletionStageEntry::TNodeSet::const_iterator stage = stDepletionStage.begin();
           stage != stDepletionStage.end(); ++stage) {
        if ((*stage)->Index() == idx - 1) {
          initialStage = (*stage);
          break;
        }
      }

      int lastIdx = idx;

      while (NextIndex(&idx, 0))
        lastIdx = idx;

      for (TDepletionStageEntry::TNodeSet::const_iterator stage = stDepletionStage.begin();
           stage != stDepletionStage.end(); ++stage) {
        if ((*stage)->Index() == lastIdx - 1)
          lastStage = (*stage);
      }
    }

    PopDir();
  }

  if (initialStage && lastStage) {
    bool retval = ReadFilosResults(*initialStage, *lastStage, antype);
    dia::ff::close();
    return retval;
  } else {
    _m()->msg("Failed to find valid depletion stages.");
    return false;
  }
}

bool CResultRegister::ReadFilosResults(const CDepletionStage &startStage, const CDepletionStage &endStage,
                                       const CAnalysisType &antype) {
  printer->info("read filos results : %s -> %s", startStage.Name().toStdString().c_str(),
                endStage.Name().toStdString().c_str());

  bool is_dsa = _g->dsa();

  assert(Mesh().Mesh().ElementSize() > 0);

  CreateCache();

  PushDir();

  // Do we have results? Look in the directories ...
  assert(XistIndexed("/RESULT", 0));
  ChangeDir("/RESULT");

  // Calculate nr of jobs
  int nJobs = endStage.Index() - startStage.Index() + 1;
  //	int nJobs = TimeStepSize();

  PushDir();

  QString primaryTitle = "";

  if (XistIndexed("LINSTA", 0) != 0) {
    printer->debug("read filos: found linear results");

    assert(antype == CAnalysisType::AT_LINEAR);
    ChangeDir("LINSTA");

    if (nJobs > 1)
      primaryTitle = getStringTableEntry(IDS_RETRIEVE_LINEAR_RESULTS);
  } else if (XistIndexed("MIXTUR", 0) != 0) {
    printer->debug("read filos: found mixture results");

    assert(antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT);
    ChangeDir("MIXTUR");

    if (nJobs > 1)
      primaryTitle = getStringTableEntry(IDS_RETRIEVE_MIXTURE_RESULTS);
  } else if (XistIndexed("NONLIN", 0) != 0) {
    printer->debug("read filos: found non-linear results");

    assert(antype == CAnalysisType::AT_NONLIN);
    ChangeDir("NONLIN");

    if (nJobs > 1)
      primaryTitle = getStringTableEntry(IDS_RETRIEVE_NONLINEAR_RESULTS);
  } else if (XistIndexed("HEATTR", 0) != 0) {
    printer->debug("read filos: found heat results");

    assert(antype == CAnalysisType::AT_HEAT);
    ChangeDir("HEATTR");

    if (nJobs > 1)
      primaryTitle = getStringTableEntry(IDS_RETRIEVE_HEAT_RESULTS);
  } else {
    printer->debug("read filos: found no results");
    assert(false);
  }

  // Instantiate progress dlg
  std::auto_ptr<IProgressBase> progress(_g->prog()->create(eProgress::Dual, primaryTitle, false, nJobs));

  ReadFilosTimeSteps(antype, startStage, endStage, *progress); // in dsa crashes here

  PopDir();

  PopDir();

  Cache().InvalidateStorage();

  //	if(bLinear) m_bLinear = true;
  //	if(bNonLinear) m_bNonlinear = true;

  // We're modified
  if (!is_dsa)
    Modified();
  // We should probably do a Modified after DSA is finished

  CModelBase &model = static_cast<CModelBase &>(Model());
  CConsistencyGuard *guard = model.GetConsistencyGuard();

  switch (antype.AnalysisType()) {
  case CAnalysisType::AT_LINEAR:
    guard->EnterResultClearing(model);
    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_LINEAR, true);
    guard->LeaveResultClearing(model);
    break;
  case CAnalysisType::AT_NONLIN:
  /* Fall-through */
  case CAnalysisType::AT_HEAT:
    guard->EnterResultClearing(model);
    guard->ResultClearingPropagate(model, CConsistencyGuard::CLEAR_NONLIN, true);
    guard->LeaveResultClearing(model);
    break;
  default:
    /* do nothing */
    break;
  }

  return true;
}

void CResultRegister::LoadBodyTensorToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType &antype,
                                            int iBegin) {
  int nVals;
  int iStage;
  int iElement;

  stream >> nVals;

  if (!Mesh().IsMesh()) {
    for (int i = 0; i < nVals; i++) {
      LoadTensor(stream);
      progress.Step();
    }
    nVals = 0;
  } else if (nVals > 0) {
    for (iStage = 0; iStage < TimeStepSize(); iStage++) {
      for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
        const geo::IElement &elm = Mesh().Mesh().Element(iElement);
        if (IsPlainElement(elm)) {
          for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
            CTensor stress = LoadTensor(stream);
            if (m_pCache) {
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin), iNode, stress.XX());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 1), iNode, stress.YY());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 2), iNode, stress.ZZ());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 3), iNode, stress.XY());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 4), iNode, stress.YZ());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 5), iNode, stress.XZ());
            }
            progress.Step();
            nVals--;
          }
        }
      }
    }
  }
  assert(nVals == 0);
}

void CResultRegister::LoadBodyDoubleToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType &antype,
                                            int iBegin) {
  int nVals;
  int iStage;
  int iElement;

  stream >> nVals;
  // There are model with an invalidated mesh, but with results in it. See bug #611
  if (!Mesh().IsMesh()) {
    double dValue;
    for (int i = 0; i < nVals; i++) {
      stream >> dValue;
      progress.Step();
    }
    nVals = 0;
  } else if (nVals > 0) {
    for (iStage = 0; iStage < TimeStepSize(); iStage++) {
      for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
        const geo::IElement &elm = Mesh().Mesh().Element(iElement);
        if (IsPlainElement(elm)) {
          for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
            double dVal;
            stream >> dVal;
            if (m_pCache)
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin), iNode, dVal);
            progress.Step();
            nVals--;
          }
        }
      }
    }
  }
  assert(nVals == 0);
}

bool CResultRegister::LoadDisplacementsToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType &antype) {
  // we will load the complete point vector for the displacements
  std::vector<geo::CPoint> vcNode;

  int iStage;
  int iElement;

  LoadPointVec(stream, progress, vcNode);

  // There are models with an invalidated mesh and results. See mantis #567
  if ((vcNode.size() == 0) || !Mesh().IsMesh())
    return false;

  if (!m_pCache)
    CreateCache();

  assert(vcNode.size() == TimeStepSize() * Mesh().Mesh().PointSize());
  for (iStage = 0; iStage < TimeStepSize(); iStage++) {
    for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
      const geo::IElement &elm = Mesh().Mesh().Element(iElement);
      for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
        // the displacement is the difference between the stored point and the mesh node
        geo::CPoint p = vcNode[iStage * Mesh().Mesh().PointSize() + elm.PointIndex(iNode)] - elm.Point(iNode);
        Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, CI_DISPLA_X), iNode, p.X());
        Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, CI_DISPLA_Y), iNode, p.Y());
        Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, CI_DISPLA_Z), iNode, p.Z());
      }
    }

    AvailableResultsSet(antype).insert(iStage); // mark as available
  }

  //	if(bLinear) m_bLinear = true;
  //	else        m_bNonlinear = true;

  return true;
}

bool CResultRegister::LoadOldLinearBodyResults(TSTREAM &stream, TPROGRESS &progress) {
  // nodal displacements
  bool bLinear = LoadDisplacementsToCache(stream, progress, CAnalysisType::AT_LINEAR);

  // pore pressure
  LoadBodyDoubleToCache(stream, progress, CAnalysisType::AT_LINEAR, CI_POREPRES);

  // stresses
  LoadBodyTensorToCache(stream, progress, CAnalysisType::AT_LINEAR, CI_STRESS_XX);

  // strains
  LoadBodyTensorToCache(stream, progress, CAnalysisType::AT_LINEAR, CI_STRAIN_XX);

  return bLinear;
}

void CResultRegister::IncrementCacheValue(int iElement, const CAnalysisType &antype, int iStage, int iCacheEntry,
                                          int iNode, const double &dAdd) {
  int iCol = ColumnNumber(antype, iStage, iCacheEntry);
  const double *pValues = Cache().RawCacher().Value(iElement, iCol);
  if (pValues)
    Cache().RawCacher().Value(iElement, iCol, iNode, pValues[iNode] + dAdd);
  else
    Cache().RawCacher().Value(iElement, iCol, iNode, dAdd);
}

bool CResultRegister::LoadOldNonlinearBodyResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // nodal displacements
  bool bNonlinear = LoadDisplacementsToCache(stream, progress, CAnalysisType::AT_NONLIN);

  // pore pressure
  LoadBodyDoubleToCache(stream, progress, CAnalysisType::AT_NONLIN, CI_POREPRES);

  // stresses
  LoadBodyTensorToCache(stream, progress, CAnalysisType::AT_NONLIN, CI_STRESS_XX);

  // strains
  LoadBodyTensorToCache(stream, progress, CAnalysisType::AT_NONLIN, CI_STRAIN_XX);

  // plastic strains
  LoadBodyTensorToCache(stream, progress, CAnalysisType::AT_NONLIN, CI_PSTRAIN_XX);

  // Load creep strains from 3.0.30
  if (CStreamVersion(3, 0, 29) < version) {
    int nVals;
    int iStage;
    int iElement;

    stream >> nVals;

    if (!Mesh().IsMesh()) // ignore them
    {
      for (int i = 0; i < nVals; i++) {
        LoadTensor(stream);
        progress.Step();
      }
      nVals = 0;
    } else if (nVals > 0) {
      for (iStage = 0; iStage < TimeStepSize(); iStage++) {
        for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
          const geo::IElement &elm = Mesh().Mesh().Element(iElement);
          if (IsPlainElement(elm)) {
            for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
              CTensor stress = LoadTensor(stream);
              if (m_pCache) {
                // add the values to the plastic strains
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX, iNode, stress.XX());
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX + 1, iNode, stress.YY());
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX + 2, iNode, stress.ZZ());
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX + 3, iNode, stress.XY());
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX + 4, iNode, stress.YZ());
                IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage, CI_PSTRAIN_XX + 5, iNode, stress.XZ());
              }
              progress.Step();
              nVals--;
            }
          }
        }
      }
    }
    assert(nVals == 0);
  }

  return bNonlinear;
}

void CResultRegister::LoadFaultDoubleToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType &antype,
                                             int iBegin) {
  int nVals;
  int iStage;
  int iElement;

  stream >> nVals;
  if (nVals > 0) {
    for (iStage = 0; iStage < TimeStepSize(); iStage++) {
      for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
        const geo::IElement &elm = Mesh().Mesh().Element(iElement);
        if (IsFaultElement(elm)) {
          for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
            double dVal;
            stream >> dVal;
            if (m_pCache)
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin), iNode, dVal);
            progress.Step();
            nVals--;
          }
        }
      }
    }
  }
}

void CResultRegister::LoadFaultVectorToCache(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress,
                                             const CAnalysisType &antype, int iBegin) {
  int nVals;
  int iStage;
  int iElement;

  stream >> nVals;
  if (nVals > 0) {
    for (iStage = 0; iStage < TimeStepSize(); iStage++) {
      for (iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
        const geo::IElement &elm = Mesh().Mesh().Element(iElement);
        if (IsFaultElement(elm)) {
          for (int iNode = 0; iNode < elm.NrOfPoints(); iNode++) {
            geo::CVector vec = LoadVector(stream, version);
            if (m_pCache && !vec.Empty()) {
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin), iNode, vec.X());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 1), iNode, vec.Y());
              Cache().RawCacher().Value(iElement, ColumnNumber(antype, iStage, iBegin + 2), iNode, vec.Z());
            }
            progress.Step();
            nVals--;
          }
        }
      }
    }
  }
}

void CResultRegister::LoadOldFaultResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Linear stuff
  // fault pressure
  LoadFaultDoubleToCache(stream, progress, CAnalysisType::AT_LINEAR, CI_POREPRES);

  // fault stress
  LoadFaultVectorToCache(stream, version, progress, CAnalysisType::AT_LINEAR, CI_STRESS_XX);

  // fault relative displacement
  LoadFaultVectorToCache(stream, version, progress, CAnalysisType::AT_LINEAR, CI_STRAIN_XX);

  // Non Linear stuff
  // fault pressure
  LoadFaultDoubleToCache(stream, progress, CAnalysisType::AT_NONLIN, CI_POREPRES);

  // fault stress
  LoadFaultVectorToCache(stream, version, progress, CAnalysisType::AT_NONLIN, CI_STRESS_XX);

  // fault relative displacement
  LoadFaultVectorToCache(stream, version, progress, CAnalysisType::AT_NONLIN, CI_STRAIN_XX);
}

void CResultRegister::LoadCacheResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress,
                                       const CAnalysisType &antype) {
  bool bIgnore = false;
  CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
  IModelDiagnosticsHook *pDiagHook = 0;
  const IModelDiagnostics *pDiagnostics = 0;

  if (pModel && ((pDiagnostics = pModel->GetDiagnosticsStrategy()))) {
    bIgnore =
        pDiagnostics->Setting(gm_diagnostics::Setting_LoadModelNoResults) ||
        (!pModel->parentModel() && pDiagnostics->Setting(gm_diagnostics::Setting_LoadMainModelNoResults)) ||
        (pModel->parentModel() && (dynamic_cast<CWellZoomInModel *>(pModel)
                                       ? pDiagnostics->Setting(gm_diagnostics::Setting_LoadWellZoominModelNoResults)
                                       : pDiagnostics->Setting(gm_diagnostics::Setting_LoadZoominModelNoResults)));

    if ((pDiagHook = pDiagnostics->Hook(gm_diagnostics::Hook_ResultRegister_LoadCacheResults)))
      pDiagHook->start(pModel);
  }

  const int *indices = 0;

  switch (antype.AnalysisType()) {
  case CAnalysisType::AT_LINEAR:
    progress.StatusMessage("Loading Linear results");
    indices = linear_indices;
    break;
  case CAnalysisType::AT_NONLIN:
    progress.StatusMessage("Loading Nonlinear results");
    indices = nonlin_indices;
    break;
  case CAnalysisType::AT_HEAT:
    progress.StatusMessage("Loading Heat Flow results");
    indices = heat_indices;
    break;
  case CAnalysisType::AT_MIXTURE:
    progress.StatusMessage("Loading Mixture results");
    indices = mixture_indices;
    break;
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
    progress.StatusMessage("Loading Mixture with Containment results");
    indices = mixture_indices;
    break;
  default:
    assert(false);
    break;
  }

  if (version >= CStreamVersion(4, 1, 47)) {
    mdc::CMeshDataCacher *pCacherToLoad = &Cache().RawCacher();

    std::vector<std::pair<int, int>> bodies;
    std::vector<std::pair<int, int>> faults;

    Mesh().Mesh().GetElementTypeRanges(bodies, faults);

    uchar buffer[CHECKSUM_BUFFER_MAX];

    std::set<int> wrongChecksumStages;
    std::set<int> wrongChecksumResults;

    int nSkip = 0;
    for (int iIndex = 0; indices[iIndex] != -1; iIndex += 2) {
      if (indices[iIndex + 1] & PLAIN) {
        for (int iRange = 0; iRange < bodies.size(); ++iRange) {
          nSkip += bodies[iRange].second - bodies[iRange].first;
        }
      }
      if (indices[iIndex + 1] & FAULT) {
        for (int iRange = 0; iRange < faults.size(); ++iRange) {
          nSkip += faults[iRange].second - faults[iRange].first;
        }
      }
    }

    for (int iStage = 0; iStage < TimeStepSize(); ++iStage) {
      int nHaveResults;
      stream >> nHaveResults;

      if (!nHaveResults)
        continue;

      if (stream.IsHDF5()) {
        progress.Step(nSkip);
        continue;
      }

      for (int iIndex = 0; indices[iIndex] != -1; iIndex += 2) {
        int iColumn = ColumnNumber(antype, iStage, indices[iIndex]);

        bool bExpectChecksum = false;

        if (indices[iIndex + 1] & PLAIN) {
          for (int iRange = 0; iRange < bodies.size(); ++iRange) {
            for (int iElement = bodies[iRange].first; iElement < bodies[iRange].second; ++iElement) {
              int nAvailable;
              stream >> nAvailable;

              if (nAvailable) {
                bExpectChecksum = true;

                const geo::IElement &elm = Mesh().Mesh().Element(iElement);

                for (int iNode = 0; iNode < elm.NrOfPoints(); ++iNode) {
                  double dValue;
                  stream >> dValue;

                  if (!bIgnore)
                    pCacherToLoad->Value(iElement, iColumn, iNode, dValue);
                }
              }

              progress.Step();
            }
          }
        }

        if (indices[iIndex + 1] & FAULT) {
          for (int iRange = 0; iRange < faults.size(); ++iRange) {
            for (int iElement = faults[iRange].first; iElement < faults[iRange].second; ++iElement) {
              int nAvailable;
              stream >> nAvailable;

              if (nAvailable) {
                bExpectChecksum = true;

                const geo::IElement &elm = Mesh().Mesh().Element(iElement);

                for (int iNode = 0; iNode < elm.NrOfPoints(); ++iNode) {
                  double dValue;
                  stream >> dValue;

                  if (!bIgnore)
                    pCacherToLoad->Value(iElement, iColumn, iNode, dValue);
                }
              }

              progress.Step();
            }
          }
        }

        if (bExpectChecksum) {
          char c;

          for (int i = 0; i < 4; ++i) {
            stream >> c;
            buffer[i] = (uchar)c;
          }

          unsigned int size = *(unsigned int *)buffer;
          assert(size < CHECKSUM_BUFFER_MAX);

          for (int i = 4; i < size; ++i) {
            stream >> c;
            buffer[i] = (uchar)c;
          }

          pCacherToLoad->SetChecksum(iColumn, buffer);

          if (!pCacherToLoad->ChecksumOK(iColumn)) {
            wrongChecksumStages.insert(iStage);
            wrongChecksumResults.insert(iColumn);
          }
        }
      }
    }

    if (wrongChecksumStages.size() > 0)
      showChecksumErrors(wrongChecksumStages, wrongChecksumResults);
  } else {
    for (int iStage = 0; iStage < TimeStepSize(); iStage++) {
      for (int iElement = 0; iElement < Mesh().Mesh().ElementSize(); iElement++) {
        const geo::IElement &elm = Mesh().Mesh().Element(iElement);

        bool elmIsPlainElement = IsPlainElement(elm);
        if (elmIsPlainElement) {
          if (antype == CAnalysisType::AT_LINEAR)
            indices = linear_plain_indices;
          else if (antype == CAnalysisType::AT_NONLIN)
            indices = nonlin_plain_indices;
          else if (antype == CAnalysisType::AT_HEAT)
            indices = heat_plain_indices;
          else if (antype == CAnalysisType::AT_MIXTURE)
            indices = mixture_plain_indices;
          else if (antype == CAnalysisType::AT_MIXTURE_CONTAINMENT)
            indices = mixture_plain_indices;
        } else {
          if (antype == CAnalysisType::AT_LINEAR)
            indices = linear_fault_indices;
          else if (antype == CAnalysisType::AT_NONLIN)
            indices = nonlin_fault_indices;
          else if (antype == CAnalysisType::AT_HEAT)
            indices = heat_fault_indices;
          else if (antype == CAnalysisType::AT_MIXTURE)
            indices = mixture_fault_indices;
          else if (antype == CAnalysisType::AT_MIXTURE_CONTAINMENT)
            indices = mixture_fault_indices;
        }

        int i;
        for (i = 0; indices[i] != -1; i++) {
          if (version < CStreamVersion(3, 0, 88)) {
            // temperature are new since this version, don't load them
            if (indices[i] == CI_TEMPERATURE)
              continue;
          }

          if (version < CStreamVersion(3, 0, 102)) {
            // preconsolidation is new since this version, don't load before it
            if (indices[i] == CI_PRECON)
              continue;
          }

          if (version < CStreamVersion(3, 7, 13)) {
            // mixture results were not saved before 3.7.13
            if (antype == CAnalysisType::AT_MIXTURE && indices[i] != -1)
              continue;
          }

          if (version < CStreamVersion(4, 1, 22)) {
            // mixture containment results were not saved before 4.1.22
            if (antype == CAnalysisType::AT_MIXTURE_CONTAINMENT && indices[i] != -1)
              continue;
          }

          bool bLoadValue;
          if (CStreamVersion(3, 0, 72) < version) {
            int navailable;
            stream >> navailable;
            bLoadValue = (navailable != 0);
          } else {
            bLoadValue = true;
          }

          if (bLoadValue) {
            int iNode;
            int nNrOfNodes = elm.NrOfPoints();
            if ((CStreamVersion(3, 0, 74) > version) && dynamic_cast<const geo::CInterfaceElement *>(&elm))
              nNrOfNodes = 4;

            mdc::CMeshDataCacher &rawCacher = Cache().RawCacher();

            bool doIncrementCacheValue = (elmIsPlainElement && antype == CAnalysisType::AT_NONLIN &&
                                          indices[i] >= CI_CSTRAIN_XX && indices[i] < CI_CSTRAIN_XZ);

            for (iNode = 0; iNode < nNrOfNodes; iNode++) {
              double dVal;
              stream >> dVal;

              if (m_pCache && !bIgnore) {
                if (doIncrementCacheValue)
                  IncrementCacheValue(iElement, CAnalysisType::AT_NONLIN, iStage,
                                      CI_PSTRAIN_XX + (indices[i] - CI_CSTRAIN_XX), iNode, dVal); // add to plastic
                else
                  rawCacher.Value(iElement, ColumnNumber(antype, iStage, indices[i]), iNode, dVal);
              }
            }
          }
        }

        progress.Step();
      }
    }
  }

  if (pDiagHook)
    pDiagHook->end(pModel);
}

void CResultRegister::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  progress.StatusMessage("Loading results");

  ClearAll(); // Reset result register

  if (version < CStreamVersion(3, 0, 3)) {
    // we need to process the stream ...
    load_old_stream(stream, version, progress);
    // ... but we delete the results, recalc is necessary
    ClearAll();
  } else {
    if (version < CStreamVersion(3, 0, 41)) {
      // the old vectors from before the cache ...

      // Linear stuff
      bool bLinear = LoadOldLinearBodyResults(stream, progress);

      // Non-linear stuff
      bool bNonlinear = LoadOldNonlinearBodyResults(stream, version, progress);

      // Load interface elements when 3.0.9
      if (CStreamVersion(3, 0, 8) < version) {
        LoadOldFaultResults(stream, version, progress);
      }

      if (!bLinear)
        ClearLinear(false);
      if (!bNonlinear)
        ClearNonLinear(false);
    } else {
      // load cache
      int i;

      if (version < CStreamVersion(3, 0, 84)) {
        // either all or no results are available
        int iLinear;
        stream >> iLinear;
        if (iLinear != 0) {
          for (i = 0; i < TimeStepSize(); ++i)
            m_stAvailableLinearResults.insert(i);
        }
        //				m_bLinear = (iLinear != 0);
      } else {
        int iSize;
        stream >> iSize;
        for (i = 0; i < iSize; ++i) {
          int index;
          stream >> index; // index of depletion stage for which results are available
          m_stAvailableLinearResults.insert(index);
        }
      }

      if (!m_stAvailableLinearResults.empty()) {
        if (!m_pCache)
          CreateCache();
        // load the linear results
        LoadCacheResults(stream, version, progress, CAnalysisType::AT_LINEAR);
      }

      if (version < CStreamVersion(3, 0, 84)) {
        // either all or no results are available
        int iNonlinear;
        stream >> iNonlinear;
        if (iNonlinear != 0) {
          for (i = 0; i < TimeStepSize(); ++i)
            m_stAvailableNonlinResults.insert(i);
        }
        //				m_bNonlinear = (iNonlinear != 0);
      } else {
        int iSize;
        stream >> iSize;
        for (i = 0; i < iSize; ++i) {
          int index;
          stream >> index;
          m_stAvailableNonlinResults.insert(index);
        }
      }

      if (!m_stAvailableNonlinResults.empty()) {
        if (!m_pCache)
          CreateCache();
        // load the non-linear results
        LoadCacheResults(stream, version, progress, CAnalysisType::AT_NONLIN);
      }

      if (version > CStreamVersion(3, 0, 116)) {
        // load heat flow results
        int iSize;
        stream >> iSize;
        for (i = 0; i < iSize; ++i) {
          int index;
          stream >> index;
          m_stAvailableHeatResults.insert(index);
        }

        if (!m_stAvailableHeatResults.empty()) {
          if (!m_pCache)
            CreateCache();
          LoadCacheResults(stream, version, progress, CAnalysisType::AT_HEAT);
        }

        // load mixture results
        stream >> iSize;
        for (i = 0; i < iSize; ++i) {
          int index;
          stream >> index;
          m_stAvailableMixtureResults.insert(index);
        }

        if (!m_stAvailableMixtureResults.empty()) {
          if (!m_pCache)
            CreateCache();
          LoadCacheResults(stream, version, progress, CAnalysisType::AT_MIXTURE);
        }
      }

      if (version >= CStreamVersion(4, 1, 22)) {
        // load mixture containment results
        int iSize;
        stream >> iSize;
        for (i = 0; i < iSize; ++i) {
          int index;
          stream >> index;
          m_stAvailableMixtureContainmentResults.insert(index);
        }

        if (!m_stAvailableMixtureContainmentResults.empty()) {
          if (!m_pCache)
            CreateCache();
          LoadCacheResults(stream, version, progress, CAnalysisType::AT_MIXTURE_CONTAINMENT);
        }
      }
    }

    // The mesh could be cleared, because of incompatibillity reasons
    // if the mesh is cleared, clear also the results otherwise build
    // a dispatch vector if we results
    if (!Mesh().IsMesh())
      ClearAll();
  }

  if (CStreamVersion(3, 0, 72) < version) {
    int import;
    stream >> import;
    // Import is not used anymore....
  }

  if (!m_pCache)
    CreateCache(); // wjrx mantis 2594

  if (m_pCache)
    Cache().InvalidateStorage();
}

void CResultRegister::load_old_stream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Clear all results
  ClearAll();

  // Build the cohesion friction map
  //	void BuildCohesionFrictionMap(0);	// No progress for this old version

  // Inform user of reading these results
  progress.StatusMessage("Loading results");

  // Load number of volumetric elements first
  int nVolumeElementSize;
  stream >> nVolumeElementSize;

  for (int nElement = 0; nElement < nVolumeElementSize; nElement++) {
    const geo::IElement &element = Mesh().Mesh().Element(nElement);

    // Load a new result for the current element
    load_old_result(stream, version, progress, element, true);
    load_old_result(stream, version, progress, element, false);
  }
}

void CResultRegister::load_old_result(TSTREAM &stream, const CStreamVersion &version, TPROGRESS &progress,
                                      const geo::IElement &element, bool bLinear) {
  // Did we get the correct element?
  assert(&Mesh().Mesh() == element.IndexingElementSet());

  // Load nr of time steps ... Should be equal to our timestep size extracted from the model
  int nTimeStepSize;
  stream >> nTimeStepSize;

  assert(nTimeStepSize == TimeStepSize() || nTimeStepSize == 0);

  // Walk over time steps and fetch result
  for (int nTimeStep = 0; nTimeStep < nTimeStepSize; nTimeStep++) {
    // Save nr of nodes
    int nNodeSize;
    stream >> nNodeSize;
    for (int nNode = 0; nNode < nNodeSize; nNode++) {
      load_old_geo_result(stream, version, element, nNode, nTimeStep, bLinear);
    }
    progress.Step();
  }
}

void CResultRegister::load_old_geo_result(TSTREAM &stream, const CStreamVersion &version, const geo::IElement &element,
                                          int /*nNodeNr*/, int /*nTimeStep*/, bool /*bLinear*/) {
  // The results are not actually loaded, but we need to process the stream

  int bInterfaceElement; // Since the old type was BOOL it seemed prudent to
                         // use it's definition.
  stream >> bInterfaceElement;
  double dPorePressure;

  if (bInterfaceElement) {
    assert(element.IndexingElementSet() == &Mesh().Mesh());

    // We read the interface element, do support it ... Recalc is the best thing to do
    LoadVector(stream, version); // Load stress
    LoadVector(stream, version); // Load strain
    stream >> dPorePressure;
  } else {
    // Load stress, strain and pore pressure
    LoadTensor(stream);          // Load stress
    LoadTensor(stream);          // Load strain
    LoadVector(stream, version); // Load displacements
    stream >> dPorePressure;
  }
}

void CResultRegister::SaveCacheResults(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType &antype) {
  mdc::CMeshDataCacher *pCacherToSave = &Cache().RawCacher();
  if (Model().getSavingSmoothedResults()) {
    assert(Cache().HasAveragedCache());
    pCacherToSave = Cache().AveragedCacher();
  }

  const int *indices = 0;

  switch (antype.AnalysisType()) {
  case CAnalysisType::AT_LINEAR:
    progress.StatusMessage("Saving Linear results");
    indices = linear_indices;
    break;
  case CAnalysisType::AT_NONLIN:
    progress.StatusMessage("Saving Nonlinear results");
    indices = nonlin_indices;
    break;
  case CAnalysisType::AT_HEAT:
    progress.StatusMessage("Saving Heat Flow results");
    indices = heat_indices;
    break;
  case CAnalysisType::AT_MIXTURE:
    progress.StatusMessage("Saving Mixture results");
    indices = mixture_indices;
    break;
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
    progress.StatusMessage("Saving Mixture with Containment results");
    indices = mixture_indices;
    break;
  default:
    assert(false);
    break;
  }

  std::vector<std::pair<int, int>> bodies;
  std::vector<std::pair<int, int>> faults;

  Mesh().Mesh().GetElementTypeRanges(bodies, faults);

  uchar buffer[CHECKSUM_BUFFER_MAX];

  std::set<int> wrongChecksumStages;
  std::set<int> wrongChecksumResults;

  CHDF5StorageInterface *HDF5StorageInterface = 0;

  if (stream.IsHDF5()) {
    HDF5StorageInterface = new CHDF5StorageInterface(*stream.GetHDF5File(), static_cast<CModelBase &>(Model()).Index(),
                                                     TimeStepSize(), NUM_COLUMNS);
  }

  for (int iStage = 0; iStage < TimeStepSize(); ++iStage) {
    int iResultsAvailable =
        ResultsAvailable(antype, static_cast<CModelBase &>(Model()).DepletionStageEntry().StageByIndex(iStage)) ? 1 : 0;
    stream << iResultsAvailable;

    if (!iResultsAvailable)
      continue;

    for (int iIndex = 0; indices[iIndex] != -1; iIndex += 2) {
      int iColumn = ColumnNumber(antype, iStage, indices[iIndex]);

      if (stream.IsHDF5()) {
        pCacherToSave->SaveCell(iColumn, HDF5StorageInterface);
        continue;
      }

      bool bHaveResults = false;

      if (indices[iIndex + 1] & PLAIN) {
        for (int iRange = 0; iRange < bodies.size(); ++iRange) {
          for (int iElement = bodies[iRange].first; iElement < bodies[iRange].second; ++iElement) {
            const geo::IElement &elm = Mesh().Mesh().Element(iElement);

            const double *pResults = pCacherToSave->Value(iElement, iColumn);

            if (pResults) {
              bHaveResults = true;
              stream << 1;

              for (int iNode = 0; iNode < elm.NrOfPoints(); ++iNode)
                stream << pResults[iNode];
            } else
              stream << 0;

            progress.Step();
          }
        }
      }

      if (indices[iIndex + 1] & FAULT) {
        for (int iRange = 0; iRange < faults.size(); ++iRange) {
          for (int iElement = faults[iRange].first; iElement < faults[iRange].second; ++iElement) {
            const geo::IElement &elm = Mesh().Mesh().Element(iElement);

            const double *pResults = pCacherToSave->Value(iElement, iColumn);

            if (pResults) {
              bHaveResults = true;
              stream << 1;

              for (int iNode = 0; iNode < elm.NrOfPoints(); ++iNode)
                stream << pResults[iNode];
            } else
              stream << 0;

            progress.Step();
          }
        }
      }

      if (bHaveResults) {
        if (!pCacherToSave->ChecksumOK(iColumn)) {
          wrongChecksumStages.insert(iStage);
          wrongChecksumResults.insert(iColumn);
        }

        pCacherToSave->GetChecksum(iColumn, buffer);

        unsigned int size = *(unsigned int *)buffer;
        assert(size < CHECKSUM_BUFFER_MAX);

        for (int i = 0; i < size; ++i)
          stream << (char)buffer[i];
      }
    }
  }

  if (HDF5StorageInterface)
    delete HDF5StorageInterface;

  if (wrongChecksumStages.size() > 0)
    showChecksumErrors(wrongChecksumStages, wrongChecksumResults);
}

void CResultRegister::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  progress.StatusMessage("Saving Results");

  TAvailableResultsSet::iterator it;

  //	stream << (Linear() ? 1 : 0);
  stream << (int)m_stAvailableLinearResults.size();
  for (it = m_stAvailableLinearResults.begin(); it != m_stAvailableLinearResults.end(); ++it)
    stream << *it;
  // linear
  if (Linear())
    SaveCacheResults(stream, progress, CAnalysisType::AT_LINEAR);

  //	stream << (NonLinear() ? 1 : 0);
  stream << (int)m_stAvailableNonlinResults.size();
  for (it = m_stAvailableNonlinResults.begin(); it != m_stAvailableNonlinResults.end(); ++it)
    stream << *it;
  // non-linear
  if (NonLinear())
    SaveCacheResults(stream, progress, CAnalysisType::AT_NONLIN);

  stream << (int)m_stAvailableHeatResults.size();
  for (it = m_stAvailableHeatResults.begin(); it != m_stAvailableHeatResults.end(); ++it)
    stream << *it;
  if (Heat())
    SaveCacheResults(stream, progress, CAnalysisType::AT_HEAT);

  stream << (int)m_stAvailableMixtureResults.size();
  for (it = m_stAvailableMixtureResults.begin(); it != m_stAvailableMixtureResults.end(); ++it)
    stream << *it;
  if (Mixture())
    SaveCacheResults(stream, progress, CAnalysisType::AT_MIXTURE);

  stream << (int)m_stAvailableMixtureContainmentResults.size();
  for (it = m_stAvailableMixtureContainmentResults.begin(); it != m_stAvailableMixtureContainmentResults.end(); ++it)
    stream << *it;
  if (MixtureContainment())
    SaveCacheResults(stream, progress, CAnalysisType::AT_MIXTURE_CONTAINMENT);

  // Results are imported or not. We don't make difference right now.
  stream << 0;
}

void CResultRegister::SaveStressTensorVec(TSTREAM &stream, TPROGRESS &progress,
                                          const std::vector<CStressTensor> &vcTensor) {
  // Save nr of tensors
  int nTensorSize = vcTensor.size();
  stream << nTensorSize;
  for (size_t i = 0; i < vcTensor.size(); i++) {
    SaveTensor(stream, vcTensor[i]);
    progress.Step();
  }
}

void CResultRegister::SaveStrainTensorVec(TSTREAM &stream, TPROGRESS &progress,
                                          const std::vector<CStrainTensor> &vcTensor) {
  // Save nr of tensors
  int nTensorSize = vcTensor.size();
  stream << nTensorSize;
  for (size_t i = 0; i < vcTensor.size(); i++) {
    SaveTensor(stream, vcTensor[i]);
    progress.Step();
  }
}

void CResultRegister::LoadStressTensorVec(TSTREAM &stream, TPROGRESS &progress, std::vector<CStressTensor> &vcTensor) {
  // Load nr of tensors
  int nTensorSize;
  stream >> nTensorSize;
  vcTensor.resize(nTensorSize);
  for (int i = 0; i < nTensorSize; i++) {
    vcTensor[i] = CStressTensor(LoadTensor(stream));
    progress.Step();
  }
}

void CResultRegister::LoadStrainTensorVec(TSTREAM &stream, TPROGRESS &progress, std::vector<CStrainTensor> &vcTensor) {
  // Load nr of tensors
  int nTensorSize;
  stream >> nTensorSize;
  vcTensor.resize(nTensorSize);
  for (int i = 0; i < nTensorSize; i++) {
    vcTensor[i] = CStrainTensor(LoadTensor(stream));
    progress.Step();
  }
}

void CResultRegister::SaveVectorVec(TSTREAM &stream, TPROGRESS &progress, const std::vector<geo::CVector> &vcVector) {
  // Load nr vectors
  int nSize = vcVector.size();
  stream << nSize;
  for (int i = 0; i < nSize; i++) {
    SaveVector(stream, vcVector[i]);
    progress.Step();
  }
}

void CResultRegister::LoadVectorVec(TSTREAM &stream, const CStreamVersion &version, TPROGRESS &progress,
                                    std::vector<geo::CVector> &vcVector) {
  int nSize;
  stream >> nSize;
  vcVector.resize(nSize);
  for (int i = 0; i < nSize; i++) {
    vcVector[i] = LoadVector(stream, version);
    progress.Step();
  }
}

void CResultRegister::SaveDoubleVec(TSTREAM &stream, TPROGRESS &progress, const std::vector<double> &vcDouble) {
  int nDoubleSize = vcDouble.size();
  stream << nDoubleSize;
  for (size_t i = 0; i < vcDouble.size(); i++) {
    stream << vcDouble[i];
    progress.Step();
  }
}

void CResultRegister::LoadDoubleVec(TSTREAM &stream, const CStreamVersion & /*version*/, TPROGRESS &progress,
                                    std::vector<double> &vcDouble) {
  int nDoubleSize;
  stream >> nDoubleSize;
  vcDouble.resize(nDoubleSize);
  for (size_t i = 0; i < vcDouble.size(); i++) {
    stream >> vcDouble[i];
    progress.Step();
  }
}

void CResultRegister::SavePointVec(TSTREAM &stream, TPROGRESS &progress, const std::vector<geo::CPoint> &vcPoint) {
  int nPointSize = vcPoint.size();
  stream << nPointSize;
  for (size_t i = 0; i < vcPoint.size(); i++) {
    SavePoint(stream, vcPoint[i]);
    progress.Step();
  }
}

void CResultRegister::LoadPointVec(TSTREAM &stream, TPROGRESS &progress, std::vector<geo::CPoint> &vcPoint) {
  int nPointSize;
  stream >> nPointSize;
  vcPoint.resize(nPointSize);
  for (int i = 0; i < nPointSize; i++) {
    vcPoint[i] = LoadPoint(stream);
    progress.Step();
  }
}

void CResultRegister::SaveTensor(TSTREAM &stream, const ITensor &tensor) {
  stream << tensor.XX();
  stream << tensor.YY();
  stream << tensor.ZZ();
  stream << tensor.XY();
  stream << tensor.XZ();
  stream << tensor.YZ();
}

CTensor CResultRegister::LoadTensor(TSTREAM &stream) {
  double xx, yy, zz, xy, yz, xz;
  stream >> xx;
  stream >> yy;
  stream >> zz;
  stream >> xy;
  stream >> xz;
  stream >> yz;
  return CTensor(xx, yy, zz, xy, xz, yz);
}

void CResultRegister::SaveVector(TSTREAM &stream, const geo::IVector &vector) {
  int bEmpty = vector.Empty() == true; // Since the old type was BOOL it seemed
                                       // prudent to use it's definition.
  stream << bEmpty;
  if (!bEmpty) {
    stream << vector.X();
    stream << vector.Y();
    stream << vector.Z();
  }
}

geo::CVector CResultRegister::LoadVector(TSTREAM &stream, const CStreamVersion &version) {
  // From version 3, 0, 19 we can have empty vectors
  if (CStreamVersion(3, 0, 18) < version) {
    int bEmpty; // Since the old type was BOOL it seemed prudent to use it's
                // definition.
    stream >> bEmpty;
    if (bEmpty) // Return an empty vector
      return geo::CVector();
  }

  double x, y, z;
  stream >> x;
  stream >> y;
  stream >> z;
  return geo::CVector(x, y, z);
}

void CResultRegister::SavePoint(TSTREAM &stream, const geo::IPoint &point) {
  stream << point.X();
  stream << point.Y();
  stream << point.Z();
}

geo::CPoint CResultRegister::LoadPoint(TSTREAM &stream) {
  double x, y, z;
  stream >> x;
  stream >> y;
  stream >> z;
  return geo::CPoint(x, y, z);
}

long CResultRegister::SavedItems() const {
  int factor = 0;
  if (Linear())
    factor += sizeof(linear_indices) / (2 * sizeof(int)) - 1;
  if (NonLinear())
    factor += sizeof(nonlin_indices) / (2 * sizeof(int)) - 1;
  if (Heat())
    factor += sizeof(heat_indices) / (2 * sizeof(int)) - 1;
  if (Mixture())
    factor += sizeof(mixture_indices) / (2 * sizeof(int)) - 1;
  if (MixtureContainment())
    factor += sizeof(mixture_indices) / (2 * sizeof(int)) - 1;

  return factor * TimeStepSize() * Mesh().Mesh().ElementSize();
}

void CResultRegister::Node(geo::IPoint &target, const CDepletionStage &stage, const CAnalysisType &antype,
                           int nNodeIndex) const {
  Node(target, stage.Index(), antype, nNodeIndex);
}

void CResultRegister::Node(geo::IPoint &target, int nTimeStep, const CAnalysisType &antype, int nNodeIndex) const {
  const geo::INode &node = Mesh().Mesh().Node(nNodeIndex);
  target = node;

  if (nTimeStep != 0) {
    assert(node.AttachedElementSize() > 0);
    const geo::IElement &element = node.AttachedElement(0);

    int i;
    for (i = 0; i < element.NrOfNodes(); i++)
      if (&element.Node(i) == &node)
        break;

    assert(i < element.NrOfNodes());

    const double *pXVal = ValuesFromCache(element.Index(), ColumnNumber(antype, nTimeStep, CI_DISPLA_X));
    const double *pYVal = ValuesFromCache(element.Index(), ColumnNumber(antype, nTimeStep, CI_DISPLA_Y));
    const double *pZVal = ValuesFromCache(element.Index(), ColumnNumber(antype, nTimeStep, CI_DISPLA_Z));
    assert(pXVal && pYVal && pZVal);
    double dx = pXVal[i];
    double dy = pYVal[i];
    double dz = pZVal[i];

    target += geo::CPoint(dx, dy, dz);
  }
}

CScalarValueSet CResultRegister::PorePressure(const CDepletionStage &stage, const CAnalysisType &antype,
                                              bool bChange) const {
  return CScalarValueSet(*this, stage, antype, CI_POREPRES, false, bChange);
}

CScalarValueSet CResultRegister::Temperature(const CDepletionStage &stage, const CAnalysisType &antype,
                                             bool bChange) const {
  return CScalarValueSet(*this, stage, antype, CI_TEMPERATURE, false, bChange);
}

CScalarValueSet CResultRegister::FaultPorePressure(const CDepletionStage &stage, const CAnalysisType &antype,
                                                   bool bChange) const {
  return CScalarValueSet(*this, stage, antype, CI_POREPRES, true, bChange);
}

CScalarValueSet CResultRegister::Consolidation(const CDepletionStage &stage, const CAnalysisType &antype,
                                               bool bChange) const {
  return CScalarValueSet(*this, stage, antype, CI_PRECON, false, bChange);
}

const double *CResultRegister::ValuesFromCache(int nElement, int nColumn) const {
  const double *vals = Cache().ActiveCacher().Value(nElement, nColumn);
  return vals;
}

void CResultRegister::TensorFromCache(ITensor &target, const CDepletionStage &stage, const CAnalysisType &antype,
                                      int nElementIndex, int nNodeIndex, int nStart) const {
  const double *pXXVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart));
  const double *pYYVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 1));
  const double *pZZVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 2));
  const double *pXYVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 3));
  const double *pYZVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 4));
  const double *pXZVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 5));

  assert(pXXVal && pYYVal && pZZVal && pXYVal && pYZVal && pXZVal);

  target.XX(pXXVal[nNodeIndex]);
  target.YY(pYYVal[nNodeIndex]);
  target.ZZ(pZZVal[nNodeIndex]);
  target.XY(pXYVal[nNodeIndex]);
  target.YZ(pYZVal[nNodeIndex]);
  target.XZ(pXZVal[nNodeIndex]);
}

void CResultRegister::VectorFromCache(geo::IVector &target, const CDepletionStage &stage, const CAnalysisType &antype,
                                      int nElementIndex, int nNodeIndex, int nStart) const {
  const double *pXVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart));
  const double *pYVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 1));
  const double *pZVal = ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nStart + 2));

  assert(pXVal && pYVal && pZVal);

  target.X(pXVal[nNodeIndex]);
  target.Y(pYVal[nNodeIndex]);
  target.Z(pZVal[nNodeIndex]);
}

CStressTensorValueSet CResultRegister::TotalStress(const CDepletionStage &stage, const CAnalysisType &antype,
                                                   bool bChange) const {
  return CStressTensorValueSet(*this, stage, antype, CI_STRESS_XX, true, bChange);
}

CStressTensorValueSet CResultRegister::EffectiveStress(const CDepletionStage &stage, const CAnalysisType &antype,
                                                       bool bChange) const {
  return CStressTensorValueSet(*this, stage, antype, CI_STRESS_XX, false, bChange);
}

CStrainTensorValueSet CResultRegister::TotalStrain(const CDepletionStage &stage, const CAnalysisType &antype,
                                                   bool bChange) const {
  assert(!bChange); // No change possible for this result
  return CStrainTensorValueSet(*this, stage, antype, CI_STRAIN_XX, false, true);
}

CStrainTensorValueSet CResultRegister::PlasticStrain(const CDepletionStage &stage, const CAnalysisType &antype,
                                                     bool bChange) const {
  assert(!bChange); // No change possible for this result
  return CStrainTensorValueSet(*this, stage, antype, CI_PSTRAIN_XX, false, true);
}

/*
CStrainTensorValueSet CResultRegister::CreepStrain(const CDepletionStage& stage, const CAnalysisType& antype, bool
bChange) const
{
  assert(!bChange); // No change possible for this result
  return CStrainTensorValueSet(*this, stage, bLinear, CI_CSTRAIN_XX, false, true, true);
}
*/

CVectorValueSet CResultRegister::FaultShearStress(const CDepletionStage &stage, const CAnalysisType &antype,
                                                  bool bChange) const {
  return CVectorValueSet(*this, stage, antype, CI_STRESS_XX, true, true, bChange);
}

CVectorValueSet CResultRegister::FaultEffectiveStress(const CDepletionStage &stage, const CAnalysisType &antype,
                                                      bool bChange) const {
  return CVectorValueSet(*this, stage, antype, CI_STRESS_XX, false, true, bChange);
}

CVectorValueSet CResultRegister::Displacement(const CDepletionStage &stage, const CAnalysisType &antype,
                                              bool bChange) const {
  assert(!bChange);
  return CVectorValueSet(*this, stage, antype, CI_DISPLA_X, false, false, true); // CI_STRAIN_XX
}

CVectorValueSet CResultRegister::FaultDisplacement(const CDepletionStage &stage, const CAnalysisType &antype,
                                                   bool /*bChange*/) const {
  return CVectorValueSet(*this, stage, antype, CI_STRAIN_XX, false, true, true); // CI_STRAIN_XX
}

/* ************************************************************************** */

double CResultRegister::MaterialParameterValue(int nElementIndex, unsigned int ValueTypeID,
                                               const CDepletionStage &stage) const {
  const geo::IElement &element = Mesh().ElementSet().Element(nElementIndex);
  const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&element);
  const geo::ITriangle *pTriangle = dynamic_cast<const geo::ITriangle *>(&element);
  const geo::CInterfaceElement *pInterface = dynamic_cast<const geo::CInterfaceElement *>(&element);
  const geo::CLineInterface *pLineInt = dynamic_cast<const geo::CLineInterface *>(&element);

  if (pBody || pTriangle) {
    const CFormationBase *pForm = Mesh().Formation(element);
    if (pForm)
      return pForm->Material(stage).Material(element).ParameterValue(ValueTypeID);

    const CWellCasingModel *pWellCasingModel = dynamic_cast<const CWellCasingModel *>(&Model());
    if (pWellCasingModel && pWellCasingModel->Mesh().IsCementElement(element))
      return pWellCasingModel->CasingCement().Material(stage).Material(element).ParameterValue(ValueTypeID);
  } else if (pInterface || pLineInt) {
    const CWellCasingModel *pWellCasingModel = dynamic_cast<const CWellCasingModel *>(&Model());
    if (pWellCasingModel && pWellCasingModel->Mesh().IsCementInterfaceElement(element))
      return pWellCasingModel->CasingNode().CementInterface().Material(stage).Material(element).ParameterValue(
          ValueTypeID);

    const CHorizonBase &horizon = *Mesh().SlipHorizon(element);
    assert(horizon.Slip());

    switch (horizon.SlipType()) {
    case CHorizonBase::SLIP:
    case CHorizonBase::FRACTURE:
      assert(false); // there is no cohesion or friction when there is complete slip
      return 0.0;
      break;
    case CHorizonBase::STICK:
    case CHorizonBase::USER: {
      const dia::IMaterial &mat = horizon.InterfaceMaterial(element, stage);
      const CInterfaceElementMaterial *pMat = dynamic_cast<const CInterfaceElementMaterial *>(&mat);
      assert(pMat);
      return pMat->ParameterValue(ValueTypeID);
    } break;
    default:
      assert(false);
    }
  }

  assert(false);
  return 0;
}

int CResultRegister::MaterialModel(int nElementIndex) const {
  const geo::IElement &element = Mesh().ElementSet().Element(nElementIndex);
  //	assert(dynamic_cast<const geo::IBody *> (&element) != 0);
  const CModelBase &model = static_cast<const CModelBase &>(Model());
  const CFormationBase *pForm = Mesh().Formation(element);
  if (pForm)
    return pForm->Material(model.InitialDepletionStage()).MaterialModel();

  const CWellCasingModel *pWellCasingModel = dynamic_cast<const CWellCasingModel *>(&model);
  if (pWellCasingModel && pWellCasingModel->Mesh().IsCementElement(element))
    return pWellCasingModel->CasingCement().Material(model.InitialDepletionStage()).MaterialModel();

  return -1;
}

/* Availability of results ********************************************************************/

bool CResultRegister::ResultAvailable(const CDepletionStage &stage, const CAnalysisType &antype, int nElementIndex,
                                      int nResultType) const {
  TAvailableResultsSet::const_iterator it = AvailableResultsSet(antype).find(stage.Index());
  if (it == AvailableResultsSet(antype).end())
    return false;

  return ValuesFromCache(nElementIndex, ColumnNumber(antype, stage.Index(), nResultType)) != 0;
}

bool CResultRegister::ResultAvailable(int nTimeStep, const CAnalysisType &antype, int nElementIndex,
                                      int nResultType) const {
  const CModelBase &model = static_cast<const CModelBase &>(Model());

  return ResultAvailable(model.DepletionStageEntry().StageByIndex(nTimeStep), antype, nElementIndex, nResultType);
}

CResultRegister::TAvailableResultsSet &CResultRegister::AvailableResultsSet(const CAnalysisType &antype) {
  const CResultRegister *constThis = this;
  const TAvailableResultsSet &stARS = constThis->AvailableResultsSet(antype);
  return const_cast<TAvailableResultsSet &>(stARS);
}

const CResultRegister::TAvailableResultsSet &CResultRegister::AvailableResultsSet(const CAnalysisType &antype) const {
  switch (antype.AnalysisType()) {
  case CAnalysisType::AT_LINEAR:
    return m_stAvailableLinearResults;
  case CAnalysisType::AT_NONLIN:
    return m_stAvailableNonlinResults;
  case CAnalysisType::AT_HEAT:
    return m_stAvailableHeatResults;
  case CAnalysisType::AT_MIXTURE:
    return m_stAvailableMixtureResults;
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
    return m_stAvailableMixtureContainmentResults;
  }

  assert(false);
  TAvailableResultsSet *pBogus = 0;
  return *pBogus;
}

bool CResultRegister::ResultsAvailable(const CAnalysisType &antype, const CDepletionStage &stage) const {
  const TAvailableResultsSet &stResults = AvailableResultsSet(antype);
  return stResults.find(stage.Index()) != stResults.end();
}

void CResultRegister::TriggerSmoothedResults() {
  // calling this method will generate the averaged cache if it doesn't exist yet
  Cache().AveragedCacher();
}

bool CResultRegister::HasSmoothedResults() const { return Cache().HasAveragedCache(); }
