// GeomecDianaRunner.h: interface for the CDianaRunner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIANARUNNER_H__D1ED1D8C_2B43_4622_91FD_7591E41F1A6E__INCLUDED_)
#define AFX_DIANARUNNER_H__D1ED1D8C_2B43_4622_91FD_7591E41F1A6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ISaveModel;
class CFFMaterial;

#include "DisplacementSupportNode.h"
#include "PressureSupportNode.h"

class CMeshBase;
class CDepletionStage;
class CDianaX;
class CGeomecApp;
class CDianaRunController;
class CVectorValueSet;
class CTetraBoundary;
class CTetraMesh;
class C3DHorizon;
class CStrainType;
class CStrainLoad;

namespace geo {
class CSurfaceDesc;
class CTetSurface;
class CBodyTriangle;
class IMesh;
} // namespace geo

namespace dia {
class IValueTable;
} // namespace dia

// minimum values for pressure and temperature loads
// loads don't get written if all (absolute) element values are below these values
#define MIN_PRESSU_LOAD_VAL 1e-8
#define MIN_TEMPER_LOAD_VAL 1e-8
#define MIN_PRESTR_LOAD_VAL 1e-8

#include "DianaRunner.h"
#include "IDCConcentrationTable.h"
#include "IDCTemperatureTable.h"
#include "ISurface.h"
#include "ModelBase.h"
#include "ResultRegister.h" // mandatory ordering, 'ResultRegister.h' must
#include "ivaluecomponent.h"
  // be the first header-file (before
  // 'Geomec2DDianaRunner.h')
#include "TensorValueSetTemplate.h"

class CGeomecDianaRunnerBase : public CDianaRunner {
protected:
  typedef std::map<const CDepletionStage *, dia::CLoadCase *> TStageMap;

public:
  CGeomecDianaRunnerBase(CMeshBase &meshbase, CModelBase &model, CDianaRunController &controller);
  virtual ~CGeomecDianaRunnerBase();
  CMeshBase &MeshBase() const;
  CModelBase &Model() const;

  // return material for the given element
  virtual const dia::IMaterial &Material(const geo::IElement &element) const;

  bool WriteFullModel() const;

  const CDianaRunController &Controller() const;

  void SetLocation(const std::string &strLocation);
  void SetFileName(const std::string &strFileName);

  virtual bool HasTemperatureValue(const geo::IElement &element) const;
  virtual bool HasConcentrationValue(const geo::IElement &element) const;
  virtual bool WriteBoundaryLoads(TStageMap &mpStages);

  virtual IValueDomainScalar::TValueVec ElementTemperatures(const geo::IElement &element,
                                                            const CDepletionStage &stage) const;
  virtual IValueDomainScalar::TValueVec ElementConcentrations(const geo::IElement &element,
                                                              const CDepletionStage &stage) const;

protected:
  typedef enum eSolver { ITERATIVE = 0, PARALLEL_DIRECT_SPARSE = 1 } TSolver;

  virtual TSolver Solver() const;

  class CEdgeLine : public geo::ILine {
    const geo::IPoint *m_pt1;
    const geo::IPoint *m_pt2;

  public:
    CEdgeLine(const geo::IPoint &pt1, const geo::IPoint &pt2) : m_pt1(&pt1), m_pt2(&pt2) {}
    CEdgeLine(const CEdgeLine &rhs) : m_pt1(rhs.m_pt1), m_pt2(rhs.m_pt2) {}
    CEdgeLine &operator=(const CEdgeLine &rhs) {
      m_pt1 = rhs.m_pt1;
      m_pt2 = rhs.m_pt2;
      return *this;
    }
    virtual const geo::IPoint &Point(int nIndex) const;
    virtual size_t Order() const { return 1; }
  };

  virtual bool OnInitialize(const std::string &title, IRetrieveDianaFileNames &retrieveDianaFileNames);

  virtual bool OnWriteMesh();

  virtual bool UseLegacyCodeForMaterialProperties() const;

  struct CElementSetInfo {
    ftn_int_t matidx;
    ftn_int_t geoidx;
    const CFormationBase *pFormation;
    ftn_int_t eltidx;

