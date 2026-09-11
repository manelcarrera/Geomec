// OpenGLView.cpp: implementation of the COpenGLView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "dimple.h"
#include "DrawDef.h"
#include "IDrawDef.h"
#include "LabelPoint.h"
#include "OpenGLScene.h"
#include "OpenGLView.h"
#include <GL/glu.h>
#include <cmath>

#include "ObjectModifier.h"

#include "Sphere.h"

#include <QRgb>

#define GL_SELECT_MOUSEDOWN 0
#define GL_SELECT_MOUSEMOVE 1

IMPLEMENT_DYNCREATE(COpenGLView, CView)

typedef void(__stdcall *TglAddSwapHintRectWIN)(GLint x, GLint y, GLsizei width, GLsizei height);
TglAddSwapHintRectWIN glAddSwapHintRectWIN = 0;

static int GetContrastColor(int color) {
  int iRed = 255 - GetRValue(color);
  int iGreen = 255 - GetGValue(color);
  int iBlue = 255 - GetBValue(color);

  return RGB(iRed, iGreen, iBlue);
}

void TraceVertex(const geo::ICoordinate &coor) { TRACE("(X,Y,Z) = (%f,%f,%f)\n", coor.X(), coor.Y(), coor.Z()); }

/////////////////////////////////////////////////////////////////////////////
// COpenGLView

// IMPLEMENT_DYNCREATE(COpenGLView, CView)

// ##ModelId=3B653BCA0244
COpenGLView::COpenGLView()
    : m_pPal(0), m_hRC(0), m_hBitmap(0), m_hDC(0), m_BackgroundColor(qRgb(0, 0, 0)), m_BackgroundColor2(qRgb(0, 0, 0)),
      m_TextColor(qRgb(255, 255, 255)), m_bTrackMouse(false) {
  // set default cursor
  SetCursor(Qt::ArrowCursor);
}

// ##ModelId=3B653BCA0274
COpenGLView::~COpenGLView() {}

COpenGLView::TScreenPoint COpenGLView::WinToOpenGL(const CPoint &point) const {
  CRect rect;
  GetClientRect(&rect);

  return TScreenPoint(point.x, rect.Height() - point.y);
}

BEGIN_MESSAGE_MAP(COpenGLView, CView)
//{{AFX_MSG_MAP(COpenGLView)
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDBLCLK()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_MBUTTONDOWN()
ON_WM_MBUTTONUP()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_WM_SETCURSOR()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
ON_WM_PALETTECHANGED()
ON_WM_QUERYNEWPALETTE()
ON_WM_MOUSEWHEEL()
ON_WM_MOUSEACTIVATE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLView drawing

// ##ModelId=3B653BCA02C3
void COpenGLView::OnDraw(CDC *pDC) { UpdateFrame(); }

/////////////////////////////////////////////////////////////////////////////
// COpenGLView diagnostics

#ifdef _DEBUG
// ##ModelId=3B653BCA02D0
void COpenGLView::AssertValid() const { CView::AssertValid(); }

