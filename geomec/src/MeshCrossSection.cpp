#include "stdafx.h"
#include "MeshCrossSection.h"
#include "FormationCrossSection.h"
#include "OIDIMeshNodeManager.h"
#include "OIDIMeshNode.h"

#include "CrossSectionManipulator.h"

#include "SoGroupIterator.h"
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include "MeshNodeSettings.h"

#include "WellCasingModel.h"
#include "OIDIMesh.h"
#include "openglnode.h"
//#include "Printer.h"
#include "ConsistencyGuard.h"

SO_NODE_SOURCE(MeshCrossSection);

void MeshCrossSection::basePointSensorCallback(void* data, SoSensor* sensor)
{
    MeshCrossSection* xsec = (MeshCrossSection*)data;
    xsec->basepointChanged();
}

void MeshCrossSection::normalSensorCallback(void* data, SoSensor* sensor)
{
    MeshCrossSection* xsec = (MeshCrossSection*)data;
    xsec->normalChanged();
}

SbPlane MeshCrossSection::getDefaultPlane()
{
  return SbPlane(SbVec3f(1.0f, 0.0f, 0.0f), 0.0f);
}

SbVec3f MeshCrossSection::getDefaultBasePoint()
{
  return SbVec3f(0.0f, 0.0f, 0.0f);
}

SbVec3f MeshCrossSection::getDefaultNormal()
{
  return SbVec3f(0.0f, 1.0f, 0.0f);
}

void MeshCrossSection::basepointChanged()
{
  planeChanged();
}

void MeshCrossSection::normalChanged()
{
    if(m_manager != 0)
    {
        for (auto node : m_formationCrossSections)
        {
            FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
            int id = xsec->tensorVectorSetId.getValue();
            if(id >= 0)
                m_manager->setVectorDataSetNormal(id, normal.getValue());
            id = xsec->tensorVectorScalarSetId.getValue();
            if (id >= 0)
                m_manager->setVectorSignDataSetNormal(id, normal.getValue());
        }
    }

    planeChanged();
}

void MeshCrossSection::planeChanged()
{
    if(!m_updatingFromParent)
    {
        SbVec3f p = basepoint.getValue();
        SbVec3f n = normal.getValue();

        SbVec3d t = m_manager->GetTranslation();

        m_parent->SetScreen(
            geo::CPoint(p[0] + t[0], p[1] + t[1], p[2] + t[2]),
            geo::CVector(n[0], n[1], n[2]),
            false);
    }
}

void MeshCrossSection::initClass()
{
  SO_NODE_INIT_CLASS(MeshCrossSection, SoGroup, "Group");
}

void MeshCrossSection::exitClass()
{
  SO__NODE_EXIT_CLASS(MeshCrossSection);
}

/*bool is_model_in_scope( const OIDIMeshVU* volumeMesh )
{
	bool res = false;
	const CModelBase* model = static_cast<const CModelBase*>( (CModelBase*)&volumeMesh->getOpenGLNode()->Model() );
	if( model )
	{
		CConsistencyGuard* guard = ((CModelBase*)model)->GetConsistencyGuard();
		res = guard && guard->is_model_in_scope( CConsistencyGuard::XSecImprovement );
	}
	return res;
}*/

MeshCrossSection* MeshCrossSection::build(OIDIMeshNodeManager& mgr, CCrossSection* parent) //TODO: is it redone when swapingof model ? 
{
  MeshCrossSection* mcs = new MeshCrossSection;
  mcs->m_manager = &mgr;
  mcs->m_parent = parent;

  const OIDISet& meshSet = mgr.getMeshSet();

  //bool op_done = false;
  //bool is_model_in_scope_ = false;

  // Add a cross section for every volume mesh in the mesh set.
  size_t n = meshSet.NrOfMesh();
  for(size_t i=0; i < n; ++i)
  {
    const OIDIMesh* mesh = meshSet.getMesh(i);
    const OIDIMeshVU* volumeMesh = dynamic_cast<const OIDIMeshVU*>(mesh);

    if(volumeMesh != 0)// && mesh != mgr.getMainVolumeMeshNode()->getMesh())
    {
		/*if( !op_done )
		{
			is_model_in_scope_ = is_model_in_scope( volumeMesh );
			op_done = true;
		}

		if( is_model_in_scope_ && !mgr.IsVisible( volumeMesh->getOpenGLNode() ) )
			continue;*/

		const CWellCasingModel* casing_model = dynamic_cast<const CWellCasingModel*>( &volumeMesh->getOpenGLNode()->Model() );
		if( casing_model && !mgr.IsVisible( volumeMesh->getOpenGLNode() ) )
			continue;

		/*const MbVec3d& mn = volumeMesh->getMin();
		const MbVec3d& mx = volumeMesh->getMax();
		const MbVec3d dif = mx-mn;

		Printer::instance()->debug("%s : min: (%.2f,%.2f,%.2f) - max: (%.2f,%.2f,%.2f) - dif: (%.2f,%.2f,%.2f)", 
			volumeMesh->name().c_str(), mn[0],mn[1],mn[2], mx[0],mx[1],mx[2], dif[0],dif[1],dif[2]);*/

      FormationCrossSection* fcs = new FormationCrossSection;
      fcs->setMesh(volumeMesh);
      mcs->add(fcs);
    }
  }

  mcs->updateFromParent();

  return mcs;
}

