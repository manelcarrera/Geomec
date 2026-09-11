#include "stdafx.h"
#include "BeachBall.h"
#include "MoMeshTensor.h"
#include "MoTensorSetElement.h"

#include <Inventor/sys/SoGL.h>

#include <MeshVizXLM/mapping/elements/MoMeshElement.h>
#include <MeshVizXLM/mapping/elements/MoCellFilterElement.h>
#include <MeshVizXLM/mapping/elements/MoColorMappingElement.h>
#include <MeshVizXLM/mapping/elements/MoScalarSetElementI.h>
#include <MeshVizXLM/mapping/interfaces/MiColorMapping.h>
#include <MeshVizXLM/mesh/MiVolumeMeshUnstructured.h>
#include <MeshVizXLM/mesh/geometry/MiGeometryI.h>
#include <MeshVizXLM/mesh/topology/MiTopologyExplicitI.h>
#include <MeshVizXLM/mesh/cell/MiCellFilterI.h>

#include <algorithm>

namespace
{
    // Calculates the bounding box of the given mesh
    void getGeometryMinMax(const MiMeshUnstructuredI* mesh, MbVec3d& minVec, MbVec3d& maxVec)
    {
        const MiGeometryI& geometry = mesh->getGeometry();
        const MiTopologyExplicitI& topology = mesh->getTopology();

        minVec = MbVec3d::numeric_limit_max();
        maxVec = -minVec;

        size_t begin = topology.getBeginNodeId();
        size_t end = topology.getEndNodeId();
        for(size_t i=begin; i < end; ++i)
        {
            MbVec3d v = geometry.getCoord(i);
            for(int j=0; j < 3; ++j)
            {
                minVec[j] = std::min(minVec[j], v[j]);
                maxVec[j] = std::max(maxVec[j], v[j]);
            }
        }
    }

    // Constructs an OpenGL 1D texture for the given colormap, to be used in the shader
    GLuint generateTextureFromColorMap(const MiColorMapping<double, SbColorRGBA>* colorMap)
    {
        const int width = 512;
        uint32_t colors[width];

        double rangeMin, rangeMax;
        colorMap->getRange(rangeMin, rangeMax);
        for(int i=0; i < width; ++i)
        {
            double val = rangeMin + i * (rangeMax - rangeMin) / width;
            // oiv: 0xrrggbbaa
            // GL:  0xaabbggrr
            uint32_t colorRGBA = colorMap->getColor(val).getPackedValue();
            uint32_t colorABGR = 
                ((colorRGBA & 0x000000ff) << 24) |
                ((colorRGBA & 0x0000ff00) <<  8) |
                ((colorRGBA & 0x00ff0000) >>  8) |
                ((colorRGBA & 0xff000000) >> 24);
            colors[i] = colorABGR;
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)colors);

