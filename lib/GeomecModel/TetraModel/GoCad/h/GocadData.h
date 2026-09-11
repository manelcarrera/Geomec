#ifndef _GOCAD_DATA_H_
#define _GOCAD_DATA_H_

#include <list>
#include <map>
#include <vector>

#include <qstring.h>

#include "StorageNode.h"
#include <qsharedpointer.h>

#ifdef SKUA_NEW
namespace gm_skua {
struct SKUAParseData;
}
#endif

class CGocadData {
public:
  typedef enum { IS_NONE = 0, IS_VERTEX, IS_ATOM, IS_SHAREDVERTEX } NODE_TYPE;

  class CVertex;
  class IGoCadTypes;

  class CNode {
  public:
    CNode() {};
    virtual ~CNode() {}

    virtual double X() const = 0;
    virtual double Y() const = 0;
    virtual double Z() const = 0;

    virtual CGocadData::NODE_TYPE Type() const { return IS_NONE; }

    virtual const CVertex &Vertex() const = 0;
    virtual void setVertex(CVertex * /*vertex*/) {}

    bool IsEqualTo(const CNode &node) const;

    int ID() const;

    virtual int PropertySize() const;
    virtual const std::vector<double> &Property(int i) const;

    virtual void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
    virtual void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                            IGoCadTypes & /*parent*/) {
      LoadStream(stream, version, progress);
    }
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const;

  protected:
    virtual const CNode *Pointee() const { return 0; }

  private:
    CNode(const CNode &rhs);
    CNode &operator=(const CNode &rhs);

    friend class CGocadData;
    friend class CGocadImport;

