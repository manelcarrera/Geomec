// attrivalueset.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attrinodalvalueset.h"
#include "NodalValueSet.h"
#include "pointset.h"
#include "FieldFactor.h"
#include "GlobalMessage.h"
#include "resourceIDS.h"
#include "GlobalUnitNode.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriNodalValueSet dialog

CAttriNodalValueSet::CAttriNodalValueSet(CNodalValueSet &value_set, CWnd* pParent)   
: CDialog(CAttriNodalValueSet::IDD, pParent), m_value_set(value_set)
{
	//{{AFX_DATA_INIT(CAttriNodalValueSet)
	//}}AFX_DATA_INIT
}

void CAttriNodalValueSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CString sName;
	if(!pDX->m_bSaveAndValidate)
	{
		sName = m_value_set.Name().toStdString().c_str();
	}


	//{{AFX_DATA_MAP(CAttriNodalValueSet)
	DDX_Control(pDX, IDC_LC_VALUE, m_lcValue);
	DDX_Text(pDX, IDC_EC_NAME, sName);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		if(sName == "")
		{
			_m()->msg(IDS_NAME_FIELD_EMPTY);
			pDX->Fail();
			return ;
		}

		m_value_set.Name((LPCSTR) sName);
	}
}


BEGIN_MESSAGE_MAP(CAttriNodalValueSet, CDialog)
	//{{AFX_MSG_MAP(CAttriNodalValueSet)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LC_VALUE, OnGetDisplayInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriNodalValueSet message handlers

BOOL CAttriNodalValueSet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWaitCursor s;

	m_lcValue.EnableWindow();
	CRect rect;
	m_lcValue.GetWindowRect(&rect);
	int nColumnWidth;
	if(m_value_set.PointSet().Dimension() == CPointSet::DIM_3D)
		nColumnWidth = rect.Width() / 4;
	else
		nColumnWidth = rect.Width() / 3;

	CGlobalUnitNode globalUnitNode;

	if(globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT)
	{
		m_lcValue.InsertColumn(0, _T("Northing(m)"), LVCFMT_LEFT, nColumnWidth);
		m_lcValue.InsertColumn(1, _T("Easting(m)"), LVCFMT_LEFT, nColumnWidth);
	}
	else
	{
		m_lcValue.InsertColumn(0, _T("Northing(ft)"), LVCFMT_LEFT, nColumnWidth);
		m_lcValue.InsertColumn(1, _T("Easting(ft)"), LVCFMT_LEFT, nColumnWidth);
	}

	if(m_value_set.PointSet().Dimension() == CPointSet::DIM_3D)
	{
		if(globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT)
			m_lcValue.InsertColumn(2, _T("Depth(m)"), LVCFMT_LEFT, nColumnWidth);
		else
			m_lcValue.InsertColumn(2, _T("Depth(ft)"), LVCFMT_LEFT, nColumnWidth);

		m_lcValue.InsertColumn(3, _T("Value"), LVCFMT_LEFT, nColumnWidth);
	}
	else
		m_lcValue.InsertColumn(2, _T("Value"), LVCFMT_LEFT, nColumnWidth);


	// Fill value box ...
	m_lcValue.SetItemCount(m_value_set.ValueSize());
	for(int i = 0; i < m_value_set.ValueSize(); i++)
		m_lcValue.InsertItem(i, LPSTR_TEXTCALLBACK);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriNodalValueSet::OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	LV_ITEM &item = pDispInfo->item;


	if(item.mask |= LVIF_TEXT)
	{
		CString sValue;
		CGlobalUnitNode globalUnitNode;
		switch(item.iSubItem)
		{
		case 0:
			if(globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT)
				sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).X());
			else
				sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).X()* FF_FACTOR_LENGTH);
			break;
		case 1:
			if(globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT)
				sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).Y());
			else
				sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).Y()* FF_FACTOR_LENGTH);
			break;
		case 2:
			if(m_value_set.PointSet().Dimension() == CPointSet::DIM_2D)
				sValue.Format(_T("%6.2f"), m_value_set.Value(item.iItem).Value());
			else
			{
				if(globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT)
					sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).Z());
				else
					sValue.Format(_T("%6.2f"), m_value_set.PointSet().PointAt(item.iItem).Z()* FF_FACTOR_LENGTH);
			}
			break;
		case 3:
			assert(m_value_set.PointSet().Dimension() == CPointSet::DIM_3D);
			sValue.Format(_T("%f6.2"), m_value_set.Value(item.iItem).Value());
			break;
		default:
			assert(FALSE);
			break;
		}

		if(strcmp(item.pszText, "")  != 0)
			free(item.pszText);
		item.pszText = _tcsdup(sValue);
	}
	
	*pResult = 0;
}