    CElementSetInfo(ftn_int_t matidx, ftn_int_t geoidx, const CFormationBase *pFormation, ftn_int_t eltidx);

    bool operator<(const CElementSetInfo &rhs) const;
  };

  struct CGroupInfo {
    const CFormationBase *pFormation;
    ftn_int_t eltsetidx;

    CGroupInfo(const CFormationBase *pFormation, ftn_int_t eltsetidx);

    bool operator<(const CGroupInfo &rhs) const;
  };

  typedef std::vector<CElementSetInfo> TElementSetInfoVector;
  typedef std::vector<CGroupInfo> TGroupInfoVector;
  virtual bool OnWriteElementSets(TElementSetInfoVector &elementset_info);
  virtual bool OnWriteElementGroups(TGroupInfoVector &group_info);

  struct CDistributedElementSetInfo {
    const CFormationBase *pFormation;
    ftn_int_t eltidx;
    const dia::IElementProperty *pProperty[2];

    CDistributedElementSetInfo(const CFormationBase *pFormation, ftn_int_t eltidx,
                               const dia::IElementProperty *pMaterial, const dia::IElementProperty *pGeometry);

    bool operator<(const CDistributedElementSetInfo &rhs) const;
  };

  typedef std::vector<CDistributedElementSetInfo> TDistributedElementSetInfoVector;
  virtual bool OnWriteElementInformation(TDistributedElementSetInfoVector &distributed_info);

  virtual bool WriteElements();

  virtual bool OnEndAnalysis(const QString &getPathName, ISaveModel &saveModel);
  virtual bool OnEndWriteCommands(const std::string &title);
  virtual bool OnEndPreprocessWriteInputFiles();
  virtual bool OnEndPostprocessWriteInputFiles(const std::string &sComFileName, const std::string &sDatFileName);
  virtual bool OnWriteCommands();
  virtual bool OnWriteMiscelaneous();
  virtual bool OnWriteElementOrder(bool forceQuadratic);
  virtual dia::ITemperatureTable *OnCreateTemperatureTable() const;
  virtual void OnExtendTemperatureTable(dia::ITemperatureTable *pTable) const;
  virtual dia::IConcentrationTable *OnCreateConcentrationTable() const;
  virtual void OnExtendConcentrationTable(dia::IConcentrationTable *pTable) const;

  virtual int MeshProgressSize() const;
  virtual int GroupsProgressSize() const;

  virtual QString DianaExecutable();

  virtual void PreExecution();
  virtual void PostExecution(bool bCalcResult);

  virtual void AnalysisSuccess(bool bAnalysisSuccess);

  virtual bool LoadPartialResults();

  virtual void OnError(eError id, const std::string &msg2 = "");

  // Return true if you want to export .com and .dat files
  // instead of actually running the analysis.
  // The analysis will not be performed, but input
  // will be run to generate the data file.
  virtual bool WriteInputFiles(std::string &comfilename, std::string &datfilename) const;

  // for retrieval of interface materials (only if element is an interface, otherwise 0) from derived classes
  virtual const dia::IMaterial *InterfaceMaterial(const geo::IElement &element) const = 0;

  virtual bool CanCleanupAfterRun() const;

  friend class CDianaRunController;
  void AllowCleanUpAfterRun(bool allow);
  void ForceCleanUp(const std::string &subdir);

  // return an existing path if you want to use that instead of a generated one
  virtual std::string UserDefinedLocation() const;

  // allows user filos file (e.g. for restart)
  //  strFilosFile - set the name of the file
  //  bInitFilos   - set to false if filos file should not be initialized
  //  return false to stop analysis
  virtual bool UserDefinedFilosFile(const QString &getPathName, std::string &strFilosFile, bool &bInitFilos) const;

  bool InitialLoadCaseActive() const;

  dia::ITemperatureTable *CreateTemperatureTable() const;

  void WriteSolveCommands();

  virtual bool AllowLinearCalc() const;
  virtual bool BehavesLinearly(const CDepletionStage &stage) const;
  void WriteDepletionStageStepSizesAndTime(const CDepletionStage &stage);

