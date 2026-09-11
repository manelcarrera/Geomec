#if !defined(__TIME_LAPSE_CONTROL_OBJECTS_H__)
#define __TIME_LAPSE_CONTROL_OBJECTS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEntryTypes.h"
#include "ComboListCtrl.h"
#include "ISubListObject.h"
#include "ModelBase.h"

class CDepletionStage;
class CModelBase;

#include "QBSettings.h"

class CTimeLapseComboListCtrl : public CComboListCtrl {
  const CModelBase &m_Model;

public:
  CTimeLapseComboListCtrl(const CModelBase &model) : m_Model(model) {}

protected:
  virtual void OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList &lstItems) {
    TDepletionStageEntry *pEntry = (TDepletionStageEntry *)(m_Model.GraphEntry(MD_BASE_DEPLETION_STAGE));
    TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
    TDepletionStageEntry::TNodeSet::const_iterator it;
    for (it = stDepletionStage.begin(); it != stDepletionStage.end(); ++it) {
      lstItems.AddTail((*it)->Name().toStdString().c_str());
    }
  }
};

template <class T, TTimeLapseType TLT = Pressure> class CTimeLapseListObject : public IListObject {
  CDepletionStage *m_t1;
  CDepletionStage *m_t2;
  T *m_Parent;
  const CModelBase &m_Model;

  CDepletionStage *Update(const CString &strDepl) {
    TDepletionStageEntry *pEntry = (TDepletionStageEntry *)(m_Model.GraphEntry(MD_BASE_DEPLETION_STAGE));
    TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
    TDepletionStageEntry::TNodeSet::const_iterator it;
    for (it = stDepletionStage.begin(); it != stDepletionStage.end(); ++it) {
      if ((*it)->Name() == strDepl)
        return *it;
    }
    assert(FALSE);
    return 0;
  }

public:
  CTimeLapseListObject(const CModelBase &model, CListCtrl &ctrl, CDepletionStage *t1, CDepletionStage *t2, T *parent)
      : IListObject(ctrl), m_Model(model), m_t1(t1), m_t2(t2), m_Parent(parent) {
    Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
    new CTimeLapseSubObject<T, TLT>(*this, 1);
    new CTimeLapseSubObject<T, TLT>(*this, 2);
  }
  virtual QString Text() const {
    QString strIndex;
    strIndex = QString("%1").arg(Index() + m_Parent->IndexOffset());
    return m_Parent->ListObjectName() + strIndex;
  }
  virtual BOOL CanEditText() const { return FALSE; }
  virtual unsigned int Icon() const { return 0; }

  void SetTime1(const CString &strDepl) {
    m_t1 = Update(strDepl);
    m_Parent->SetTimeLapse(Index(), m_t1, m_t2, TLT);
  }
  void SetTime2(const CString &strDepl) {
    m_t2 = Update(strDepl);
    m_Parent->SetTimeLapse(Index(), m_t1, m_t2, TLT);
  }
  const CDepletionStage *Time1() const { return m_t1; }
  const CDepletionStage *Time2() const { return m_t2; }
};

template <class T, TTimeLapseType TLT> class CTimeLapseSubObject : public ISubListObject {
  CTimeLapseListObject<T, TLT> &m_parent;

public:
  CTimeLapseSubObject(CTimeLapseListObject<T, TLT> &parent, int nIndex)
      : ISubListObject(parent, nIndex), m_parent(parent) {}
  virtual QString Text() const {
    if (Index() == 1)
      return m_parent.Time1()->Name();
    else
      return m_parent.Time2()->Name();
  }
  virtual BOOL EditText(const QString &strText) {
    if (Index() == 1)
      m_parent.SetTime1(strText.toStdString().c_str());
    else
      m_parent.SetTime2(strText.toStdString().c_str());
    return TRUE;
  }
  virtual BOOL CanEditText() const { return TRUE; }
};

#endif