MeshCrossSection::MeshCrossSection()
    : m_manager(0)
    , m_parent(0)
    , m_formationCrossSections(new SoGroup)
    , m_clipPlane(new SoClipPlane)
    , m_updatingFromParent(false)
    , m_tensorVectorMode(TensorVectorFull)
{
  SO_NODE_CONSTRUCTOR(MeshCrossSection);
  SO_NODE_ADD_FIELD(plane, (getDefaultPlane()));
  SO_NODE_ADD_FIELD(basepoint, (getDefaultBasePoint()));
  SO_NODE_ADD_FIELD(normal, (getDefaultNormal()));
  SO_NODE_ADD_FIELD(clip, (false));
  SO_NODE_ADD_FIELD(vectorScale, (1.0f));
  SO_NODE_ADD_FIELD(vectorFilter, (1.0f));
  SO_NODE_ADD_FIELD(showEdges, (true));
  SO_NODE_ADD_FIELD(showVectors, (false));
  SO_NODE_ADD_FIELD(showTensorVectors, (false));
  SO_NODE_ADD_FIELD(showOutline, (false));
  SO_NODE_ADD_FIELD(showIsoline, (false));
  SO_NODE_ADD_FIELD(coloring, (COLOR));

  SO_NODE_DEFINE_ENUM_VALUE(ColoringType, COLOR);
  SO_NODE_DEFINE_ENUM_VALUE(ColoringType, CONTOURING);
  SO_NODE_SET_SF_ENUM_TYPE(coloring, ColoringType);

  m_clipPlane->on.connectFrom(&clip);
  m_clipPlane->plane.connectFrom(&plane);

  m_basepointSensor = new SoFieldSensor(basePointSensorCallback, this);
  m_basepointSensor->setPriority(0);
  m_basepointSensor->attach(&basepoint);

  m_normalSensor = new SoFieldSensor(normalSensorCallback, this);
  m_normalSensor->setPriority(0);
  m_normalSensor->attach(&normal);

  addChild(m_formationCrossSections);
  addChild(m_clipPlane);
}

const CCrossSection* MeshCrossSection::getParent() const
{
  return m_parent;
}

void MeshCrossSection::setColor(const MiMesh* formationMesh, SbColor color)
{
    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    if(xsec->getMesh() == formationMesh)
      xsec->color = color;
  }
}

void MeshCrossSection::setScalarSetId(const MiMesh* formationMesh, int scalarSetId)
{
    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    if(xsec->getMesh() == formationMesh)
      xsec->colorScalarSetId = scalarSetId;
  }
}

int MeshCrossSection::getScalarSetId(const MiMesh* formationMesh) const
{
    for (auto node : m_formationCrossSections)
    {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
        if(xsec->getMesh() == formationMesh)
            return xsec->colorScalarSetId.getValue();
    }

    return -1;
}

void MeshCrossSection::setVectorSetId(const MiMesh* formationMesh, int vectorSetId)
{
    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    if(xsec->getMesh() == formationMesh)
      xsec->vectorSetId = vectorSetId;
  }
}

int MeshCrossSection::getVectorSetId(const MiMesh* formationMesh) const
{
    for (auto node : m_formationCrossSections)
    {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
        if(xsec->getMesh() == formationMesh)
            return xsec->vectorSetId.getValue();
    }

    return -1;
}

