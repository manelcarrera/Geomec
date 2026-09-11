// Annotation example - Gnomon (aka compass)
//
// Mercury Computer Systems Inc
// Visualization Sciences Group
//
// Original: MMH
// Modified:
//
// Notes:
// 1) Uses some direct OpenGL calls to avoid depth buffer problems.
//    On Windows it should automatically link in the OpenGL library.
#include "stdAfx.h"

#include "opengl.h"

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

// Open Inventor nodes we will need
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

// Open Inventor traversal state elements we will need
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

// Make sure we link against OpenGL (convenient hack on Windows)
#ifdef WIN32
#pragma comment(lib, "OpenGL32.lib")
#endif

////////////////////////////////////////////////////////////////////////
//
// Gnomon parameters
static const float GNOMON_CAMDIST = 4.0;
static const int GNOMON_WIDTH = 100; // pixels
static const int GNOMON_HEIGHT = GNOMON_WIDTH;

////////////////////////////////////////////////////////////////////////
//
// Define the gnomon geometry (simple axes in this case)
//
// We could define these programmatically, but using a memory stream
// avoids including headers for nodes that we don't actually use in our
// program. We could define these in a .iv file, but the memory stream
// avoids the danger of not having the file available at run time.
//
// For convenience we define the gnomon geometry in a -1..1 space
// because this is the default view volume for Open Inventor cameras.
//
// Note that, to be safe, we reset some attributes that might be
// inherited from the scene graph and are not appropriate for our axes.
// But to be safe you should also keep the actual scene under its own
// Separator, so its attributes won't affect the gnomon.

char *gnomonGeometry = "\
                       #Inventor V2.0 ascii\n\
                       \
                       DEF GnomonGeom Separator { \
                           PickStyle { style UNPICKABLE } \
                           DrawStyle { style FILLED } \
                           LightModel { model PHONG } \
                           MaterialBinding { value OVERALL } \
                           Complexity { value .2 } \
                           ShapeHints { vertexOrdering COUNTERCLOCKWISE shapeType SOLID } \
                           Font { name \"Arial\" size 12 } \
                           Separator { \
                               Material { \
                               diffuseColor    [ 0.5 0 0 ] \
                               emissiveColor   [ 0.5 0 0 ] \
                               } \
                               RotationXYZ { axis Z angle -1.570796327 } \
                               Cylinder { height 2 radius .06 } \
                               Translation { translation 0 1 0 } \
                               Cone { bottomRadius .18 height .3 } \
                               Translation { translation 0 0.2 0 } \
                               Material { \
                                   diffuseColor    [ 1 1 0 ] \
                                   emissiveColor   [ 1 1 0 ] \
                               } \
                               Text2 { string \"N\" } \
                           } \
                           Separator { \
                               Material { \
                                   diffuseColor    [ 0 0.5 0 ] \
                                   emissiveColor   [ 0 0.5 0 ] \
                               } \
                               Cylinder { height 2 radius .06 } \
                               Translation { translation 0 1 0 } \
                               Cone { bottomRadius .18 height .3 } \
                               Translation { translation 0 0.2 0 } \
                               Material { \
                                   diffuseColor    [ 1 1 0 ] \
                                   emissiveColor   [ 1 1 0 ] \
                               } \
                               Text2 { string \"E\" } \
                           } \
                           Separator { \
                               Material { \
                                   diffuseColor    [ 0 0 0.5 ] \
                                   emissiveColor   [ 0 0 0.5 ] \
                               } \
                               RotationXYZ { axis X angle 1.570796327 } \
                               Cylinder { height 2 radius .06 } \
                               Translation { translation 0 1 0 } \
                               Cone { bottomRadius .18 height .3 } \
                               Translation { translation 0 0.2 0 } \
                               Material { \
                                   diffuseColor    [ 1 1 0 ] \
                                   emissiveColor   [ 1 1 0 ] \
                               } \
                               Text2 { string \"D\" } \
                           } \
                       } ";

