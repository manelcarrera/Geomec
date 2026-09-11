// TetraHorizonBase.cpp: implementation of the CTetraHorizonBase class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraModel.h"
#include "TetraHorizonBase.h"
#include "MeshBase.h"
#include "TetraMesh.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "BranchState.h"
#include "ModelBase.h"
#include "StreamVersion.h"
#include "ElementGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraHorizonBase::CTetraHorizonBase(CFemAppModel& model)
: C3DHorizon(model),
  m_pInterfaceElements(0)
{
}

CTetraHorizonBase::CTetraHorizonBase(CSurfaceBase &surface, CFemAppModel& model, bool bSlip, bool bAttachToEntry)
: C3DHorizon(surface, model, bSlip, bAttachToEntry),
  m_pInterfaceElements(0)
{
}
  
CTetraHorizonBase::CTetraHorizonBase(const QString& strInstanceName, const double dDepth, CFemAppModel& model, bool bAttachToEntry)
: C3DHorizon(strInstanceName, dDepth, model, bAttachToEntry),
  m_pInterfaceElements(0)
{
}

CTetraHorizonBase::CTetraHorizonBase(const QString& strInstanceName, CFemAppModel& model, bool bAttachToModel)
: C3DHorizon(strInstanceName, model, bAttachToModel),
  m_pInterfaceElements(0)
{
}

CTetraHorizonBase::CTetraHorizonBase(const C3DHorizon &rhs)
: C3DHorizon(rhs),
  m_pInterfaceElements(0)
{
}

CTetraHorizonBase::~CTetraHorizonBase()
{
  if(m_pInterfaceElements)
  delete m_pInterfaceElements;
}

void CTetraHorizonBase::init()
{
  // We link to the mesher
  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  LinkTo(model.Mesh());
}

bool CTetraHorizonBase::Flip() const
{
  return m_bFlip;
}

void CTetraHorizonBase::Flip(bool bFlip)
{
  m_bFlip = bFlip;
}

int CTetraHorizonBase::OutputSurfaceSize() const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  if(pModel->Mesh().IsMesh())
  {
    // We only can supply output when the mesh is build
    if(ConstantDepth()/* || Slip()*/)
      return 1;

    return SurfaceSize();
  }

  return 0;
}

void CTetraHorizonBase::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  C3DHorizon::LoadStream(stream, version, progress);

  if(CStreamVersion(3,0,10) < version)
  {
    int bFlip;
    stream >> bFlip;
    m_bFlip = bFlip;
  }

  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  LinkTo(model.Mesh());
}

void CTetraHorizonBase::SaveStream(TSTREAM& stream, TPROGRESS &progress)
{
  C3DHorizon::SaveStream(stream, progress);

  int bFlip = m_bFlip;
  stream << bFlip;
}

bool CTetraHorizonBase::operator==(const CTetraHorizonBase& rhs) const
{
  if(!C3DHorizon::operator ==(rhs))
    return false;

  return m_bFlip == rhs.m_bFlip;
}

CTetraHorizonBase& CTetraHorizonBase::operator=(const CTetraHorizonBase& rhs)
{
  C3DHorizon::operator=(rhs);

  m_bFlip = rhs.m_bFlip;

  return *this;
}


const geo::CSurfaceDesc& CTetraHorizonBase::OutputSurface(int nIndex) const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  assert(pModel->Mesh().IsMesh());

  // In case we have a mesh, we have tet surfaces ....
  const CTetraMesh& mesh = dynamic_cast<const CTetraMesh&>(pModel->Mesh()); 

  // Search for a slipping surface descriptor
  for(int nSurfaceDesc = 0; nSurfaceDesc < mesh.InputSurfaceSize(); nSurfaceDesc++)
  {
    // We can have a constant depth
    if(ConstantDepth())
    {
      assert(nIndex == 0);
      if(&GeneratedConstantDepthSurface() == &mesh.InputSurface(nSurfaceDesc).first->Surface())
        return *mesh.InputSurface(nSurfaceDesc).first;
    }
    else
    {
      if(&Surface(nIndex).Surface() == &mesh.InputSurface(nSurfaceDesc).first->Surface())
        return *mesh.InputSurface(nSurfaceDesc).first;
    }
  }

  assert(false);

  // Keep friends with compiler
  geo::CSurfaceDesc *pBogus = 0;
  return *pBogus;
}

int CTetraHorizonBase::MeshedSurfaceSize() const
{
  int iRet = 0;

  for(int i = 0; i < OutputSurfaceSize(); ++i)
  iRet += OutputSurface(i).TetSurfaceSize();

  return iRet;
}

const geo::ISurface& CTetraHorizonBase::MeshedSurface(int nIndex) const
{
  const geo::ISurface* pSurface = 0;

  for(int i = 0; i < OutputSurfaceSize() && !pSurface; ++i)
  {
  if(nIndex < OutputSurface(i).TetSurfaceSize())
      pSurface = &OutputSurface(i).TetSurface(nIndex);

  nIndex -= OutputSurface(i).TetSurfaceSize();
  }

  assert(pSurface);
  return *pSurface;
}

