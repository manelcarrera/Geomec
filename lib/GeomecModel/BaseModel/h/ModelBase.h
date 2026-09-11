// ModelBase.h: interface for the CModelBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELBASE_H__313C9293_EC41_4C00_B9FB_5BF9A1632CDA__INCLUDED_)
#define AFX_MODELBASE_H__313C9293_EC41_4C00_B9FB_5BF9A1632CDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VersionManager.h"
#include "MeshBase.h"

class IDianaXWrapper;
class CAnalysisType;
//class CVersionManager;

class CDepletionStageEntry;
class CResultRegister;
class CGlobalPressure;
class CBoundaryBase;
class CGlobalInitialStressNode;
class CMeshBase;
class CResultTree;
class CGlobalTemperature;
class CFormationBase;
class CPointSet;
class CHorizonBase;
class IPointSet;
class IFormationElementSet;
class CHomogenizationBox;
class CBranchState;
class CGVTSettings;
class CQBSettings;
class CMaterialResultTree;
class ISaveModel;
class IRetrieveDianaFileNames;
class CMeshResultTree;
class IModelDiagnostics;
class CConsistencyGuard;
class CGVTResultTree;
class CGeneralDataBlock;

#define PRIMARY_RESULT 0

namespace gm {
  class CModel;
  class CFormation;
}

#include "FemAppModel.h"
#include "Acceleration.h"
#include "StorageNode.h"
#include "NewModel.h"
#include "DepletionStage.h"

class CGeomecDianaRunnerBase;
class CDianaRunController;

class IMesherDlg;

namespace geo {
  class IElement;
  class ICoordinate;
}

namespace dia {
class IAnalysisStatusContainer;
}

#include "MaterialErrorHandler.h"
#include "GeomecModelVisitor.h"
#include "ElementOrder.h"

class CModelBase : public CFemAppModel 
{
  friend class CGm3HexaModel;
public:
  typedef enum _FaultInit
  {
  FI_FULLSLIP = 0,
  FI_MOD_COHESION,
  FI_MOD_FLUIDPRESSURE
  } TFaultInit;

  typedef enum
  {
  PRD_EQUAL = 0,
  PRD_LINEAR,
  PRD_NONLIN
  } TParentResultsDef;

  class IModelLoadSave
  {
  QString m_appVersion;
  public:
  virtual ~IModelLoadSave() {}
  virtual bool LoadModel(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prog, CModelBase& model) = 0;
    virtual bool SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog, CModelBase& model) = 0;
    virtual long SavedItems(CModelBase& model) const = 0;

  virtual void SetVersionManager(CVersionManager& /*versionManager*/) {}
  virtual QString GetAppVersion() const { return m_appVersion; }
  virtual void SetAppVersion(const QString& appVersion) { m_appVersion = appVersion; }

  virtual bool SupportGMP() const { return false; }
  virtual CArchiveInterface *createCArchive(const QString& /* filename */) const { return 0; }
  };

  class CModelLoadSaveDefault : public IModelLoadSave
  {
  CVersionManager   *m_pVersionManager;
  CModelBase        *m_pParentModel;
  IModelDiagnostics *m_pModelDiagnostics;
  public:
  CModelLoadSaveDefault(CModelBase *pParentModel = 0, IModelDiagnostics *pModelDiagnostics = 0);
  CModelLoadSaveDefault(const QString& appVersion, CModelBase *pParentModel = 0, IModelDiagnostics *pModelDiagnostics = 0);

  virtual ~CModelLoadSaveDefault();
  virtual bool LoadModel(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prog, CModelBase& model);
    virtual bool SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog, CModelBase& model);
    virtual long SavedItems(CModelBase& model) const;

  virtual void SetVersionManager(CVersionManager& versionManager);
  };

private:
  class CGravityNotifier : public CGraphNode
  {
  public:
  CGravityNotifier()
  : CGraphNode("")
  {
  }

  virtual unsigned int IconId() const { return 0; }
  virtual unsigned int TypeId() const { return 0; }
  };

protected:
  IMesherDlg* m_mesher_dlg;