////////////////////////////////////////////////////////////////////////
//
// Gnomon callback function
//
// This function will be called (from our Callback node) just before
// the gnomon camera is traversed.  It does some drawing setup and
// modifies the gnomon camera, as follows:
//
// 1) Modify OpenGL viewport to position the gnomon in the window.
// 2) Clear the OpenGL depth buffer in the region where the gnomon
//    will be drawn (to ensure gnomon is "on top").
// 3) Modify the gnomon camera to:
//    a) Have the same orientation as the 3D scene camera.
//    b) View the gnomon geometry at a consistent size.
//

void gnomonCB(void *userData, SoAction *action) {
  // Don't do anything if this is not a render traversal
  if (action->isOfType(SoGLRenderAction::getClassTypeId())) {

    // Suppress auto-caching above the callback node to ensure
    // that this code will actually be traversed on every render.
    //
    // The gnomon geometry itself is under a Separator, which
    // should be cached, so there is no performance penalty.
    SoState *state = action->getState();
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);

    // Set the OpenGL viewport to position gnomon in the window.
    //
    // Note that we modify the Open Inventor traversal state (rather
    // than calling OpenGL directly) so that any nodes that depend on
    // knowing the viewport will function correctly.
    //
    // This gnomon position is in the lower-left corner.
    SbViewportRegion vport(GNOMON_WIDTH, GNOMON_HEIGHT);
    SoViewportRegionElement::set(state, vport);

    // Reset depth buffer so gnomon is always "on top of" other geometry.
    //
    // Open Inventor does this automatically in a few special cases, but
    // currently we don't have a public mechanism, so we have to call
    // OpenGL directly.  It should be safe to do this here because the
    // gnomon is the last thing in the scene graph.
    glClear(GL_DEPTH_BUFFER_BIT);

    // Get the current camera rotation from the viewing matrix.
    //
    // Note that the viewing matrix is applied to geometry, so it's
    // actually the inverse of the matrix we need.  The getTransform
    // method will return the rotation in variable cameraRotation.
    //
    // We could get the camera orientation by hiding a pointer to the
    // camera, but this approach is very "fragile".  For example, if the
    // user changes the camera type by clicking the viewer button, the
    // viewer destroys the current camera and creates a new one.
    SbMatrix viewMat = SoViewingMatrixElement::get(state);
    SbVec3f tran, scale;
    SbRotation cameraRotation, orient;
    viewMat.inverse().getTransform(tran, cameraRotation, scale, orient);

    // Get ptr to the gnomon's camera
    SoCamera *camera = (SoCamera *)userData;

    // Disable notification because we're already traversing
    camera->enableNotify(FALSE);

    // Set the new orientation for the gnomon camera
    camera->orientation = cameraRotation;

    // Get the current "focal distance" (distance to pt-of-rotation)
    float distance = camera->focalDistance.getValue();

    // Reposition the camera so it's looking at the pt-of-rotation
    // (which is the center of the gnomon or 0,0,0 in this case).
    //
    // We'll use an algorithm similar to what the examiner viewer uses
    // to spin the camera around the pt-of-rotation.
    // First get the rotation as a matrix.  Conveniently SbMatrix has an
    // assignment operator that takes an SbRotation value.
    // Next extract the view direction vector from the rotation matrix.
    // Now move the camera radius units along the direction vector.
    SbMatrix mx;
    mx = cameraRotation;
    SbVec3f direction(-mx[2][0], -mx[2][1], -mx[2][2]);
    camera->position = SbVec3f(0, 0, 0) - distance * direction;
  }
}

