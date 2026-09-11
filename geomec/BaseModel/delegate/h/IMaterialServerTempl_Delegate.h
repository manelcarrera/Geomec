#ifndef _IMaterialServerTempl_Delegate_h_
#define _IMaterialServerTempl_Delegate_h_

#include "IMaterialServerTempl.h"
#include "IMaterialServerBase_Delegate.h"
#include "ContextMenuInvoker.h"
#include "DistriMatPropDlg.h"
#include "FemAppMainWindow.h"

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  class IMaterialServerTempl_Delegate : public IMaterialServerBase_Delegate
{
public:
  IMaterialServerTempl_Delegate(
    IMaterialServerTempl <MATERIAL, PARENT> * materialServerTempl);
  ~IMaterialServerTempl_Delegate();

  virtual bool CanAttributes() const;
  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanDestroy() const;
  virtual bool Destroy();

  bool CanConfigureDistributions() const;
  bool ConfigureDistributions();

private:
  IMaterialServerTempl_Delegate(const IMaterialServerTempl_Delegate& rhs);
  IMaterialServerTempl_Delegate& operator = (
    const IMaterialServerTempl_Delegate& rhs);

  IMaterialServerTempl <MATERIAL, PARENT> * m_materialServerTempl;

  typedef IMaterialServerTempl <MATERIAL, PARENT> IMaterialServerTemplTemplate;
  typedef IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT>
    IMaterialServerTempl_DelegateTemplate;

  REGISTER_DELEGATE(IMaterialServerTemplTemplate,
    IMaterialServerTempl_DelegateTemplate);
};

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    IMaterialServerTempl_Delegate(
      IMaterialServerTempl <MATERIAL, PARENT> * materialServerTempl)
: IMaterialServerBase_Delegate(materialServerTempl)
, m_materialServerTempl(materialServerTempl)
{
  ACTIVATE_TEMPLATE_DELEGATE(IMaterialServerTemplTemplate,
    IMaterialServerTempl_DelegateTemplate);
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    ~IMaterialServerTempl_Delegate()
{
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT>::
    CanAttributes() const
{
  return m_materialServerTempl->LibraryMaterial() != 0;
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT>::
    Attributes()
{
  if (m_materialServerTempl->LibraryMaterial())
  {
    return static_cast <MATERIAL_DELEGATE*> (
      m_materialServerTempl->LibraryMaterial()->getDelegate())->Attributes();
  }

  return false;
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  void IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <
    IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> >
      TMaterialServer_DelegateCommand;

  invoker.AddCommand(_T("&Copy"), *(new TMaterialServer_DelegateCommand(*this,
    &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
      OnCopy)));
  invoker.AddCommand(_T("&Paste"), *(new TMaterialServer_DelegateCommand(*this,
    &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
      OnPaste,
    &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
      OnCanPaste)));
  invoker.AddCommand(_T("&Remove from formation"),
    *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Destroy,
      &CGraphNode_Delegate::CanDestroy)));

  typedef CSingleCommandTemplate <IMaterialServerTempl <MATERIAL, PARENT> >
    TMaterialServerCommand;

  invoker.AddCommand(_T("Configure distributed properties"),
    *new TMaterialServer_DelegateCommand(*this,
      &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
        ConfigureDistributions,
      &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
        CanConfigureDistributions));
  invoker.AddSeparator();

  // Append attributes ...

  invoker.AddCommand(_T("&Attributes"), *(new TMaterialServer_DelegateCommand(
    *this,
    &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
      Attributes,
    &IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
      CanAttributes)));
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    CanDestroy() const
{
  return m_materialServerTempl->CanDestroy();
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    Destroy()
{
  return m_materialServerTempl->Destroy();
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    CanConfigureDistributions() const
{
  return m_materialServerTempl->CanConfigureDistributions();
}

template <class MATERIAL, class MATERIAL_DELEGATE, class PARENT>
  bool IMaterialServerTempl_Delegate <MATERIAL, MATERIAL_DELEGATE, PARENT> ::
    ConfigureDistributions()
{
  std::vector <const CValueType*> vcValueType;

  for (size_t ii = 0; ii < m_materialServerTempl->ValueTypePairVec().size(); ++ii)
  {
    vcValueType.push_back(
      m_materialServerTempl->ValueTypePairVec().operator [](ii).first);
  }

  assert(CanConfigureDistributions());

  IMaterialServerTempl <MATERIAL, PARENT> ::TValueTypeSet
    extrapolatingValueTypesCopy =
      m_materialServerTempl->getExtrapolatingValueTypes();

  CDistriMatPropDlg dlg(vcValueType,
    m_materialServerTempl->getExtrapolatingValueTypes(), FemAppGetMainWnd());

  dlg.DoModal();

  //m_materialServerTempl->clearCachingComponents(vcValueType,
  //  extrapolatingValueTypesCopy);

  return true;
}

#endif  // _IMaterialServerTempl_Delegate_h_
