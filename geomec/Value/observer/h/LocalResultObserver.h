#if !defined(__LOCAL_RESULT_OBSERVER_H__)
#define __LOCAL_RESULT_OBSERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeCtrlBase;

#include "ResultObserver.h"
#include "ResultTree.h"
#include "LocalResult.h"
#include "resourceIDI.h"
#include "ValueType.h"
#include "GVTResult.h"


// use the CLocalResultGroupObserver if you want a result tree in a dialog where the user
// needs to select a result.
template<class PARENT, unsigned int CHECKED = IDI_CHECKED, unsigned int UNCHECKED = IDI_UNCHECKED>
class CValueTypeObserverTempl : public CChildEnumerator<CValueComponent>
{
public:
  // TODO AppendContextMenu
  // CComponentObserver is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However
  // one could verify that this function is never used!

  class CComponentObserver : public CTreeNode
  {
  CValueComponent& m_component;
  public:
  // Construction
  CComponentObserver(CValueComponent& component, CValueTypeObserverTempl& parent)
      : CTreeNode(parent, TVI_LAST), m_component(component)
  {
      Observer().LinkTo(component);
      Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
      Update();
  }

  virtual const CGraphNode& ObservedItem() const
  {
      return m_component;
  }

  virtual CGraphNode& ObservedItem()
  {
      return m_component;
  }

  virtual const CGraphNode_Delegate& Delegate() const
  {
      assert(FALSE);
      return *(new CGraphNode_Delegate(0));
  }

  virtual CGraphNode_Delegate& Delegate()
  {
      assert(FALSE);
      return *(new CGraphNode_Delegate(0));
  }

  CValueTypeObserverTempl& ValueTypeObserver()
  {
      return dynamic_cast<CValueTypeObserverTempl<PARENT, CHECKED, UNCHECKED>&>(*Parent());
  }

  const CValueTypeObserverTempl& ValueTypeObserver() const
  {
      return dynamic_cast<const CValueTypeObserverTempl<PARENT, CHECKED, UNCHECKED>&>(*Parent());
  }

  virtual unsigned int StateIcon() const
  {
      if (ValueTypeObserver().ParentObserver().Dlg().Selected(&m_component))
    return CHECKED;
      return UNCHECKED;
  }

  virtual void ToggleState()
  {
      ValueTypeObserver().ParentObserver().Dlg().Select(&m_component);
  }

  virtual void AppendContextMenu(CContextMenuInvoker &invoker) {}
  };
private:
  CValueType& m_value_type;
public:
  CValueTypeObserverTempl(CValueType& value_type, PARENT& parent)
  : CChildEnumerator<CValueComponent>(parent, TVI_LAST), m_value_type(value_type)
  {
  Observer().LinkTo(value_type);
  Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }

  virtual const CGraphNode& ObservedItem() const
  {
  return m_value_type;
  }

  virtual CGraphNode& ObservedItem()
  {
  return m_value_type;
  }

  virtual const CGraphNode_Delegate& Delegate() const
  {
  assert(FALSE);
  return *(new CGraphNode_Delegate(0));
  }

  virtual CGraphNode_Delegate& Delegate()
  {
  assert(FALSE);
  return *(new CGraphNode_Delegate(0));
  }

  PARENT& ParentObserver() { return dynamic_cast<PARENT&>(*Parent()); }
  const PARENT& ParentObserver() const { return dynamic_cast<const PARENT&>(*Parent()); }

  virtual unsigned int StateIcon() const
  {
  return ParentObserver().ResultStateIconId(m_value_type);
  }

  virtual void ToggleState()
  {
  ParentObserver().ResultToggleState(m_value_type);
  }

  virtual void AppendContextMenu(CContextMenuInvoker &invoker)
  {
  }

  virtual CTreeNode* InsertChild(CValueComponent& component)
  {
  return new CComponentObserver(component, *this);
  }

  virtual BOOL OnFilter(const CValueComponent& t) const
  {
  return t.Parent().ComponentSize() > 1;
  }
};

template<class OWNER>
class CGVTResultGroupObserverTempl : public IResultGroupObserver
{
  OWNER& m_dlg;
  std::set<const CValueComponent *> m_selection;
public:
  CGVTResultGroupObserverTempl(CResultGroup& group, IResultGroupObserver& parent, OWNER& dlg)
   : IResultGroupObserver(group, parent)
   , m_dlg(dlg)
  {
  }

  virtual BOOL OnFilter(const CGraphNode& node) const
  {
  if (dynamic_cast<const TDeltaT *>(&node) || dynamic_cast<const TDeltaV *>(&node))
      return TRUE;

  return IResultGroupObserver::OnFilter(node);
  }

  virtual CTreeNode* InsertChild(CGraphNode& node)
  {
  if (dynamic_cast<const TDeltaT *>(&node) || dynamic_cast<const TDeltaV *>(&node))
      return new CValueTypeObserverTempl<CGVTResultGroupObserverTempl<OWNER> >(static_cast<CValueType&>(node), *this);
  else
      return IResultGroupObserver::InsertChild(node);
  }

  OWNER& Dlg() { return m_dlg; }
  const OWNER& Dlg() const { return m_dlg; }
};

