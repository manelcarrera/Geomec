// IListObject.h: interface for the IListObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILISTOBJECT_H__EBD66F8A_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
#define AFX_ILISTOBJECT_H__EBD66F8A_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcview.h>
#include "ICtrlObject.h"
#include <map>

class ISubListObject;
class IListObject : public ICtrlObject
{
  friend class ISubListObject;	// Just for registering ...
  CListCtrl& m_ctrl;
  typedef std::map<int, ISubListObject*> TSubListMap;
  TSubListMap m_mpSubObject;
protected:
  CListCtrl& Ctrl() const;
  IListObject(CListCtrl& ctrl, int nIndex = -1, BOOL bSelected = FALSE);
public:
  int Index() const;
  std::size_t SubObjectSize() const;
  ISubListObject* SubObject(const int nIndex);
  const ISubListObject* SubObject(const int nIndex) const;
  virtual BOOL IsSelected() const;
  virtual void SelectionStateChanged(bool bSelected);

  virtual ~IListObject();
  virtual void Update();

  virtual void OnDoubleClick(); // double click on the entire row
  virtual void OnDoubleClick(int nItemIndex); // double click on an item (0 = main item)

};

typedef std::vector<IListObject*> TListObjectVec;

#endif // !defined(AFX_ILISTOBJECT_H__EBD66F8A_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