        return tex;
    }

  // Get a list of unique node indices for the given topology and cellfilter. When rendering per-node, care must be taken
  // to render only a single beachball on each node, even when it's shared between several cells. The cell filter must also
  // be taken into account so the "decimate" slider works for beachballs as well
  void getActiveNodeIndices(const MiTopologyExplicitI& topology, const MiCellFilterI* cellFilter, std::vector<size_t>& nodeIndices)
  {
    nodeIndices.clear(); // just in case

    // First gather all the node indices for which to render the tensors.
    size_t nc = topology.getNumCells();
    for(size_t i=0; i < nc; ++i)
    {
      if(cellFilter == 0 || cellFilter->acceptCell(i))
      {
        const MiCell* cell = topology.getCell(i);
        size_t n = cell->getNumNodes();
        for(size_t j=0; j < n; ++j)
          nodeIndices.push_back(cell->getNodeIndex(j));
      }
    }  

    // Since nodes are shared between cells, the list will probably contain duplicates. These
    // are filtered out here.
    std::sort(nodeIndices.begin(), nodeIndices.end(), std::less<size_t>());
    std::vector<size_t>::iterator newEnd = std::unique(nodeIndices.begin(), nodeIndices.end());
    nodeIndices.erase(newEnd, nodeIndices.end());
  }

  // Create rotation matrix from principal directions
  SbMatrix3 getRotationMatrix(const CTensor& tensor)
  {
    // Build a rotation matrix for the beachball based on the principal directions of the tensor.
    // tensor.GetPrincipalDir() uses tensor.Matrix().EigenVectors(false). Changing the parameter
    // to true causes the EigenVectors() function to ensure that it returns the vectors as a 
    // right-handed system, which is what we need for the rotation matrix
    geo::CMatrix pdirs = tensor.Matrix().EigenVectors(true);

    // The geometry for an individual beachball is built with the following orientation
    //   x-axis: colorIndex 0 ==> min principal direction
    //   y-axis: colorIndex 1 ==> med principal direction
    //   z-axis: top          ==> max principal direction
    // The corresponding rotation matrix can then be built by using the principal direction vectors
    // as the columns of the matrix:
    // 
    //     minX    medX    maxX
    //     minY    medY    maxY
    //     minZ    medZ    maxZ
    //
    // The eigenvectors of the tensor matrix are returned sorted from the smallest to the largest
    // corresponding eigenvalue, so the rotation matrix looks like this (SbMatrix3 stores the values
    // in column-major order):
	  SbMatrix3 rotationMatrix = SbMatrix3(
		  (float)pdirs.Value(0, 0), (float)pdirs.Value(1, 0), (float)pdirs.Value(2, 0),
		  (float)pdirs.Value(0, 1), (float)pdirs.Value(1, 1), (float)pdirs.Value(2, 1),
		  (float)pdirs.Value(0, 2), (float)pdirs.Value(1, 2), (float)pdirs.Value(2, 2));

	  return rotationMatrix;
  }

    /**
    * Extract the necessary parameters for instancing from the OIV components. These include the
    * position, orientation, and (if applicable) the property values for each visible beachball
    */
    void generateInstancedParams(
        const MiMeshUnstructuredI& mesh, // the mesh on which to display beachballs
        const MiTensorSetI& tensorSet,   // the tensor set to visualize
        const MiScalardSetI* scalarSet1, // first scalar set for coloring, may be NULL
        const MiScalardSetI* scalarSet2, // second scalar set for coloring, may be NULL
        const MiCellFilterI* cellFilter, // may be NULL
        std::vector<VBOMesh::InstancedData>& instancedData) // Array that will be filled with instanced data
    {
        instancedData.clear();

        const MiGeometryI& geometry = mesh.getGeometry();
        const MiTopologyExplicitI& topology = mesh.getTopology();

        if(tensorSet.getBinding() == MiDataSet::PER_NODE)
        {
          // When extracting the parameters for PER_NODE mapping, the nodes for all visible cells need
          // to be collected, and duplicates removed. This is what getActiveNodeIndices() does.
          std::vector<size_t> nodeIndices;
          getActiveNodeIndices(topology, cellFilter, nodeIndices);

          instancedData.reserve(nodeIndices.size());

          for(size_t i=0; i < nodeIndices.size(); ++i)
          {
            size_t nodeIndex = nodeIndices[i];
            MbVec3d pos = geometry.getCoord(nodeIndex);

            VBOMesh::InstancedData data;
            data.position = SbVec3f((float)pos[0], (float)pos[1], (float)pos[2]);
            data.rotation = getRotationMatrix(tensorSet.get(nodeIndex));
            data.propertyValue1 = (scalarSet1 == 0) ? 0.0f : (float)scalarSet1->get(nodeIndex);
            data.propertyValue2 = (scalarSet2 == 0) ? 0.0f : (float)scalarSet2->get(nodeIndex);

            instancedData.push_back(data);
          }
        }
        else // extract values PER_CELL
        {
          size_t nc = topology.getNumCells();
          for(size_t i=0; i < nc; ++i)
          {
            if(cellFilter == 0 || cellFilter->acceptCell(i))
            {
              const MiCell* cell = topology.getCell(i);
              // For PER_CELL mapping, place beachball in the cell center
              MbVec3d center = cell->getCenter(geometry);

              VBOMesh::InstancedData data;
              data.position = SbVec3f((float)center[0], (float)center[1], (float)center[2]);
              data.rotation = getRotationMatrix(tensorSet.get(i));
              data.propertyValue1 = (scalarSet1 == 0) ? 0.0f : (float)scalarSet1->get(i);
              data.propertyValue2 = (scalarSet1 == 0) ? 0.0f : (float)scalarSet2->get(i);

              instancedData.push_back(data);
            }
          }
        }
  }

  /**
   * Creates the VBOMesh for a tensor set. This includes the geometry for a single beachball, as well as the 
   * instance parameters for all visible beachballs.
   * \param mesh       The MeshViz unstructured mesh interface giving us the geometry and topology on which to map the tensors
   * \param cellFilter The cell filter that is in use, or NULL if none
   * \param tensors    The tensor dataset
   * \param scalars1   The primary scalar set (may be NULL)
   * \param scalars2   The secondary scalar set (may be NULL)
   * \param tensorMesh Mesh object containing the geometry to be used for a single tensor (beachball)
   */
  VBOMesh generateInstancedTensorsMesh(
      const MiMeshUnstructuredI* mesh, 
      const MiCellFilterI* cellFilter, 
      const MiTensorSetI& tensors, 
      const MiScalardSetI* scalars1,
      const MiScalardSetI* scalars2,
      const Mesh& tensorMesh)
  {
    glBindVertexArray(0);

    size_t vbSize = tensorMesh.vertices.size() * sizeof(Vertex); // Size in bytes of the vertex buffer
    size_t ibSize = tensorMesh.indices.size() * sizeof(unsigned short); // Size in bytes of the index buffer

    GLGETERROR();

    // Generate 3 buffer objects: 
    //   - one for the vertex data
    //   - one for the indices
    //   - one for the instanced data
    GLuint buffers[3];
    glGenBuffers(3, buffers);

    // Set up the vertex buffer object with the geometry for a beachball
    GLuint vertexBuffer = buffers[0];
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vbSize, &tensorMesh.vertices[0], GL_STATIC_DRAW);
    GLGETERROR();

    // Set up the index buffer object for a beachball
    GLuint indexBuffer = buffers[1];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibSize, &tensorMesh.indices[0], GL_STATIC_DRAW);
    GLGETERROR();

    // Setup instance data for all beachballs (position, orientation, and property values if any)
    std::vector<VBOMesh::InstancedData> instancedData;
    generateInstancedParams(*mesh, tensors, scalars1, scalars2, cellFilter, instancedData);

    size_t vbSizeInst = instancedData.size() * sizeof(VBOMesh::InstancedData); // size in bytes of instance data VBO

    // Create the VBO for instancing data
    GLuint instancedDataBuffer = buffers[2];
    glBindBuffer(GL_ARRAY_BUFFER, instancedDataBuffer);
    glBufferData(GL_ARRAY_BUFFER, vbSizeInst, &instancedData[0], GL_STATIC_DRAW);
    GLGETERROR();

    // Reset buffer bindings
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLGETERROR();

    // Store results in VBOMesh
    VBOMesh result(vertexBuffer, indexBuffer, GL_UNSIGNED_SHORT, (GLsizei)tensorMesh.indices.size());
    result.setInstancedParams(instancedDataBuffer, instancedData.size());

    return result;
  }
}

