 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include <qgl.h>
#include <GL/glu.h>
#include "IOpenGLFrame.h"
#include "OpenGLDraw.h"
#include "IDrawDef.h"

#define SELECT_BUFFER_LENGTH 1024
#define SELECT_ID_OFFSET 200
#define SELECT_ID_NONE 150

/*!
  \class IOpenGLFrame
  \brief This class is interface of the OpenGL-frame parent-child hierarchy. A frame represents a view port in OpenGL.

  The function of the class is
  - implement a parent-child relationship between the frames
  - make abstraction of Qt or MFC OpenGL window
  - supply helper functions for drawing (setting matrices, creating displaylists, etc.)
  - distributing mouse-messages over the frames
  - implement selection
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IOpenGLFrame::IOpenGLFrame()
: m_pParent(0), m_ViewPort(0), m_bLightingEnabled(false), m_updateCount( 0 )
{
  //optimizsation for the swapbuffer
  m_ptDraw = new COpenGLDraw(*this);
}

IOpenGLFrame::~IOpenGLFrame()
{
  ReParent(0);
  delete m_ptDraw;
}

IOpenGLFrame* IOpenGLFrame::Parent()
{
  return m_pParent;
}

/*!
  Returns the parent of the current frame. When the function returns a 0-pointer the current frame has no parent.

  \sa Root(), ReParent()
*/
const IOpenGLFrame* IOpenGLFrame::Parent() const
{
  return m_pParent;
}

/*!
  Returns the root parent, the parent with Parent() == 0, of the current frame. Note the frame itself could also be the root.
  
  \sa Parent()
*/
const IOpenGLFrame& IOpenGLFrame::Root() const
{
  if(Parent())
  return Parent()->Root();
  return *this;
}

/*!
  Returns the root parent, the parent with Parent() == 0, of the current frame. Note the frame itself could also be the root.
  
  \sa Parent()
*/
IOpenGLFrame& IOpenGLFrame::Root()
{
  if(Parent())
  return Parent()->Root();
  return *this;
}

/*!
  Changes the parent of the frame. When the old parent frame has a device context the attached displaylists are flushed.
  The OnDetachChild() and OnAttachChild() function on respectively the old en new parent are called.
  When the parent has a device context the OnResizeParent function is called.

  \sa Parent(), OnAttachChild(), OnDetachChild(), OnResizeParent()
*/
bool IOpenGLFrame::ReParent(IOpenGLFrame* pParent)
{

  //first clear selection lists, before a call to UpdateFrame
  m_SelectionMap.clear();
  m_SelectionMapRev.clear();

  if(DeviceContext())
  FlushDisplayList();

  if(Parent())
  {
  Parent()->OnDetachChild(*this);
  for(size_t i = 0; i < Parent()->m_vcChild.size(); i++)
  {
      if(Parent()->m_vcChild[i] == this)
    Parent()->m_vcChild.erase(Parent()->m_vcChild.begin() + i);
  }
  }


  m_pParent = pParent;

  if(Parent())
  {
  Parent()->m_vcChild.push_back(this);
  Parent()->OnAttachChild(*this);
  
  if(DeviceContext())
      OnResizeParent(); // Force a resize event
  }
  
  return true;
}

bool IOpenGLFrame::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if(MouseListener())
  {
  if(MouseListener()->MousePress(state, button, point))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  { 
  
  if(Child(i).HitTest(point))
  {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      
      if(Child(i).MousePress(state, button, child_point))
    return true;
  }
  }

  return false;
}

bool IOpenGLFrame::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if(MouseListener())
  {
  if(MouseListener()->MouseRelease(state, button, point))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  
  if(Child(i).HitTest(point))
  {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());

      if(Child(i).MouseRelease(state, button, child_point))
    return true;
  }
  }

  return false;
}

bool IOpenGLFrame::MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if(MouseListener())
  {
  if(MouseListener()->MouseDblClk(state, button, point))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  if(Child(i).HitTest(point))
  {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());

      if(Child(i).MouseDblClk(state, button, child_point))
    return true;
  }
  }

  return false;
}