// ##ModelId=3B653BCA02D2
void COpenGLView::Dump(CDumpContext &dc) const { CView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLView message handlers

// ##ModelId=3B653BCA02C1
void COpenGLView::OnInitialUpdate() {
  CView::OnInitialUpdate();

  // Initial Compilitation of Scenes
}

// ##ModelId=3B653BCA0245
void COpenGLView::OutputGlError(char *label) {
  GLenum errorno = glGetError();
  if (errorno != GL_NO_ERROR)
    TRACE("%s had error: #(%d) %s\r\n", label, errorno, gluErrorString(errorno));
}

void COpenGLView::ClearGlError() {
  GLenum errorno = 1;
  while (errorno != GL_NO_ERROR)
    errorno = glGetError();
}

//////////////////////////////////////////////////////////////////////////////////////
//
// OpenGLpalette
//
// ##ModelId=3B653BCA0205
unsigned char COpenGLView::m_threeto8[8] = {0, 0111 >> 1, 0222 >> 1, 0333 >> 1, 0444 >> 1, 0555 >> 1, 0666 >> 1, 0377};
// ##ModelId=3B653BCA01F7
unsigned char COpenGLView::m_twoto8[4] = {0, 0x55, 0xaa, 0xff};
// ##ModelId=3B653BCA01F6
unsigned char COpenGLView::m_oneto8[2] = {0, 255};

// ##ModelId=3B653BCA0206
int COpenGLView::m_defaultOverride[13] = {0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91};

// ##ModelId=3B653BCA0216
PALETTEENTRY COpenGLView::m_defaultPalEntry[20] = {
    {0, 0, 0, 0}, // 0
    {0x80, 0, 0, 0},       {0, 0x80, 0, 0},    {0x80, 0x80, 0, 0},    {0, 0, 0x80, 0},
    {0x80, 0, 0x80, 0},    {0, 0x80, 0x80, 0}, {0xC0, 0xC0, 0xC0, 0}, // 7

    {192, 220, 192, 0},                                            // 8
    {166, 202, 240, 0},    {255, 251, 240, 0}, {160, 160, 164, 0}, // 11

    {0x80, 0x80, 0x80, 0}, // 12
    {0xFF, 0, 0, 0},       {0, 0xFF, 0, 0},    {0xFF, 0xFF, 0, 0},    {0, 0, 0xFF, 0},
    {0xFF, 0, 0xFF, 0},    {0, 0xFF, 0xFF, 0}, {0xFF, 0xFF, 0xFF, 0} // 19
};

//
// ComponentFromIndex
//
// ##ModelId=3B653BCA0256
unsigned char COpenGLView::ComponentFromIndex(int i, unsigned int nbits, unsigned int shift) {
  unsigned char val;

  val = (unsigned char)(i >> shift);
  switch (nbits) {

  case 1:
    val &= 0x1;
    return m_oneto8[val];

  case 2:
    val &= 0x3;
    return m_twoto8[val];

  case 3:
    val &= 0x7;
    return m_threeto8[val];

  default:
    return 0;
  }
}

void COpenGLView::InitializePalette(HDC hDC) {
  TRACE0("InitializePalette\r\n");

  PIXELFORMATDESCRIPTOR pfd;
  LOGPALETTE *pPal;
  int iPixelFormat;
  int iColors;

  BYTE byteRedRange;
  BYTE byteGreenRange;
  BYTE byteBlueRange;

  iPixelFormat = GetPixelFormat(hDC);
  DescribePixelFormat(hDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  if (!(pfd.dwFlags & PFD_NEED_PALETTE)) {
    return;
  }

  iColors = 1 << pfd.cColorBits;

  pPal = (LOGPALETTE *)new char[sizeof(LOGPALETTE) + iColors * sizeof(PALETTEENTRY)];

  pPal->palVersion = 0x300;
  pPal->palNumEntries = iColors;

  byteRedRange = (1 << pfd.cRedBits) - 1;
  byteGreenRange = (1 << pfd.cGreenBits) - 1;
  byteBlueRange = (1 << pfd.cBlueBits) - 1;

  for (int i = 0; i < iColors; i++) {
    pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & byteRedRange;
    pPal->palPalEntry[i].peRed = (BYTE)((double)pPal->palPalEntry[i].peRed * 255.0 / byteRedRange);

    pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & byteGreenRange;
    pPal->palPalEntry[i].peGreen = (BYTE)((double)pPal->palPalEntry[i].peGreen * 255.0 / byteGreenRange);

    pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & byteBlueRange;
    pPal->palPalEntry[i].peBlue = (BYTE)((double)pPal->palPalEntry[i].peBlue * 255.0 / byteBlueRange);

    pPal->palPalEntry[i].peFlags = (BYTE)0;
  }

  if (m_pPal)
    delete m_pPal;

  m_pPal = new CPalette;

  m_pPal->CreatePalette(pPal);
  SelectPalette(hDC, *m_pPal, FALSE);
  RealizePalette(hDC);
  delete[] pPal;
}

//
// CreateRGBPalette
//
// ##ModelId=3B653BCA0254
BOOL COpenGLView::CreateRGBPalette(HDC hDC) {
  //
  // Check to see if we need a palette
  //

  PIXELFORMATDESCRIPTOR pfd;
  int n = GetPixelFormat(hDC);
  DescribePixelFormat(hDC, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  if (!(pfd.dwFlags & PFD_NEED_PALETTE))
    return FALSE;

  TRACE0("Creating palette\r\n");
  // allocate a log pal and fill it with the color table info
  LOGPALETTE *pPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));
  if (!pPal) {
    TRACE("Out of memory for logpal");
    return FALSE;
  }
  pPal->palVersion = 0x300;  // Windows 3.0
  pPal->palNumEntries = 256; // table size

  //
  // Create RGB Palette
  //
  assert(pfd.cColorBits == 8);
  n = 1 << pfd.cColorBits;
  for (int i = 0; i < n; i++) {
    pPal->palPalEntry[i].peRed = ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
    pPal->palPalEntry[i].peGreen = ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
    pPal->palPalEntry[i].peBlue = ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
    pPal->palPalEntry[i].peFlags = 0;
  }

  //
  // Fix up color table with system colors.
  //
  if ((pfd.cColorBits == 8) && (pfd.cRedBits == 3) && (pfd.cRedShift == 0) && (pfd.cGreenBits == 3) &&
      (pfd.cGreenShift == 3) && (pfd.cBlueBits == 2) && (pfd.cBlueShift == 6)) {
    for (int j = 1; j <= 12; j++)
      pPal->palPalEntry[m_defaultOverride[j]] = m_defaultPalEntry[j];
  }

  // Delete any existing GDI palette
  if (m_pPal)
    delete m_pPal;

  m_pPal = new CPalette;

  BOOL bResult = m_pPal->CreatePalette(pPal);

  SelectPalette(hDC, *m_pPal, FALSE);
  RealizePalette(hDC);

  free(pPal);

  return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView message handlers

//
// PreCreateWindow
//
// ##ModelId=3B653BCA0266
BOOL COpenGLView::PreCreateWindow(CREATESTRUCT &cs) {
  TRACE0("PreCreateWindow\r\n");
  cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);

  return CView::PreCreateWindow(cs);
}

// virtual
BYTE COpenGLView::StencilBufferDepth() const { return 3; }

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  int ret;

  ret = CView::OnCreate(lpCreateStruct);

  TRACE0("OnCreate\r\n");
  if (ret == -1)
    return ret;

  int iPixelFormat;
  m_hDC = ::GetDC(m_hWnd);

  BYTE stencil = StencilBufferDepth();

  // CreateRGBPalette(GetCurrentDC());
  static PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
                                      1,
                                      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SWAP_COPY,
                                      PFD_TYPE_RGBA,
                                      24,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      32,
                                      stencil,
                                      0,
                                      PFD_MAIN_PLANE,
                                      0,
                                      0,
                                      0,
                                      0};

  iPixelFormat = ChoosePixelFormat(GetCurrentDC(), &pfd);
  VERIFY(SetPixelFormat(GetCurrentDC(), iPixelFormat, &pfd));

  InitializePalette(GetCurrentDC());
  m_hRC = wglCreateContext(GetCurrentDC());
  InitGL();

  if (!glAddSwapHintRectWIN)
    glAddSwapHintRectWIN = (TglAddSwapHintRectWIN)wglGetProcAddress("glAddSwapHintRectWIN");

  return ret;
}

