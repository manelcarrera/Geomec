// WellPath.h: interface for the CWellPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPATH_H__1E0C7EC5_F5D3_4890_8609_6C8415C5CEA0__INCLUDED_)
#define AFX_WELLPATH_H__1E0C7EC5_F5D3_4890_8609_6C8415C5CEA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DCasingPointResult.h"
#include "FormationBase.h"
#include "StatusValue.h"
#include "wellpathbase.h"
#include "AnalysisType.h"

#define DEFAULT_WELL_NAME "New Wellpath %1"

// Define for the popup menu
#define WELLPATHDIALOG 101

class CWellPath;
class C3DModel;
class CWellSceneInterMed;
class CWellZoomInModel;
class CCachingIntermediate;

#include "NewModel.h"

//section list id
#define LST_SC_FORM 1  //formation section list
#define LST_SC_WELL 2  //well total section list

class COpenGLView;
class CProgressDlg_MFC;

namespace well {
  class CWellLog;
  class CWellPointValueMap;
}

namespace gm {
  class C3DWellPath;
}

#include "IWellSection.h"
#include "DCasintSlotHandler.h"
#include "WellPoint.h"

//*******************************************************************
//              
//	 Geomec well point
//
//*******************************************************************
class CGeoWellPoint : public well::CWellPoint
{
  friend class CWellPath;

private:
  mutable CFormationBase* m_pFormation;
  mutable bool m_bInit;
  void InitPoint() const;
  const CModelBase* m_model;
public:
  CGeoWellPoint(const CWellPath& wellpath , const double& tmd);
  CGeoWellPoint(CModelBase& model , const well::CWellPoint& wellpoint);
  CGeoWellPoint&		operator=(const CGeoWellPoint& rhs);
  CGeoWellPoint(const CGeoWellPoint& rhs);

  virtual ~CGeoWellPoint();

  // well::CWellPoint virtual overides
  virtual bool Defined() const;
  virtual void Invalidate(); 

  //return the formation
  const CFormationBase* Formation() const; 
};




//*******************************************************************
//              
//  Dstabor well point              
//
//*******************************************************************
class CDstaborWellPoint : public CGeoWellPoint
{
public:
  CDstaborWellPoint(const CWellPath& wellpath , const double& tmd);
  ~CDstaborWellPoint();
};


//*******************************************************************
//              
//  A less clsass for storing results per point and per depletion stage and per analysistype
//
//*******************************************************************
class CWellPointResultKey
{
friend class CWellPath;
  const well::CWellPoint& m_point;
  const CDepletionStage& m_stage;
  CAnalysisType m_antype;
public:
  CWellPointResultKey(const well::CWellPoint& point ,const CDepletionStage& stage, const CAnalysisType& antype);
  bool operator<( const CWellPointResultKey& rhs) const;
};


//*************************************************************************************

template<class T>
class CResultMap;

typedef CResultMap<CDsbStatusValue> CDstaborResultMap;
typedef CResultMap<CDCasingPointResult> CCasingResultMap;

//*******************************************************************
//
//	CFormationSection
//	is created after the intersection points with the mesh and the formations are calculated
//
//*******************************************************************
class CFormationSection : public well::IWellSection
{
public:
  //ISection interface
  CFormationSection(	well::CWellPathBase& WellPath,
            const CFormationBase* Formation,
            well::CWellPoint* Top,
            well::CWellPoint* Bottom)
  :well::IWellSection(WellPath)
  {
    m_Formation=Formation;
    m_Top=Top;
    m_Bottom=Bottom;
  }
  virtual const well::CWellPoint& Top() const {return *m_Top;}
  virtual const well::CWellPoint& Bottom() const {return *m_Bottom;}
  virtual QString GetDescription() const{return QString(m_Formation->Name());}

  const CFormationBase* Formation() const {return m_Formation;} 

private:

  const CFormationBase* m_Formation; 
  well::CWellPoint* m_Top;
  well::CWellPoint* m_Bottom;
};




//*******************************************************************
//
//		The Geomec WellPath object derived from well::CWellPath
//
//*******************************************************************
class CWellPath : public CColorNode , public well::CWellPathBase, public CDCasintSlotHandler
{
public:
  class IIntermediate
  {
  public:
    enum SECTION_TYPE
    {
      LINE,
      ARROW
    };

  public:
  IIntermediate() {}
  virtual ~IIntermediate() {}