bool IOpenGLFrame::MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point)
{
  if(MouseListener())
  {
  if(MouseListener()->MouseWheel(state, nDelta, point))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  
  if(Child(i).HitTest(point))
  {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      
      if(Child(i).MouseWheel(state, nDelta, child_point))
    return true;
  }
  }

  return false;
}

bool IOpenGLFrame::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if(MouseListener())
  {
  if(MouseListener()->MouseMove(state, button, point))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  
  if(Child(i).HitTest(point))
  {
      TScreenPoint child_point(point.x() - Child(i).ViewPortX(), point.y() - Child(i).ViewPortY());
      
      if(Child(i).MouseMove(state, button, child_point))
    return true;
  }
  }

  return false;
}

bool IOpenGLFrame::KeyPress(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount)
{
  if(MouseListener())
  {
  if(MouseListener()->KeyPress(state, nAscii, key, nRepCount))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  if(Child(i).KeyPress(state, nAscii, key, nRepCount))
      return true;
  }

  return false;
}

bool IOpenGLFrame::KeyRelease(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount)
{
  if(MouseListener())
  {
  if(MouseListener()->KeyRelease(state, nAscii, key, nRepCount))
      return true;
  }

  for(int i = 0; i < ChildSize(); i++)
  {
  if(Child(i).KeyRelease(state, nAscii, key, nRepCount))
      return true;
  }

  return false;
}

void IOpenGLFrame::OnResizeParent()
{
  for(int i = 0; i < ChildSize(); i++)
  Child(i).OnResizeParent();
}

void IOpenGLFrame::OnResizeChild(IOpenGLFrame &/*child*/)
{
}

void IOpenGLFrame::DrawScene()
{
  if(MouseListener()) MouseListener()->DrawScene();
}

void IOpenGLFrame::EnableLighting(bool lighting)
{
  m_bLightingEnabled = lighting;
  UpdateFrame();
}

long IOpenGLFrame::CountScene()
{
  long lRet = 0;
  for(int i = 0; i < ChildSize(); i++)
  lRet += Child(i).CountScene();
  return lRet;
}

CMouseListener* IOpenGLFrame::MouseListener()
{
  if(m_skMouseListener.size())
  return m_skMouseListener.top();

  return 0;
}

IOpenGLFrame::TColor IOpenGLFrame::BackgroundColor() const
{
  if(Parent())
  return Parent()->BackgroundColor();

  return qRgb(0, 0, 0);
}

IOpenGLFrame::TColor IOpenGLFrame::BackgroundColor2() const
{
  if (Parent())
  return Parent()->BackgroundColor2();

  return qRgb(0, 0, 0);
}

IOpenGLFrame::TColor IOpenGLFrame::TextColor() const
{
  if (Parent())
  return Parent()->TextColor();

  return qRgb(255, 255, 255);
}

//void IOpenGLFrame::BackgroundColor(TColor color)
//{
//  BackgroundColor(color);
//}

/*!
  The function is called when the client has to flush it displaylists, because the devicecontext is changed or
  detached.

  \sa DeleteList()
*/
void IOpenGLFrame::FlushDisplayList()
{
  for(int i = 0; i < ChildSize(); i++)
  Child(i).FlushDisplayList();
}

/*! 
  Draws an existing displaylist. The displaylist can be generated with the CompileList() function.
  The displaylist is deleted with DeleteList(). Note that by detaching the frame with device context FlushDisplayList() is called and existing lists should be deleted.
  This function can be used in the DrawScene() override.

  \sa DrawScene(), CompileList(), DeleteList(), FlushDisplayList()
*/ 
void IOpenGLFrame::DrawList(int nList, const IDrawDef& DrawDef)
{
  //call the display list
  m_ptDraw->SetOpenGLAtrib(DrawDef);
  if ( glIsList( nList ) != GL_TRUE ) {
  DIA_ASSERT( false );
  GLuint code = glGetError();
  if ( code ) qDebug( "glGetError() = %d", code );
  }
  glCallList(nList);
  m_ptDraw->RestoreOpenGLAtrib();
}

