// ProgressCallBack.h: interface for the CProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSCALLBACK_H__4C38D331_E2BA_406C_BF10_D873FA268190__INCLUDED_)
#define AFX_PROGRESSCALLBACK_H__4C38D331_E2BA_406C_BF10_D873FA268190__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProgressCallBack  
{
public:
	virtual void OnUpdateProgress(int iPercentage) const { }; //called by the process
	virtual void OnFinished(BOOL bCancelled) const { }        //called by the process if finished
	virtual BOOL CancelProcess() const {return FALSE;}        //asked by the process

	CProgressCallBack();
	virtual ~CProgressCallBack();
};
#endif // !defined(AFX_PROGRESSCALLBACK_H__4C38D331_E2BA_406C_BF10_D873FA268190__INCLUDED_)
