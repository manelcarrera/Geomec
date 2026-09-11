#include "stdafx.h"

#include <MeshVizXLM/mapping/nodes/MoCellFilter.h>
#include <MeshVizXLM/mapping/nodes/MoMeshVector.h>
#include <MeshVizXLM/mapping/nodes/MoColorMapping.h>
#include <MeshVizXLM/mapping/nodes/MoLevelColorMapping.h>
#include <MeshVizXLM/mapping/nodes/MoMesh.h>
#include <MeshVizXLM/mapping/nodes/MoMeshAnnotatedIsoline.h>

#include <Inventor/nodes/SoSwitch.h>

// mandatory order ("TensorViz/MoMeshTensor.h") to prevent the error
// #error gl.h included before <Inventor/sys/SoGL.h>

#include "MoMeshTensor.h" //View/TensorViz

#include "OIDIMeshNode.h"
#include "OIDIMesh.h"
#include "DecimatingCellFilter.h"
#include "MoMeshTensorVector.h" //View/TensorViz
#include "OIDIMeshNodeManager.h"
#include "MeshNodeSettings.h"

OIDIMeshNode::OIDIMeshNode(const OIDIMesh * meshData, OIDIMeshNodeManager * meshNodeManager)
    : m_meshNodeManager(meshNodeManager)
    , m_mesh(meshData)
    , m_colorScalarSetId(-1)
    , m_valueColorScalarSetId(-1)
    , m_isoScalarSetId(-1)
    , m_tensorSetId(-1)
    , m_geologyColor (0.0f, 0.0f, 0.0f)
    , m_vectorsSwitch(0)
    , m_cellFilterSwitch(0)
    , m_cellFilter(0)
    , m_decimatingCellFilter(0)
    , m_vectors(0)
    , m_tensorVectors(0)
    , m_tensorSwitch(0)
    , m_beachballSwitch(0)
    , m_beachballs(0)
	, m_visibility(false)
{
}

const OIDIMesh * OIDIMeshNode::getMesh()
{
    return m_mesh;
}

OIDIMeshNodeManager * OIDIMeshNode::getMeshNodeManager()
{
    return m_meshNodeManager;
}

MbVec3d OIDIMeshNode::getMax()
{
    return m_mesh->getMax();
}

MbVec3d OIDIMeshNode::getMin()
{
    return m_mesh->getMin();
}

size_t OIDIMeshNode::getNumCells()
{
    return m_mesh->getNumCells();
}

void OIDIMeshNode::buildCellFilterNode()
{
  if(m_cellFilter != 0)
    return;

  m_cellFilter = new MoCellFilter;
  m_decimatingCellFilter = new DecimatingCellFilterI;
  m_decimatingCellFilter->init(m_mesh->getNumCells());
  m_cellFilter->setCellFilter(m_decimatingCellFilter);

  m_cellFilterSwitch = new SoSwitch;
  m_cellFilterSwitch->addChild(m_cellFilter);
  m_cellFilterSwitch->whichChild = SO_SWITCH_NONE;
}

/**
 *
 *       switch
 *         |
 *      separator
 *      /    \
 *     /      \
 *  switch   vectors
 *    |
 *  filter
 *
 */
void OIDIMeshNode::buildVectorsNode()
{
    m_vectors = new MoMeshVector;
    m_vectors->colorScalarSetId = -1;
    m_vectors->vectorSetId = -1;

    m_tensorVectors = new MoMeshTensorVector;
    m_tensorVectors->colorScalarSetId = -1;
    m_tensorVectors->vectorSetId = -1;

    buildCellFilterNode();

    SoSeparator* vectorSeparator = new SoSeparator;
    vectorSeparator->addChild(m_cellFilterSwitch);
    vectorSeparator->addChild(m_vectors);

    MoColorMapping * tensorVectorColorMapping = OIDIMeshNodeManager::GetTensorVectorColorMapping();

    SoSeparator* tensorVectorSeparator = new SoSeparator;
    tensorVectorSeparator->addChild(m_cellFilterSwitch);
    tensorVectorSeparator->addChild(tensorVectorColorMapping);
    tensorVectorSeparator->addChild(m_tensorVectors);

    m_tensorSwitch = new SoSwitch;
    m_tensorSwitch->setName("tensorVectorSwitch");
    m_tensorSwitch->whichChild = SO_SWITCH_NONE;
    m_tensorSwitch->addChild(tensorVectorSeparator);

    m_vectorsSwitch = new SoSwitch;
    m_vectorsSwitch->setName("vectorsSwitch");
    m_vectorsSwitch->whichChild = SO_SWITCH_NONE;
    m_vectorsSwitch->addChild(vectorSeparator);

    addChild(m_tensorSwitch);
    addChild(m_vectorsSwitch);
}