  // The new ones
  const CStrainLoad &StrainLoad(const geo::IElement &element, const CDepletionStage &stage) const;
  virtual void _CreateVolumetricStrainLoad(const geo::IElement &elm, const CDepletionStage &stage,
                                           dia::CLoadCase &lcase);
  virtual void _CreatePlanarStrainLoad(const geo::IElement &elm, const CDepletionStage &stage, dia::CLoadCase &lcase);
  virtual void CreateStrainLoad(const geo::IElement &elm, const CDepletionStage &stage, dia::CLoadCase &lcase);

  virtual const dia::IElementGeometry *ElementGeometry(const geo::IElement &element) const;
  void WriteNonlinDepletionStageParameters(const CDepletionStage *pStage, bool bHasSave);

  bool CreateTopLoad3D(const CHorizonBase &top, dia::CLoadCase &lcase);
  virtual bool IsZoomInModel() const;
  bool WriteZoomInInitialStresses(const geo::IElement &element, dia::CLoadCase &lcase,
                                  const IValueDomainScalar::TValueVec &vcInitialPressures);
  void PerformInitialStressCalculation(bool b);
  bool PerformInitialStressCalculation() const;
  virtual bool InitialStressesDefined() const;
  virtual bool ExportCommandFileWithDefaults() const;
  geo::CVector GetDisplacementVector(const CVectorValueSet &displa, const geo::IElement &elm,
                                     const geo::IPoint &ptSample) const;
  virtual void WriteConvergenceCommands();
  virtual bool CheckConvergenceMinimumVolume() const;
  virtual std::string GroupName(const geo::IElementSet &group, int nIndex);

protected:
  mutable std::vector<dia::IElementProperty *> m_vcAllocatedElementProperties;

private:
  class CStagePointCombi {
    const CDepletionStage &m_stage;
    const geo::IPoint &m_pt;

  public:
    CStagePointCombi(const CDepletionStage &stage, const geo::IPoint &pt);
    bool operator<(const CStagePointCombi &rhs) const;
  };

  class CComboSurface : public geo::ISurface {
    std::map<const geo::IPoint *, size_t> m_mpPointIndices;
    std::vector<const geo::IPoint *> m_vcPoints;
    std::vector<const geo::IFace *> m_vcFaces;

  public:
    CComboSurface(const std::vector<const geo::ISurface *> &vcSurface);
    ~CComboSurface();

    void AddFace(const geo::IFace &face);
    virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint &p) const;
    virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint &p) const;
    virtual const geo::IFace &Face(int nIndex) const;
    virtual int FaceSize() const;
    virtual const geo::IPoint &Point(int nIndex) const;
    virtual int PointSize() const;
    virtual geo::IPoint &PointAt(int nIndex);
    virtual const geo::IElement &Element(int nIndex) const;
    virtual int ElementSize() const;
    virtual std::vector<int> Nodes(const geo::IElement &element) const;
  };

  typedef std::map<const geo::INode *, const CDisplacementSupportNode::CNodalSupportDef *> TSupportDefMap;
  typedef std::map<CStagePointCombi, CDisplacementSupportNode::CNodalSupportDef::CSupportDef> TStagePointSupportMap;

  typedef std::map<const CFormationBase *, QString> TFormationElmsetNames;
  TFormationElmsetNames m_mpFormationElmsetNames;

private:
  CMeshBase &m_MeshBase;
  CModelBase &m_Model;
  CDianaRunController &m_controller;
  void (*m_pFilosFatal)(int erno, const char *errm, const char *proc, const char *args);
  bool m_bAnalysisSuccess;
  std::string m_strLocation;
  std::string m_strFileName;

  bool m_AllowCleanUpAfterRun;

  // if we are exporting input files
  std::string m_sDatFile;
  std::string m_sComFile;

  // if we are exporting a quad dat file
  std::string m_sQuadDatFileDir;
  std::string m_sQuadDatFileName;
  bool m_bPerformInitialStressCalculation;
  mutable bool m_bBehaveLinearly;
  mutable bool m_bBehaveLinearlyDirty;

  TStagePointSupportMap m_mpProcessedNodes;

