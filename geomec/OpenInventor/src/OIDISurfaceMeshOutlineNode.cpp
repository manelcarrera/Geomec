#include "stdafx.h"

#include "OIDISurfaceMeshOutlineNode.h"
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
#include <MeshVizXLM/mapping/nodes/MoMeshOutline.h>
#include <MeshVizXLM/mapping/nodes/MoScalarSetI.h>


#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>

#include <MeshVizXLM/MxTimeStamp.h>
#include "MeshNodeSettings.h"

// The cell shape appearance node contains a draw style and a material node.
void OIDISurfaceMeshOutlineNode::buildSurfaceAppearanceNode()
{
    // The draw style specifies which components to draw (faces / edges / points)
    m_surfaceDrawStyle = new MoDrawStyle;
    m_surfaceDrawStyle->displayFaces = true;
    m_surfaceDrawStyle->displayEdges = false; // draw the edges of the triangles
    m_surfaceDrawStyle->displayPoints = false;

    // MoMaterial defines the material of surface representations
    m_surfaceMaterial = new MoMaterial;
    // m_surfaceMaterial->enhancedColoring = TRUE;

    m_surfaceMaterial->faceColoring = MoMaterial::COLOR;
    m_surfaceMaterial->faceColor = SbColor(0.7f, 0.7f, 0.0f); // green

    m_surfaceMaterial->transparency = 1.0f;

    // Group both draw style and material together
    m_surfaceAppearance = new SoGroup;

    m_surfaceAppearance->addChild(m_surfaceDrawStyle);
    m_surfaceAppearance->addChild(m_surfaceMaterial);
}



void OIDISurfaceMeshOutlineNode::buildSurfaceNode()
{
    // Add a MoMeshCellShape node to visualize the all cells in the mesh.
    m_surface = new MoMeshSurface;
    m_surface->colorScalarSetId = -1;
    

    buildSurfaceAppearanceNode();

    // Put the cell shape node under a separator with the given cellShapeAppearance, so 
    // the cellShapeAppearance won't affect other visualization nodes 
    m_surfaceSeparator = new SoSeparator;
    m_surfaceSeparator->addChild(m_surfaceAppearance);
    m_surfaceSeparator->addChild(m_surface);


    m_outline = new MoMeshOutline;
    m_outline->colorScalarSetId = -1;
    buildOutlineAppearanceNode();

    m_outlineSeparator = new SoSeparator;
    m_outlineSeparator->addChild(m_outlineAppearance);
    m_outlineSeparator->addChild(m_outline);

    // Add the cell shape node to a switch, so we can disable it easily
    m_surfaceSwitch = new SoSwitch;
    m_surfaceSwitch->addChild(m_surfaceSeparator);
    m_surfaceSwitch->addChild(m_outlineSeparator);
    m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
}


// Builds a node containing the mesh data, a cell shape rendering node, a dummy property set
// and a color map for the properties
void OIDISurfaceMeshOutlineNode::buildSurfaceMeshNode()
{
    buildMeshNode();
    buildSurfaceNode();
    buildVectorsNode();

    // Make all these nodes children of this one
    addChild(m_surfaceSwitch);
}


// Build a partial scene graph for a OIDIMeshSU instance. The returned node
// can then be added to the objects group.
void OIDISurfaceMeshOutlineNode::buildNodes()
{
    buildSurfaceMeshNode();
}

/**
* Constructor
*/
OIDISurfaceMeshOutlineNode::OIDISurfaceMeshOutlineNode(const OIDIMesh* meshData, OIDIMeshNodeManager * meshNodeManager)
    : OIDIMeshNode(meshData, meshNodeManager)
    , m_surfaceSwitch(0)
    , m_surfaceSeparator(0)
    , m_surfaceAppearance(0)
    , m_surfaceDrawStyle(0)
    , m_surfaceMaterial(0)
    , m_surface(0)
    , m_outlineSeparator(0)
    , m_outlineAppearance(0)
    , m_outlineDrawStyle(0)
    , m_outlineMaterial(0)
    , m_outline(0)
{
    buildNodes();
}


void OIDISurfaceMeshOutlineNode::setIsoValues(int isoCount, double min, double max)
{
}

void OIDISurfaceMeshOutlineNode::setIsoGap(double gap)
{
}

void OIDISurfaceMeshOutlineNode::UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor)
{
    OIDIMeshNode::UpdateDisplaySettings(meshNodeSettings, showColor);

    if (!isVisible())
    {
        m_surfaceSwitch->whichChild = SO_SWITCH_NONE;
        m_outlineMaterial->transparency = 1.0f;

    }
    else
    {
        m_surfaceSwitch->whichChild = SO_SWITCH_ALL;
        m_outlineMaterial->transparency = meshNodeSettings.transparency / 100.0f;
    }
}

void OIDISurfaceMeshOutlineNode::updateColorScalarSet()
{
    // m_surface->colorScalarSetId = m_colorScalarSetId;
}

void OIDISurfaceMeshOutlineNode::updateIsoScalarSet()
{
}


void OIDISurfaceMeshOutlineNode::updateGeologyColors()
{
    // m_surfaceMaterial->faceColor = m_geologyColor;
}


void OIDISurfaceMeshOutlineNode::buildOutlineAppearanceNode()
{
    // The draw style specifies which components to draw (faces / edges / points)
    m_outlineDrawStyle = new MoDrawStyle;
    m_outlineDrawStyle->displayFaces = false;
    m_outlineDrawStyle->displayEdges = true; // draw the edges of the triangles
    m_outlineDrawStyle->displayPoints = false;

    // MoMaterial defines the material of outline representations
    m_outlineMaterial = new MoMaterial;

    // CONTOURING means that rendering uses the color mapping defined by a scalar 
    // set and a color mapping function
    m_outlineMaterial->faceColoring = MoMaterial::COLOR;
    // Use a solid color for rendering the edges
    m_outlineMaterial->lineColoring = MoMaterial::COLOR;
    m_outlineMaterial->lineColor = SbColor(1.0f, 1.0f, 1.0f); // white;
    m_outlineMaterial->transparency = .0f;

    m_lineDrawStyle = new SoDrawStyle;
    m_lineDrawStyle->style = SoDrawStyle::LINES;
    m_lineDrawStyle->lineWidth = 2;

    // Group both draw style and material together
    m_outlineAppearance = new SoGroup;
    m_outlineAppearance->addChild(m_lineDrawStyle);

    m_outlineAppearance->addChild(m_outlineDrawStyle);
    m_outlineAppearance->addChild(m_outlineMaterial);
}

bool OIDISurfaceMeshOutlineNode::highlight()
{
    m_outlineMaterial->lineColoring = MoMaterial::COLOR;
    m_outlineMaterial->lineColor = SbColor(1.0f, 1.0f, 0.0f); // yellow;
    m_lineDrawStyle->style = SoDrawStyle::LINES;
    m_lineDrawStyle->lineWidth = 4;

    m_surfaceMaterial->transparency = 0.4f;

    return true;
}
bool OIDISurfaceMeshOutlineNode::dehighlight()
{
    m_outlineMaterial->lineColoring = MoMaterial::COLOR;
    m_outlineMaterial->lineColor = SbColor(1.0f, 1.0f, 1.0f); // white;
    m_lineDrawStyle->style = SoDrawStyle::LINES;
    m_lineDrawStyle->lineWidth = 2;

    m_surfaceMaterial->transparency = 1.0f;
    return true;
}




