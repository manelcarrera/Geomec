// 3dmodel.h: interface for the C3DModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DMODEL_H__0F641E59_DAEF_4F17_B8F0_7ABA92DEFDB9__INCLUDED_)
#define AFX_3DMODEL_H__0F641E59_DAEF_4F17_B8F0_7ABA92DEFDB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecModelVisitor.h"
#include "ICoordinate.h"
#include "ModelBase.h"

namespace geo {
class IFace;
}

class C3DHorizon;
class CMeasuredTopDisplacementsNode;

class C3DModel : public CModelBase {
protected:
  // Default scene
  virtual CGeomecDianaRunnerBase *OnCreateDianaRunner(CDianaRunController &controller);

  C3DModel(CAnalysisLogger &logger, const CVersionManager &versionManager);
  virtual ~C3DModel();

public:
  virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog);
  virtual long SavedItems() const;

  virtual const double &Depth(const geo::ICoordinate &coord) const { return coord.Z(); }
  virtual double Northing(const geo::ICoordinate &coord) const { return coord.X(); }
  virtual double Easting(const geo::ICoordinate &coord) const { return coord.Y(); }

  // This routine only creates the entries for the graph
  virtual void createContainers();
  // This routine creates all default items we want to have.
  virtual void createDefaults();
  virtual void OnCloseModel();

  virtual int Dimension() const { return 3; }

  virtual void CollectBoundaryNodes(TNodeElementsMap &mpNodeElements) const;

  const CMeasuredTopDisplacementsNode &MeasuredTopDisplacementsNode() const;
  CMeasuredTopDisplacementsNode &MeasuredTopDisplacementsNode();

  virtual ModelType modelType() const;

  ACCEPT_GEOMECMODELVISITORS(Visit3DModel);

private:
  void CollectNodesFromBodyFace(TNodeElementsMap &mpNodeElements, const geo::IFace &face) const;
  void CollectBoundaryNodesFromHorizon(TNodeElementsMap &mpNodeElements, const C3DHorizon &hor) const;

private:
  CMeasuredTopDisplacementsNode *m_pTopDisplacementsNode;
};

#endif // !defined(AFX_3DMODEL_H__0F641E59_DAEF_4F17_B8F0_7ABA92DEFDB9__INCLUDED_)
