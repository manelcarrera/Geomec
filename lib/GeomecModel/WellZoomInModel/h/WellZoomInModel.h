#ifndef _WELLZOOMINMODEL_H_
#define _WELLZOOMINMODEL_H_

#include "IWellModel.h"
#include "WellZoomInMesh.h"

class CWellPath;
class CNewWellPath;
class CWellFormation;
class CBoundaryInterfaceMaterial;
class CWellCasingModel;

#include "NewModel.h"

class CWellZoomInMesh;

class CWellZoomInModel : public IWellModel {
private:
  friend class IModelLifetimeFacade;

  CWellZoomInModel(CFemAppModel &model, CWellPath &wellpath, CAnalysisLogger &logger,
                   const CVersionManager &versionManager);
  CWellZoomInModel(CFemAppModel &model, CNewWellPath &wellpath, CAnalysisLogger &logger,
                   const CVersionManager &versionManager);
  virtual ~CWellZoomInModel();

public:
  CWellZoomInModel(const CWellZoomInModel &rhs);
  CWellZoomInModel &operator=(const CWellZoomInModel &rhs);
  bool operator==(const CWellZoomInModel &rhs) const;

  virtual void createContainers();
  virtual void createDefaults();

  virtual unsigned int ModelIconId() const;
  virtual unsigned int TypeId() const;
  virtual QString documentType() const;

  virtual void CreateDensityPoints(const geo::IElement &element, CPointSet &pointset);
  virtual CGeomecDianaRunnerBase *OnCreateDianaRunner(CDianaRunController &controller);

  virtual bool IsResult() const;
  virtual void Calculate();
  virtual int Dimension() const;

  virtual const CModelBase &ParentModel() const;
  virtual CModelBase &ParentModel();

  virtual CWellPath *WellPath();
  virtual const CWellPath *WellPath() const;
  virtual CNewWellPath *NewWellPath();
  virtual const CNewWellPath *NewWellPath() const;

  virtual CWellZoomInMesh &Mesh();
  virtual const CWellZoomInMesh &Mesh() const;

  virtual void OnCloseModel();

  virtual long SavedItems() const;
  bool LoadWellZoomIn1(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  bool LoadWellZoomIn2(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  CWellCasingModel &CasingModel();
  const CWellCasingModel &CasingModel() const;

  virtual int childModelSize() const;
  virtual CModelBase &childModel(int index);
  virtual CModelBase *parentModel();

  virtual ModelType modelType() const;

  void setNewModelFunction(bool (CNewModel::*newModel)(bool));
  CNewModel &getNewModel();

  ACCEPT_GEOMECMODELVISITORS(VisitWellZoomInModel);

private:
  bool (CNewModel::*m_newModelFunction)(bool);
  CNewModel m_newModel;

protected:
  virtual QString AttributesDialogCaption() const;

private:
  bool IsParentEntry(int nEntryType) const;

private:
  CWellPath *m_pWellPath;
  CNewWellPath *m_pNewWellPath;
  CWellZoomInMesh *m_pMesh;

  CWellCasingModel *m_pCasingModel;
};

#endif // _WELLZOOMINMODEL_H_
