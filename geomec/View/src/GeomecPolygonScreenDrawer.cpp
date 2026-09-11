#include "stdafx.h"

#include "GeomecPolygonScreenDrawer.h"

#include <Inventor/events/SoEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include "intersection.h"
#include "OpenInventorEventsHandler.h"
#include "Inventor/nodes/SoCoordinate3.h"
#include "Inventor/nodes/SoMaterial.h"
#include "Inventor/nodes/SoMarkerSet.h"
#include "Inventor/nodes/SoSwitch.h"
#include "Inventor/nodes/SoSeparator.h"
#include "IvTune/SoIvTune.h"
#include "Inventor/nodes/SoMaterialBinding.h"

SO_NODE_SOURCE( GeomecPolygonScreenDrawer );

GeomecPolygonScreenDrawer::GeomecPolygonScreenDrawer() : Base()
{
  SO_NODE_CONSTRUCTOR( GeomecPolygonScreenDrawer );


  isBuiltIn = TRUE;

  switchToGreen();
  m_selfIntersects = false;
  m_finalized = false;
  m_dragging = false;
  m_firstMouseDown = true;
  m_rotated = false;
  m_drawerMode = Creating;
  m_polygonMode = Polygon;


  SoNode * node = sceneGraph.getValue();
  SoSeparator * nodeAsSeparator = dynamic_cast<SoSeparator *> (node);
  if (!nodeAsSeparator) return;

  SoMaterialBinding *markerMaterialBinding = new SoMaterialBinding;
  markerMaterialBinding->value = SoMaterialBinding::PER_VERTEX;
  nodeAsSeparator->addChild(markerMaterialBinding);

     m_pointMarkerGroup = new SoGroup;
  nodeAsSeparator->addChild(m_pointMarkerGroup);
}

//------------------------------------------------------------------------------
// 
GeomecPolygonScreenDrawer::~GeomecPolygonScreenDrawer()
{
}

//------------------------------------------------------------------------------
// 
void GeomecPolygonScreenDrawer::initClass()
{
  SO__NODE_INIT_CLASS( GeomecPolygonScreenDrawer, "GeomecPolygonScreenDrawer", SoPolygonScreenDrawer );
}

//------------------------------------------------------------------------------
// 
void GeomecPolygonScreenDrawer::exitClass()
{
  SO__NODE_EXIT_CLASS( GeomecPolygonScreenDrawer );
}


/** Called on keyDown event. Should be implemented by sub-classes. */
void GeomecPolygonScreenDrawer::onKeyDown( SoHandleEventAction*  action)
{
  if (m_finalized) return;
  int numPoints = point.getNum();

  const SoKeyboardEvent * event = static_cast<const SoKeyboardEvent *> (action->getEvent());
  switch (event->getKey())
  {
  case SoKeyboardEvent::RETURN:
    revertOnSelfIntersect();
    finalize(action);
    break;
  case SoKeyboardEvent::ESCAPE:
    clean();
    finalize(action);
    break;
  default:
    Base::onKeyDown(action);
  }

  updateMarkers();
}

void GeomecPolygonScreenDrawer::onMouseUp( SoHandleEventAction* action)
{
  if (m_finalized) return;
  // TRACE("1: onMouseUp: points = %d\n", point.getNum());

  if (m_dragging && (m_drawerMode == Modifying || m_polygonMode == Rectangular) )
  {
    onMouseDown(action );
  }

  Base::onMouseUp(action);
  // TRACE("2: onMouseUp: points = %d\n", point.getNum());
}

