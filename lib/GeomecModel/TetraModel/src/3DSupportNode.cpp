// TetraSupportNode.cpp: implementation of the C3DSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#include "3DSupportNode.h"
#include "TetraBoundary.h"
#include "TetraModel.h"
#include "TetraMesh.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "TetraHorizonBase.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

C3DSupportNode::C3DSupportNode(unsigned int uName, CInterfaceBoundary &boundary)
: CBaseSupportNode(uName, boundary)
{
}

C3DSupportNode::C3DSupportNode(const C3DSupportNode &rhs) :
  CBaseSupportNode(rhs),
  m_GlobalTensors(rhs.m_GlobalTensors)
{
}

C3DSupportNode::~C3DSupportNode()
{
}

C3DSupportNode &C3DSupportNode::operator=(const C3DSupportNode &rhs)
{
  m_GlobalTensors = rhs.m_GlobalTensors;
  return *this;
}

bool C3DSupportNode::operator==(const C3DSupportNode &rhs) const
{
//	if(m_nGlobalTensorInput != rhs.m_nGlobalTensorInput) return false;
  if(!(m_GlobalTensors == rhs.m_GlobalTensors)) return false;

  return true;
}

const CInterfaceBoundary &C3DSupportNode::Boundary() const
{
  assert(parent());
  return (CInterfaceBoundary&)*parent();
}

CInterfaceBoundary &C3DSupportNode::Boundary()
{
  assert(parent());
  return (CInterfaceBoundary&)*parent();
}

bool C3DSupportNode::CanEditTensor() const
{
  if(Boundary().CreateInterfaces())
    return false;
  return true;
}


C3DSupportNode::TColor C3DSupportNode::Color() const
{
  return qRgb(255, 255, 255);
}

void C3DSupportNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int sz;
  stream >> sz;

  const CModelBase *pModel = dynamic_cast<const CModelBase *> (&Model());
  assert(pModel != 0);

  const CDepletionStage *pStage = &pModel->InitialDepletionStage();

  if(pStage->Last()) pStage = 0;
  else pStage = &pStage->Next();

  m_GlobalTensors.clear();

  for(int i = 0; i < sz; i++)
  {
    assert(pStage != 0);

    std::pair<TTensorMap::iterator, bool> prInsert = m_GlobalTensors.insert(std::make_pair(pStage, CGlobalTensorDef()));
    assert(prInsert.second);
    prInsert.first->second.LoadStream(stream, version, progress);

    if(pStage->Last()) pStage = 0;
    else pStage = &pStage->Next();

    progress.Step();
  }

  CBaseSupportNode::LoadStream(stream, version, progress);
}

void C3DSupportNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  int sz = m_GlobalTensors.size();
  stream << sz;

  for(TTensorMap::iterator it = m_GlobalTensors.begin(); it != m_GlobalTensors.end(); it++)
  {
    it->second.SaveStream(stream, progress);

    progress.Step();
  }

  CBaseSupportNode::SaveStream(stream, progress);
}

long C3DSupportNode::SavedItems() const
{
  long lTotal = CBaseSupportNode::SavedItems();

  lTotal += m_GlobalTensors.size();

  return lTotal;
}

void C3DSupportNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  const TDepletionStageEntry *pEntry = dynamic_cast<const TDepletionStageEntry *> (&node);
  if(pEntry && uHint != MarkedAsInitialChanged)
  {
    if(pEntry->EntryNodes().size() > 0)
    {
      // number of depletion stages might have changed, update map
      const CModelBase *pModel = dynamic_cast<const CModelBase*> (&Model());
      assert(pModel);
      const CDepletionStage *pStage = &pModel->InitialDepletionStage();
      if(pStage->Last()) pStage = 0;
      else pStage = &pStage->Next();
      typedef std::set<const CDepletionStage *> TStageSet;
      TStageSet stStages;
      while(pStage)
      {
        VERIFY(stStages.insert(pStage).second);
        if(!pStage->Last())
          pStage = &pStage->Next();
        else
          pStage = 0;
      }

      TTensorMap::iterator it = m_GlobalTensors.begin();
      while(it != m_GlobalTensors.end())
      {
        if(stStages.find(it->first) == stStages.end())
        {
          TTensorMap::iterator temporary = it++;

          // stage doesn't exist anymore, delete it from the map
          m_GlobalTensors.erase(temporary);
        }
        else it++;
      }

      for(TStageSet::iterator its = stStages.begin(); its != stStages.end(); its++)
      {
        m_GlobalTensors.insert(std::make_pair(*its, CGlobalTensorDef()));
      }
    }
  }

  CBaseSupportNode::OnNeighbourModified(node, uHint);
}

void C3DSupportNode::EditGlobalTensor()
{
    for(TTensorMap::iterator it = m_GlobalTensors.begin(); it != m_GlobalTensors.end(); it++)
    {
      if(it->second.GlobalTensorInput() == GTI_UNDEFINED)
      {
        OnGlobalTensorInputUndefined(*it->first);
      }
    }
}