public:
  void mesher_dlg( IMesherDlg* val ){ m_mesher_dlg = val; };
  IMesherDlg* mesher_dlg(){ return m_mesher_dlg; };

private:
  CDepletionStageEntry* m_pDepletionStageEntry;
  CMaterialResultTree* m_pMatResultTree;
  CResultTree* m_pResultTree;
  CMeshResultTree* m_pMeshResultTree;
  CGVTResultTree* m_pGVTResultTree;
  CGlobalPressure* m_pPressure;
  CGlobalInitialStressNode* m_pGlobalStressNode;
  CGlobalTemperature *m_pGlobalTemperature;
  QString m_strProjectInfo;
  CAcceleration m_gravity;
  CGravityNotifier m_GravityNotifier; // sends a modified signal when the m_gravity values changes
  CBranchState* m_pBranchState;
//	bool m_bTimeAnalysis;
  bool m_bNoCSHE; // true when we don't want to use the enhanced assumed strain method for calculation
  CElementOrder m_nElementOrder;
  TFaultInit m_nFaultInit;
  bool m_bLargeDeform;
  bool m_bLoading;
  bool m_bInvalidateResultsAfterLoad;
  bool m_bAllowLinearCalc;

  //bool m_bLinstaSolveDirect;
  //bool m_bNonlinSolveDirect;
  bool m_bSaltStressInitRelax;

  int m_nIniStressMaxIterations;
  double m_dIniStressConvCriterion;
  bool m_bLinstaOnlyInitialStress;
  bool m_bPressureSupportNodeLoadWarningDone;
  bool m_bAverageResults;

  CGVTSettings *m_pGVTSettings;
  CQBSettings *m_pQBSettings;

  bool m_bUseParallelSolver;
  int m_nNumSolverThreads;
  bool m_bUseStressSmootheningOnDSF;
  int m_nDSFInterSurfType;
  bool m_bUseDecompactionParams;
  bool m_bAllowRedrawing;
  bool m_bForcingRedraw;
  bool m_bAutomaticallyDisableDrawing;
  bool m_bExportCommandFilesWithDefaults;
  bool m_bWriteOutputStreamFile;
  bool m_bCleanupOldBranchResults;

  bool m_bShowConvexHulls;

  IModelLoadSave* m_pChildModelLoadSave;
  CVersionManager m_versionManager;
  CNewModel m_NewModel;
  int m_nIndex;
  CModelBase* m_pParentModel;
  bool m_bSourceDepletionStagesChanged;
  bool m_bSourceGlobalPressureChanged;
  bool m_bSourceGlobalTemperatureChanged;
  bool m_bSourceGravityChanged;
  TParentResultsDef m_ParentResultsDef;
  bool m_EnableResultCache;
  int m_ResultCacheSize;

  // options for convergence in nonlinear analyses
  bool m_bConvergenceChecks; // execute these checks or not
  bool m_bConvergenceMinimumAngle; // disable nonlinearity when too small angle
  double m_dConvergenceMinimumAngle; // the minimum angle
  bool m_bConvergenceMinimumRatio; // disable nonlinearity when too small edge ratio
  double m_dConvergenceMinimumRatio; // the minimum edge ratio
  bool m_bConvergenceMinimumVolume; // disable nonlinearity when too low element volume
  double m_dConvergenceMinimumVolume; // the minimum element volume

  bool m_bDefaultDianaZoominSetting; // for testing purposes, not persistent, default false; if we set this to true in main model, and 1st dep stage to secant, we can force Diana crash

  const IModelDiagnostics *m_pModelDiagnostics;
  CConsistencyGuard *m_pConsistencyGuard;

