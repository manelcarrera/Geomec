#ifndef BEACHBALL_H_INCLUDED
#define BEACHBALL_H_INCLUDED

#include <MeshVizXLM/MbVec3.h>
#include <SbTypes.h>

#include <vector>

struct Vertex {
  MbVec3f position;
  MbVec3f normal;
  float colorIndex;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<unsigned short> indices;
};

Mesh generateBeachBall(float radius = 1.0f);

#endif
