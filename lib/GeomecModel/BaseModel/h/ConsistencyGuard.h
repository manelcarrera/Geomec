#ifndef _CONSISTENCY_GUARD_H_
#define _CONSISTENCY_GUARD_H_

#include <vector>
#include <deque>
#include <map>
#include <cstddef>

class CModelBase;
class CHDF5StorageInterface;
class CHDF5File;

#include "AnalysisType.h"
#include "StorageNode.h"


struct CConsistencyGuardOptions
{
  typedef enum { CGU_NEVER = 0, CGU_ALWAYS } TConsistencyGuardUsageType;
  TConsistencyGuardUsageType consistencyGuardUsage;

  typedef enum { PRC_NEVER = 0, PRC_ALWAYS } TPropagateResultClearingType;
  TPropagateResultClearingType propagateResultClearing;

  typedef enum { PWP_NEVER = 0, PWP_ALWAYS } TPropagateWellpathType;
  TPropagateWellpathType propagateWellpaths;

  typedef enum { MPS_NEVER = 0, MPS_ALWAYS } TMovePointsetType;
  TMovePointsetType movePointsets;

  typedef enum { ARE_NEVER = 0, ARE_ALWAYS } TAutoResultExportType;
  TAutoResultExportType autoResultExport;

  CConsistencyGuardOptions();
  bool operator==(const CConsistencyGuardOptions& rhs) const;
  bool operator!=(const CConsistencyGuardOptions& rhs) const;
  
  void LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& prg);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg);

  bool bLoaded;
};


class CConsistencyGuard
{
  std::vector<CModelBase *> m_models; // m_models[0] should always be the root model
  CModelBase *m_rootModel;
  CModelBase *m_currentModel;

  CConsistencyGuardOptions m_options;

  CHDF5File *m_storageFile;

  typedef std::map<int, CHDF5StorageInterface *> TStorageInterfaces;
  mutable TStorageInterfaces m_storageInterfaces;

  int m_nAttaching;
  int m_nChildLoading;
  int m_nDeleting;
  std::deque<CModelBase *> m_resultClearingOriginators;

public:

  //enum eScope{ Deformation, XSecImprovement };
  //enum eModel{ Casing, ZoomIn, Main };

public:
  CConsistencyGuard(CModelBase& rootModel);
  ~CConsistencyGuard();

  // Model management
  void AttachChildModelStart(CModelBase& model);
  void AttachChildModelFinish(CModelBase& model);
  void DetachChildModel(CModelBase& model);
  void SwitchToModel(CModelBase& model);
  void ChildIsLoading(bool loading);
  void IsDeleting(bool deleting);

  // Status management
  typedef enum { OK = 0, UNKNOWN, NEED_CLEANUP, INVALID, BROKEN } TStatus;
  TStatus Status(CModelBase& model) const;
  bool CanCalculate(CModelBase& model, CAnalysisType::TAnalysisType anyType);

  //eModel current_model_type();
  //bool is_model_in_scope( eScope scope );

  void NotifyChange(CModelBase& model);

  void Validate(CModelBase& model, bool extensive = false);
  void CleanUp(CModelBase& model, bool extensive = false);

  void GetErrors(QStringList& errors);
  void ClearErrors();

  // Options
  CConsistencyGuardOptions Options() const;
  void Options(CConsistencyGuardOptions& options);
  void LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& prg);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg);

  // HDF5File management
  void AttachHDF5File(CHDF5File *h5file, bool bClearInterfaces = false);
  QString HDF5FileName() const;
  CHDF5StorageInterface *HDF5StorageInterface(int modelIndex, int maxStages, int maxColumns) const;
  void InvalidateHDF5StorageInterface(int modelIndex);

  // Result clearing management
  void EnterResultClearing(CModelBase& model);
  void LeaveResultClearing(CModelBase& model);
  
  bool AllowResultClearing(CModelBase& model);

  typedef enum { CLEAR_LINEAR, CLEAR_NONLIN, CLEAR_HEAT, CLEAR_MIXTURE, CLEAR_MIXTURE_CONTAINMENT, CLEAR_ALL } TResultClearingType;
  void ResultClearingPropagate(CModelBase& model, TResultClearingType resultType, bool sendModified);

  void AutoResultExport(CModelBase& model, CAnalysisType::TAnalysisType antype);

  // Pointsets & Wellpaths
  bool AllowMovePointsets() const;
  void CopyAllWellpaths();

  // Notifications
  typedef enum { WRONG_CLEARANCE_ATTEMPT } TModelErrorType;
  void NotifyModelError(CModelBase& model, TModelErrorType error);

private:
  TStatus m_status;

  typedef std::map<TModelErrorType, size_t> TModelErrorEvents;
  typedef std::map<CModelBase *, TModelErrorEvents> TModelErrors;
  TModelErrors m_modelErrors;

  void SyncModelHDF5(bool bClearInterfaces);
  void ClearHDF5Interfaces();

  bool isDescendant(CModelBase& model, CModelBase& ancestor, bool strict = false);
  void getChildren(CModelBase& model, std::vector<CModelBase *>& children);
};



#endif