#include "stdafx.h"

#include "DecimatingCellFilter.h"
#include "OIDILineMeshNode.h"
#include "OIDIMeshNodeManager.h"
#include "OpenInventorDataInterface.h"

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include <MeshVizXLM/mesh/data/MiDataSetI.h>

#include <MeshVizXLM/mapping/nodes/MoMesh.h>

#include <MeshVizXLM/mapping/nodes/MoMeshLine.h>
#include <MeshVizXLM/mapping/nodes/MoMeshSurface.h>
#include <MeshVizXLM/mapping/nodes/MoScalarSetI.h>

#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>
#include <MeshVizXLM/mapping/nodes/MoMaterial.h>

#include <MeshVizXLM/MxTimeStamp.h>

#include "MeshNodeSettings.h"

void OIDILineMeshNode::buildSurfaceNode() {
  m_outline = new MoMeshLine;
  m_outline->colorScalarSetId = -1;
  buildOutlineAppearanceNode();

  m_meshLineSeparator = new SoSeparator;
  m_meshLineSeparator->addChild(m_appearanceGroup);
  m_meshLineSeparator->addChild(m_outline);

  // Add the cell shape node to a switch, so we can disable it easily
  m_visibilitySwitch = new SoSwitch;

  m_visibilitySwitch->addChild(m_meshLineSeparator);
  m_visibilitySwitch->whichChild = SO_SWITCH_ALL;
}

// Builds a node containing the mesh data, a cell shape rendering node, a dummy property set
// and a color map for the properties
void OIDILineMeshNode::buildSurfaceMeshNode() {
  buildMeshNode();
  buildSurfaceNode();
  buildVectorsNode();

  // Make all these nodes children of this one
  addChild(m_visibilitySwitch);
}

// Build a partial scene graph for a OIDIMeshSU instance. The returned node
// can then be added to the objects group.
void OIDILineMeshNode::buildNodes() { buildSurfaceMeshNode(); }

/**
 * Constructor
 */
OIDILineMeshNode::OIDILineMeshNode(const OIDIMesh *meshData, OIDIMeshNodeManager *meshNodeManager)
    : OIDIMeshNode(meshData, meshNodeManager), m_visibilitySwitch(0), m_meshLineSeparator(0), m_appearanceGroup(0),
      m_meshLineDrawStyle(0), m_meshLineMaterial(0), m_outline(0) {
  buildNodes();
}

void OIDILineMeshNode::setIsoValues(int isoCount, double min, double max) {}

void OIDILineMeshNode::setIsoGap(double gap) {}

void OIDILineMeshNode::UpdateDisplaySettings(const MeshNodeSettings &meshNodeSettings, bool showColor) {
  OIDIMeshNode::UpdateDisplaySettings(meshNodeSettings, showColor);

  if (!isVisible()) {
    m_visibilitySwitch->whichChild = SO_SWITCH_NONE;
    m_meshLineMaterial->transparency = 1.0f;

  } else {
    m_visibilitySwitch->whichChild = SO_SWITCH_ALL;
    m_meshLineMaterial->transparency = meshNodeSettings.transparency / 100.0f;
  }
}

void OIDILineMeshNode::updateColorScalarSet() {}

void OIDILineMeshNode::updateIsoScalarSet() {}

void OIDILineMeshNode::updateGeologyColors() { m_meshLineMaterial->lineColor = m_geologyColor; }

void OIDILineMeshNode::buildOutlineAppearanceNode() {
  // The draw style specifies which components to draw (faces / edges / points)
  m_meshLineDrawStyle = new MoDrawStyle;
  m_meshLineDrawStyle->displayFaces = true;
  m_meshLineDrawStyle->displayEdges = true; // draw the edges of the triangles
  m_meshLineDrawStyle->displayPoints = true;

  // MoMaterial defines the material of outline representations
  m_meshLineMaterial = new MoMaterial;
  // m_meshLineMaterial->enhancedColoring = TRUE;

  // Use a solid color for rendering the edges
  m_meshLineMaterial->lineColoring = MoMaterial::COLOR;
  m_meshLineMaterial->lineColor = SbColor(1.0f, 1.0f, 1.0f); // white;
  m_meshLineMaterial->transparency = .0f;

  m_lineDrawStyle = new SoDrawStyle;
  m_lineDrawStyle->style = SoDrawStyle::LINES;
  m_lineDrawStyle->lineWidth = 1;

  // Group both draw style and material together
  m_appearanceGroup = new SoGroup;
  m_appearanceGroup->addChild(m_lineDrawStyle);

  m_appearanceGroup->addChild(m_meshLineDrawStyle);
  m_appearanceGroup->addChild(m_meshLineMaterial);
}