void GeomecPolygonScreenDrawer::onMouseDown( SoHandleEventAction* action)
{
  if (m_finalized) return;

  // TRACE("1: onMouseDown: points = %d\n", point.getNum());

  Base::onMouseDown(action); // will add one or two (m_firstMouseDown) points

  // TRACE("2: onMouseDown: points = %d\n", point.getNum());

  if (m_drawerMode == Modifying)
  {
    // we don't want new points, just (re)move them
    removeLastPoint();
    if (m_firstMouseDown)
      removeLastPoint();

    if(m_dragging)
    {
      // dragging a point
      if (!selfIntersects())
      {
        // legal polygon, 
        m_dragging = false;
      }

      if (m_polygonMode == SinglePoint)
      {
        assert (point.getNum() == 2);
        removePoint(0); // remove original point
        finalize(action);
        return;
      }
    }
    else
    {
      // selecting a point
      if (!handleModificationOnMouseDown(action))    
      {
        // no point was selected
        finalize(action);
        return;
      }
      m_dragging = true;
    }
  }
  else // Creating
  {
    int numPoints = point.getNum();
    if (numPoints > 2 && (actionOnOrigin(action) || m_polygonMode == Rectangular))
    {
      // polygon is closed
      removeLastPoint();
      if (m_polygonMode != Rectangular) //  If rectangular, we need only the four points we already have
        removeLastPoint();
      // TRACE("4: onMouseDown: points = %d\n", point.getNum());
      finalize(action);
    }
    else if (numPoints == 2 && m_polygonMode == SinglePoint && !m_firstMouseDown)
    {
      removeLastPoint();
      finalize(action);
    }
    else
    {
      revertOnSelfIntersect();
    }
    m_dragging = true;
  }

  m_firstMouseDown = false;

  updateMarkers();
  // TRACE("3: onMouseDown: points = %d\n", point.getNum());
}

void GeomecPolygonScreenDrawer::onMouseMove( SoHandleEventAction* action)
{
  if (m_polygonMode == SinglePoint && m_firstMouseDown)
  {
    onMouseDown(action);
    Base::onMouseUp(action);

    if (m_drawerMode == Modifying)
    {
      int numPoints = point.getNum();
      assert (numPoints == 1);
      addPoint (point[0]);
    }
    else
    {
      removeLastPoint();
    }
  }

  // TRACE("# points: %d\n", point.getNum());
  if (m_finalized)
    return;

  if (m_dragging)
    Base::onMouseMove(action);

  // do not exactly know when onMouseDragging() is called and when onMouseMove() ...
  onMousePositionChanged(action);

  updateMarkers();
}

void GeomecPolygonScreenDrawer::onMouseDragging( SoHandleEventAction* action)
{
  if (m_finalized)
    return;

  if (m_dragging)
    Base::onMouseDragging(action);

  // do not exactly know when onMouseDragging() is called and when onMouseMove() ...
  onMousePositionChanged(action);

  updateMarkers();
}

void GeomecPolygonScreenDrawer::onMouseDblClick( SoHandleEventAction* action)
{
  if (m_finalized) return;
  revertOnSelfIntersect();
  finalize(action);
}

void GeomecPolygonScreenDrawer::rotatePoints( int pLast )
{
  std::vector<SbVec2f> rotatedPoints;
  int numPoints = point.getNum();
  for (int p = 1; p <= numPoints; ++p)
  {
    rotatedPoints.push_back(point[(p + pLast) % numPoints]);
  }

  clear();
  for (int p = 0; p < numPoints; ++p)
  {
    addPoint(rotatedPoints[p]);
  }
}

void GeomecPolygonScreenDrawer::checkSelfIntersection()
{
  std::vector<SbVec2f> pointVector;
  int numPoints = point.getNum();
  for (int p = 0; p < numPoints; ++p)
  {
    pointVector.push_back(point[p]);
  }

  if ((m_selfIntersects = ::selfIntersects(pointVector)))
  {
    switchToRed();
  }
  else
  {
    switchToGreen();
  }
}

void GeomecPolygonScreenDrawer::makeRectangular()
{
  if (m_polygonMode != Rectangular) return;

  if (point.getNum() == 0) return;
                                                  // 0 ---> 1
  if (point.getNum() == 2)                      // |      |
  {                                             // |      |
    addPoint(point[1]);                       // 3 <--- 2
    addPoint(point[1]);
  }
  assert (point.getNum() == 4);

  // adapt points 0 and 2 to match 1 and 3 as point 3 is being moved
     point.set1Value(0, point[1][0], point[3][1]);
     point.set1Value(2, point[3][0], point[1][1]);
}