SO_NODE_SOURCE(MoMeshTensor);

const MiMesh* MoMeshTensor::getExtractedMesh(MeshType& meshType)
{
  return 0; // Not creating any new mesh
}

void MoMeshTensor::doExtract(SoAction* action)
{
}

/**
 * This is where the actual rendering of the beachballs takes place
 */
void MoMeshTensor::GLRender(SoGLRenderAction *action)
{
  SoState* state = action->getState();

  GLGETERROR();

  // Get tensor data set from the scenegraph state
  int id = tensorSetId.getValue();
  const MiTensorSetI* tensors = MoTensorSetElement::get(state, id);
  if(tensors == 0)
    return;

  // Get mesh from the state
  size_t meshTimeStamp;
  MeshType meshType;
  const MiMesh* mesh = MoMeshElement::getMesh(state, meshType, meshTimeStamp);

  // Get cell filter from the state
  CellFilterType filterType;
  const MiCellFilter* cellFilter = MoCellFilterElement::getCellFilter(state, filterType);
  const MiCellFilterI* cellFilterI = 0;
  if(filterType == CELL_FILTER_I)
    cellFilterI = dynamic_cast<const MiCellFilterI*>(cellFilter);

  // Get scalarsets from the state
  const MiScalardSetI* primaryScalarSet = 0;
  const MiScalardSetI* secondaryScalarSet = 0;
  int scalarSetId1 = colorScalarSetId.getValue();
  int scalarSetId2 = secondaryColorScalarSetId.getValue();
  if(scalarSetId1 >= 0)
      primaryScalarSet = MoScalarSetElementI::get(state, scalarSetId1);
  if(scalarSetId2 >= 0)
      secondaryScalarSet = MoScalarSetElementI::get(state, scalarSetId2);

  if(primaryScalarSet != 0 || secondaryScalarSet != 0)
  {
      // If the user has setup rendering to use scalar sets and a colormap, then a texture must be
      // created to store the colormap, in order for the shader to correctly apply the colors
      const MiColorMapping<double, SbColorRGBA>* colorMap = MoColorMappingElement::getColorMapping(state);
      if(colorMap != 0 && colorMap->getTimeStamp() != m_colorMapTimestamp) // need to refresh?
      {
          // Make sure to cleanup any previous colormap texture
          if(m_colorMapTexture != 0)
              glDeleteTextures(1, &m_colorMapTexture);

          m_colorMapTexture = generateTextureFromColorMap(colorMap);
          m_colorMapTimestamp = colorMap->getTimeStamp();

          // The range of the colormap also needs to be passed to the shader, so property values
          // can be converted to normalized texture coordinates in the range 0.0 - 1.0
          double rangeMin, rangeMax;
          colorMap->getRange(rangeMin, rangeMax);
          m_colorMapRangeMin = (float)rangeMin;
          m_colorMapRangeMax = (float)rangeMax;
      }
  }

  // Each beachball has 2 colors; a primary and a secondary color. They can be separately setup to be either a
  // solid color, or a lookup in the colormap based on a property value. This leaves us with 4 possible combinations.
  // In order to eliminate dynamic branching in the shader (which can be quite expensive on a lot of graphics hardware),
  // the preprocessor is used to build different programs for all combinations, using the shader build flags.
  int shaderBuildFlags = 
      ((primaryScalarSet   != 0) ? Shader::PRIMARY_COLORMAP_ENABLED   : 0) |
      ((secondaryScalarSet != 0) ? Shader::SECONDARY_COLORMAP_ENABLED : 0);

  // This only works for unstructured volume and surface meshes
  if(meshType == VOLUME_MESH_UNSTRUCTURED || meshType == SURFACE_MESH_UNSTRUCTURED)
  {
    const MiMeshUnstructuredI* meshUnstructured = dynamic_cast<const MiMeshUnstructuredI*>(mesh);
    const MiGeometryI& geometry = meshUnstructured->getGeometry();
    const MiTopologyExplicitI& topology = meshUnstructured->getTopology();

    // Check timestamps to see if any updating needs to be done
    size_t currentTopologyTimestamp = topology.getTimeStamp();
    size_t currentGeometryTimestamp = geometry.getTimeStamp();
    size_t currentFilterTimestamp = (cellFilterI == 0) ? 0 : cellFilterI->getTimeStamp();
    size_t currentScalarSet1Timestamp = (primaryScalarSet == 0) ? 0 : primaryScalarSet->getTimeStamp();
    size_t currentScalarSet2Timestamp = (secondaryScalarSet == 0) ? 0 : secondaryScalarSet->getTimeStamp();
    size_t currentTensorTimestamp = tensors->getTimeStamp();

    if(
      (currentTopologyTimestamp != m_topologyTimestamp) ||
      (currentGeometryTimestamp != m_geometryTimestamp) ||
      (currentFilterTimestamp != m_cellFilterTimestamp) ||
      (currentScalarSet1Timestamp != m_scalarSet1Timestamp) ||
      (currentScalarSet2Timestamp != m_scalarSet2Timestamp) ||
      (currentTensorTimestamp != m_tensorsTimestamp))
    {
      m_topologyTimestamp = currentTopologyTimestamp;
      m_geometryTimestamp = currentGeometryTimestamp;
      m_cellFilterTimestamp = currentFilterTimestamp;
      m_scalarSet1Timestamp = currentScalarSet1Timestamp;
      m_scalarSet2Timestamp = currentScalarSet2Timestamp;
      m_tensorsTimestamp = currentTensorTimestamp;

      // Compile the shader for the given build flags. TODO: only do this when build flags
      // have actually changed. However, since we only get here when the user changes 
      // something in the UI, it's not much of a problem to do it everytime.
      if(m_shaders[shaderBuildFlags].program == 0)
        m_shaders[shaderBuildFlags].init(shaderBuildFlags);

      // Cleanup the current mesh if necessary
      if(m_vboMesh.valid())
        m_vboMesh.free();

      // Generate a new VBOMesh
      m_vboMesh = generateInstancedTensorsMesh(
          meshUnstructured, 
          cellFilterI, 
          *tensors, 
          primaryScalarSet, 
          secondaryScalarSet,
          m_mesh);
    }

    // Set some OpenGL state to prevent other OIV stuff from interfering with our rendering
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_STIPPLE); // because of MoMaterial::transparency 
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Choose shader program based on current build flags
    Shader& shader = m_shaders[shaderBuildFlags];

    // Get the currently active shader, so we can restore it afterwards
    GLuint prevProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&prevProgram);
    glUseProgram(shader.program);
    GLGETERROR();

    // Because we're using a shader program to render the beachballs, we need to do all the
    // transforms & lighting ourselves. In order to do that, we need to retrieve some of the
    // current opengl state, such as the modelview and projection matrices, lighting direction,
    // and clipplane, and pass those to the shader as uniform parameters.

    // Get the current modelview and projection matrices from OpenGL. Also compute the inverse of
    // the modelview matrix, so we can use it later on to transform the lighting direction from
    // world space to object space. This allows us to do all lighting in object space, which is
    // more efficient than having to transform all normal vectors to world space and do the lighting
    // there.
    SbMatrix projectionMatrix, modelViewMatrix;
    glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix[0]);
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix[0]);
    SbMatrix mvpMatrix = modelViewMatrix * projectionMatrix;
    SbMatrix invModelViewMatrix = modelViewMatrix.inverse();
    GLGETERROR();

    // Get the plane equation for the current clipping plane (if any)
    double clipPlane[4];
    glGetClipPlane(GL_CLIP_PLANE0, clipPlane);

    // Get the light direction from OpenGL, so we can feed it to the shader.
    // OIV normally uses a directional light, so we'll go with that assumption here.
    float lightPos[4];
    glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    SbVec3f lightDir(lightPos[0], lightPos[1], lightPos[2]), localLightDir; // in view space
    invModelViewMatrix.multDirMatrix(lightDir, localLightDir); // transform to mesh local space
    GLGETERROR();

    // Setup texture unit 0 to use the colormap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, m_colorMapTexture);

    SbColorRGBA primaryColorValue = primaryColor.getValue();
    SbColorRGBA secondaryColorValue = secondaryColor.getValue();

    // Pass the parameters to the shader
    glUniformMatrix4fv(shader.mvMatrixLocation, 1, false, modelViewMatrix[0]);
    glUniformMatrix4fv(shader.mvpMatrixLocation, 1, false, mvpMatrix[0]);
    glUniform1i(shader.colorScaleLocation, 0);
    glUniform2f(shader.colorScaleRangeLocation, m_colorMapRangeMin, m_colorMapRangeMax);
    glUniform3f(shader.primaryColorLocation, primaryColorValue[0], primaryColorValue[1], primaryColorValue[2]);
    glUniform3f(shader.secondaryColorLocation, secondaryColorValue[0], secondaryColorValue[1], secondaryColorValue[2]);
    glUniform1f(shader.scaleLocation, scaleFactor.getValue());
    glUniform3fv(shader.lightDirLocation, 1, localLightDir.getValue());
    glUniform4f(shader.clipPlaneLocation, (float)clipPlane[0], (float)clipPlane[1], (float)clipPlane[2], (float)clipPlane[3]);
    GLGETERROR();

    // Finally, do the actual rendering
    m_vboMesh.render();
    GLGETERROR();

    // restore state
    glUseProgram(prevProgram);
    GLGETERROR();
  }
}

