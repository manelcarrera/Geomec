// PressureSupportNode.h: interface for the CPressureSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSURESUPPORTNODE_H__5C93407F_265D_4686_9161_59737716453B__INCLUDED_)
#define AFX_PRESSURESUPPORTNODE_H__5C93407F_265D_4686_9161_59737716453B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DSupportNode.h"

namespace geo {
  class ISurface;
}

class CModelBase;

class CPressureSupportNode : public C3DSupportNode  
{
  friend class CFaceSupportDlg;

public:
  class CSurfaceDef
  {
    friend class CPressureSupportNode;
    CSurfaceDef(); // only for loading purposes
  public:
    CSurfaceDef(const geo::ISurface &s);
    bool operator<(const CSurfaceDef &rhs) const;
    bool operator==(const CSurfaceDef &rhs) const;

    void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
    void SaveStream(TSTREAM& stream, TPROGRESS& progress) const;

  private:
    geo::CPoint m_ptMin;
    geo::CPoint m_ptMax;
  };

  typedef enum
  {
    SL_SIDE = 0,
    SL_TOP,
    SL_BOTTOM
  } TSurfaceLocation;

  class CFaceSupportDef
  {
  public:
    class CSupportDef
    {
    public:
      typedef enum _PressureType
      {
        PT_UNLOADED = 0,
        PT_LOCAL,
        PT_GLOBAL
      } TPressureType;

      CSupportDef();
      ~CSupportDef();

      bool operator==(const CSupportDef &rhs) const;

      TPressureType PressureType() const;
      void PressureType(TPressureType nType);
      const geo::IVector &LocalVector() const;
      geo::IVector &LocalVector();

      void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
      void SaveStream(TSTREAM& stream, TPROGRESS& progress);

    private:
      TPressureType m_nPressureType;
      geo::CVector m_vecLocal;
    };

    typedef enum _SupportType
    {
      ST_NORMAL = 0,
      ST_FULL,
      ST_STRESS,
      ST_HORIZONTAL,
      ST_UNSUPPORTED,
      ST_VERTICAL
    } TSupportType;

    CFaceSupportDef(CMeshBase& mesh);
    CFaceSupportDef(TSurfaceLocation SurfaceLocation, CMeshBase& mesh);
    CFaceSupportDef(const CFaceSupportDef &rhs);
    CFaceSupportDef &operator=(const CFaceSupportDef &rhs);
    bool operator==(const CFaceSupportDef &rhs) const;

    TSupportType SupportType() const;
    void SupportType(TSupportType nType);

    TSurfaceLocation SurfaceLocation() const;
    void SurfaceLocation(TSurfaceLocation SurfaceLocation);

//		bool PreScribedDisplacement() const;
//		void PreScribedDisplacement(bool bEnable);

    const CSupportDef &SupportDef(const CDepletionStage &stage) const;
    CSupportDef &SupportDef(const CDepletionStage &stage);
    void Update(const CDepletionStage &Initial);

    void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress, const CModelBase &model);
    void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  private:
    typedef std::map<const CDepletionStage*, CSupportDef> TSupportMap;
    TSupportMap m_SupportMap;
    TSupportType m_nSupportType;
//		bool m_bPreScribedDisplacement;
    TSurfaceLocation m_SurfaceLocation;
  CMeshBase& m_mesh;
  };

  CPressureSupportNode(CInterfaceBoundary &boundary);
  CPressureSupportNode(const CPressureSupportNode &rhs);
  virtual ~CPressureSupportNode();
  virtual bool Less(const CGraphNode &node) const;
  
  virtual unsigned int IconId() const;

// Save and load stream
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual bool Empty() const;
  virtual long SavedItems() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  void OnMeshModified();

  virtual COpenGLNode::CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;

  virtual bool UsingGlobalTensor(const CDepletionStage &Stage) const;
  virtual void OnGlobalTensorInputUndefined(const CDepletionStage &Stage) const;

  const CFaceSupportDef &FaceSupportDef(const CSurfaceDef &SurfaceDef) const;

  geo::CVector StressFromDistribution(const geo::IPoint &pt, const CDepletionStage &stage, const geo::IVector &VecNorm) const;

  virtual bool IsValueTypeAllowed(const CDepletionStage& stage) const;
  virtual bool IsValidValueTypeId(unsigned int uValueType) const;

  typedef std::vector <std::pair <const geo::ISurface*, TSurfaceLocation> >
  TSurfaceVec;

  TSurfaceVec getSurfaces() const;
  void setSelectedSurface(const geo::ISurface* selectedSurface);
  const geo::ISurface* getSelectedSurface() const;

  ACCEPT_GEOMECMODELVISITORS(VisitPressureSupportNode);

private:
  class CDefaultDrawDef : public COpenGLNode::CDrawDef
  {
  public:
    CDefaultDrawDef(const COpenGLNode& node);
    virtual DrawDecisionFloat LineWidth() const;
  };

  class CFilledSurfaceDrawDef : public COpenGLNode::CDrawDef
  {
  public:
    CFilledSurfaceDrawDef(const COpenGLNode& node);
    virtual DrawDecisionVector Color(const geo::IObject &object) const;
    virtual DrawDecisionBool PolyFillFront() const;
    virtual DrawDecisionBool PolyFillBack() const;
  };

  class CTransparentSurfaceDrawDef : public COpenGLNode::CDrawDef
  {
  public:
    CTransparentSurfaceDrawDef(const COpenGLNode& node);
    virtual DrawDecisionVector Color(const geo::IObject &object) const;
    virtual DrawDecisionBool PolyFillFront() const;
    virtual DrawDecisionBool PolyFillBack() const;
  };

  mutable TSurfaceVec m_vcSurfaces;
  mutable const geo::ISurface *m_pSelectedSurface;

public:
  typedef std::map<CSurfaceDef, CFaceSupportDef> TStageSupportMap;

  TStageSupportMap& getSupports() const;

private:
  mutable TStageSupportMap m_Supports;

  void BuildSurfaceVector() const;
  void BuildStageSupportMap() const;
  void UpdateStageSupportMap() const;
};

#endif // !defined(AFX_PRESSURESUPPORTNODE_H__5C93407F_265D_4686_9161_59737716453B__INCLUDED_)