/*! 
  The displaylist can be generated with the CompileList() function. DrawList() draws an existing displaylist.
  The displaylist is deleted with DeleteList(). Note that by detaching the frame with device context
  FlushDisplayList() is called and existing lists should be deleted. If bDrawSteps = true the OnDrawStep()
  is called during drawing.

  \sa DrawScene(), DrawList(), DeleteList(), FlushDisplayList()
*/ 
int IOpenGLFrame::CompileList(const geo::IObject & object, const IDrawDef& DrawDef, bool bDrawSteps)
{
  VERIFY(MakeCurrent());
  
  // Compile displaylist ... So get displaylist nr ...
  GLuint nDisplayListNr = glGenLists(1);

  //compile the display list
  glNewList(nDisplayListNr, GL_COMPILE);
  m_ptDraw->Draw(object, DrawDef, bDrawSteps);
  glEndList();
  DIA_ASSERT(glIsList(nDisplayListNr) == GL_TRUE);

  return nDisplayListNr;
}

/*! 
  Draws a geometry object based on a IDrawDef. The drawdef specifies how the geometry object is painted.
  DrawObject is called during the DrawScene() call. If bDrawSteps = true the OnDrawStep() is called during
  drawing.

  \sa DrawScene()
*/
void IOpenGLFrame::DrawObject(const geo::IObject &object, const IDrawDef &DrawDef, bool bDrawSteps)
{
  bool bRegistered = false;
  if(DrawDef.IsSelectable(object))
  bRegistered = RegisterSelectableObject(object);

  m_ptDraw->SetOpenGLAtrib(DrawDef);
  m_ptDraw->Draw(object, DrawDef, bDrawSteps);
  m_ptDraw->RestoreOpenGLAtrib();

  if(bRegistered) glPopName();
}

long IOpenGLFrame::CountObject(const geo::IObject& object, const IDrawDef& DrawDef) const
{
  return m_ptDraw->Count(object, DrawDef);
}

/*! 
  DeleteList() deletes the displaylist. Note that by detaching the frame with device context FlushDisplayList() is called and existing lists should be deleted.
  The displaylist can be generated with the CompileList() function. DrawList() draws an existing displaylist.

  \sa DrawScene(), DrawList(), CompileList(), FlushDisplayList()
*/
void IOpenGLFrame::DeleteList(int nList)
{
  VERIFY(MakeCurrent());
  DIA_ASSERT(glIsList(nList) == GL_TRUE);
  glDeleteLists(nList, 1);
  GLuint code = glGetError();
  if ( code ) {
  DIA_ASSERT(glGetError() != GL_INVALID_OPERATION);
  DIA_ASSERT(glGetError() != GL_INVALID_VALUE);
  }
  DIA_ASSERT(glIsList(nList) == GL_FALSE);
}

void IOpenGLFrame::RenderText(const double& dX,
              const double& dY,
              const double& dZ,
              const QString& text,
              const QFont& fnt)
{
  assert(Parent());
  Parent()->RenderText(dX, dY, dZ, text, fnt);
}

bool IOpenGLFrame::RegisterSelectableObject(const geo::IObject& object)
{

  GLuint id = GetSelectionID(object);
  if(m_CurrenNameID != id)
  {
  glPushName(id);
  m_CurrenNameID=id;
  return true;
  }
  return false;
}

void IOpenGLFrame::SetCursor(TCursor cursor)
{
  assert(Parent());
  Parent()->SetCursor(cursor);
}

/*!
  The function is called when a child is attached to the frame with the ReParent() function.

  \sa ReParent(), OnDetachChild()
*/
void IOpenGLFrame::OnAttachChild(IOpenGLFrame& /*child*/)
{
}

/*!
  The function is called when a child is detached to the frame with the ReParent() function.
*/
void IOpenGLFrame::OnDetachChild(IOpenGLFrame& /*child*/)
{
}

/*!
  Returns the amount of attached childs.
*/
int IOpenGLFrame::ChildSize() const
{
  return (int)m_vcChild.size();
}