void COpenGLView::InitGL() {
  MakeCurrent();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE,GL_FILL
  glShadeModel(GL_SMOOTH);                   // GL_FLAT
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black Background
  glClearDepth(1.0f);                   // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);              // Enables Depth Testing
  glDepthFunc(GL_LESS /*QUAL*/);        // The Type Of Depth Testing To Do
  BuildFont();                          // Build The Font
}

void COpenGLView::BuildFont() // Build Our Bitmap Font
{
  // use menufont
  NONCLIENTMETRICS UserMetrics;
  UserMetrics.cbSize = sizeof(UserMetrics);
  SystemParametersInfo(SPI_GETNONCLIENTMETRICS, NULL, &UserMetrics, NULL); //  'get it

  if (HFONT(m_font))
    m_font.DeleteObject();
  m_font.CreateFontIndirect(&(UserMetrics.lfMenuFont));
  if (HFONT(m_fontRot))
    m_fontRot.DeleteObject();
  m_fontRot.CreateFontIndirect(&(UserMetrics.lfMenuFont));

  UserMetrics.lfMenuFont.lfUnderline = true;

  if (HFONT(m_font2))
    m_font2.DeleteObject();
  m_font2.CreateFontIndirect(&(UserMetrics.lfMenuFont));

  m_FontID = FontCreateBitMaps(GetCurrentDC(), m_font);
  m_FontID2 = FontCreateBitMaps(GetCurrentDC(), m_font2);

  // m_FontIDRot = FontCreateBitMaps(GetCurrentDC(), m_fontRot);

  m_FontIDRot = FontCreateOutline(GetCurrentDC(), m_fontRot);
}

int COpenGLView::RegisterFont(QFont font, bool bRotate) {
  GLint base = GetFontID(font);
  if (base == -1) {
    MakeCurrent();
    if (bRotate)
      base = FontCreateOutline(GetCurrentDC(), /*font.handle()*/ 0);
    else
      base = FontCreateBitMaps(GetCurrentDC(), /*font.handle()*/ 0);

    m_fontMap[font.key()] = base;
  }

  return base;
}

