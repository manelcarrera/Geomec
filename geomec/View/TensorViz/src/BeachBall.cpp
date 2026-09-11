#include "stdafx.h"
#include "BeachBall.h"

#include <Inventor/sys/SoGL.h>

/**
 * Generates a mesh for a beach ball with certain radius
 */
Mesh generateBeachBall(float radius)
{
  // The mesh is divided into 4 parts: 2 red ones, and 2 white ones
  int nparts = 4;

  // m and n determine how many triangles are used to approximate the geometry of a sphere. m is the number
  // of horizontal subdivisions of a single part, and n is the number of vertical subdivisions. In order to 
  // get a smoother mesh, increase the value of m. Note that this will have a direct impact on the performance
  // of the rendering, since the number of beachballs to be displayed can be quite high.
  int m = 4;
  int n = 2*m;

  Mesh result;
  size_t vertexCount = (n + 1) * nparts * (m + 1);

  // This sets up the geometry as follows: the x-axis gets colorIndex 0, the y-axis 
  // gets colorIndex 1, and the z-axis goes through the top.
  for(int i=0; i < nparts; ++i)
  {
  for(int j=0; j <= m; ++j)
  {
      float angle1 = (i * m + j) * 2 * (float)M_PI / (nparts * m) - (float)M_PI / 4;
      float x = cosf(angle1);
      float y = sinf(angle1);

      for(int k=0; k <= n; ++k)
      {
    float angle2 = k * (float)M_PI / n;

    float z = -cosf(angle2);
    float r =  sinf(angle2);

    // Components of the normal vector
    float nx = r * x;
    float ny = r * y;
    float nz = z;

    Vertex v;
    v.position   = MbVec3f(nx, ny, nz) * radius;
    v.normal     = MbVec3f(nx, ny, nz);
    v.colorIndex = (float)(i % 2);

    result.vertices.push_back(v);
      }
  }
  }

  // Set up the index buffer
  int ntriangles = 2 * n * m * nparts;
  size_t indexCount = 3 * ntriangles;

  // Use the same access pattern for every quad (2 triangles --> 6 indices)
  int templ[] = { 0, n, n+1, 0, n+1, 1 };

  int vertsPerPart = (n+1) * (m+1);
  for(int i=0; i < nparts; ++i)
  {
  for(int j=0; j < m; ++j)
  {
      for(int k=0; k < n; ++k)
      {
    int base = i * vertsPerPart + j * (n+1) + k;
    for(int q=0; q < 6; ++q)
          result.indices.push_back((unsigned short)(base + templ[q]));
      }
  }
  }

  return result;
}