protected:
  bool HeatFlowCalculationCriteriaMet() const;

  // Save function (STREAM)
  long SavedItemsPointSets() const;
  void SavePointSets(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& progress) const;

  // Load functions (STREAM)
  void LoadPointSets(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& progress);
  void EnablePointSets();

  long SavedItemsSurfaces() const;
  void SaveSurfaces(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
  void LoadSurfaces(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);

  long SavedItemsRockMaterials() const;
  void SaveRockMaterials(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
  void LoadRockMaterials(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);

  // Generic copy functions
  void CopyDepletionStages(const CDepletionStageEntry& source, CDepletionStageEntry& target, CStorageNode::TPROGRESS& progress);
  void CopyHomogenizationBox(const CHomogenizationBox &source,CHomogenizationBox &target);
  void CopyMaterial(const CModelBase& source, CStorageNode::TPROGRESS& progress);
  void CopyFormation(const CFormationBase& source, CFormationBase& target);
  void CopyFormationElementSet(const IFormationElementSet& source, IFormationElementSet& target);
  void CopyHorizon(const CHorizonBase& source, CHorizonBase& target);
  void CopyInitialStressAndPressure(const CModelBase& source);
  void CopyTemperature(const CModelBase& source);
  bool m_bIsMeshed;
  virtual CDepletionStage::eIterationScheme DefaultIterationScheme(const CDepletionStage& source_stage) const;
  void LoadZoominProperties(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  virtual bool DeleteEntry(const int nEntryType);

public:
  bool m_show_cm2_dlg;

private:
  void FindParentFormations();

public:
  void SetDiagnosticsStrategy(const IModelDiagnostics *pModelDiagnostics);
  const IModelDiagnostics *GetDiagnosticsStrategy();

  CConsistencyGuard *GetConsistencyGuard();

  void LoadMaterial(const QString &sPath, IProgressBase &progress);

protected:
  virtual void CreateDensityPoints(const geo::IElement &element, CPointSet &pointset) = 0;

public:
  void DoDensityCorrection();

protected:
  CBoundaryBase* m_pBoundary;

  // create model-specific DIANA runner object
  // the method will allocated the pointer, ownership is transferred to the calling routine
  virtual CGeomecDianaRunnerBase* OnCreateDianaRunner(CDianaRunController& controller) = 0;

  virtual bool IsHexaModel() const { return false; }
  virtual bool OnModelSpecificAnalysisCheck(const CAnalysisType& antype, bool bWriteInputFiles, bool bWriteQuadDat);

public:
  virtual bool IsCylindricalModel() const { return false; }
  
  // The filos relationship between nodes and elements
  typedef std::vector<int> TNodeVec;
  typedef std::pair<const geo::IElement*, TNodeVec> TFilosElement;
  typedef std::map<unsigned int, TFilosElement> TFilosElementMap;

  // returns the depth component of the given coordinate
  virtual const double &Depth(const geo::ICoordinate &coord) const = 0;
  virtual double Northing(const geo::ICoordinate& coord) const = 0;
  virtual double Easting(const geo::ICoordinate& coord) const = 0;

  bool WriteFilosModel(const QString& getPathName,
             IDianaXWrapper* dianaXWrapper,
             const std::string &title, 
             const CAnalysisType& antype, 
             bool bWriteInputFiles, 
             bool bWriteQuadDat,
             ISaveModel& saveModel,
             IRetrieveDianaFileNames& retrieveDianaFileNames,
             dia::IAnalysisStatusContainer *pAnalysisStatusContainer = 0);

  bool CheckForAnalysis(const CAnalysisType& antype,
                    bool bWriteInputFiles,
                  bool bWriteQuadDat);

//	bool TimeAnalysis() const;
//	void TimeAnalysis(bool bTimeAnalysis);
  bool NoCSHE() const {return m_bNoCSHE;} 
  void NoCSHE(bool bNoCSHE);
  bool LargeDeformations() const;
  void LargeDeformations(bool bLargeDeform, bool bUpdateResults = true);
  bool AllowLinearCalc() const;
  void AllowLinearCalc(bool bAllowLinearCalc);

  bool UseParallelSolver() const;
  void UseParallelSolver(bool bOn);
  int NumSolverThreads() const;
  void NumSolverThreads(int nThreads);
  int MaxNumSolverThreads() const;
  bool UseStressSmootheningOnDSF() const;
  void UseStressSmootheningOnDSF(bool bOn);

  int DSFIntermediateSurfaceType() const;
  void DSFIntermediateSurfaceType(int type);

  CElementOrder::TElementOrder ElementOrder() const;
  void ElementOrder(CElementOrder::TElementOrder nElementOrder);

  TFaultInit FaultInit() const { return m_nFaultInit; }
  void FaultInit(TFaultInit nFaultInit);
//	virtual bool ValidateForAnalysis() const = 0;

  virtual bool SaltInitRelax() const { return m_bSaltStressInitRelax; }
  void SaltInitRelax(bool bRelax) { m_bSaltStressInitRelax = bRelax; }
  virtual bool CanDisableSaltInitRelax() const { return true; }

  virtual bool CanUseCalculatedTemperatures() const;

  const CGlobalInitialStressNode& GlobalInitialStress() const;
  CGlobalInitialStressNode& GlobalInitialStress();
  const CGlobalPressure& GlobalPressure() const;
  CGlobalPressure& GlobalPressure();
  const CGlobalTemperature &InitialTemperature() const;
  CGlobalTemperature &InitialTemperature();
  // Gradients ...
  //##ModelId=3BC55D6102A5
  void ProjectInfo(const QString & strProjectInfo);
  //##ModelId=3BC55D6102A7
  const QString& ProjectInfo() const;
  //##ModelId=3BC55D6102A9
protected:
  CModelBase(CAnalysisLogger& logger, const CVersionManager& versionManager);
public:

  virtual CStreamVersion documentVersion() const;
  virtual CStreamVersion currentVersion() const;

  bool LoadPre(CStorageNode::TSTREAM &stream, CStorageNode::TSTREAM** ppcstream, CStreamVersion &version, CStorageNode::TPROGRESS &prog, bool bLoadHeader);
  bool Load(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog, bool bLoadHeader);
  bool LoadPost(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  int LoadSceneMarker(CStorageNode::TSTREAM& stream, CStreamVersion& version);
  bool Save(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog, bool bSaveHeader);
  void SaveFileHeader(CStorageNode::TSTREAM &stream,
    const QString& currentAppVersion);
  void SaveSceneMarker(CStorageNode::TSTREAM& stream, bool marker);

  virtual bool OnLoad(CStorageNode::TSTREAM &/*stream*/, CStreamVersion &/*version*/, CStorageNode::TPROGRESS& /*prog*/){ return true;}
  virtual bool OnSave(CStorageNode::TSTREAM &/*stream*/, CStorageNode::TPROGRESS& /*prog*/){ return true;}
  virtual long SavedItems() const;

  bool LoadChildModel(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog, CModelBase& childModel);
  bool SaveChildModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog, CModelBase& childModel);
  long SavedItemsChildModel(CModelBase& childModel) const;

  // Numbering before saving
  void NumberOpenGLNodes();
  void NumberValueComposites();
  void NumberModels();
private:
  int MaxModelIndex();
  void NumberModels(int& nIndex);
public:
  void RemoveModelNumbering();

  virtual void CreatePointSet() const; // wjrx mantis 2896
        
  //##ModelId=3BC55D6102AB
protected:
  friend class IModelLifetimeFacade;

  virtual ~CModelBase();
  virtual void destroy();

  virtual void CloseModel();

public:
  virtual QString documentType() const { return ""; }

  // Acces to  the gravity
  const CAcceleration& Gravity() const;
  void Gravity(const double& value, const CAcceleration::UNIT = CAcceleration::SI_UNIT);
  CGraphNode& GravityNotifier();

  // Access to GVTSettings
  const CGVTSettings &GVTSettings() const;
  CGVTSettings &GVTSettings();

  // Access to QBSettings
  const CQBSettings &QBSettings() const;
  CQBSettings &QBSettings();

  // Access to the pressure gradient

  // First timestep
  const CDepletionStage& InitialDepletionStage() const;
  CDepletionStage& InitialDepletionStage();

  const CDepletionStage& LastDepletionStage() const;
  CDepletionStage& LastDepletionStage();

  const CDepletionStageEntry& DepletionStageEntry() const;
  CDepletionStageEntry& DepletionStageEntry();

  int NrOfDepletionStages() const;

    const CDepletionStage* FindDepletionStage
          (int timeStepIndex) const;
  // Boundary
  CBoundaryBase& Boundary();
  const CBoundaryBase& Boundary() const;

  static const std::set<unsigned int>& DataStorageEntryTypes();
  virtual CGraphEntry* GraphEntry(const int nEntryType);
  virtual const CGraphEntry* GraphEntry(const int nEntryType) const;

  // This routine only creates the entries for the graph
  virtual void createContainers();
  // This routine creates all default items we want to have.
  virtual void createDefaults();
  virtual void OnExit();
  virtual void DestroyCaches();
  virtual void OnCloseModel();

  // The result register contains the basic results
  virtual int ResultRegisterSize() const;
  virtual QString ResultRegisterName(int nIndex) const;
  virtual	CResultRegister& ResultRegister(int nRegister = PRIMARY_RESULT);
  virtual const CResultRegister& ResultRegister(int nRegister = PRIMARY_RESULT) const;

  // The resulttree represents the resulttree in the tree control. Derived results can
  // be accessed from here
  CMaterialResultTree& MaterialResultTree();
  const CMaterialResultTree& MaterialResultTree() const;
  CResultTree& ResultTree();
  const CResultTree& ResultTree() const;
  CMeshResultTree& MeshResultTree();
  const CMeshResultTree& MeshResultTree() const;
  CGVTResultTree& GVTResultTree();
  const CGVTResultTree& GVTResultTree() const;

  // Mesh/state methods.
  virtual bool HasFormations() const;
  virtual bool HasHorizons() const;
  virtual bool HasFaults() const;
  virtual bool HasDoubleSidedFaults() const;
  virtual bool CanCreateMesh();
  virtual bool IsMesh() const;
  virtual bool CanCalculate() const;
  virtual bool CanCalculateHeatFlow() const;
  virtual bool CanCalculateMixture() const;
  virtual bool IsResult() const = 0;

  virtual void CreateMesh();
  virtual void Calculate() = 0;
  virtual void InvalidateMesh();
  virtual const CMeshBase& Mesh() const = 0;
  virtual CMeshBase& Mesh() = 0;

  // see if we are currently loading
  bool Loading() const { return m_bLoading; }

  // invalidate the results after the load (must be loading)
  void InvalidateResultsAfterLoad();

  virtual int Dimension() const = 0;

  virtual void ConvertSet(IPointSet* pSet, bool b3DSystem);

  int IniStressMaxIterations() const;
  void IniStressMaxIterations(int n);

  double IniStressConvCriterion() const;
  void IniStressConvCriterion(double c);

  bool LinstaOnlyInitialStress() const;
  void LinstaOnlyInitialStress(bool b);

  bool isPressureSupportNodeLoadWarningDone() const;
  void setPressureSupportNodeLoadWarningDone();

  const CBranchState& BranchState() const;
  CBranchState& BranchState();
  void ClearBranch();
  bool HasBranches() const;
  bool HasPhases() const;

  CGeomecDianaRunnerBase* CreateDianaRunner(CDianaRunController& controller);

  void PrepareFaultsForAnalysis(bool bSilent = false);

  bool AverageResults() const;
  void AverageResults(bool bAverage);

  // mesh node index (global) to set of attached element indices
  typedef std::map<int, std::set<int> > TNodeElementsMap;

  virtual void CollectBoundaryNodes(TNodeElementsMap& mpNodeElements) const;
  void CollectDepletingFormationNodes(TNodeElementsMap& mpNodeElements, CAnalysisType::TAnalysisType analysisType) const;

  //wjrx mantis 2976
  bool CanImportResults() const;
  bool CanInsertNewWellPath() const;

  bool UseDecompactionParameters() const;
  void UseDecompactionParameters(bool bUse);

  bool WriteOutputStreamFile() const;
  void WriteOutputStreamFile(bool bWrite);

  bool CleanupOldBranchResults() const;
  void CleanupOldBranchResults(bool bCleanup);

  // whether or not to automatically disable drawing
  void AutomaticallyDisableDrawing(bool bDisable);
  bool AutomaticallyDisableDrawing() const;
  bool AutomaticallyDisableDrawingFor(const CGraphNode& node) const;

  // disable redrawing automatically
  void DisableRedrawing();
  void setAllowRedrawing(bool allowRedrawing);
  void setForcingRedraw(bool forcingRedraw);
  bool RedrawingDisabled() const;

  // the scene should ask here whether it can draw or not
  bool SceneCanDraw() const;

  bool ShowConvexHulls() const;
  void ShowConvexHulls(bool bShowConvexHulls);

  virtual const CModelBase& RootModel() const;
  virtual CModelBase& RootModel();

  bool ExportCommandFilesWithDefaults() const;
  void ExportCommandFilesWithDefaults(bool b);

  virtual int childModelSize() const;
  virtual CModelBase& childModel(int index);
  virtual const CModelBase& childModel(int index) const;
  virtual CModelBase* parentModel();
  virtual const CModelBase* parentModel() const;
  void setParentModel(CModelBase* pParentModel);

  void SetChildModelLoadSave(IModelLoadSave* pChildModelLoadSave);

  CVersionManager getVersionManager() const;
  int Index() const;
  void Index(int nIndex);
  void SetAppVersion(const QString& applicationVersion);

  CModelBase *Model(int nIndex);

  void OnNewChildModel(CModelBase& child, bool attachToDocument);
  CNewModel& getNewModelObject();

  virtual bool CanSwitchTo() const;
  virtual void SwitchTo(CFemAppModel* currentModel);
  virtual void SwitchToParent(CModelBase& parent);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  void MenuInvokedBuildDepletionStages();

  TParentResultsDef ParentResultsDefinition() const;
  void ParentResultsDefinition(TParentResultsDef def);
  bool ParentLinearResults(const CAnalysisType& antype) const;

  // get an equivalent depletion stage from the parent model for a stage in this model
  const CDepletionStage& ParentEquivalentDepletionStage(const CDepletionStage& stage) const;

  bool CheckZoominSecant();
  bool CheckParentResults(const CAnalysisType& antype) const;
  bool CheckCasingContainedInWell() const;

  bool getEnableResultCache() const;
  int getResultCacheSize() const;

  void setResultCache(bool enableResultCache, int resultCacheSize);

  CMaterialErrorHandler& getMaterialErrorHandler();

  // options for convergence in nonlinear analyses
  // execute these checks or not
  void ConvergenceChecks(bool bCheck);
  bool ConvergenceChecks() const;
  // disable nonlinearity when too small angle
  void ConvergenceMinimumAngleCheck(bool bCheck);
  bool ConvergenceMinimumAngleCheck() const;
  // the minimum angle
  void ConvergenceMinimumAngleValue(double dValue);
  double ConvergenceMinimumAngleValue() const;
  // disable nonlinearity when too small edge ratio
  void ConvergenceMinimumRatioCheck(bool bCheck);
  bool ConvergenceMinimumRatioCheck() const;
  // the minimum edge ratio
  void ConvergenceMinimumRatioValue(double dValue);
  double ConvergenceMinimumRatioValue() const;
  // disable nonlinearity when too low element volume
  void ConvergenceMinimumVolumeCheck(bool bCheck);
  bool ConvergenceMinimumVolumeCheck() const;
  // the minimum element volume
  void ConvergenceMinimumVolumeValue(double dValue);
  double ConvergenceMinimumVolumeValue() const;

  ACCEPT_GEOMECMODELVISITORS(VisitModelBase);

  void DefaultDianaZoominSetting(bool bDianaZoominSetting);

private:
  friend class CGeomecCompactionDianaRunner;
  bool DefaultDianaZoominSetting() const;

  template <class ENTRY> void NumberNodes(unsigned int nEntryId, int& nStartIdx, std::set<CStorageNode*>& stDone);
  bool LoadHeader(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prg);
  void SaveHeader(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg, int& nMaxNrOfSteps, quint64& dwStepPos);
  void FinalizeSave(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prg, int nMaxNrOfSteps, quint64 dwStepPos);

  void DuplicateGlobalPressure();
  void DuplicateGlobalTemperature();
  void DuplicateGravity();
  void BuildDepletionStages();

  void ConvergenceChecksChanged();

  std::map<QString, CGeneralDataBlock *> m_generalDataBlocks;

  CMaterialErrorHandler m_materialErrorHandler;
};

#endif // !defined(AFX_MODELBASE_H__313C9293_EC41_4C00_B9FB_5BF9A1632CDA__INCLUDED_)
