#if !defined(AFX_ATTRIBOUNDARYDLG_H__E1BD400E_064E_483D_AA4F_D88D53FF1731__INCLUDED_)
#define AFX_ATTRIBOUNDARYDLG_H__E1BD400E_064E_483D_AA4F_D88D53FF1731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AttributesTemplate.h"
#include "BaseEntryTypes.h"
#include "AnalysisPoint.h"
#include "CrossSection.h"
#include "BranchState.h"
#include "BoundarySpecific.h"
#include "GlobalMessage.h"

// The boundary dialog is a template for the basic functionality of a boundary.

template<class T>
class CAttriBoundaryDlg : public CGeomecAttriTemp<T>
{
public:
	// Construction
	CAttriBoundaryDlg(T& boundary, unsigned int nIDTemplate, CWnd* pParent = NULL);   // standard constructor

	// Windows messages
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnWndMsg( unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );

	// Message maps called by OnWndMessage
protected:
	void OnBestFit();
	void OnUserDefined();
  void OnRotated();
	void OnChangeColor();
	void OnPaint();
	void OnKillFocusCoordinate();
	virtual void OnOK();
};

// Implementation of template functions
template<class T>
CAttriBoundaryDlg<T>::CAttriBoundaryDlg(T& boundary, unsigned int nIDTemplate, CWnd* pParent)
: CGeomecAttriTemp<T>(nIDTemplate, boundary, pParent)
{
}

