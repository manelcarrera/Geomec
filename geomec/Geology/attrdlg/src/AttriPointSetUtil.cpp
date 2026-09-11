// AttriPointSetUtil.cpp: implementation of the AttriPointSetUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "AttriPointSetUtil.h"
#include "ContextMenuInvoker.h"
#include "GlobalMessage.h"
#include "IValueSet_Delegate.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "RpnValueSet.h"
#include "RpnValueSet_Delegate.h"
#include "ValueComponent.h"
#include "ValueType_Delegate.h"
#include "geomec.h"
#include "resourceIDS.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueTypeTreeObject::CValueTypeTreeObject(CListCtrl &list, CTreeCtrl &tree, CValueType &value_type)
    : ITreeObject(tree, TVI_ROOT, TVI_LAST), m_value_type(value_type), m_tree(tree), m_list(list),
      valueType_Delegate(0) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));

  assert(value_type.IsCopy());

  for (int i = 0; i < ValueType().ComponentSize(); i++)
    new CComponentTreeObject(*this, i);
  Update();
}

CValueTypeTreeObject::~CValueTypeTreeObject() {
  if (valueType_Delegate != 0) {
    delete valueType_Delegate;
  }
}

CComponentTreeObject *CValueTypeTreeObject::Validate() {
  // Iterate over components and fetch the CValueComponent
  TChildren vcComponent = Children();

  if (IsCoordinate()) {
    CComponentTreeObject *pTreeComponent0 = dynamic_cast<CComponentTreeObject *>(vcComponent[0]);
    assert(pTreeComponent0);
    CComponentTreeObject *pTreeComponent1 = dynamic_cast<CComponentTreeObject *>(vcComponent[1]);
    assert(pTreeComponent1);
    CModelBase &model = (CModelBase &)(pTreeComponent1->Component().Model());
    if (!pTreeComponent0->Component().Defined())
      return pTreeComponent0;
    if (!pTreeComponent1->Component().Defined())
      return pTreeComponent1;
  } else {
    for (size_t i = 0; i < vcComponent.size(); i++) {
      CComponentTreeObject *pTreeComponent = dynamic_cast<CComponentTreeObject *>(vcComponent[i]);
      assert(pTreeComponent);
      if (!pTreeComponent->Component().Defined())
        return pTreeComponent;
    }
  }

  return 0;
}

QString CValueTypeTreeObject::Text() const { return ValueType().Name(); }

BOOL CValueTypeTreeObject::CanEditText() const { return TRUE; }

BOOL CValueTypeTreeObject::EditText(const QString &sText) {
  ValueType().Name(sText);
  return TRUE;
}

/* NOT OBSOLETE */ void CValueTypeTreeObject::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CValueTypeTreeObject> TCommandTemp;
  typedef CSingleCommandTemplate<CValueType> TValueTypeTemp;
  typedef CSingleCommandTemplate<CValueType_Delegate> TValueTypeTemp_Delegate;
  invoker.AddCommand(_T("&Remove"),
                     *(new TCommandTemp(*this, &CValueTypeTreeObject::Delete, &CValueTypeTreeObject::CanRemove)));
}

void CValueTypeTreeObject::AssertValid() {}

const CValueType &CValueTypeTreeObject::ValueType() const { return m_value_type; }

CValueType &CValueTypeTreeObject::ValueType() { return m_value_type; }

const CTreeCtrl &CValueTypeTreeObject::TreeCtrl() const { return m_tree; }

CTreeCtrl &CValueTypeTreeObject::TreeCtrl() { return m_tree; }

const CListCtrl &CValueTypeTreeObject::ListCtrl() const { return m_list; }

CListCtrl &CValueTypeTreeObject::ListCtrl() { return m_list; }

// ##ModelId=3C74E3FC004E
unsigned int CValueTypeTreeObject::Icon() const { return ValueType().IconId(); }

bool CValueTypeTreeObject::IsCoordinate() const {
  const CPointSet *pPointSet = dynamic_cast<const CPointSet *>(&ValueType().PointSet());
  if (pPointSet)
    return &pPointSet->Coordinates() == &ValueType().Source();
  return false;
}

// ##ModelId=3C74E3FC0050
BOOL CValueTypeTreeObject::OnBeginDrag() { return FALSE; }

// ##ModelId=3C74E3FC0041
BOOL CValueTypeTreeObject::CanDelete() const { return ValueType().CanDestroy(); }

// ##ModelId=3C74E3FC0043
void CValueTypeTreeObject::Delete() {
  assert(CanDelete());

  // Move the value set to the list control
  for (int i = 0; i < ValueType().ComponentSize(); i++) {
    if (ValueType().Component(i).Defined()) {
      CValueComponent *pComponent = (CValueComponent *)&ValueType().Component(i);
      new CValueSetListObject(ListCtrl(), pComponent->ValueSet());
    }
  }

  CValueType *pValueType = &ValueType();

  // Remove list item from list
  delete this;

  pValueType->Destroy();
}