void COpenGLView::UnRegisterFont(QFont font) {
  GLint id = GetFontID(font);
  if (id >= 0) {
    QMap<QString, GLuint>::iterator it = m_fontMap.find(font.key());
    if (it != m_fontMap.end())
      m_fontMap.erase(it);
    FontDelete(id);
  }
}

GLuint COpenGLView::FontCreateBitMaps(HDC hdc, HFONT font) {
  GLuint base;

  MakeCurrent();
  if ((base = glGenLists(96)) == 0)
    return (0);

  SelectObject(hdc, font);

  wglUseFontBitmaps(hdc, 32, 96, base);

  return base;
}

GLuint COpenGLView::FontCreateOutline(HDC hdc, HFONT font) {
  GLuint base;

  MakeCurrent();
  if ((base = glGenLists(96)) == 0)
    return (0);

  SelectObject(hdc, font);

  GLYPHMETRICSFLOAT agmf[256];
  wglUseFontOutlines(hdc, 32, 96, base, 0.0f, 0.0f, WGL_FONT_POLYGONS, agmf);

  return base;
}

void COpenGLView::FontDelete(GLuint fontId) {
  if (fontId == 0)
    return;

  MakeCurrent();
  glDeleteLists(fontId, 96);
}

GLint COpenGLView::GetFontID(QFont font) const {
  QMap<QString, GLuint>::const_iterator it = m_fontMap.find(font.key());
  if (it == m_fontMap.end())
    return (GLuint)-1;
  return it.value();
}

COpenGLView::TScreenPoint COpenGLView::GlobalScreenPoint(const TScreenPoint &point) const {
  CRect rect;
  GetClientRect(&rect);
  CPoint p(point.x(), rect.Height() - point.y());
  ClientToScreen(&p);
  return TScreenPoint(p.x, p.y);
}

void COpenGLView::BackgroundColor(TColor color) {
  m_BackgroundColor = color;
  UpdateFrame();
}

COpenGLView::TColor COpenGLView::BackgroundColor() const { return m_BackgroundColor; }

void COpenGLView::BackgroundColor2(TColor color) {
  m_BackgroundColor2 = color;
  UpdateFrame();
}

COpenGLView::TColor COpenGLView::BackgroundColor2() const { return m_BackgroundColor2; }

void COpenGLView::TextColor(TColor color) {
  m_TextColor = color;
  UpdateFrame();
}

COpenGLView::TColor COpenGLView::TextColor() const { return m_TextColor; }

void COpenGLView::SetCursor(HCURSOR hCursor) {
  HCURSOR hCurrent = ::SetCursor(hCursor);

#if _MSC_VER >= 1400 // VS2005
  SetClassLongPtr(GetSafeHwnd(), GCLP_HCURSOR, 0);
  assert(GetClassLongPtr(GetSafeHwnd(), GCLP_HCURSOR) == 0);
#else // VS60
  SetClassLong(GetSafeHwnd(), GCL_HCURSOR, 0);
  assert(GetClassLong(GetSafeHwnd(), GCL_HCURSOR) == 0);
#endif
}

void COpenGLView::SetCursor(TCursor cursor) {
  HCURSOR hCursor = 0;
  switch (cursor) {
  case Qt::ArrowCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    break;
  case Qt::UpArrowCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_UPARROW);
    break;
  case Qt::ForbiddenCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_NO);
    break;
  case Qt::CrossCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
    break;
  case Qt::WaitCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
    break;
  case Qt::IBeamCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_IBEAM);
    break;
  case Qt::SizeVerCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
    break;
  case Qt::SizeHorCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
    break;
  case Qt::SizeBDiagCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
    break;
  case Qt::SizeFDiagCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
    break;
  case Qt::SizeAllCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
    break;
  case Qt::PointingHandCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);
    break;
  case Qt::WhatsThisCursor:
    hCursor = AfxGetApp()->LoadStandardCursor(IDC_HELP);
    break;
  case Qt::SplitVCursor:
  case Qt::SplitHCursor:
    break;
  default:
    assert(false);
    break;
  };

  SetCursor(hCursor);

  //	return hCurrent;

  //	::SetCursor(hCursor);
}

