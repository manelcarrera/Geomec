// GocadExport.h: interface for the CGocadExport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADEXPORT_H__B4CD2DF5_19EF_445C_A615_49DF251D8CD6__INCLUDED_)
#define AFX_GOCADEXPORT_H__B4CD2DF5_19EF_445C_A615_49DF251D8CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITensorGroup.h"
#include "LocalResult.h"

class CModelBase;
class CDepletionStage;
class C3DFormation;
class CFormationVolume;
class CFormationBase;
class C3DHorizon;

class CHorizonBase;
class CPointSet;

#include <vector>

#include "BodyGroup.h"
#include "ivaluecomponent.h"

class CGocadExport {
public:
  typedef std::vector<IValueDomainScalar::TValueVec> TElementValueVec;

private:
  class CNodeBase {
    int m_nId;

  protected:
    void AppendValues(FILE *fp, const TElementValueVec &vcElementValues, int nLocalIndex) const;

  public:
    CNodeBase();
    CNodeBase(const CNodeBase &rhs);
    virtual ~CNodeBase();

    virtual void Write(FILE *fp, int iUnit, const TElementValueVec &vcElementValues, int nLocalIndex) const = 0;
    int Id() const;
    void Id(int id);
  };

  class CVertex : public CNodeBase {
    const geo::INode &m_node;

  public:
    CVertex(const geo::INode &node);
    CVertex(const CVertex &rhs);

    virtual void Write(FILE *fp, int iUnit, const TElementValueVec &vcElementValues, int nLocalIndex) const;
  };

  class CAtom : public CNodeBase {
    const CVertex &m_vertex;

  public:
    CAtom(const CVertex &vertex);

    virtual void Write(FILE *fp, int iUnit, const TElementValueVec &vcElementValues, int nLocalIndex) const;
  };

  class CTetra {
    const geo::IElement &m_element;
    CNodeBase *m_pNode[4];

  public:
    CTetra(const geo::IElement &element, CNodeBase **pNode);
    ~CTetra();
    void Write(FILE *fp, const IValueDomainScalar::TValueVec &vcAverageElementValues,
               const QString *pstrFormationName) const;
    const geo::IElement &Element() const;
  };

  class CTrgl {
    CNodeBase *m_pNode[3];

  public:
    CTrgl(CNodeBase **pNode);
    ~CTrgl();
    void Write(FILE *fp) const;
  };

  typedef std::set<const CDepletionStage *> TTimeStepSet;

  template <class OBJECT> class CObjectLess {
  public:
    bool operator()(const OBJECT *pLhs, const OBJECT *pRhs) const;
  };

  typedef std::set<const CFormationBase *, CObjectLess<CFormationBase>> TFormationSet;
  typedef std::set<const CHorizonBase *, CObjectLess<CHorizonBase>> THorizonSet;
  typedef std::set<const CPointSet *, CObjectLess<CPointSet>> TPointSetSet;

  CModelBase &m_model;
  TTimeStepSet m_stTimeSteps;
  TFormationSet m_stFormations;
  THorizonSet m_stHorizons;
  TPointSetSet m_stPointSets;
  TResultComponentSet m_stMaterialParameterComponents;
  TResultComponentSet m_stResultComponents;
  bool m_bLinear;
  bool m_bNonLinear;
  bool m_bHeat;
  bool m_bMixture;
  bool m_bMixtureContainment;
  int m_iUnit;
  bool m_bVectorESize;
  bool m_bTensorESize;
  bool m_bAverage;
  bool m_bSplitFormations; // if true, write each formation to a TSolid (with one TVolume)

  typedef std::set<const ITensorGroup::CComponentComposite *> TTensorSet;
  typedef std::set<const IVectorResult *> TVectorSet;

  TTensorSet m_stFullTensors;
  TVectorSet m_stFullVectors;

  FILE *m_fp;

  typedef std::map<const geo::INode *, CNodeBase *> TNodeMap;
  TNodeMap m_mpNodes;

  typedef std::set<const geo::INode *, geo::ICoordinate::CCoordinateLess> TUniqueNodeSet;
  TUniqueNodeSet m_stUniqueNodes;

  typedef std::vector<CTetra *> TElementVec;
  TElementVec m_vcElements;

