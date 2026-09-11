#include "stdafx.h"
#include "FormationCrossSection.h"
#include "MoMeshTensorVector.h" // View/TensorViz
#include "DecimatingCellFilter.h"
#include "OIDIMesh.h"
#include "OIDIMeshNodeManager.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <MeshVizXLM/mapping/nodes/MoMesh.h>
#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>
#include <MeshVizXLM/mapping/nodes/MoColorMapping.h>
#include <MeshVizXLM/mapping/nodes/MoMeshVector.h>
#include <MeshVizXLM/mapping/nodes/MoMeshOutline.h>
#include <MeshVizXLM/mapping/nodes/MoMeshIsoline.h>
#include <MeshVizXLM/mapping/nodes/MoMeshPlaneSlice.h>
#include <MeshVizXLM/mapping/nodes/MoCellFilter.h>
#include <MeshVizXLM/extractors/MiExtractorCallback.h>
#include <MeshVizXLM/extractors/MiPlaneSliceExtractUnstructured.h>

/**
 * This is called every time a new plane slice is extracted, in order to update the cell filter
 */
class ExtractorCallback : public MiExtractorCallback
{
  MoMesh*                m_mesh;
  MoMeshPlaneSlice*      m_slice;
  DecimatingCellFilterI* m_filter;

public:

  /**
   * Constructor
   * @param mesh The extracted mesh data. Gets its data by connecting it to the MoMeshPlaneSlice.
   *          This parameter is included so we can manually call touch() after an extraction, to 
   *          make sure that all vectors, outlines etc. are defined on an up-to-date mesh.
   * @param slice The plane slice object used for the cross section. Included so we can adjust the
   *          cell filter after an extraction
   * @param filter The cell filter that works on the vectors and tensor vector defined on the cross
   *          section. Needs to be updated after every extraction because the number of cells has 
   *          most likely changed.
   */
  ExtractorCallback(MoMesh* mesh, MoMeshPlaneSlice* slice, DecimatingCellFilterI* filter)
  : m_mesh(mesh)
  , m_slice(slice)
  , m_filter(filter)
  {
  }

  virtual ~ExtractorCallback()
  {
  }

  virtual void beginExtract(
  const std::string extractorName, 
  bool geomChanged, 
  bool topoChanged, 
  bool dataSetChanged, 
  size_t numPhases)
  {
  }

  virtual void endExtract()
  {
  // This is somehow necessary to update the items that are defined on the extracted mesh
  // (outline, isoline, vectors, tensor vectors). If we don't do this, they sometimes stay
  // behind when dragging the cross section. Probably an OIV bug, but this nicely works 
  // around the issue.
  if(m_mesh)
      m_mesh->touch();

  //[svdr] TODO: this is done on every extraction, even when no filtering on vectors / tensors is
  // being done. Still need to find an elegant way of doing this only when necessary.
  const MiPlaneSliceExtractUnstructured* extractor = m_slice->getUnstructuredExtractor();
  const MeXSurfaceMeshUnstructured& surfaceMesh = extractor->getExtract();

  // Update the cell filter for the correct number of cells
  size_t numCells = surfaceMesh.getTopology().getNumCells();
  m_filter->init(numCells);
  }

  // not used
  virtual bool beginPhase(size_t phaseId, std::string phaseName, size_t numIterationInPhase)  { return true; }
  virtual bool endPhase() { return true; }
  virtual bool endStep(size_t numIterationDone) { return true; }
  virtual double getEndStepCallPeriod() { return 1.0; }
};

SO_NODE_SOURCE(FormationCrossSection);

SbPlane FormationCrossSection::getDefaultPlane()
{
  return SbPlane(SbVec3f(1.0f, 0.0f, 0.0f), 0.0f);
}

void FormationCrossSection::sensorCallback(void* data, SoSensor* sensor)
{
  FormationCrossSection* xsec = reinterpret_cast<FormationCrossSection*>(data);
  xsec->onSensorTriggered(sensor);
}

void FormationCrossSection::onSensorTriggered(SoSensor* sensor)
{
  float value = vectorFilter.getValue();
  m_cellFilterSwitch->whichChild = (value >= 1.0f) ? SO_SWITCH_NONE : SO_SWITCH_ALL;
  m_decimatingCellFilter->setVisibleFraction(value);
}