template<class T>
void CAttriBoundaryDlg<T>::DoDataExchange(CDataExchange* pDX)
{
	CGeomecAttriTemp<T>::DoDataExchange(pDX);

	int nType;
	geo::CPoint ptMin(0,0,0), ptMax(0, 0, 0), ptMid(0, 0, 0);
  double dAzimuth;
  double dLength;
  double dWidth;
  double dMinDepth(Copy().Min().Z());
  double dMaxDepth(Copy().Max().Z());
	CLengthQuantity ql;
  CAngleQuantity qa;

	bool bBranch = (static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch();

	if(!pDX->m_bSaveAndValidate)
	{
		ptMin = Copy().MinPoint();
		ptMax = Copy().MaxPoint();

    ptMid    = Copy().Mid();
    dAzimuth = Copy().Azimuth();
    dLength  = Copy().Length();
    dWidth   = Copy().Width();

		// Disable "Undefined"
		GetDlgItem(IDC_RD_UNDEFINED)->EnableWindow(FALSE);

		CBoundaryBase::TMinMax minmax = Copy().BestFit();
		GetDlgItem(IDC_RD_BEST_FIT)->EnableWindow(!minmax.first.Empty() && !minmax.second.Empty() && !bBranch);
		GetDlgItem(IDC_RD_USER_DEFINED)->EnableWindow(!bBranch);
    EnableBoundarySpecificWindow <T> (this, IDC_RD_ROTATED, !bBranch && Copy().CanRotate());

		// Enable Boundaries ...
		((CEdit*)GetDlgItem(IDC_ED_MIN_EAST))->SetReadOnly(Copy().State() != CBoundaryBase::USER_DEFINED || bBranch);
		((CEdit*)GetDlgItem(IDC_ED_MIN_NORTH))->SetReadOnly(Copy().State() != CBoundaryBase::USER_DEFINED || bBranch);
		((CEdit*)GetDlgItem(IDC_ED_MAX_EAST))->SetReadOnly(Copy().State() != CBoundaryBase::USER_DEFINED || bBranch);
		((CEdit*)GetDlgItem(IDC_ED_MAX_NORTH))->SetReadOnly(Copy().State() != CBoundaryBase::USER_DEFINED || bBranch);

    // ... and rotation info
    SetBoundarySpecificReadOnly <T> (this, IDC_ED_ROT_N  , Copy().State() != CBoundaryBase::ROTATED || bBranch);
    SetBoundarySpecificReadOnly <T> (this, IDC_ED_ROT_E  , Copy().State() != CBoundaryBase::ROTATED || bBranch);
    SetBoundarySpecificReadOnly <T> (this, IDC_ED_ROT_AZI, Copy().State() != CBoundaryBase::ROTATED || bBranch);
    SetBoundarySpecificReadOnly <T> (this, IDC_ED_ROT_L  , Copy().State() != CBoundaryBase::ROTATED || bBranch);
    SetBoundarySpecificReadOnly <T> (this, IDC_ED_ROT_W  , Copy().State() != CBoundaryBase::ROTATED || bBranch);

		switch(Copy().State())
		{
    case CBoundaryBase::ROTATED:
      nType = 3;
      break;
		case CBoundaryBase::USER_DEFINED:
			nType = 2;
			break;
		case CBoundaryBase::BEST_FIT:
			nType = 1;
			break;
		case CBoundaryBase::DEFAULT_DEFINED:
			nType = 0;
			break;
		default:
			assert(FALSE);
		}

    SetBoundarySpecificWindowText <T> (this, IDC_UN_ROT_AZI, qa.UnitName(UnitNode().Unit()).c_str());
    SetBoundarySpecificWindowText <T> (this, IDC_UN_ROT_L, ql.UnitName(UnitNode().Unit()).c_str());
    SetBoundarySpecificWindowText <T> (this, IDC_UN_ROT_W, ql.UnitName(UnitNode().Unit()).c_str());
  }

	DDX_Radio(pDX, IDC_RD_UNDEFINED, nType);
	DDX_Point(pDX, IDC_ED_MIN_EAST, IDC_ED_MIN_NORTH, IDC_ED_MIN_Z, ptMin, IDC_ST_UNIT1, IDC_ST_UNIT2, IDC_ST_UNIT3);
	DDX_Point(pDX, IDC_ED_MAX_EAST, IDC_ED_MAX_NORTH, IDC_ED_MAX_Z, ptMax, IDC_ST_UNIT4, IDC_ST_UNIT5, IDC_ST_UNIT6);
  BoundarySpecificDDX_Point <T> (this, pDX, IDC_ED_ROT_N, IDC_ED_ROT_E, 0, ptMid, IDC_UN_ROT_N, IDC_UN_ROT_E, 0);
  BoundarySpecificDDX_Text <T> (pDX, IDC_ED_ROT_AZI, dAzimuth);
  BoundarySpecificDDX_Text <T> (pDX, IDC_ED_ROT_L, dLength);
  BoundarySpecificDDX_Text <T> (pDX, IDC_ED_ROT_W, dWidth);

	if(pDX->m_bSaveAndValidate)
	{
		if(Copy().State() == CBoundaryBase::USER_DEFINED)
		{
			// Validate points
			if(ptMin.X() >= ptMax.X())
			{
				_m()->msg("Maximum of East is smaller than or equal to minimum");
				pDX->Fail();
			}

			if(ptMin.Y() >= ptMax.Y())
			{
				_m()->msg("Maximum of North is smaller than or equal to minimum");
				pDX->Fail();
			}
			
			CBoundaryBase::TMinMax min_max = Copy().SnapToGrid(CBoundaryBase::TMinMax(ptMin, ptMax));
			Copy().Set(min_max.first, min_max.second);
		}
    else if(Copy().State() == CBoundaryBase::ROTATED)
    {
      // validate
      if(dAzimuth < 0 || dAzimuth >= 180)
      {
        _m()->msg("The azimuth angle must be at least zero and less than 180");
        pDX->Fail();
      }

      if(dLength <= 0)
      {
        _m()->msg("The length must be greater than 0");
        pDX->Fail();
      }

      if(dWidth <= 0)
      {
        _m()->msg("The width must be greater than 0");
        pDX->Fail();
      }

      CBoundaryBase::TRotated rot = Copy().SnapToGrid(CBoundaryBase::TRotated(ptMid, dAzimuth, dLength, dWidth, dMinDepth, dMaxDepth));
      Copy().Set(rot.ptMid, rot.dAzimuth, rot.dLength, rot.dWidth, rot.dMinDepth, rot.dMaxDepth);
    }
	}		
}

template<class T>
BOOL CAttriBoundaryDlg<T>::OnCommand( WPARAM wParam, LPARAM lParam )
{
	DWORD nNotification = HIWORD(wParam);
	DWORD nControlID = LOWORD(wParam);
	unsigned int nID = wParam;

	if(nNotification == BN_CLICKED)
	{
		switch(nControlID)
		{
		case IDC_RD_BEST_FIT:
			OnBestFit();
			return TRUE;
		case IDC_RD_USER_DEFINED:
			OnUserDefined();
			return TRUE;
    case IDC_RD_ROTATED:
      OnRotated();
      return TRUE;
		case IDC_ST_COLOR:
			OnChangeColor();
			return TRUE;
		}
	}

	if(nNotification == EN_KILLFOCUS)
	{
		if((nControlID == IDC_ED_MIN_EAST) ||
		   (nControlID == IDC_ED_MAX_EAST) ||
		   (nControlID == IDC_ED_MIN_NORTH) ||
		   (nControlID == IDC_ED_MAX_NORTH))
		{
			OnKillFocusCoordinate();
			return TRUE;
		}
	}

	return CAttributesTemplate<T>::OnCommand(wParam, lParam );
}

template<class T>
BOOL CAttriBoundaryDlg<T>::OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(message == WM_PAINT)
		OnPaint();

	return CAttributesTemplate<T>::OnWndMsg(message, wParam, lParam, pResult);
}

