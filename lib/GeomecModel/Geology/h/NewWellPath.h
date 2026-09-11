#ifndef NEWWELLPATH_H
#define NEWWELLPATH_H

#include "AngleQuantity.h"
#include "FormationBase.h"
#include "NewWellPathBase.h"
#include "NewWellPoint.h"
#include "NewWellSection.h"
#include "PolyLine.h"
#include "StatusValue.h"
#include "unitnode.h"
#include <list>
#include <vector>

class CWellZoomInModel;
class C3DModel;
class CModelBase;
class CFormationBase;
class CNewWellPath;
class CNewWellPathInput;
class CVectorSymbol;
class CLabelPoint;
class QDate;
class CGroupNode;

namespace geo {
class CConus;
}

class CNewGeoWellPoint : public CNewWellPoint {
private:
  mutable bool m_bInit;
  void InitPoint() const;
  const CModelBase *m_model;

public:
  CNewGeoWellPoint(const CNewWellPath &wellpath, const double &tmd);
  CNewGeoWellPoint(CModelBase &model, const CNewWellPoint &wellpoint);
  CNewGeoWellPoint &operator=(const CNewGeoWellPoint &rhs);
  CNewGeoWellPoint(const CNewGeoWellPoint &rhs);

  virtual ~CNewGeoWellPoint();

  // CNewWellPoint virtual overides
  virtual bool Defined() const;
  virtual void Invalidate();

  // return the formation
  const CFormationBase *Formation() const;
};

//*******************************************************************
//
//  A less clsass for storing results per point and per depletion stage and per analysistype
//
//*******************************************************************
class CNewWellPointResultKey {
  friend class CNewWellPath;
  const CNewWellPoint &m_point;
  const CDepletionStage &m_stage;
  CAnalysisType m_antype;

public:
  CNewWellPointResultKey(const CNewWellPoint &point, const CDepletionStage &stage, const CAnalysisType &antype);
  bool operator<(const CNewWellPointResultKey &rhs) const;
};

//*******************************************************************
//
// a map for storing results(Dstabor and Casing)
// the results are stored per point and per depletion stage
// and per analysis type
//
//*******************************************************************
template <class T> class CNewResultMap {
public:
  typedef std::pair<CNewWellPointResultKey, T> TResultPair;
  typedef std::map<CNewWellPointResultKey, T> TResultMap;

  CNewResultMap(CNewWellPath *wellpath);

  // remove all results for this point
  void RemoveResult(const CNewWellPoint &wellpoint);

  // creates an default result if not existst
  T &GetResult(const CNewWellPoint &Wellpoint, const CDepletionStage &stage, const CAnalysisType &antype) const;
  bool ResultExist(const CNewWellPoint &Wellpoint, const CDepletionStage &stage, const CAnalysisType &antype) const;

  TResultMap &GetResultMap() { return m_Result; }

  void ClearAllResults();

private:
  mutable TResultMap m_Result;
  CNewWellPath *m_pWellPath;
};

//*****************************************************************************

typedef CNewResultMap<CDsbStatusValue> CNewDstaborResultMap;
// typedef CNewResultMap<CDCasingPointResult> CCasingResultMap;

//*****************************************************************************
class CNewFormationSection : public INewWellSection {
public:
  CNewFormationSection(CNewWellPathBase &wellPath, const CFormationBase *pFormation, const double &topTMD,
                       const double &bottomTMD);
  const CFormationBase *Formation() const { return m_pFormation; }
  virtual const CNewWellPoint &Top() const { return m_Top; }
  virtual const CNewWellPoint &Bottom() const { return m_Bottom; }
  virtual std::string GetDescription() const { return m_pFormation->Name().toStdString(); }

private:
  const CFormationBase *m_pFormation;
  CNewWellPoint m_Top;
  CNewWellPoint m_Bottom;
};

class CAngleInfoVector {
private:
  std::vector<std::pair<CAngleQuantity, CAngleQuantity>> m_AziInc;

public:
  size_t Size() const;
  CAngleQuantity Azimuth(size_t index) const;
  CAngleQuantity Inclination(size_t index) const;
  void PushBack(const double &azi, const double &inc);
  void Clear();
};

