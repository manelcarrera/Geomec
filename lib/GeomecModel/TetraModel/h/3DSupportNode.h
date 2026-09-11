// 3DSupportNode.h: interface for the C3DSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSUPPORTNODE_H__A15AB831_F7B7_4C64_B491_8520E8F03D24__INCLUDED_)
#define AFX_3DSUPPORTNODE_H__A15AB831_F7B7_4C64_B491_8520E8F03D24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseSupportNode.h"
#include "GeomecModelVisitor.h"

class CInterfaceBoundary;
class CMeshBase;

#include "mfTensor.h"

class ITensor;

class C3DSupportNode : public CBaseSupportNode  
{
public:
  typedef enum eGlobalTensorInput
  {
    GTI_UNDEFINED = 0,
    GTI_GENERAL = 1,
    GTI_VERTICAL = 2
  } TGlobalTensorInput;

  C3DSupportNode(unsigned int uName, CInterfaceBoundary &boundary);
  C3DSupportNode(const C3DSupportNode &rhs);
  virtual ~C3DSupportNode();

  C3DSupportNode &operator=(const C3DSupportNode &rhs);
  bool operator==(const C3DSupportNode &rhs) const;

  const CInterfaceBoundary &Boundary() const;
  CInterfaceBoundary &Boundary();

  virtual unsigned int TypeId() const { return 0; }
  virtual unsigned int IconId() const { assert(false); return 0; }
  virtual bool Empty() const { assert(false); return false; }
  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  
// Save and load stream
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;

  bool CanEditTensor() const;


  virtual TColor Color() const;

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  bool GlobalTensorDefined(const CDepletionStage &stage) const;
  const ITensor &GlobalTensor(const CDepletionStage &stage) const;
  ITensor &GlobalTensor(const CDepletionStage &stage);
  TGlobalTensorInput GlobalTensorInput(const CDepletionStage &stage) const;
  void GlobalTensorInput(const CDepletionStage &stage, TGlobalTensorInput nInput);

  virtual bool UsingGlobalTensor(const CDepletionStage &/*Stage*/) const { assert(false); return false; }

  virtual void OnGlobalTensorInputUndefined(const CDepletionStage &/*Stage*/) const { assert(false); }

  virtual bool BlocksDrawing() const { return false; }

  ACCEPT_GEOMECMODELVISITORS(Visit3DSupportNode);

protected:
  const CMeshBase &Mesh() const;
  void BuildSurfaceEdgeVector() const;
public:
  void EditGlobalTensor();

protected:
  typedef std::vector<geo::CArray<geo::CLine> > TSurfaceEdgeVec;

  mutable TSurfaceEdgeVec m_vcSurfaceEdges;

protected:
  class CLineLess
  {
  public:
    bool operator()(const geo::ILine *l1, const geo::ILine *l2) const
    { return (*l1) < (*l2); }
  };

private:
  class CGlobalTensorDef
  {
  public:
    CGlobalTensorDef();
    bool operator==(const CGlobalTensorDef &rhs) const;
    TGlobalTensorInput GlobalTensorInput() const;
    void GlobalTensorInput(TGlobalTensorInput nInput);
    const ITensor &Tensor() const;
    ITensor &Tensor();

    void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
    void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  private:
    TGlobalTensorInput m_nGlobalTensorInput;
    CTensor m_GlobalTensor;
  };

  typedef std::map<const CDepletionStage*, CGlobalTensorDef> TTensorMap;
  TTensorMap m_GlobalTensors;

  typedef std::map<const geo::IPoint*, int, geo::ICoordinate::CCoordinateLess> TLineMap;
};



#endif // !defined(AFX_3DSUPPORTNODE_H__A15AB831_F7B7_4C64_B491_8520E8F03D24__INCLUDED_)