/*!
  Returns the child by index. The index is zero based.
*/
const IOpenGLFrame& IOpenGLFrame::Child(int nIndex) const
{
  return *m_vcChild[nIndex];
}

/*!
  Returns the child by index. The index is zero based.
*/  
IOpenGLFrame& IOpenGLFrame::Child(int nIndex)
{
  return *m_vcChild[nIndex];
}

/*!
  When true is returned the root frame can supply a device context and the client can draw in the frame.

  \sa MakeCurrent()
*/
bool IOpenGLFrame::DeviceContext() const
{
  if(Parent())
  return Parent()->DeviceContext();
  return false;
}

/*!
  Make the devicecontext the cureent devicecontext. Returns false when there is no devicecontext.
  
  \sa DeviceContext()
*/
bool IOpenGLFrame::MakeCurrent() const
{
  if(Parent())
  return Parent()->MakeCurrent();
  return false;
}

void IOpenGLFrame::FlipBuffers()
{
  assert(Parent());
  Parent()->FlipBuffers();
}

void IOpenGLFrame::OnUpdateFrame()
{
  if ( m_updateCount > 1 ) {
  // qDebug( "IOpenGLFrame::m_updateCount = %d", m_updateCount );
  return;
  }
  
  ClearFrame();  // Clear Frame with own background color

  int i;
  for ( i = 0; i < ChildSize(); i++ ) {
  Child(i).m_updateCount++;
  }
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  SetupProjectionMatrix();

  //cash the projection
  glGetDoublev(GL_PROJECTION_MATRIX, m_ProjectionMatrix);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  SetupModelMatrix();

  //cash the modelview matix
  glGetDoublev(GL_MODELVIEW_MATRIX, m_ModelMatrix);

  SetupViewPortMatrix();

  //cash viewport
  glGetIntegerv(GL_VIEWPORT,m_ViewPortMatrix);

  m_CurrenNameID=(unsigned)-1;
  glInitNames();
  glPushName(0);
  DrawScene();
  glPopName();

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  for ( i = 0; i < ChildSize(); i++ ) {
  Child(i).OnUpdateFrame();
  }
  for ( i = 0; i < ChildSize(); i++ ) {
  Child(i).m_updateCount--;
  if ( Child(i).m_updateCount > 0 ) {
      // An intermediate update request for child, make sure
      // it is done when this update cascade is finished.
      Child( i ).m_updateCount = 0;
      m_updateCount++;
  }
  }
}

void IOpenGLFrame::UpdateFrame()
{
  m_updateCount++; // ignore updates during this update

  while ( m_updateCount == 1 ) {
  // Keep updating until no update requests left

  if(MakeCurrent()) {

      OnBeginDraw( OnCountDrawSteps() );
      OnUpdateFrame();

      glFlush();
      FlipBuffers();
      OnEndDraw();
  }
  if ( m_updateCount > 1 ) {
      // Apparently some requests for update came during
      // this update. Frame might be out of date so do one
      // extra update.
      m_updateCount = 1;
  } else {
      if ( m_updateCount != 1 ) {
    qDebug( "IOpenGLFrame::m_updateCount != 1" );
      }
      m_updateCount = 0;
  }
  }
}

void IOpenGLFrame::SetupProjectionMatrix() const
{
  VERIFY(MakeCurrent());

  

  glOrtho(0, 
      ViewPortWidth(),
      0,
      ViewPortHeight(),
      0, 
      1);
}

void IOpenGLFrame::SetupViewPortMatrix() const
{
  VERIFY(MakeCurrent());
  
  TScreenPoint point = ChildToRoot(TScreenPoint(0, 0));
  glViewport(point.x(), point.y(), ViewPortWidth(), ViewPortHeight());
}

void IOpenGLFrame::SetupModelMatrix() const
{
  VERIFY(MakeCurrent());

  glLoadIdentity();


  //make the matrix, regular 2D viewing matrix..... 
  gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

  if ( m_bLightingEnabled ) glEnable(  GL_LIGHTING );
  else                      glDisable( GL_LIGHTING );
}