bool GeomecPolygonScreenDrawer::selfIntersects()
{
  return m_selfIntersects;
}

void GeomecPolygonScreenDrawer::setDrawerMode( DrawerMode mode )
{
  m_drawerMode = mode;
}

void GeomecPolygonScreenDrawer::setPolygonMode( PolygonMode mode )
{
  m_polygonMode = mode;
}
/// Selected point is to be made the last point by rotating the list of points so it can be moved around
bool GeomecPolygonScreenDrawer::handleModificationOnMouseDown( SoHandleEventAction* action )
{
  if (m_polygonMode == SinglePoint) return true;

  int p = getActionPointIndex(action, false);
  if (p >= 0)
  {
    size_t numPoints = point.getNum();
    rotatePoints (p);
    m_rotated = true;
    m_rotatedOut = point[numPoints - 1];
    // removePoint(numPoints - 1);
    return true;
  }

  return false;
}

void GeomecPolygonScreenDrawer::clean()
{
  clear();
  updateMarkers();
  m_rotated = false;
  m_selfIntersects = false;
  m_drawerMode = Creating;
}

SbVec2f GeomecPolygonScreenDrawer::getActionPoint( SoHandleEventAction* action )
{
  const SoEvent* event = action->getEvent();
  const SbViewportRegion& viewport = action->getViewportRegion();

  float width = viewport.getWindowSize()[0];
  float height = viewport.getWindowSize()[1];

  float x =  (2.0f * event->getPosition()[0] / width) - 1.0f;
  float y =  (2.0f * event->getPosition()[1] / height) - 1.0f;

  return SbVec2f (x, y);
}

bool GeomecPolygonScreenDrawer::actionOnOrigin( SoHandleEventAction* action )
{
  return point.getNum() > 0 && CloseEnough(point[0], getActionPoint(action));
}

int GeomecPolygonScreenDrawer::getActionPointIndex( SoHandleEventAction* action, bool doNotUseLastPoint )
{
  SbVec2f mousePoint = getActionPoint(action);
  size_t numPoints = point.getNum();

  size_t offset = doNotUseLastPoint ? 1 : 0;

  for (size_t i = 0; i + offset < numPoints; ++i)
  {
    if (CloseEnough(point[i], mousePoint))
      return i;
  }

  return -1;
}

void GeomecPolygonScreenDrawer::switchToRed()
{
  color = SbColor(1.0f, 0.0f, 0.0f);
}

void GeomecPolygonScreenDrawer::switchToGreen()
{
  color = SbColor(0.0f, 1.0f, 0.0f);
}

void GeomecPolygonScreenDrawer::finalize( SoHandleEventAction* action )
{
  switchToGreen();
  COpenInventorEventsHandler::SetCursor(Qt::ArrowCursor);
  Base::finalize(action);
  clean();
  m_finalized = true;
}

bool GeomecPolygonScreenDrawer::revertOnSelfIntersect()
{
  bool intersecting = selfIntersects();
  if (intersecting)
  {
    // remove the point that creates the self intersect and replace by old point if necessary.
    removePoint(point.getNum() - 1);
    if (m_rotated)
    {
      addPoint(m_rotatedOut);
    }
  }
  m_rotated = false;

  return intersecting;
}

void GeomecPolygonScreenDrawer::removeLastPoint()
{
  removePoint(point.getNum() - 1);
}

bool GeomecPolygonScreenDrawer::actionAtDestination( SoHandleEventAction* action )
{
  return (m_drawerMode == Creating ? actionOnOrigin (action) : (getActionPointIndex(action, m_dragging ? true : false) >= 0));
}

void GeomecPolygonScreenDrawer::onMousePositionChanged( SoHandleEventAction* action )
{
  makeRectangular(); // if needed
  checkSelfIntersection();

  if (actionAtDestination(action))
  {
    COpenInventorEventsHandler::SetCursor(Qt::ArrowCursor);
  }
  else
  {
    COpenInventorEventsHandler::SetCursor(Qt::CrossCursor);
  }
}