class CNewWellPath : public CNewWellPathBase, public CColorNode {
  friend class CNewWellPoint;

#if 0
  /* DrawDefs {{{ */
  class CWellPathDrawDef : public COpenGLNode::CDrawDef
  {
  private:
    TColor m_color;
  public:
    CWellPathDrawDef(const COpenGLNode &node)
      : CDrawDef(node)
    {;}
    DrawDecisionFloat LineWidth() const { return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 2.0); };
    virtual void SetColor(const TColor &c) {m_color= c;}
    virtual DrawDecisionVector
      Color(const geo::IObject &/*object*/) const
    { std::vector<TColor> c(2); c[0]= c[1]= m_color; return std::make_pair(true, c); }
  };

  class CLineDrawDef : public COpenGLNode::CDrawDef
  {
  public:
    CLineDrawDef(const COpenGLNode &node)
      : CDrawDef(node)
    {}
    DrawDecisionFloat LineWidth() const { return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 1.0); };
    virtual DrawDecisionVector
      Color(const geo::IObject &/*object*/) const
    { std::vector<TColor> c(2); c[0]= c[1]= qRgb(255, 255, 255); return std::make_pair(true, c); }
    virtual DrawDecisionColor TextColor()const { return std::make_pair(true, qRgb(255,255,255)); }
  };

  class CArrowDrawDef : public CLineDrawDef
  {
  public:
    CArrowDrawDef(const COpenGLNode &node)
      : CLineDrawDef(node)
    {}
    virtual DrawDecisionVector
      Color(const geo::IObject &/*object*/) const
    { std::vector<TColor> c(2); c[0]= qRgb(255, 255, 255); c[1]= qRgb(255, 255, 255); return std::make_pair(true, c); }
    virtual DrawDecisionBool RequireCompile() const {return std::make_pair(true, false);}
  };

  class CLabelDrawDef : public CArrowDrawDef
  {
  public:
    CLabelDrawDef(const COpenGLNode &node)
      : CArrowDrawDef(node)
    {}
    virtual DrawDecisionColor TextColor()const { return std::make_pair(true, qRgb(255,255,255)); }
  };

  class CStaborPointsDrawDef : public COpenGLNode::CDrawDef
  {
    // not used, does this draw points?
  public:
    CStaborPointsDrawDef(const COpenGLNode &node)
      : CDrawDef(node)
    {;}
    DrawDecisionFloat PointWidth() const { return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 8.0); };
    virtual DrawDecisionVector
      Color(const geo::IObject &/*object*/) const
    { std::vector<TColor> c(1); c[0]= qRgb(255, 0, 0); return std::make_pair(true, c); }
  };

  /* }}} */

  COpenGLNode::CDrawDef* OnCreateDrawDef
    ( const geo::IObject& object ) const;
#endif

  const CNewWellPathInput *m_input;

public:
  CNewWellPath(CFemAppModel &model);
  // CNewWellPath(const QString &sName, CFemAppModel &model);
  // CNewWellPath( const QString &sName, CFemAppModel &model, const geo::CPoint &BasePoint);
  // CNewWellPath(const CNewWellPath& newWellPath, CFemAppModel &model);
  // CNewWellPath(const QString &file, const QString &name, CFemAppModel &model, CQuantity::UNIT unit, IProgressBase
  // &dlg);
  CNewWellPath(const CNewWellPathInput &input, CFemAppModel &model);
  virtual ~CNewWellPath();

  const CNewWellPathInput *WellPathInput() const;

  virtual bool Destroy();
  virtual bool CanDisconnectItem(const CGraphNode &item);

private:
  void InitFromWellPathInput(const CNewWellPathInput &input);

  void InitVerticalWell(bool bResetGlobalTVD);
  void InitVertical(double start_northing, double start_easting, double start_depth, double start_tmd,
                    double end_depth);
  void GetInterSectionPointsWithFormationSkin(const geo::ILine &line, std::set<geo::CPoint> &PointSet) const;

public:
  bool IsVertical() const;

  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

  CGroupNode *GetGroup() const;
  virtual bool CanDestroy() const;
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  void Init();
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;
  void Color(const TColor &color);
  virtual TColor Color() const;

private:
  void InitFromPointArray(QVector<geo::CPoint> &vcPoint);
  void InitFromPointSet(const IPointSet &PointSet);

  void Sample(QVector<geo::CPoint> &vcPoint);
  void Clear();
  void ClearAndDelete();
  void Recalculate(const double &offset);
  void Recalculate(const geo::CPoint &point);
  void Recalculate(const double &GlobalNorthing, const double &GlobalEasting, const double &GlobalTVD);

public:
  std::list<CNewGeoWellPoint> &FormIntersecPoints();

  std::list<CNewFormationSection> &FormationSections();
  const std::list<CNewFormationSection> &FormationSections() const;
  std::list<INewWellSection *> &FormationSectionPointers() { return m_FormationSectionPointers; }
  void UpdateFormationSections();
  const CNewFormationSection *FormationSection(const CNewWellPoint &pt) const;

  virtual double GlobalNorthing() const { return m_dGlobalNorthing; }
  virtual double GlobalEasting() const { return m_dGlobalEasting; }
  virtual double GlobalTVD() const { return m_dGlobalTVD; }
  virtual double DerElev() const { return GlobalTVD(); }