template<class OWNER>
class CGVTResultGroupObserverSingleSelectTempl : public CGVTResultGroupObserverTempl<OWNER>
{
public:
  CGVTResultGroupObserverSingleSelectTempl(CResultGroup& group, IResultGroupObserver& parent, OWNER& dlg)
  : CGVTResultGroupObserverTempl<OWNER>(group, parent, dlg)
  {
  }

  virtual CTreeNode* InsertChild(CGraphNode& node)
  {
  if (dynamic_cast<const TDeltaT *>(&node) || dynamic_cast<const TDeltaV *>(&node))
      return new CValueTypeObserverTempl<CGVTResultGroupObserverSingleSelectTempl<OWNER> >(static_cast<CValueType&>(node), *this);
  else
      return IResultGroupObserver::InsertChild(node);
  }

  virtual unsigned int ResultStateIconId(const IValueComposite& value_type) const
  {
  if (value_type.ComponentSize() > 1)
      return 0;

  if (Dlg().Selected(&value_type.Component()))
      return IDI_RADIO_CHECKED;
  return IDI_RADIO_UNCHECKED;
  }

  virtual void ResultToggleState(const IValueComposite& value_type)
  {
  if (value_type.ComponentSize() == 1)
  {
      Dlg().Select(&value_type.Component());
  }
  }
};



// owner is typically a dialog, but can also be another object.
template<class OWNER>
class CLocalResultGroupObserver : public IResultGroupObserver
{
  const OWNER &m_Owner;

  TResultComponentSet m_stResultComponent;

  BOOL Valid(const CResultGroup& group) const
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
      if(pResult)
      {
    if(m_Owner.Valid(*pResult))
          return TRUE;
      }
      const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
      if(pResultGroup)
      {
    if(Valid(*pResultGroup))
          return TRUE;
      }
      const CValueType *pValueType = dynamic_cast<const CValueType *>(&group.Child(i));
      if (pValueType)
      {
    if (m_Owner.Valid(*pValueType))
          return TRUE;
      }
  }
  return FALSE;
  }

  bool AllSelected(const CResultGroup& group) const
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
      if(pResult && !pResult->Name().isEmpty() && m_Owner.Valid(*pResult))
      {
    if(!AllSelected(*pResult))
          return false;
      }
      const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
      if(pResultGroup && Valid(*pResultGroup))
      {
    if(!AllSelected(*pResultGroup))
          return false;
      }
      const CValueType *pValueType = dynamic_cast<const CValueType *>(&group.Child(i));
      if (pValueType && m_Owner.Valid(*pValueType))
      {
    if (!AllSelected(*pValueType))
          return false;
      }
  }
  return true;
  }

  bool AllNotSelected(const CResultGroup& group) const
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
      if(pResult && m_Owner.Valid(*pResult))
      {
    if(!AllNotSelected(*pResult))
          return false;
      }
      const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
      if(pResultGroup && Valid(*pResultGroup))
      {
    if(!AllNotSelected(*pResultGroup))
          return false;
      }
      const CValueType *pValueType = dynamic_cast<const CValueType *>(&group.Child(i));
      if (pValueType && m_Owner.Valid(*pValueType))
      {
    if (!AllNotSelected(*pValueType))
          return false;
      }
  }
  return true;
  }


  bool AllSelected(const IValueComposite& result) const
  {
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result. ComponentSize(); i++)
  {
      if(!ResultComponent(result, i))
    return false;
  }
  return true;
  }

  bool AllNotSelected(const IValueComposite& result) const
  {
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
  {
      if(ResultComponent(result, i))
    return false;
  }

  return true;
  }

  void SelectAll(CResultGroup& group)
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      IResult *pResult = dynamic_cast<IResult*>(&group.Child(i));
      if(pResult && !pResult->Name().isEmpty() )
    SelectAll(*pResult);
      CResultGroup *pResultGroup = dynamic_cast<CResultGroup*>(&group.Child(i));
      if(pResultGroup)
    SelectAll(*pResultGroup);
      CValueType *pValueType = dynamic_cast<CValueType *>(&group.Child(i));
      if (pValueType)
    SelectAll(*pValueType);
  }
  Update();
  }

  void DeselectAll(CResultGroup& group)
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      IResult *pResult = dynamic_cast<IResult*>(&group.Child(i));
      if(pResult)
    DeselectAll(*pResult);
      CResultGroup *pResultGroup = dynamic_cast<CResultGroup*>(&group.Child(i));
      if(pResultGroup)
    DeselectAll(*pResultGroup);
      CValueType *pValueType = dynamic_cast<CValueType *>(&group.Child(i));
      if (pValueType)
    DeselectAll(*pValueType);
  }
  Update();
  }

  void SelectAll(IValueComposite& result)
  {
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
      ResultComponent(result, i, true);
  Update();
  }

  void DeselectAll(IValueComposite& result)
  {
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
      ResultComponent(result, i, false);
  Update();
  }

