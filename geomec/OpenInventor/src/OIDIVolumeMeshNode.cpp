#include "stdafx.h"

#include "OIDIVolumeMeshNode.h"
#include "OpenInventorDataInterface.h"
#include "OIDIMeshNodeManager.h"
#include "DecimatingCellFilter.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <MeshVizXLM/mesh/data/MiDataSetI.h>

#include <MeshVizXLM/mapping/nodes/MoMesh.h>
#include <MeshVizXLM/mapping/nodes/MoMeshSkin.h>
#include <MeshVizXLM/mapping/nodes/MoMeshVector.h>
#include <MeshVizXLM/mapping/nodes/MoMeshCellShape.h>
#include <MeshVizXLM/mapping/nodes/MoScalarSetI.h>
#include <MeshVizXLM/mapping/nodes/MoCellFilter.h>

#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>
#include <MeshVizXLM/mapping/nodes/MoPredefinedColorMapping.h>

#include <MeshVizXLM/mapping/nodes/MoMeshAnnotatedIsoline.h>

#include <MeshVizXLM/MxTimeStamp.h>

#include "CrossSection.h"
#include "MeshNodeSettings.h"

// The skin appearance node contains a draw style and a material node.
void OIDIVolumeMeshNode::buildSkinAppearanceNode()
{
    // The draw style specifies which components to draw (faces / edges / points)
    m_skinDrawStyle = new MoDrawStyle;
    m_skinDrawStyle->displayFaces = true;
    m_skinDrawStyle->displayEdges = true; // draw the edges of the triangles
    m_skinDrawStyle->displayPoints = false;

    // MoMaterial defines the material of surface representations
    m_skinMaterial = new MoMaterial;
    // m_skinMaterial->enhancedColoring = TRUE;

    // CONTOURING means that rendering uses the color mapping defined by a scalar 
    // set and a color mapping function
    m_skinMaterial->faceColoring = MoMaterial::COLOR;
    // Use a solid color for rendering the edges
    m_skinMaterial->lineColoring = MoMaterial::COLOR;
    m_skinMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f); // black
    m_skinMaterial->transparency = .0f;

    // Group both draw style and material together
    m_skinAppearance = new SoGroup;
    m_skinAppearance->addChild(m_skinDrawStyle);
    m_skinAppearance->addChild(m_skinMaterial);
}

// The skin appearance node contains a draw style and a material node.
void OIDIVolumeMeshNode::buildCellShapeAppearanceNode()
{
    // The draw style specifies which components to draw (faces / edges / points)
    m_cellShapeDrawStyle = new MoDrawStyle;
    m_cellShapeDrawStyle->displayFaces = false;
    m_cellShapeDrawStyle->displayEdges = true; // draw the edges of the triangles
    m_cellShapeDrawStyle->displayPoints = false;


    // MoMaterial defines the material of surface representations
    m_cellShapeMaterial = new MoMaterial;
    // m_cellShapeMaterial->enhancedColoring = TRUE;

    // CONTOURING means that rendering uses the color mapping defined by a scalar 
    // set and a color mapping function
    m_cellShapeMaterial->faceColoring = MoMaterial::CONTOURING;
    // Use a solid color for rendering the edges
    m_cellShapeMaterial->lineColoring = MoMaterial::CONTOURING;
    m_cellShapeMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f); // black;
    m_cellShapeMaterial->transparency = .0f;

    // Group both draw style and material together
    m_cellShapeAppearance = new SoGroup;
    m_cellShapeAppearance->addChild(m_cellShapeDrawStyle);
    m_cellShapeAppearance->addChild(m_cellShapeMaterial);
}

void OIDIVolumeMeshNode::buildSkinNode()
{
    // Add a MoMeshSkin node to visualize the mesh data. The mesh skin is made 
    // up of all the faces that belong to only one cell. 
    m_skin = new MoMeshSkin;

    buildSkinAppearanceNode();

    // Put the skin node under a separator with the given skinAppearance, so 
    // the skinAppearance won't affect other visualization nodes
    m_skinSeparator = new SoSeparator;

    m_skinSeparator->addChild(m_skinAppearance);
    m_skinSeparator->addChild(m_skin);

    BuildSkinIsolineNode();

    // Add the skin node to a switch, so we can disable it easily
    m_skinSwitch = new SoSwitch;
    m_skinSwitch->setName("skinSwitch");
    m_skinSwitch->addChild(m_skinSeparator);
    m_skinSwitch->addChild(m_skinIsolineSwitch);
    m_skinSwitch->whichChild = SO_SWITCH_ALL;
}

