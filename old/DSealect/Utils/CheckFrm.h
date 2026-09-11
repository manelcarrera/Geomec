#include "stdafx.h"

#ifndef __CHECKFRM_H__
#define __CHECKFRM_H__

//Enable or Disable all windows in the area(rectangle) of a frame

BOOL IsRectContainedInRect(CRect &rcChild, CRect &rcMother);

class CCheckFrame : public CObject
{

private:

protected:

public:
	virtual void Enable(BOOL bEnable);
	void Set(CWnd *pParentWnd,UINT nFrmCtl);


	CWnd* m_pFrame; 
	CWnd* m_pDialog;
	CRect m_rFrm;
	CDWordArray m_adwWndHandles;


protected:
};

#endif