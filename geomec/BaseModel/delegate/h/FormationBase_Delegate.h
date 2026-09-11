#ifndef _FormationBase_Delegate_h_
#define _FormationBase_Delegate_h_

class CMaterialServer;

#include "MaterialServerParent_Delegate.h"
#include "ColorNode_Delegate.h"
#include "FormationBase.h"
#include "ElementSet_Delegate.h"
#include "IValueComposite_Delegate.h"
#include "ContextMenuInvoker.h"
#include "GocadExportWrapper.h"

class IFormationElementSet_Delegate : public IElementSet_Delegate
{
public:
  IFormationElementSet_Delegate(IFormationElementSet* formationElementSet);

private:
  IFormationElementSet_Delegate(const IFormationElementSet_Delegate& rhs);
  IFormationElementSet_Delegate& operator = (
    const IFormationElementSet_Delegate& rhs);

  IFormationElementSet* m_formationElementSet;

  REGISTER_DELEGATE(IFormationElementSet, IFormationElementSet_Delegate);
};

class CFormationBase_Delegate :
  public CMaterialServerParent_Delegate <
    CColorNode_Delegate, CColorNode, CMaterialServer>
{
  typedef CMaterialServerParent_Delegate <
    CColorNode_Delegate, CColorNode, CMaterialServer> TBase;

public:
  CFormationBase_Delegate(CFormationBase* formationBase);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanExport() const;
  void Export();

  virtual bool CanDestroy() const;

private:
  CFormationBase_Delegate(const CFormationBase_Delegate& rhs);
  CFormationBase_Delegate& operator = (const CFormationBase_Delegate& rhs);

  CFormationBase* m_formationBase;

  REGISTER_DELEGATE(CFormationBase, CFormationBase_Delegate);
};

class CFormationThickness_Delegate : public IValueComposite_Delegate
{
public:
  CFormationThickness_Delegate(CFormationThickness* formationThickness);

private:
  CFormationThickness_Delegate(const CFormationThickness_Delegate& rhs);
  CFormationThickness_Delegate& operator = (
    const CFormationThickness_Delegate& rhs);

  CFormationThickness* m_formationThickness;

  REGISTER_DELEGATE(CFormationThickness, CFormationThickness_Delegate);
};

template <class FORMATION>
  class CFormationEntryTempl_Delegate :
    public CStorageNodeEntry_Delegate <FORMATION>
{
public:
  CFormationEntryTempl_Delegate(CFormationEntryTempl <FORMATION> *
    formationEntryTempl);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanExportGocad() const;
  void OnExportGocad();

private:
  CFormationEntryTempl_Delegate(const CFormationEntryTempl_Delegate& rhs);
  CFormationEntryTempl_Delegate& operator = (
    const CFormationEntryTempl_Delegate& rhs);

  CFormationEntryTempl <FORMATION> * m_formationEntryTempl;

  REGISTER_DELEGATE(CFormationEntryTempl <FORMATION>,
    CFormationEntryTempl_Delegate <FORMATION>);
};

template <class FORMATION>
  CFormationEntryTempl_Delegate <FORMATION> ::
    CFormationEntryTempl_Delegate(CFormationEntryTempl <FORMATION> *
      formationEntryTempl)
: CStorageNodeEntry_Delegate(formationEntryTempl)
, m_formationEntryTempl(formationEntryTempl)
{
  ACTIVATE_TEMPLATE_DELEGATE(CFormationEntryTempl <FORMATION>,
    CFormationEntryTempl_Delegate <FORMATION>);
}

template <class FORMATION>
  void CFormationEntryTempl_Delegate <FORMATION> ::
    AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CFormationEntryTempl <FORMATION> >
    TForCommand;

#if 0
  invoker.AddCommand("Show element center points",
    *(new TForCommand(*m_formationEntryTempl,
      &CFormationEntryTempl <FORMATION> ::OnShowMidpoints, 0,
      &CFormationEntryTempl <FORMATION> ::ShowMidpoints)));

  invoker.AddCommand("Show full elements",
    *(new TForCommand(*m_formationEntryTempl,
      &CFormationEntryTempl <FORMATION> ::OnShowFullMesh, 0,
      &CFormationEntryTempl <FORMATION> ::ShowFullMesh)));
#endif
}

template <class FORMATION>
  bool CFormationEntryTempl_Delegate <FORMATION> ::CanExportGocad() const
{
  return m_formationEntryTempl->CanExportGocad();
}

template <class FORMATION>
  void CFormationEntryTempl_Delegate <FORMATION> ::OnExportGocad()
{
  CGocadExportWrapper gew;

  gew.Export(static_cast <CModelBase&> (m_formationEntryTempl->Model()));
}

class TFormationBaseEntry_Delegate :
  public CGraphEntryTemp_Delegate <CFormationBase>
{
public:
  TFormationBaseEntry_Delegate(TFormationBaseEntry* formationBaseEntry);

private:
  TFormationBaseEntry_Delegate(const TFormationBaseEntry_Delegate& rhs);
  TFormationBaseEntry_Delegate& operator = (
    const TFormationBaseEntry_Delegate& rhs);

  TFormationBaseEntry* m_formationBaseEntry;

  REGISTER_DELEGATE(TFormationBaseEntry, TFormationBaseEntry_Delegate);
};

#endif  // _FormationBase_Delegate_h_