// ##ModelId=3C74E3FC01B5
CValueSetListObject::CValueSetListObject(CListCtrl &ctrl, IValueSet &value_set)
    : IListObject(ctrl), m_value_set(value_set), m_valueSet_Delegate(0) {
  assert(value_set.IsCopy());
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
}

CValueSetListObject::~CValueSetListObject() {
  if (m_valueSet_Delegate != 0) {
    delete m_valueSet_Delegate;
  }
}

// ##ModelId=3C74E3FC01B8
QString CValueSetListObject::Text() const { return ValueSet().Name(); }

// ##ModelId=3C74E3FC01BA
unsigned int CValueSetListObject::Icon() const { return ValueSet().IconId(); }

// ##ModelId=3C74E3FC01C5
BOOL CValueSetListObject::CanEditText() const { return TRUE; }

BOOL CValueSetListObject::CanDelete() const { return TRUE; }

void CValueSetListObject::Delete() {
  assert(ValueSet().IsCopy());
  IValueSet *pValueSet = &ValueSet();
  delete this;
  pValueSet->Destroy();
}

// ##ModelId=3C74E3FC01C7
BOOL CValueSetListObject::EditText(const QString &sText) {
  ValueSet().Name(sText);
  return TRUE;
}

// ##ModelId=3C74E3FC01CA
IValueSet &CValueSetListObject::ValueSet() const {
  assert(m_value_set.IsCopy());
  return m_value_set;
}

// ##ModelId=3C74E3FC01D5
/* NOT OBSOLETE */ void CValueSetListObject::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CValueSetListObject> TCommandTemp;
  typedef CSingleCommandTemplate<IValueSet> TValueSetTemp;
  typedef CSingleCommandTemplate<IValueSet_Delegate> TValueSetTemp_Delegate;
  invoker.AddCommand(_T("&Remove"),
                     *(new TCommandTemp(*this, &CValueSetListObject::Delete, &CValueSetListObject::CanRemove)));
  invoker.AddSeparator();

  if (m_valueSet_Delegate) {
    delete m_valueSet_Delegate;
    m_valueSet_Delegate = 0;
  }

  IValueSet_Delegate *valueSet_Delegate = static_cast<IValueSet_Delegate *>(ValueSet().getDelegate());

  if (valueSet_Delegate == 0) {
    valueSet_Delegate = m_valueSet_Delegate = new IValueSet_Delegate(&ValueSet());
  }
  invoker.AddCommand(_T("&Attributes"),
                     *(new TValueSetTemp_Delegate(*valueSet_Delegate, &IValueSet_Delegate::Attributes)));
}

CComponentTreeObject::CComponentTreeObject(CValueTypeTreeObject &root, int nIndex)
    : ITreeObject(root, TVI_LAST), m_nIndex(nIndex), m_valueSet_Delegate(0) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));
  Update();

  assert(Component().IsCopy());
}

CComponentTreeObject::~CComponentTreeObject() {
  if (m_valueSet_Delegate != 0) {
    delete m_valueSet_Delegate;
  }
}

// ##ModelId=3C74E3FC00FD
QString CComponentTreeObject::Text() const {
  //	if(Component().Defined())
  //		return Component().ValueSet().Name();

  //	if(ValueTypeTreeObject().ValueType().ComponentSize() > 1)
  return Component().Name();

  //	return "Value"; //Component().ComponentName();
}

// ##ModelId=3C74E3FC00FF
BOOL CComponentTreeObject::CanEditText() const { return Component().Defined(); }

// ##ModelId=3C74E3FC010B
BOOL CComponentTreeObject::EditText(const QString &sText) {
  assert(Component().Defined());
  Component().ValueSet().Name(sText);
  return TRUE;
}

// ##ModelId=3C74E3FC0119
const CValueComponent &CComponentTreeObject::Component() const {
  assert(m_nIndex > -1);
  assert(m_nIndex < ValueTypeTreeObject().ValueType().ComponentSize());

  return (const CValueComponent &)ValueTypeTreeObject().ValueType().Component(m_nIndex);
}

int CComponentTreeObject::Index() const { return m_nIndex; }

CValueComponent &CComponentTreeObject::Component() {
  assert(m_nIndex > -1);
  assert(m_nIndex < ValueTypeTreeObject().ValueType().ComponentSize());

  return (CValueComponent &)ValueTypeTreeObject().ValueType().Component(m_nIndex);
}
// Access to
// ##ModelId=3C74E3FC00FA
const CValueTypeTreeObject &CComponentTreeObject::ValueTypeTreeObject() const {
  assert(Parent());
  return (const CValueTypeTreeObject &)(*Parent());
}

// ##ModelId=3C74E3FC00FC
CValueTypeTreeObject &CComponentTreeObject::ValueTypeTreeObject() {
  assert(Parent());
  return (CValueTypeTreeObject &)(*Parent());
}

// ##ModelId=3C74E3FC010E
unsigned int CComponentTreeObject::Icon() const { return Component().IconId(); }

// ##ModelId=3C74E3FC011C
BOOL CComponentTreeObject::OnBeginDrag() { return Component().Defined(); }