void GeomecPolygonScreenDrawer::updateMarkers()
{
  int numPoints = point.getNum();

  const int32_t markerIndex = SoMarkerSet::SQUARE_FILLED_9_9;
  const SbColor markerYellowColor (1.0f, 1.0f, 0.1f);
  const SbColor markerRedColor (1.0f, 0.1f, 0.1f);
  const SbColor markerGreenColor (0.1f, 1.0f, 0.1f);

  // remove obsolete point markers
  for (int p = m_pointMarkerGroup->getNumChildren() - 1; p >= numPoints; --p)
  {
    removePointMarker(p);
  }

  // create missing point markers
  for (int p = m_pointMarkerGroup->getNumChildren(); p < numPoints; ++p)
  {
    addPointMarker();
  }

  for (int p = 0; p < numPoints; ++p)
  {
    SbVec2f pt = point[p];
    setPointMarkerCoordinates (p, pt);
    setPointMarkerIndex (p, markerIndex);
    if (p == 0)
      setPointMarkerColor (p, markerGreenColor);
    else if (p == numPoints - 1)
      setPointMarkerColor (p, markerRedColor);
    else
      setPointMarkerColor (p, markerYellowColor);
  }     
}

void GeomecPolygonScreenDrawer::addPointMarker()
{
  SoCoordinate3 * pointMarkerCoordinates = new SoCoordinate3;
  SoMaterial * pointMarkerMaterial = new SoMaterial;
  SoMarkerSet * pointMarkerSet = new SoMarkerSet;

  pointMarkerCoordinates->point.setNum(1);
  pointMarkerSet->markerIndex.setNum(1);
  pointMarkerMaterial->diffuseColor.setNum(1);
  pointMarkerMaterial->specularColor.setNum(1);
  pointMarkerMaterial->ambientColor.setNum(1);
  pointMarkerMaterial->emissiveColor.setNum(1);

  SoSeparator * pointMarker = new SoSeparator;
  pointMarker->addChild(pointMarkerCoordinates);
  pointMarker->addChild(pointMarkerMaterial);
  pointMarker->addChild(pointMarkerSet);

  m_pointMarkerGroup->addChild(pointMarker);
}

void GeomecPolygonScreenDrawer::removePointMarker( int p )
{
  assert (p < m_pointMarkerGroup->getNumChildren());
  m_pointMarkerGroup->removeChild(p);
}

void GeomecPolygonScreenDrawer::setPointMarkerCoordinates( int p, const SbVec2f & pt )
{
  SoSeparator * pointMarker = static_cast<SoSeparator *> (m_pointMarkerGroup->getChild(p));
  SoCoordinate3 * pointMarkerCoordinates = static_cast<SoCoordinate3 *> (pointMarker->getChild(0));
  pointMarkerCoordinates->point.set1Value(1, pt[0], pt[1], 0);
}

void GeomecPolygonScreenDrawer::setPointMarkerIndex( int p, const int32_t markerIndex )
{
  SoSeparator * pointMarker = static_cast<SoSeparator *> (m_pointMarkerGroup->getChild(p));
  SoMarkerSet * pointMarkerSet = static_cast<SoMarkerSet *> (pointMarker->getChild(2));
  pointMarkerSet->markerIndex.set1Value(1, markerIndex);
}

void GeomecPolygonScreenDrawer::setPointMarkerColor( int p, const SbColor & markerHighlightColor )
{
  SoSeparator * pointMarker = static_cast<SoSeparator *> (m_pointMarkerGroup->getChild(p));
  SoMaterial * pointMarkerMaterial = static_cast<SoMaterial *> (pointMarker->getChild(1));
  pointMarkerMaterial->diffuseColor.set1Value(1, markerHighlightColor);
  pointMarkerMaterial->specularColor.set1Value(1, markerHighlightColor);
  pointMarkerMaterial->ambientColor.set1Value(1, markerHighlightColor);
  pointMarkerMaterial->emissiveColor.set1Value(1, markerHighlightColor);
}

bool CloseEnough( const SbVec2f & pt, const SbVec2f & mousePoint )
{
  float distance = (pt - mousePoint).length();
  return  distance < 0.02;
}

