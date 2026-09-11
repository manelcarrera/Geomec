#ifndef _OIDI_SURFACE_MESH_NODE_H__
#define _OIDI_SURFACE_MESH_NODE_H__

#include <MeshVizXLM/MbVec3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>

#include <map>

#include "OIDIMeshNode.h"

class OIDIMeshSU;

class MoMesh;
class MoScalarSetI;
class MoPredefinedColorMapping;
class MoDrawStyle;
class MoMaterial;
class MoMeshSurface;
class MoMeshOutline;

class SoLightModel;
class SoPickStyle;
class SoClipPlaneManip;
class SoSwitch;
class SoSeparator;
class SoMaterial;
class MoMeshAnnotatedIsoline;

class MoCellFilter;
class DecimatingCellFilterI;
class MoMeshVector;

template<class T>
class MiDataSetI;


class OIDISurfaceMeshNode : public OIDIMeshNode
{
    // Cell shape members
    SoSwitch*    m_surfaceSwitch;
    SoSeparator* m_surfaceSeparator;
    SoGroup*     m_surfaceAppearance;
    MoDrawStyle* m_surfaceDrawStyle;
    MoMaterial*  m_surfaceMaterial;
    SoMaterial*  m_soSurfaceMaterial;
    MoMeshSurface* m_surface;
    SoSwitch *   m_isolineSwitch;
    MoMeshAnnotatedIsoline *m_isoline;

    void buildSurfaceAppearanceNode();

    bool isPointSetMeshNode();

    void buildSurfaceNode();

    void buildIsolineNode();

    void buildSurfaceMeshNode();
    void buildNodes();

public:

    OIDISurfaceMeshNode(const OIDIMesh* meshData, OIDIMeshNodeManager * meshNodeManager);

    virtual void updateColorScalarSet();

    virtual void updateIsoScalarSet();


    virtual void setIsoValues(int isoCount, double min, double max);
    virtual void setIsoGap(double gap);

    virtual void UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor);
    virtual void updateGeologyColors();

};

#endif