  int m_nCurrentId;
  bool m_bFormationNames;

private:
  void ExportSolid(IProgressBase &prog, const QString &title);
  void ExportFault(const C3DHorizon &hor, IProgressBase &prog);
  void ExportVSets(IProgressBase &prog);
  QString FixName(const QString &name) const;
  void ClearElements();
  void WriteAxisUnit();
  void WriteVSetHeader(const CPointSet &ptSet);
  void WritePointSets(IProgressBase &prog);
  void WritePointSet(const CPointSet &ptSet, IProgressBase &prog);
  void WriteSolidHeader(const QString &title);
  void WriteFaultHeader(const C3DHorizon &hor);
  void WritePropertiesHeader(bool bSolid, const COpenGLNode &object);
  void WriteFormations(IProgressBase &prog);
  void WriteFormation(const C3DFormation &form, IProgressBase &prog);
  void WriteFault(const C3DHorizon &hor, IProgressBase &prog);
  void AddPointValueFromComponent(const geo::IPoint &point, const IValueComponentBase &comp,
                                  IValueDomainScalar::TValueVec &vcPointValues, const COpenGLNode &object) const;
  void AddPointValuesFromResult(const geo::IPoint &point, const IResult &result, const std::vector<int> &indices,
                                IValueDomainScalar::TValueVec &vcPointValues, const COpenGLNode &object) const;
  void AddElementValuesFromComponent(const geo::IElement &element, const IValueComponentBase &comp,
                                     TElementValueVec &vcElementValues, const COpenGLNode &object) const;
  void AddElementValuesFromResult(const geo::IElement &element, const IResult &result,
                                  const std::vector<int> &vcIndices, TElementValueVec &vcElementValues,
                                  const COpenGLNode &object) const;
  void GetElementValues(const geo::IElement &element, TElementValueVec &vcElementValues,
                        const COpenGLNode &object) const;
  void GetPointValues(const geo::IPoint &point, IValueDomainScalar::TValueVec &vcPointValues,
                      const COpenGLNode &object) const;
  void CreateNodes(const geo::IElement &element, const int *pNodeIndices, CNodeBase **pNode,
                   const TElementValueVec &vcElementValues);
  void WriteVolume(const CFormationVolume &vol, IProgressBase &prog);
  void WriteSolidSurfaces();
  const CNodeBase &Vertex(const geo::INode &node) const;
  void WriteTFace(const geo::CBodyGroup::CSideSurface &surface, int idx);
  void WriteTFaceFault(const CTetraHorizonBase &fault, int idx);
  void WriteTrailer();
  bool HasProperties() const;
  template <class COMPOSITE> void FetchComposites(int nComponents, std::set<const COMPOSITE *> &stComposites);
  void SortComposites();

public:
  CGocadExport(CModelBase &model);
  CGocadExport(const CGocadExport &rhs);
  virtual ~CGocadExport();

  CGocadExport &operator=(const CGocadExport &rhs);
  bool operator==(const CGocadExport &rhs) const;

  void Export(const QString &sPath, const QString &title);

  const CModelBase &Model() const;
  CModelBase &Model();

  void AddDepletionStage(const CDepletionStage &stage);
  void RemoveDepletionStage(const CDepletionStage &stage);
  bool DepletionStage(const CDepletionStage &stage) const;

  void AddPointSet(const CPointSet &ptSet);
  void RemovePointSet(const CPointSet &ptSet);
  void AddFormation(const CFormationBase &formation);
  void RemoveFormation(const CFormationBase &formation);
  void AddHorizon(const CHorizonBase &horizon);
  void RemoveHorizon(const CHorizonBase &horizon);
  bool PointSet(const CPointSet &ptSet);
  bool Formation(const CFormationBase &formation) const;
  bool Horizon(const CHorizonBase &horizon) const;
  bool HasPointSets() const;
  bool HasFormations() const;
  bool HasHorizons() const;
  bool HasFaults() const;
  const CFormationBase &FirstFormation() const;
  const CHorizonBase &FirstFault() const;
  TResultComponentSet &MaterialParameters() { return m_stMaterialParameterComponents; }
  TResultComponentSet &Results() { return m_stResultComponents; }

  void Linear(bool bOn);
  bool Linear() const;

  void NonLinear(bool bOn);
  bool NonLinear() const;

  void Heat(bool bOn);
  bool Heat() const;

  void Mixture(bool bOn);
  bool Mixture() const;

  void MixtureContainment(bool bOn);
  bool MixtureContainment() const;

  void Unit(int iUnit); // 0 = SI, 1 = field
  int Unit() const;

  void VectorESize(bool bOn);
  bool VectorESize() const;

  void TensorESize(bool bOn);
  bool TensorESize() const;

  void Average(bool bOn);
  bool Average() const;

  void SplitFormations(bool bOn);
  bool SplitFormations() const;

  void FormationNames(bool bOn);
  bool FormationNames() const;
};

#endif // !defined(AFX_GOCADEXPORT_H__B4CD2DF5_19EF_445C_A615_49DF251D8CD6__INCLUDED_)