void MoMeshTensor::getBoundingBox(SoGetBoundingBoxAction* action)
{
  // Note that this function gets the bounding box for the complete geometry
  // used, not taking into account cell filters, or the fact that the topology
  // might only use part of the available geometry.
  SoState* state = action->getState();

  // Get mesh from the state
  size_t meshTimeStamp;
  MeshType meshType;
  const MiMesh* mesh = MoMeshElement::getMesh(state, meshType, meshTimeStamp);

  if(meshType == VOLUME_MESH_UNSTRUCTURED || meshType == SURFACE_MESH_UNSTRUCTURED)
  {
    const MiMeshUnstructuredI* meshUnstructured = dynamic_cast<const MiMeshUnstructuredI*>(mesh);
    const MiGeometryI& geometry = meshUnstructured->getGeometry();
    const MiTopologyExplicitI& topology = meshUnstructured->getTopology();
    // See if we need to update the min- and max vectors. This only needs to happen when either
    // the geometry or the topology has changed. Check this by looking at the timestamps.
    if(
        m_bboxTopologyTimestamp != topology.getTimeStamp() ||
        m_bboxGeometryTimestamp != geometry.getTimeStamp())
    {
        getGeometryMinMax(meshUnstructured, m_minVec, m_maxVec);
        m_bboxTopologyTimestamp = topology.getTimeStamp();
        m_bboxGeometryTimestamp = geometry.getTimeStamp();
    }

    float delta = scaleFactor.getValue();

    SbBox3f& box = action->getXfBoundingBox();
    box.setBounds(
      (float)m_minVec[0] - delta, (float)m_minVec[1] - delta, (float)m_minVec[2] - delta,
      (float)m_maxVec[0] + delta, (float)m_maxVec[1] + delta, (float)m_maxVec[2] + delta);
  }
}