/**********************************************************************/
/* CreateCompatibleDIB                                                */
/*                                                                    */
/* Create a DIB section with an optimal format for the specified hDC. */
/**********************************************************************/
static HBITMAP CreateCompatibleDIB(HDC hDC, HPALETTE hPalette, int iWidth, int iHeight) {
  HBITMAP hBitmap = NULL;
  HBITMAP hBitmapDevice = NULL;
  BYTE aj[sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * 255)];
  BITMAPINFO *pbmi = (BITMAPINFO *)aj;
  LPVOID pBits;
  ULONG cColors;
  BYTE ajp[sizeof(PALETTEENTRY) * 256];
  LPPALETTEENTRY lppe = (LPPALETTEENTRY)ajp;
  unsigned int i;

  // Make sure this is a display DC...
  if (GetObjectType(hDC) != OBJ_DC)
    return NULL;

  // Create a temporary bitmap from which to get display's format...
  hBitmapDevice = CreateCompatibleBitmap(hDC, 1, 1);
  if (!hBitmapDevice)
    return NULL;

  // Fill in BITMAPINFOHEADER for hBitmapTemp...
  ZeroMemory(pbmi, sizeof(aj));
  pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  GetDIBits(hDC, hBitmapDevice, 0, 0, NULL, pbmi, DIB_RGB_COLORS);

  if (pbmi->bmiHeader.biBitCount <= 8) {
    cColors = 1 << pbmi->bmiHeader.biBitCount;
    if (cColors <= 256) {
      if (hPalette)
        GetPaletteEntries(hPalette, 0, cColors, lppe);
      else
        GetSystemPaletteEntries(hDC, 0, cColors, lppe);
      for (i = 0; i < cColors; i++) {
        pbmi->bmiColors[i].rgbRed = lppe[i].peRed;
        pbmi->bmiColors[i].rgbGreen = lppe[i].peGreen;
        pbmi->bmiColors[i].rgbBlue = lppe[i].peBlue;
        pbmi->bmiColors[i].rgbReserved = 0;
      }
    }
  } else {
    // Get color masks if needed...
    if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
      GetDIBits(hDC, hBitmapDevice, 0, pbmi->bmiHeader.biHeight, NULL, pbmi, DIB_RGB_COLORS);
  }

  // Set DIB size to requested size...
  pbmi->bmiHeader.biWidth = iWidth;
  pbmi->bmiHeader.biHeight = iHeight;

  // Create the DIB section...
  hBitmap = CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);

  DeleteObject(hBitmapDevice);

  return hBitmap;
}

//
// OnSize
//
// ##ModelId=3B653BCA0283
void COpenGLView::OnSize(unsigned int nType, int cx, int cy) {
  OnResizeParent();
  //	SetScrollSizes(MM_TEXT, CSize(cx, cy));
}

int COpenGLView::ViewPortX() const { return 0; }

int COpenGLView::ViewPortY() const { return 0; }

int COpenGLView::ViewPortWidth() const {
  CRect rect;
  GetClientRect(&rect);

  return rect.Width();
}

int COpenGLView::ViewPortHeight() const {
  CRect rect;
  GetClientRect(&rect);

  return rect.Height();
}

// OnDestroy
// ##ModelId=3B653BCA0294
void COpenGLView::OnDestroy() {
  while (ChildSize() > 0)
    Child(0).ReParent(0);

  CancelAllMouseListeners();
  TRACE("COpenGLView::OnDestroy()\n");
  /*
  QMap<QString,GLuint>::Iterator it;
  for ( it = m_fontMap.begin(); it!=m_fontMap.end(); ++it ) FontDelete( it.value() );
  m_fontMap.clear();

  FontDelete(m_FontID);
  FontDelete(m_FontID2);
  FontDelete(m_FontIDRot);
  */
  if (IsCurrent())
    MakeNotCurrent();

  if (m_pPal)
    delete m_pPal;

  if (m_hRC) {
    wglDeleteContext(m_hRC);
    m_hRC = NULL;
  }

  ::ReleaseDC(m_hWnd, m_hDC);
  CView::OnDestroy();
}

// ##ModelId=3B653BCA02A3
void COpenGLView::OnPaletteChanged(CWnd *pFocusWnd) {
  TRACE0("OnPaletteChanged\r\n");
  if (m_pPal && (pFocusWnd != this)) {
    SelectPalette(GetCurrentDC(), *m_pPal, FALSE);
    RealizePalette(GetCurrentDC());
    UpdateColors(GetCurrentDC());
    return;
  }
  CView::OnPaletteChanged(pFocusWnd);
}

// ##ModelId=3B653BCA02B1
BOOL COpenGLView::OnQueryNewPalette() {
  //	return CView::OnQueryNewPalette();
  TRACE0("OnQueryNewPalette\r\n");
  // We are going active or the system palette has changed,
  // so realize our palette.
  if (m_pPal) {
    SelectPalette(GetCurrentDC(), *m_pPal, FALSE);
    int iRet = RealizePalette(GetCurrentDC());
    CRect rcClient;
    GetClientRect(&rcClient);
    InvalidateRect(&rcClient, TRUE);
    return iRet;
  }
  return CView::OnQueryNewPalette();
}

