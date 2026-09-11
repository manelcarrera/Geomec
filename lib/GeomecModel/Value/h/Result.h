// Result.h: interface for the CResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULT_H__6FDF76F2_D951_4CBB_B42A_3B084B8B1476__INCLUDED_)
#define AFX_RESULT_H__6FDF76F2_D951_4CBB_B42A_3B084B8B1476__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The elementry results are stored in the resultregister. Other results can be derived from
// these elemenry results. The resultregister is not visible for the user. The user can only
// access CResultGroup, IResult and IResultComponent.
// The resultgroup can contain IResult and other resultgroups. The treestructure is made with
// resultgroup.

#include "AnalysisType.h"
#include "DepletionStage.h"
#include <string>

class CModelBase;
class IResult;
class CResultInfo;

#include "IValueDataInterface.h"
#include "IValueSet.h"
#include "ResultRegister.h" // mandatory ordering, 'ResultRegister.h' must
#include "ValueType.h"
#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
// be before 'TensorValueSetTemplate.h'
#include "TensorValueSetTemplate.h"

class CResultGroup : public CGraphNode {
  // friend class IResult;
  CResultGroup *m_pParent;
  std::vector<CGraphNode *> m_vcChild;
  CFemAppModel &m_model;

public:
  // Construction
  CResultGroup(const QString &sGroupName, CModelBase &model); // Root constructor
  CResultGroup(unsigned int uGroupName, CModelBase &model);   // Root constructor
  CResultGroup(const QString &sGroupName, CResultGroup &parent);
  CResultGroup(unsigned int uGroupName, CResultGroup &parent);
  virtual ~CResultGroup();

  // Parent
  const CResultGroup *Parent() const;
  CResultGroup *Parent();

  // Children
  int ChildSize() const;
  const CGraphNode &Child(int nIndex) const;
  CGraphNode &Child(int nIndex);
  void PushBack(CGraphNode &child);

  // Some graph overrides
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Less(const CGraphNode &node) const;
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  bool CanExportGocad() const;
  bool CanExport() const;
  bool CanImport() const;

  CFemAppModel &Model() { return m_model; }
  const CFemAppModel &Model() const { return m_model; }
  virtual bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  ACCEPT_GEOMECMODELVISITORS(VisitResultGroup);
};

// The result is always member of the resultgroup. The result contains result components
// for every depletion stage and for a nonlin or a linear calculation. The resultcomponent
// represent the actual value of the result at a certain location. The result always has access
// to the resultregister.

class CResultEntry;
class IResultComponent;
class CResultRegister;
class IResult : public IValueComposite {
private:
  friend class IResultComponent;
  CResultGroup &m_group;
  class CResultKey {
    CAnalysisType m_antype;
    int m_nStage;
    int m_nRegister;

  public:
    CResultKey(int nStage, const CAnalysisType &antype, int nRegister);
    bool operator<(const CResultKey &rhs) const;
  };

public:
  typedef std::map<CResultKey, unsigned int> TResultMap;

private:
  TResultMap m_mpResult;

  std::pair<const CDepletionStage *, const CDepletionStage *> NeighboringStages(const CGeomecTime &time) const;

protected:
  // Build components
  void BuildComponents();
  virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) = 0;
  void ClearComponents();
  virtual IResultComponent *OnGetResultComponent(int nTimeStep, const CAnalysisType &antype, int nRegister,
                                                 unsigned int nComponent) const;

public:
  QString ExportLabel() const;
  virtual QString ExportLabel(int nComponent) const = 0;

  typedef IValueComponentBase::UNIT UNIT;
  // Construction
  IResult(unsigned int uResultName, CResultGroup &group);
  IResult(const QString &sResultName, CResultGroup &group);
  virtual ~IResult();

  // Graph interface
  virtual unsigned int IconId() const;
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual unsigned int TypeId() const;

  // Result type
  int RegisterMode(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
  int Mode(const CAnalysisType &antype, const CDepletionStage &stage, int nRegister = 0) const;
  int Mode(const CAnalysisType &antype, int nTimeStep, int nRegister = 0) const;
  const IResultComponent *ResultComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister = 0,
                                          unsigned int nComponent = 0) const;
  IResultComponent *ResultComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister = 0,
                                    unsigned int nComponent = 0);
  const IResultComponent *ResultComponent(int nTimeStep, const CAnalysisType &antype, int nRegister = 0,
                                          unsigned int nComponent = 0) const;
  virtual IResultComponent *ResultComponent(int nTimeStep, const CAnalysisType &antype, int nRegister = 0,
                                            unsigned int nComponent = 0);

  bool Available(const CAnalysisType &antype, const CDepletionStage &stage, int nRegister = 0) const;

  geo::CValue ResultAtTime(const CAnalysisType &antype, const CGeomecTime &time, const geo::IPoint &pt,
                           const IQuantityDouble::UNIT unit = IQuantityDouble::SI_UNIT, int nComponent = 0) const;
  std::vector<geo::CValue> ResultAtTime(const CAnalysisType &antype, const CGeomecTime &time,
                                        const geo::IElement &element,
                                        const IQuantityDouble::UNIT unit = IQuantityDouble::SI_UNIT,
                                        int nComponent = 0) const;

  // This function is called when defined is called at the component by the framework. The
  // default implementation returns true when linear results are available.
  virtual bool OnDefined(const IResultComponent &component) const;

  // Parent
  const CResultGroup *Parent() const;
  CResultGroup *Parent();

  // The result entry
  int ResultRegisterSize() const;
  const CResultRegister &ResultRegister(int nIndex) const;
  CResultRegister &ResultRegister(int nIndex);

  // We cannot save and load derived results, but implement the functions
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual bool Less(const CGraphNode &node) const;
  virtual bool isResult() const;

  // Export functions
  bool CanExport() const;

  virtual bool CanMap(const COpenGLNode &node, int nRegister) const = 0;
  virtual bool ValidName(const std::string &name, CResultInfo &resultInfo) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIResult);

