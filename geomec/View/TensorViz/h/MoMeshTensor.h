#ifndef MOMESHTENSOR_H_INCLUDED
#define MOMESHTENSOR_H_INCLUDED

#include "BeachBall.h"
#include "VBOMesh.h"
#include "Shader.h"

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFColorRGBA.h>

#include <MeshVizXLM/mapping/nodes/MoMeshRepresentation.h>

class SoBufferedShape;
struct Mesh;

/**
 * The MoMeshTensor class is used to display tensors on a mesh, visualized as 'beachballs'. The
 * orientation of the beachballs is determined by the principal directions of the tensor. The top
 * of the beachball is oriented along the maximum principal direction, and the other 2 directions
 * can be distinguishes by coloring them: either with a single solid color, or using a scalar set.
 * 2 scalar sets can be defined, using the colorScalarSetId (primary) and the secondaryScalarSetId.
 * The primary color is used for the minimum principal direction, the secondary color for the medium
 * principal direction. When a scalar set is specified for a direction, the current colormap is used
 * for coloring the corresponding part of the beachball. When no scalar set is defined, the single 
 * color as set in the primaryColor or secondaryColor fields is used.
 */
class MoMeshTensor : public MoMeshRepresentation
{
  SO_NODE_HEADER(MoMeshTensor);

  size_t  m_topologyTimestamp;
  size_t  m_geometryTimestamp;
  size_t  m_tensorsTimestamp;
  size_t  m_cellFilterTimestamp;
  size_t  m_scalarSet1Timestamp;
  size_t  m_scalarSet2Timestamp;
  size_t  m_colorMapTimestamp;
    
  size_t  m_bboxTopologyTimestamp;
  size_t  m_bboxGeometryTimestamp;

  float   m_colorMapRangeMin;
  float   m_colorMapRangeMax;

  MbVec3d m_minVec;
  MbVec3d m_maxVec;

  Mesh    m_mesh;
  VBOMesh m_vboMesh;
  GLuint  m_positionsVBO;
  size_t  m_positionsCount;
  GLuint  m_colorMapTexture;

  Shader  m_shaders[4];

  virtual const MiMesh* getExtractedMesh(MeshType& meshType);

  virtual void doExtract(SoAction* action);

protected:

  virtual void GLRender(SoGLRenderAction *action);

  virtual void getBoundingBox(SoGetBoundingBoxAction* action);

public:

  static void initClass();
  static void exitClass();

  MoMeshTensor();
  ~MoMeshTensor();

  SoSFInt32 tensorSetId;

  // The colorScalarSetId field inherited from MoMeshRepresentation is
  // used as the 'primary' colorScalarSetId
  SoSFInt32 secondaryColorScalarSetId;

  SoSFColorRGBA primaryColor;
  SoSFColorRGBA secondaryColor;

  SoSFFloat scaleFactor;
};


#endif
