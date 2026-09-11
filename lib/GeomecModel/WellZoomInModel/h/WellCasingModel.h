#ifndef _WELLCASINGMODEL_H_
#define _WELLCASINGMODEL_H_

#include "IWellModel.h"
#include "WellCasingMesh.h"
#include "WellFormation.h"
#include "WellZoomInModel.h"

class CWellZoomInModel;
class CWellCasingNode;
class CWellCasingCement;
class CWellCasingMesh;

#include "TensorValueSetTemplate.h"

class IDeformation;

class CWellCasingModel : public IWellModel {
public:
  IDeformation *m_deformation;

private:
  friend class IModelLifetimeFacade;

  CWellCasingModel(CWellZoomInModel &parent, CAnalysisLogger &logger, const CVersionManager &versionManager);
  ~CWellCasingModel();

public:
  CWellCasingModel(const CWellCasingModel &rhs);
  CWellCasingModel &operator=(const CWellCasingModel &rhs);
  bool operator==(const CWellCasingModel &rhs) const;

  virtual void createContainers();
  virtual void createDefaults();
  virtual void CreateChildren();

  virtual unsigned int ModelIconId() const;
  virtual unsigned int TypeId() const;
  virtual QString documentType() const;

  virtual void CreateDensityPoints(const geo::IElement &element, CPointSet &pointset);
  virtual CGeomecDianaRunnerBase *OnCreateDianaRunner(CDianaRunController &controller);

  virtual const double &Depth(const geo::ICoordinate &coord) const;
  virtual double Northing(const geo::ICoordinate &coord) const;
  virtual double Easting(const geo::ICoordinate &coord) const;
  virtual bool IsResult() const;
  virtual void Calculate();
  virtual int Dimension() const;

  virtual const CWellZoomInModel &ParentModel() const;
  virtual CWellZoomInModel &ParentModel();

  virtual CWellCasingMesh &Mesh();
  virtual const CWellCasingMesh &Mesh() const;
  virtual bool CanCalculate() const;
  virtual bool CanCalculateMixture() const;
  virtual bool CanCalculateHeatFlow() const;

  virtual void OnCloseModel();

  virtual long SavedItems() const;
  bool LoadWellCasing(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  virtual CWellPath *WellPath();
  virtual const CWellPath *WellPath() const;
  virtual CNewWellPath *NewWellPath();
  virtual const CNewWellPath *NewWellPath() const;

  CWellCasingNode &CasingNode();
  const CWellCasingNode &CasingNode() const;

  CWellCasingCement &CasingCement();
  const CWellCasingCement &CasingCement() const;

  virtual void CollectBoundaryNodes(TNodeElementsMap &mpNodeElements) const;

  virtual bool SaltInitRelax() const { return true; }
  virtual bool CanDisableSaltInitRelax() const { return false; }

  void SwitchToWarning();

  virtual void SwitchTo(CFemAppModel *currentModel);

  virtual int childModelSize() const;
  virtual CModelBase &childModel(int index);
  virtual CModelBase *parentModel();

  virtual ModelType modelType() const;
  virtual bool IsCylindricalModel() const { return true; }

  void InvalidateDeformation();

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingModel);

protected:
  virtual QString AttributesDialogCaption() const;
  virtual CDepletionStage::eIterationScheme DefaultIterationScheme(const CDepletionStage &source_stage) const;

private:
  CWellZoomInModel &m_parent;
  CWellCasingMesh *m_pMesh;
  CWellCasingNode *m_pCasingNode;
  CWellCasingCement *m_pCasingCement;

  static bool s_bBetaWarningSeen;
};

class CWellCasingFormationEntry : public CWellFormationEntry {
public:
  CWellCasingFormationEntry(int nEntryId, CWellCasingModel &model);

  virtual void OnShowSkin();
  virtual void OnShowMidpoints();
  virtual void OnShowFullMesh();

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingFormationEntry);
};

#endif // _WELLCASINGMODEL_H_
