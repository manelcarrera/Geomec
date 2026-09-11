// EclipseFormation.cpp: implementation of the CEclipseFormation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "EclipseFormation.h"
#include "EntryTypes.h"
#include "EclipseModel.h"
#include "ValueCompositeObserver.h"
#include "PressureCompositeObserver.h"
#include "MaterialServer.h"
#include "AttriEclipseFormationDlg.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEclipseFormation::CEclipseFormation(CEclipseModel& model, const CEclipseReservoir& reservoir, CEclipseFormation* pUpper)
: CFormationBase(_T("Formation"), model), m_pLower(0), m_pUpper(pUpper), m_reservoir(reservoir)
{
	if(pUpper)
		pUpper->m_pLower = this;

	for(int x = 0; x < reservoir.SizeX(); x++)
	{
		for(int y = 0; y < reservoir.SizeY(); y++)
		{
			for(int z = reservoir.LowerBoundary(); z < reservoir.UpperBoundary(); z++)
			{
				if(reservoir.CellEnable(x,y,z))
					m_arView.PushBack(reservoir.Cell(x, y, z));
			}
		}
	}
	TEclipseFormationEntry* pEntry = (TEclipseFormationEntry*)(model.GraphEntry(MD_GEO_FORMATION));
	LinkTo(*pEntry);

	model.Boundary().LinkTo(*this);
}

UINT CEclipseFormation::TypeNameId() const
{
	return 0;
}

CEclipseFormation::~CEclipseFormation()
{

}

const geo::IArray& CEclipseFormation::DisplayList() const
{
	return m_arView;
}

const CEclipseReservoir& CEclipseFormation::Reservoir() const
{
	return m_reservoir;
}

void CEclipseFormation::Properties()
{
	CAttriEclipseFormationDlg dlg(*this);
	dlg.DoModal();
}

int CEclipseFormation::ElementSize() const
{
	return m_arView.Size();
}

const geo::IBody &CEclipseFormation::Element(int index) const
{
	return m_arView.Object(index);
}

CEclipseFormationEntryObserver::CEclipseFormationEntryObserver(TEclipseFormationEntry &entry,
														   CGraphTreeView &view,
														   HTREEITEM hParent,
														   HTREEITEM hInsertAfter)
: CStateBranch<TEclipseFormationEntry, CEclipseFormation, TEclipseFormationObserver, TRUE, FIXED_ITEM>(entry, view, IDI_TRI_CHECKED, FALSE, FIXED_ITEM, hParent, hInsertAfter, FALSE, FALSE) 
{
	OnUpdate();
}

CTreeNode* CEclipseFormationEntryObserver::InsertChild(CEclipseFormation &t)
{
	typedef CNodeObserver<TPressure, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TDisPresObs;	
	typedef CNodeObserver<CPressure, TPressure, CPressureCompositeObserver, FALSE, UNLINK_ITEM> TPressureObj;
	typedef CEnumerationBranch<CPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TPressureEnumerator;
	typedef CNodeObserver<CMaterialServer, CValueType, CValueCompositeObserver, FALSE, UNLINK_ITEM> TMaterialObserver;

	CTreeNode *pTreeObserver = CStateBranch<TEclipseFormationEntry, CEclipseFormation, TEclipseFormationObserver, TRUE, FIXED_ITEM>::InsertChild(t);

	new TPressureEnumerator(*pTreeObserver,
						   _T("Pressure"),
						   IDI_PRESSURES);
	new TMaterialObserver(t.Material(),
						  *pTreeObserver,
						  TRUE,
						  DELETE_ITEM,
						  TVI_LAST,
						  TRUE,
						  FALSE);


	return pTreeObserver;
}

