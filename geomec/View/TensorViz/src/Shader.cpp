#include "Shader.h"
#include "stdafx.h"

// Most of the work in the program is done in the vertex shader. We are using instanced rendering, which
// means we only have the geometry for a single beachball, and must use the vertex shader to transform
// the beachball into its proper position and orientation, using the instance parameters. This leaves the
// vertex in the local space of the mesh, so it is then transformed using the model-view-projection
// matrix.
// Clipping is done by the GPU, but to make sure clipping works properly with our custom render algorithm,
// we must compute the clip distance for each vertex. This is the closest distance from the vertex to the
// clipping plane.
// The primary and secondary colors are also assigned in the vertex shader. Depending on the build flags,
// these are taken from the corresponding uniform parameter, or looked up in the colormap, based on the
// normalized property value.
// The final step in the shader is to perform vertex lighting. A simple diffuse lighting equation is applied
// using some hardcoded parameters that look good when used with other OIV objects
const char
    *
        vertexShaderSrc
            [] =
                {"#version 330 core\n", "#define PRIMARY_COLORMAP_ENABLED\n", "#define SECONDARY_COLORMAP_ENABLED\n",
                 "layout(location=0) in vec3  position;\n",       // vertex position for beachball mesh, in local space
                 "layout(location=1) in vec3  normal;\n",         // vertex normal
                 "layout(location=2) in float colorIndex;\n",     // 0 for primaryColor, 1 for secondaryColor
                 "layout(location=3) in float propertyValue1;\n", //[instanced]
                 "layout(location=4) in float propertyValue2;\n", //[instanced]
                 "layout(location=5) in vec3  center;\n",         //[instanced]
                 "layout(location=6) in mat3  rotation;\n",       //[instanced]

                 "uniform sampler1D colorScale;\n", // The texture containing the colormap
                 "uniform vec2 colorScaleRange;\n", // The minimum (x) and maximum (y) property values to map onto the
                                                    // colorscale
                 "uniform mat4 mvMatrix;\n",  // The model-view matrix, transforms coordinates from object to view space
                 "uniform mat4 mvpMatrix;\n", // The model-view-projection matrix, same as mvMatrix, but does projection
                                              // as well
                 "#ifndef PRIMARY_COLORMAP_ENABLED\n",
                 "uniform vec3 primaryColor;\n", // Primary solid color, for when no primary colormap is specified
                 "#endif\n", "#ifndef SECONDARY_COLORMAP_ENABLED\n",
                 "uniform vec3 secondaryColor;\n", // Secondary solid color, for when no secondary colormap is specified
                 "#endif\n",
                 "uniform float scale;\n",    // Global scale factor for beachballs
                 "uniform vec3 lightdir;\n",  // Lighting direction, in model space
                 "uniform vec4 clipPlane;\n", // Coefficients of the plane equation for the clip plane, in view space
                 "out vec4 vColor;\n",        // Computed vertex color output
                 "void main()\n", "{\n",
                 "  vec4 vertex = vec4(center + scale * (rotation * position), 1.0);\n", // Transform the vertex to
                                                                                         // model space, using the
                                                                                         // instanced parameters
                 "  gl_ClipDistance[0] = dot(clipPlane, mvMatrix * vertex);\n", // Necessary for the GPU to do proper
                                                                                // clipping of our beachballs, if
                                                                                // enabled
                 "  gl_Position = mvpMatrix * vertex;\n", // Transform the vertex position by the model-view-projection
                                                          // matrix
                 "#ifdef PRIMARY_COLORMAP_ENABLED\n",
                 "  float texcoord1 = (propertyValue1 - colorScaleRange.x) / (colorScaleRange.y - "
                 "colorScaleRange.x);\n",                                       // Normalize
                                                                                // property
                                                                                // value
                 "  vec3 primaryColor = texture(colorScale, texcoord1).xyz;\n", // Lookup color in colormap using
                                                                                // normalized property value as texture
                                                                                // coordinate
                 "#endif\n", "#ifdef SECONDARY_COLORMAP_ENABLED\n",
                 "  float texcoord2 = (propertyValue2 - colorScaleRange.x) / (colorScaleRange.y - "
                 "colorScaleRange.x);\n",
                 "  vec3 secondaryColor = texture(colorScale, texcoord2).xyz;\n", "#endif\n",
                 "  vec3 color = mix(primaryColor, secondaryColor, colorIndex);\n", // choose between primary and
                                                                                    // secondary color
                 "  vColor = vec4((.1 + .9 * max(0.0, dot(lightdir, rotation * normal))) * color, 1.0);\n", // apply
                                                                                                            // simple
                                                                                                            // lighting
                                                                                                            // to color
                 "}\n"};

