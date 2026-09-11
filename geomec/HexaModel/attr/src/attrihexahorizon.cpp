// attrihorizon.cpp : implementation file
//

#include "stdafx.h"
#include <cmath>
#include "geomec.h"
#include "attrihexahorizon.h"
#include "BaseEntryTypes.h"
#include "BoundaryBase.h"
#include "Modelbase.h"
#include "FieldFactor.h"
#include "TetraModel.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

CAttri3DHorizonDlg::CSurfaceListObject::CSurfaceListObject(CListCtrl &ctrl, CSurfaceBase& surface, BOOL bSelected)
: IListObject(ctrl, -1, bSelected), m_pSurface(&surface)
{
	assert(surface.IsCopy());
	ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR)this);
}

CAttri3DHorizonDlg::CSurfaceListObject::~CSurfaceListObject()
{
}

QString CAttri3DHorizonDlg::CSurfaceListObject::Text() const
{
	return m_pSurface->Name();
}

unsigned int CAttri3DHorizonDlg::CSurfaceListObject::Icon() const
{
	return m_pSurface->IconId();
}

const CSurfaceBase& CAttri3DHorizonDlg::CSurfaceListObject::Surface() const
{
	return *m_pSurface;
}

CSurfaceBase& CAttri3DHorizonDlg::CSurfaceListObject::Surface()
{ 
	return *m_pSurface;
}

/////////////////////////////////////////////////////////////////////////////
// CAttri3DHorizonDlg dialog


//##ModelId=3BC55D6501BD
CAttri3DHorizonDlg::CAttri3DHorizonDlg(C3DHorizon &horizon, CWnd* pParent /*=NULL*/)
: CAttriHorizon<C3DHorizon>(CAttri3DHorizonDlg::IDD, horizon, true, pParent), m_bInitialUpdate(TRUE)
{
	//{{AFX_DATA_INIT(CAttri3DHorizonDlg)
	//}}AFX_DATA_INIT
}


//##ModelId=3BC55D6501D0
void CAttri3DHorizonDlg::DoDataExchange(CDataExchange* pDX)
{
	// Variables ....
	CString strName, strUnit;
	double dDepth, dBottom, dTop;
	int nSelect, nDivisionNorthing, nDivisionEasting;
	BOOL bSlip;
	bool bBranch = (static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch();

	if(!pDX->m_bSaveAndValidate)
	{
		strName = Copy().Name().toStdString().c_str();

		GetDlgItem(IDC_RD_CONST_DEPTH)->EnableWindow(!bBranch);
		GetDlgItem(IDC_ST_DEPTH_UNIT)->EnableWindow(Copy().ConstantDepth() && !bBranch);
		GetDlgItem(IDC_ED_DEPTH)->EnableWindow(Copy().ConstantDepth() && !bBranch);
		GetDlgItem(IDC_ED_DIVISION_NORTHING)->EnableWindow(Copy().ConstantDepth() && !bBranch);
		GetDlgItem(IDC_ED_DIVISION_EASTING)->EnableWindow(Copy().ConstantDepth() && !bBranch);
		GetDlgItem(IDC_LB_SURFACE)->EnableWindow(!Copy().ConstantDepth());

    bool bReadOnly = dynamic_cast<CTetraModel*>(&Copy().Model()) || bBranch;
		((CEdit*)GetDlgItem(IDC_ED_DIVISION_NORTHING))->SetReadOnly(bReadOnly);
		((CEdit*)GetDlgItem(IDC_ED_DIVISION_EASTING))->SetReadOnly(bReadOnly);
		// Bottom and top
		CLengthQuantity qnDepth;

		if(Copy().ConstantDepth())
		{
			dBottom = Copy().Depth().Value(UnitNode().Unit());
			dTop = dBottom;
		}
		else
		{
			assert(Copy().SurfaceSize() == 1);
			dBottom = Copy().Surface(0).Surface().Max().Z();
			dTop = Copy().Surface(0).Surface().Min().Z();
			if(UnitNode().Unit() == CQuantity::FIELD_UNIT)
			{
				dBottom *= FF_FACTOR_LENGTH;
				dTop *= FF_FACTOR_LENGTH;
			}
		}
	//	dBottom = qnDepth.Convert(Copy().DisplayList(0).Max().Z(), UnitNode().Unit(), CQuantity::SI_UNIT);
	//	dTop = qnDepth.Convert(Copy().DisplayList(0).Min().Z(), UnitNode().Unit(), CQuantity::SI_UNIT);
		strUnit = CString(qnDepth.UnitName(UnitNode().Unit()).c_str());
		bSlip = Copy().Slip();
		
		if(Copy().ConstantDepth())
		{
			dDepth  = Copy().Depth().Value(UnitNode().Unit());
			strUnit = CString(Copy().Depth().UnitName(UnitNode().Unit()).c_str());
			nDivisionNorthing = Copy().DivisionNorthing();
			nDivisionEasting = Copy().DivisionEasting();

			nSelect = 0;
		}
		else
		{
			dDepth = 0;
			nSelect = 1;
		}
	}

	CAttriHorizon<C3DHorizon>::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttri3DHorizonDlg)
	DDX_Control(pDX, IDC_LB_SURFACE, m_lbSurface);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_ST_DEPTH_UNIT, strUnit);
	DDX_Text(pDX, IDC_ST_TOP_UNIT, strUnit);
	DDX_Text(pDX, IDC_BOTTOM_UNIT, strUnit);
	DDX_Text(pDX, IDC_ST_BOTTOM, dBottom);
	DDX_Text(pDX, IDC_ST_TOP, dTop);

	if(Copy().ConstantDepth())
	{
		DDX_Text(pDX, IDC_ED_DEPTH, dDepth);
		DDX_Text(pDX, IDC_ED_DIVISION_NORTHING, nDivisionNorthing);
		DDX_Text(pDX, IDC_ED_DIVISION_EASTING, nDivisionEasting);
	}

	DDX_Radio(pDX, IDC_RD_CONST_DEPTH, nSelect);
	if(pDX->m_bSaveAndValidate)
	{
		if(!Copy().ConstantDepth())
		{
			if(m_lbSurface.GetSelectedCount() < 1)
			{
				_m()->msg("You have to select at least one surface");
				pDX->Fail();
			}

			assert(m_lbSurface.GetItemCount() > 0);
			for(int i = 0; i < m_lbSurface.GetItemCount(); i++)
			{
				CSurfaceListObject *pListObject = (CSurfaceListObject*)m_lbSurface.GetItemData(i);
				assert(pListObject);
				
				if(Copy().IsLinkedTo(pListObject->Surface()) && !pListObject->IsSelected())
					Copy().UnLink(pListObject->Surface());

				if(!Copy().IsLinkedTo(pListObject->Surface()) && pListObject->IsSelected())
					Copy().LinkTo(pListObject->Surface());
			}


		}
		else
		{
			if(!Copy().IsDepthValid(dDepth, UnitNode().Unit()))
			{
				_m()->msg("Constant depth value is not valid in the model");
				pDX->Fail();
			}
			Copy().Depth(dDepth, UnitNode().Unit());
			Copy().Division(nDivisionNorthing, nDivisionEasting);
		}

	}


}