bool C3DSupportNode::GlobalTensorDefined(const CDepletionStage &stage) const
{
  assert(m_GlobalTensors.find(&stage) != m_GlobalTensors.end());
  return (m_GlobalTensors.find(&stage)->second.GlobalTensorInput() != GTI_UNDEFINED);
}

const ITensor &C3DSupportNode::GlobalTensor(const CDepletionStage &stage) const
{
  assert(GlobalTensorDefined(stage));
  assert(m_GlobalTensors.find(&stage) != m_GlobalTensors.end());
  return m_GlobalTensors.find(&stage)->second.Tensor();
}

ITensor &C3DSupportNode::GlobalTensor(const CDepletionStage &stage)
{
  assert(GlobalTensorDefined(stage));
  assert(m_GlobalTensors.find(&stage) != m_GlobalTensors.end());
  return m_GlobalTensors.find(&stage)->second.Tensor();
}

C3DSupportNode::TGlobalTensorInput C3DSupportNode::GlobalTensorInput(const CDepletionStage &stage) const
{
  assert(m_GlobalTensors.find(&stage) != m_GlobalTensors.end());
  return m_GlobalTensors.find(&stage)->second.GlobalTensorInput();
}

void C3DSupportNode::GlobalTensorInput(const CDepletionStage &stage, TGlobalTensorInput nInput)
{
  assert(m_GlobalTensors.find(&stage) != m_GlobalTensors.end());
  m_GlobalTensors.find(&stage)->second.GlobalTensorInput(nInput);
}

int C3DSupportNode::DisplayListSize() const
{
  if(Mesh().IsMesh())
  {
    return m_vcSurfaceEdges.size();
  }

  return 0;
}

const geo::IObject& C3DSupportNode::DisplayList(int nIndex) const
{
  assert(Mesh().IsMesh());
  assert(nIndex >= 0 && nIndex < m_vcSurfaceEdges.size());

  return m_vcSurfaceEdges[nIndex];
}

const CMeshBase& C3DSupportNode::Mesh() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return model.Mesh();
}

void C3DSupportNode::BuildSurfaceEdgeVector() const
{
  m_vcSurfaceEdges.clear();

  if(Mesh().IsMesh())
  {
  std::vector<const geo::ISurface*> vcSurfaces = Boundary().GetSideMeshSurfaces();

    for(size_t i = 0; i < vcSurfaces.size(); ++i)
      m_vcSurfaceEdges.push_back(vcSurfaces[i]->Edge());

    for(int i = 0; i < Boundary().GetTopHorizon().MeshedSurfaceSize(); i++)
      m_vcSurfaceEdges.push_back(Boundary().GetTopHorizon().MeshedSurface(i).Edge());

    for(int i = 0; i < Boundary().GetBottomHorizon().MeshedSurfaceSize(); i++)
      m_vcSurfaceEdges.push_back(Boundary().GetBottomHorizon().MeshedSurface(i).Edge());
  }
}

// CGlobalTensorDef class implementation
C3DSupportNode::CGlobalTensorDef::CGlobalTensorDef() :
  m_nGlobalTensorInput(GTI_UNDEFINED), m_GlobalTensor(0)
{
}

bool C3DSupportNode::CGlobalTensorDef::operator==(const CGlobalTensorDef &rhs) const
{
  if(m_nGlobalTensorInput != rhs.m_nGlobalTensorInput) return false;

  return (m_GlobalTensor == rhs.m_GlobalTensor);
}

C3DSupportNode::TGlobalTensorInput C3DSupportNode::CGlobalTensorDef::GlobalTensorInput() const
{
  return m_nGlobalTensorInput;
}

void C3DSupportNode::CGlobalTensorDef::GlobalTensorInput(TGlobalTensorInput nInput)
{
  m_nGlobalTensorInput = nInput;
}

const ITensor &C3DSupportNode::CGlobalTensorDef::Tensor() const
{
  return m_GlobalTensor;
}

ITensor &C3DSupportNode::CGlobalTensorDef::Tensor()
{
  return m_GlobalTensor;
}

void C3DSupportNode::CGlobalTensorDef::LoadStream(TSTREAM &stream, CStreamVersion &/*version*/, TPROGRESS &/*progress*/)
{
  int dum;
  stream >> dum;
  m_nGlobalTensorInput = (TGlobalTensorInput) dum;

  double xx, yy, zz, xy, yz, xz;
  stream >> xx;
  stream >> yy;
  stream >> zz;
  stream >> xy;
  stream >> yz;
  stream >> xz;

  m_GlobalTensor = CTensor(xx, yy, zz, xy, yz, xz);
}

void C3DSupportNode::CGlobalTensorDef::SaveStream(TSTREAM &stream, TPROGRESS &/*progress*/)
{
  stream << (int) m_nGlobalTensorInput;

  stream << m_GlobalTensor.XX();
  stream << m_GlobalTensor.YY();
  stream << m_GlobalTensor.ZZ();
  stream << m_GlobalTensor.XY();
  stream << m_GlobalTensor.YZ();
  stream << m_GlobalTensor.XZ();
}


