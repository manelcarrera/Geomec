// FormationBase.h: interface for the CFormationBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONBASE_H__419B2161_22C3_4E11_8F81_75C3590EDF72__INCLUDED_)
#define AFX_FORMATIONBASE_H__419B2161_22C3_4E11_8F81_75C3590EDF72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ElementSet.h"
#include "MaterialServer.h"
#include "MaterialServerParent.h"

class CPressure;
class CFractureMatrixPressure;
class CTemperature;
class CCalculatedTemperature;
class CStrainLoad;
class CStrainType;

#include "ValueStrain.h"
#include "ValueTypeFactory.h"

#include "ExportFormat.h"
#include "FormationElementOrder.h"
#include "GeomecModelVisitor.h"
#include "IParallelInitializationCallback.h"
#include "LengthQuantity.h"
#include "resourceIDS.h"

class IFormationElementSet : public IElementSet {
  //	bool m_bInitialPressure;
  CFormationBase *m_pFormation;
  mutable geo::CArray<geo::CElementPoint> *m_pElementCenters;

private:
  void ClearElementCenterCache() const;
  void CreateElementCenters() const;

protected:
  const geo::IObject &ElementCenterPoints() const;

public:
  // Construction
  IFormationElementSet(CFormationBase &formation, bool bDoLink = true);
  IFormationElementSet(const IFormationElementSet &rhs);

  virtual ~IFormationElementSet();

  // Assignment
  bool operator==(const IFormationElementSet &rhs) const;
  IFormationElementSet &operator=(const IFormationElementSet &rhs);

  //	bool InitialPressure() const;
  //	void InitialPressure(bool bInitialPressure);

  // Identifier
  virtual TGraphNodeSet Identifier() const;

  // 0 Same
  // 1...n Differences
  int Resemblance(const TGraphNodeSet &identifier) const;

  // These function are linked to the source in copy mode
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual DIMENSION Dimension() const;
  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();

  // The formation we are attached to
  const CFormationBase &Formation() const;
  CFormationBase &Formation();

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  // Formation volume load
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  // Functions are not in use
  virtual int RegisterPoint(const geo::IPoint & /*point*/) {
    assert(false);
    return -1;
  }
  virtual int CreateElement(const TNodeVec & /*vcIndex*/) {
    assert(false);
    return -1;
  }

  // must be overridden
  virtual geo::CValue ThicknessAt(const geo::IPoint & /*pt*/, geo::IPoint & /*ptTop*/, geo::IPoint & /*ptBottom*/,
                                  geo::IParallelInitializationCallback * /*cb*/) const {
    assert(false);
    return geo::CValue();
  }

  ACCEPT_GEOMECMODELVISITORS(VisitIFormationElementSet);
};

class CFormationThickness;

class CFormationBase : public CMaterialServerParent<CColorNode, CMaterialServer> {
  typedef CMaterialServerParent<CColorNode, CMaterialServer> TBase;

private:
  typedef std::vector<IFormationElementSet *> TElementSetVec;
  typedef std::map<const CDepletionStage *, CPressure *> TPressureMap;
  typedef std::map<const CDepletionStage *, CFractureMatrixPressure *> TFractureMatrixPressureMap;
  typedef std::map<const CDepletionStage *, CTemperature *> TTemperatureMap;
  typedef std::map<const CDepletionStage *, CCalculatedTemperature *> TCalcTemperMap;
  typedef std::map<const CDepletionStage *, CStrainLoad *> TStrainMap;
  TElementSetVec m_vcElementSet;
  //	CMaterialServer* m_pMaterial;
  //  typedef std::map<const CDepletionStage*, CMaterialServer*> TMaterialMap;
  //  TMaterialMap m_mpMaterial;
  TPressureMap m_mpPressure;
  TFractureMatrixPressureMap m_mpFractureMatrixPressure;
  TTemperatureMap m_mpTemperature;
  TCalcTemperMap m_mpCalcTemper;
  TStrainMap m_mpStrains;
  CLengthQuantity m_MinThickness;

  mutable geo::CPoint m_min;
  mutable geo::CPoint m_max;

protected:
  const CFormationBase *m_pParent;

public:
  virtual void FindParentFormation();
  virtual const CFormationBase *ParentFormation() const;

private:
  geo::CValue MinZ() const;
  virtual bool ShowCenterPoints() const;
  int StageIndexFromName(const QString &strName) const;

protected:
  CFormationBase(CFemAppModel &model);
  CFormationBase(const QString &strName, CModelBase &model);
  void CreatePressuresAndTemperatures();
  //  virtual void SaveMaterial(CDepletionStage& stage, TSTREAM& stream, TPROGRESS& progress);
  //  virtual void LoadMaterial(CDepletionStage& stage, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  //  virtual long SavedMaterialItems(const CDepletionStage& stage) const;
  //  bool HasMaterial(const CDepletionStage& stage) const;

public:
  CFormationBase(const CFormationBase &rhs);
  virtual ~CFormationBase();

