#include "stdafx.h"
#include "MeshPointProbeGroup.h"
#include "MeshVizXLM\mapping\nodes\MoMeshPointProbe.h"
#include "MeshVizXLM\mapping\nodes\MoMesh.h"
#include "MeshVizXLM\mesh\MiVolumeMeshUnstructured.h"
#include "ValueTracker.h"
#include "CrossSectionManipulator.h"
#include "SoGroupIterator.h"


MeshPointProbe::MeshPointProbe( const std::string & meshName, const MiVolumeMeshUnstructured * miVolumeMesh, ValueTracker * valueTracker ) :
    m_meshName(meshName),
    m_miVolumeMesh(miVolumeMesh),
    m_valueTracker (valueTracker),
    m_default(false)
{
    setName("MeshPointProbe");
    MoMesh * mesh = new MoMesh;
    addChild (mesh);
    mesh->setMesh(miVolumeMesh);
    m_meshPointProbe = new GMoMeshPointProbe;
    addChild(m_meshPointProbe);

    m_meshPointProbe->setProbeCallback(*this);
    SbBool result = m_meshPointProbe->position.connectFrom(&valueTracker->getDragger()->translation);
}

void MeshPointProbe::motionCallback( size_t cellId, const MeXScalardSetI& scalars, const MeXVec3dSetI& vectors )
{
    if (isDefault() || (long long) cellId != -1)
    {
        m_valueTracker->motionCallback(m_meshPointProbe, getMesh(), m_meshName, cellId, scalars, vectors);
    }
}

const MiVolumeMeshUnstructured * MeshPointProbe::getMesh()
{
    return m_miVolumeMesh;
}

void MeshPointProbe::setScalarSetId( int scalarSetId )
{
    m_meshPointProbe->scalarSetIds.setValue(scalarSetId);
}

void MeshPointProbe::setVectorSetId( int vectorSetId )
{
    m_meshPointProbe->vectorSetIds.setValue(vectorSetId);
}

void MeshPointProbe::setTensorSetId( int tensorSetId )
{
    m_meshPointProbe->setTensorSetId(tensorSetId);
}

void MeshPointProbe::setDefault()
{
    m_default = true;
}

bool MeshPointProbe::isDefault()
{
    return m_default;
}

MeshPointProbeGroup::MeshPointProbeGroup() : SoSwitch()
{
    setName("MeshPointProbeGroup");
}

void MeshPointProbeGroup::addProbe( const std::string & meshName, const MiVolumeMeshUnstructured * miVolumeMesh, ValueTracker * valueTracker )
{
    MeshPointProbe * probe = new MeshPointProbe (meshName, miVolumeMesh, valueTracker);
    addChild (probe);
    if (size(this)== 1)
    {
        probe->setDefault();
    }
}

void MeshPointProbeGroup::removeProbe( const MiVolumeMeshUnstructured * miVolumeMesh )
{
    for (auto node : this)
    {
        MeshPointProbe * probe = static_cast<MeshPointProbe *> (node);
        if (probe->getMesh() == miVolumeMesh)
        {
            removeChild(probe);
            break;
        }
    }

    if (!empty(this))
    {
        MeshPointProbe * probe = static_cast<MeshPointProbe *> (* begin(this));
        probe->setDefault();
    }
}

void MeshPointProbeGroup::enable( bool state)
{
    whichChild = state ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}


void MeshPointProbeGroup::setScalarSetId( const MiVolumeMeshUnstructured * miVolumeMesh, int scalarSetId )
{
    for (auto node : this)
    {
        MeshPointProbe * probe = static_cast<MeshPointProbe *> (node);
        if (probe->getMesh() == miVolumeMesh)
        {
            probe->setScalarSetId (scalarSetId);
            break;
        }
    }
}

void MeshPointProbeGroup::setVectorSetId( const MiVolumeMeshUnstructured * miVolumeMesh, int vectorSetId )
{
    for (auto node : this)
    {
        MeshPointProbe * probe = static_cast<MeshPointProbe *> (node);
        if (probe->getMesh() == miVolumeMesh)
        {
            probe->setVectorSetId (vectorSetId);
            break;
        }
    }
}

void MeshPointProbeGroup::setTensorSetId( const MiVolumeMeshUnstructured * miVolumeMesh, int tensorSetId )
{
    for (auto node : this)
    {
        MeshPointProbe * probe = static_cast<MeshPointProbe *> (node);
        if (probe->getMesh() == miVolumeMesh)
        {
            probe->setTensorSetId (tensorSetId);
            break;
        }
    }
}

void GMoMeshPointProbe::doAction( SoAction *action )
{
    // To prevent our probe from being triggered inadvertently
    if (action->isOfType(SoGetBoundingBoxAction::getClassTypeId()))
        return;
    MoMeshPointProbe::doAction(action);
}

void GMoMeshPointProbe::setTensorSetId( int id )
{
    m_tensorSetId = id;
}

int GMoMeshPointProbe::getTensorSetId()
{
    return m_tensorSetId;
}