std::vector<const geo::ISurface*> CTetraHorizonBase::DisplaySurfaces() const
{
  std::vector<const geo::ISurface*> vcRet;
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  assert(pModel->Mesh().IsMesh());

  int size = 0;
  for (int i = 0; i < OutputSurfaceSize(); i++)
  {
  const geo::CSurfaceDesc &desc = OutputSurface(i);
  size += desc.TetSurfaceSize();
  }

  vcRet.reserve(size);

  for(int i = 0; i < OutputSurfaceSize(); i++)
  {
    const geo::CSurfaceDesc &desc = OutputSurface(i);
    for(int j = 0; j < desc.TetSurfaceSize(); j++)
    {
      vcRet.push_back(&desc.TetSurface(j));
    }
  }

  return vcRet;
}

const geo::CElementGroup* CTetraHorizonBase::InterfaceElementGroup() const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  assert(Slip());
//	assert(OutputSurfaceSize() == 1);

  if(!pModel->Mesh().IsMesh())
  return 0;

  if(m_pInterfaceElements)
  return m_pInterfaceElements;

  bool hasInterfaceElements = false;

  for (int i = 0; (i < OutputSurfaceSize()) && !hasInterfaceElements ; ++i)
  {
  hasInterfaceElements = (OutputSurface(i).Slip() &&
      (OutputSurface(i).interfaceElements().ElementSize() > 0));
  }

  if (hasInterfaceElements)
  {
  m_pInterfaceElements = new geo::CElementGroup(const_cast<geo::IMesh&>(pModel->Mesh().Mesh()));
  int i;
  for(i = 0; i < OutputSurfaceSize(); ++i)
  {
      int j;
      for(j = 0; j < OutputSurface(i).interfaceElements().ElementSize(); ++j)
    m_pInterfaceElements->AddMeshElement(const_cast<geo::IElement&>(OutputSurface(i).interfaceElements().Element(j)));
  }
  }

  return m_pInterfaceElements;
}

const geo::IObject& CTetraHorizonBase::DisplayList(int nIndex) const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  if(pModel->Mesh().IsMesh()) {
    if(Slip() && InterfaceElementGroup() && OutputSurface(nIndex).Slip())
      return OutputSurface(nIndex).interfaceElements();
    else
      return *DisplaySurfaces()[nIndex];
  }
  return C3DHorizon::DisplayList(nIndex);
}

int CTetraHorizonBase::DisplayListSize() const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  if(pModel->Mesh().IsMesh()) {
    if(Slip() && InterfaceElementGroup())
      return OutputSurfaceSize();
    else
      return DisplaySurfaces().size();
  }


  return C3DHorizon::DisplayListSize();
}

void CTetraHorizonBase::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  if(&model.Mesh() == &node)
  {
  if(m_pInterfaceElements)
  {
      // walk over the mesh's element groups to check whether this group still exists
      // (the mesh deletes its element groups when it's cleared)
      int i;
      for(i = 0; i < model.Mesh().Mesh().ElementGroupSize(); ++i)
      {
    if(&model.Mesh().Mesh().ElementGroup(i) == m_pInterfaceElements)
    {
          delete m_pInterfaceElements;
          break;
    }
      }
      m_pInterfaceElements = 0;
  }

    Modified();
  }

  C3DHorizon::OnNeighbourModified(node, uHint);
}

int CTetraHorizonBase::BodyFaceSize() const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);

  int sz = 0;

  if(pModel->Mesh().IsMesh())
  {
    for(int i = 0; i < OutputSurfaceSize(); i++)
    {
      const geo::CSurfaceDesc &surfdesc = OutputSurface(i);

      for(int j = 0; j < surfdesc.TetSurfaceSize(); j++)
        sz += surfdesc.TetSurface(j).FaceSize();
    }
  }

  return sz;
}

const geo::IFace &CTetraHorizonBase::BodyFace(int nIndex) const
{
  const CTetraModel *pModel = dynamic_cast<const CTetraModel*> (&Model());
  assert(pModel);
  assert(pModel->Mesh().IsMesh());

  for(int i = 0; i < OutputSurfaceSize(); i++)
  {
    const geo::CSurfaceDesc &surfdesc = OutputSurface(i);

    for(int j = 0; j < surfdesc.TetSurfaceSize(); j++)
    {
      if(nIndex < surfdesc.TetSurface(j).FaceSize())
        return surfdesc.TetSurface(j).Face(nIndex);

      nIndex -= surfdesc.TetSurface(j).FaceSize();
    }
  }

  assert(false);
  const geo::IFace *pBogus = 0;
  return *pBogus;
}

bool CTetraHorizonBase::CanDisconnectItem(const CGraphNode& item) const
{
  CSurfaceBase *pSurf = const_cast<CSurfaceBase*>(dynamic_cast<const CSurfaceBase*> (&item));

  if(pSurf)
    return !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

  return C3DHorizon::CanDisconnectItem(item);
}

bool CTetraHorizonBase::Destroy()
{
  COperation operation(Model()); // prevent screen updates before we're really done

  if(SurfaceSize() > 0)
  {
    // Invalidate the mesh ...
    CModelBase& model = static_cast<CModelBase&>(Model());
    model.InvalidateMesh();
  }
  
  delete this;

  return true;
}
