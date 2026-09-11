// GraphMainFrm.h: interface for the CGraphMainFrm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHMAINFRM_H__8944D9F4_47D0_45DF_B04D_7DDFCE3D3749__INCLUDED_)
#define AFX_GRAPHMAINFRM_H__8944D9F4_47D0_45DF_B04D_7DDFCE3D3749__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stack>
#include "GraphPtr.h"

//##ModelId=3B653D0201B7
class CProgressCtrl;
class CGraphMainFrm : public CFrameWnd
{
	class CProgressStackItem
	{
		int m_pos;
		int m_maxpos;
		int m_skippos;
		CString m_caption;

	public:
		CProgressStackItem(const CString &caption, int maxpos);
		~CProgressStackItem();
		void Step();
		int Pos() const;
		int MaxPos() const;
		int SkipPos() const;

		const CString &Caption() const;
	};

	CGraphPtr<CGraphNode> m_clip_board;
	CProgressCtrl*	m_pProgress;
	CString			m_sStatusText;
	std::stack<CProgressStackItem> m_ProgressStack;

	void CreateProgress(const CProgressStackItem &item);
	void DestroyProgress();

protected: // create from serialization only
	//##ModelId=3B653D020272
	CGraphMainFrm();

	virtual CStatusBar& StatusBar() = 0;

// Implementation
public:
	void Copy(CGraphNode& node);
	CGraphNode* Paste();
	const CGraphNode* Paste() const;

	// Progress in the status bar
	void StartProgress(const CString& sCaption, int nSteps);
	void ProgressStep();
	void EndProgress();
};

#endif // !defined(AFX_GRAPHMAINFRM_H__8944D9F4_47D0_45DF_B04D_7DDFCE3D3749__INCLUDED_)
