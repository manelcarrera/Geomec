#include "stdafx.h"

#include "OIDISurfaceMeshNode.h"
#include "OpenInventorDataInterface.h"
#include "DecimatingCellFilter.h"
#include "OIDIMeshNodeManager.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoMaterial.h>

#include <MeshVizXLM/mesh/data/MiDataSetI.h>

#include <MeshVizXLM/mapping/nodes/MoMesh.h>

#include <MeshVizXLM/mapping/nodes/MoMeshSurface.h>

#include <MeshVizXLM/mapping/nodes/MoScalarSetI.h>

#include <MeshVizXLM/mapping/nodes/MoMeshAnnotatedIsoline.h>

#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>

#include <MeshVizXLM/MxTimeStamp.h>
#include "MeshNodeSettings.h"

// The cell shape appearance node contains a draw style and a material node.
void OIDISurfaceMeshNode::buildSurfaceAppearanceNode()
{
  // The draw style specifies which components to draw (faces / edges / points)
  m_surfaceDrawStyle = new MoDrawStyle;
  m_surfaceDrawStyle->displayFaces = true;
  m_surfaceDrawStyle->displayEdges = true; // draw the edges of the triangles
  m_surfaceDrawStyle->displayPoints = false;

  // MoMaterial defines the material of surface representations
  // to make point set meshes better visible
  m_soSurfaceMaterial =(isPointSetMeshNode()) ? new SoMaterial : 0;
  
  m_surfaceMaterial = new MoMaterial;
  // m_surfaceMaterial->enhancedColoring = TRUE;

  // CONTOURING means that rendering uses the color mapping defined by a scalar 
  // set and a color mapping function
  m_surfaceMaterial->faceColoring = MoMaterial::CONTOURING;
  // Use a solid color for rendering the edges
  m_surfaceMaterial->lineColoring = MoMaterial::COLOR;
  m_surfaceMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f); // black;
  m_surfaceMaterial->transparency = .0f;

  SoDrawStyle * pointDrawStyle = new SoDrawStyle;
  pointDrawStyle->style = SoDrawStyle::POINTS;
  pointDrawStyle->pointSize = 6;
   
  // Group both draw style and material together
  m_surfaceAppearance = new SoGroup;
  m_surfaceAppearance->addChild(pointDrawStyle);
  m_surfaceAppearance->addChild(m_surfaceDrawStyle);
  if (isPointSetMeshNode()) m_surfaceAppearance->addChild(m_soSurfaceMaterial);
  m_surfaceAppearance->addChild(m_surfaceMaterial);
}



void OIDISurfaceMeshNode::buildSurfaceNode()
{
  // Add a MoMeshCellShape node to visualize the all cells in the mesh.
  m_surface = new MoMeshSurface;

  buildSurfaceAppearanceNode();

  // Put the cell shape node under a separator with the given cellShapeAppearance, so 
  // the cellShapeAppearance won't affect other visualization nodes 
  m_surfaceSeparator = new SoSeparator;
  m_surfaceSeparator->addChild(m_surfaceAppearance);
  m_surfaceSeparator->addChild(m_surface);

  // Add the cell shape node to a switch, so we can disable it easily
  m_surfaceSwitch = new SoSwitch;
  m_surfaceSwitch->addChild(m_surfaceSeparator);
  m_surfaceSwitch->whichChild = SO_SWITCH_ALL;

  buildIsolineNode();
  m_surfaceSwitch->addChild(m_isolineSwitch);
}


// Builds a node containing the mesh data, a cell shape rendering node, a dummy property set
// and a color map for the properties
void OIDISurfaceMeshNode::buildSurfaceMeshNode()
{
  buildMeshNode();
  buildSurfaceNode();
  buildTensorsNode();
  buildVectorsNode();

  // Make all these nodes children of this one
  addChild(m_surfaceSwitch);
}


// Build a partial scene graph for a OIDIMeshSU instance. The returned node
// can then be added to the objects group.
void OIDISurfaceMeshNode::buildNodes()
{
  buildSurfaceMeshNode();
}

/**
* Constructor
*/
OIDISurfaceMeshNode::OIDISurfaceMeshNode(const OIDIMesh* meshData, OIDIMeshNodeManager * meshNodeManager)
  : OIDIMeshNode(meshData, meshNodeManager)
  , m_surfaceSwitch(0)
  , m_surfaceSeparator(0)
  , m_surfaceAppearance(0)
  , m_surfaceDrawStyle(0)
  , m_surfaceMaterial(0)
  , m_surface(0)
  , m_isoline(0)
  , m_isolineSwitch(0)
{
  buildNodes();
}


