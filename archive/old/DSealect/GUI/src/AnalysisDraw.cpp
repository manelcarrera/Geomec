// AnalysisDraw.cpp: implementation of the CAnalysisDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "CDataModel.h"
#include "AnalysisDraw.h"


#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAnalysisDraw, CDSealectDrawView)

//##ModelId=3C0DC16100EA
CAnalysisDraw::CAnalysisDraw()
{
}

//##ModelId=3C0DC16100EB
CAnalysisDraw::~CAnalysisDraw()
{

}

BEGIN_MESSAGE_MAP(CAnalysisDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CAnalysisDraw)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3C0DC16100ED
void CAnalysisDraw::OnDraw(CDC* pDC) 
{
  CBatchTableRow* pCurrentBatchTableRow = GetModel()->BatchTableSelection().SelectedRow();
	if(!pCurrentBatchTableRow)
		return;

	if(pCurrentBatchTableRow->SaveDiagnostic() && !pCurrentBatchTableRow->GetDiagnosticFileName().IsEmpty())
	{
		pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT))); 
		//pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(SYSTEM_FONT))); 
		CFile file;
		if(!file.Open(pCurrentBatchTableRow->GetDiagnosticFileName(),CFile::modeRead ))
			return;

		CString sBuff;
		DWORD dwLength = file.GetLength();
		LPTSTR buff = sBuff.GetBufferSetLength(dwLength);
		file.Read(buff,dwLength);
		file.Close();
		CRect rc(0,0,1,1);
		pDC->DrawText(sBuff,&rc,DT_CALCRECT | DT_EXTERNALLEADING);
		pDC->DrawText(sBuff,&rc,DT_LEFT | DT_EXTERNALLEADING     );
		
		
		//pDC->SelectObject(pFont);
	}
}

//##ModelId=3DDA0C600060
void CAnalysisDraw::SetSize() 
{
  if(GetModel())
  {
    CBatchTableRow* pCurrentBatchTableRow = GetModel()->BatchTableSelection().SelectedRow();
	  if(!pCurrentBatchTableRow)
	  {
		  SetScrollSizes(MM_TEXT,CSize(10,10));
		  return;
	  }
  	
	  if(pCurrentBatchTableRow->SaveDiagnostic() && !pCurrentBatchTableRow->GetDiagnosticFileName().IsEmpty())
	  {
		  CFile file;
		  if(!file.Open(pCurrentBatchTableRow->GetDiagnosticFileName(),CFile::modeRead ))
			  return;

		  GetDC()->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT))); 
		  CString sBuff;
		  DWORD dwLength = file.GetLength();
		  LPTSTR buff = sBuff.GetBufferSetLength(dwLength);
		  file.Read(buff,dwLength);
		  file.Close();
		  CRect rc(0,0,1,1);
		  //CRect rc2(0,0,1000,100000);
		  DrawText(GetDC()->m_hDC,buff,dwLength,&rc,DT_CALCRECT | DT_EXTERNALLEADING);
		  //TRACE("\nrc.Height = %d\n", rc.Height());
		  //SetScrollSizes(MM_TEXT,CSize(100,100));
		  SetScrollSizes(MM_TEXT,CSize(rc.Width(),rc.Height()));
		  //TRACE("\nGetScrollPos() = %d\n", GetScrollPos(SB_VERT));
  		
	  }
	  else
	  {
		  SetScrollSizes(MM_TEXT,CSize(10,10));
	  }
  }
}

//##ModelId=3DDA0C600082
void CAnalysisDraw::OnSize(UINT nType, int cx, int cy) 
{
	SetSize();
}


//##ModelId=3DDA0C6000A0
void CAnalysisDraw::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	/*
	if(nSBCode = SB_THUMBPOSITION )
    {
        //const MSG* pMsg = GetCurrentMessage();
        //nPos = HIWORD(pMsg->wParam);
		//SetScrollPos(SB_VERT,nPos);
    }
	*/
	// TODO: Add your message handler code here and/or call default
	CDSealectDrawView::OnVScroll(nSBCode, nPos, pScrollBar);
}
