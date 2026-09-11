#include "stdafx.h"
#include "geomec.h"
#include "homoboxattridlg.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif


const int MESH_DIVISION_OFFSET = 4;


CHomoBoxAttriDlg::CHomoBoxAttriDlg(const CFemAppModel &model, CHomogenizationBox &homo_box, CWnd* pParent /*=NULL*/)
:CAttributesTemplate<CHomogenizationBox>(CHomoBoxAttriDlg::IDD, homo_box, pParent)
{
	//{{AFX_DATA_INIT(CHomoBoxAttriDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_unit = GetGeomecDoc()->UnitNode().Unit();
}


void CHomoBoxAttriDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHomoBoxAttriDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	double center_northing;
	double center_easting;
	double center_depth;

	double edge_length_northing;
	double edge_length_easting;
	double edge_length_depth;

	double azimuth;

	int division_northing;
	int division_easting;
	int division_depth;

	CLengthQuantity qLength;
	CAngleQuantity qAngle;

	CString strUnitLength;
	CString strUnitAngle;
	CString sName;
	strUnitLength = qLength.UnitName(m_unit).c_str();
	strUnitAngle = qAngle.UnitName(m_unit).c_str();
	
	CComboBox *pComboN = (CComboBox*)(GetDlgItem(IDC_COM_DIVISION_N));
	CComboBox *pComboE = (CComboBox*)(GetDlgItem(IDC_COM_DIVISION_E));
	CComboBox *pComboD = (CComboBox*)(GetDlgItem(IDC_COM_DIVISION_D));

	if(!pDX->m_bSaveAndValidate)
	{
		sName = Copy().Name().toStdString().c_str();

		edge_length_northing = Copy().Mesh().XSpace() * Copy().Mesh().LengthDivision();
		edge_length_easting = Copy().Mesh().YSpace() * Copy().Mesh().WidthDivision();
		edge_length_depth = Copy().Mesh().ZSpace() * Copy().Mesh().HeightDivision();

		center_northing = Copy().Mesh().XBase() + (edge_length_northing / 2.0);
		center_easting = Copy().Mesh().YBase() + (edge_length_easting / 2.0);
		center_depth = Copy().Mesh().ZBase() + (edge_length_depth / 2.0);

		azimuth = Copy().Azimuth();

		pComboN->SetCurSel(Copy().Mesh().LengthDivision() - MESH_DIVISION_OFFSET);
		pComboE->SetCurSel(Copy().Mesh().WidthDivision() - MESH_DIVISION_OFFSET);
		pComboD->SetCurSel(Copy().Mesh().HeightDivision() - MESH_DIVISION_OFFSET);
	}

	edge_length_northing = qLength.Convert(edge_length_northing, m_unit, IQuantityDouble::SI_UNIT);
	edge_length_easting = qLength.Convert(edge_length_easting, m_unit, IQuantityDouble::SI_UNIT);
	edge_length_depth = qLength.Convert(edge_length_depth, m_unit, IQuantityDouble::SI_UNIT);

	center_northing = qLength.Convert(center_northing, m_unit, IQuantityDouble::SI_UNIT);
	center_easting = qLength.Convert(center_easting, m_unit, IQuantityDouble::SI_UNIT);
	center_depth = qLength.Convert(center_depth, m_unit, IQuantityDouble::SI_UNIT);

	azimuth = qAngle.Convert(azimuth, m_unit, IQuantityDouble::SI_UNIT);
	
	DDX_Text(pDX, IDC_EDIT_BOXNAME, sName);

	DDX_Text(pDX, IDC_NORTH_CENTER, center_northing);
	DDX_Text(pDX, IDC_EAST_CENTER, center_easting);
	DDX_Text(pDX, IDC_DEPTH_CENTER, center_depth);
	DDX_Text(pDX, IDC_N_UNIT, strUnitLength);
	DDX_Text(pDX, IDC_E_UNIT, strUnitLength);
	DDX_Text(pDX, IDC_D_UNIT, strUnitLength);

	DDX_Text(pDX, IDC_NORTH_LENGTH, edge_length_northing);
	DDX_Text(pDX, IDC_EAST_LENGTH, edge_length_easting);
	DDX_Text(pDX, IDC_DEPTH_LENGTH, edge_length_depth);
	DDX_Text(pDX, IDC_SIZEN_UNIT, strUnitLength);
	DDX_Text(pDX, IDC_SIZEE_UNIT, strUnitLength);
	DDX_Text(pDX, IDC_SIZED_UNIT, strUnitLength);

	DDX_Text(pDX, IDC_AZIMUTH, azimuth);
	DDX_Text(pDX, IDC_AZIMUTH_UNIT, strUnitAngle);

	if(pDX->m_bSaveAndValidate)
	{
		center_northing = qLength.Convert(center_northing, IQuantityDouble::SI_UNIT, m_unit);
		center_easting = qLength.Convert(center_easting, IQuantityDouble::SI_UNIT, m_unit);
		center_depth = qLength.Convert(center_depth, IQuantityDouble::SI_UNIT, m_unit);

		// perform validity test in SI units
		if(edge_length_northing < EPS || edge_length_easting < EPS || edge_length_depth < EPS)
		{
			_m()->msg("One or more edge lengths too small, please correct your input.", MB_OK|MB_ICONEXCLAMATION);
			pDX->Fail();
		}

		edge_length_northing = qLength.Convert(edge_length_northing, IQuantityDouble::SI_UNIT, m_unit);
		edge_length_easting = qLength.Convert(edge_length_easting, IQuantityDouble::SI_UNIT, m_unit);
		edge_length_depth = qLength.Convert(edge_length_depth, IQuantityDouble::SI_UNIT, m_unit);

		azimuth = qAngle.Convert(azimuth, IQuantityDouble::SI_UNIT, m_unit);

		division_northing = pComboN->GetCurSel() + MESH_DIVISION_OFFSET;
		division_easting = pComboE->GetCurSel() + MESH_DIVISION_OFFSET;
		division_depth = pComboD->GetCurSel() + MESH_DIVISION_OFFSET;

		Copy().SetMesh(geo::CPoint(center_northing, center_easting, center_depth),
					   edge_length_northing, edge_length_easting, edge_length_depth,
					   division_northing, division_easting, division_depth, azimuth);

		Copy().Name((LPCSTR) sName);
	}
}


BEGIN_MESSAGE_MAP(CHomoBoxAttriDlg, CDialog)
	//{{AFX_MSG_MAP(CHomoBoxAttriDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHomoBoxAttriDlg::configureDivisionCombo(CComboBox& combo, int division)
{
	CString str;
	int i;
	for(i = 0; i < 17; ++i)
	{
		str.Format(_T("%d"), i + MESH_DIVISION_OFFSET);
		combo.InsertString(i, str);
	}

	assert(division < 21);

	if(division == 0)
		combo.SetCurSel(0);
	else
		combo.SetCurSel(division - MESH_DIVISION_OFFSET);
}

BOOL CHomoBoxAttriDlg::OnInitDialog()
{
	CAttributesTemplate<CHomogenizationBox>::OnInitDialog();

	configureDivisionCombo(*(CComboBox*)(GetDlgItem(IDC_COM_DIVISION_N)), Copy().Mesh().LengthDivision());
	configureDivisionCombo(*(CComboBox*)(GetDlgItem(IDC_COM_DIVISION_E)), Copy().Mesh().WidthDivision());
	configureDivisionCombo(*(CComboBox*)(GetDlgItem(IDC_COM_DIVISION_D)), Copy().Mesh().HeightDivision());

	return TRUE;
}