// The fragment shader doesn't really do anything, other than assigning the interpolated vertex color to the output
const char *fragmentShaderSrc[] = {
    "#version 330 core\n",
    "in vec4 vColor;\n",
    "out vec4 fColor;\n",
    "void main()\n",
    "{\n",
    "  fColor = vColor;\n",
    "}\n",
};

namespace {
/**
 * Compile a shader from source
 * @param type The type of shader, either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 * @param src The shader source code
 * @param lineCount The length of the src array
 */
GLuint createShader(GLenum type, const char *src[], GLsizei lineCount) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, lineCount, src, 0);
  glCompileShader(shader);

  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE) {
    char buf[512];
    glGetShaderInfoLog(shader, sizeof(buf), 0, buf);

    OutputDebugString("Error compiling shader");
    OutputDebugString(buf);
  }

  return shader;
}

/**
 * Creating a shader program involves the following steps:
 *   - compile a vertex shader
 *   - compile a fragment shader
 *   - link the shaders to create a program
 *   - get the locations of the uniform parameters from the linked program,
 *     so we can use the corresponding glUniformXXX() functions to pass values
 *     to those parameters for rendering
 */
GLuint createProgram(int buildFlags) {
  // Based on the build flags, we modify some #define directives in the shader
  // source code, in order to build different programs. This feels a bit like
  // a hack, but GLSL has no other way to reuse different bits of code.
  vertexShaderSrc[1] = (buildFlags & Shader::PRIMARY_COLORMAP_ENABLED) ? "#define PRIMARY_COLORMAP_ENABLED\n" : "\n";
  vertexShaderSrc[2] =
      (buildFlags & Shader::SECONDARY_COLORMAP_ENABLED) ? "#define SECONDARY_COLORMAP_ENABLED\n" : "\n";

  // Work out the number of lines of code for the shaders
  GLsizei vsCount = sizeof(vertexShaderSrc) / sizeof(const char *);
  GLsizei fsCount = sizeof(fragmentShaderSrc) / sizeof(const char *);

  // Create both the vertex shader and the fragment shader
  GLuint vs = createShader(GL_VERTEX_SHADER, vertexShaderSrc, vsCount);
  GLuint fs = createShader(GL_FRAGMENT_SHADER, fragmentShaderSrc, fsCount);

  // Link to vertex and fragment shaders together to form a program
  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  // Check if everything went OK
  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE) {
    char buf[512];
    glGetProgramInfoLog(program, sizeof(buf), 0, buf);

    OutputDebugString("Error linking program");
    OutputDebugString(buf);
  }

  return program;
}
} // namespace

Shader::Shader()
    : program(0), colorScaleLocation(-1), colorScaleRangeLocation(-1), mvMatrixLocation(-1), mvpMatrixLocation(-1),
      primaryColorLocation(-1), secondaryColorLocation(-1), scaleLocation(-1), lightDirLocation(-1),
      clipPlaneLocation(-1) {}

void Shader::init(int buildFlags) {
  flags = buildFlags;

  program = createProgram(buildFlags);

  // Get the locations of all uniform parameters. These are the parameters that are
  // set once at the beginning of rendering a set of beachballs, and are the same
  // for the entire data set.
  colorScaleLocation = glGetUniformLocation(program, "colorScale");
  colorScaleRangeLocation = glGetUniformLocation(program, "colorScaleRange");
  mvMatrixLocation = glGetUniformLocation(program, "mvMatrix");
  mvpMatrixLocation = glGetUniformLocation(program, "mvpMatrix");
  primaryColorLocation = glGetUniformLocation(program, "primaryColor");
  secondaryColorLocation = glGetUniformLocation(program, "secondaryColor");
  scaleLocation = glGetUniformLocation(program, "scale");
  lightDirLocation = glGetUniformLocation(program, "lightdir");
  clipPlaneLocation = glGetUniformLocation(program, "clipPlane");

  // Get the locations of the vertex attributes
  int positionLocation = glGetAttribLocation(program, "position");
  int normalLocation = glGetAttribLocation(program, "normal");
  int colorLocation = glGetAttribLocation(program, "color");
  int centerLocation = glGetAttribLocation(program, "center");
}
