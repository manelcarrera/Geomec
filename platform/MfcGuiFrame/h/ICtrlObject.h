// ICtrlObject.h: interface for the ICtrlObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICTRLOBJECT_H__EBD66F89_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
#define AFX_ICTRLOBJECT_H__EBD66F89_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICtrlObjectBase.h"
#include <vector>

class ICtrlObject;
class CContextMenuInvoker;
typedef std::vector<ICtrlObject*> TCtrlObjectVec;

class ICtrlObject : public ICtrlObjectBase  
{
protected:
  // Contruction
  ICtrlObject();
  virtual ~ICtrlObject();
public:
  // Icon interface ...
  virtual unsigned int SelectedIcon() const;
  virtual unsigned int StateIcon() const;
  virtual unsigned int Icon() const = 0;

  // Drag and drop
  virtual BOOL OnBeginDrag();
  virtual	DROPEFFECT CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const;
  virtual void Drop(TCtrlObjectVec &vcDragged, BOOL bMove);
  virtual void OnDragLeave(TCtrlObjectVec& vcDragged) const;

  // Delete actions
  virtual void Delete();
  virtual BOOL CanDelete() const;

  virtual void Modify();
  virtual BOOL CanModify() const;


  // Selection
  virtual BOOL OnSelect();
  virtual BOOL IsSelected() const = 0;

  // Double click and context menu
  virtual void OnDoubleClick();
  virtual void AppendToMenu(CMenu& menu);
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  // State ...
  virtual void ToggleState();

  // Update label and text
  virtual void Update() = 0;
};



#endif // !defined(AFX_ICTRLOBJECT_H__EBD66F89_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