void FormationCrossSection::buildNode()
{
  m_mesh = new MoMesh;

  m_crossSectionMaterial = new MoMaterial;
  m_crossSectionMaterial->faceColor.connectFrom(&color);
  m_crossSectionMaterial->faceColoring.connectFrom(&coloring);
  m_crossSectionMaterial->lineColor = SbColor(0.0f, 0.0f, 0.0f);
  m_crossSectionMaterial->lineColoring = MoMaterial::COLOR;

  m_crossSectionDrawStyle = new MoDrawStyle;
  m_crossSectionDrawStyle->displayEdges.connectFrom(&showEdges);
  m_crossSectionDrawStyle->displayFaces = true;
  m_crossSectionDrawStyle->displayPoints = false;

  m_slice = new MoMeshPlaneSlice;
  m_slice->plane.connectFrom(&plane);
  m_slice->colorScalarSetId.connectFrom(&colorScalarSetId);

  SoSeparator* sliceSep = new SoSeparator;
  sliceSep->addChild(m_mesh);
  sliceSep->addChild(m_crossSectionMaterial);
  sliceSep->addChild(m_crossSectionDrawStyle);
  sliceSep->addChild(m_slice);

  m_cellFilterSwitch = new SoSwitch;
  m_cellFilterSwitch->whichChild = SO_SWITCH_NONE;
  m_cellFilter = new MoCellFilter;
  m_decimatingCellFilter = new DecimatingCellFilterI;
  m_cellFilter->setCellFilter(m_decimatingCellFilter);
  m_cellFilterSwitch->addChild(m_cellFilter);

  m_vectorSwitch = new SoSwitch;
  m_vectorSwitch->whichChild.connectFrom(&m_showVectors->whichChild);

  m_tensorVectorSwitch = new SoSwitch;
  m_tensorVectorSwitch->whichChild.connectFrom(&m_showTensorVectors->whichChild);

  m_outlineSwitch = new SoSwitch;
  m_outlineSwitch->whichChild.connectFrom(&m_showOutline->whichChild);
  
  m_isolineSwitch = new SoSwitch;
  m_isolineSwitch->whichChild.connectFrom(&m_showIsoline->whichChild);

  m_planeMesh = new MoMesh;
  m_planeMesh->connectFrom(m_slice);

  m_extractorCallback = new ExtractorCallback(m_planeMesh, m_slice, m_decimatingCellFilter);
  m_slice->setExtractorCallback(*m_extractorCallback);

  m_vectors = new MoMeshVector;
  m_vectors->colorScalarSetId = -1;
  m_vectors->vectorSetId.connectFrom(&vectorSetId);
  m_vectors->scaleFactor.connectFrom(&vectorScale);
  m_vectorSwitch->addChild(m_vectors);

  m_tensorVectors = new MoMeshTensorVector;
  m_tensorVectors->vectorSetId.connectFrom(&tensorVectorSetId);
  m_tensorVectors->colorScalarSetId.connectFrom(&tensorVectorScalarSetId);
  m_tensorVectors->scaleFactor.connectFrom(&vectorScale);
  m_tensorVectorSwitch->addChild(OIDIMeshNodeManager::GetTensorVectorColorMapping());
  m_tensorVectorSwitch->addChild(m_tensorVectors);

  m_vectorGroup = new SoSeparator;
  m_vectorGroup->addChild(m_cellFilterSwitch);
  m_vectorGroup->addChild(m_vectorSwitch);
  m_vectorGroup->addChild(m_tensorVectorSwitch);

  m_outlineMaterial = new MoMaterial;
  m_outlineMaterial->lineColor = SbColor(1.0f, 1.0f, 1.0f);
  m_outlineMaterial->lineColoring = MoMaterial::COLOR;

  m_outlineDrawStyle = new MoDrawStyle;
  m_outlineDrawStyle->displayEdges = true;

  m_soOutlineDrawStyle = new SoDrawStyle;
  m_soOutlineDrawStyle->style = SoDrawStyle::LINES;
  m_soOutlineDrawStyle->lineWidth = 2;
  m_outlineSwitch->addChild(m_soOutlineDrawStyle);

  m_outline = new MoMeshOutline;
  m_outlineSwitch->addChild(m_outline);

  m_isoline = new MoMeshIsoline;
  m_isoline->isoScalarSetId.connectFrom(&isoScalarSetId);
  m_isoline->isovalues.connectFrom(&isovalues);
  m_isolineSwitch->addChild(m_isoline);

  SoSeparator* vectorSep = new SoSeparator;
  vectorSep->addChild(m_planeMesh);
  vectorSep->addChild(m_vectorGroup);
  vectorSep->addChild(m_outlineMaterial);
  vectorSep->addChild(m_outlineDrawStyle);

  vectorSep->addChild(m_isolineSwitch);
  vectorSep->addChild(m_outlineSwitch);

  addChild(sliceSep);
  addChild(vectorSep);
}