void MeshCrossSection::setTensorVectorSetId(const MiMesh* formationMesh, int tensorVectorSetId, int colorScalarSetId)
{
    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    if(xsec->getMesh() == formationMesh)
    {
      xsec->tensorVectorSetId = tensorVectorSetId;
      xsec->tensorVectorScalarSetId = colorScalarSetId;

      if(m_manager != 0)
      {
          SbVec3f normalVec = normal.getValue();
          m_manager->setVectorDataSetNormal(tensorVectorSetId, normalVec);
          m_manager->setVectorSignDataSetNormal(colorScalarSetId, normalVec);
      }
    }
  }
}

void MeshCrossSection::getTensorVectorSetIds(const MiMesh* formationMesh, int& vectorSetId, int& scalarSetId) const
{
    for (auto node : m_formationCrossSections)
    {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
        if(xsec->getMesh() == formationMesh)
        {
            scalarSetId = xsec->tensorVectorScalarSetId.getValue();
            vectorSetId = xsec->tensorVectorSetId.getValue();
            return;
        }
    }

    // not found...
    scalarSetId = -1;
    vectorSetId = -1;
}

void MeshCrossSection::setIsoScalarSetId(const MiMesh* formationMesh, int isoScalarSetId)
{
    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    if(xsec->getMesh() == formationMesh)
      xsec->isoScalarSetId = isoScalarSetId;
  }
}

void MeshCrossSection::setIsoValues(int count, float minval, float maxval)
{
  std::vector<float> values;
  for(int i=0; i < count; ++i)
    values.push_back(minval + i * (maxval - minval) / count);

    for (auto node : m_formationCrossSections)
  {
        FormationCrossSection* xsec = static_cast<FormationCrossSection*>(node);
    xsec->isovalues.setValues(0, count, &values[0]);
  }
}

MeshCrossSection::~MeshCrossSection()
{
}

void MeshCrossSection::add(FormationCrossSection* fmtXSec)
{
  fmtXSec->plane.connectFrom(&plane);
  fmtXSec->coloring.connectFrom(&coloring);
  fmtXSec->showEdges.connectFrom(&showEdges);
  fmtXSec->showVectors.connectFrom(&showVectors);
  fmtXSec->showTensorVectors.connectFrom(&showTensorVectors);
  fmtXSec->showOutline.connectFrom(&showOutline);
  fmtXSec->showIsoline.connectFrom(&showIsoline);
  fmtXSec->vectorScale.connectFrom(&vectorScale);
  fmtXSec->vectorFilter.connectFrom(&vectorFilter);

  m_formationCrossSections->addChild(fmtXSec);
}

void MeshCrossSection::updateFromParent()
{
    m_updatingFromParent = true;

    // Extract plane parameters from Geomec CCrossSection instance
    geo::CPlane parentPlane = m_parent->IntersectPlaneScreen();
    const geo::IPoint& point = parentPlane.BasePoint();
    const geo::IVector& normalVec = parentPlane.Normal();

    const SbVec3d translation = m_manager->GetTranslation();

    SbVec3f p(point.X() - translation[0], point.Y() - translation[1], point.Z() - translation[2]);
    SbVec3f n(normalVec.X(), normalVec.Y(), normalVec.Z());

    basepoint = p;
    normal = n;
    plane = SbPlane(n, p);

    m_updatingFromParent = false;
}

CCrossSection::OrientationType MeshCrossSection::getOrientation() const
{
    return m_parent->OrientationScreen();
}

void MeshCrossSection::updateDisplaySettings(const MeshNodeSettings& meshNodeSettings)
{
  showEdges   = meshNodeSettings.meshLinesDisplayed;
  showOutline = meshNodeSettings.formationOutlinesDisplayed;
  showIsoline = meshNodeSettings.contourLineModeSelected;        

  vectorScale  = meshNodeSettings.vectorBaseScale * meshNodeSettings.vectorScale;
  vectorFilter = meshNodeSettings.vectorVisibleFraction;

  setIsoValues(meshNodeSettings.isoCount, (float)meshNodeSettings.dataMin, (float)meshNodeSettings.dataMax);
  if (meshNodeSettings.tensorVectorMode != m_tensorVectorMode)
  {
      normalChanged();
      
      m_tensorVectorMode = meshNodeSettings.tensorVectorMode;
  }
}

void MeshCrossSection::connectFrom(CrossSectionManipulator* manip)
{
  plane.connectFrom(&manip->plane);
  basepoint.connectFrom(&manip->translation);
  normal.connectFrom(&manip->normal);
}

void MeshCrossSection::disconnect()
{
  plane.disconnect();
  basepoint.disconnect();
  normal.disconnect();
}