void OIDIMeshNode::buildTensorsNode()
{
  buildCellFilterNode();

  m_beachballs = new MoMeshTensor;
  m_beachballs->tensorSetId = -1;
  m_beachballs->colorScalarSetId = -1;

  // Build a colormapping node for the beachballs, containing 4 colors. The first 2 colors are for tensile / compressive
  // stresses on the min principal vector, the last 2 colors for the med principal vector
  float values[] = { -.5f, 0.5f, 1.5f, 2.5f, 3.5f };
  float colors[][4] = 
    { 
        1.0f, 0.2f, 0.2f, 1.0f, // red      scalar value 0.0: min. principal direction, eigenValue <  0 (tensile)
        0.2f, 1.0f, 0.2f, 1.0f, // green    scalar value 1.0: min. principal direction, eigenValue >= 0 (compressive)
        1.0f, 1.0f, 1.0f, 1.0f, // white    scalar value 2.0: med. principal direction, eigenValue <  0 (tensile)
        1.0f, 1.0f, 1.0f, 1.0f, // white    scalar value 3.0: med. principal direction, eigenValue >= 0 (compressive)
    };
  MoLevelColorMapping* colorMap = new MoLevelColorMapping;
  colorMap->values.setValues(0, 5, values);
  colorMap->colors.setValues(0, 4, colors);

  SoSeparator* sep = new SoSeparator;
  sep->renderCaching = false;
  sep->addChild(m_cellFilterSwitch);
  sep->addChild(colorMap);
  sep->addChild(m_beachballs);

  m_beachballSwitch = new SoSwitch;
  m_beachballSwitch->setName("beachballSwitch");
  m_beachballSwitch->addChild(sep);
  m_beachballSwitch->whichChild = SO_SWITCH_NONE;
  
  addChild(m_beachballSwitch);
}

void OIDIMeshNode::buildMeshNode()
{
    // Create a MoMesh node that holds the mesh data, which in our case is an 
    // instance of the OIDIMeshSU wrapper class. The purpose of the MoMesh node
    // is to provide the mesh data to subsequent nodes, it doesn't do anything
    // itself.
    MoMesh * mesh = new MoMesh;
    
    const MiMesh * miMesh = dynamic_cast<const MiMesh *> (m_mesh);
    assert (miMesh);
    mesh->setMesh(miMesh);

    addChild(mesh);
}

