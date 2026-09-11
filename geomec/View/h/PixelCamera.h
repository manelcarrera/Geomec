#ifndef _PIXEL_CAMERA_H__
#define _PIXEL_CAMERA_H__

#include <Inventor/SbBase.h>
class SoGroup;

extern void addPixelSpaceCamera( SoGroup * pParent, SbBool invertX = 0, SbBool invertY = 0 );

#endif