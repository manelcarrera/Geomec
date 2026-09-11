#ifndef __CHECKFRM_H__
#define __CHECKFRM_H__

//Enable or Disable all windows in the area(rectangle) of a frame

BOOL IsRectContainedInRect(CRect &rcChild, CRect &rcMother);

class CCheckFrame
{

private:

protected:

public:
	virtual void Enable(BOOL bEnable);
	virtual void Visible(BOOL bShow,BOOL bDoFrame=FALSE);

	void Set(CWnd *pParentWnd,unsigned int nFrmCtl);


	CWnd* m_pFrame; 
	CWnd* m_pDialog;
	CRect m_rFrm;
	CPtrArray m_adwWndHandles;


protected:
};

#endif