void OIDISurfaceMeshNode::setIsoValues(int isoCount, double min, double max)
{
  std::vector<float> isovalues(isoCount);
  computeIsovalues(isovalues, min, max);
  m_isoline->isovalues.setValue(0.0);
  m_isoline->isovalues.setValues(0, isoCount, &isovalues[0]);
}

void OIDISurfaceMeshNode::setIsoGap(double gap)
{
  m_isoline->gap = gap;
}

void OIDISurfaceMeshNode::UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor)
{
  OIDIMeshNode::UpdateDisplaySettings(meshNodeSettings, showColor);

  if (!isVisible())
  {
    m_surfaceSwitch->whichChild = SO_SWITCH_NONE;
    m_surfaceMaterial->transparency = 1.0f;
    m_isolineSwitch->whichChild = SO_SWITCH_NONE;
  }
  else if (getMesh()->meshType() == OIDIMesh::PointSetMesh)
  {
    m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
    m_surfaceDrawStyle->displayEdges = false;
    m_surfaceDrawStyle->displayFaces = false;
    m_surfaceDrawStyle->displayPoints = true;
    m_surfaceMaterial->pointColoring = MoMaterial::CONTOURING;
    m_surfaceMaterial->pointColor = m_geologyColor;
    m_isoline->colorScalarSetId = -1; 

  }
  else if (getMesh()->meshType() == OIDIMesh::PointMesh)
  {
    m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
    m_surfaceDrawStyle->displayEdges = false;
    m_surfaceDrawStyle->displayFaces = true;
    m_surfaceDrawStyle->displayPoints = true;
    m_surfaceMaterial->pointColoring = MoMaterial::CONTOURING;
    m_surfaceMaterial->pointColor = m_geologyColor;
    m_isoline->colorScalarSetId = -1; 
  }
  else
  {
    m_surfaceMaterial->transparency = meshNodeSettings.transparency / 100.0f;
    m_isolineSwitch->whichChild = showIsoLines(meshNodeSettings) ? SO_SWITCH_ALL : SO_SWITCH_NONE;

    if (meshNodeSettings.meshFilled)
    {
      m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
      m_surfaceDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed && !showIsoLines(meshNodeSettings); // draw the edges of the triangles
      m_surfaceDrawStyle->displayFaces = true;
      m_surfaceDrawStyle->displayPoints = false;
      m_surfaceMaterial->lineColoring = MoMaterial::COLOR;
      m_surfaceMaterial->lineColor = SbColor (0.0f, 0.0f, 0.0f);
      m_isoline->colorScalarSetId = -1; 
    }
    else
    {
      m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
      m_surfaceDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed && !showIsoLines(meshNodeSettings); // draw the edges of the triangles
      m_surfaceDrawStyle->displayFaces = false;
      m_surfaceDrawStyle->displayPoints = false;
      m_surfaceMaterial->lineColoring = MoMaterial::CONTOURING;
      m_surfaceMaterial->lineColor = m_geologyColor;
      m_isoline->colorScalarSetId = m_colorScalarSetId;
    }
  }
}

void OIDISurfaceMeshNode::updateColorScalarSet()
{
     m_surface->colorScalarSetId = m_colorScalarSetId;
}

void OIDISurfaceMeshNode::updateIsoScalarSet()
{
  m_isoline->isoScalarSetId = m_isoScalarSetId;
}


void OIDISurfaceMeshNode::updateGeologyColors()
{
  m_surfaceMaterial->faceColor = m_geologyColor;
  if (m_soSurfaceMaterial)
  {
    // m_soSurfaceMaterial->ambientColor = m_geologyColor;
    // m_soSurfaceMaterial->diffuseColor = m_geologyColor;
    // m_soSurfaceMaterial->specularColor = m_geologyColor;
    m_soSurfaceMaterial->emissiveColor = m_geologyColor;
    // m_soSurfaceMaterial->reflectiveColor = m_geologyColor;
  }
  m_surfaceMaterial->lineColor = m_geologyColor;
}

void OIDISurfaceMeshNode::buildIsolineNode()
{
  m_isolineSwitch = new SoSwitch;
  m_isolineSwitch->setName("IsoLineSwitch");
  m_isolineSwitch->whichChild = SO_SWITCH_NONE;

  m_isoline = CreateIsoline();
  m_isolineSwitch->addChild(m_isoline);

}

bool OIDISurfaceMeshNode::isPointSetMeshNode()
{
  return (dynamic_cast<const OIDIPointSetMesh *> (getMesh()) !=  0);
}