private:
  void GlobalNorthing(const double &d) { m_dGlobalNorthing = d; }
  void GlobalEasting(const double &d) { m_dGlobalEasting = d; }
  void GlobalTVD(const double &d) { m_dGlobalTVD = d; }
  void DerElev(const double &d) { GlobalTVD(d); }

public:
  virtual unsigned int WellPointSize() const;
  virtual CNewWellPoint WellPoint(unsigned int index) const;
  virtual double TMD(unsigned int index, CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT) const;
  virtual bool InitWellPoint(CNewWellPoint &point) const;

  // wjrx mantis 3322
  void ToggleDrawWellInfo();
  void DrawWellInfo(bool bDraw);
  bool DrawWellInfo() const;

  // wjrx mantis 3401
  bool CanZoomIn() const;
  const CWellZoomInModel &ZoomInModel() const;
  CWellZoomInModel &ZoomInModel();

private:
  void SetBirthDate(const QDate &date) { m_BirthDate = date; }

public:
  const QDate &GetBirthDate() const { return m_BirthDate; }

  const std::list<CNewWellPoint> &MeshIntersecPoints();

  // wjrx mantis 3440
  //
  void CalcGraphPoints();
  std::list<CNewWellPoint> &GraphPoints();
  const std::list<CNewWellPoint> &GraphPoints() const;

  typedef std::pair<CNewWellPoint, geo::CValue> TPointValue;
  typedef std::vector<TPointValue> TPointValueVector;

  int MapValuesSize() const;
  void MapValues(const IValueComponentBase &component, CUnitNode::TUnitType unit, TPointValueVector &vcPointValues);

  // wjrx mantis 3564
  //
  std::list<const QDate *> &FistDates();
  const std::list<const QDate *> &FistDates() const;
  std::list<CNewWellPoint> &FistExportPoints();
  const std::list<CNewWellPoint> &FistExportPoints() const;
  bool CanExportToFist() const;
  void AddDefaultFistPoints(std::list<CNewWellPoint> &FistPoints) const;

  // wjrx mantis 3647
  const std::list<CNewWellPoint> &DStaborPoints() const;
  std::list<CNewWellPoint> &DStaborPoints(bool bCheck = false);

  // wjrx mantis 3608
  void ImportLAS(const QString &fileName);
  bool CanImportLAS() const;

  // wjrx mantis 3647
  void AddDefaultDstaborPoints(std::list<CNewWellPoint> &DstaborPoints) const;
  void MudGradRefDepth(double newval);
  virtual double MudGradRefDepth() const;

  static double CalcInclination(const geo::IPoint &pt1, const geo::IPoint &pt2);
  static double CalcAzimuth(const geo::IPoint &pt1, const geo::IPoint &pt2);
  virtual CAngleQuantity Azimuth(size_t index) const;
  virtual CAngleQuantity Inclination(size_t index) const;
  virtual void AzimuthInclination(double azimuth, double inclination);
  virtual size_t AngleIndex(const CNewWellPoint &pt) const;
  void ClearAngles();
  void UpdateDsbAnalysisPointsInScene();
  void ShowDsbAnalysisPointsInScene(bool bShow);
  bool ShowDsbAnalysisPointsInScene() const;
  CNewDstaborResultMap &DsbResults();
  const CNewDstaborResultMap &DsbResults() const;
  bool DsbCanCalculate(const CFormationBase &formation) const;
  const geo::CPolyLine &PolyLine() const;

  void GetIntersectionPointsWithPlane(geo::CPtrArray<CNewGeoWellPoint> &points, const geo::CPlane &plane) const;

  ACCEPT_GEOMECMODELVISITORS(VisitNewWellPath);

