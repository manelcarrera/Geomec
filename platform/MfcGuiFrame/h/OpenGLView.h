// OpenGLView.h: interface for the COpenGLView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVIEW_H__DFBC72A2_6608_4C08_A085_2E83EB05DD42__INCLUDED_)
#define AFX_OPENGLVIEW_H__DFBC72A2_6608_4C08_A085_2E83EB05DD42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Adds OpenGL Functionality to View

#include "IOpenGLFrame.h"
#include <qgl.h>
#include <qmap.h>

class COpenGLScene;

class COpenGLView : public CView, public IOpenGLFrame {
private:
  TMouseButton MouseButton(unsigned int uFlags) const;
  TKeyboardModifiers KeyboardModifiers(unsigned int uFlags) const;

  DECLARE_DYNCREATE(COpenGLView)

  QMap<QString, GLuint> m_fontMap;

  // Helper functions
  void InitGL();
  void BuildFont();

  // Lookup the listID for a given font
  GLint GetFontID(QFont font) const;

  mutable int m_nSteps;
  mutable int m_iStep;
  TColor m_BackgroundColor; // Color of the back ground
  TColor m_BackgroundColor2;
  TColor m_TextColor;

  // returns the current device contex
  HDC GetCurrentDC() const;

  virtual BYTE StencilBufferDepth() const;
  // opengl internal stuff
  static unsigned char m_oneto8[2];
  static unsigned char m_twoto8[4];
  static unsigned char m_threeto8[8];
  static int m_defaultOverride[13];
  static PALETTEENTRY m_defaultPalEntry[20];

  CPalette *m_pPal; // Palette

  // return the ID for the default font
  GLuint DefaultFontID();
  TScreenPoint WinToOpenGL(const CPoint &point) const;

public:
  virtual DIMENSION Dimension() const { return DIM_2D; }
  virtual bool AlignToParent() const { return false; }

  // deletes the list (glDeleteLists) allocated this font
  void FontDelete(GLuint font);

  void SetCursor(HCURSOR hCursor);
  void SetCursor(TCursor cursor);

  // Frame overrides
  virtual int ViewPortX() const;
  virtual int ViewPortY() const;
  virtual int ViewPortWidth() const;
  virtual int ViewPortHeight() const;

  virtual TScreenPoint GlobalScreenPoint(const TScreenPoint &point) const;

  virtual void OnResizeChild(IOpenGLFrame &child);

  virtual TColor BackgroundColor() const;
  virtual void BackgroundColor(TColor iColor);

  virtual TColor BackgroundColor2() const;
  virtual void BackgroundColor2(TColor iColor);

  virtual TColor TextColor() const;
  virtual void TextColor(TColor iColor);

  virtual void FlipBuffers();

  virtual void OnStartDraw(int nSteps) const; // called when a draw sequence is started, given the number of times
                                              // OnDrawStep will be called during the process
  virtual void OnDrawStep() const;            // called when a progress step is done by the drawing
  virtual void OnEndDraw() const;             // called when the drawing process is finished

  // for each font there is a allocated list.
  // there is a Lookup for Fonts and the corresponding listID
  int RegisterFont(QFont font, bool bRotate = false);

  // for each font there is a allocated list
  void UnRegisterFont(QFont font);

  // say to OpenGl this DC is the current contex
  void MakeNotCurrent() const;

  // say to OpenGl this DC is NOT the current contex
  virtual bool MakeCurrent() const;
  virtual bool DeviceContext() const;

  // See if this is the current rendering context
  BOOL IsCurrent() const;

  // get OpenGL Context handle
  HGLRC getHGLRC() const { return m_hRC; }

  // set the document for this view (necessary if the view is used in a dialog)
  void SetDocument(CDocument *pDoc);

private:
  HGLRC m_hRC;          // OpenGL Rendering Context
  HDC m_hDC;            // Device Contex
  HBITMAP m_hBitmap;    // Bitmap handle
  HBITMAP m_hOldBitmap; // Old bitmap handle

  // default fonts to use
  int m_FontID;
  int m_FontID2;
  int m_FontIDRot;
  CFont m_font;
  CFont m_font2;
  CFont m_fontRot;

  bool m_bTrackMouse;

  // internal "creation of font bitmap" of the font
  // returning the allocated ListID
  GLuint FontCreateBitMaps(HDC hdc, HFONT font);
  GLuint FontCreateOutline(HDC hdc, HFONT font);

  // pallete creation
  BOOL CreateRGBPalette(HDC hDC);

  // opengl internal stuff
  unsigned char ComponentFromIndex(int i, unsigned int nbits, unsigned int shift);
  void InitializePalette(HDC hDC);
  void ClearGlError();

  virtual void RenderText(const double &dX, const double &dY, const double &dZ, const QString &text, const QFont &fnt);

public:
  // OPENGL function to display OpenGL errors using TRACE
  void OutputGlError(char *label);

  // background
  void ClearView();

  virtual ~COpenGLView();

  BOOL ClientToClipBoard(const CString &sPathName = "");

  // retrieve the OpenGL client as some bitmap info
  // this will be used for clipboard operations etc.
  // the pointer ppPixelData points to will be allocated with size *pDataSize, and should be
  // freed with delete[]
  void ClientAsBitmap(BITMAPINFOHEADER *pHeader, unsigned char **ppPixelData, int *pDataSize, int *pSizeX, int *pSizeY);

  // copy the given bitmap to file
  bool BitmapToFile(BITMAPINFOHEADER *pBMIH, unsigned char *pPixelData, int DataSize, const CString &szPathName);
  // copy the given bitmap to the clipboard
  void BitmapToClipboard(BITMAPINFOHEADER *pHeader, unsigned char *pPixelData, int DataSize);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpenGLView)
protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  //}}AFX_VIRTUAL

  // Generated message map functions
protected:
  COpenGLView();
  int OnMouseActivate(CWnd *pDesktopWnd, unsigned int nHitTest, unsigned int message);

  //{{AFX_MSG(COpenGLView)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnDestroy();
  afx_msg BOOL OnEraseBkgnd(CDC *pDC);
  afx_msg void OnPaletteChanged(CWnd *pFocusWnd);
  afx_msg BOOL OnQueryNewPalette();
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(unsigned int nFlags, CPoint point);
  afx_msg void OnMouseMove(unsigned int nFlags, CPoint point);
  afx_msg void OnLButtonUp(unsigned int nFlags, CPoint point);
  afx_msg void OnRButtonDown(unsigned int nFlags, CPoint point);
  afx_msg void OnRButtonUp(unsigned int nFlags, CPoint point);
  afx_msg void OnMButtonDown(unsigned int nFlags, CPoint point);
  afx_msg void OnMButtonUp(unsigned int nFlags, CPoint point);
  afx_msg void OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
  afx_msg void OnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
  afx_msg BOOL OnSetCursor(CWnd *pWnd, unsigned int nHitTest, unsigned int message);
  afx_msg BOOL OnMouseWheel(unsigned int nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpenGLView)

  virtual void EnterView();
  virtual void LeaveView();

public:
  virtual void OnInitialUpdate();

protected:
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  //}}AFX_VIRTUAL

#ifdef _DEBUG
  // ##ModelId=3B653BCA02D0
  virtual void AssertValid() const;
  // ##ModelId=3B653BCA02D2
  virtual void Dump(CDumpContext &dc) const;
#endif
};

#endif // !defined(AFX_OPENGLVIEW_H__DFBC72A2_6608_4C08_A085_2E83EB05DD42__INCLUDED_)