void OIDIVolumeMeshNode::buildCellShapeNode()
{
    // Add a MoMeshCellShape node to visualize the all cells in the mesh.
    m_cellShape = new MoMeshCellShape;
    m_cellShape->inclusiveList = FALSE;
    m_cellShape->colorScalarSetId = 0;
    m_cellShape->parallel = TRUE;

    buildCellShapeAppearanceNode();

    // Put the cell shape node under a separator with the given cellShapeAppearance, so 
    // the cellShapeAppearance won't affect other visualization nodes 
    m_cellShapeSeparator = new SoSeparator;
    SoDrawStyle * pointDrawStyle = new SoDrawStyle;
    pointDrawStyle->style = SoDrawStyle::POINTS;
    pointDrawStyle->pointSize = 6;
    m_cellShapeSeparator->addChild(pointDrawStyle);
    m_cellShapeSeparator->addChild(m_cellShapeAppearance);
    m_cellShapeSeparator->addChild(m_cellShape);

    // Add the skin node to a switch, so we can disable it easily
    m_cellShapeSwitch = new SoSwitch;
    m_cellShapeSwitch->setName("cellShapeSwitch");
    m_cellShapeSwitch->addChild(m_cellShapeSeparator);
    m_cellShapeSwitch->whichChild = SO_SWITCH_NONE;
}


// Builds a node containing the mesh data, a skin rendering node, a dummy property set
// and a color map for the properties
void OIDIVolumeMeshNode::buildVolumeMeshNode()
{
    // Create a MoMesh node that holds the mesh data, which in our case is an 
    // instance of the OIDIMeshVU wrapper class. The purpose of the MoMesh node
    // is to provide the mesh data to subsequent nodes, it doesn't do anything
    // itself.
    buildMeshNode();
    buildSkinNode();
    buildCellShapeNode();
    buildTensorsNode();
    buildVectorsNode();

    // Make all these nodes children of this one
    addChild(m_skinSwitch);
    addChild(m_cellShapeSwitch);
}

// Build a partial scene graph for a OIDIMeshVU instance. The returned node
// can then be added to the objects group.
void OIDIVolumeMeshNode::buildNodes()
{
    buildVolumeMeshNode();
}

/**
* Constructor
*/
OIDIVolumeMeshNode::OIDIVolumeMeshNode(const OIDIMesh* meshData, OIDIMeshNodeManager * meshNodeManager)
    : OIDIMeshNode(meshData, meshNodeManager)
    , m_skinSwitch(0)
    , m_skinSeparator(0)
    , m_skinAppearance(0)
    , m_skinDrawStyle(0)
    , m_skinMaterial(0)
    , m_skin(0)
    , m_cellShapeSwitch(0)
    , m_cellShapeSeparator(0)
    , m_cellShapeAppearance(0)
    , m_cellShapeDrawStyle(0)
    , m_cellShapeMaterial(0)
    , m_cellShape(0)
{
    buildNodes();

    QByteArray name = meshData->getOpenGLNode()->Name().toLatin1();
    name.replace(' ', '_');
    setName(name.data());

    updateColorScalarSet();
}

