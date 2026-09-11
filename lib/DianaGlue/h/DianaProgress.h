// DianaProgress.h: interface for the CDianaProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIANAPROGRESS_H__FE9B9B2C_31BF_4560_9C0B_43C9578FE89F__INCLUDED_)
#define AFX_DIANAPROGRESS_H__FE9B9B2C_31BF_4560_9C0B_43C9578FE89F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProgressDlg_MFC;

class CDianaProgress : public dia::IProgressBase  
{
	int m_nSteps;
	CProgressDlg_MFC *m_pDlg;
	int m_nStep;

public:
	CDianaProgress();
	virtual ~CDianaProgress();

	virtual void Begin(int nSteps);
	virtual void End();
	virtual void Step(bool &bContinue);
	virtual void Text(const std::string &strText);
};

#endif // !defined(AFX_DIANAPROGRESS_H__FE9B9B2C_31BF_4560_9C0B_43C9578FE89F__INCLUDED_)
