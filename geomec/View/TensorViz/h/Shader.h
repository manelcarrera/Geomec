#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <Inventor/sys/SoGL.h>

// Stores the linked shader program and uniform locations
struct Shader {
  enum Flags { PRIMARY_COLORMAP_ENABLED = 1, SECONDARY_COLORMAP_ENABLED = 2 };

  int flags; // combination of Flags

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint program;

  // Locations of uniform parameters
  GLint colorScaleLocation;      // texture unit for colormap
  GLint colorScaleRangeLocation; // range of property values to map to colorscale
  GLint mvMatrixLocation;        // model-view matrix
  GLint mvpMatrixLocation;       // model-view-projection matrix
  GLint primaryColorLocation;    // primary color
  GLint secondaryColorLocation;  // secondary color
  GLint scaleLocation;           // beachball scale
  GLint lightDirLocation;        // lighting direction
  GLint clipPlaneLocation;       // clip plane

  Shader();

  void init(int buildFlags);
};

#endif