void OIDIVolumeMeshNode::UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor)
{
    OIDIMeshNode::UpdateDisplaySettings(meshNodeSettings, showColor);
    m_skinMaterial->transparency = meshNodeSettings.transparency / 100.0f;
    m_cellShapeMaterial->transparency = meshNodeSettings.transparency / 100.0f;

    if (!isVisible())
    {
        // Display only faces but fully transparent
        m_skinSwitch->whichChild = SO_SWITCH_ALL;
        m_cellShapeSwitch->whichChild = SO_SWITCH_NONE;
        setDisplayBeachBalls(false);

        m_skinDrawStyle->displayFaces = true;
        m_skinDrawStyle->displayEdges = false;
        m_skinDrawStyle->displayPoints = false;

        m_skinMaterial->transparency = 1.0f;
        m_cellShapeMaterial->transparency = 1.0f;

        m_skinIsolineSwitch->whichChild = SO_SWITCH_NONE;
    }
    else if (getMesh()->meshType() == OIDIMesh::PointSetMesh)
    {
        m_skinSwitch->whichChild = SO_SWITCH_NONE;
        m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
        m_cellShapeDrawStyle->displayEdges = false;
        m_cellShapeDrawStyle->displayFaces = false;
        m_cellShapeDrawStyle->displayPoints = true;
        m_cellShapeMaterial->pointColoring = MoMaterial::CONTOURING;
        m_cellShapeMaterial->pointColor = m_geologyColor;
    }
    else if (getMesh()->meshType() == OIDIMesh::PointMesh)
    {
        m_skinSwitch->whichChild = SO_SWITCH_NONE;
        m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
        m_cellShapeDrawStyle->displayEdges = false;
        m_cellShapeDrawStyle->displayFaces = true;
        m_cellShapeDrawStyle->displayPoints = true;
        m_cellShapeMaterial->pointColoring = MoMaterial::CONTOURING;
        m_cellShapeMaterial->pointColor = m_geologyColor;
        m_cellShape->factor = 1;
    }
    else
    {
        m_skinIsolineSwitch->whichChild = showIsoLines(meshNodeSettings) ? SO_SWITCH_ALL : SO_SWITCH_NONE;

        if (meshNodeSettings.meshFilled)
        {
            switch (meshNodeSettings.meshMode)
            {
            case CenterMesh:
                m_skinSwitch->whichChild = SO_SWITCH_NONE;
                m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed; // draw the edges of the triangles
                m_cellShapeDrawStyle->displayFaces = true;
                m_cellShapeDrawStyle->displayPoints = false;
                m_cellShapeMaterial->lineColoring = MoMaterial::CONTOURING;
                m_cellShapeMaterial->lineColor = m_geologyColor;
                m_cellShape->factor = 0.05f;
                break;
            case  FullMesh:		
                m_skinSwitch->whichChild = SO_SWITCH_NONE;
                m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed; // draw the edges of the triangles
                m_cellShapeDrawStyle->displayFaces = true;
                m_cellShapeDrawStyle->displayPoints = false;
                m_cellShapeMaterial->lineColoring = MoMaterial::COLOR;
                m_cellShapeMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f);
                m_cellShape->factor = 1;
                break;

            case SkinMesh:
                m_skinSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeSwitch->whichChild = SO_SWITCH_NONE;
                m_skinDrawStyle->displayFaces = true;
                m_skinDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed && !showIsoLines(meshNodeSettings); // do not draw edges if drawing isolines
                m_skinMaterial->lineColoring = MoMaterial::COLOR;
                m_skinMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f);
                m_skinIsoline->colorScalarSetId = -1;
                break;

            default:
                assert(false);
            }
        }
        else
        {
            switch (meshNodeSettings.meshMode)
            {
            case  FullMesh:
                m_skinSwitch->whichChild = SO_SWITCH_NONE;
                m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed; // draw the edges of the triangles
                m_cellShapeDrawStyle->displayFaces = false;
                m_cellShapeDrawStyle->displayPoints = false;
                m_cellShapeMaterial->lineColoring = MoMaterial::CONTOURING;
                m_cellShapeMaterial->lineColor = m_geologyColor;
                m_cellShape->factor = 1;
                break;

            case SkinMesh:
                m_skinSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeSwitch->whichChild = SO_SWITCH_NONE;

                m_skinDrawStyle->displayFaces = false;
                m_skinDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed && !showIsoLines(meshNodeSettings);
                m_skinMaterial->lineColoring = MoMaterial::CONTOURING;
                m_skinMaterial->lineColor = m_geologyColor;

                m_skinIsoline->colorScalarSetId = m_colorScalarSetId;
                break;

            case CenterMesh:
                m_skinSwitch->whichChild = SO_SWITCH_NONE;
                m_cellShapeSwitch->whichChild = SO_SWITCH_ALL;
                m_cellShapeDrawStyle->displayEdges = meshNodeSettings.meshLinesDisplayed; // draw the edges of the triangles
                m_cellShapeDrawStyle->displayFaces = true;
                m_cellShapeDrawStyle->displayPoints = false;
                m_cellShapeMaterial->lineColoring = MoMaterial::CONTOURING;
                m_cellShapeMaterial->lineColor = m_geologyColor;
                m_cellShape->factor = 0.05f;
                break;

            default:
                assert (false);
            }
        }
    }
}

void OIDIVolumeMeshNode::setIsoValues(int isoCount, double min, double max)
{
    std::vector<float> isovalues(isoCount);
    computeIsovalues(isovalues, min, max);
    m_skinIsoline->isovalues.setValue(0.0);
    m_skinIsoline->isovalues.setValues(0, isoCount, &isovalues[0]);
}

void OIDIVolumeMeshNode::setIsoGap(double gap)
{
    m_skinIsoline->gap = gap;

}

void OIDIVolumeMeshNode::updateColorScalarSet()
{
    m_cellShape->colorScalarSetId = m_colorScalarSetId;
    m_skin->colorScalarSetId = m_colorScalarSetId;

    MoMaterial::ColoringType colorType = m_colorScalarSetId == -1
      ? MoMaterial::COLOR
      : MoMaterial::CONTOURING;

    m_skinMaterial->faceColoring = colorType;
    m_cellShapeMaterial->faceColoring = colorType;
}

void OIDIVolumeMeshNode::updateIsoScalarSet()
{
    m_skinIsoline->isoScalarSetId = m_isoScalarSetId;
}

void OIDIVolumeMeshNode::updateGeologyColors()
{
    m_cellShapeMaterial->faceColor = m_geologyColor;
    m_skinMaterial->faceColor = m_geologyColor;
}

void OIDIVolumeMeshNode::BuildSkinIsolineNode()
{
    m_skinIsolineSwitch = new SoSwitch;
    m_skinIsolineSwitch->setName("IsoLineSwitch");
    m_skinIsolineSwitch->whichChild = SO_SWITCH_NONE;

    MoMesh* meshForIsoline = new MoMesh;
    meshForIsoline->connectFrom(m_skin);
    m_skinIsolineSwitch->addChild(meshForIsoline);

    m_skinIsoline = CreateIsoline();
  
    m_skinIsolineSwitch->addChild(m_skinIsoline);
}





