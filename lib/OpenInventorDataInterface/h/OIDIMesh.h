#ifndef _OIDI_MESH_H__
#define _OIDI_MESH_H__

#include <vector>
#include <assert.h>

#include <Inventor/SbVec.h>

#include <MeshVizXLM/mesh/geometry/MiGeometryI.h>
#include <MeshVizXLM/mesh/MiVolumeMeshUnstructured.h>
#include <MeshVizXLM/mesh/MiSurfaceMeshUnstructured.h>
#include <MeshVizXLM/mesh/MiLineMeshUnstructured.h>
#include <MeshVizXLM/mesh/data/MiDataSetI.h>
#include <MeshVizXLM/MxTimeStamp.h>

#include "IObject.h"
#include "IBody.h"
#include "IFace.h"
#include "Point.h"
#include "IElementSet.h"
#include "IVectorResult.h"
#include "ITensorGroup.h"
#include "BodyGroup.h"
#include "MeshSurface.h"
#include "Line.h"
#include "IBox.h"
#include "InterfaceElement.h"

#include "OIDICell.h"
#include "OIDITopology.h"
#include "OIDIGeometry.h"
#include "OIDIDataSetI.h"

#include <QHash.h>

#include <vector>

#include <QString>

class COpenGLNode;
class IValueComponentBase;
class MiPointProbeUnstructured;

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

MbVec3d computeAverage(const std::vector<geo::CVector>& vectors);
double computeAverage(const IValueDataInterfaceScalar::TValueVec& valueVec, bool isInterfaceElt = false);




// Everything not dependent on any template parameter is in OIDIMesh
class OIDIMesh
{
private:
    const COpenGLNode *m_openGLNode;

    const OIDIGeometryI m_geometry;

    const geo::IObject * m_objectSet;

protected:
    mutable MbVec3d m_min;
    mutable MbVec3d m_max;

public:
    OIDIMesh (const COpenGLNode *openGLNode, const geo::IElementSet *elementSet) : m_openGLNode(openGLNode), m_geometry(elementSet), m_objectSet(elementSet), m_min(DBL_MAX), m_max(-DBL_MAX) 
    {
    }

    OIDIMesh (const COpenGLNode *openGLNode, const geo::IArray *pointSet) : m_openGLNode(openGLNode), m_geometry(pointSet), m_objectSet(pointSet), m_min(DBL_MAX), m_max(-DBL_MAX)
    {
    }

    OIDIMesh (const COpenGLNode *openGLNode, const geo::IBox *boxSet) : m_openGLNode(openGLNode), m_geometry(boxSet), m_objectSet(boxSet), m_min(DBL_MAX), m_max(-DBL_MAX)
    {
    }

    virtual ~OIDIMesh() {}

    enum MeshType
    {
        Unknown = 0,
        LineMesh = 1,
        SurfaceMesh = 2,
        VolumeMesh = 3,
        PointSetMesh = 4,
        PointMesh = 5
    };

    // To avoid dynamic_casts ...
    virtual MeshType meshType() const
    {
        return Unknown;
    }

    const COpenGLNode *getOpenGLNode() const { return m_openGLNode; }

    const OIDIGeometryI& getGeometry() const { return m_geometry; }

    size_t getSize() const { return m_geometry.getSize(); }

    virtual const MiCell * getCell(size_t id) const = 0;

    const geo::IObject * getObjectSet() const
    {
        return m_objectSet;
    }

    MbVec3d getCoord(size_t i) const { return m_geometry.getCoord(i); }