void OIDIMeshNode::setDisplayBeachBalls(bool display)
{
	if (m_beachballSwitch) m_beachballSwitch->whichChild = display ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void OIDIMeshNode::setDisplayVectors(bool display)
{
  m_vectorsSwitch->whichChild = display ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void OIDIMeshNode::setDisplayTensorVectors(bool display)
{
    m_tensorSwitch->whichChild = display ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void OIDIMeshNode::setTensorSetId(int id)
{
    m_tensorSetId = id;

    if (m_beachballs != 0)
    {
      m_beachballs->tensorSetId = id;
    }
}

int OIDIMeshNode::getTensorSetId() const
{
  assert(m_beachballs->tensorSetId.getValue() == m_tensorSetId);

	return m_tensorSetId;
}

void OIDIMeshNode::setVectorSetId(int id)
{
  m_vectors->vectorSetId = id;
}

int OIDIMeshNode::getVectorSetId()
{
    return m_vectors->vectorSetId.getValue();
}

void OIDIMeshNode::setTensorVectorSetId(int id)
{
    m_tensorVectors->vectorSetId = id;
}

int OIDIMeshNode::getTensorVectorSetId() const
{
    return m_tensorVectors->vectorSetId.getValue();
}

void OIDIMeshNode::setVectorScalarSetId(int id)
{
    m_tensorVectors->colorScalarSetId = id;
}

void OIDIMeshNode::setBeachballScalarSetIds(int primaryId, int secondaryId)
{
    if (m_beachballs)
    {
        m_beachballs->colorScalarSetId = primaryId;
        m_beachballs->secondaryColorScalarSetId = secondaryId;
    }
}

int OIDIMeshNode::getVectorScalarSetId()
{
    return m_tensorVectors->colorScalarSetId.getValue();
}

void OIDIMeshNode::setVectorVisibleFraction(float factor)
{
  m_cellFilterSwitch->whichChild = (factor < 1.0f) ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  m_decimatingCellFilter->setVisibleFraction(factor);
}

void OIDIMeshNode::UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor)
{
    float scaleFactor = meshNodeSettings.vectorBaseScale * meshNodeSettings.vectorScale;
    m_vectors->scaleFactor = scaleFactor;
    m_tensorVectors->scaleFactor = scaleFactor;
    setVectorVisibleFraction(meshNodeSettings.vectorVisibleFraction);

	if (m_beachballs) 
    {
      m_beachballs->scaleFactor = meshNodeSettings.beachBallBaseScale * meshNodeSettings.vectorScale;
      m_beachballs->tensorSetId = meshNodeSettings.viewBeachBalls ? m_tensorSetId : -1;
    }

    setIsoValues(meshNodeSettings.isoCount, meshNodeSettings.dataMin, meshNodeSettings.dataMax);
    setIsoGap (meshNodeSettings.isoGap);

    if (showColor) switchToGeologyColors();
    else switchToValueColors();
}

void OIDIMeshNode::setColorScalarSetId( int id )
{
    m_valueColorScalarSetId = id;
    m_colorScalarSetId = id;
    updateColorScalarSet();
}

int OIDIMeshNode::getValueColorScalarSetId()
{
    return m_valueColorScalarSetId;
}

void OIDIMeshNode::switchToGeologyColors()
{
    m_colorScalarSetId = -1;
    updateColorScalarSet();
}

void OIDIMeshNode::switchToValueColors()
{
    m_colorScalarSetId = m_valueColorScalarSetId;
    updateColorScalarSet();
}

void OIDIMeshNode::setIsoScalarSetId( int id )
{
    m_isoScalarSetId = id;
    updateIsoScalarSet();
}

int OIDIMeshNode::getIsoScalarSetId()
{
    return m_isoScalarSetId;
}

void OIDIMeshNode::setGeologyColor( float red, float green, float blue )
{
    m_geologyColor = SbColor (red, green, blue);
    updateGeologyColors();
}

SbBox3f OIDIMeshNode::getBoundingBox()
{
    MbVec3d max = getMax();
    MbVec3d min = getMin();

    SbBox3f bbox;
    bbox.setBounds(min[0], min[1], min[2], max[0], max[1], max[2]);

    return bbox;
}

void OIDIMeshNode::computeIsovalues( std::vector<float> & isovalues, double min, double max )
{
    size_t numIsoValues = isovalues.size();
    float delta = (max - min) / numIsoValues;
    for (size_t i=0; i< numIsoValues; i++)
        isovalues[i] = i*delta + min;
}

bool OIDIMeshNode::showIsoLines(const MeshNodeSettings & meshNodeSettings)
{
    bool doShow = meshNodeSettings.contourLineModeSelected;
    if (m_isoScalarSetId < 0) doShow = false;
    // if (meshNodeSettings.showValuesPerElement) doShow = false;

    return doShow;
}

MoMeshAnnotatedIsoline * OIDIMeshNode::CreateIsoline()
{
    MoMeshAnnotatedIsoline * isoline = new MoMeshAnnotatedIsoline;
    isoline->gap = 2.0f;
    isoline->fontSize = 12.0f;
    isoline->isBackground = TRUE;
    isoline->isText2D = TRUE;
    isoline->isoScalarSetId = -1;
    isoline->colorScalarSetId = -1;
    isoline->majorPeriod = 1;
    isoline->color = SbColor (0.8f, 0.8f, 0.8f);
    isoline->path = MoMeshAnnotatedIsoline::TANGENTIAL_PATH;

    return isoline;
}

void OIDIMeshNode::setVisibility(bool visibility)
{
	m_visibility = visibility;
}

bool OIDIMeshNode::isVisible() const
{
	return m_visibility;
}


void OIDIMeshNode::pick(SoPickAction* action)
{
    if(!isVisible())
        return;
    SoGroup::pick(action);
}