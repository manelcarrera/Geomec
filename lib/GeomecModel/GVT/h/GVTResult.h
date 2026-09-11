#pragma once


#include "Result.h"
#include "StreamVersion.h"


class CGVTResultVersion: public CStreamVersion
{
public:
  CGVTResultVersion() {}
  CGVTResultVersion(int nMajor, int nMinor, int nRevision) : CStreamVersion(nMajor, nMinor, nRevision) {}
  CGVTResultVersion(int nMajor, int nMinor) : CStreamVersion(nMajor, nMinor) {}
};


class CGVTFactor : public IResult
{
public:
  class CGVTFactorComponent : public IResultComponent
  {
    geo::CValue getElementFactor(const geo::IElement& elm) const;

  public:
    CGVTFactorComponent(const QString& strName, CGVTFactor& parent, const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
    virtual ~CGVTFactorComponent();

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual	QString UnitName(const UNIT unit) const;
    virtual long SavedItems() const;
    virtual QString ExportLabel() const;

    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

    virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
  };

  CGVTFactor(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group);
  virtual ~CGVTFactor();

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Less(const CGraphNode &node) const;
  virtual bool OnDefined(const IResultComponent& component) const;
  virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
  virtual QString ExportLabel(int nComponent) const;
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  const QString& StoredExportLabel() const;
  QString UnitName(const UNIT unit) const;
  double UnitFactor(const UNIT unit) const;

  IValueComponentBase& Component(unsigned int /*uComponent*/, unsigned int /*uMode*/);
  const IValueComponentBase& Component(unsigned int /*uComponent*/, unsigned int /*uMode*/) const;

  ACCEPT_GEOMECMODELVISITORS(VisitGVTFactor);

protected:
  virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
  virtual IResultComponent* OnGetResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const;

private:
  unsigned int m_uiValueTypeID;
  unsigned int m_uiIconID;
  QString m_strExportLabel;

  typedef std::vector<CGVTFactorComponent *> TComponentVector;
  TComponentVector m_vcComponents;
};




class CGVTResultGroup : public CResultGroup
{
public:
  CGVTResultGroup(CResultGroup& group);

  virtual unsigned int IconId() const;

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  long SavedItems() const;
  void LoadStream(CStorageNode::TSTREAM& stream, CGVTResultVersion& version, CStorageNode::TPROGRESS& prog);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prog);
 
  ACCEPT_GEOMECMODELVISITORS(VisitGVTResultGroup);
};



class CGVTResultTree : public CResultGroup
{
public:
  CGVTResultTree(CModelBase& model);
  virtual unsigned int IconId() const;

  long SavedItems() const;
  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& prog);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& prog);

  ACCEPT_GEOMECMODELVISITORS(VisitGVTResultTree);

private:
  CGVTResultGroup *m_pOutput;
  CDerivedResultGroup *m_pDerived;
};