private:
  void LoadStreamOldToNew(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void LoadMeshIntersectionNew_Skip(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void LoadMeshIntersection_Skip(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void LoadWellLogData_Skip(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  void LoadWellLog_Skip(/*well::CWellLog *log,*/ TSTREAM &stream, TPROGRESS &progress);
  void LoadDCasint_Skip(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

  CNewDstaborResultMap m_DsbResults;

  bool m_bShowDsbPointsInScene;
  CAngleInfoVector m_vcAzimuthInclination;

  QDate m_BirthDate;
  geo::CPoint m_VerticalPointDef;

  geo::CPolyLine m_WellPathPolyLine;
  std::vector<geo::CPolyLine> m_FormationLines;   // with m_FormationArrows+label
  std::vector<CLabelPoint> m_FormationLabels;     // 1 label per FormationLine
  std::vector<CVectorSymbol *> m_FormationArrows; // 2 arrows per FormationLine
  geo::CPolyLine m_DsbPoints;                     // DStabor

  std::vector<double> m_WellPathTMD;

  std::list<CNewWellPoint> m_MeshIntersectionPoints;
  std::list<CNewGeoWellPoint> m_FormationIntersectionPoints;

  // Both lists contain the same (actual) information.
  // The pointer list is secondary.
  std::list<CNewFormationSection> m_FormationSections;
  std::list<INewWellSection *> m_FormationSectionPointers;
  std::list<CNewWellPoint> m_GraphPoints;

  std::list<CNewWellPoint> m_DStaborPoints;
  std::list<CNewWellPoint> m_FistExportPoints;
  std::list<const QDate *> m_FistDates;

  double m_dGlobalNorthing;
  double m_dGlobalEasting;
  double m_dGlobalTVD;

  void CalculateMeshIntersecPoints();
  void CalculateFormationIntersecPoints();
  void CalculateIntersections(const geo::IMesh &mesh);
  static geo::CPoint QuadLineIntersect(const geo::IQuadrilateral &quad, const geo::ILine &line);
  bool CalculateInterSectionWithFace(const geo::IFace &face, const CNewWellPoint &pt1, const CNewWellPoint &pt2,
                                     CNewWellPoint &intersection // output, invalid if return false
  ) const;

  bool m_bDrawWellInfo;

  CWellZoomInModel *m_pZoomInModel; // wjrx mantis 3401

  double m_dMudGradRefDepth;

public:
  bool hasZoomInModel() const;
  void CreateZoomInModel();

private:
  void DestroyZoomInModel();
};

class CNewWellPathEntry : public CStorageNodeEntry<CNewWellPath> {
  typedef CStorageNodeEntry<CNewWellPath> BASE_CLASS;

public:
  CNewWellPathEntry(C3DModel &model);
  virtual ~CNewWellPathEntry();

  static void addPointSet2DataStorage(const CGraphNode::TNodeVec &nodeVec, CModelBase &model);

  static bool ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model,
                         const CQuantity::UNIT &unit);
  static bool ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model);
  static bool ImportPetrelFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model,
                               const CQuantity::UNIT &lateralUnit, const CQuantity::UNIT &depthUnit);
  bool CanInsertNew() const;
  bool CanExport() const;

  // Drag and drop properties; waij TFS 92430
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  void ToggleDrawWellInfo();
  void DrawWellInfo(bool bDraw);
  bool DrawWellInfo() const;
  bool CanExportToFist() const;

  static const QString DEFAULT_NEWWELL_NAME;
  static const size_t DEFAULT_INITIAL_NUMBER;

  ACCEPT_GEOMECMODELVISITORS(VisitNewWellPathEntry);

private:
  bool m_bDrawWellInfo;
};

template <class T> CNewResultMap<T>::CNewResultMap(CNewWellPath *pWellPath) : m_pWellPath(pWellPath) {}

template <class T> void CNewResultMap<T>::RemoveResult(const CNewWellPoint &wellpoint) {
  // point found
  CDepletionStage *pStage = &(((CModelBase *)&m_pWellPath->Model())->InitialDepletionStage());
  while (pStage) {

    typename TResultMap::iterator result;
    result = m_Result.find(CNewWellPointResultKey(wellpoint, *pStage, CAnalysisType::AT_LINEAR));
    if (!(result == m_Result.end()))
      m_Result.erase(result);

    result = m_Result.find(CNewWellPointResultKey(wellpoint, *pStage, CAnalysisType::AT_NONLIN));
    if (!(result == m_Result.end()))
      m_Result.erase(result);

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

template <class T>
bool CNewResultMap<T>::ResultExist(const CNewWellPoint &Wellpoint, const CDepletionStage &stage,
                                   const CAnalysisType &antype) const {
  // point found
  typename TResultMap::iterator it = m_Result.find(CNewWellPointResultKey(Wellpoint, stage, antype));
  return !(it == m_Result.end());
}

template <class T>
T &CNewResultMap<T>::GetResult(const CNewWellPoint &Wellpoint, const CDepletionStage &stage,
                               const CAnalysisType &antype) const {

  if (!ResultExist(Wellpoint, stage, antype)) {
    // if it not exist add it to the map
    TResultPair pair(CNewWellPointResultKey(Wellpoint, stage, antype), T());
    m_Result.insert(pair);
  }
  // TRACE("\nm_Result.size() = %i\n",m_Result.size());
  return m_Result[CNewWellPointResultKey(Wellpoint, stage, antype)];
}

template <class T> void CNewResultMap<T>::ClearAllResults() { m_Result.clear(); }

bool less(const QDate *first, const QDate *second);
bool equal(const QDate *first, const QDate *second);

#endif // NEWWELLPATH_H