/*static*/ void MoMeshTensor::initClass()
{
  SO_NODE_INIT_CLASS(MoMeshTensor, MoMeshRepresentation, "MoMeshRepresentation");
}

/*static*/ void MoMeshTensor::exitClass()
{
  SO__NODE_EXIT_CLASS(MoMeshTensor);
}

MoMeshTensor::MoMeshTensor()
  : m_topologyTimestamp(0)
  , m_geometryTimestamp(0)
  , m_tensorsTimestamp(0)
  , m_cellFilterTimestamp(0)
  , m_scalarSet1Timestamp(0)
  , m_scalarSet2Timestamp(0)
  , m_colorMapTimestamp(0)
  , m_bboxTopologyTimestamp(0)
  , m_bboxGeometryTimestamp(0)
  , m_colorMapRangeMin(0.0f)
  , m_colorMapRangeMax(1.0f)
  , m_minVec( MbVec3d::numeric_limit_max())
  , m_maxVec(-MbVec3d::numeric_limit_max())
  , m_positionsVBO(0)
  , m_positionsCount(0)
  , m_colorMapTexture(0)
{
  SO_NODE_CONSTRUCTOR(MoMeshTensor);

  SO_NODE_ADD_FIELD(tensorSetId, (-1));
  SO_NODE_ADD_FIELD(secondaryColorScalarSetId, (-1));

  SO_NODE_ADD_FIELD(primaryColor, (1.0f, 0.0f, 0.0f, 1.0f)); // red
  SO_NODE_ADD_FIELD(secondaryColor, (1.0f, 1.0f, 1.0f, 1.0f)); // white
  SO_NODE_ADD_FIELD(scaleFactor, (1.0f));

  SO_ENABLE(SoGLRenderAction, MoTensorSetElement);
  SO_ENABLE(SoGLRenderAction, MoColorMappingElement);

  m_mesh = generateBeachBall();
}

MoMeshTensor::~MoMeshTensor()
{
    if(m_colorMapTexture != 0)
        glDeleteTextures(1, &m_colorMapTexture);
}