protected:
  virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  bool validateName(CResultInfo &resultInfo, const std::string &name, const std::string &label,
                    const std::string &componentList, const std::string &analysisTypeList) const;
  bool validateName(CResultInfo &resultInfo, const std::string &name, const std::string &label,
                    const std::string &componentList, const std::string &analysisTypeList,
                    const ITensorGroup *parent) const;
  bool validateDerivedName(CResultInfo &resultInfo, const std::string &name, const std::string &label,
                           const std::string &componentList, const std::string &analysisTypeList) const;
};

typedef CGraphEntryTemp<IResult> TResultEntry;

// The result component delivers the interface for querying value for points and elements.
// For every type of calculation linear / non linear the interfaces supplies value component.
class CMeshBase;
class CHexaHorizon;
class IResultComponent : public IValueComponentBase {
private:
  const CDepletionStage &m_stage;
  CAnalysisType m_antype;
  int m_nRegister;

public:
  typedef IValueDataInterfaceScalarTempl<IResultComponent> IValueDataInterfaceResultScalar;

  // Construction
  IResultComponent(unsigned int uName, IResult &result, const CDepletionStage &stage, const CAnalysisType &antype,
                   int nRegister, unsigned int uComponent);
  IResultComponent(const QString &sName, IResult &result, const CDepletionStage &stage, const CAnalysisType &antype,
                   int nRegister, unsigned int uComponent);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar &ScalarData() const;
  virtual IValueDataInterfaceScalar &ScalarData();

  // Access to the mesh
  const CMeshBase &Mesh() const;
  CMeshBase &Mesh();

  // Access to result register
  const CResultRegister &ResultRegister() const;
  CResultRegister &ResultRegister();

  // Timestep / Linearity / RegisterIndex
  const CAnalysisType &AnalysisType() const;
  const CDepletionStage &Stage() const;
  int RegisterIndex() const;

  // Min / Max
  virtual bool Defined() const;
  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  virtual bool Empty() const;
  virtual long SavedItems() const;

  typedef CValueType::TMapEnum TMapType;
  // Get the result for a given point, above or below the given horizon
  virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                               UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const = 0;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue GradientValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                        geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm, const UNIT unit = CQuantity::SI_UNIT,
                                                     geo::IParallelInitializationCallback *cb = 0) const;

  virtual void MapTensorElement(const geo::IElement & /*elm*/, std::vector<CTensor> & /*values*/,
                                geo::IParallelInitializationCallback * /*cb*/ = 0) const {
    assert(false);
  }

  virtual std::vector<CTensor> TensorElement(const geo::IElement &element,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                             geo::IParallelInitializationCallback *cb = 0) const;

  virtual bool CanMap(const COpenGLNode &node) const;
  virtual QString ExportLabel() const;

  // Export functions
  bool CanExport() const;

  virtual bool Gradient() const;

  ACCEPT_GEOMECMODELVISITORS(VisitIResultComponent);

protected:
  IValueDataInterfaceResultScalar m_Data;
};

class IScalarResultComponent : public IResultComponent {
  class CResultValueSet : public geo::IValueSet {
    const IScalarResultComponent &m_component;
    CQuantity::UNIT m_unit;

  public:
    CResultValueSet(const IScalarResultComponent &component, CQuantity::UNIT = CQuantity::SI_UNIT);
    virtual const geo::IElementSet &ElementSet() const;
    virtual void ElementValues(TValueVec &values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
    CQuantity::UNIT Unit() const;
    virtual void Empty();
    virtual bool IsEmpty() const;

  protected:
    virtual int PushBack(const TValueVec &values);
  };
  CResultValueSet m_si;
  CResultValueSet m_field;
  // Value sets
  const geo::IValueSet &ValueSet(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

public:
  IScalarResultComponent(unsigned int uName, IResult &result, const CDepletionStage &stage, const CAnalysisType &antype,
                         int nRegister, unsigned int uComponent);
  IScalarResultComponent(const QString &sName, IResult &result, const CDepletionStage &stage,
                         const CAnalysisType &antype, int nRegister, unsigned int uComponent);

  // Get the result for a given point, above or below the given horizon
  virtual geo::CValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *cb = 0) const;
  virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                               UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  virtual void ElementValues(IValueDomainScalar::TValueVec &values, const geo::IElement &element, UNIT unit,
                             geo::IParallelInitializationCallback *cb) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIScalarResultComponent);
};

#endif // !defined(AFX_RESULT_H__6FDF76F2_D951_4CBB_B42A_3B084B8B1476__INCLUDED_)