////////////////////////////////////////////////////////////////////////
//
// Create the gnomon scene graph
//
SoSeparator *makeGnomon() {
  SoSeparator *pRoot = new SoSeparator(1);
  SoSwitch *pSwitch = new SoSwitch(2);
  SoCallback *pCallb = new SoCallback;
  SoPerspectiveCamera *pCam = new SoPerspectiveCamera;
  SoResetTransform *pReset = new SoResetTransform;
  pRoot->ref();
  pRoot->setName("Gnomon");

  // Switch node will allow us turn the gnomon on and off
  pSwitch->setName("GnomonSwitch");
  pRoot->addChild(pSwitch);

  // Callback node allows us to modify the OpenGL viewport
  // and modify the gnomon camera during traversal
  pCallb->setCallback(gnomonCB, (void *)pCam);
  pSwitch->addChild(pCallb);

  // Setup gnomon camera (viewAll might not work here because we want
  // the point of rotation to be the origin of the axes (i.e. 0,0,0),
  // not the geometric center of the gnomon geometry.
  pCam->setName("GnomonCam");
  pCam->position = SbVec3f(0, 0, GNOMON_CAMDIST);
  pCam->nearDistance = 1;
  pCam->farDistance = 6;
  pCam->focalDistance = GNOMON_CAMDIST;
  pSwitch->addChild(pCam);

  // Read the gnomon geometry from a memory buffer
  SoInput in;
  in.setBuffer((void *)gnomonGeometry, (size_t)strlen(gnomonGeometry));
  SoNode *node;
  SbBool ok = SoDB::read(&in, node);
  if (ok && node != NULL) {
    pSwitch->addChild(node);
    pSwitch->whichChild = SO_SWITCH_ALL;
  }

  // Reset the bounding box so gnomon geometry will not
  // affect bbox of scene (ie. will not affect viewAll).
  pSwitch->addChild(pReset);
  pReset->whatToReset = SoResetTransform::BBOX;

  pRoot->unrefNoDelete();
  return pRoot;
}

#ifdef TEST_GNOMON
////////////////////////////////////////////////////////////////////////
//
// Application main function
void main(int argc, char **argv) {
  char *filename = "test.iv";
  if (argc > 1)
    filename = argv[1];

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Unable to open '%s'\n", filename);
    filename = NULL; // Indicate we do not have a file
  } else
    fclose(fp);

  // Initialize Inventor
  HWND myWindow = SoWin::init(argv[0]);

  // If we have an input file, try to read it
  SoSeparator *pScene = NULL;
  if (filename != NULL) {
    SoInput in;
    in.openFile(filename);
    pScene = SoDB::readAll(&in);
    in.closeFile();
  } else {
    // Else create a simple scene graph
    pScene = new SoSeparator;
    SoMaterial *pMatl = new SoMaterial;
    SoCone *pCone = new SoCone;

    pMatl->diffuseColor.setValue(1, 0, 0);
    pScene->addChild(pMatl);
    pScene->addChild(pCone);
  }
  pScene->setName("_3D_Scene");

  // Keep the scene under its own separator so it
  // won't affect the appearance of the gnomon.
  SoSeparator *pRoot = new SoSeparator;
  pRoot->ref();
  pRoot->setName("SceneRoot");
  pRoot->addChild(pScene);

  // Create and initialize viewer
  SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
  myViewer->setBackgroundColor(SbColor(.68f, .68f, .78f));
  myViewer->setSceneGraph(pRoot);
  myViewer->setTitle("Gnomon Example");
  myViewer->show();

  // Add gnomon to scene graph
  // Specifically the gnomon subgraph must be inserted after the
  // camera that the viewer is controlling, because it uses the
  // viewing matrix inherited through the scene graph.  There are
  // other ways to do this, e.g. passing a pointer to the scene
  // camera to the callback, but this is fragile and breaks for
  // example if the user switches to an orthographic camera.
  //
  // Note that we need to make this addChild call *after* the
  // initial setScenegraph call, to ensure that the viewer will
  // control the scene camera.  If the actual scene does not
  // contain a camera (typical), then the viewer would find the
  // camera in the gnomon subgraph and control that one!
  SoSeparator *pGnomon = makeGnomon();
  pRoot->addChild(pGnomon);

  // Loop then cleanup
  SoXt::show(myWindow);
  SoXt::mainLoop();
  delete myViewer;
  pRoot->unref();
  SoXt::finish();
}
#endif // TEST_GNOMON