  virtual void Detach() = 0;
  virtual void AddSection(well::IWellSection& section,
              SECTION_TYPE type,
              QRgb color,
              int iLineWidth,
              bool bVisible,
              const QString& strDescription) = 0;
  virtual void AddPoint(well::CWellPoint& point,
                          QRgb color,
                          int iPointSize,
                          bool bVisible) = 0;
  virtual void Color(QRgb color) = 0;
  virtual void RemoveAllPoints() = 0;
  virtual void SetSectionDescription(well::IWellSection& section, const QString& strDescription) = 0;
  virtual bool IsVisible() const = 0;
  virtual void OnNeighbourDeleted(const CGraphNode& node) = 0;
  virtual bool OnNewNeighbour(const CGraphNode& node) = 0; // returns whether update must be triggered
  virtual void DrawInfo(bool bDraw) = 0;
  virtual bool DrawInfo() const = 0;
  };

private:
  typedef std::pair<CDepletionStage*, well::CWellLog*> TStageLogPair;
  IIntermediate* m_pIntermediate;
  CCachingIntermediate* m_pCachingIntermediate;
  well::CWellDateList m_FistDates;
  bool m_graphPointsCalculated;

public:
  typedef std::map<CFormationSection*, TStageLogPair> TSectDeplLogMap;

  void InitVerticalWell(bool bResetGlobalTVD);
  
  TSectDeplLogMap m_mpSectDplLog;
  std::map<CFormationSection*, bool> m_mpCorrectAllStages;

  void Init();
  void CalculateMeshIntersecPoints();
  void CalculateFormationIntersecPoints();

  geo::CPtrArray<geo::IObject> m_display_list;

  double m_DsbLiquidFluid;
  double m_DsbFluidLossCoefficient;

  double m_dMudGradRefDepth;
  
  // called by CalcDefDSBPoints()
  bool m_bShowDsbPointsInScene;

  geo::CPoint m_VerticalPointDef;
  CDstaborResultMap* m_pDsbResults;

  bool m_bSectionsDirty;

  void CalcGraphPoints();
  void GetInterSectionPointsWithFormationSkin(const geo::ILine& line,std::set<geo::CPoint>& PointSet) const;
  void UpdateFormationSections();
  void SetSectionColor(TColor nColor,long SectionListID);

  void slotOnFinished();

  // Functions for settings used for a DCasint analysis //////////////////////////////////////////
  well::CWellLog *WellLog(CFormationSection *pFormation) const;
  void InsertLog(CFormationSection *pSection, CDepletionStage *pStage, well::CWellLog *pWellLog);
  void InsertCorrection(CFormationSection *pSection, bool bCorrect);
  void ClearLogdataMaps();
  bool UseCorrection(CFormationSection *pSection) const;
  
  const std::map<CFormationSection*, bool> &StagesCorrectionMap() const { return m_mpCorrectAllStages; }
  