BOOL COpenGLView::OnSetCursor(CWnd *pWnd, unsigned int nHitTest, unsigned int message) {
  //	if(MouseListener())
  //		return MouseListener()->OnSetCursor(pWnd,nHitTest,message);
  //	else
  // TODO htg

  return CView::OnSetCursor(pWnd, nHitTest, message);
}

COpenGLView::TKeyboardModifiers COpenGLView::KeyboardModifiers(unsigned int uFlags) const {
  int state = Qt::NoModifier;

  if (uFlags & MK_CONTROL)
    state |= Qt::ControlModifier;
  if (uFlags & MK_SHIFT)
    state |= Qt::ShiftModifier;

  return (TKeyboardModifiers)state;
}

COpenGLView::TMouseButton COpenGLView::MouseButton(unsigned int uFlags) const {
  int state = Qt::NoButton;

  if (uFlags & MK_LBUTTON)
    state |= Qt::LeftButton;
  if (uFlags & MK_MBUTTON)
    state |= Qt::MidButton;
  if (uFlags & MK_RBUTTON)
    state |= Qt::RightButton;

  return (TMouseButton)state;
}

void COpenGLView::OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {
  if (!KeyPress(KeyboardModifiers(nFlags), nChar, (Qt::Key)0, nRepCnt))
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COpenGLView::OnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {
  if (!KeyRelease(KeyboardModifiers(nFlags), nChar, (Qt::Key)0, nRepCnt))
    CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void COpenGLView::OnMouseMove(unsigned int nFlags, CPoint point) {
  if (!m_bTrackMouse) {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = this->m_hWnd;

    if (::_TrackMouseEvent(&tme)) {
      m_bTrackMouse = true;
      EnterView();
    }
  }

  int button = Qt::NoButton;
  if (nFlags & MK_LBUTTON)
    button |= Qt::LeftButton;
  if (nFlags & MK_MBUTTON)
    button |= Qt::MidButton;
  if (nFlags & MK_RBUTTON)
    button |= Qt::RightButton;

  if (!MouseMove(KeyboardModifiers(nFlags), (TMouseButton)button, WinToOpenGL(point)))
    CView::OnMouseMove(nFlags, point);
}

void COpenGLView::OnLButtonUp(unsigned int nFlags, CPoint point) {
  if (!MouseRelease(KeyboardModifiers(nFlags), Qt::LeftButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnLButtonDblClk(unsigned int nFlags, CPoint point) {
  if (!MouseDblClk(KeyboardModifiers(nFlags), Qt::LeftButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnLButtonDown(unsigned int nFlags, CPoint point) {
  if (!MousePress(KeyboardModifiers(nFlags), Qt::LeftButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnRButtonDown(unsigned int nFlags, CPoint point) {
  if (!MousePress(KeyboardModifiers(nFlags), Qt::RightButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnRButtonUp(unsigned int nFlags, CPoint point) {
  if (!MouseRelease(KeyboardModifiers(nFlags), Qt::RightButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnMButtonDown(unsigned int nFlags, CPoint point) {
  if (!MousePress(KeyboardModifiers(nFlags), Qt::MiddleButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::OnMButtonUp(unsigned int nFlags, CPoint point) {
  if (!MouseRelease(KeyboardModifiers(nFlags), Qt::MiddleButton, WinToOpenGL(point)))
    CView::OnLButtonUp(nFlags, point);
}

BOOL COpenGLView::OnMouseWheel(unsigned int nFlags, short zDelta, CPoint point) {
  ScreenToClient(&point);
  if (!MouseWheel(KeyboardModifiers(nFlags), zDelta, RootToChild(WinToOpenGL(point))))
    return CView::OnMouseWheel(nFlags, zDelta, point);

  return TRUE;
}

BOOL COpenGLView::IsCurrent() const { return m_hRC == wglGetCurrentContext(); }

bool COpenGLView::MakeCurrent() const {
  if (!IsCurrent())
    VERIFY(wglMakeCurrent(GetCurrentDC(), m_hRC));
  return true;
}

bool COpenGLView::DeviceContext() const { return true; }

void COpenGLView::MakeNotCurrent() const { VERIFY(wglMakeCurrent(NULL, NULL)); }

GLuint COpenGLView::DefaultFontID() { return m_FontID; }

void COpenGLView::OnResizeChild(IOpenGLFrame &child) {
  //	SetScrollSizes(MM_TEXT, CSize(child.ViewPortWidth(), child.ViewPortHeight()));
}

HDC COpenGLView::GetCurrentDC() const { return m_hDC; }

void COpenGLView::FlipBuffers() { SwapBuffers(GetCurrentDC()); }

void COpenGLView::RenderText(const double &dX, const double &dY, const double &dZ, const QString &text,
                             const QFont &fnt) {
  QRgb color = TextColor();
  glPushAttrib(GL_CURRENT_BIT);
  glColor3f(qRed(color) / 255.0, qGreen(color) / 255.0, qBlue(color) / 255.0);

  glRasterPos3d(dX, dY, dZ);
  GLuint fontId = RegisterFont(fnt);

  // after registering the font all characters should exist as a displaylist
  // if not another function may have deleted it so we need to reregister
  for (int i = 0; i < text.length(); ++i) {
    if (glIsList(text.toStdString().c_str()[i] + fontId - 32) == GL_FALSE) {
      // reregister font
      TRACE("Reregistering font...\n");
      UnRegisterFont(fnt);
      fontId = RegisterFont(fnt);
      break;
    }
  }

  glPushAttrib(GL_LIST_BIT);                                                // Pushes The Display List Bits
  glListBase(fontId - 32);                                                  // Sets The Base Character to 32
  glCallLists(text.length(), GL_UNSIGNED_BYTE, text.toStdString().c_str()); // Draws The Display List Text
  glPopAttrib();                                                            // Pops The Display List Bits
  glPopAttrib();
}

//
//	OnEraseBkgnd
//
// ##ModelId=3B653BCA0296
// override to keep the background from being erased every time
// the window is repainted
BOOL COpenGLView::OnEraseBkgnd(CDC *) { return TRUE; }

// retrieve the OpenGL client as some bitmap info
// this will be used for clipboard operations etc.
// the pointer ppPixelData points to will be allocated with size *pDataSize, and should be
// freed with delete[]
void COpenGLView::ClientAsBitmap(BITMAPINFOHEADER *pHeader, unsigned char **ppPixelData, int *pDataSize, int *pSizeX,
                                 int *pSizeY) {
  MakeCurrent();

  // Get client geometry.
  CRect rect;
  GetClientRect(&rect);
  CSize size(rect.Width(), rect.Height());

  // Lines have to be 32 bytes aligned, suppose 24 bits per pixel.
  size.cx -= size.cx % 4;

  *pSizeX = size.cx;
  *pSizeY = size.cy;

  // Alloc pixel bytes.
  *pDataSize = 3 * size.cx * size.cy;
  *ppPixelData = new unsigned char[*pDataSize];

  // Copy from OpenGL.
  // GL_BGR_EXT (instead of GL_RGB) is used because Red and Blue were switched.
  ::glReadPixels(0, 0, size.cx, size.cy, GL_BGR_EXT, GL_UNSIGNED_BYTE, *ppPixelData);

  // Fill header
  pHeader->biWidth = size.cx;
  pHeader->biHeight = size.cy;
  pHeader->biSizeImage = *pDataSize;
  pHeader->biSize = 40;
  pHeader->biPlanes = 1;
  pHeader->biBitCount = 3 * 8; // RGB.
  pHeader->biCompression = 0;
  pHeader->biXPelsPerMeter = 0;
  pHeader->biYPelsPerMeter = 0;
  pHeader->biClrUsed = 0;
  pHeader->biClrImportant = 0;
}

// write the given bitmap to file //wjrx mantis 2461
bool COpenGLView::BitmapToFile(BITMAPINFOHEADER *pBMIH, unsigned char *pPixelData, int DataSize,
                               const CString &szPathName) {
  // Create a new file for writing
  FILE *pFile = fopen(szPathName, "wb");
  if (pFile == NULL) {
    return false;
  }

  assert(DataSize == pBMIH->biSizeImage); // duplicate data

  // http://www.codeproject.com/bitmap/drawing2bitmap.asp
  BITMAPFILEHEADER bmfh;
  int nBitsOffset = sizeof(BITMAPFILEHEADER) + pBMIH->biSize;
  ULONG lImageSize = pBMIH->biSizeImage;
  ULONG lFileSize = nBitsOffset + lImageSize;
  bmfh.bfType = 'B' + ('M' << 8);
  bmfh.bfOffBits = nBitsOffset;
  bmfh.bfSize = lFileSize;
  bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

  // Write the bitmap file header
  unsigned int nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
  if (nWrittenFileHeaderSize != sizeof(BITMAPFILEHEADER)) {
    fclose(pFile);
    return false;
  }

  // And then the bitmap info header
  unsigned int nWrittenInfoHeaderSize = fwrite(pBMIH, 1, sizeof(BITMAPINFOHEADER), pFile);
  if (nWrittenInfoHeaderSize != sizeof(BITMAPINFOHEADER)) {
    fclose(pFile);
    return false;
  }

  // Finally, write the image data itself
  //-- the data represents our drawing
  unsigned int nWrittenDIBDataSize = fwrite(pPixelData, 1, lImageSize, pFile);
  if (nWrittenDIBDataSize != lImageSize) {
    fclose(pFile);
    return false;
  }

  fclose(pFile);
  return true;
}

// copy the given bitmap to the clipboard
void COpenGLView::BitmapToClipboard(BITMAPINFOHEADER *pHeader, unsigned char *pPixelData, int DataSize) {
  // Generate handle.
  HANDLE handle = (HANDLE)::GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + DataSize);
  if (handle != NULL) {
    // Lock handle.
    char *pData = (char *)::GlobalLock((HGLOBAL)handle);
    // Copy header and data.
    memcpy(pData, pHeader, sizeof(BITMAPINFOHEADER));
    memcpy(pData + sizeof(BITMAPINFOHEADER), pPixelData, DataSize);
    // Unlock handle.
    ::GlobalUnlock((HGLOBAL)handle);

    // Push DIB in clipboard.
    OpenClipboard();
    EmptyClipboard();
    SetClipboardData(CF_DIB, handle);
    CloseClipboard();
  }
}

// Snap OpenGL client and send it to ClipBoard so that you can insert it
// in your favorite image editor, Paint Shop Pro, Powerpoint, etc...
//
BOOL COpenGLView::ClientToClipBoard(const CString &sPathName) {
  BOOL bSuccess = true;
  CWaitCursor cursor;

  BITMAPINFOHEADER header;
  unsigned char *pPixelData = 0;
  int NbBytes;
  int sizex;
  int sizey;

  ClientAsBitmap(&header, &pPixelData, &NbBytes, &sizex, &sizey);
  assert(pPixelData != 0);

  if (sPathName == "")
    BitmapToClipboard(&header, pPixelData, NbBytes);
  else // wjrx mantis 2461
    bSuccess = BitmapToFile(&header, pPixelData, NbBytes, sPathName);

  // Cleanup.
  delete[] pPixelData;

  cursor.Restore();
  return bSuccess;
}

void COpenGLView::OnDrawStep() const {
  // empty, must be filled by derived class
  m_iStep++;
}

void COpenGLView::OnStartDraw(int nSteps) const {
  // empty, must be filled by derived class
  m_nSteps = nSteps;
  m_iStep = 0;
}

void COpenGLView::OnEndDraw() const {
  // empty, must be filled by derived class

  //!!!!!!!!!!!!!!!!!!!!!!!
  // assert(m_iStep == m_nSteps);
  // temporary out commented
  // because its not working properly for ISymbol htg
}

void COpenGLView::SetDocument(CDocument *pDoc) { m_pDocument = pDoc; }

int COpenGLView::OnMouseActivate(CWnd *pDesktopWnd, unsigned int nHitTest, unsigned int message) {
  // I am not sure this is even necessary
  int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
  if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
    return nResult; // frame does not want to activate

  CFrameWnd *pParentFrame = GetParentFrame();
  if (pParentFrame != NULL && (pParentFrame == pDesktopWnd || pDesktopWnd->IsChild(pParentFrame))) {
    // either re-activate the current view, or set this view to be active
    CView *pView = pParentFrame->GetActiveView();
    HWND hWndFocus = ::GetFocus();
    if (pView == this && m_hWnd != hWndFocus && !::IsChild(m_hWnd, hWndFocus)) {
      // re-activate this view
      OnActivateView(TRUE, this, this);
    } else {
      // activate this view
      pParentFrame->SetActiveView(this);
    }
  }
  return nResult;

#if 0
  // Very sloppy!!
  // this function is overridden to support use of a view in a dialog
  // the base functionality in CView asserts because it can not find a parent FrameWnd window
  int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
  if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
    return nResult;   // frame does not want to activate

  return nResult;
#endif
}

LRESULT COpenGLView::OnMouseLeave(WPARAM wParam, LPARAM lParam) {
  m_bTrackMouse = false;
  LeaveView();
  return TRUE;
}

void COpenGLView::EnterView() {}

void COpenGLView::LeaveView() {}