template<class T>
void CAttriBoundaryDlg<T>::OnBestFit() 
{
	if(Copy().State() != CBoundaryBase::BEST_FIT)
	{
		Copy().State(CBoundaryBase::BEST_FIT);
		UpdateData(FALSE);
	}
}

template<class T>
void CAttriBoundaryDlg<T>::OnUserDefined() 
{
	if(Copy().State() != CBoundaryBase::USER_DEFINED)
	{
		Copy().State(CBoundaryBase::USER_DEFINED);
		UpdateData(FALSE);
	}
}

template<class T>
void CAttriBoundaryDlg<T>::OnRotated()
{
  if(Copy().State() != CBoundaryBase::ROTATED)
  {
    Copy().State(CBoundaryBase::ROTATED);
    UpdateData(FALSE);
  }
}

template<class T>
void CAttriBoundaryDlg<T>::OnChangeColor() 
{
	Copy().Color(SelectColor(Copy().Color()));
	Invalidate();
}

template<class T>
void CAttriBoundaryDlg<T>::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

template<class T>
void CAttriBoundaryDlg<T>::OnKillFocusCoordinate() 
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

template <class T>
void CAttriBoundaryDlg<T>::OnOK()
{
  UpdateData(TRUE);
	std::vector<CGraphNode*> vc_nodes = Copy().ItemsOutsideBoundary(Copy().RectangleXY());

	if(vc_nodes.size() > 0)
	{
		CString message;
		message.Format("One or more items like vertical wells, analysis points and/or cross sections\nare outside the new boundary and will be deleted.\nContinue?");
		if(_m()->msg((LPCSTR)(message), MB_YESNO) == IDYES)
		{
			for(size_t i = 0; i < vc_nodes.size(); i++)
			{
				IModelObject* pModelObject = dynamic_cast<IModelObject*>(vc_nodes[i]);
				if(pModelObject)
					pModelObject->destroy();
				else
					delete vc_nodes[i];
			}
			CGeomecAttriTemp<T>::OnOK();			
		}

	}
	else
		CGeomecAttriTemp<T>::OnOK();
}

#endif // !defined(AFX_ATTRIBOUNDARYDLG_H__E1BD400E_064E_483D_AA4F_D88D53FF1731__INCLUDED_)
