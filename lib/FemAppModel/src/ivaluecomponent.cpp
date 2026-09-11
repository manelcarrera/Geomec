// IValueComponentBase.cpp: implementation of the IValueComponentBase class.
//
//////////////////////////////////////////////////////////////////////

#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "IElementSet.h"
#include "IValueDataInterface.h"
#include "IParallelInitializationCallback.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IValueComponentBase::IValueComponentBase(unsigned int uName, IValueComposite &composite, unsigned int uComponent, unsigned int uMode)
: CStorageNode(uName, composite.Model())
{
  if(uMode >= composite.m_vcMode.size())
    composite.m_vcMode.resize(uMode + 1);

  if(uComponent >= composite.m_vcMode[uMode].size())
    composite.m_vcMode[uMode].resize(uComponent + 1);

  composite.m_vcMode[uMode][uComponent] = this;

  reParent(&composite);
}

IValueComponentBase::IValueComponentBase(const QString& sName, IValueComposite &composite, unsigned int uComponent, unsigned int uMode)
: CStorageNode(sName, composite.Model())
{
  if(uMode >= composite.m_vcMode.size())
    composite.m_vcMode.resize(uMode + 1);

  if(uComponent >= composite.m_vcMode[uMode].size())
    composite.m_vcMode[uMode].resize(uComponent + 1);

  composite.m_vcMode[uMode][uComponent] = this;

  reParent(&composite);
}

IValueComponentBase::IValueComponentBase(IValueComposite &composite, unsigned int uComponent, unsigned int uMode)
: CStorageNode("", composite.Model())
{
  if(uMode >= composite.m_vcMode.size())
    composite.m_vcMode.resize(uMode + 1);

  if(uComponent >= composite.m_vcMode[uMode].size())
    composite.m_vcMode[uMode].resize(uComponent + 1);

  composite.m_vcMode[uMode][uComponent] = this;
  
  reParent(&composite);
}

IValueComponentBase::IValueComponentBase(const IValueComponentBase &rhs)
: CStorageNode(rhs)
{
}

const IValueDataInterfaceVector *IValueComponentBase::VectorData() const
{
  return 0;
}

IValueDataInterfaceVector *IValueComponentBase::VectorData()
{
  return 0;
}

const IValueDataInterfaceTensor *IValueComponentBase::TensorData() const
{
  return 0;
}

IValueDataInterfaceTensor *IValueComponentBase::TensorData()
{
  return 0;
}

const IValueDataInterface *IValueComponentBase::Data(int /*type*/) const
{
  return 0;
}

IValueDataInterface *IValueComponentBase::Data(int /*type*/)
{
  return 0;
}

const IValueComposite& IValueComponentBase::Parent() const
{
  assert(parent());
  assert(!parent()->IsCopy());
  if(IsCopy() && parent()->IsCopied())
    return (const IValueComposite&) (parent()->Copy());

  return (const IValueComposite&)*parent();
}

IValueComposite& IValueComponentBase::Parent()
{
  assert(parent());
  assert(!parent()->IsCopy());
  if(IsCopy() && parent()->IsCopied())
    return (IValueComposite&) (parent()->Copy());

  return (IValueComposite&)*parent();
}

IValueComponentBase& IValueComponentBase::operator=(const IValueComponentBase &rhs)
{
  assert(rhs.IsCopy());
  CStorageNode::operator =(rhs);
  return *this;
}

bool IValueComponentBase::operator==(const IValueComponentBase &rhs) const
{
  return CStorageNode::operator ==(rhs);
}

bool IValueComponentBase::Less(const CGraphNode &node) const
{
  const IValueComponentBase *pComponent = dynamic_cast<const IValueComponentBase*>(&node);
  if(pComponent)
  {
    // If we have equal parents we compare on component index
    if(&Parent() == &pComponent->Parent())
    {
      int this_index = -1, rhs_index = -1;
      for(unsigned int nMode = 0; nMode < Parent().ModeSize(); nMode++)
      {
        for(unsigned int nComponent = 0; nComponent < Parent().ComponentSize(nMode); nComponent++)
        {
          if(&Parent().Component(nComponent, nMode) == pComponent)
            rhs_index = nComponent;
          if(&Parent().Component(nComponent, nMode) == this)
            this_index = nComponent;
        }
      }

      return this_index < rhs_index;
    }
  }

  return CStorageNode::Less(node);
}

int IValueComponentBase::ComponentIndex() const
{
  for(unsigned int nMode = 0; nMode < Parent().ModeSize(); nMode++)
  {
    for(int nComponent = 0; nComponent < Parent().ComponentSize(nMode); nComponent++)
    {
      if(&Parent().Component(nComponent, nMode) == this)
        return nComponent;
    }
  }

  assert(false);	// Should always work
  return -1;
}

int IValueComponentBase::ModeIndex() const
{
  for(unsigned int nMode = 0; nMode < Parent().ModeSize(); nMode++)
  {
    for(unsigned int nComponent = 0; nComponent < Parent().ComponentSize(nMode); nComponent++)
    {
      if(&Parent().Component(nComponent, nMode) == this)
        return nMode;
    }
  }

  assert(false);	// Should always work
  return -1;
}

// Default we can map !!!
bool IValueComponentBase::CanMap(const COpenGLNode& /*node*/) const
{
  return true;
}

bool IValueComponentBase::CanComputeOnPoints() const
{
  return true;
}

int IValueComponentBase::MinMaxSteps(const geo::IObject &object) const
{
  const geo::IElementSet *pElementSet=dynamic_cast<const geo::IElementSet*> (&object);
  if(pElementSet) return pElementSet->ElementSize();

  int nRet = 0;
  const geo::IArray* ptArr = dynamic_cast<const geo::IArray *> (&object);
  if(ptArr) {
    for(size_t i = 0; i < ptArr->Size(); i++) {
      nRet += MinMaxSteps( ptArr->ObjectBase(i) );
      nRet++;
    }
  } 

  return nRet;
}

bool IValueComponentBase::NeedParallelInitializationCallback() const
{
  return false;
}

geo::IParallelInitializationCallback *IValueComponentBase::GetParallelInitializationCallback()
{
  return 0;
}

bool IValueComponentBase::PrepareMapping(const geo::IElementSet *pElementSet)
{
  return Parent().PrepareMapping(pElementSet, this);
}

void IValueComponentBase::FinishMapping()
{
  Parent().FinishMapping();
}
