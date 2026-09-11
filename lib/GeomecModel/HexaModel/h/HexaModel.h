// HexaModel.h: interface for the CHexaModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXAMODEL_H__957DC602_268D_4B70_BC0A_75F5ED7183DA__INCLUDED_)
#define AFX_HEXAMODEL_H__957DC602_268D_4B70_BC0A_75F5ED7183DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3dmodel.h"

// ##ModelId=3BC55D6103AF
class CSurfaceBase;
class CHexaMesh;

class CHexaModel : public C3DModel {
  long SavedItemsMeshRegions() const;
  void SaveMeshRegions(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);
  void LoadMeshRegions(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);

  // Pointer to the mesher
  CHexaMesh *m_pMesh;

protected:
  virtual void CreateDensityPoints(const geo::IElement &element, CPointSet &pointset);
  virtual bool IsHexaModel() const { return true; }

public:
  // Meshing Functions
  virtual void createContainers();
  virtual void createDefaults();

private:
  friend class IModelLifetimeFacade;

  // ##ModelId=3BC55D6103C1
  CHexaModel(CAnalysisLogger &logger, const CVersionManager &versionManager);
  // ##ModelId=3BC55D6103C3
  virtual ~CHexaModel();

public:
  // ##ModelId=3BC55D6103C5
  virtual unsigned int ModelIconId() const;
  // ##ModelId=3BC55D6103CE
  virtual QString documentType() const;

  static unsigned int staticModelIconId();
  static QString staticDocumentType();

  // ##ModelId=3BC55D6103D0
  // ##ModelId=3BC55D6103D3
  virtual void OnCloseModel();
  bool CreateHorizon(const CSurfaceBase &s);

  bool LoadHexa1(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  bool LoadHexa2(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  bool LoadHexa3(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog);

  // Mesh/state methods.
  virtual bool IsResult() const;

  virtual void Calculate();

  virtual CMeshBase &Mesh();
  virtual const CMeshBase &Mesh() const;

  virtual ModelType modelType() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaModel);

protected:
  virtual long SavedItems() const;
};

#endif // !defined(AFX_HEXAMODEL_H__957DC602_268D_4B70_BC0A_75F5ED7183DA__INCLUDED_)