private:
  bool calculateVolumetricStrain(std::vector<double> &vcElementValues, const geo::IElement &elm,
                                 const CDepletionStage &stage) const;
  double calculateBulkModulus(const geo::IElement &elm) const;
  double calculateBulkModulus(const CFFMaterial *cffMaterial) const;

  void WriteTableCommands();
  void GenerateTimePoints(dia::IValueTable &table) const;
  void GenerateMoreTimePoints(dia::IValueTable &table) const;

  bool WriteTetraBoundaryLoads(TStageMap &mpStages);
  bool WriteHexaBoundaryLoads(TStageMap &mpStages);

  bool WriteHexaHorizonBoundaryLoads(const C3DHorizon &hor, TStageMap &mpStages, const CPressureSupportNode &pnode);

  bool WriteInterfaceBoundaryLoads(TStageMap &mpStages, const CInterfaceBoundary &boundary, const CMeshBase &mesh);

  bool WriteBoundaryStressLoadsFromParentModel(TStageMap &mpStages, const CInterfaceBoundary &boundary,
                                               const CMeshBase &mesh);

  bool WriteBoundaryDisplacementLoadsFromParentModel(TStageMap &mpStages, const CInterfaceBoundary &boundary,
                                                     const CMeshBase &mesh);

  bool WriteBoundaryLoadsUsingDistributions(TStageMap &mpStages, const CInterfaceBoundary &boundary,
                                            const CMeshBase &mesh);

  bool WriteBoundaryLoadsForSurfaceDesc(TStageMap &mpStages, const geo::CSurfaceDesc &surfdesc,
                                        const CPressureSupportNode &pnode);

  bool WriteBoundaryLoadsForSurfaces(TStageMap &mpStages, const std::vector<const geo::ISurface *> &vcSurface,
                                     const CPressureSupportNode &pnode);

  void CreatePrescDisplLoad(const CDisplacementSupportNode &node, int nNode, const geo::IMesh &mesh,
                            const CDepletionStage &stage, dia::CLoadCase &lc, bool bDistrAvailable,
                            bool bDistrAvInitial);

  void CreateBodyFaceLoad(const CPressureSupportNode &node, const geo::IElement &element, const CDepletionStage &stage,
                          dia::CLoadCase &lc, bool bDistrAvailable, bool bDistrAvInitial);

  void DisplacementVector(const CDisplacementSupportNode &node, int nNode, const geo::IMesh &mesh,
                          const CDepletionStage &stage, bool bDistrAvailable, geo::CVector &displacement);

  void StressVectors(const CPressureSupportNode &node, const geo::IElement &element, const CDepletionStage &stage,
                     const geo::IFace &face, bool bDistrAvailable, std::vector<geo::CVector> &vcPressure);

  bool WritePrescribedDisplacements(TStageMap &mpStages, const std::vector<const geo::ISurface *> &vcSurface,
                                    CPressureSupportNode::CFaceSupportDef::TSupportType suptype);

  geo::CMatrix CreateGlobalTensorPreMultMatrix(const CDisplacementSupportNode &dnode, const CDepletionStage &stage);

  void WritePrescribedDisplacementForSupport(dia::CLoadCase &lcase, const geo::IVector &vecDispla,
                                             const geo::CTranslationSupport &support);

  bool WritePrescribedDisplacementsFromInterpolation(const geo::IMesh &mesh,
                                                     // const geo::CTetSurface &surface,
                                                     const geo::ISurface &surface,
                                                     const CDisplacementSupportNode &dnode,
                                                     const CDepletionStage &stage, dia::CLoadCase &lcase,
                                                     const geo::IMatrix &MatPreMult, TSupportDefMap &mpSupportDef);

  CDisplacementSupportNode::CNodalSupportDef::CSupportDef
  InterpolateSupportDef(const geo::IPoint &pt, const geo::IPoint &pt1, const geo::IPoint &pt2,
                        const geo::IMatrix &MatPreMult, const CDepletionStage &stage,
                        const CDisplacementSupportNode &dnode);

  void WritePrescribedDisplacementsFromGlobalTensor(const geo::IMesh &mesh,
                                                    // const geo::CTetSurface &surface,
                                                    const geo::ISurface &surface, const CDisplacementSupportNode &dnode,
                                                    const CDepletionStage &stage, dia::CLoadCase &lcase,
                                                    const geo::IMatrix &MatPreMult);

  geo::CVector PressureVector(const geo::IFace &face, const CDepletionStage &stage, const geo::ISurface &surface);

  void WriteBoundaryPressureEntry(const CDepletionStage &stage, dia::CLoadCase &lcase, const geo::ISurface &surface);

  void WriteBoundaryPressure(TStageMap &mpStages, const geo::ISurface &surfacedesc,
                             const CPressureSupportNode::CFaceSupportDef &facedef);

  std::vector<double> recalculateElementConcentrations(bool &bValid, bool &bWrite, bool &bEqual,
                                                       IValueDomainScalar::TValueVec &vcValues,
                                                       const geo::IElement &elm, const CDepletionStage &stage) const;

  bool isAnisotropic(const CFFMaterial *cffMaterial) const;
  bool isAnisotropic(const geo::IElement &element) const;

  class CTemperatureTable : public dia::ITemperatureTable {
    CGeomecDianaRunnerBase *m_runner;
    const CModelBase &m_model;
    typedef std::map<const dia::IValueTable::CTimePoint *, const CDepletionStage *> TTimePoint2DeplStageMap;
    TTimePoint2DeplStageMap m_mpTimePoint2DeplStage;

  public:
    CTemperatureTable(const CGeomecDianaRunnerBase &runner, const CModelBase &model);
    void RegisterTemperaturePoint(const dia::IValueTable::CTimePoint &tpoint, const CDepletionStage &dstage);

    virtual void SetRunner(dia::IDianaRunner *runner);

  protected:
    virtual bool HasValue(const geo::IElement &element) const;
    virtual void ValueAt(const geo::IElement &element, const dia::IValueTable::CTimePoint &tpoint,
                         std::vector<double> &vcValues) const;
  };

  class CConcentrationTable : public dia::IConcentrationTable {
    CGeomecDianaRunnerBase *m_runner;
    const CModelBase &m_model;
    typedef std::map<const dia::IValueTable::CTimePoint *, const CDepletionStage *> TTimePoint2DeplStageMap;
    TTimePoint2DeplStageMap m_mpTimePoint2DeplStage;

  public:
    CConcentrationTable(const CGeomecDianaRunnerBase &runner, const CModelBase &model);
    void RegisterConcentrationPoint(const dia::IValueTable::CTimePoint &tpoint, const CDepletionStage &dstage);

    virtual void SetRunner(dia::IDianaRunner *runner);

  protected:
    virtual bool HasValue(const geo::IElement &element) const;
    virtual void ValueAt(const geo::IElement &element, const dia::IValueTable::CTimePoint &tpoint,
                         std::vector<double> &vcValues) const;
    virtual bool CompressTable() const;
  };

  class CUpscalingAnisotropyElementGeometry : public dia::IElementGeometry {
  public:
    // angles in degrees
    CUpscalingAnisotropyElementGeometry(double dInclination, double dAzimuth);
    virtual bool WriteFilos(IDianaRunner &diarunner) const;
    virtual bool operator<(const IElementProperty &rhs) const;

    virtual int Type() const;
    virtual int WriteFilosParamSize(IDianaRunner &diarunner) const;
    virtual bool WriteFilosParamName(IDianaRunner &diarunner, int i, char *name) const;
    virtual void WriteFilosParamValues(IDianaRunner &diarunner, double *values, int stride = 1) const;

  private:
    double m_dInclination;
    double m_dAzimuth;
  };

  class CFractureApertureElementGeometry : public dia::IElementGeometry {
  public:
    CFractureApertureElementGeometry(const geo::IVector &vecHighDensity, const geo::IVector &vecLowDensity);
    virtual bool WriteFilos(IDianaRunner &diarunner) const;
    virtual bool operator<(const IElementProperty &rhs) const;

    virtual int Type() const;
    virtual int WriteFilosParamSize(IDianaRunner &diarunner) const;
    virtual bool WriteFilosParamName(IDianaRunner &diarunner, int i, char *name) const;
    virtual void WriteFilosParamValues(IDianaRunner &diarunner, double *values, int stride = 1) const;

  private:
    geo::CVector m_vecHighDensity;
    geo::CVector m_vecLowDensity;
  };
};

#endif // !defined(AFX_DIANARUNNER_H__D1ED1D8C_2B43_4622_91FD_7591E41F1A6E__INCLUDED_)