  typedef enum _enumFormationType {
    FT_NOTSPECIFIED = 0,
    FT_SHALE,
    FT_SAND,
    FT_CHALK,
    FT_SALT,
    FT_LIMESTONE,
    FT_SANDSTONE
  } TFormationType;

  CFormationBase &operator=(const CFormationBase &rhs);
  bool operator==(const CFormationBase &rhs) const;

  const CLengthQuantity &MinThickness() const;
  void MinThickness(const double &value, CQuantity::UNIT unit);

  virtual int ElementSetSize() const;
  virtual const IFormationElementSet &ElementSet(int nIndex) const;
  IFormationElementSet &ElementSet(int nIndex);

  virtual const geo::IObject &DisplayList(int nIndex) const;
  virtual int DisplayListSize() const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNewNeighbour(const CGraphNode &node);
  void Export(const QString &fileName, const IExportFormat::CExportArg &arg);
  bool CanExport() const;

  // Pressure of an depletionstages ...
  CPressure &Pressure(const CDepletionStage &dep);
  const CPressure &Pressure(const CDepletionStage &dep) const;

  CFractureMatrixPressure &FractureMatrixPressure(const CDepletionStage &dep);
  const CFractureMatrixPressure &FractureMatrixPressure(const CDepletionStage &dep) const;

  CTemperature &UserTemperature(const CDepletionStage &dep);
  const CTemperature &UserTemperature(const CDepletionStage &dep) const;

  CCalculatedTemperature &CalculatedTemperature(const CDepletionStage &stage);
  const CCalculatedTemperature &CalculatedTemperature(const CDepletionStage &stage) const;

  CStrainLoad &Strain(const CDepletionStage &dep);
  const CStrainLoad &Strain(const CDepletionStage &dep) const;

  // effective temperature to be used based on 'UseCalculatedTemperature' setting in CModelBase
  IValueComposite &EffectiveTemperature(const CDepletionStage &stage);
  const IValueComposite &EffectiveTemperature(const CDepletionStage &stage) const;

  bool Depleting() const;
  bool PressureChange() const;
  bool TemperatureChange() const;
  bool StrainChange() const;
  /*
    // returns the 'active' material for the given depletion stage
    CMaterialServer& Material(const CDepletionStage& stage);
    const CMaterialServer& Material(const CDepletionStage& stage) const;

    // returns a material when it is actually defined for the stage
    // (always returns a valid pointer for the initial stage)
    CMaterialServer* ConnectedMaterial(const CDepletionStage& stage);
    const CMaterialServer* ConnectedMaterial(const CDepletionStage& stage) const;
  */
  // returns the material for the active stage (see CBranchState)
  CMaterialServer &ActiveMaterial();
  const CMaterialServer &ActiveMaterial() const;

  CFormationThickness &FormationThickness();
  const CFormationThickness &FormationThickness() const;

  virtual bool Less(const CGraphNode &node) const;

  // save and load
  // Stream
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;
  virtual bool CanDestroy() const;

  TFormationType FormationType() const;
  void FormationType(TFormationType newtype);

  enum eValueType { PRESSURE = 1, MATERIAL };
  bool InvalidValue(const CDepletionStage &dep, eValueType type) const;

  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool CanDisconnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  bool Contains(const geo::IPoint &pt, bool bIncludeEdge) const;

  geo::CValue ThicknessAt(const geo::IPoint &pt, geo::IPoint &ptTop, geo::IPoint &ptBottom,
                          geo::IParallelInitializationCallback *cb) const;
  void clearThicknessCache();

  CElementOrder::TElementOrder ElementOrder() const;
  void ElementOrder(CElementOrder::TElementOrder elementOrder);

  bool inheritElementOrderFromModel() const;
  void inheritElementOrderFromModel(bool inheritElementOrderFromModel);

  ACCEPT_GEOMECMODELVISITORS(VisitFormationBase);

private:
  TFormationType m_FormationType;
  CFormationThickness *m_pThickness;
  CFormationElementOrder m_formationElementOrder;

  void CalculateBoundingBox() const;
  bool InBoundingBox(const geo::CPoint &point, double eps = 1E-6) const;
};

class CFormationThickness : public IValueComposite {
private:
  class CFormationThicknessComponent : public IValueComponentBase {
  public:
    typedef IValueDataInterfaceScalarTempl<CFormationThicknessComponent> IValueDataInterfaceFormationThicknessScalar;

    CFormationThicknessComponent(CFormationThickness &parent, const CFormationBase &formation);

