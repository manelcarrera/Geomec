// IValueComposite.cpp: implementation of the IValueComposite class.
//
//////////////////////////////////////////////////////////////////////


#include "FemAppEntryTypes.h"
#include "ivaluecomposite.h"
#include "FemAppModel.h"
#include "IParallelInitializationCallback.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

/*!
\class IValueComposite
\brief The IValueComposite has IValueComponentBase components.

The IValueComposite represents a set of distributed values in space. The IValueComposite can
have serveral modi and components. The components are for instance the X, Y and Z components
of displacement. The modi are programmer defined indices for having a set of component series.
For example a set of X, Y and Z components for each analysis type. IValueComposites are collected
in the TValueCompositeEntry. The IValueComposite is also supported by the COpenGLValueScene.
\sa IValueComponentBase
\sa COpenGLValueScene
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IValueComposite::IValueComposite(CFemAppModel& model)
: CStorageNode(model)
{
}

IValueComposite::IValueComposite(const QString &strName, CFemAppModel& model)
: CStorageNode(strName, model)
{
  assert(Model().GraphEntry(MD_BASE_VALUE_COMPOSITE));
  LinkTo(*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE));
}

IValueComposite::IValueComposite(unsigned int uName, CFemAppModel& model)
: CStorageNode(uName, model)
{
  assert(Model().GraphEntry(MD_BASE_VALUE_COMPOSITE));
  LinkTo(*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE));
}

IValueComposite::IValueComposite(const IValueComposite &rhs)
: CStorageNode(rhs), m_vcMode(rhs.m_vcMode)
{
}

IValueComposite::~IValueComposite()
{
  // Kill all of the copied components
  TModeVec vcMode = m_vcMode;
  for(size_t nMode = 0; nMode < vcMode.size(); nMode++)
  {
    for(size_t nComponent = 0; nComponent < vcMode[nMode].size(); nComponent++)
    {
      IValueComponentBase *pComponent = vcMode[nMode][nComponent];
      assert(pComponent);
      if(IsCopy())
      {
        if(pComponent->IsCopied())
          delete &pComponent->Copy();
      }
      else
        delete pComponent;
    }

  }
}

unsigned int IValueComposite::ComponentSize(unsigned int uMode) const
{
  assert(uMode < ModeSize());
  return m_vcMode[uMode].size();
}

const IValueComponentBase& IValueComposite::Component(unsigned int uComponent, unsigned int uMode) const
{
  assert(uMode < m_vcMode.size());
  assert(uComponent < m_vcMode[uMode].size());

  if(m_vcMode[uMode][uComponent]->IsCopied() && IsCopy())
    return (const IValueComponentBase&) m_vcMode[uMode][uComponent]->Copy();

  return (const IValueComponentBase&)*m_vcMode[uMode][uComponent];
}

IValueComponentBase& IValueComposite::Component(unsigned int uComponent, unsigned int uMode)
{
  assert(uMode < m_vcMode.size());
  assert(uComponent < m_vcMode[uMode].size());

  if(m_vcMode[uMode][uComponent]->IsCopied() && IsCopy())
    return (IValueComponentBase&) m_vcMode[uMode][uComponent]->Copy();

  return (IValueComponentBase&)*m_vcMode[uMode][uComponent];
}

unsigned int IValueComposite::ModeSize() const
{
  return m_vcMode.size();
}

QString IValueComposite::ModeName(unsigned int /*uMode*/) const
{
  return "";
}

void IValueComposite::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if(dynamic_cast<const IValueComponentBase*>(&node))
  {
    for(unsigned int nMode = 0; nMode < ModeSize(); nMode++)
    {
      for(unsigned int nComponent = 0; nComponent < ComponentSize(nMode); nComponent++)
      {
        if(&Component(nComponent, nMode) == &node)
          Modified(uHint);
      }
    }
  }

  CStorageNode::OnNeighbourModified(node, uHint);
}

void IValueComposite::OnNeighbourDeleted(const CGraphNode &node)
{
  // Try to find the indices of the node
  for(TModeVec::iterator it = m_vcMode.begin(); it != m_vcMode.end(); it++)
  {
    for(TComponentVec::iterator itt = it->begin(); itt != it->end(); itt++)
    {
      if((*itt) == &node)
      {
        it->erase(itt);
        if(it->size() == 0)
          m_vcMode.erase(it);
        
        CStorageNode::OnNeighbourDeleted(node);
        return;
      }
    }
  }

  CStorageNode::OnNeighbourDeleted(node);
}
        

IValueComposite& IValueComposite::operator=(const IValueComposite &rhs)
{
  CStorageNode::operator =(rhs);
  m_vcMode = rhs.m_vcMode;
  return *this; 
}

bool IValueComposite::operator==(const IValueComposite &rhs) const
{
  return CStorageNode::operator==(rhs);
}

bool IValueComposite::Empty() const
{
  return m_vcMode.size() == 0;
}

bool IValueComposite::Less(const CGraphNode& rhs) const
{
  int myType = -1;
  int theirType = -1;

  if (isMeshResult())
  myType = 0;
  else if (isMaterialResult())
  myType = 1;
  else if (isResult())
  myType = 2;

  if (dynamic_cast<const IValueComposite *>(&rhs))
  {
  const IValueComposite *valueComposite = static_cast<const IValueComposite *>(&rhs);
  if (valueComposite->isMeshResult())
      theirType = 0;
  else if (valueComposite->isMaterialResult())
      theirType = 1;
  else if (valueComposite->isResult())
      theirType = 2;
  }

  if (myType != theirType)
  return myType < theirType;

  return CStorageNode::Less(rhs);
}

bool IValueComposite::isFaultResult() const
{
  return false;
}

bool IValueComposite::isMeshResult() const
{
  return false;
}

bool IValueComposite::isMaterialResult() const
{
  return false;
}

bool IValueComposite::isResult() const
{
  return false;
}

bool IValueComposite::NeedParallelInitializationCallback() const
{
  return false;
}

geo::IParallelInitializationCallback *IValueComposite::GetParallelInitializationCallback()
{
  return 0;
}

bool IValueComposite::PrepareMapping(const geo::IElementSet *, const IValueComponentBase *)
{
  return true;
}

void IValueComposite::FinishMapping()
{
}