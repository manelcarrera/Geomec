// HexaFormationObserver.h: interface for the CHexaFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXAFORMATIONOBSERVER_H__77C74751_A9B7_4b20_B1F1_A1D916FAA9B6__INCLUDED_)
#define AFX_HEXAFORMATIONOBSERVER_H__77C74751_A9B7_4b20_B1F1_A1D916FAA9B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DFormation.h"
#include "FormationVolume.h"
#include "FormationPlane.h"
#include "OpenGLNodeObserver.h"
#include "MeshSurface.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "HexaFormation_Delegate.h"
#include "StateBranch_Delegate.h"
#include "FormationPlane_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CHexaFormation, CHexaFormation_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> THexaFormationObserver;
class CHexaFormationEntryObserver : public CStateBranch_Delegate<CHexaFormationEntry, CHexaFormationEntry_Delegate, CHexaFormation, THexaFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>
{
	typedef COpenGLNodeObserver_Delegate<CFormationPlane, CFormationPlane_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TFormationPlaneObs;
	class CFormationPlaneObs : public TFormationPlaneObs
	{
	public:
		CFormationPlaneObs(CFormationPlane& node,
							CTreeNode &parent,
							const BOOL rename,
							const enum REMOVE_TYPE remove,
							HTREEITEM hInsertAfter = TVI_LAST,
							BOOL bInitialUpdate = TRUE,
							BOOL bSortAfterInsertion = FALSE );
		CFormationPlaneObs(CFormationPlane& node,
							CTreeCtrl &ctrl,
							const BOOL rename,
							const enum REMOVE_TYPE remove,
							HTREEITEM hParent = TVI_ROOT,
							HTREEITEM hInsertAfter = TVI_LAST,
							BOOL bInitialUpdate = TRUE,
							BOOL bSortAfterInsertion = FALSE);
		virtual unsigned int StateIcon() const;
		virtual void ToggleState();
	};
public:
	CHexaFormationEntryObserver(CHexaFormationEntry& entry, CTreeCtrl &ctrl, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	virtual CTreeNode* InsertChild(CHexaFormation& t);
};

#endif // !defined(AFX_HEXAFORMATIONOBSERVER_H__77C74751_A9B7_4b20_B1F1_A1D916FAA9B6__INCLUDED_)
