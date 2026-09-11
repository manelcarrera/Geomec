#ifndef _IWELLMODEL_H_
#define _IWELLMODEL_H_

#include "ModelBase.h"
#include "BoundaryBase.h"
#include "InterfaceElement.h"
#include "BoundaryInterfaceDef.h"
#include "DepletionStage.h"

class CWellPath;
class CNewWellPath;
class CWellFormation;
class CFormationSection;
class CNewFormationSection;

namespace geo {
  class CInterfaceElement;
}

#include "BaseSupportNode.h"

class IWellModel : public CModelBase
{
public:
  class CPositionDef : public CGraphNode
  {
  public:
    typedef enum
    {
      PDM_DEFAULT = 0,
      PDM_AHD,
      PDM_FORMATION
    } TMethod;

    /*
    enum
    {
      PD_POSCHANGED = 1
    };
    */
  public:
    CPositionDef(IWellModel& model, bool bStart);
    CPositionDef(const CPositionDef& rhs);

    CPositionDef& operator=(const CPositionDef& rhs);
    bool operator==(const CPositionDef& rhs) const;

    virtual unsigned int IconId() const { return 0; }
    virtual unsigned int TypeId() const { return 0; }

    virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

    // input method
    TMethod Method() const;

    // the resulting AHD and formation
    double AHD() const;
    bool InsideFormation() const; // is the position inside a formation?
    const CFormationBase& Formation() const;

    void SetDefault();
    void SetAHD(double dAHD);
    void SetFormation(const CFormationBase& formation);

    void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
	  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);

  private:
    IWellModel& m_model;
    TMethod m_nMethod;
    double m_dAHD;
    const CFormationBase* m_pFormation;
    bool m_bStart;
  };

  /*
  enum
  {
    GEOMETRY_CHANGED = 1
  };
  */

  class CBoundary : public CBoundaryBase
  {
  public:
    class IWellSupportNode : public CBaseSupportNode
    {
    public:
      IWellSupportNode(unsigned int uName, IWellModel::CBoundary& boundary);
	    virtual unsigned int TypeId() const;
	    virtual bool Empty() const;
	    virtual int DisplayListSize() const;
	    virtual const geo::IObject& DisplayList(int nIndex) const;
	    virtual TColor Color() const;

    private:
      IWellModel::CBoundary& m_boundary;
    };

    class CDisplacementSupport : public IWellSupportNode
    {
    public:
      CDisplacementSupport(IWellModel::CBoundary& boundary);
      virtual bool IsValidValueTypeId(unsigned int uValueType) const;
	    virtual unsigned int IconId() const;
    };

    class CPressureSupport : public IWellSupportNode
    {
    public:
      CPressureSupport(IWellModel::CBoundary& boundary);
      virtual bool IsValidValueTypeId(unsigned int uValueType) const;
	    virtual unsigned int IconId() const;
    };

  public:
    CBoundary(CFemAppModel& model);
    ~CBoundary();

  	virtual TMinMax SnapToGrid(const TMinMax& minmax) const;

	  virtual const geo::IObject& DisplayList(int nIndex) const;
	  virtual int DisplayListSize() const;
    virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

    void Clear();
    void AddBoundaryElement(geo::CInterfaceElement& iface, CBoundaryInterfaceDef::TBoundarySurface surfacetype);
  	const CBoundaryInterfaceDef &InterfaceDefinition() const;

    int BoundaryElementSize() const;
    const geo::CInterfaceElement& BoundaryElement(int nIndex) const;

    CBoundaryInterfaceDef::TBoundarySurface BoundarySurfaceDef(const geo::CInterfaceElement& iface) const;
    const CBoundaryInterfaceMaterial& InterfaceMaterial(const geo::CInterfaceElement& iface) const;

	  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	  virtual long SavedItems() const;

    virtual bool HasInterfaces() const;
	  virtual int InterfaceNodeSize() const;
	  virtual const geo::INode& InterfaceNode( int nIndex ) const;

    const CDisplacementSupport& DisplacementSupport() const;
    const CPressureSupport& PressureSupport() const;

  protected:
    virtual bool InvalidateMeshOnSet() const { return false; }

  private:
    void CreateChildren();

  private:
    geo::CPtrArray<geo::CInterfaceElement> m_arBoundaryElements;
   	CBoundaryInterfaceDef *m_pBoundaryInterfaceDef;
    typedef std::map<const geo::CInterfaceElement*, CBoundaryInterfaceDef::TBoundarySurface> TFace2BoundarySurfaceMap;
    TFace2BoundarySurfaceMap m_mpFace2BoundarySurface;
    typedef std::vector<const geo::INode*> TNodeVec;
    mutable TNodeVec m_vcInterfaceNode;
    CDisplacementSupport* m_pDisplacementSupport;
    CPressureSupport* m_pPressureSupport;
  };