void FormationCrossSection::initClass()
{
  SO_NODE_INIT_CLASS(FormationCrossSection, SoGroup, "Group");
}

void FormationCrossSection::exitClass()
{
  SO__NODE_EXIT_CLASS(FormationCrossSection);
}

FormationCrossSection::FormationCrossSection()
  : m_mesh(0)
  , m_crossSectionMaterial(0)
  , m_crossSectionDrawStyle(0)
  , m_slice(0)
  , m_vectorSwitch(0)
  , m_outlineSwitch(0)
  , m_isolineSwitch(0)
  , m_vectors(0)
  , m_planeMesh(0)
  , m_outlineMaterial(0)
  , m_outlineDrawStyle(0)
  , m_soOutlineDrawStyle(0)
  , m_outline(0)
  , m_isoline(0)
{
  SO_NODE_CONSTRUCTOR(FormationCrossSection);
  SO_NODE_ADD_FIELD(plane, (getDefaultPlane()));
  SO_NODE_ADD_FIELD(coloring, (COLOR));
  SO_NODE_ADD_FIELD(color, (1.0f, 1.0f, 1.0f));
  SO_NODE_ADD_FIELD(colorScalarSetId, (-1));
  SO_NODE_ADD_FIELD(isoScalarSetId, (-1));
  SO_NODE_ADD_FIELD(vectorSetId, (-1));
  SO_NODE_ADD_FIELD(tensorVectorSetId, (-1));
  SO_NODE_ADD_FIELD(tensorVectorScalarSetId, (-1));
  SO_NODE_ADD_FIELD(isovalues, (0.0f));
  SO_NODE_ADD_FIELD(showEdges, (true));
  SO_NODE_ADD_FIELD(showVectors, (false));
  SO_NODE_ADD_FIELD(showTensorVectors, (false));
  SO_NODE_ADD_FIELD(showOutline, (false));
  SO_NODE_ADD_FIELD(showIsoline, (false));
  SO_NODE_ADD_FIELD(vectorScale, (1.0f));
  SO_NODE_ADD_FIELD(vectorFilter, (1.0f));

  SO_NODE_DEFINE_ENUM_VALUE(ColoringType, COLOR);
  SO_NODE_DEFINE_ENUM_VALUE(ColoringType, CONTOURING);
  SO_NODE_SET_SF_ENUM_TYPE(coloring, ColoringType);

  m_showVectors = new SoSwitchBool;
  m_showVectors->on.connectFrom(&showVectors);

  m_showTensorVectors = new SoSwitchBool;
  m_showTensorVectors->on.connectFrom(&showTensorVectors);
  
  m_showOutline = new SoSwitchBool;
  m_showOutline->on.connectFrom(&showOutline);

  m_showIsoline = new SoSwitchBool;
  m_showIsoline->on.connectFrom(&showIsoline);

  m_vectorFilterSensor.setFunction(sensorCallback, this);
  m_vectorFilterSensor.attach(&vectorFilter);
  m_vectorFilterSensor.setPriority(0);

  buildNode();
}

FormationCrossSection::~FormationCrossSection()
{
  // Apparently we need to disconnect these, otherwise the application
  // crashes somewhere in the field destructor

  // Crashes were caused by SoSwitchBool not being new'ed

  delete m_extractorCallback;
}

void FormationCrossSection::setMesh(const OIDIMeshVU* mesh)
{
  m_mesh->setMesh(mesh);

  // Get solid color from OpenGL node
  COpenGLNodeBase::TColor formationColor = mesh->getOpenGLNode()->Color();
  float red   = qRed(formationColor)   / 255.0f;
  float green = qGreen(formationColor) / 255.0f;
  float blue  = qBlue(formationColor)  / 255.0f;
  color = SbColor(red, green, blue);
}

const OIDIMeshVU* FormationCrossSection::getMesh() const
{
  MeshType meshType;
  return dynamic_cast<const OIDIMeshVU*>(m_mesh->getMesh(meshType));
}