void IOpenGLFrame::ClearFrame()
{
  // Clear the color and depth buffers
  TColor iColor = BackgroundColor();
  float iRed   = qRed(iColor) / 255.0;
  float iGreen = qGreen(iColor) / 255.0;
  float iBlue  = qBlue(iColor) / 255.0;
  glClearColor(iRed, iGreen, iBlue, 0.0f) ;
  glClearStencil(0);
  
  QPoint p(0,0);
  p = ChildToRoot(p);
  glScissor(p.x(),p.y(),ViewPortWidth(),ViewPortHeight());
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);

//#if 0
  TColor iColor2 = BackgroundColor2();
  if (iColor2 != iColor) // gradient
  {
  float iRed2 = qRed(iColor2) / 255.0;
  float iGreen2 = qGreen(iColor2) / 255.0;
  float iBlue2 = qBlue(iColor2) / 255.0;

  glBegin(GL_QUADS);
  glColor3f(iRed, iGreen, iBlue);
  glVertex2f(1.0, -1.0);
  glVertex2f(-1.0, -1.0);
  glColor3f(iRed2, iGreen2, iBlue2);
  glVertex2f(-1.0, 1.0);
  glVertex2f(1.0, 1.0);
  glEnd();
  }
//#endif
}


IOpenGLFrame::TObjectVec IOpenGLFrame::ProcessSelection(const TScreenPoint& point)
{
  // Our return type
  TObjectVec vcRet;
  VERIFY(MakeCurrent());


  //setup a selection buffer
  GLuint selectBuff[SELECT_BUFFER_LENGTH];
  GLint hits=0,viewport[4];
  glSelectBuffer(SELECT_BUFFER_LENGTH,selectBuff);

  //get and setup matrixmode
  glRenderMode(GL_SELECT);

  SetupViewPortMatrix();
  glGetIntegerv(GL_VIEWPORT,viewport);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  
  //set up a pick matrix (is projection matrix)
  TScreenPoint root_point = ChildToRoot(point);
  gluPickMatrix(root_point.x(), root_point.y(), 8, 8, viewport);
  SetupProjectionMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  SetupModelMatrix();

  //render the scene (now using the OpenGl name stack !!)
  glInitNames();
  glPushName(0);
  DrawScene();
  glPopName();

  //get the number of hitrecords and process the hits
  hits=glRenderMode(GL_RENDER);
  vcRet = ProcessHits(hits,selectBuff);

  //restore matrices
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  UpdateFrame();
  
  return vcRet;
}

geo::CPoint IOpenGLFrame::UnProject(const IOpenGLFrame::TScreenPointDepth& screen_point) const
{
  return UnProject(screen_point.first,screen_point.second);
}

geo::CPoint IOpenGLFrame::UnProject(const TScreenPoint& screen_point, const double& dDepth) const
{
  // TODO: Match screen coor with viewp.
  double dX, dY, dZ;

  TScreenPoint root_point = ChildToRoot(screen_point);

  gluUnProject(root_point.x(),
         root_point.y(),
         dDepth,
         m_ModelMatrix,
         m_ProjectionMatrix,
         m_ViewPortMatrix,
         &dX,
         &dY,
         &dZ);

  return geo::CPoint(dX,dY,dZ);
}

IOpenGLFrame::TScreenPointDepth IOpenGLFrame::Project(const geo::IPoint &world_point) const
{
  // TODO: Match screen coor with viewp.
  double dX, dY, dZ;

  gluProject(world_point.X(),
         world_point.Y(),
         world_point.Z(),
    m_ModelMatrix,
    m_ProjectionMatrix,
    m_ViewPortMatrix,
         &dX,
         &dY,
         &dZ);

  return TScreenPointDepth(RootToChild(TScreenPoint(int(dX), int(dY))), dZ);
}



