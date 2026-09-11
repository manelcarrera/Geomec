// ModelObserver.cpp: implementation of the CModelObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "ModelObserver.h"
#include "GraphTreeView.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif
/*
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3BC55D610228
CModelObserver::CModelObserver(CFemAppDoc &doc,
							   CGraphTreeView &view,
							   HTREEITEM hParent,
							   HTREEITEM hInsertAfter)
: ITreeObject(view.GetTreeCtrl(), hParent, hInsertAfter), m_document(doc)
{
	Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));

}


//##ModelId=3BC55D61023F
unsigned int CModelObserver::Icon() const
{
	return 0;
}

//##ModelId=3BC55D61022D
void CModelObserver::Delete() 
{
	assert(FALSE);
}

//##ModelId=3BC55D61022F
BOOL CModelObserver::CanDelete() const
{
	return FALSE;
}

//##ModelId=3BC55D610239
BOOL CModelObserver::CanEditText() const
{
	return false;
}
/*
//##ModelId=3BC55D61023B
const CGraphNode& CModelObserver::ObservedItem() const
{
	return *this;
}


//##ModelId=3BC55D61023D
CGraphNode& CModelObserver::ObservedItem()
{
	return *this;
}
*/