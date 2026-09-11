// ModelView.cpp : implementation file
//


#include "stdafx.h"
#include "FemAppOpenGLView.h"
#include "FemAppDoc.h"
#include "GraphMainFrm.h"
#include "OpenGLScene.h"
#include "OpenGLSceneBase.h"
#include "ISceneWrapper.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif



/////////////////////////////////////////////////////////////////////////////
// CFemAppOpenGLView

IMPLEMENT_DYNCREATE(CFemAppOpenGLView, COpenGLView)

CFemAppOpenGLView::CFemAppOpenGLView()
: m_bStep(FALSE)
{
}

CFemAppOpenGLView::~CFemAppOpenGLView()
{
	
}

void CFemAppOpenGLView::OnDestroy()
{
	COpenGLView::OnDestroy();
}


void CFemAppOpenGLView::OnStartDraw(int nSteps) const
{
	if(const_cast<CFemAppOpenGLView*>(this)->CurrentScene())
	{
		if((nSteps > 200) && (const_cast<CFemAppOpenGLView*>(this)->CurrentScene()->Dimension() == DIM_3D))
		{
			CGraphMainFrm* pMain = (CGraphMainFrm*)FemAppGetMainWnd();
			pMain->StartProgress("Drawing ...", nSteps);
			m_bStep = true;
			return;
		}
	}
	
	m_bStep = false;
}

void CFemAppOpenGLView::OnDrawStep() const
{
	if(m_bStep)
	{
		CGraphMainFrm* pMain = (CGraphMainFrm*)FemAppGetMainWnd();
		pMain->ProgressStep();
	}
}

void CFemAppOpenGLView::OnEndDraw() const
{
	if(m_bStep)
	{
		CGraphMainFrm* pMain = (CGraphMainFrm*)FemAppGetMainWnd();
		pMain->EndProgress();
	}
}



BEGIN_MESSAGE_MAP(CFemAppOpenGLView, COpenGLView)
	//{{AFX_MSG_MAP(CFemAppOpenGLView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFemAppOpenGLView drawing



/////////////////////////////////////////////////////////////////////////////
// CFemAppOpenGLView diagnostics

#ifdef _DEBUG

void CFemAppOpenGLView::AssertValid() const
{
	CView::AssertValid();
}


void CFemAppOpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFemAppOpenGLView message handlers

void CFemAppOpenGLView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CMouseListener *pMouseListener = 0;

	switch(lHint)
	{
	case NEW_CURRENT_SCENE:
		
		pMouseListener = MouseListener();
		if(pMouseListener)
		{
			pMouseListener->OK();
		}

	  CurrentScene(pHint ? reinterpret_cast<ISceneWrapper *>(pHint)->OpenGLScene() : 0);

		Invalidate();
		break;
	default:
		break;
	}
}

void CFemAppOpenGLView::CurrentScene(COpenGLSceneBase *pCurrentScene)
{
	assert(ChildSize() < 2);
	if(ChildSize() == 1)
		Child(0).ReParent(0);
	assert(ChildSize() == 0);
	if(pCurrentScene)
		pCurrentScene->ReParent(this);
	assert(CurrentScene() == pCurrentScene);
}

COpenGLSceneBase *CFemAppOpenGLView::CurrentScene()
{
	assert(ChildSize() < 2);
	if(ChildSize() == 1)
		return (COpenGLSceneBase*)&Child(0);
	return 0;
}

void CFemAppOpenGLView::OnEditDelete() 
{
	if(MouseListener())
		MouseListener()->Delete();
}

void CFemAppOpenGLView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MouseListener() != 0);
}

BOOL CFemAppOpenGLView::CopyBitmapToFile(const CString &sPathName) //wjrx mantis 2461
{
  return CopyBitmap(sPathName);
}

void CFemAppOpenGLView::CopyClipboard()
{
  CopyBitmap();
}

BOOL CFemAppOpenGLView::CopyBitmap(const CString &sPathName)
{
        BOOL bSuccess= TRUE;
	COpenGLView *pLegend = LegendView();

	if(!pLegend)
	{
		return ClientToClipBoard(sPathName);
	}

	// legend view exists
	// get bitmap data from both views and combine them
	BITMAPINFOHEADER headerView;
	unsigned char *pPixelDataView = 0;
	int NbBytesView;
	int sizexView;
	int sizeyView;

	ClientAsBitmap(&headerView, &pPixelDataView, &NbBytesView, &sizexView, &sizeyView);

	if(sizexView == 0 || sizeyView == 0) return FALSE; // no view to copy (?)

	int bppView = NbBytesView / (sizexView * sizeyView);

	BITMAPINFOHEADER headerLegend;
	unsigned char *pPixelDataLegend = 0;
	int NbBytesLegend;
	int sizexLegend;
	int sizeyLegend;

	pLegend->ClientAsBitmap(&headerLegend, &pPixelDataLegend, &NbBytesLegend, &sizexLegend, &sizeyLegend);

	if(sizexLegend == 0 || sizeyLegend == 0)
	{
		// no legend
		return ClientToClipBoard(sPathName);
	}

	int bppLegend = NbBytesLegend / (sizexLegend * sizeyLegend);

	assert(bppView == bppLegend);

	// the view's header (must be equal to legend's header) is used as the header
	// the pixel data is combined, where the legend is at the left of the view
	// if one view is higher than the other it's clipped
	int NbBytes = NbBytesView + NbBytesLegend;
	int sizex = sizexView + sizexLegend;

	int sizey = std::min(sizeyView, sizeyLegend);

	int bpp = bppView;
	assert(sizex % 4 == 0);
	unsigned char *pPixelData = new unsigned char[NbBytes];
	unsigned char *pos = pPixelData;

	int linesizeLegend = sizexLegend * bpp;
	int linesizeView = sizexView * bpp;

	unsigned char *posLegend = pPixelDataLegend;
	unsigned char *posView = pPixelDataView;

	// now copy the bytes line by line
	for(int i = 0; i < sizey; i++)
	{
		int j;
		for(j = 0; j < linesizeLegend; j++) *(pos++) = *(posLegend++);
		for(j = 0; j < linesizeView;   j++) *(pos++) = *(posView++);
	}

	delete[] pPixelDataLegend;
	delete[] pPixelDataView;

	// combine header
	BITMAPINFOHEADER header;
	header.biWidth = sizex;
	header.biHeight = sizey;
	header.biSizeImage = NbBytes;
	header.biSize = 40;
	header.biPlanes = 1;
	header.biBitCount =  3 * 8; // RGB.
	header.biCompression = 0;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	// copy the new bitmap to the clipboard or to file
        if ( sPathName == "" )
        {
          BitmapToClipboard(&header, pPixelData, NbBytes);
          bSuccess= true;
        }
        else
        { //wjrx mantis 2461
          bSuccess= BitmapToFile(&header, pPixelData, NbBytes, sPathName);
        }

	// free memory
	delete[] pPixelData;

        return bSuccess;
}