//examen the hitrecords in the selection buffer and fill
//the selectionset structure
IOpenGLFrame::TObjectVec IOpenGLFrame::ProcessHits(GLint nHitSize , GLuint *pSelectBuff) 
{
  typedef std::map<GLuint, std::vector<const geo::IObject *> > TZBufferObjectsMap;
  TZBufferObjectsMap mpZBufferObjects;

  // Loop over all hit records
  int nBufferPosition = 0;
  for(int nHit = 0; nHit < nHitSize; nHit++)
  {
  // The buffer contains nHitSize records with the following format.
  // 0. StackSize for the Names Stack nStackSize
  // 1. Minimum Z value
  // 2. Maximum Z value
  // 3. Bottom name stack
  // ....
  // nStackSize + 2. Top of stack
  // Read stack size, Z min and Z max
  int nStackSize = pSelectBuff[nBufferPosition];
  nBufferPosition++;
  GLuint uintZmin = pSelectBuff[nBufferPosition];
  nBufferPosition++;
  GLuint uintZmax = pSelectBuff[nBufferPosition];
  nBufferPosition++;

  // divide first to prevent overflow
  GLuint uintZAverage = uintZmin / 2 + uintZmax / 2;

  TZBufferObjectsMap::iterator itMap =
      mpZBufferObjects.insert(std::make_pair(uintZAverage, std::vector<const geo::IObject *>())).first;

  // Read the name stack
  for(int i = 0; i < nStackSize; i++)
  {
      // Search name index of the stack in the selection map
      TSelectionMapRev::iterator it = m_SelectionMapRev.find(pSelectBuff[nBufferPosition]);
      if(it != m_SelectionMapRev.end())
      {
    // We have a displaylist in our selection so put in the vector...
    itMap->second.push_back(it->second);
      }
      nBufferPosition++;
  }
  }

  // Our return type
  std::vector<const geo::IObject*> vcRet;

  for(TZBufferObjectsMap::iterator it = mpZBufferObjects.begin(); it != mpZBufferObjects.end(); it++)
  {
  for(size_t i = 0; i < it->second.size(); i++)
  {
      vcRet.push_back(it->second[i]);
  }
  }

  return vcRet;
}

//return the current renderstate (GL_RENDER,GL_SELECT,GL_FEEDBACK)
int IOpenGLFrame::GLRenderMode() const
{
  int iRenderMode;
  glGetIntegerv(GL_RENDER_MODE,&iRenderMode);
  return iRenderMode;
}

const geo::IObject* IOpenGLFrame::GetSelectableObject(GLuint selectionID) const
{
  TSelectionMapRev::iterator it = m_SelectionMapRev.find(selectionID);
  assert(it != m_SelectionMapRev.end());

  return it->second;
}

GLuint IOpenGLFrame::GetSelectionID(const geo::IObject& object) const
{
  TSelectionMap::iterator it = m_SelectionMap.find(&object);
  
  if(it == m_SelectionMap.end())
  {
  GLuint id = (GLuint)m_SelectionMap.size()+SELECT_ID_OFFSET;
  std::pair<TSelectionMap::iterator, bool> check = m_SelectionMap.insert(std::make_pair(&object,id));
  assert(check.second);
  m_SelectionMapRev.insert(std::make_pair(id,&object));

  return id;
  }

  return (*it).second;
}

void IOpenGLFrame::CancelAllMouseListeners()
{
  // Some mouselisteners refuse to delete, because they are fixed in the view.
  CMouseListener *pPrevious = 0;

  while((MouseListener() != pPrevious) && MouseListener())
  {
  pPrevious = MouseListener();
  MouseListener()->Cancel();
  }
}

void IOpenGLFrame::ValidateAllMouseListeners()
{
  // Some mouselisteners refuse to delete, because they are fixed in the view.
  CMouseListener *pPrevious = 0;

  while((MouseListener() != pPrevious) && MouseListener())
  {
  pPrevious = MouseListener();
  MouseListener()->Cancel();
  }
}

IOpenGLFrame::TScreenPoint IOpenGLFrame::GlobalScreenPoint(const TScreenPoint& point) const
{
  assert(Parent());
  return Parent()->GlobalScreenPoint(ChildToRoot(point));
}

