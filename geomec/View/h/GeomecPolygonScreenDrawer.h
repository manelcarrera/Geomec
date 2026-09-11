#pragma once

#include <Inventor/drawers/SoPolygonScreenDrawer.h>
#include <Inventor/fields/SoSFEnum.h>

class SoCoordinate3;
class SoMarkerSet;
class SoMaterial;

bool CloseEnough( const SbVec2f & pt, const SbVec2f & mousePoint );

/**
* A modifiable version of OIV SoPolygonScreenDrawer.
*/
class GeomecPolygonScreenDrawer : public SoPolygonScreenDrawer 
{
  SO_NODE_HEADER( GeomecPolygonScreenDrawer );

  friend class PolyLineScreenDrawerRetainer;

private:
  typedef SoPolygonScreenDrawer Base;

public:
  /** Constructor */
  GeomecPolygonScreenDrawer();

  enum DrawerMode
  {
    Creating,
    Modifying
  };

  enum PolygonMode
  {
    SinglePoint,
    SingleLine,
    Rectangular,
    Polygon
  };

public:

  /** Register in database */
  static void initClass();

  /** Unregister from database */
  static void exitClass();

  void setDrawerMode (DrawerMode mode);
  void setPolygonMode (PolygonMode mode);

  void clean();

protected:


  /** Called on keyDown event. Should be implemented by sub-classes. */
  virtual void onKeyDown( SoHandleEventAction* );

  /** Called on mouseDown event. Should be implemented by sub-classes. */
  virtual void onMouseDown( SoHandleEventAction* );

  void removeLastPoint();

  bool handleModificationOnMouseDown( SoHandleEventAction* action );

  SbVec2f getActionPoint( SoHandleEventAction* action );

  /** Called on mouseMove event. Should be implemented by sub-classes. */
  virtual void onMouseMove( SoHandleEventAction* );

  void onMousePositionChanged( SoHandleEventAction* action );

  bool actionAtDestination( SoHandleEventAction* action );

  /** Called on double click event. Should be implemented by sub-classes. */
  virtual void onMouseDblClick( SoHandleEventAction* );

  /** Called on mouseDrag event. Should be implemented by sub-classes. */
  virtual void onMouseDragging( SoHandleEventAction* );

  void makeRectangular();
  void checkSelfIntersection();

  void switchToGreen();

  void switchToRed();

  /** Called on mouseUp event. Should be implemented by sub-classes. */
  virtual void onMouseUp( SoHandleEventAction* );

  bool revertOnSelfIntersect();

  bool actionOnOrigin( SoHandleEventAction* action );

  int getActionPointIndex( SoHandleEventAction* action, bool doNotUseLastPoint = false );

protected:

  /** Destructor. */
  virtual ~GeomecPolygonScreenDrawer();

private:
  void finalize( SoHandleEventAction* action ); // function is also defined in base class, but non-virtual.
  bool selfIntersects();
  void rotatePoints( int p );
  void updateMarkers();
  void addPointMarker();
  void removePointMarker( int p );
  void setPointMarkerCoordinates( int p, const SbVec2f & pt );
  void setPointMarkerIndex( int p, const int32_t markerIndex );
  void setPointMarkerColor( int p, const SbColor & markerHighlightColor );
  bool m_rotated;
  SbVec2f m_rotatedOut;

  DrawerMode m_drawerMode;
  PolygonMode m_polygonMode;

  bool m_dragging;
  bool m_firstMouseDown;

  bool m_selfIntersects;
  bool m_finalized;

  SoGroup *             m_pointMarkerGroup;
};



