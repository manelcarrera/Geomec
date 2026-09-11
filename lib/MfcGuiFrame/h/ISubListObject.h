// ISubListObject.h: interface for the ISubListObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISUBLISTOBJECT_H__00AB66A5_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
#define AFX_ISUBLISTOBJECT_H__00AB66A5_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IListObject.h"

class ISubListObject : public ICtrlObjectBase
{
	IListObject& m_list_object;
	CWnd *m_pCustomControl;
	int m_nIndex;
public:
	ISubListObject(IListObject &list_object, int nSubIndex);
	virtual ~ISubListObject();
	const IListObject& ListObject() const;
	IListObject& ListObject();
	int Index() const;

	CRect Rect() const;
	void SetCustomControl(CWnd *pWnd);
	const CWnd *Control() const;
	CWnd *Control();

	virtual void OnDoubleClick(const CPoint &point) {}

protected:
  CListCtrl& Ctrl() const;
};

#endif // !defined(AFX_ISUBLISTOBJECT_H__00AB66A5_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
