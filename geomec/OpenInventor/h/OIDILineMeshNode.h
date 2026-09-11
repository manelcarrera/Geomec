#ifndef _OIDI_LINE_MESH_NODE_H__
#define _OIDI_LINE_MESH_NODE_H__

#include <MeshVizXLM/MbVec3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>

#include <map>

#include "OIDIMeshNode.h"

class OIDIMeshLU;

class MoMesh;
class MoScalarSetI;
class MoPredefinedColorMapping;
class MoDrawStyle;
class MoMaterial;
class MoMeshSurface;
class MoMeshLine;

class SoLightModel;
class SoPickStyle;
class SoClipPlaneManip;
class SoSwitch;
class SoSeparator;
class SoDrawStyle;

class MoCellFilter;
class DecimatingCellFilterI;
class MoMeshVector;

template<class T>
class MiDataSetI;


class OIDILineMeshNode : public OIDIMeshNode
{
    // Cell shape members
    SoSwitch*    m_visibilitySwitch;

    SoSeparator* m_meshLineSeparator;
    SoGroup*     m_appearanceGroup;
    MoDrawStyle* m_meshLineDrawStyle;
    MoMaterial*  m_meshLineMaterial;
    SoDrawStyle * m_lineDrawStyle;
    MoMeshLine * m_outline;

    void buildSurfaceAppearanceNode();
    void buildSurfaceNode();


    void buildSurfaceMeshNode();
    void buildNodes();

public:

    explicit OIDILineMeshNode(const OIDIMesh* meshData, OIDIMeshNodeManager * meshNodeManager);

    virtual void updateColorScalarSet();

    virtual void updateIsoScalarSet();


    virtual void setIsoValues(int isoCount, double min, double max);
    virtual void setIsoGap(double gap);

    virtual void UpdateDisplaySettings(const MeshNodeSettings & meshNodeSettings, bool showColor);
    virtual void updateGeologyColors();
    void buildOutlineAppearanceNode();
};

#endif