    virtual OIDIScalarSetI* collectValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const = 0;
    virtual OIDIScalarSetI* collectGeometryValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const = 0;
    virtual OIDIVectorSetI* collectVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit,IProgressBase & progress) const = 0;
    virtual OIDITensorSetI* collectTensorsByTopology(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const = 0;
    virtual OIDIVectorSetI* collectGeometryVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit,IProgressBase & progress) const = 0;

    // Ditto, we don't own the returned pointer
    virtual OIDIScalarSetI* collectValuesByGeometry(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDIVectorSetI* collectVectorsByGeometry(const IVectorResult::IVectorComponent* pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDITensorSetI* collectTensorsByGeometry(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    const MbVec3d & getMax() const
    {
        if (m_max[0] == -DBL_MAX)
            m_max = getGeometry().getMax();

        //         if (m_max[0] == -DBL_MAX)
        //         {
        //             checkConsistency(m_min, m_max);           
        //         }

        return m_max;           
    }

    const MbVec3d & getMin() const
    {
        if (m_min[0] == DBL_MAX)
            m_min = getGeometry().getMin();
        //         if (m_min[0] == DBL_MAX)

        //         {
        //             checkConsistency(m_min, m_max);
        //         }   

        return m_min;
    }

    virtual void checkConsistency (MbVec3d & min, MbVec3d & max) const = 0;

    virtual size_t getNumCells() const = 0;

	std::string name() const{ return m_openGLNode->Name().toStdString(); };
};

/// Template base class for all OIDI...Mesh classes
/// OIDMesh contains everything that does not depend on any of the template parameters
template < class TopologyType, class CellType, class MiMeshUnstructuredType >
class OIDIMeshTemplate : public MiMeshUnstructuredType, public OIDIMesh
{
private:
    const TopologyType m_topology;

public:
    OIDIMeshTemplate(const COpenGLNode *openGLNode, const geo::IElementSet *elementSet, const std::vector<CellType >& cells)
        : OIDIMesh(openGLNode, elementSet), m_topology(elementSet, cells) {}
    OIDIMeshTemplate(const COpenGLNode *openGLNode, const geo::IArray *pointSet, const std::vector<CellType >& cells)
        : OIDIMesh(openGLNode, pointSet), m_topology(pointSet, cells) {}
    OIDIMeshTemplate(const COpenGLNode *openGLNode, const geo::IBox *boxSet, const std::vector<CellType >& cells)
        : OIDIMesh(openGLNode, boxSet), m_topology(boxSet, cells) {}
    ~OIDIMeshTemplate() {}

    const OIDIGeometryI& getGeometry() const
    {
        return OIDIMesh::getGeometry();
    }

    const TopologyType & getTopology() const
    {
        return m_topology;
    }

    virtual const CellType * getCell(size_t id) const
    {
        return m_topology.getCell(id);
    }

    virtual size_t getNumCells() const
    {
        return m_topology.getNumCells();
    }

    void checkConsistency(MbVec3d & min, MbVec3d & max) const;

    virtual OIDIScalarSetI* collectValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDIScalarSetI* collectGeometryValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    // Again, we don't own the returned pointer.
    virtual OIDIVectorSetI* collectVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDIVectorSetI* collectGeometryVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    virtual OIDITensorSetI* collectTensorsByTopology( const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress ) const;
    virtual OIDITensorSetI* collectGeometryTensorsByTopology(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
};

// Could also use typedefs, e.g.
// typedef OIDIMesh<OIDIVolumeTopologyExplicitI,OIDIVolumeCell,MiVolumeMeshUnstructured> OIDIMeshVU;
// Unfortunately, we cannot forward-declare typedefs as we can classes.

class OIDIMeshVU : public OIDIMeshTemplate<OIDIVolumeTopologyExplicitI, OIDIVolumeCell, MiVolumeMeshUnstructured>
{
private:
    mutable int m_lastFound;
    mutable MiPointProbeUnstructured * m_probe;
public:
    OIDIMeshVU(const COpenGLNode *openGLNode, const geo::IElementSet *elementSet, const std::vector<OIDIVolumeCell>& cells) 
        : OIDIMeshTemplate(openGLNode, elementSet, cells), m_probe(0)  
    {
        checkConsistency(m_min, m_max);
        m_lastFound = -1;

    }
    OIDIMeshVU(const COpenGLNode *openGLNode, const geo::IArray *pointSet, const std::vector<OIDIVolumeCell>& cells) 
        : OIDIMeshTemplate(openGLNode, pointSet, cells)   
    {
        checkConsistency(m_min, m_max);
    }
    OIDIMeshVU(const COpenGLNode *openGLNode, const geo::IBox *pointSet, const std::vector<OIDIVolumeCell>& cells) 
        : OIDIMeshTemplate(openGLNode, pointSet, cells)   
    {
        checkConsistency(m_min, m_max);
    }

    ~OIDIMeshVU();

  	virtual OIDIMesh::MeshType meshType() const;

    bool computeCellMap( std::string meshName, const OIDIMeshVU * mainMesh, std::vector<int> & cellMap ) const;
    int findSimilarCell( const OIDIVolumeCell * cell ) const;

    MiPointProbeUnstructured * getProbe() const;


    OIDIScalarSetI* collectValuesByGeometry(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    OIDIVectorSetI* collectVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDITensorSetI* collectTensorsByGeometry(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
};


class OIDIMeshSU : public OIDIMeshTemplate<OIDISurfaceTopologyExplicitI, OIDISurfaceCell, MiSurfaceMeshUnstructured>
{
public:
    OIDIMeshSU(const COpenGLNode *openGLNode, const geo::IElementSet *elementSet, const std::vector<OIDISurfaceCell>& cells) 
        : OIDIMeshTemplate(openGLNode, elementSet, cells)
    {
        checkConsistency(m_min, m_max);
    }
    OIDIMeshSU(const COpenGLNode *openGLNode, const geo::IArray *facesSet, const std::vector<OIDISurfaceCell>& cells) 
        : OIDIMeshTemplate(openGLNode, facesSet, cells)
    {
        checkConsistency(m_min, m_max);
    }
    virtual OIDIMesh::MeshType meshType() const;

    virtual bool isFault() const;
    virtual bool needFault(const CGraphNode *node) const;

    virtual OIDIScalarSetI* collectValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    OIDIScalarSetI* collectFaultedValuesByTopology( IProgressBase & progress, const IValueComponentBase * pValueComponent ) const;

    OIDIScalarSetI* collectValuesByGeometry(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    OIDIScalarSetI* collectFaultedValuesByGeometry( const IValueComponentBase * pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress ) const;


    // Again, we don't own the returned pointer.
    virtual OIDIVectorSetI* collectVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    OIDIVectorSetI* collectFaultedVectorsByTopology( const IVectorResult::IVectorComponent * pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress ) const;

    virtual OIDIVectorSetI* collectVectorsByGeometry(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;

    OIDIVectorSetI* collectFaultedVectorsByGeometry( const IVectorResult::IVectorComponent * pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
    virtual OIDITensorSetI* collectTensorsByGeometry(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const;
};

class OIDIMeshLU : public OIDIMeshTemplate<OIDILineTopologyExplicitI, OIDILineCell, MiLineMeshUnstructured>
{

public:
    OIDIMeshLU(const COpenGLNode *openGLNode, const geo::IElementSet *elementSet, const std::vector<OIDILineCell>& cells) : OIDIMeshTemplate(openGLNode, elementSet, cells)
    {
        checkConsistency(m_min, m_max);
    }
    OIDIMeshLU(const COpenGLNode *openGLNode, const geo::IArray *lineSet, const std::vector<OIDILineCell>& cells) : OIDIMeshTemplate(openGLNode, lineSet, cells)
    {
        checkConsistency(m_min, m_max);
    }
    OIDIMeshLU(const COpenGLNode *openGLNode, const geo::IBox *lineSet, const std::vector<OIDILineCell>& cells) : OIDIMeshTemplate(openGLNode, lineSet, cells)
    {
        checkConsistency(m_min, m_max);
    }
	virtual OIDIMesh::MeshType meshType() const;
};




class OIDIPointSetMesh : public MiSurfaceMeshUnstructured, public OIDIMesh
{
    OIDIPointSetTopology m_topology;

public:
    OIDIPointSetMesh (const COpenGLNode *openGLNode, const geo::IArray *pointSet) : OIDIMesh(openGLNode, pointSet), m_topology(pointSet)
    {
        checkConsistency(m_min, m_max);
    }

    virtual const MiSurfaceTopologyExplicitI& getTopology () const
    {
        return m_topology;
    }

    virtual size_t getNumCells() const
    {
        return m_topology.getNumCells();
    }

    virtual const MiCell * getCell(size_t id) const
    {
        return m_topology.getCell(id);
    }

    virtual OIDIMesh::MeshType meshType() const
    {
        return OIDIMesh::PointSetMesh;
    }

    const OIDIGeometryI& getGeometry() const
    {
        return OIDIMesh::getGeometry();
    }

    void checkConsistency( MbVec3d & min, MbVec3d & max ) const
    {
        min = DBL_MAX;
        max = -DBL_MAX;

        std::size_t numCells = getNumCells();
        for (std::size_t c = 0; c < numCells; c++)
        {
            const MiCell * cell = getCell (c);

            std::size_t numCellNodes = cell->getNumNodes();
            for(std::size_t n = 0; n < numCellNodes; ++n)
            {
                std::size_t id = cell->getNodeIndex(n);
                MbVec3d coord = getGeometry().getCoord(id);
                for (int j = 0; j < 3; ++j)
                {
                    min[j] = std::min(min[j], coord[j]);
                    max[j] = std::max(max[j], coord[j]);
                }
            }
        }
    }

    virtual OIDIScalarSetI* collectValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const
    {
        return 0;
    }

    virtual OIDIScalarSetI* collectGeometryValuesByTopology(const IValueComponentBase *pValueComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const
    {
        return collectValuesByGeometry(pValueComponent, unit, progress);
    }


    virtual OIDIVectorSetI* collectVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const
    {
        return 0;
    }

    virtual OIDIVectorSetI* collectGeometryVectorsByTopology(const IVectorResult::IVectorComponent *pVectorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const
    {
        return collectVectorsByGeometry(pVectorComponent, unit, progress);
    }

    virtual OIDITensorSetI* collectTensorsByTopology(const IValueComponentBase* pTensorComponent, IQuantityDouble::UNIT unit, IProgressBase & progress) const
    {
        return 0;
    }
};

geo::CPtrArray<geo::IPoint> * CreateArray( const geo::IPoint &Point );

/// class for the display of analysis points
/// implemented in terms of OIDIPointMesh by creating multiple the original point three times
/// Note: we (probably) need a single point as a mesh, instead of an SoPointSet derivative, for X-sections
class OIDIPointMesh : public OIDIPointSetMesh
{
    geo::IArray * const m_pointSet;

    OIDIPointMesh (const COpenGLNode *openGLNode, geo::IArray *pointSet) :
        OIDIPointSetMesh (openGLNode, pointSet), m_pointSet(pointSet)
    {
    }

public:
    ~OIDIPointMesh()
    {
        assert (m_pointSet);
        m_pointSet->ClearAndDelete();
        delete m_pointSet;
    }

    virtual OIDIMesh::MeshType meshType() const
    {
        return OIDIMesh::PointMesh;
    }

    // Couldn't do this directly in a constructor
    static OIDIPointMesh * Create( const COpenGLNode * openGLNode, const geo::IPoint & Point )
    {
        return new OIDIPointMesh (openGLNode, CreateArray (Point));
    }
};

void ScaleTensor(CTensor &v, IQuantityDouble::UNIT unit, const ITensorGroup * pTensorGroup);


#endif