public:
  CLocalResultGroupObserver(const OWNER &owner, CResultGroup& tree, CTreeCtrlBase &m_tcResults)
  : IResultGroupObserver(tree, m_tcResults, TVI_ROOT, TVI_LAST), m_Owner(owner)
  {
  Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }

  virtual ~CLocalResultGroupObserver()
  {
  const_cast<OWNER&>(m_Owner).AddResults(Results());
  }

  void newSelection(const TResultComponentSet& newSelection)
  {
  m_stResultComponent = newSelection;
  }

  virtual unsigned int ResultGroupStateIconId(const CResultGroup& group) const
  {
  if(AllNotSelected(group))
      return IDI_UNCHECKED;

  if(AllSelected(group))
      return IDI_CHECKED;

  return IDI_TRI_CHECKED;
  }

  virtual void ResultGroupToggleState(CResultGroup& group)
  {
  if(AllNotSelected(group))
      SelectAll(group);
  else if(AllSelected(group))
      DeselectAll(group);
  else SelectAll(group);
  Update();
  m_Owner.toggleState();
  }

  virtual void AppendResultGroupContextMenu(CResultGroup_Delegate& group_Delegate, CContextMenuInvoker &invoker)
  {
  }

  virtual BOOL CanInsertResultGroup(const CResultGroup& group) const
  {
  return Valid(group);
  }

  virtual BOOL CanInsertResult(const IResult& result) const
  {
  return m_Owner.Valid(result);
  }

  virtual CTreeNode* InsertChild(CGraphNode& node)
  {
  CGVTResultGroup *pGVTGroup = dynamic_cast<CGVTResultGroup *>(&node);
  if (pGVTGroup)
      return new CGVTResultGroupObserverTempl<OWNER>(*pGVTGroup, *this, const_cast<OWNER&>(m_Owner));
  return IResultGroupObserver::InsertChild(node);
  }

  virtual BOOL OnFilter(const CGraphNode& node) const
  {
  if (dynamic_cast<const CGVTResultGroup *>(&node))
      return Valid(static_cast<const CGVTResultGroup&>(node));
  return IResultGroupObserver::OnFilter(node);
  }


  // Handlers for results
  virtual unsigned int ResultStateIconId(const IValueComposite& result) const
  {
  assert(result.ModeSize() > 0);
  assert(result.ComponentSize() > 1);	// If component size == 1 we have a scalar 
  if(AllNotSelected(result))
      return IDI_UNCHECKED;

  if(AllSelected(result))
      return IDI_CHECKED;

  return IDI_TRI_CHECKED;
  }

  virtual void ResultToggleState(IValueComposite& result)
  {
  assert(result.ModeSize() > 0);
  assert(result.ComponentSize() > 1);	// If component size == 1 we have a scalar 
  if(AllNotSelected(result))
      SelectAll(result);
  else if(AllSelected(result))
      DeselectAll(result);
  else SelectAll(result);
  m_Owner.toggleState();
  }

  virtual void AppendResultContextMenu(IResult_Delegate& result, CContextMenuInvoker &invoker)
  {
  }


  // Handlers result components
  virtual unsigned int ResultComponentStateIconId(const IValueComposite& result, int nComponent) const
  {
  if(ResultComponent(result, nComponent))
      return IDI_CHECKED;
  return IDI_UNCHECKED;
  }

  virtual void ResultComponentToggleState(IValueComposite& result, int nComponent)
  {
  ResultComponent(result, nComponent, !ResultComponent(result, nComponent));
  Update();
  m_Owner.toggleState();
  }

  virtual void AppendResultComponentContextMenu(const IResult_Delegate& result, int nComponent, CContextMenuInvoker &invoker)
  {
  }

  bool ResultComponent(const IValueComposite& composite, int nComponent) const
  {
  TResultComponent component(&composite, nComponent);

  // Try to find it
  return m_stResultComponent.find(component) != m_stResultComponent.end();
  }

  void ResultComponent(const IValueComposite& composite, int nComponent, bool bEnable)
  {
  TResultComponent component(&composite, nComponent);
  if(bEnable)
  {
      if(m_Owner.Valid(composite.Component(nComponent)))
    m_stResultComponent.insert(component);
  }
  else
      m_stResultComponent.erase(component);
  }

  const TResultComponentSet &Results()
  {
  return m_stResultComponent;
  }

  bool ResultComposite(const IValueComposite& composite) const
  {
  for(int i = 0; i < composite.ComponentSize(); i++)
  {
      if(!ResultComponent(composite, i)) return false;
  }
  return true;
  }

  void ResultComposite(const IValueComposite& composite, bool bEnable)
  {
  for(int i = 0; i < composite.ComponentSize(); i++)
  {
      ResultComponent(composite, i, bEnable);
  }
  }

  void ResultGroup(const CResultGroup& group, bool bEnable)
  {
  for(int i = 0; i < group.ChildSize(); i++)
  {
      // Toggle the children till they are in the desired state
      const CResultGroup* pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
      if(pResultGroup)
    ResultGroup(*pResultGroup, bEnable);

      const IValueComposite* pValueComposite = dynamic_cast<const IValueComposite*>(&group.Child(i));
      if(pValueComposite)
    ResultComposite(*pValueComposite, bEnable);
  }
  }
};

#endif