    int m_nID;
    std::vector<std::vector<double>> m_vcProperties;
  };

  class CVertex : public CNode {
  public:
    CVertex() {}

    virtual double X() const;
    virtual double Y() const;
    virtual double Z() const;

    virtual CGocadData::NODE_TYPE Type() const { return IS_VERTEX; }

    virtual const CVertex &Vertex() const { return *this; }

    virtual void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                            IGoCadTypes & /* parent */);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const;

  private:
    CVertex(const CVertex &rhs);
    CVertex &operator=(CVertex rhs);

    friend class CGocadData;
    friend class CGocadImport;
    double m_dCoord[3];
  };

  class CAtom : public CNode {
  public:
    CAtom() : m_pVertex(0) {}

    virtual double X() const;
    virtual double Y() const;
    virtual double Z() const;

    virtual CGocadData::NODE_TYPE Type() const { return IS_ATOM; }

    const CVertex &Vertex() const;
    void setVertex(const CVertex *vertex);

    virtual void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                            IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const;

  protected:
    virtual const CNode *Pointee() const;

  private:
    CAtom(const CAtom &rhs);
    CAtom &operator=(CAtom rhs);

    friend class CGocadData;
    friend class CGocadImport;

    const CVertex *m_pVertex;

    CAtom(const CVertex *vertex) : m_pVertex(vertex) {}
  };

  class CSharedVertex : public CNode {
  public:
    CSharedVertex() : m_pVertex(0) {}

    virtual double X() const;
    virtual double Y() const;
    virtual double Z() const;

    virtual CGocadData::NODE_TYPE Type() const { return IS_SHAREDVERTEX; }

    const CVertex &Vertex() const;
    void setVertex(const CVertex *vertex);

    virtual void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                            IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const;

  protected:
    virtual const CNode *Pointee() const;

  private:
    CSharedVertex(const CSharedVertex &rhs);
    CSharedVertex &operator=(CSharedVertex rhs);

    friend class CGocadData;
    friend class CGocadImport;

    const CVertex *m_pVertex;

    CSharedVertex(const CVertex *vertex) : m_pVertex(vertex) {}
  };

  class CTetra {
  public:
    CTetra() {}

    const CNode &Node(int i) const;

    int PropertySize() const;
    const std::vector<double> &Property(int i) const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                    IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CTetra(const CTetra &rhs);
    CTetra &operator=(CTetra rhs);

    friend class CGocadData;
    friend class CGocadImport;

    const CNode *m_Nodes[4];
    std::vector<std::vector<double>> m_vcProperties;

    QString m_strOppositeFace[4];
  };

  class CTriangle {
  public:
    CTriangle() {}
    CTriangle(const CTriangle &rhs);

    const CNode &Node(int i) const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                    IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CTriangle &operator=(CTriangle rhs);

    friend class CGocadData;
    friend class CGocadImport;
    const CNode *m_Nodes[3];
  };

  class CTFace {
  public:
    CTFace() {}
    ~CTFace();

    const QString &Name() const;
    size_t TriangleSize() const;
    const CTriangle &Triangle(size_t i) const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                    IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CTFace(const CTFace &rhs);
    CTFace &operator=(CTFace rhs);

    friend class CGocadData;
    friend class CGocadImport;

    QString m_strName;
    std::vector<QSharedPointer<CTriangle>> m_vcTriangles;
  };

  class CSurface {
  public:
    CSurface();
    ~CSurface();

    const QString &Name() const;
    int TFaceSize() const;
    const CTFace &TFace(int i) const;

    bool DoubleSidedFaultSurface() const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                    IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CSurface(const CSurface &rhs);
    CSurface &operator=(CSurface rhs);

    friend class CGocadData;
    friend class CGocadImport;

    QString m_strName;
    std::vector<QSharedPointer<CTFace>> m_vcTFaces;

    bool m_bDoubleSidedFaultSurface;
  };

  class CTVolume {
  public:
    CTVolume();
    ~CTVolume();

    const QString &Name() const;
    int VertexSize() const;
    const CVertex &Vertex(int i) const;

    int AtomSize() const;
    const CAtom &Atom(int i) const;

    int TetraSize() const;
    const CTetra &Tetra(int i) const;

    bool DoubleSidedFaultVolume() const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                    IGoCadTypes &parent);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CTVolume(const CTVolume &rhs);
    CTVolume &operator=(CTVolume rhs);

    friend class CGocadData;
    friend class CGocadImport;
    QString m_strName;
    std::vector<const CVertex *>
        m_vcVertices; // the pointers of vertices and atoms are owned by a IGoCadTypes derived class
    std::vector<const CAtom *> m_vcAtoms;
    std::vector<QSharedPointer<CTetra>> m_vcTetras;

    bool m_bDoubleSidedFaultVolume;
  };

  class IGoCadTypes {
  public:
    virtual const QString &Name() const;
    virtual int PropertiesSize() const;
    virtual const QString &PropertyName(int i) const;

    double ZFactor() const;
    double UnitFactor(int i) const;

    int VertexIDsSize() const;
    const CVertex *VertexID(int i) const;
    int AtomIDsSize() const;
    const CAtom *AtomID(int i) const;
    int SharedVertexIDsSize() const;
    const CSharedVertex *SharedVertexID(int i) const;
    const CNode *NodeID(int i) const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

    typedef std::map<int, const CVertex *> TVertexMap;
    typedef std::map<int, const CAtom *> TAtomMap;
    typedef std::map<int, const CSharedVertex *> TSharedVertexMap;

    void AddNode(const CVertex *pVertex);
    void AddNode(const CAtom *pAtom);
    void AddNode(const CSharedVertex *pSharedVertex);

  protected:
    virtual ~IGoCadTypes();

  private:
    IGoCadTypes(const IGoCadTypes &rhs);
    IGoCadTypes &operator=(IGoCadTypes rhs);
    IGoCadTypes();

    friend class CGocadData;
    friend class CGocadImport;

    double m_dZFactor; // for ZPOSITIVE == Elevation -> multiply by -1
    double m_dUnitFactor[3];

    QString m_strName;
    std::vector<QString> m_vcPropertyNames;

    std::vector<double> m_vcNoDataValues;
    std::vector<int> m_vcESizes;
    std::vector<QString> m_vcTetraPropertyNames;
    std::vector<double> m_vcTetraNoDataValues;
    std::vector<int> m_vcTetraESizes;

    TVertexMap m_mpVertexIDs; // only VRTX
    TAtomMap m_mpAtomIDs;
    TSharedVertexMap m_mpSharedVertexIDs;
    std::vector<const CNode *> m_vcNodes; // m_mpVertexIDs and m_mpAtomIDs combined
  };

  class CTSolid : public IGoCadTypes {
  public:
    CTSolid(bool bLightTSolid = false);
    virtual ~CTSolid();

    typedef enum eSolidType { TSolid = 0, LightTSolid } TSolidType;
    virtual TSolidType Type() const;

    int NoDataValueSize() const;
    double NoDataValue(int i) const;
    int ESizeSize() const;
    int ESize(int i) const;

    int TetraPropertiesSize() const;
    const QString &TetraPropertyName(int i) const;
    int TetraNoDataValueSize() const;
    double TetraNoDataValue(int i) const;
    int TetraESizeSize() const;
    int TetraESize(int i) const;

    int VolumeSize() const;
    const CTVolume &Volume(int i) const;

    int SurfaceSize() const;
    const CSurface &Surface(int i) const;

    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

    const std::vector<QString> &TetraRegions() const { return m_tetraRegions; }
    const std::map<CTVolume *, int> &TetraRegionMap() const { return m_tetraRegionMap; }

    void MinMaxNodeIDs(int &minID, int &maxID) const;

  private:
    CTSolid(const CTSolid &rhs);
    CTSolid &operator=(CTSolid rhs);

    friend class CGocadData;
    friend class CGocadImport;

    TSolidType
        m_type; // differences are minimal, and mostly outside this class, by routines that process volumes and surfaces

    std::vector<QSharedPointer<CTVolume>> m_vcVolumes;
    std::vector<QSharedPointer<CSurface>> m_vcSurfaces;

    std::vector<QString> m_tetraRegions;
    std::map<CTVolume *, int> m_tetraRegionMap;
  };

  class CVset : public IGoCadTypes // wjrx mantis 2896
  {
  public:
    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CVset(const CVset &rhs);
    CVset &operator=(CVset rhs);

    friend class CGocadData;
    friend class CGocadImport;

    CVset() {}
    virtual ~CVset() {}
  };

  class CTSurf : public IGoCadTypes {
  public:
    void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
    void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

  private:
    CTSurf(const CTSurf &rhs);
    CTSurf &operator=(CTSurf rhs);

    friend class CGocadData;
    friend class CGocadImport;

    CTSurf() {}
    virtual ~CTSurf() {}

    QSharedPointer<CSurface> m_pSurface;
  };
};

#ifdef SKUA_NEW
namespace gm_skua {

void transform(const CGocadData::CTSolid &old_data, struct gm_skua::SKUAParseData &new_data);

void LoadStreamLegacy(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                      struct gm_skua::SKUAParseData &data);

} // namespace gm_skua
#endif

#endif