BEGIN_MESSAGE_MAP(CAttri3DHorizonDlg, CDialog)
	//{{AFX_MSG_MAP(CAttri3DHorizonDlg)
	ON_BN_CLICKED(IDC_RD_CONST_DEPTH, OnConstantDepth)
	ON_BN_CLICKED(IDC_RD_SURFACE, OnSurface)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttri3DHorizonDlg message handlers

//##ModelId=3BC55D6501DB


BOOL CAttri3DHorizonDlg::OnInitDialog() 
{
	CAttriHorizon<C3DHorizon>::OnInitDialog();
	
	m_lbSurface.InsertColumn(0, _T("Name"), LVCFMT_LEFT, -1, 0);
	OnUpdateLb();
	m_lbSurface.SetColumnWidth(0, LVSCW_AUTOSIZE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttri3DHorizonDlg::OnUpdateLb()
{
	assert(Copy().Model().GraphEntry(MD_BASE_SURFACE));

	CSurfaceEntry *pEntry = (CSurfaceEntry*)(Copy().Model().GraphEntry(MD_BASE_SURFACE));

	CSurfaceEntry::TNodeSet stNode = pEntry->EntryNodes();

	m_lbSurface.DeleteAllItems();

	for(CSurfaceEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		assert((*it)->IsCopied());

		if(Copy().CanConnectItem((*it)->Copy()) || Copy().IsLinkedTo((*it)->Copy()))
		{
			CSurfaceListObject *pObject = new CSurfaceListObject(m_lbSurface, (CSurfaceBase&)((*it)->Copy()), FALSE);
			if(Copy().IsLinkedTo((*it)->Copy()))
				m_lbSurface.SetItemState(pObject->Index(), LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	
	GetDlgItem(IDC_RD_SURFACE)->EnableWindow(m_lbSurface.GetItemCount() > 0 && !(static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch());
}

void CAttri3DHorizonDlg::OnConstantDepth() 
{

	if(!Copy().ConstantDepth())
	{
		// Get boundary ...
		CBoundaryBase* pBoundary = &((CModelBase&)Copy().Model()).Boundary();

		double dMax = pBoundary->Max().Z(); 
		double dMin = pBoundary->Min().Z();

		Copy().Depth((dMax + dMin)/2);
	}

	UpdateData(FALSE);
}

void CAttri3DHorizonDlg::OnSurface() 
{
	assert(m_lbSurface.GetItemCount() > 0);
	
	// Link default to the first surface ...
	if(Copy().ConstantDepth())
	{
		CSurfaceListObject *pSurfaceObject = (CSurfaceListObject*)(m_lbSurface.GetItemData(0));
		assert(pSurfaceObject->Surface().IsCopy());
		Copy().LinkTo(pSurfaceObject->Surface());
	}

	UpdateData(FALSE);
}


		
