// AttriPointSetUtil.h: interface for the AttriPointSetUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIPOINTSETUTIL_H__878F8A8F_BB03_4C15_BA95_501061BC4DFF__INCLUDED_)
#define AFX_ATTRIPOINTSETUTIL_H__878F8A8F_BB03_4C15_BA95_501061BC4DFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CValueType_Delegate;
class IValueSet_Delegate;
class CValueComponent;

#include "IValueSet.h"
#include "ValueType.h"
#include "IPointSet.h"
#include "treectrlbase.h"
#include "IListObject.h"
#include "ListCtrlBase.h"

template<class T>
class CTreeCtrlTemp : public CTreeCtrlBase
{
	T& m_dlg;
public:
	CTreeCtrlTemp(T& dlg);
	virtual CWnd& GetContextWnd() const;
};

template<class T>
class CListCtrlTemp : public CListCtrlBase
{
	T& m_dlg;
public:
	CListCtrlTemp(T & dlg);
	virtual CWnd& GetContextWnd() const;
	virtual DROPEFFECT OnDragOver(TCtrlObjectVec vcDragged,
								  COleDataObject* pDataObject, 
								  DWORD dwKeyState, 
								  CPoint point);
	virtual BOOL OnDrop(TCtrlObjectVec vcDragged, 
						COleDataObject* pDataObject,
						DROPEFFECT dropEffect, 
						CPoint point);

};

class CComponentTreeObject;

// TODO AppendContextMenu
// CValueTypeTreeObject is not derived from CGraphNode, hence
// the function AppendContextMenu does not ask for a delegate. However one
// could verify that this function is never used!

class CValueTypeTreeObject : public ITreeObject
{
	CTreeCtrl& m_tree;
	CListCtrl& m_list;
	CValueType& m_value_type;
  CValueType_Delegate* valueType_Delegate;
public:
	// Construction 
	CValueTypeTreeObject(CListCtrl& list, CTreeCtrl& tree, CValueType& value_type);
  ~CValueTypeTreeObject();

	// Access to quantity
	CValueType& ValueType();
	const CValueType& ValueType() const;

	// Function to indentify coordinate value type of pointset
	bool IsCoordinate() const;

	// Control Access
	const CTreeCtrl& TreeCtrl() const;
	CTreeCtrl& TreeCtrl();
	const CListCtrl& ListCtrl() const;
	CListCtrl& ListCtrl();	

	// Operations ...
	virtual QString Text() const;
	virtual BOOL CanEditText() const;
	virtual BOOL EditText(const QString &sText);
	virtual BOOL CanDelete() const;
	virtual bool CanRemove() const { return m_value_type.CanDestroy(); }
	virtual void Delete();
	virtual unsigned int Icon() const;
	virtual BOOL OnBeginDrag();
	virtual void AppendContextMenu(CContextMenuInvoker &invoker);
	void AssertValid();
	CComponentTreeObject* Validate();
};

  // TODO AppendContextMenu
  // CComponentTreeObject is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

	class CComponentTreeObject : public ITreeObject
	{
		int m_nIndex;
    IValueSet_Delegate* m_valueSet_Delegate;
	public:
		// Construction ...
		CComponentTreeObject(CValueTypeTreeObject &root, int nIndex);
    ~CComponentTreeObject();

		// Access to TreeObject
		const CValueTypeTreeObject& ValueTypeTreeObject() const;
		CValueTypeTreeObject& ValueTypeTreeObject();

		// Operations
		virtual QString Text() const;
		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString &sText);
		virtual unsigned int Icon() const;
		CValueComponent& Component();
		const CValueComponent& Component() const;
		virtual BOOL OnBeginDrag();
		virtual DROPEFFECT CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const;
		virtual void Drop(TCtrlObjectVec &vcDragged, BOOL bMove);
		virtual BOOL CanDelete() const;
		virtual bool CanRemove() const { return (CanDelete() != FALSE); }
		virtual void Delete();
		virtual void AppendContextMenu(CContextMenuInvoker &invoker);
		bool IsInRange(const IValueSet& value_set) const;
		int Index() const;
		void AssertValid();
	};


  // TODO AppendContextMenu
  // CValueSetListObject is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

	class CValueSetListObject : public IListObject
	{
		CString m_sName;
		IValueSet &m_value_set;
    IValueSet_Delegate* m_valueSet_Delegate;
	public:
		CValueSetListObject(CListCtrl& ctrl, IValueSet& set);
    ~CValueSetListObject();
		virtual QString Text() const;
		virtual unsigned int Icon() const;
		virtual BOOL CanEditText() const;
		virtual BOOL EditText(const QString &sText);
		virtual BOOL CanDelete() const;
		virtual bool CanRemove() const { return (CanDelete() != FALSE); }
		virtual void Delete();
		virtual IValueSet& ValueSet() const;
		virtual void AppendContextMenu(CContextMenuInvoker &invoker);
		void AssertValid();
	}; 

///////////////////////////////////////////////////////////////////////////////////////////
/// Implementation of CTreeCtrlTemp<T>
///////////////////////////////////////////////////////////////////////////////////////////
template<class T>
CTreeCtrlTemp<T>::CTreeCtrlTemp(T& dlg)
: m_dlg(dlg)
{
}

template<class T>
CWnd& CTreeCtrlTemp<T>::GetContextWnd() const
{
	return (CWnd&)(m_dlg);
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Implementation of CListCtrlTemp<T>
///////////////////////////////////////////////////////////////////////////////////////////
template<class T>
CListCtrlTemp<T>::CListCtrlTemp(T& dlg)
: m_dlg(dlg)
{
}

template<class T>
DROPEFFECT CListCtrlTemp<T>::OnDragOver(TCtrlObjectVec vcDragged,
										COleDataObject* pDataObject, 
										DWORD dwKeyState, 
										CPoint point)
{
	if(vcDragged.size() == 1)
	{
		CComponentTreeObject *pComponentObj = dynamic_cast<CComponentTreeObject*>(vcDragged[0]);
		if(pComponentObj)
		{
			if(pComponentObj->Component().Defined())
				return DROPEFFECT_MOVE;
		}
	}

	return CListCtrlBase::OnDragOver(vcDragged, pDataObject, dwKeyState, point);
}

template<class T>
BOOL CListCtrlTemp<T>::OnDrop(TCtrlObjectVec vcDragged, 
							  COleDataObject* pDataObject,
							  DROPEFFECT dropEffect, 
							  CPoint point)
{
	if(vcDragged.size() == 1)
	{
		CComponentTreeObject *pComponentObj = dynamic_cast<CComponentTreeObject*>(vcDragged[0]);
		if(pComponentObj)
		{
			if(pComponentObj->Component().Defined())
			{
				pComponentObj->Delete();
				return TRUE;
			}
		}
	}

	return CListCtrlBase::OnDrop(vcDragged, pDataObject, dropEffect, point);
}

template<class T>
CWnd& CListCtrlTemp<T>::GetContextWnd() const
{
	return (CWnd&)(m_dlg);
}

#endif // !defined(AFX_ATTRIPOINTSETUTIL_H__878F8A8F_BB03_4C15_BA95_501061BC4DFF__INCLUDED_)