protected:
  IWellModel(CAnalysisLogger& logger, const CVersionManager& versionManager);
  virtual ~IWellModel();

  virtual void CloseModel();
  virtual void OnCloseModel();

public:
  IWellModel(const IWellModel& rhs);
  IWellModel& operator=(const IWellModel& rhs);
  bool operator==(const IWellModel& rhs) const;

	virtual void createDefaults();
  virtual void createContainers();
  virtual void CreateChildren();

  virtual const double &Depth(const geo::ICoordinate &coord) const;
	virtual double Northing(const geo::ICoordinate& coord) const;
	virtual double Easting(const geo::ICoordinate& coord) const;

  virtual const CModelBase& ParentModel() const = 0;
  virtual CModelBase& ParentModel() = 0;

  virtual const CModelBase& RootModel() const;
  virtual CModelBase& RootModel();

  virtual bool CanCalculateHeatFlow() const;
  virtual bool CanCalculateMixture() const;
  virtual bool CanUseCalculatedTemperatures() const;

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
	virtual bool OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
	virtual bool OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  double Radius() const;
  void Radius(double dRadius);

  void DestroyZoomInModel();
  virtual bool Empty() const;

  CWellFormation* AssociatedFormation(const CFormationBase& formation); // returns 0 if no formation is associated (zoom-in model doesn't run through it)
  const CWellFormation* AssociatedFormation(const CFormationBase& formation) const;

  virtual CWellPath* WellPath() = 0;
  virtual const CWellPath* WellPath() const = 0;

  virtual CNewWellPath* NewWellPath() = 0;
  virtual const CNewWellPath* NewWellPath() const = 0;

  const CPositionDef& StartPosition() const;
  CPositionDef& StartPosition();
  const CPositionDef& EndPosition() const;
  CPositionDef& EndPosition();

  double FirstValidAHD() const;
  double LastValidAHD() const;

  // NULL if not in any formation
  const CFormationBase* FormationContaining(double dAHD) const;

  // AHD at which the well enters or exits the given formation
  // can be invalid (if the well does not cross the formation)
  geo::CValue FormationEntryAHD(const CFormationBase& formation) const;
  geo::CValue FormationExitAHD(const CFormationBase& formation) const;

  std::vector<const CFormationBase*> ValidFormations() const;

  bool HasDistributedBoundaryLoads() const;

  virtual ModelType modelType() const;

  void setModelProxy(CModelProxy* modelProxy);
  virtual void SwitchTo(CFemAppModel* currentModel);

  ACCEPT_GEOMECMODELVISITORS(VisitIWellModel);

protected:
  void CreateBoundary();

public:
  virtual QString AttributesDialogCaption() const = 0;
  void addFormation(const CFormationBase* formation);

private:
  void DetectFormations();
  void ClearFormations();
  const CFormationSection* FormationSection(const CFormationBase& formation) const;
  const CNewFormationSection* NewFormationSection
    (const CFormationBase& formation) const;

private:
  double m_dRadius; // the radius, measured from the center to a node
  CPositionDef m_StartPosDef;
  CPositionDef m_EndPosDef;

  typedef std::map<const CFormationBase*, CWellFormation*> TFormationMap;
  TFormationMap m_mpFormations;

  CModelProxy* m_modelProxy;
};

#endif // _IWELLMODEL_H_
