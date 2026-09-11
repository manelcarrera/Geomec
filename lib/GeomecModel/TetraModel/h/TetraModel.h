// TetraModel.h: interface for the CTetraModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRAMODEL_H__B46CAEDA_5160_49A2_8CB8_FC08B7780A09__INCLUDED_)
#define AFX_TETRAMODEL_H__B46CAEDA_5160_49A2_8CB8_FC08B7780A09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3dmodel.h"

class CTetraMesh;
class CTetraSubBoundary;
class C3DSupportNode;

class CMeshRefinementModel;

#include "openglnode.h"

class CTetraModel : public C3DModel  
{
  public: //FIXME

  CMeshRefinementModel* m_mesh_refinement_data;

  CTetraMesh* m_pMesh;
  CTetraSubBoundary *m_pSubBoundary;

  bool CheckZoomInBoundaryLoads(const C3DSupportNode& supportnode) const;

protected:
  virtual void CreateDensityPoints(const geo::IElement &/*element*/, CPointSet &/*pointset*/) { assert(false); }
  virtual bool OnModelSpecificAnalysisCheck(const CAnalysisType& antype, bool bWriteInputFiles, bool bWriteQuadDat);

public:
  virtual void createContainers();
  virtual void createDefaults();
  virtual void OnCloseModel();
  //##ModelId=3B6905BF01C5
  virtual unsigned int ModelIconId() const;
  //##ModelId=3B6905BF01D5
  virtual QString documentType() const;

  static unsigned int staticModelIconId();
  static QString staticDocumentType();

protected:
  //##ModelId=3B6905BF01F4
  CTetraModel(CAnalysisLogger& logger, const CVersionManager& versionManager);
  //##ModelId=3B6905BF01F6
  virtual ~CTetraModel();

public:
  // Mesh/state methods.
  virtual bool IsResult() const;

  virtual CMeshBase& Mesh();
  virtual const CMeshBase& Mesh() const;

  const CTetraSubBoundary& SubBoundary() const;
  CTetraSubBoundary& SubBoundary();

  virtual void Calculate();

  virtual long SavedItems() const;
  bool LoadTetra1(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  bool LoadTetra2(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  virtual ModelType modelType() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTetraModel);
};

// Sub model entry contains :
// - All faults in sub model
// - All horizons in sub model
// - The boundary in the sub model

typedef CGraphEntryTemp<COpenGLNode> TSubModelEntry;

#endif // !defined(AFX_TETRAMODEL_H__B46CAEDA_5160_49A2_8CB8_FC08B7780A09__INCLUDED_)