IOpenGLFrame::TScreenPoint IOpenGLFrame::RootToChild(const TScreenPoint& root_point) const
{
  const IOpenGLFrame *pFrame = this;
  TScreenPoint ret = root_point;

  while(pFrame)
  {
  ret = ret - TScreenPoint(pFrame->ViewPortX(), pFrame->ViewPortY());
  pFrame = pFrame->Parent();
  }

  return ret;
}

IOpenGLFrame::TScreenPoint IOpenGLFrame::ChildToRoot(const TScreenPoint& child_point) const
{
  const IOpenGLFrame *pFrame = this;
  TScreenPoint ret = child_point;

  while(pFrame)
  {
  ret = ret + TScreenPoint(pFrame->ViewPortX(), pFrame->ViewPortY());
  pFrame = pFrame->Parent();
  }

  return ret;
}

int IOpenGLFrame::ViewPortX() const
{
  if(m_ViewPort)
  return m_ViewPort[0];
  return 0;
}

int IOpenGLFrame::ViewPortY() const
{
  if(m_ViewPort)
  return m_ViewPort[1];
  return 0;
}

int IOpenGLFrame::ViewPortWidth() const
{
  if(m_ViewPort)
  return m_ViewPort[2];
  return Parent()->ViewPortWidth();
}

int IOpenGLFrame::ViewPortHeight() const
{
  if(m_ViewPort)
  return m_ViewPort[3];
  return Parent()->ViewPortHeight();
}

/*!
  Function returns when frame align to the parent size. When frame has a fixed size align to parent function return false.

  \sa SetViewPort(), RestoreAlignToParent()
*/
bool IOpenGLFrame::AlignToParent() const
{
  return m_ViewPort == 0;
}

/*!
  Restores the alignment to parent size.

  \sa SetViewPort(), AlignToParent()
*/  
void IOpenGLFrame::RestoreAlignToParent()
{
  delete []m_ViewPort;
  m_ViewPort = 0;
}

/*!
  Sets the viewport of the frame. AlignToParent() will return false. Automatic alignment can be restored by calling RestoreAlignToParent().

  \sa AlignToParent(), RestoreAlignToParent()
*/
void IOpenGLFrame::SetViewPort(int lX, int lY, int lWidth, int lHeight)
{
  if(m_ViewPort == 0)
  m_ViewPort = new int[4];
  m_ViewPort[0] = lX;
  m_ViewPort[1] = lY;
  m_ViewPort[2] = lWidth;
  m_ViewPort[3] = lHeight;
  if(Parent())
  OnResizeParent();
}

bool IOpenGLFrame::HitTest(const TScreenPoint& point) const
{
  QRect rect(ViewPortX(), ViewPortY(), ViewPortWidth(), ViewPortHeight());
  return rect.contains(point);
}

/*!
  Function is called to collect the number of steps made during drawscene.
  The number of steps is returned and default the number of steps of the childs
  are collected...
*/
int IOpenGLFrame::OnCountDrawSteps() const
{
  // Count steps for the children
  int nRet = 0;
  for(int i = 0; i < ChildSize(); i++)
  nRet += Child(i).OnCountDrawSteps();
  return nRet;
}

/*!
  Function is called when an UpdateFrame() is called. The number of steps to make
  is returned. Default this function propagate to the parent.
*/
void IOpenGLFrame::OnBeginDraw(int nSteps)
{
  if(Parent()) Parent()->OnBeginDraw(nSteps);
}

void IOpenGLFrame::OnDrawStep(int nSteps)
{
  if(Parent()) Parent()->OnDrawStep(nSteps);
}

/*!
  Function is called when an UpdateFrame() is ended. Default this function propagate to the parent.
*/
void IOpenGLFrame::OnEndDraw()
{
  if(Parent()) Parent()->OnEndDraw();
}

/*!
   Step function to perform one or more steps. The OnDrawStep is called.
*/
void IOpenGLFrame::DrawStep(int nSteps) const
{
  const_cast<IOpenGLFrame&>(*this).OnDrawStep(nSteps);
}


