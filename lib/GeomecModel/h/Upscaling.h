#ifndef _UPSCALINGBRANCH_H_
#define _UPSCALINGBRANCH_H_

class CFormationBase;

#include "MeshSampler.h"
#include "GeomecModelVisitor.h"

class C3DModel;
class CUpscalingFormationsBranch;
class CUpscalingPointsetRangesBranch;
class CUpscalingTargetPointsetBranch;

#include "StorageNode.h"
#include "openglnode.h"

class IUpscalingNodeBase : public CStorageNode
{
public:
  IUpscalingNodeBase(CFemAppModel& model);
  IUpscalingNodeBase(const QString& strName, C3DModel& model);
  IUpscalingNodeBase(const unsigned int uName, C3DModel& model);
  IUpscalingNodeBase(const IUpscalingNodeBase &rhs);

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  const CUpscalingFormationsBranch& FormationsBranch() const;
  CUpscalingFormationsBranch &FormationsBranch();
  const CUpscalingPointsetRangesBranch &PointsetRangesBranch() const;
  CUpscalingPointsetRangesBranch &PointsetRangesBranch();
  const CUpscalingTargetPointsetBranch &TargetPointsetBranch() const;
  CUpscalingTargetPointsetBranch &TargetPointsetBranch();

  virtual int FactoryType() const = 0;
  virtual bool CanExecute() const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIUpscalingNodeBase);

protected:
  static const QString DEFAULT_2D_POINT_SET_NAME;
  static const QString DEFAULT_2D_POINT_SET_NUMBER;
  static const QString DEFAULT_3D_POINT_SET_NAME;
  static const QString DEFAULT_3D_POINT_SET_NUMBER;
  static const size_t DEFAULT_INITIAL_NUMBER;

private:
  CUpscalingFormationsBranch* m_pFormationsBranch;
  CUpscalingPointsetRangesBranch* m_pPointsetRangesBranch;
  CUpscalingTargetPointsetBranch* m_pTargetPointsetBranch;
};

const int FACTORYTYPE_THINLAYERUPSCALING = 1;
const int FACTORYTYPE_CAMCLAYUPSCALING = 2;

class CUpscalingNodeFactory
{
public:
  CUpscalingNodeFactory(C3DModel& model);
  IUpscalingNodeBase* Create(int nType);

private:
  C3DModel& m_model;
};

class CUpscalingEntry : public CGraphEntryTemp<IUpscalingNodeBase>
{
public:
  CUpscalingEntry(C3DModel& model);

  long SavedItems() const;
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
  void LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& progress);

  void CreateThinLayerUpscaling();
  void CreateCamClayUpscaling();

  ACCEPT_GEOMECMODELVISITORS(VisitUpscalingEntry);
};

class IFormationFilter;
class CUpscalingFormationsBranch : public COpenGLNode
{
public:
  CUpscalingFormationsBranch(CFemAppModel& model);
  CUpscalingFormationsBranch(IUpscalingNodeBase& parent);
  CUpscalingFormationsBranch(const CUpscalingFormationsBranch &rhs);

  bool HasFormations() const;

  virtual	bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;
  virtual TColor Color() const;

  bool IsInRange(const geo::IBody& body) const;
  void FormationFilter(const IFormationFilter *formationFilter){ m_formationFilter = formationFilter; }
  const IFormationFilter *FormationFilter() const { return m_formationFilter; }

  ACCEPT_GEOMECMODELVISITORS(VisitUpscalingFormationsBranch);

protected:
 	virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

private:
  class CFormationDrawDef : public COpenGLNode::CDrawDef
  {
  public:
  CFormationDrawDef(const CFormationBase& formation);
    virtual DrawDecisionVector Color(const geo::IObject &object) const;
  };

private:
  const IFormationFilter *m_formationFilter;
  typedef std::set<const CFormationBase*> TFormationsSet;
  TFormationsSet m_stFormations;
};

class IUpscalingPointsetBranchBase : public COpenGLNode
{
public:
  IUpscalingPointsetBranchBase(CFemAppModel& model);
  IUpscalingPointsetBranchBase(const QString& strName, IUpscalingNodeBase& parent);
  IUpscalingPointsetBranchBase(const IUpscalingPointsetBranchBase& rhs);

  virtual unsigned int TypeId() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual TColor Color() const;

  ACCEPT_GEOMECMODELVISITORS(VisitIUpscalingPointsetBranchBase);
};

class CUpscalingPointsetRangesBranch : public IUpscalingPointsetBranchBase
{
public:
  CUpscalingPointsetRangesBranch(CFemAppModel& model);
  CUpscalingPointsetRangesBranch(IUpscalingNodeBase& parent);
  CUpscalingPointsetRangesBranch(const CUpscalingPointsetRangesBranch& rhs);

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual unsigned int IconId() const;

  virtual bool CanConnectItem(const CGraphNode& node) const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;

  bool UseAllPointsets() const;
  void UseAllPointsets(bool b);

  bool IsInRange(const geo::IPoint& point) const;

  ACCEPT_GEOMECMODELVISITORS(VisitUpscalingPointsetRangesBranch);

protected:
 	virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

private:
  class CPointsetDrawDef : public COpenGLNode::CDrawDef
  {
  public:
  CPointsetDrawDef(const CPointSet& pointset);
    virtual DrawDecisionVector Color(const geo::IObject &object) const;
  };

private:
  typedef std::set<const CPointSet*> TPointsetSet;
  TPointsetSet m_stPointsets;
  bool m_bAllPointsets;
};

class ITargetPointsetSource;
class CUpscalingTargetPointsetBranch : public IUpscalingPointsetBranchBase
{
public:
  CUpscalingTargetPointsetBranch(CFemAppModel& model);
  CUpscalingTargetPointsetBranch(IUpscalingNodeBase& parent);
  CUpscalingTargetPointsetBranch(const CUpscalingTargetPointsetBranch& rhs);
  ~CUpscalingTargetPointsetBranch();

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual unsigned int IconId() const;

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  bool HasPointset() const;

  CPillarMap::const_iterator begin() const;
  CPillarMap::const_iterator end() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual TColor Color() const;

  void CopyPillarMapAsPointset(const CPointSet& set2d, const QString& sName);

  void CreateFromRegular2DGrid(const geo::IPoint& corner1,
  const geo::IPoint& corner2, int pointsNorthing, int pointsEasting);
  bool CanCreateFromRegular2DGrid() const;
  void CreateFromExisting2D(const IPointSet& pointSet);
  bool CanCreateFromExisting2D() const;
  void CreateFromSurfaceNodes(const IPointSet& pointSet);
  bool CanCreateFromSurfaceNodes() const;
  void CreateFromHexaMidpoints();
  bool CanCreateFromHexaMidpoints() const;

  ACCEPT_GEOMECMODELVISITORS(VisitUpscalingTargetPointsetBranch);

private:
  void CreateTargetPointset(const ITargetPointsetSource& source);

private:
  CPillarMap* m_pPillarMap;
};
#endif // _UPSCALINGBRANCH_H_