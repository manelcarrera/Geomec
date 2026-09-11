// MeshRefinementDraw.h: interface for the CMeshRefinementDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHREFINEMENTDRAW_H__CD3493DD_4925_4ADF_B7BF_DBAACA8BB23E__INCLUDED_)
#define AFX_MESHREFINEMENTDRAW_H__CD3493DD_4925_4ADF_B7BF_DBAACA8BB23E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC159004E
class CMeshRefinementDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC1590050
	CMeshRefinementDraw();
	//##ModelId=3C0DC1590051
	virtual ~CMeshRefinementDraw();

protected:
	DECLARE_DYNCREATE(CMeshRefinementDraw)

	//{{AFX_VIRTUAL(CMeshRefinementDraw)
	protected:
	//##ModelId=3C0DC1590053
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CMeshRefinementDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CMeshRefinementDraw)
	//##ModelId=3DDA0C510070
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_MESHREFINEMENTDRAW_H__CD3493DD_4925_4ADF_B7BF_DBAACA8BB23E__INCLUDED_)
