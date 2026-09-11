#ifndef _GraphEntry_Delegate_h_
#define _GraphEntry_Delegate_h_

#include "GraphNode_Delegate.h"
#include "GraphEntry.h"

class CGraphEntry_Delegate : public CGraphNode_Delegate
{
public:
  CGraphEntry_Delegate(CGraphEntry* graphEntry);

private:
  CGraphEntry_Delegate(const CGraphEntry_Delegate& rhs);
  CGraphEntry_Delegate& operator = (const CGraphEntry_Delegate& rhs);

  CGraphEntry* m_graphEntry;

  REGISTER_DELEGATE(CGraphEntry, CGraphEntry_Delegate);
};

template <class T>
  class CGraphEntryTemp_Delegate : public CGraphEntry_Delegate
{
public:
  CGraphEntryTemp_Delegate(CGraphEntryTemp <T> * graphEntryTemp);

  virtual bool CanModify() const;
  virtual bool Modify();

  virtual bool CanDestroy() const;
  virtual bool Destroy();

private:
  CGraphEntryTemp_Delegate(const CGraphEntryTemp_Delegate& rhs);
  CGraphEntryTemp_Delegate& operator = (const CGraphEntryTemp_Delegate& rhs);

  CGraphEntryTemp <T> * m_graphEntryTemp;

  REGISTER_DELEGATE(CGraphEntryTemp <T>, CGraphEntryTemp_Delegate <T>);
};

template <class T>
  CGraphEntryTemp_Delegate <T> ::
    CGraphEntryTemp_Delegate(CGraphEntryTemp <T> * graphEntryTemp)
: CGraphEntry_Delegate(graphEntryTemp)
, m_graphEntryTemp(graphEntryTemp)
{
  ACTIVATE_TEMPLATE_DELEGATE(CGraphEntryTemp <T>, CGraphEntryTemp_Delegate <T>);
}

template <class T>
  bool CGraphEntryTemp_Delegate <T> ::CanModify() const
{
  CGraphEntryTemp <T> ::TNodeSet stNodes =
    m_graphEntryTemp->EntryNodes();

  for (CGraphEntryTemp <T> ::TNodeSet::const_iterator it = stNodes.begin();
    it != stNodes.end(); it++)
  {
    CGraphNode_Delegate* graphNode_Delegate =
      static_cast <CGraphNode_Delegate*> ((*it)->getDelegate());

    if (graphNode_Delegate->CanEdit())
    {
      return true;
    }
  }

  return false;
}

template <class T>
  bool CGraphEntryTemp_Delegate <T> ::Modify()
{
  return false;
}

template <class T>
  bool CGraphEntryTemp_Delegate <T> ::CanDestroy() const
{
  return m_graphEntryTemp->CanDestroy();
}

template <class T>
  bool CGraphEntryTemp_Delegate <T> ::Destroy()
{
  return m_graphEntryTemp->Destroy();
}

#endif  // _GraphEntry_Delegate_h_
