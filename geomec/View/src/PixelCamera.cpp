#include "stdafx.h"
#include "opengl.h"

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoCallback.h> 
#include <Inventor/elements/SoViewportRegionElement.h>

#include <Inventor/actions/SoAction.h>


////////////////////////////////////////////////////////////////////////
//
// Declarations and code to simulate a "pixel space camera"
//
// It's simulated because we're really using a standard orthographic
// camera plus a callback node that modifies the view volume to match
// the current pixel viewport dimensions.  In other words, each pixel
// space camera is 
//
// Actually creating a new kind of camera would be more elegant, but
// also much more complicated and involving a lot of stuff that's not
// directly relevant to this example.

//----------------------------------------------------------------------
// Information an instance of pixelSpaceCamera needs
//   - The ortho camera that will be modified
//   - Invert flags for X and Y
//     If FALSE (default), the range of values is 0..N-1 pixels.
//     Else the range is -(N-1)..0, which allows geometry to be
//     positioned relative to the right and/or top edge of the window.
//   - Last viewport (so we don't modify camera unless there is a change).
class pixelCameraInfo {
public:
    pixelCameraInfo() : 
        m_pCamera(NULL), 
        m_invertX(0), 
        m_invertY(0),
        m_lastViewport(0,0) {}

    SoOrthographicCamera *m_pCamera;      // Ortho camera we will modify
    SbBool                m_invertX;      // 0..N-1 if FALSE, else -(N-1)..0
    SbBool                m_invertY;      // ditto
    SbVec2i32             m_lastViewport; // 
};

//----------------------------------------------------------------------
// Callback function that modifies the camera
//
// Gets the current viewport from the traversal state and, if the
// viewport has changed, modifies the associated orthographic camera
// so we have a "pixel space" view volume in X and Y.
//
//   - userData : Callback instance info setup by application.
//                In this case should be ptr to a pixelCameraInfo object.
//   - pAction  : The action currently traversing the scene graph.
//                Could be a render action, bounding box action, etc.
//                In this case we do the same thing for all actions.
//
void pixelCameraCB( void *userData, SoAction *pAction )
{
    // Don't do this for SoSearchAction (viewport element is not correct).
    if (pAction->isOfType(SoSearchAction::getClassTypeId()))
        return;

    // Get the current traversal state
    SoState *pState = pAction->getState();
    if (!pState)
        return;

    // Get the current viewport from the traversal state
    const SbViewportRegion &vpRegion = SoViewportRegionElement::get( pState );
    const SbVec2i32 viewport = vpRegion.getViewportSizePixels_i32();

    // Get pixel space info object
    pixelCameraInfo *pInfo = (pixelCameraInfo *)userData;

    glClear( GL_DEPTH_BUFFER_BIT );
    // Return if viewport same as last time (no need to change camera)
    if (viewport == pInfo->m_lastViewport)
         return;

    // Save viewport and compute aspect ratio
    pInfo->m_lastViewport = viewport;
    int vpWidth, vpHeight;
    viewport.getValue( vpWidth, vpHeight );
    float vpAspect = (float)vpWidth / vpHeight;

    // First disable notification on the camera node
    // (we're already in the middle of a traversal)
    // 
    // Set the view volume height to number of pixels in Y.
    // Set the aspect ratio the same as the viewport
    // (effectively setting view volume width to pixels in X).
    SoOrthographicCamera *pCamera = pInfo->m_pCamera;
    pCamera->enableNotify( FALSE );
    pCamera->height = (float)vpHeight;
    pCamera->aspectRatio = vpAspect;

    // Compute an offset that will make the view volume 0..N-1
    // (or the reverse if inverted).
    //
    // Remember that Inventor first computes a view volume centered around
    // 0,0.  I.e. -N/2 to N/2, where N is the width or the height.
    // Then it translates the view volume by pCamera->position.  So...
    //  - If the viewport width is even, for example 400, we need the left
    //    edge at zero, the center at 199.5 and the right edge at 399.
    //  - If the viewport width is odd, for example 399, we need the left
    //    edge at zero, the center at 199 and the right edge at 398.
    //
    // If the invert flag is set, that axis will be -(N-1)..0 instead.
    // This allows positioning geometry relative to the right/top edge.
    float xRadius = 0.5f * (vpWidth - 1);
    float yRadius = 0.5f * (vpHeight - 1);
    if (pInfo->m_invertX)
        xRadius = -xRadius;
    if (pInfo->m_invertY)
        yRadius = -yRadius;
    pCamera->position = SbVec3f( xRadius, yRadius, 2 );
    pCamera->enableNotify( TRUE );

#ifdef DEBUG
    fprintf( stderr, "--- Camera 0x%p updated, vp = %d %d\n", (void *)pCamera, vpWidth, vpHeight );
#endif
}

//----------------------------------------------------------------------
// Convenience function to set up a simulated "pixel space" camera
//
//   - pParent : Group node to add the camera under
//   - invertX : X will be 0..N-1 if False, else -(N-1)..0
//   - invertY : Y will be 0..N-1 if False, else -(N-1)..0
//
void addPixelSpaceCamera( SoGroup *pParent, 
                         SbBool invertX = 0, SbBool invertY = 0 )
{
    // Create an orthographic camera
    SoOrthographicCamera *p2dCam = new SoOrthographicCamera();

    // Set the viewportMapping to LEAVE_ALONE because we're going to
    // make the view volume aspect match the viewport ourselves.
    p2dCam->viewportMapping = SoCamera::LEAVE_ALONE;

    // We're going to put the camera position at +2 in Z, so these
    // near and far distances make the view volume 2 units deep in
    // Z centered around zero.  Obviously we're assuming all the
    // 2D annotation geometry really is flat!
    p2dCam->nearDistance = 1;
    p2dCam->farDistance  = 3;

    // Setup info for pixel space camera callback
    pixelCameraInfo *pInfo = new pixelCameraInfo();
    pInfo->m_pCamera = p2dCam;
    pInfo->m_invertX = invertX;
    pInfo->m_invertY = invertY;

    // Create callback node to modify the camera.
    // Set it to call pixelCameraCB with pInfo
    SoCallback *p2dCamCB = new SoCallback();
    p2dCamCB->setCallback( pixelCameraCB, (void*)pInfo );

    // Add callback and camera to scene graph (callback before camera)
    pParent->addChild( p2dCamCB );
    pParent->addChild( p2dCam   );
}