// ##ModelId=3C74E3FC0138
DROPEFFECT CComponentTreeObject::CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const {
  assert(vcDragged.size() == 1);

  CComponentTreeObject *pTreeSet = dynamic_cast<CComponentTreeObject *>(vcDragged[0]);

  if (pTreeSet) {
    if ((pTreeSet != this) && (pTreeSet->Component().Defined()) &&
        (!ValueTypeTreeObject().IsCoordinate() ||
         (dynamic_cast<const CRpnValueSet *>(&pTreeSet->Component().ValueSet()) == 0)))
      return DROPEFFECT_MOVE;
  }

  CValueSetListObject *pListSet = dynamic_cast<CValueSetListObject *>(vcDragged[0]);

  if (pListSet &&
      (!ValueTypeTreeObject().IsCoordinate() || (dynamic_cast<const CRpnValueSet *>(&pListSet->ValueSet()) == 0)))
    return DROPEFFECT_MOVE;

  return DROPEFFECT_NONE; // Default no drop target ..
}

// ##ModelId=3C74E3FC014B
BOOL CComponentTreeObject::CanDelete() const { return Component().Defined(); }

// ##ModelId=3C74E3FC014D
void CComponentTreeObject::Delete() {
  assert(CanDelete());
  assert(Component().IsCopy());

  new CValueSetListObject(ValueTypeTreeObject().ListCtrl(), Component().ValueSet());

  assert(Component().IsCopy());
  assert(Component().ValueSet().IsCopy());
  Component().UnLink(Component().ValueSet());
  assert(!Component().Defined());

  Update();
}

// ##ModelId=3C74E3FC013C
void CComponentTreeObject::Drop(TCtrlObjectVec &vcDragged, BOOL bMove) {
  assert(vcDragged.size() == 1);
  IValueSet *pValueSet = 0;

  CComponentTreeObject *pTree = dynamic_cast<CComponentTreeObject *>(vcDragged[0]);
  CValueSetListObject *pList = dynamic_cast<CValueSetListObject *>(vcDragged[0]);
  if (pTree) {
    if ((pTree != this) && pTree->Component().Defined()) {
      // Check range first
      if (!IsInRange(pTree->Component().ValueSet()))
        return; // Cannot use it, because out of range

      // Disconnect value set from source component
      pValueSet = &pTree->Component().ValueSet();
      pTree->Component().UnLink(*pValueSet);
      pTree->Update();
    }
  }

  if (pList) {
    // Check range first
    if (!IsInRange(pList->ValueSet()))
      return; // Cannot use it, because out of range
    pValueSet = &pList->ValueSet();
    delete pList;
  }

  assert(pValueSet);

  if (Component().Defined()) {
    new CValueSetListObject(ValueTypeTreeObject().ListCtrl(), Component().ValueSet());
    Component().UnLink(Component().ValueSet());
  }

  // Connect to new ...
  Component().LinkTo(*pValueSet);
  if (Component().Parent().ComponentSize() == 1)
    Component().Parent().Name(pValueSet->Name());
  ValueTypeTreeObject().Update();
}

bool CComponentTreeObject::IsInRange(const IValueSet &value_set) const {
  CGeomecDoc &doc = *GetGeomecDoc();
  if (!Component().IsInRange(value_set)) {
    CString sMessage;
    if (Component().RangeMax(doc.UnitNode().Unit()).Valid() && Component().RangeMin(doc.UnitNode().Unit()).Valid())
      sMessage.Format(IDS_COMPONENT_NOT_IN_RANGE_MIN_MAX, Component().RangeMin().Value(),
                      Component().RangeMax().Value());
    else if (Component().RangeMax(doc.UnitNode().Unit()).Valid())
      sMessage.Format(IDS_COMPONENT_NOT_IN_RANGE_MAX, Component().RangeMax().Value());
    else
      sMessage.Format(IDS_COMPONENT_NOT_IN_RANGE_MIN, Component().RangeMin().Value());
    _m()->msg(sMessage);
    return false;
  }

  return true;
}

// ##ModelId=3C74E3FC014F
/* NOT OBSOLETE */ void CComponentTreeObject::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CComponentTreeObject> TCommandTemp;
  typedef CSingleCommandTemplate<IValueSet> TValueSetTemp;
  typedef CSingleCommandTemplate<IValueSet_Delegate> TValueSetTemp_Delegate;
  invoker.AddCommand(_T("&Remove"),
                     *(new TCommandTemp(*this, &CComponentTreeObject::Delete, &CComponentTreeObject::CanRemove)));
  if (Component().Defined()) {
    invoker.AddSeparator();

    if (m_valueSet_Delegate != 0) {
      delete m_valueSet_Delegate;
      m_valueSet_Delegate = 0;
    }

    IValueSet_Delegate *valueSet_Delegate = static_cast<IValueSet_Delegate *>(Component().ValueSet().getDelegate());

    if (valueSet_Delegate == 0) {
      assert(FALSE);
      valueSet_Delegate = m_valueSet_Delegate = new IValueSet_Delegate(&Component().ValueSet());
    }

    invoker.AddCommand(_T("&Attributes"),
                       *(new TValueSetTemp_Delegate(*valueSet_Delegate, &IValueSet_Delegate::Attributes)));
  }
}