  void SaveWellLog(well::CWellLog *log, TSTREAM& stream, TPROGRESS& progress);
  void LoadWellLog(well::CWellLog *log, TSTREAM& stream, TPROGRESS& progress);
  void LoadMeshIntersection(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  void LoadMeshIntersectionNew(TSTREAM& stream,CStreamVersion& version,TPROGRESS& progress);
  void LoadWellLogData(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  void SaveWellLogData(TSTREAM& stream, TPROGRESS& progress);
  void SaveMeshIntersectionNew(TSTREAM& stream,TPROGRESS& progress);

private:
  void LoadDCasint(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  well::CWellSectionList& FormationSectionsForLoadingOldFiles();
  void UpdateFormationSectionsForLoadingOldFiles();
  well::CWellPointList& FormIntersecPointsForLoadingOldFiles();
  void CalculateFormationIntersecPointsForLoadingOldFiles();
  well::CWellPointList& MeshIntersecPointsForLoadingOldFiles();
  void CalculateMeshIntersecPointsForLoadingOldFiles();

  void SetSectionsDirty();
  bool assignContainingElement(CGeoWellPoint& pt, const std::set<const geo::IElement*>& stElements) const;

public:
  bool hasZoomInModel() const;
  void CreateZoomInModel();
  void constructZoomInModel();
  void setNewModelFunction(bool (CNewModel::*newModel)(bool));
  CNewModel& getNewModel();

private:
  void DestroyZoomInModel();
  CWellZoomInModel* m_pZoomInModel;
  bool (CNewModel::*m_newModelFunction)(bool);
  CNewModel m_newModel;

public:
  void GetGeomecStrainValues(well::CWellPointList &list, well::CWellPointValueMap &map, const CDepletionStage &stage, const CAnalysisType& antype) const;
  const TSectDeplLogMap &SectionDeplLogMap() const { return m_mpSectDplLog; }
  ////////////////////////////////////////////////////////////////////////////////////////////////

  virtual ~CWellPath();

  virtual bool Destroy()
  {
    UnLinkAll();
    return CColorNode::Destroy();
  }

  //well::IWellSection &Section(const CFormationBase *pFormation) const;

  const well::CWellPointList &FistExportPoints() const;
  well::CWellPointList &FistExportPoints();
  void  AddDefaultFistPoints(well::CWellPointList& list) const;
  bool CanExportToFist() const;
  void InvalidatePointLists();	

  const well::CWellPointList& DStaborPoints() const;
  well::CWellPointList& DStaborPoints(bool bCheck= false);
  
  const well::CWellPointList& DCasintPoints() const;
  well::CWellPointList& DCasintPoints();
  
  const well::CWellPointList& GraphPoints() const;
  well::CWellPointList& GraphPoints();

  const well::CWellPointList& FormIntersecPoints() const;
  well::CWellPointList& FormIntersecPoints();

  const well::CWellPointList& MeshIntersecPoints() const;
  well::CWellPointList& MeshIntersecPoints();

  virtual const well::CWellDefinitionPointList& DefPointList() const;
  virtual well::CWellDefinitionPointList& DefPointList();

  const well::CWellSectionList& FormationSections() const;
  well::CWellSectionList& FormationSections();

  //snake well if well has extremes in z direction(snaking up and down)
  geo::CPoint MinPoint() const;
  geo::CPoint MaxPoint() const;

  void ShowDsbAnalysisPointsInScene(bool bShow);
  bool ShowDsbAnalysisPointsInScene();
  void UpdateDsbAnalysisPointsInScene(); //wjrx mantis 2542

  void AddDefaultDstaborPoints(well::CWellPointList& list) const; 

  /* NEVER USED void CalculateDsbResult(const CDepletionStage& stage, const CAnalysisType& antype); */

  //the stored results for DStabor
  CDstaborResultMap& DsbResults();	
  const CDstaborResultMap& DsbResults() const;	

  well::CWellDateList &FistDates();
  const well::CWellDateList &FistDates() const;

  const CFormationSection *FormationSection(const CGeoWellPoint &pt) const;

  //returns the vertical line (in case wellpath is vertical obviously)
  geo::CLine GetVerticalLine() const;



  //see if this formation type can be calculated 
  bool DsbCanCalculate(const CFormationBase& formation) const;
  bool DsbCanCalculateEFG(const CFormationBase& formation) const;
  
  void GetIntersectionPointsWithPlane(geo::CPtrArray<CGeoWellPoint>& points,const geo::CPlane& plane) const;


  bool operator==(const CWellPath &rhs) const;


  virtual const geo::IObject& DisplayList(int nIndex) const;
  geo::CPtrArray <geo::IObject>& DisplayList();
  virtual int DisplayListSize() const;

  virtual void Color(TColor nColor);
  virtual TColor Color() const;

  //virtual void		 OnSelect(CGraphNode* PreviousSeleceted);
  //virtual void		 OnDeselect(CGraphNode* NewSeleceted);

  double MudGradRefDepth() const;
  void MudGradRefDepth(double newval);
  
  double DerElev() const;
  void DerElev(double newval);
  
  //used for drawing improved upperlimit in wellpath graph
    double DsbLiquidFluid() const;
    void DsbLiquidFluid(double newval);

    double DsbFluidLossCoefficient() const;
    void DsbFluidLossCoefficient(double newval);
  //////////////


  // only for vertical wellpaths.
  void Recalculate(const geo::CPoint &NewPoint);

  virtual bool IsVertical() const;

  void Recalculate();

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual long SavedItems() const;
  virtual bool Empty() const;
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNewNeighbour(const CGraphNode& node);

  void ToggleDrawWellInfo();
  void DrawWellInfo(bool bDraw);
  bool DrawWellInfo() const;

  bool CanZoomIn() const;
  const CWellZoomInModel& ZoomInModel() const;
  CWellZoomInModel& ZoomInModel();

  void SetIntermediate(IIntermediate* pIntermediate);
  IIntermediate* Intermediate() const;

private:
  friend class CStorageNodeEntry <CWellPath>;
  friend class CWellPathEntry;

  CWellPath(CFemAppModel &model);
  CWellPath(const QString &sName, CFemAppModel &model);
  CWellPath(const QString &sName, CFemAppModel &model, const geo::CPoint &BasePoint);
  CWellPath(const CWellPath &well_path, CFemAppModel &model);

  CWellPath(const CWellPath &WellPath);
  CWellPath& operator =(const CWellPath &rhs);
};

typedef CSingleCommandTemplate<CWellPath> TWellPathCommand;



//*******************************************************************
//
//	CWellPathEntry
//
//*******************************************************************
class CWellPathEntry : public CStorageNodeEntry<CWellPath>
{
  typedef CStorageNodeEntry<CWellPath> BASE_CLASS;
  void AllWellPathInfo(bool bShow);

public:
  virtual ~CWellPathEntry();
  static bool ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model, const CQuantity::UNIT& unit);

  void ShowAllWellPathInfo();
  void HideAllWellPathInfo();

  bool CanExport() const;
  bool CanExportToFist() const;

private:
  friend class C3DModel;

  CWellPathEntry(C3DModel &model);
};

//*******************************************************************
//
//		a map for storing results(Dstabor and Casing)
//		the results are stored per point and per depletion stage and per analysis type
//
//*******************************************************************
template<class T>
class CResultMap
{
public:
  typedef std::pair<CWellPointResultKey,T> TResultPair;
  typedef std::map<CWellPointResultKey,T> TResultMap;

  CResultMap(CWellPath& wellpath);

  //remove all results for this point
  void RemoveResult(const well::CWellPoint& wellpoint);

  //creates an default result if not existst
  T& GetResult(const well::CWellPoint& Wellpoint,const CDepletionStage& stage, const CAnalysisType& antype) const;
  bool ResultExist(const well::CWellPoint& Wellpoint,const CDepletionStage& stage, const CAnalysisType& antype) const;

  TResultMap& GetResultMap() {return m_Result;}

  void ClearAllResults();	
private:

  mutable TResultMap m_Result;
  CWellPath* m_pWellPath;
};

template<class T>
CResultMap<T>::CResultMap(CWellPath& wellpath)
{
  m_pWellPath = &wellpath;
}

template<class T>
void CResultMap<T>::RemoveResult(const well::CWellPoint& wellpoint)
{
  //point found
  CDepletionStage *pStage = &(((CModelBase*)&m_pWellPath->Model())->InitialDepletionStage());
  while(pStage)
  {

    typename TResultMap::iterator result;
  result = m_Result.find(CWellPointResultKey(wellpoint, *pStage, CAnalysisType::AT_LINEAR));
    if(!(result == m_Result.end()))
      m_Result.erase(result);

  result = m_Result.find(CWellPointResultKey(wellpoint, *pStage, CAnalysisType::AT_NONLIN));
    if(!(result == m_Result.end()))
      m_Result.erase(result);
    
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

template<class T>
bool CResultMap<T>::ResultExist(const well::CWellPoint& Wellpoint,const CDepletionStage& stage,const CAnalysisType& antype) const
{
  //point found
  typename TResultMap::iterator it =
    m_Result.find(CWellPointResultKey(Wellpoint, stage,antype));
  return !(it == m_Result.end());
}


template<class T>
T& CResultMap<T>::GetResult(const well::CWellPoint& Wellpoint,const CDepletionStage& stage,const CAnalysisType& antype) const
{

  if(!ResultExist(Wellpoint,stage,antype))
  {
    //if it not exist add it to the map
    TResultPair pair(CWellPointResultKey(Wellpoint,stage,antype),T());
    m_Result.insert(pair);
  }
  //TRACE("\nm_Result.size() = %i\n",m_Result.size());
  return m_Result[CWellPointResultKey(Wellpoint,stage,antype)];

}

template<class T>
void CResultMap<T>::ClearAllResults()
{
  m_Result.clear();
}

#endif // !defined(AFX_WELLPATH_H__1E0C7EC5_F5D3_4890_8609_6C8415C5CEA0__INCLUDED_)