    virtual TValueComponentType Type() const;

    virtual const IValueDataInterfaceScalar &ScalarData() const;
    virtual IValueDataInterfaceScalar &ScalarData();

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual bool Empty() const;
    virtual long SavedItems() const;
    virtual QString ExportLabel() const;
    virtual QString UnitName(const UNIT unit) const;
    virtual IValueDomainScalar::TMinMax MinMax(IProgressBase &progressBase,
                                               const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                  geo::IParallelInitializationCallback *cb = 0) const;
    virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm, const UNIT unit = CQuantity::SI_UNIT,
                                                       geo::IParallelInitializationCallback *cb = 0) const;
    virtual IValueDomainScalar::TValue Average(IProgressBase &progressBase,
                                               const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual bool Defined() const;

    const CFormationBase &Formation() const;

  protected:
    IValueDataInterfaceFormationThicknessScalar m_Data;

  private:
    const CFormationBase &m_formation;
  };

public:
  CFormationThickness(CFormationBase &formation, CFemAppModel &model);
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitFormationThickness);
};

// #define NEW_FORMATION_ATTACHED  300
// #define FORMATION_DELETED		310

template <class FORMATION> class CFormationEntryTempl : public CStorageNodeEntry<FORMATION> {
public:
  CFormationEntryTempl(int nEntryId, CModelBase &model);

  bool CanExportGocad() const;

  bool ShowSkin() const;
  virtual void OnShowSkin();

  bool ShowMidpoints() const;
  virtual void OnShowMidpoints();

  bool ShowFullMesh() const;
  virtual void OnShowFullMesh();

private:
  void UpdateFormationElementSets();

private:
  typedef enum { FULL, MIDPOINTS, SKIN } TShowState;

  TShowState m_ShowState;
};

template <class FORMATION>
CFormationEntryTempl<FORMATION>::CFormationEntryTempl(int nEntryId, CModelBase &model)
    : CStorageNodeEntry<FORMATION>(nEntryId, IDI_FORMATIONS, IDS_TREE_FORMATIONS, model), m_ShowState(FULL) {}

template <class FORMATION> bool CFormationEntryTempl<FORMATION>::CanExportGocad() const {
  return ((static_cast<const CModelBase &>(this->Model())).Mesh().IsMesh());
}

template <class FORMATION> bool CFormationEntryTempl<FORMATION>::ShowSkin() const { return m_ShowState == SKIN; }

template <class FORMATION> void CFormationEntryTempl<FORMATION>::OnShowSkin() {
  if (m_ShowState != SKIN) {
    m_ShowState = SKIN;
    UpdateFormationElementSets();
  }
}

template <class FORMATION> bool CFormationEntryTempl<FORMATION>::ShowMidpoints() const {
  return m_ShowState == MIDPOINTS;
}

template <class FORMATION> void CFormationEntryTempl<FORMATION>::OnShowMidpoints() {
  if (m_ShowState != MIDPOINTS) {
    m_ShowState = MIDPOINTS;
    UpdateFormationElementSets();
  }
}

template <class FORMATION> bool CFormationEntryTempl<FORMATION>::ShowFullMesh() const { return m_ShowState == FULL; }

template <class FORMATION> void CFormationEntryTempl<FORMATION>::OnShowFullMesh() {
  if (m_ShowState != FULL) {
    m_ShowState = FULL;
    UpdateFormationElementSets();
  }
}

template <class FORMATION> void CFormationEntryTempl<FORMATION>::UpdateFormationElementSets() {
  // Update all formation volumes
  COperation oper(CStorageNodeEntry<FORMATION>::Model());

  typename CStorageNodeEntry<FORMATION>::TNodeSet stFormation = CStorageNodeEntry<FORMATION>::EntryNodes();
  for (typename CStorageNodeEntry<FORMATION>::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end();
       it++) {
    FORMATION &formation = **it;
    for (int i = 0; i < formation.ElementSetSize(); i++)
      formation.ElementSet(i).Modified();
  }
}

// typedef CGraphEntryTemp<CFormationBase> TFormationBaseEntry;
class TFormationBaseEntry : public CGraphEntryTemp<CFormationBase> {
public:
  TFormationBaseEntry(int nEntryId, unsigned int uIconId, const QString &strName, CFemAppModel &model);
  TFormationBaseEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model);

protected:
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
};

// typedef COpenGLNodeObserver_Delegate<CFormationBase, CFormationBase_Delegate, CDummyNode, CDummyObserver, false,
// FIXED_ITEM> TFormationObserver;

#endif // !defined(AFX_FORMATIONBASE_H__419B2161_22C3_4E11_8F81_75C3590EDF72__INCLUDED_)
