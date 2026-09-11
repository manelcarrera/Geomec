#if !defined(AFX_SELSURFDLG_H__90F726F1_FEAE_49A4_AB4D_7F0652EAE488__INCLUDED_)
#define AFX_SELSURFDLG_H__90F726F1_FEAE_49A4_AB4D_7F0652EAE488__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelSurfDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelSurfDlg dialog

#include <map>
#include "NamedSurfaceMap.h"
#include "TriaIntersecBox.h"
#include <afxtempl.h>


namespace geo
{
	class ISurface;
};


typedef std::vector <STNamedSurface> CNamedSurfaces;



// Instantiate classes vector<int> and vector<char>
// This does not create an object. It only forces the generation of all
// of the members of classes vector<int> and vector<char>. It exports
// them from the DLL and imports them into the .exe file.
//#ifdef EXP_CUT_SURFACE

//#endif


class SelSurfFrame;
class CGeoObjectHandlerBase;
class SelSurfFormView;

class DECLSPECIFIER SelSurfDlg : public CDialog
{

// Construction
public:


	friend class SelSurfFormView;

	SelSurfDlg(CWnd* pParent = NULL, const char *caption = 0);   // standard constructor
	virtual ~SelSurfDlg();
// Dialog Data
	//{{AFX_DATA(SelSurfDlg)
	//enum { IDD = IDD_DIALOG_SELECT_SURFACE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	//get selected surfaces
	//void GetSelecetedSurfaces(int index,geo::ISurface*& surf,std::string& name ) const;
	//TNamedSurfMap GetSelecetedSurfacesMap() const;
	//TNamedSurfMap GetUnSelecetedSurfacesMap() const;

	int SelectedSurfacesCount() const;
	const STNamedSurface& GetSelectedSurfaces(int index ) const;

	void InsertSurface(const geo::ISurface* surf,const char* name);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelSurfDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelSurfDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(unsigned int nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual void OnOK( );
	virtual void OnCancel( );

  const char *m_Caption;
	SelSurfFrame* m_pFrameWnd;
	CNamedSurfaces* m_surfaces;
	SelSurfFormView* m_Form;

	CNamedSurfaces* slected_surfaces;
	CNamedSurfaces* not_slected_surfaces;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELSURFDLG_H__90F726F1_FEAE_49A4_AB4D_7F0652EAE488__INCLUDED_)
