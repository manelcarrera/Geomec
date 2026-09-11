
#include <memory>

#include "GeomecUtils.h"
#include "3dmodel.h"
#include "ImportFormat.h"
#include "PropertyFile.h"
#include "PropertyExcel.h"
#include "NewWellPath.h"
#include "MeshBase.h"
#include "LabelPoint.h"
#include "VectorSymbol.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "GlobalMessage.h"
#include "WellZoomInModel.h"
#include "ModifiedHint.h"
#include "BodyGroup.h"
#include "LogASCIIStandardFile.h"
#include "StatusValue.h"
#include <QDate>
#include "ValidateModel.h"
#include "LasPointSet.h"
#include "NewArchiveStdStringStream.h"
#include "ImportPetrel.h"
#include "IProgressFactory.h"
#include "NewFormationSectionList.h"
#include "WellPathFile.h"
#include "FvGocadWellFile.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "Temperature.h"
#include "NewWellPathInput.h"
#include "ModelLifetimeFacade.h"
#include "Result.h"
#include "MeshResultTree.h"
#include "IStressTensorGroup.h"
#include "IStrainTensorGroup.h"
#include "GroupNode.h"
#include "BoundingBox.h"

#include <cmath>


//
//============================================================================
//
CNewWellPath::CNewWellPath( CFemAppModel &model)
: CColorNode(model)
, m_input(0)
, m_DsbResults(this)
, m_bShowDsbPointsInScene(false)
, m_dGlobalNorthing(0)
, m_dGlobalEasting(0)
, m_dGlobalTVD(0)
, m_bDrawWellInfo(true)
, m_pZoomInModel(0)
, m_dMudGradRefDepth(0)
{
  Init();
}

CNewWellPath::CNewWellPath(const CNewWellPathInput& input, CFemAppModel &model)
: CColorNode(input.Name(), model)
, m_input(0)
, m_DsbResults(this)
, m_bShowDsbPointsInScene(false)
, m_dGlobalNorthing(0)
, m_dGlobalEasting(0)
, m_dGlobalTVD(0)
, m_bDrawWellInfo(true)
, m_pZoomInModel(0)
, m_dMudGradRefDepth(0)
{
  CGraphNode::create(Model().GraphEntry(MD_NEW_WELLPATH));
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  InitFromWellPathInput(input);
}

#if 0
CNewWellPath::CNewWellPath(const QString& sName, CFemAppModel& model)
: CColorNode(sName,model)
, m_DsbResults(this)
, m_bShowDsbPointsInScene(false)
, m_dGlobalNorthing(0)
, m_dGlobalEasting(0)
, m_dGlobalTVD(0)
, m_bDrawWellInfo(true)
, m_pZoomInModel(0)
, m_dMudGradRefDepth(0)
{
  CGraphNode::create( Model().GraphEntry(MD_NEW_WELLPATH) );
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  Init();

  CNewWellPathInput::createNewWellPathInput(Name(), Model(), this);
}

// wjrx mantis 3549
//
CNewWellPath::CNewWellPath
( const QString &sName
, CFemAppModel &model
, const geo::CPoint &BasePoint
)
: CColorNode(sName, model)
, m_DsbResults(this)
, m_bShowDsbPointsInScene(false)
, m_VerticalPointDef(BasePoint)
, m_dGlobalNorthing(0)
, m_dGlobalEasting(0)
, m_dGlobalTVD(0)
, m_bDrawWellInfo(true)
, m_pZoomInModel(0)
, m_dMudGradRefDepth(0)
{
  InitVerticalWell(true);
  CGraphNode::create(Model().GraphEntry(MD_NEW_WELLPATH));
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  Init();

  CNewWellPathInput::createNewWellPathInput(Name(), Model(), this);
}

CNewWellPath::CNewWellPath(const CNewWellPath& newWellPath, CFemAppModel &model)
: CColorNode(newWellPath.Name().toStdString().c_str(), model)
, m_DsbResults(newWellPath.m_DsbResults)
, m_bShowDsbPointsInScene(newWellPath.m_bShowDsbPointsInScene)
, m_vcAzimuthInclination(newWellPath.m_vcAzimuthInclination)
, m_BirthDate(newWellPath.m_BirthDate)
, m_VerticalPointDef(newWellPath.m_VerticalPointDef)
, m_WellPathPolyLine(newWellPath.m_WellPathPolyLine)
, m_DsbPoints(newWellPath.m_DsbPoints)
, m_WellPathTMD(newWellPath.m_WellPathTMD)
, m_GraphPoints(newWellPath.m_GraphPoints)
, m_DStaborPoints(newWellPath.m_DStaborPoints)
, m_FistExportPoints(newWellPath.m_FistExportPoints)
, m_FistDates(newWellPath.m_FistDates)
, m_dGlobalNorthing(newWellPath.m_dGlobalNorthing)
, m_dGlobalEasting(newWellPath.m_dGlobalEasting)
, m_dGlobalTVD(newWellPath.m_dGlobalTVD)
, m_bDrawWellInfo(newWellPath.m_bDrawWellInfo)
, m_pZoomInModel(newWellPath.m_pZoomInModel)
, m_dMudGradRefDepth(newWellPath.m_dMudGradRefDepth)
{
  Init();

  CGraphNode::create(Model().GraphEntry(MD_NEW_WELLPATH));
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  // Copy color, derrickelevation, easting, northing, vertical and points
  Color(newWellPath.Color());

  CNewWellPathInput::createNewWellPathInput(Name(), Model(), this);
}

CNewWellPath::CNewWellPath(const QString &file, const QString &name, CFemAppModel &model, CQuantity::UNIT unit, IProgressBase &dlg)
: CColorNode(name, model)
, m_DsbResults(this)
, m_bShowDsbPointsInScene(false)
, m_dGlobalNorthing(0)
, m_dGlobalEasting(0)
, m_dGlobalTVD(0)
, m_bDrawWellInfo(true)
, m_pZoomInModel(0)
, m_dMudGradRefDepth(0)
{
  IModelObject::create();
  Init();
  //QString sPath,sFile;

  //SplitPathAndFileName(strFileName,sPath,sFile);
  //CWellPath *pWellPath = new CWellPath(sFile, (CFemAppModel&)model);
  //CWellPath *pWellPath = new CWellPath((CFemAppModel&)model);


  CWellPathFile wp_file(this, unit);
//	WellPath->Name(sFile.Left(sFile.GetLength() - 4));
  if(wp_file.Open(file, dlg))
  {
    CGraphNode::create((Model()).GraphEntry(MD_NEW_WELLPATH));
    LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
    LinkTo(*(Model()).GraphEntry(MD_BASE_DEPLETION_STAGE));
  } else {
    _m()->msg(wp_file.ErrorMessage().toAscii().constData());
  }

  CNewWellPathInput::createNewWellPathInput(Name(), Model(), this);
}
#endif

const CNewWellPathInput *CNewWellPath::WellPathInput() const
{
  return m_input;
}

void CNewWellPath::Init()
{
  CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());

  CDepletionStage &initial_stage = pModel->InitialDepletionStage();
  SetBirthDate
  ( QDate(initial_stage.Time().GetYear()
  , initial_stage.Time().GetMonth()
  , 1));
  m_dMudGradRefDepth=0;
}

void CNewWellPath::InitFromWellPathInput(const CNewWellPathInput& input)
{
  if (input.IsVertical())
  {
  SetBirthDate(input.BirthDate());

  GlobalNorthing(input.GlobalNorthing());
  GlobalEasting(input.GlobalEasting());
  GlobalTVD(input.GlobalTVD());

  m_VerticalPointDef.Set(GlobalNorthing(), GlobalEasting(), GlobalTVD());

  if (input.PointSize() >= 2)
      InitVertical(GlobalNorthing(), GlobalEasting(), input.PointAt(0).Z(), 0, input.PointAt(input.PointSize() - 1).Z());
  }
  else
  {
  InitFromPointSet(input);
  }

  m_input = &input;

  const_cast<CNewWellPathInput&>(input).LinkTo(*this);
}


double CNewWellPath::CalcInclination
( const geo::IPoint &pt1
, const geo::IPoint &pt2
)
{
  geo::CVector v(pt1,pt2);
  return RAD2DEG( acos(v.Z()/v.Length()) );
}

double CNewWellPath::CalcAzimuth
( const geo::IPoint &pt1
, const geo::IPoint &pt2
)
{
  geo::CVector v(pt1,pt2);

  // Defined in mantis 3564:
  // Azimuth = 180/PI()*ATAN(E/N)-MIN(SIGN(N),0)*180
  //
  if ( v.X() == 0 )
  return v.Y() > 0 ? 90 : v.Y() == 0 ? 0 : -90;
  else
  return RAD2DEG( atan(v.Y()/v.X()) ) - (v.X()<0 ? -1 : 0)*180;
}

CAngleQuantity CNewWellPath::Azimuth(size_t index) const
{
  return  m_vcAzimuthInclination.Azimuth(index);
}

CAngleQuantity CNewWellPath::Inclination(size_t index) const
{
  return  m_vcAzimuthInclination.Inclination(index);
}

void CNewWellPath::AzimuthInclination(double azimuth, double inclination)
{
  m_vcAzimuthInclination.PushBack(azimuth, inclination);
}

// Determine index for looking up Inclination and Azimuth
// as defined in mantis 3564
//
size_t CNewWellPath::AngleIndex(const CNewWellPoint &pt) const
{
  for (size_t ii= 0; ii< WellPointSize(); ++ii)
  {
  if (  pt.TMD() < TMD(ii) ) 
  {
      return ii;
  }
  }

  return WellPointSize()-1;
}

void CNewWellPath::ClearAngles()
{
  m_vcAzimuthInclination.Clear();
}

// wjrx mantis 3549
//
void CNewWellPath::InitVerticalWell(bool bResetGlobalTVD)
{
  assert(!m_VerticalPointDef.Empty());

  CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());

  if(!pModel->IsMesh())
          return;

  double Zmax = pModel->Mesh().Mesh().Max().Z();
  double Zmin = pModel->Mesh().Mesh().Min().Z();
  
  double maxZ = Zmax +(Zmax-Zmin)*0.5;
  double minZ = Zmin -(Zmax-Zmin)*0.5;

  geo::CPoint p1(m_VerticalPointDef.X(),m_VerticalPointDef.Y(),minZ); //
  geo::CPoint p2(m_VerticalPointDef.X(),m_VerticalPointDef.Y(),maxZ); //
  geo::CLine line(p1,p2);
  std::set<geo::CPoint> PointSet;
  GetInterSectionPointsWithFormationSkin(line,PointSet);
  
  if(PointSet.size() == 0)
          return;

  const geo::CPoint* first = &*PointSet.begin();
  const geo::CPoint* last  = &*PointSet.rbegin();

  //initialization of a vertical wellpath (SI_UNIT)
  InitVertical( first->X()
              , first->Y()
              , bResetGlobalTVD ? first->Z() : GlobalTVD()
              , 0
              , last->Z()
              );

  UpdateFormationSections();
}

// wjrx mantis 3549
//
void CNewWellPath::GetInterSectionPointsWithFormationSkin(const geo::ILine& line , std::set<geo::CPoint>& PointSet) const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());

  if(!pModel->IsMesh())
  return;

  //loop over the volumes in a formation
  CNewWellPath* pThis=const_cast<CNewWellPath*>(this);

  TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) pThis->Model().GraphEntry(MD_BASE_FORMATION))->EntryNodes();
  //loop over the formations
  for(TFormationBaseEntry::TNodeSet::iterator iter = formations.begin(); iter != formations.end(); iter++)
  {
  //loop over the volumes in a formation
  for(int i = 0; i < (*iter)->ElementSetSize(); i++)
  {
      geo::CBodyGroup *pGeoVolume = dynamic_cast<geo::CBodyGroup*>(&(*iter)->ElementSet(i).ElementSet());
      assert(pGeoVolume);

      //get the intersection points with the volume and the vertical line
      pGeoVolume->IntersectionWithEdgeFaces(line,PointSet);
  }
  }
}

// wjrx mantis 3549
//
void CNewWellPath::InitVertical
( double start_northing
, double start_easting
, double start_depth
, double start_tmd
, double end_depth
)
{
  m_WellPathPolyLine.RemoveAll();
  m_WellPathTMD.clear();
  ClearAngles();
  // Clear();

  m_dGlobalNorthing= start_northing;
  m_dGlobalEasting= start_easting;
  m_dGlobalTVD= start_depth;

  geo::CPoint point1( start_northing, start_easting, start_tmd + start_depth);
  geo::CPoint point2( start_northing, start_easting, end_depth);

  // as defined in mantis 3564
  double azi= CalcAzimuth(point1, point2);
  double inc= CalcInclination(point1,point2);
  m_vcAzimuthInclination.PushBack(azi,inc); // point 1
  m_vcAzimuthInclination.PushBack(azi,inc); // identical for point 2

  m_WellPathPolyLine.PushBack( point1);
  m_WellPathTMD.push_back(start_tmd);
  m_WellPathPolyLine.PushBack( point2);
  m_WellPathTMD.push_back(start_tmd + end_depth - start_depth);

  assert(IsVertical());

}

bool CNewWellPath::IsVertical() const
{
  return ! m_VerticalPointDef.Empty();

  /*
   * We could define that deviated wells are vertical if they are
   * geometrically vertical.
   *
   * The code below performs this check.
   *
  if ( m_WellPathPolyLine.PointSize() < 2 ) 
  return true;

  for ( int ii= 1; ii< m_WellPathPolyLine.PointSize(); ++ii)
  {
  if ( fabs(m_WellPathPolyLine.Point(ii-1).X() - m_WellPathPolyLine.Point(ii).X()) > EPS) 
      return false;
  if ( fabs(m_WellPathPolyLine.Point(ii-1).Y() - m_WellPathPolyLine.Point(ii).Y()) > EPS) 
      return false;
  }
  return true;
  */

}

CNewWellPath::~CNewWellPath()
{
  m_WellPathPolyLine.RemoveAll();
  m_WellPathTMD.clear();
  ClearAngles();
  Clear();
}

unsigned int CNewWellPath::IconId() const
{
  return IDI_WELLPATH;
}

unsigned int CNewWellPath::TypeId() const
{
  return IDT_TREE_WELL_PATHS;
}

QString CNewWellPath::TypeName() const
{
  return getStringTableEntry(IDS_TREE_WELL_PATHS);
}

void CNewWellPath::ToggleDrawWellInfo()
{
  DrawWellInfo(!DrawWellInfo());
}

void CNewWellPath::DrawWellInfo(bool bDraw)
{
  if(bDraw != m_bDrawWellInfo)
  {
  m_bDrawWellInfo= bDraw;
  Modified();
  }
}

bool CNewWellPath::DrawWellInfo() const
{
  return m_bDrawWellInfo;
}


bool CNewWellPath::Empty() const
{
  return m_WellPathPolyLine.PointSize()==0?true:false; //DefPointList().NrOfPoints()==0;
}

long CNewWellPath::SavedItems() const
{
  long nRet = CColorNode::SavedItems();

  nRet += DStaborPoints().size();

/*
  nRet += m_mpSectDplLog.size();
  nRet += m_mpSectEpsType.size();
  nRet += m_mpCorrectAllStages.size();

*/
  if(m_pZoomInModel)
  nRet += m_pZoomInModel->SavedItems();

  return nRet;
}


int CNewWellPath::DisplayListSize() const
{
  int retval= 1; // m_WellPathPolyLine

  if ( m_bShowDsbPointsInScene ) ++retval; // m_DsbPoints

  if ( m_bDrawWellInfo) // wjrx mantis 3322
  retval+=
    m_FormationLines.size()
      + m_FormationLabels.size()
      + m_FormationArrows.size();

  return retval;
}

const geo::IObject& CNewWellPath::DisplayList(int nIndex) const 
{
  if ( nIndex==0 )
  return m_WellPathPolyLine;

  if ( m_bShowDsbPointsInScene)
  {
  if ( nIndex==1 )
  {
      return m_DsbPoints;
  }
  else
      --nIndex;
  }

  --nIndex;
  assert(nIndex>=0);

  if ( nIndex < m_FormationLines.size() ) 
  return m_FormationLines[nIndex];

  nIndex-= m_FormationLines.size();
  assert(nIndex>=0);

  if ( nIndex < m_FormationLabels.size() ) 
  return m_FormationLabels[nIndex];

  nIndex-= m_FormationLabels.size();
  assert(nIndex>=0);

  return *(m_FormationArrows[nIndex]);
}


#if 0
COpenGLNode::CDrawDef* CNewWellPath::OnCreateDrawDef
( const geo::IObject& object ) const
{
  if ( &object == &DisplayList(0) )
  {
  return CColorNode::OnCreateDrawDef(object);
  }

  if ( m_bShowDsbPointsInScene)
  if ( &object == &DisplayList(1) )
      return new CStaborPointsDrawDef(*this);

  for ( size_t ii= 0; ii< m_FormationLines.size(); ++ii)
  if ( &object == &m_FormationLines[ii] )
      return new CLineDrawDef(*this);

  for ( size_t ii= 0; ii< m_FormationLabels.size(); ++ii)
  if ( &object == &m_FormationLabels[ii] )
      return new CLabelDrawDef(*this);

  for ( size_t ii= 0; ii< m_FormationArrows.size(); ++ii)
  if ( &object == m_FormationArrows[ii] )
      return new CArrowDrawDef(*this);

  assert(false);
  return 0;
}
#endif

unsigned int CNewWellPath::WellPointSize() const
{
  return m_WellPathPolyLine.PointSize();
}

CNewWellPoint CNewWellPath::WellPoint(unsigned int index) const
{
  assert( index < WellPointSize() );

  if ( index >= WellPointSize() )
  return CNewWellPoint(*this,0,0,0,0);

  return CNewWellPoint
  ( *this, m_WellPathPolyLine.Point(index), m_WellPathTMD[index]);
}

double CNewWellPath::TMD
( unsigned int index
, CDoubleQuantity::UNIT unit
) const
{
  assert( index < WellPointSize() );

  if ( index >= WellPointSize() ) return 0;
  return CLengthQuantity( m_WellPathTMD[index], unit).Value();
}



CNewWellPath::TColor CNewWellPath::Color() const
{
  // during loading (conversion) we may get asked for a color while we have no m_input yet, in which case we return our own color
  return m_input ? m_input->Color() : CColorNode::Color();
}

void CNewWellPath::Color(const TColor &/*color*/)
{
  // shouldn't be called; call CNewWellPathInput::Color
  assert(false);
}


void CNewWellPath::InitFromPointSet( const IPointSet &PointSet)
{
  QVector<geo::CPoint> vcPoint;

  for (int ii= 0; ii< PointSet.PointSize(); ++ii)
  {
  const geo::IPoint &p=  PointSet.PointAt(ii);
  vcPoint.push_back( geo::CPoint(p) );
  }

  InitFromPointArray( vcPoint);
}

void CNewWellPath::InitFromPointArray( QVector<geo::CPoint>& vcPoint)
{
  m_WellPathPolyLine.RemoveAll();
  m_WellPathTMD.clear();
  ClearAngles();
  Clear();

  if (vcPoint.size() == 0 ) return;

  Sample(vcPoint);

  m_dGlobalNorthing= vcPoint[0].X();
  m_dGlobalEasting= vcPoint[0].Y();
  m_dGlobalTVD= vcPoint[0].Z();
  m_WellPathPolyLine.PushBack( vcPoint[0]);
  double distance1= 0.0;
  m_WellPathTMD.push_back( distance1);

  int previousPointIndex= 0;
  for (int ii= 1; ii< vcPoint.size(); ++ii)
  {
  double distance2= vcPoint[ii].Distance(vcPoint[previousPointIndex]);

  double azi= CalcAzimuth(vcPoint[previousPointIndex], vcPoint[ii]);
  double inc= CalcInclination(vcPoint[previousPointIndex], vcPoint[ii]);
  if ( previousPointIndex == 0 )
  {
      // as defined in mantis 3564
      m_vcAzimuthInclination.PushBack(azi,inc);
  }
  m_vcAzimuthInclination.PushBack(azi,inc);

  m_WellPathPolyLine.PushBack( vcPoint[ii]);
  previousPointIndex= ii;
  m_WellPathTMD.push_back( m_WellPathTMD[m_WellPathTMD.size()-1]+distance2);
  }

  FormationSections();
}

// wjrx mantis 3443
//
void CNewWellPath::Sample( QVector<geo::CPoint>& vcPoint)
{
  if ( vcPoint.size() <= 2 ) return; // no sampling

  QVector<geo::CPoint> vcSampled;  // sampled version of vcPoint
  vcSampled.push_back( vcPoint[0]);

  for (int ii= 1; ii< vcPoint.size()-1; ++ii)
  {
  // add point to sampled set if point distance >= 1 meter
  //
  if ( vcSampled[vcSampled.size()-1].SquareDistance(vcPoint[ii]) >= 1 )
      vcSampled.push_back( vcPoint[ii]);
  }

  // Add the last point if we currently have only one point in vcSampled OR
  // if the distance from the previous point >= 1 meter
  //
  size_t lastPt= vcPoint.size()-1;
  if ( vcSampled.size() < 2 || 
       vcSampled[vcSampled.size()-1].SquareDistance(vcPoint[lastPt]) >= 1
     )
  {
  vcSampled.push_back( vcPoint[lastPt]);
  }
  else
  {
  // Replace the last added point with this last point, we always want the
  // last point.
  //
  vcSampled.pop_back();
  vcSampled.push_back( vcPoint[lastPt]);
  }

  // replace wellpath with the sampled wellpath
  //
  vcPoint= vcSampled;
}

void CNewWellPath::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Let save base first
  CColorNode::SaveStream(stream, progress);

  // Update progress indicator
  std::string pr("Saving WellPath (ps)");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  stream << int(m_input ? m_input->Index() : -1);

  // Save birth date
  stream << GetBirthDate().year(); 
  stream << GetBirthDate().month();
  stream << GetBirthDate().day();

  //ref depth
  stream << MudGradRefDepth();

  // Save all wellpoints
  stream << WellPointSize();

  for (unsigned int ii= 0; ii<WellPointSize(); ++ii)
  {
  stream << WellPoint(ii).Northing() + GlobalNorthing();
  stream << WellPoint(ii).Easting() + GlobalEasting();
  stream << WellPoint(ii).TVD() + GlobalTVD();
  progress.Step();
  }

  if(m_pZoomInModel)
  {
  stream << 1;
  CModelBase& model = static_cast<CModelBase&>(Model());
  model.SaveChildModel(stream, progress, *m_pZoomInModel);
  }
  else
  {
  stream << 0;
  }

  // FIST export
  stream << (unsigned int )m_FistExportPoints.size();

  for (std::list<CNewWellPoint>::const_iterator  it= m_FistExportPoints.begin()
      ; it!=m_FistExportPoints.end()
      ; ++it
      )
  {
  stream << (*it).TMD();
  }

//---

  // Save d-stabor analysis
  int nSize = DStaborPoints().size();
  stream << nSize;

  for ( std::list<CNewWellPoint>::const_iterator it= DStaborPoints().begin()
      ; it != DStaborPoints().end()
      ; ++it
      )
  { 
  const CNewWellPoint* point = &*it;

  // Save location
  stream << point->X() << point->Y() << point->Z();

  // Save alonghole depth
  stream << point->TMD();

  // Save results
  CDepletionStage *pStage = &(((CModelBase*)&Model())->InitialDepletionStage());
  while(pStage)
  {
      if(DsbResults().ResultExist(*point,*pStage,CAnalysisType::AT_LINEAR))
      {
    CDsbStatusValue &linval = DsbResults().GetResult(*point,*pStage,CAnalysisType::AT_LINEAR);
    if (linval.IsCalculated())
    {
          // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
          stream << (int) TRUE; // Start reading tag
          stream << pStage->Index();
          // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
          stream << (int) TRUE;  // Linear
          stream << linval.Status();
          stream << linval.Value();
    }
      }

      if(DsbResults().ResultExist(*point,*pStage,CAnalysisType::AT_NONLIN))
      {
    CDsbStatusValue &nonlinval = DsbResults().GetResult(*point,*pStage,CAnalysisType::AT_NONLIN);
    if (nonlinval.IsCalculated())
    {
          // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
          stream << (int) TRUE;// Start reading tag
          stream << pStage->Index();
          // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.
          stream << (int) FALSE;// Non linear
          stream << (int)nonlinval.Status();
          stream << nonlinval.Value();
    }
      }

      if(pStage->Last())
    pStage = 0;
      else
    pStage = &pStage->Next();
  }

  // Delimit results of point  with end tag
  stream << (int) FALSE;  // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.

  progress.Step();
  }
}

// wjrx mantis 3687 read CWellPath and convert it to CNewWellPath
//
void CNewWellPath::LoadStreamOldToNew
( TSTREAM& stream
, CStreamVersion& version
, TPROGRESS& progress
)
{
  // An old wellpath (in any version) is loaded from file and
  // written to 'stream' (in the current version).
  // It is then loaded into LoadStreamOldToNew and we know
  // it is a current CStreamVersion.
  //
  assert( version >= CStreamVersion(4, 1, 11) );

  // Load the base first
  CColorNode::LoadStream(stream, version, progress);
  Name( Name() + " (conv.)");

  // Set progress indicator
  std::string pr("Converting WellPath ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  //Load Birth Date 
  {
  int year;
  int month;
  int day;
  stream >> year;
  stream >> month;
  stream >> day;
  SetBirthDate(QDate(year, month, day));
  }
  

  // Load position and elevation

  double global_northing,global_easting,global_depth;

  stream >> global_northing;
  stream >> global_easting;
  stream >> global_depth;

  stream >> m_dMudGradRefDepth;
          
  // Is the well vertical?
  int bVertical;
  stream >> bVertical;

  if ( bVertical ) m_dGlobalTVD= global_depth;// wjrx mantis 2555

  if ( bVertical )
  {
  double dX;
  double dY;
  stream >> dX;
  stream >> dY;
  m_VerticalPointDef = geo::CPoint(dX,dY);
  }

  ///QVector<double> tmd;
  ///QVector<double> azi;
  ///QVector<double> inc;

  // Load points
  int nSize;
  stream >> nSize;
  QVector<geo::CPoint> vcPoint;
  vcPoint.reserve(nSize);

  for(int i = 0; i < nSize; i++)
  {
  double x, y, z, dummy;

  stream >> x;
  stream >> y;
  stream >> z;
  vcPoint.push_back(geo::CPoint(x,y,z));

  stream >> dummy; // azimuth;
  stream >> dummy; // inclination;
  stream >> dummy; // alongholedepth;
  progress.Step();
  }
  InitFromPointArray( vcPoint);

  // load the d-stabor points
  int nDStaborSize;
  stream >> nDStaborSize;


  for(int i = 0; i < nDStaborSize; i++)
  {
  // Load the point
  double x, y, z, dummy, TMD;

  stream >> x;
  stream >> y;
  stream >> z;
  stream >> dummy; // azimuth
  stream >> dummy; // inclination
  stream >> TMD; // = alongholedepth;

  CNewWellPoint point(*this, TMD);
  DStaborPoints().push_back(point);

  // Get the results 
  int bLoadRes;
  stream >> bLoadRes;

  while(bLoadRes)
  {
      // Load depletion stage
      int nIndex;
      CAnalysisType antype;
      stream >> nIndex;

      if(nIndex < 0 )  //old databse
    nIndex=0;
      
      CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
      CDepletionStage *pDepl = dep_entry.FindIndex(nIndex);
  int bLinear;

      if(pDepl==0) //old database
      {
    //fake reading
    double dum;
    int idum;
    stream >> bLinear;
    stream >> idum;
    stream >> dum;
    stream >> bLoadRes;
    continue;
      }
      int status;
      double value;
      // Load linearity tag, status and value
      stream >> bLinear;
      CDsbStatusValue &rs = DsbResults().GetResult(point, *pDepl, (bLinear ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN));
      stream >> status;
      stream >> value ;
      rs.Value(value, (CDsbStatusValue::eDsbStatus)status);
      
      // Continue?
      stream >> bLoadRes;
  }

  progress.Step();
  }

  progress.Step();

  double dummy;
  stream >> dummy; // m_DsbLiquidFluid;
  stream >> dummy; // m_DsbFluidLossCoefficient;

  // lose this data
  //
  {
  if(version < CStreamVersion(3, 8, 2))
  {
      // DCasing material index
                  int nMaterial;
                  stream >> nMaterial;

      // Load DCasingGeneralParameters (13 CDoubleQuantity values)

      int iDefined;
      double dValue;
      for(int i = 0; i < 13; ++i)
      {
    stream >> iDefined;
    if(iDefined) stream >> dValue;
      }

      QString str;
      stream >> str;

      int iBool;
      stream >> iBool;
  }
  }

  LoadMeshIntersectionNew_Skip(stream, version, progress);

  if (version < CStreamVersion(3, 0, 42))
  LoadMeshIntersection_Skip(stream,version,progress);

  LoadWellLogData_Skip(stream,version,progress);

  if(version < CStreamVersion(3, 8, 2))
  LoadDCasint_Skip(stream,version,progress);

  // Fist
  {
  long NrOfFistPoints;
  stream >> NrOfFistPoints;

  for (int ii= 0; ii< NrOfFistPoints; ++ii)
  {
      double tmd;
      stream >> tmd;
      CNewWellPoint pt(*this, tmd);
      m_FistExportPoints.push_back(pt);
  }
  }

  // Derrick Elevation wjrx mantis 2549
  {
  double dDerElev;
  stream >> dDerElev;
  GlobalTVD(dDerElev);
  }

  // LAS Pointsets
  {
  // link LAS pointsets
  int sz;
  stream >> sz;
  TPointSetEntry* psentry = static_cast<TPointSetEntry*>(Model().GraphEntry(MD_BASE_POINTSET));

  for(int i = 0; i < sz; ++i)
  {
      int idx;
      stream >> idx;
      IPointSet* pPointSet = psentry->FindIndex(idx);
      const CLasPointSet* pLas= dynamic_cast<const CLasPointSet*>(pPointSet);
      assert(pLas);
      if (pLas)
      {
    const_cast<CLasPointSet*>(pLas)->NewWellPath(this);
    LinkTo(*pPointSet);
      }
  }
  }

  CGraphNode::create(Model().GraphEntry(MD_NEW_WELLPATH));
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  // zoomin
  {
  int iHasZoomIn;
  stream >> iHasZoomIn;

  if(iHasZoomIn)
  {
      DestroyZoomInModel();
      CreateZoomInModel();
      assert(m_pZoomInModel);
      CModelBase& model = static_cast<CModelBase&>(Model());
      model.LoadChildModel(stream, version, progress, *m_pZoomInModel);
  }
  }
}

// lose this data
//
void CNewWellPath::LoadMeshIntersectionNew_Skip
( TSTREAM& stream
, CStreamVersion& /*version*/
, TPROGRESS& /*progress*/)
{
  int NrOfInterSecPoints;
  stream >> NrOfInterSecPoints;

  for(int i =0;i<NrOfInterSecPoints;i++)
  {
  double tmd;
  stream >> tmd;
  }
}

// lose this data
//
void CNewWellPath::LoadMeshIntersection_Skip(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& /*progress*/)
{
  //load the mesh intersection points
  long NrOfMeshInterSecPoints;
  stream >> NrOfMeshInterSecPoints;

  int i;
  for(i =0;i<NrOfMeshInterSecPoints;i++)
  {
  double tmd;
  stream >> tmd;
  }

  //load the formation intersection points
  long NrOfFormInterSecPoints;
  stream >> NrOfFormInterSecPoints;

  for(i =0;i<NrOfFormInterSecPoints;i++)
  {
  double tmd;
  stream >> tmd;
  }

  {
  int use_defpoint;
  stream >> use_defpoint;
  stream >> use_defpoint;
  }

  // construct the formationsections
  for(i =0;i<NrOfFormInterSecPoints;i++)
  {
  int index;
  stream >> index;
  }
}

// lose this data
//
void CNewWellPath::LoadWellLogData_Skip
( TSTREAM& stream
, CStreamVersion& version
, TPROGRESS& progress
)
{
  if(version == CStreamVersion(3, 0, 38))
  {
  int nMapSize;
  stream >> nMapSize;
  stream >> nMapSize;
  stream >> nMapSize;
  return;
  }


  {
  // wedx 18012008:
  // Load formation sections status from file
  // Return if no sections are available
  int iHasFormationSections;
  stream >> iHasFormationSections;
  if(!iHasFormationSections)
      return;
  }

  int nSectionNr, nStageNr;
  int nMapSize;
  stream >> nMapSize;

  for(int d = 0; d < nMapSize; d++)
  {
  stream >> nStageNr;
  stream >> nSectionNr;
  LoadWellLog_Skip(stream, progress); // progress is increased in this function
  }

  stream >> nMapSize;
  int correction;
  for(int k = 0; k < nMapSize; k++)
  {
  stream >> nSectionNr;

  stream >> correction;
  progress.Step();
  }
}

// lose this data
//
void CNewWellPath::LoadWellLog_Skip
( /*well::CWellLog *log*/
  TSTREAM& stream
, TPROGRESS& progress
)
{
  int point_size, rat_size;
  double val;
  // std::vector<double> vcTMD;
  // std::vector<double> vcTVD;

  stream >> point_size;
  if(point_size > 0)
  {	
  // std::vector<double> vcShaly;
  // std::vector<double> vcComp;

  // load the tmd's
  for(int i = 0; i < point_size; i++)
  {
      stream >> val;
      // vcTMD.push_back(val);
  }
  // now the tvd's
  for(int j = 0; j < point_size; j++)
  {
      stream >>val;
      // vcTVD.push_back(val);
  }
  // next the shalyness...
  for(int k = 0; k < point_size; k++)
  {
      stream >> val;
      // vcShaly.push_back(val);
  }
  // the compressibility.....
  for(int l = 0; l < point_size; l++)
  {
      stream >> val;
      // vcComp.push_back(val);
  }
  double sand_cutoff;
  stream >> sand_cutoff;

  // intitiate the log...
  // log->InitLog(vcTMD, vcTVD, vcShaly, vcComp, sand_cutoff);
  }
  stream >> rat_size;
  if(rat_size > 0)
  {
  // vcTMD.clear();
  // vcTVD.clear();
  // std::vector<double> vcRelDispl;

  // load the tmd's...
  for(int m = 0; m < rat_size; m++)
  {
      stream >> val;
      // vcTMD.push_back(val);
  }
  // now the tvd's...
  for(int n = 0; n < rat_size; n++)
  {
      stream >> val;
      // vcTVD.push_back(val);
  }
  // now the relative displacements....
  for(int z = 0; z < rat_size; z++)
  {
      stream >> val;
      // vcRelDispl.push_back(val);
  }

  // initiate the radioactive tags...
  // log->InitRadioactiveTags(vcTMD, vcTVD, vcRelDispl);
  }

  progress.Step();
}

// lose this data
//
void CNewWellPath::LoadDCasint_Skip
( TSTREAM& stream
, CStreamVersion& version
, TPROGRESS& progress
)
{
  long nSize;
  stream >> nSize;

  for(int i=0; i< nSize ; i++)
  { 
  double tmd;
  // store
  stream >> tmd;

  int iDefined;
  double dValue;
  for(int i2 = 0; i2 < 13; ++i2)
  {
      stream >> iDefined;
      if(iDefined)
      stream >> dValue;
  }
  }

  stream >> nSize;

  //loop over casint-result and store 
  for(int i=0; i< nSize ; i++)
  {
  int dep_index;
  int point_index;
  CAnalysisType antype;

  antype.LoadStream(stream, version, progress);
  stream >> dep_index;
  stream >> point_index;

  double ddum;
  int idum;
  stream >> ddum;
  stream >> ddum;
  stream >> ddum;
  stream >> idum;

  stream >> ddum;
  stream >> ddum;
  stream >> ddum;
  stream >> idum;

  stream >> ddum;
  stream >> ddum;
  stream >> idum;
  }
}


void CNewWellPath::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // wjrx mantis 3687: read CWellPath and treat it as CNewWellPath
  //
  if ( dynamic_cast<CNewArchiveStdStringStream *>(&stream) != 0 )
  {
  LoadStreamOldToNew(stream, version, progress);

  CNewWellPathInput *input = new CNewWellPathInput(*this);
  CNewWellPathInput *unique = CNewWellPathInput::EnsureUniqueness(input);

  m_input = unique;

  if (m_input)
      const_cast<CNewWellPathInput *>(m_input)->LinkTo(*this);

  return;
  }

  // Load the base first
  CColorNode::LoadStream(stream, version, progress);

  // Set progress indicator
  std::string pr("Loading WellPath (ps) ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  if (version > CStreamVersion(4, 1, 42))
  {
  int inputIndex;
  stream >> inputIndex;

  m_input = CNewWellPathInput::FindByIndex(static_cast<const CModelBase&>(Model()), inputIndex);
  }

  //Load Birth Date 
  int year;
  int month;
  int day;
  stream >> year;
  stream >> month;
  stream >> day;
  SetBirthDate(QDate(year, month, day));

  if ( version >= CStreamVersion(4,1,7) )
  stream >> m_dMudGradRefDepth;

  // Load wellpath pointset
  int nPoints;
  stream >> nPoints;

  QVector<geo::CPoint> vcPoint;
  vcPoint.reserve(nPoints);

  for (int ii= 0; ii< nPoints; ++ii)
  {
  double x,y,z;
  stream >> x;
  stream >> y;
  stream >> z;
  vcPoint.push_back(geo::CPoint(x,y,z));
  }
  InitFromPointArray( vcPoint);

  CGraphNode::create( Model().GraphEntry(MD_NEW_WELLPATH) );
  LinkTo(dynamic_cast<CModelBase&>(Model()).Mesh());
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  if (!m_input)
  {
  CNewWellPathInput *input = new CNewWellPathInput(*this);
  CNewWellPathInput *unique = CNewWellPathInput::EnsureUniqueness(input);

  if (unique)
      unique->Color(Color());

  m_input = unique;
  }
  if (m_input)
  const_cast<CNewWellPathInput *>(m_input)->LinkTo(*this);

  // wjrx mantis 3401
  if(version >= CStreamVersion(3, 7, 27) )
  {
  int iHasZoomIn;
  stream >> iHasZoomIn;
  
  if ( iHasZoomIn )
  {
      DestroyZoomInModel();
      CreateZoomInModel();
      assert(m_pZoomInModel);
      CModelBase& model = static_cast<CModelBase&>(Model());
      model.LoadChildModel(stream, version, progress, *m_pZoomInModel);
  }
  }

  if(version >= CStreamVersion(4, 1, 3) )
  {
  unsigned int NrOfFistPoints;
  stream >> NrOfFistPoints;

  for (unsigned int ii= 0; ii< NrOfFistPoints; ++ii)
  {
      double tmd;
      stream >> tmd;
      CNewWellPoint pt(*this, tmd);
      m_FistExportPoints.push_back(pt);
  }
  }

  if ( version >= CStreamVersion(4, 1, 8 ) )
  {
  int nDStaborSize;
  stream >> nDStaborSize;
  for (int ii= 0; ii< nDStaborSize; ++ii)
  {
      double x, y, z, TMD;
      stream >> x;
      stream >> y;
      stream >> z;
      stream >> TMD;

      CNewWellPoint point(*this, TMD);
      DStaborPoints().push_back(point);

      // Get the results 
      int bLoadRes;
      stream >> bLoadRes;

      while(bLoadRes)
      {
    // Load depletion stage
    int nIndex;
    CAnalysisType antype;
    stream >> nIndex;

    if(nIndex < 0 )  //old databse
        nIndex=0;
    
    CDepletionStageEntry& dep_entry = (CDepletionStageEntry&)*Model().GraphEntry(MD_BASE_DEPLETION_STAGE);
    CDepletionStage *pDepl = dep_entry.FindIndex(nIndex);
      int bLinear;

    if(pDepl==0) //old database
    {
          //fake reading
          double dum;
          int idum;
          stream >> bLinear;
          stream >> idum;
          stream >> dum;
          stream >> bLoadRes;
          continue;
    }
    int status;
    double value;
    // Load linearity tag, status and value
    stream >> bLinear;
    CDsbStatusValue &rs = DsbResults().GetResult(point, *pDepl, (bLinear ? CAnalysisType::AT_LINEAR : CAnalysisType::AT_NONLIN));
    stream >> status;
    stream >> value ;
    rs.Value(value, (CDsbStatusValue::eDsbStatus)status);
    
    // Continue?
    stream >> bLoadRes;
      }

      progress.Step();
  }
  } // if ( version >= CStreamVersion(4, 1, 8 ) )
}

CGroupNode *CNewWellPath::GetGroup() const
{
  std::set<CGroupNode *> groups = Links<CGroupNode>();

  return groups.empty() ? nullptr : *groups.begin();
}

bool CNewWellPath::CanDestroy() const
{
  CGroupNode *group = GetGroup();

  if (group && group->ReadOnly())
  return false;

  return CanZoomIn();
}

bool CNewWellPath::Destroy()
{
  UnLinkAll();
  return CColorNode::Destroy();
}

bool CNewWellPath::CanDisconnectItem(const CGraphNode& item)
{
  const CGroupNode *group = dynamic_cast<const CGroupNode *>(&item);

  return !group || !group->ReadOnly();
}

void CNewWellPath::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if ( &node == &static_cast<CModelBase&>(Model()).Mesh() && uHint == MeshCreated )
  {
  Clear();
  UpdateFormationSections();
  Modified();
  }
  else if (&node == m_input)
  {
  if (uHint != COLOR_CHANGED)
  {
      if (uHint == WELLPATH_FLIPPED)
      {
    InitFromPointSet(*m_input);
      }
      else
      {
    SetBirthDate(m_input->BirthDate());
    Name(m_input->Name());

    if (uHint != NAME_CHANGED)
    {
          if (m_input->IsVertical())
      Recalculate(m_input->GlobalNorthing(), m_input->GlobalEasting(), m_input->GlobalTVD());
          else
      Recalculate(m_input->GlobalTVD() - GlobalTVD());
    }
      }
      if (uHint != NAME_CHANGED)
    UpdateFormationSections();
  }
  Modified(uHint);
  }

  CColorNode::OnNeighbourModified(node,uHint);
}

void CNewWellPath::OnNeighbourDeleted(const CGraphNode &node)
{
  if(&node == m_pZoomInModel)
  m_pZoomInModel= 0;
  CColorNode::OnNeighbourDeleted(node);
}

void CNewWellPath::OnNewNeighbour(const CGraphNode& node)
{
  FormationSections();
  CColorNode::OnNewNeighbour(node);
}

const std::list<CNewFormationSection>& CNewWellPath::FormationSections() const
{
  return const_cast<CNewWellPath *>(this)->FormationSections();
}

std::list<CNewFormationSection>& CNewWellPath::FormationSections()
{
  if ( m_FormationSections.size() == 0 )
  UpdateFormationSections();

  return m_FormationSections;
}


// This function creates the formation-sections,
// based on the formation intersection points
//
void CNewWellPath::UpdateFormationSections()
{
  if ( m_WellPathTMD.size() < 2) return;

  CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());
  if(!pModel->IsMesh()) return;

  Clear();

  std::list<CNewGeoWellPoint>& formIntersecPointsList = FormIntersecPoints();
  if(formIntersecPointsList.size()== 0)
  return;

  std::list<CNewGeoWellPoint>::iterator it = formIntersecPointsList.begin();

  // Intersection points of both formation surfaces with the well
  //
  CNewGeoWellPoint*  pFormationTopPoint = 0;
  CNewGeoWellPoint*  pFormationBottomPoint = 0;

  if(it != formIntersecPointsList.end())
  {
  pFormationTopPoint = &(*it);
  ++it;

  if(it != formIntersecPointsList.end())
      pFormationBottomPoint = &(*it);
  }

  while(pFormationTopPoint && pFormationBottomPoint)
  {
  CNewGeoWellPoint mid(*this,(pFormationTopPoint->TMD() + pFormationBottomPoint->TMD())/2.0);


  if(!mid.Formation())
  { // this only happens when there is a void in the model
      // caused by a fault made up from two surfaces
      pFormationTopPoint = pFormationBottomPoint;
      if ( it != formIntersecPointsList.end() )
      {
    ++it;
    if(it != formIntersecPointsList.end())
          pFormationBottomPoint = &(*it);
    else
          pFormationBottomPoint = 0;
      }
      continue;
  }
  CNewFormationSection
      section(*this, mid.Formation(), pFormationTopPoint->TMD(), pFormationBottomPoint->TMD());
  m_FormationSections.push_back(section);
  m_FormationSectionPointers.push_back(&*m_FormationSections.rbegin());

  //
  // ------------------------------- wellPath  
  //                 ^
  //                 |
  //                 | offset
  //                 |
  //                 V
  //         <--------------->
  //             arrowLine
  //
  //
  // arrow line offset from wellpath
  //
  // Offset depends on well path length
  //
  const double offset= ( m_WellPathTMD[m_WellPathTMD.size()-1] - 
      m_WellPathTMD[0] ) / 20.0;

  // Arrow line offset is applied to direction of smallest change.
  // Usually, Z is the direction of largest change.
  //
  geo::CPoint offsetVector= *pFormationTopPoint - *pFormationBottomPoint;
  offsetVector.X( fabs(offsetVector.X()) );
  offsetVector.Y( fabs(offsetVector.Y()) );
  offsetVector.Z( fabs(offsetVector.Z()) );

  if ( offsetVector.X() < offsetVector.Y() ) 
  {
      if ( offsetVector.X() < offsetVector.Z() ) 
    offsetVector= geo::CPoint(offset,0,0);
      else
    offsetVector= geo::CPoint(0,0,offset);
  }
  else // X >= Y
  {
      if ( offsetVector.Y() < offsetVector.Z() ) 
    offsetVector= geo::CPoint(0,offset,0);
      else
    offsetVector= geo::CPoint(0,0,offset);
  }
  
  //
  // top arrow
  //
  geo::CPoint topArrowPoint= *pFormationTopPoint + offsetVector;

  //
  // ------------------------------- wellPath  
  //         |               |
  // topLine |               | bottomLine
  //         |               |
  //         <--------------->
  //             arrowLine
  //
  geo::CPolyLine topLine;
  topLine.PushBack(topArrowPoint);
  topLine.PushBack(*pFormationTopPoint);

  geo::CPolyLine arrowLine;
  arrowLine.PushBack(topArrowPoint);

  /* 
  //      + topArrowPoint
  //     /|\
  //    /_+_\ ArrowBase
  //      |
  //      |
  //       \
  //        \ arrowLine follows the wellpath at offset
  //         \
  //          \
  //           +
  */
  bool bArrowBaseFound= false;

  // determine arrowLine and the ArrowBase
  for (int ii= 0; ii< m_WellPathPolyLine.PointSize(); ++ii)
  {
      // if we have a very short formation and have not determined
      // the basepoint of the arrow yet ...
      //
      if ( m_WellPathTMD[ii] > pFormationBottomPoint->TMD() )
      {
    if ( !bArrowBaseFound )
    {
          geo::CPoint pt= *pFormationBottomPoint + offsetVector;
          if ( geo::CLine(pt, topArrowPoint).SquareLength() > EPS*EPS)
          {
      bArrowBaseFound= true;
      geo::CVector dir( pt, topArrowPoint);
      m_FormationArrows.push_back(
              new CVectorSymbol( topArrowPoint,dir,20,false)
              );
          }
    }
      }
      else if ( m_WellPathTMD[ii] > pFormationTopPoint->TMD() )
      {
    // fill arrowLine
    geo::CPoint pt= m_WellPathPolyLine.Point(ii) + offsetVector;
    arrowLine.PushBack(pt);

    if ( ! bArrowBaseFound )
    {
          if ( geo::CLine(pt, topArrowPoint).SquareLength() > EPS*EPS)
          {
      bArrowBaseFound= true;
      geo::CVector dir( pt, topArrowPoint);
      m_FormationArrows.push_back(
              new CVectorSymbol( topArrowPoint,dir,20,false));
          }
    }
      }
  }

  //
  // bottom arrow
  //
  geo::CPoint bottomArrowPoint= *pFormationBottomPoint +offsetVector;

  geo::CPolyLine bottomLine;
  bottomLine.PushBack(bottomArrowPoint);
  bottomLine.PushBack(*pFormationBottomPoint);

  const geo::IPoint *pArrowBase= 0;
  for ( int ii= arrowLine.PointSize()-1; ii>=0 && pArrowBase==0; --ii)
  {
      if ( arrowLine.Point(ii) != bottomArrowPoint )
      {
    pArrowBase= &arrowLine.Point(ii);
    if (geo::CLine(*pArrowBase, bottomArrowPoint).SquareLength() > EPS*EPS)
    {
          geo::CVector dir2( *pArrowBase, bottomArrowPoint);
          m_FormationArrows.push_back(
      new CVectorSymbol( bottomArrowPoint,dir2,20,false));
    }

      }
  }

  arrowLine.PushBack(bottomArrowPoint);

  if ( arrowLine.PointSize() >= 2 )
  {
      m_FormationLines.push_back( arrowLine);
      m_FormationLines.push_back( topLine);
      m_FormationLines.push_back( bottomLine);
      geo::CPoint textPos= mid +offsetVector;
      textPos.X( textPos.X() +4);
      textPos.Y( textPos.Y() +4);
      m_FormationLabels.push_back
    ( CLabelPoint(textPos,QString(mid.Formation()->Name())));
  }

  pFormationTopPoint    = pFormationBottomPoint;

  ++it;
  if(it != formIntersecPointsList.end())
      pFormationBottomPoint = &(*it);
  else
      pFormationBottomPoint = 0;
  }

  if (! m_FormationSections.empty() ) 
  Modified();
}

const CNewFormationSection *CNewWellPath::FormationSection
(const CNewWellPoint &pt) const
{
  const std::list<CNewFormationSection> &list= FormationSections();
  CNewFormationSectionList newFormationSectionList(list);
  std::list <const CNewFormationSection*> formation_list =
  newFormationSectionList.getSections(pt);

  if( formation_list.empty() ) return 0;
  assert( formation_list.size() == 1);
  return *formation_list.begin();
}

std::list<CNewGeoWellPoint>& CNewWellPath::FormIntersecPoints()
{
  if ( m_FormationIntersectionPoints.size() == 0 )
  CalculateFormationIntersecPoints();

  return m_FormationIntersectionPoints;
}

void CNewWellPath::CalculateFormationIntersecPoints()
{
  Clear();

  const std::list<CNewWellPoint> &MeshPoints = MeshIntersecPoints();

  // wjrx mantis 3842 Wellpath does not intersect mesh
  //
  if ( MeshIntersecPoints().empty() )
  return;

  std::list<CNewWellPoint>::const_iterator it = MeshPoints.begin();

  const CNewWellPoint* p1 = 0;
  const CNewWellPoint* p2 = 0;

  if(it != MeshPoints.end())
  {
  p1 = &(*it);
  ++it;
  if(it != MeshPoints.end())
      p2 = &(*it);
  }

  const CFormationBase* pCurrentFormation=0;

  while(p1 && p2)
  {
  //take a point in the middle
  double middle = (p1->TMD() + p2->TMD())/2.0;
  CNewGeoWellPoint mid1(*this,middle);
  mid1.TVD( (p1->TVD()+p2->TVD())/2.0);
  const CFormationBase* form = mid1.Formation();

  // wjrx mantis 2540
  // If the midpoint is not in the formation look near p1 and p2
  //
  if ( ! form )
  {
      middle = p1->TMD() + 0.01 * (p2->TMD() - p1->TMD());
      CNewGeoWellPoint mid2(*this,middle);
      mid2.TVD( p1->TVD() + 0.01 * (p2->TVD() - p1->TVD()) );
      form = mid2.Formation();
  }

  // wjrx mantis 2540
  if ( ! form )
  {
      middle = p1->TMD() + 0.99 * (p2->TMD() - p1->TMD());
      CNewGeoWellPoint mid3(*this,middle);
      mid3.TVD( p1->TVD() + 0.99 * (p2->TVD() - p1->TVD()) );
      form= mid3.Formation();
  }

  if ( pCurrentFormation != form )
  {
      pCurrentFormation= form;
      CNewGeoWellPoint p(*this,p1->TMD());
      m_FormationIntersectionPoints.push_back(p);
  }

/*
  if(!form)
  {
      CNewGeoWellPoint p(*this,p1->TMD());
      lst.push_back(p);
      pForm = form;
  }
  else if( p1 == &(*(MeshIntersecPoints().begin() ) ) )
  {
      CNewGeoWellPoint p(*this,p1->TMD());
      lst.push_back(p);
      pForm = form;
  }
  else
  {
      if(!pForm)
      {
    pForm = form;
      }
      else if(pForm != form)
      {
    pForm = form;
    CNewGeoWellPoint p(*this,p1->TMD());
    lst.push_back(p);
      }
  }
*/
  p1 = p2;
  ++it;
  if(it != MeshPoints.end())
      p2 = &(*it);
  else
      p2 = 0;
  }

  // wedx 09052008: add the last point
  if(!p2)
  {
  if(m_WellPathTMD[m_WellPathTMD.size()-1] > p1->TMD() + EPS)
  {
      CNewGeoWellPoint p(*this,m_WellPathTMD[m_WellPathTMD.size()-1]);
      if(p.Defined())
      {
    m_FormationIntersectionPoints.push_back(p);
      }
      else
      {
    CNewGeoWellPoint p3(*this,p1->TMD());
    m_FormationIntersectionPoints.push_back(p3);
      }
  }
  else
  {
      CNewGeoWellPoint p(*this,p1->TMD());
      m_FormationIntersectionPoints.push_back(p);
  }
  }
}

const std::list<CNewWellPoint>& CNewWellPath::MeshIntersecPoints()
{
  if ( m_MeshIntersectionPoints.size() == 0 )
  CalculateMeshIntersecPoints();

  return m_MeshIntersectionPoints;
}

void CNewWellPath::CalculateMeshIntersecPoints()
{
  CalculateIntersections(dynamic_cast<CModelBase*>(&Model())->Mesh().Mesh());
}

namespace
{

  class ZLess
  {
  public:
  bool operator()(const CNewWellPoint& p1, const CNewWellPoint& p2) const
  {
      double TMD1 = round(p1.TMD() * 1E8) / 1E8;
      double TMD2 = round(p2.TMD() * 1E8) / 1E8;
      return TMD1 < TMD2;
  }
  };

} // anonymous namespace

void CNewWellPath::CalculateIntersections(const geo::IMesh& mesh)
{
  if ( m_WellPathPolyLine.PointSize() < 2 ) return;

  assert(m_WellPathPolyLine.PointSize() == m_WellPathTMD.size());

  int lineIndex= 0;
  const geo::IPoint *pWellPoint= &m_WellPathPolyLine.Point(lineIndex);
  double dWellPointTMD= m_WellPathTMD[lineIndex];

  typedef std::set<CNewWellPoint, ZLess> TWellPointSet;
  TWellPointSet stPoints;

  while( ++lineIndex < m_WellPathPolyLine.PointSize())
  {
  const geo::IPoint *pNext= &m_WellPathPolyLine.Point(lineIndex);
  double dNextTMD= m_WellPathTMD[lineIndex];

  CNewWellPoint pt1(*this,*pWellPoint,dWellPointTMD);
  CNewWellPoint pt2(*this,*pNext,dNextTMD);

  std::set<int> stElementIndices =
      mesh.Candidates(pWellPoint->Min(*pNext), pWellPoint->Max(*pNext));

  for( std::set<int>::iterator it_eltIndex = stElementIndices.begin()
       ; it_eltIndex != stElementIndices.end()
       ; it_eltIndex++)
  {
      const geo::IBody* pInspectedBodyElement =
    dynamic_cast<const geo::IBody*>(&mesh.Element(*it_eltIndex));

      if( ! pInspectedBodyElement ) continue;

      const geo::IBody *pIntersectedBodyElement= 0;
      const geo::IFace *pIntersectedFace= 0;

      if(geo::BBox::Intersects( *pWellPoint
                             , *pNext
                             , pInspectedBodyElement->Max()
                             , pInspectedBodyElement->Min()
                             , true)
    )
      {
    for( int inspectedBodyElementFaceIndex = 0
           ; inspectedBodyElementFaceIndex < pInspectedBodyElement->NrOfFaces()
           ; inspectedBodyElementFaceIndex++)
    {
          const geo::IFace& inspectedFace=
      pInspectedBodyElement->Face(inspectedBodyElementFaceIndex);
          if(geo::BBox::Intersects( *pWellPoint
                                 , *pNext
                                 , inspectedFace.Max()
                                 , inspectedFace.Min()
                                 , true)
      )
          {
      CNewWellPoint intersectionPoint(*this,0,0,0,0);
      if ( CalculateInterSectionWithFace
                   (inspectedFace,pt1,pt2,intersectionPoint)
               )
      {
              pIntersectedFace= &inspectedFace;
              pIntersectedBodyElement= pInspectedBodyElement;

              // If we intersected at this intersectionPoint before,
              // we only add the intersected element to it 
              // else
              // we also add intersectionPoint to stPoints
              //
              TWellPointSet::iterator it= stPoints.find(intersectionPoint);

              if ( it != stPoints.end() )
              {
        (*it).addElement(pIntersectedBodyElement);
              }
              else
              {
        intersectionPoint.addElement(pIntersectedBodyElement);
        stPoints.insert(intersectionPoint);
              }
      }
    else
    {
              pIntersectedFace= 0;
              pIntersectedBodyElement= 0;
              continue; // no intersection, check the next face
      }

      // wjrx mantis 3756
      // Find out if the wellpath leaves the mesh.
      //
      bool bMeshEdgeIntersection= false;

      if (  pIntersectedFace 
               && pIntersectedFace->NrOfNodes() > 0 )
      {
              // We are going to query all elements that are attached
              // to the intersected face. For this we query one of
              // the nodes of pIntersectedFace.
              //
              const geo::INode &IntersectedFaceNode0=
        pIntersectedFace->Node(0);

              // If there is only one element attached to the node of
              // the intersected face, we are on the edge of the mesh.
              //
              bMeshEdgeIntersection= true;
              if (IntersectedFaceNode0.AttachedElementSize() != 1 )
              for ( int attachedElementIndex= 0
                  ; attachedElementIndex<
                      IntersectedFaceNode0.AttachedElementSize()
                  ; ++attachedElementIndex
                  )
              {
        const geo::IElement& AttachedElement=
                  IntersectedFaceNode0.AttachedElement(attachedElementIndex);

        // One of the elements attached to the face-node is the
        // element that owns the face. We want to know about the
        // OTHER elements.
        //
        if ( &AttachedElement == pIntersectedBodyElement ) continue;
        
        const geo::CInterfaceElement *pAttachedInterfaceElement= 
                  dynamic_cast<const geo::CInterfaceElement*>(&AttachedElement);
        const geo::IBody *pAttachedBody= 
                  dynamic_cast<const geo::IBody*>(&AttachedElement);

        assert (pAttachedInterfaceElement || pAttachedBody);

        // If the queried element is an interface element AND
        // the front or backface is absent, we might be on the edge
        // of the mesh.
        //
        // According to Chantal Frissen (TNO/Diana),
        // interface elements only exist at the edge of the mesh
        // for the zoom-in model.
        //
        if ( pAttachedInterfaceElement
                   && ( pAttachedInterfaceElement->FrontFace()==0 
                      || pAttachedInterfaceElement->BackFace()==0
                      )
                   )
        {
                  const geo::IFace *pFace= 0;
                  if ( ! ( pFace= pAttachedInterfaceElement->FrontFace() ) )
          pFace= pAttachedInterfaceElement->BackFace();

                  assert(pFace);
                  assert(pFace->NrOfNodes() == pIntersectedFace->NrOfNodes() );

                  // If the intersected element and the attached interface
                  // element (with just one face) share all nodes,
                  // we are crossing the mesh edge.
                  //
                  unsigned int identicalNodeCounter= 0;

                  for ( int attachedFaceNodeIndex= 0
                      ; attachedFaceNodeIndex< pFace->NrOfNodes()
                      ; ++attachedFaceNodeIndex
                      )
                  {
          for ( int intersectedFaceNodeIndex= 0
            ; intersectedFaceNodeIndex
                          < pIntersectedFace->NrOfNodes()
            ; ++intersectedFaceNodeIndex
            )
          {
                      if ( &pFace->Node(attachedFaceNodeIndex)
                          == &pIntersectedFace->Node(intersectedFaceNodeIndex) )
                      {
            ++identicalNodeCounter;
            break;
                      }
          }
                  }
                  if ( pIntersectedFace->NrOfNodes() == identicalNodeCounter)
          bMeshEdgeIntersection= false;
        }
        else if ( pAttachedBody ) // IBody
        {
                  // Requirement for a mesh edge intersection:
                  // Each face of pAttachedBody may not share all nodes
                  // with pIntersectedFace
                  //
                  for ( int attachedBodyFaceIndex= 0
                      ; attachedBodyFaceIndex <pAttachedBody->NrOfFaces()
                      ; ++attachedBodyFaceIndex 
                      )
                  {
          unsigned int identicalNodeCounter= 0;
          const geo::IFace& AttachedFace =
                      pAttachedBody->Face(attachedBodyFaceIndex);

          for ( int attachedFaceNodeIndex= 0
            ; attachedFaceNodeIndex< AttachedFace.NrOfNodes()
            ; ++attachedFaceNodeIndex
            )
          {

                      for ( int intersectedFaceNodeIndex= 0
                          ; intersectedFaceNodeIndex
              < pIntersectedFace->NrOfNodes()
                          ; ++intersectedFaceNodeIndex
                          )
                      {
            if ( &AttachedFace.Node(attachedFaceNodeIndex)
                             == &pIntersectedFace->
                               Node(intersectedFaceNodeIndex) )
            {
                          ++identicalNodeCounter;
                          break;
            }
                      }
          }
          if (pIntersectedFace->NrOfNodes() == identicalNodeCounter)
                      bMeshEdgeIntersection= false;
                  }
        } // else if ( pAttachedBody )
              } // for ( int attachedElementIndex= 0
      } // if ( pIntersectedFace )

      if ( bMeshEdgeIntersection)
      {
              TWellPointSet::iterator it= stPoints.find(intersectionPoint);

              assert( it != stPoints.end() );

              if ( it != stPoints.end() )
              {
        (*it).addElement(0); // signifies a mesh edge intersection
              }
      }
          } // if( InterSecBoundingBox( *pWellPoint <for face>
    } // for( int inspectedBodyElementFaceIndex = 0
      } // if( InterSecBoundingBox( *pWellPoint <for element>
  } // for( std::set<int>::iterator it_eltIndex

  pWellPoint = pNext;
  dWellPointTMD = dNextTMD;
  }

  for(TWellPointSet::iterator it = stPoints.begin(); it != stPoints.end(); ++it)
  m_MeshIntersectionPoints.push_back(*it);

  // wjrx mantis 3716
  //
  // When the first and/or last well point is inside a mesh element,
  // we treat it as an intersection point.
  //
  // We need to take special care if these 2 points are exactly on a face.
  //
  CNewGeoWellPoint firstPoint(*this, m_WellPathTMD.front() );
  if ( firstPoint.Formation()
     && (  m_MeshIntersectionPoints.empty()
    || m_WellPathTMD.front() < m_MeshIntersectionPoints.front().TMD()
    )
     ) 
  {
  m_MeshIntersectionPoints.push_front( firstPoint);
  }

  CNewGeoWellPoint lastPoint(*this, m_WellPathTMD.back() );
  if ( lastPoint.Formation()
     && ( m_MeshIntersectionPoints.empty()
    || m_WellPathTMD.back() > m_MeshIntersectionPoints.back().TMD()
    )
     ) 
  {
  m_MeshIntersectionPoints.push_back( lastPoint);
  }

  // end wjrx mantis 3716

}

void CNewWellPath::ClearAndDelete()
{
  m_WellPathPolyLine.RemoveAll();
  m_WellPathTMD.clear();
  ClearAngles();
}


void CNewWellPath::Clear()
{
  //m_WellPathPolyLine.RemoveAll();
  //m_WellPathTMD.clear();
  m_FormationLines.clear();
  m_FormationLabels.clear();
  for ( size_t ii= 0; ii< m_FormationArrows.size(); ++ii)
  delete m_FormationArrows[ii];
  m_FormationArrows.clear();
  m_MeshIntersectionPoints.clear();
  m_FormationIntersectionPoints.clear();
  m_FormationSections.clear();
  m_FormationSectionPointers.clear();
  m_GraphPoints.clear();
  DestroyZoomInModel();
}

const std::list<CNewWellPoint> & CNewWellPath::DStaborPoints() const
{
  return m_DStaborPoints;
}

std::list<CNewWellPoint> & CNewWellPath::DStaborPoints(bool bCheck)
{
  if ( bCheck)
  {
  std::list<CNewWellPoint>::iterator it= m_DStaborPoints.begin();
  CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());
  CValidateModel validateModel(pModel);

  while ( it != m_DStaborPoints.end() )
  {
      CNewGeoWellPoint gwp( *pModel, *it);
      const CFormationBase *pFormation= gwp.Formation();
      if ( pFormation==0 || validateModel.checkMaterial(*pFormation)==false)
    it= m_DStaborPoints.erase(it);
      else
    ++it;
  }
  }
  return m_DStaborPoints;
}

// wjrx mantis 3322 Attributes
// offset comes from a change in dialog Wellpath_attr/Derrick Depth Coord
void CNewWellPath::Recalculate(const double &offset)
{
  QVector<geo::CPoint> tmpLine;
  tmpLine.reserve( m_WellPathPolyLine.PointSize());

  for ( int ii= 0; ii< m_WellPathPolyLine.PointSize(); ++ii)
  {
  const geo::IPoint &curPt= m_WellPathPolyLine.Point(ii);
  tmpLine.push_back(geo::CPoint( curPt.X(), curPt.Y(), curPt.Z()+offset));
  }

  InitFromPointArray( tmpLine);

  Modified();
}

void CNewWellPath::Recalculate( const geo::CPoint &point)
{
  Recalculate( point.X(), point.Y(), point.Z() );
}

void CNewWellPath::Recalculate
( const double &Global_Northing
, const double &Global_Easting
, const double &Global_TVD
)
{
  GlobalNorthing(Global_Northing);
  GlobalEasting(Global_Easting);
  GlobalTVD(Global_TVD);

  m_WellPathPolyLine.RemoveAll();
  m_WellPathTMD.clear();
  ClearAngles();
  Clear();
  m_VerticalPointDef= geo::CPoint( Global_Northing, Global_Easting, Global_TVD);
  InitVerticalWell(false);
  FormationSections();
  Modified();
}

// wjrx mantis 3822
//
geo::CPoint CNewWellPath::QuadLineIntersect
( const geo::IQuadrilateral &quad
, const geo::ILine &line
)
{
  geo::CPoint ret;
  /*
  // Elements are preferably created in such a way that the angles
  // between the sides are as close to 60 degrees as possible.
  // This is approximated by cutting the quad into two triangles across
  // the shortest of the two diagonals.
  // 
  //       3                      0
  //       |\       triangles     |\       triangles
  //       | \      (0,1,2)       | \      (1,3,0)
  //       |  \     (2,3,0)       |  \     (1,2,3)
  //       |   \                  |   \
  //       |    \ 2               |    \ 3
  //       |     |                |     |
  //       |_____|                |_____|
  //      0       1              1       2
  */
  if ( geo::CLine(quad.Point(0), quad.Point(2)).SquareLength() <=
       geo::CLine(quad.Point(1), quad.Point(3)).SquareLength() )
  {
  geo::CTriangle triangle1( quad.Point(0), quad.Point(1), quad.Point(2));
  ret= triangle1.Intersection(line);
  if (ret.Empty() )
  {
      geo::CTriangle triangle2( quad.Point(2), quad.Point(3), quad.Point(0));
      ret= triangle2.Intersection(line);
  }
  }
  else
  {
  geo::CTriangle triangle1( quad.Point(1), quad.Point(3), quad.Point(0));
  ret= triangle1.Intersection(line);
  if (ret.Empty() )
  {
      geo::CTriangle triangle2( quad.Point(1), quad.Point(2), quad.Point(3));
      ret= triangle2.Intersection(line);
  }
  }

  return ret;
}
  

bool CNewWellPath::CalculateInterSectionWithFace
( const geo::IFace &face
, const CNewWellPoint &pt1
, const CNewWellPoint &pt2
, CNewWellPoint &intersection // output, invalid if return false
) const
{
  assert( pt1 != pt2 );
  if ( pt1 == pt2 ) return false;

  geo::CPoint point;
  geo::CLine line( pt1,pt2);

  const geo::IQuadrilateral *pQuad=
  dynamic_cast<const geo::IQuadrilateral *>(&face);
  if ( pQuad != 0 )
  point= CNewWellPath::QuadLineIntersect( *pQuad, line);
  else
  point= face.Intersection( line);

  if ( point.Empty() ) return false;

  // 'fraction' determines if intersection 'point' is between 
  // arguments 'pt1' and 'pt2'
  //
  // 'fraction' is also used for determining TMD
  //
  double fraction= 0;
  if ( pt1.X() != pt2.X() )
  fraction= (point.X()-pt1.X()) / (pt2.X()-pt1.X());
  else if ( pt1.Y() != pt2.Y() )
  fraction= (point.Y()-pt1.Y()) / (pt2.Y()-pt1.Y());
  else
  fraction= (point.Z()-pt1.Z()) / (pt2.Z()-pt1.Z());

  if ( fraction < 0 || fraction > 1 ) return false;

  intersection.Northing( point.X() - GlobalNorthing());
  intersection.Easting( point.Y() - GlobalEasting());
  intersection.TVD( point.Z() - GlobalTVD() );
  intersection.TMD( pt1.TMD() + fraction * (pt2.TMD()-pt1.TMD()) );
  return true;
}

bool CNewWellPath::InitWellPoint(CNewWellPoint &point) const
{
  unsigned int ptSize= this->WellPointSize();

  if ( ptSize == 0 )
  {
  point.Northing(0);
  point.Easting(0);
  point.TVD(0);
  return false;
  }

  CNewWellPoint firstPt= this->WellPoint(0);
  if ( point.TMD() <= firstPt.TMD() )
  {
  point.Northing( firstPt.Northing());
  point.Easting( firstPt.Easting());
  point.TVD( firstPt.TVD());
  return true;
  }

  CNewWellPoint lastPt= this->WellPoint(ptSize-1);
  if ( point.TMD() >= lastPt.TMD() )
  {
  point.Northing( lastPt.Northing());
  point.Easting( lastPt.Easting());
  point.TVD( lastPt.TVD());
  return true;
  }

  // ptSize >= 2
  //
  for ( unsigned int ii= 0; ii< ptSize-1; ++ii)
  {
  CNewWellPoint pt= this->WellPoint(ii);
  CNewWellPoint ptNext= this->WellPoint(ii+1);
  if (  pt.TMD() <= point.TMD() && ptNext.TMD() >= point.TMD() )
  {
      // new point is between pt and ptNext
      //
      double frac;
      if ( pt.TMD() == ptNext.TMD() )
       frac= 0.5 ;
      else
       frac= ( point.TMD() - pt.TMD() ) / ( ptNext.TMD() - pt.TMD() );

      double deltaNorthing= ptNext.Northing() - pt.Northing();
      double deltaEasting = ptNext.Easting () - pt.Easting();
      double deltaTVD     = ptNext.TVD     () - pt.TVD();

      point.Northing( pt.Northing() + frac * deltaNorthing);
      point.Easting ( pt.Easting () + frac * deltaEasting );
      point.TVD     ( pt.TVD     () + frac * deltaTVD     );
      return true;
  }
  }

  // should not happen
  point.Northing( 0);
  point.Easting( 0);
  point.TVD( 0);
  return false;
}

bool CNewWellPath::CanZoomIn() const
{
  if(m_pZoomInModel != 0)
  return false;

  return true; //const_cast<CNewWellPath *>(this)->FormIntersecPoints().size() >= 2;
}

const CWellZoomInModel& CNewWellPath::ZoomInModel() const
{
  assert(m_pZoomInModel);
  return *m_pZoomInModel;
}

CWellZoomInModel& CNewWellPath::ZoomInModel()
{
  assert(m_pZoomInModel);
  return *m_pZoomInModel;
}

bool CNewWellPath::hasZoomInModel() const
{
  return m_pZoomInModel != 0 ;
}

void CNewWellPath::CreateZoomInModel()
{
  if(!m_pZoomInModel)
  {
  if ( 1 ) //FormIntersecPoints().size() >= 2 )
  {
      CModelBase& modelBase = dynamic_cast <CModelBase&> (Model());
      m_pZoomInModel = static_cast<CWellZoomInModel *>(IModelLifetimeFacade::NewChild(WELL_ZOOMIN_MODEL, modelBase, *this, Model().Logger(), modelBase.getVersionManager()));
      modelBase.OnNewChildModel(*m_pZoomInModel, false);
      Modified();
  }
  }
}

void CNewWellPath::DestroyZoomInModel()
{
  if(m_pZoomInModel)
  {
  m_pZoomInModel->DestroyZoomInModel();
  Modified();
  }
}

namespace
{

  // sort formations based on their z-stacking in relation to 'pt'
  //
  class ZLess_FormationBase
  {
  public:
  ZLess_FormationBase(const geo::IPoint &pt)
  {
      m_pt= pt;
  }

  bool operator()(const CFormationBase* f1, const CFormationBase* f2) const
  {
      geo::CPoint ptTop1, ptTop2, ptDummy;
      f1->ThicknessAt(m_pt, ptTop1, ptDummy, geo::IParallelInitializationCallback::Sequential);
      f2->ThicknessAt(m_pt, ptTop2, ptDummy, geo::IParallelInitializationCallback::Sequential);

      return (ptTop1.Z() < ptTop2.Z());
  }
   private:
  geo::CPoint m_pt;
  };

} // anonymous namespace

void CNewWellPath::CalcGraphPoints()
{
  m_GraphPoints.clear();

  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());

  if(!pModel->Mesh().IsMesh()) return;

  const std::list<CNewWellPoint>& intersect = MeshIntersecPoints();

  if(intersect.size() < 2) return;

  // wjrx mantis 3756
  //
  // Initialization of 'bCurrentlyOutsideMesh'
  // -----------------------------------------
  // Determine if we start our well/mesh intersections inside or outside
  // the mesh. If the well crosses the mesh edge, one of the
  // attached elements for this CNewWellPoint will be null.
  //
  bool bCurrentlyOutsideMesh= false;
  for ( std::set<const geo::IElement *>::const_iterator
          it_elt= (*intersect.begin()).getElements().begin()
      ; it_elt!=(*intersect.begin()).getElements().end()
      && bCurrentlyOutsideMesh==false
      ; ++it_elt
      )
  {
  if ( *it_elt == 0 )
      bCurrentlyOutsideMesh= true;
  }

  std::list<CNewWellPoint>::const_iterator it_pt = intersect.begin();

  while(it_pt != intersect.end())
  {
  bool bCrossingMeshEdge= false;

  // wjrx mantis 3716
  //
  // For each mesh intersection point, visit its intersected elements and
  // find out to which formation that element belongs.
  // The formation/element is then stored in map_formation_element.
  // For each formation we store one element.
  //
  // This element will be assigned to the mesh intersection point as the 
  // element that will determine in which formation the point resides further
  // down the line. The mesh intersection point will be duplicated for each
  // formation.
  //
  typedef std::map< const CFormationBase *, const geo::IElement *
      , ZLess_FormationBase> TFormationMap;
  ZLess_FormationBase zLess_FormationBase( *it_pt);
  TFormationMap map_formation_element(zLess_FormationBase);

  for ( std::set<const geo::IElement *>::const_iterator
      it_elt= (*it_pt).getElements().begin()
    ; it_elt!=(*it_pt).getElements().end()
    ; ++it_elt
    )
  {
      // wjrx mantis 3756
      // if the well enters or leaves the mesh,
      // one of the IElement* entries is
      // set to 0 by CNewWellPath::CalculateIntersections
      //
      if ( *it_elt == 0 )
      {
    bCrossingMeshEdge= true;
      }
      else
      {
    const CFormationBase *pEltFormation= pModel->Mesh().Formation(**it_elt);
    map_formation_element[pEltFormation]= *it_elt;
      }
  }

  // wjrx mantis 3716
  //
  // For each formation, copy the current point *it_pt and assign an element
  // that is within that formation.
  // Point *it_pt is duplicated into m_GraphPoints for each formation that
  // it is part of.


  if ( bCurrentlyOutsideMesh )
  {
      if ( bCrossingMeshEdge )
      {
    // wjrx mantis 3756
    // The well enters the mesh at point *it_pt.
    // Duplicate *pt_it with no assigned elements.
    //
    bCrossingMeshEdge= false;
    bCurrentlyOutsideMesh= false;
    CNewWellPoint p = *it_pt; // copy mesh intersection point
    std::set<const geo::IElement*> emptySet;
    p.setElements( emptySet); // no mesh elements, outside mesh
    m_GraphPoints.push_back(p);
      }
      else if (it_pt != intersect.begin()) // bCrossingMeshEdge == false
      {
    // wjrx mantis 3796
    // We are outside the mesh but the current mesh intersection
    // point does not cross the mesh edge back into the mesh
    // (bCrossingMeshEdge would be true then). This means that the
    // previous mesh intersection point (which moved the well outside
    // the mesh) just touched the mesh edge. For a touching intersection
    // point we remove the version with no elements in its set.
    //
    // The only exception is the first point intersect.begin() because
    // here a point on the mesh edge is meaningful.
    //
    assert( m_GraphPoints.back().getElements().empty() == true );
    m_GraphPoints.pop_back();
    bCurrentlyOutsideMesh= false;
      }
  }

  TFormationMap::const_iterator it_map;
  for ( it_map= map_formation_element.begin()
    ; it_map != map_formation_element.end(); ++it_map)
  {
      CNewWellPoint p = *it_pt; // copy mesh intersection point
      std::set<const geo::IElement*> emptySet;
      p.setElements( emptySet); // clear elements
      p.addElement( (*it_map).second ); // assign one element per point
      m_GraphPoints.push_back(p);
  }

  if ( !bCurrentlyOutsideMesh && bCrossingMeshEdge )
  {
      // wjrx mantis 3756
      // The well leaves the mesh at point *it_pt.
      // Duplicate *pt_it with no assigned elements.
      //
      bCrossingMeshEdge= false;
      bCurrentlyOutsideMesh= true;
      CNewWellPoint p = *it_pt; // copy mesh intersection point
      std::set<const geo::IElement*> emptySet;
      p.setElements( emptySet); // no mesh elements, outside mesh
      m_GraphPoints.push_back(p);
  }

  ++it_pt;
  }
}


std::list<CNewWellPoint> & CNewWellPath::GraphPoints()
{
  if ( m_GraphPoints.size() == 0 ) CalcGraphPoints();
  return m_GraphPoints;
}

const std::list<CNewWellPoint> & CNewWellPath::GraphPoints() const
{
  return const_cast<CNewWellPath*>(this)->GraphPoints();
}


int CNewWellPath::MapValuesSize() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const std::list<CNewWellPoint>& ptList = const_cast<CNewWellPath&>(*this).MeshIntersecPoints();

  int size = 0;

  for (std::list<CNewWellPoint>::const_iterator itPoint = ptList.begin(); itPoint != ptList.end(); ++itPoint)
  {
  const CNewWellPoint& p = *itPoint;

  if (p.getElements().empty())
  {
      ++size;
  }
  else
  {
      typedef std::vector<const geo::IElement *> TElements;
      typedef std::map<const CFormationBase *, TElements> TFormationElementsMap;
      TFormationElementsMap mpFormationElements;

      for (std::set<const geo::IElement *>::iterator itElt = p.getElements().begin(); itElt != p.getElements().end(); ++itElt)
      {
    const geo::IElement *pElement = *itElt;

    if (!pElement) // apparently sometimes we have a null pointer included (TODO: check out why)
          continue;

    const CFormationBase *pFormation = model.Mesh().Formation(*pElement);
    TFormationElementsMap::iterator retIt = mpFormationElements.insert(std::make_pair(pFormation, TElements())).first;
    retIt->second.push_back(pElement);
      }

      size += mpFormationElements.size();
  }
  }

  return size;
}


// Map result values onto the mesh intersection points, without averaging across formations
void CNewWellPath::MapValues(const IValueComponentBase& component, CUnitNode::TUnitType unit, TPointValueVector& vcPointValues)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const std::list<CNewWellPoint>& ptList = MeshIntersecPoints();

  // we create a vector with a pair of formation, point-value list
  // formation can be 0 if the point is outside the mesh

  typedef std::pair<const CFormationBase *, TPointValueVector> TFormationPointValueVector;
  typedef std::vector<TFormationPointValueVector> TFormationPointValueVectorVector;

  TFormationPointValueVectorVector intermediate;

  bool bDoPointCalculation = dynamic_cast<const IStressTensorGroup::CWellPathStressComposite::CWellPathStressComponent *>(&component)
  || dynamic_cast<const IStrainTensorGroup::CWellPathAxialStrainComposite::CWellPathAxialStrainComponent *>(&component);

  for (std::list<CNewWellPoint>::const_iterator itPoint = ptList.begin(); itPoint != ptList.end(); ++itPoint)
  {
  geo::CValue value;

  const CNewWellPoint& p = *itPoint;

  if (p.getElements().empty())
  {
      TPointValueVector vc; vc.push_back(std::make_pair(p, value));
      intermediate.push_back(std::make_pair(static_cast<const CFormationBase *>(0), vc));
  }
  else
  {
      // Find out in how many formations this point lies (should be one or two)
      typedef std::vector<const geo::IElement *> TElements;
      typedef std::map<const CFormationBase *, TElements> TFormationElementsMap;
      TFormationElementsMap mpFormationElements;

      for (std::set<const geo::IElement *>::iterator itElt = p.getElements().begin(); itElt != p.getElements().end(); ++itElt)
      {
    const geo::IElement *pElement = *itElt;

    if (!pElement) // apparently sometimes we have a null pointer included (TODO: check out why)
          continue;

    const CFormationBase *pFormation = model.Mesh().Formation(*pElement);
    TFormationElementsMap::iterator retIt = mpFormationElements.insert(std::make_pair(pFormation, TElements())).first;
    retIt->second.push_back(pElement);
      }

      for (TFormationElementsMap::iterator itForm = mpFormationElements.begin(); itForm != mpFormationElements.end(); ++itForm)
      {
    if (bDoPointCalculation)
    {
          value = component.ScalarData().ValuePoint(p, unit);
    }
    else
    {
          // calc values (averaging here, as it is within one formation)
          int nElements = 0;
          value = 0;
          for (TElements::iterator itElt = itForm->second.begin(); itElt != itForm->second.end(); ++itElt)
          {
      IValueDomainScalar::TValueVec values = component.ScalarData().ValueElement(**itElt, unit);
      value += (*itElt)->InterpolateValue(p, values);

      ++nElements;
          }
          if (nElements > 0)
      value /= nElements;
    }

    // add to existing list of points,
    bool added = false;

    CNewWellPoint newP(p);
    newP.Formation(const_cast<CFormationBase *>(itForm->first));

    for (int i = int(intermediate.size()) - 1; i >= 0; --i)
    {
          if (intermediate[i].first == itForm->first)
          {
      intermediate[i].second.push_back(std::make_pair(newP, value));
      added = true;
          }
    }
    // or create new
    if (!added)
    {
          TPointValueVector vc; vc.push_back(std::make_pair(newP, value));
          intermediate.push_back(std::make_pair(itForm->first, vc));
    }
      }
  }
  }


  vcPointValues.reserve(ptList.size());
  for (int i = 0; i < intermediate.size(); ++i)
  {
  for (int j = 0; j < intermediate[i].second.size(); ++j)
  {
      vcPointValues.push_back(intermediate[i].second[j]);
  }
  }
}


std::list<const QDate *> &CNewWellPath::FistDates()
{
  return m_FistDates;
}

const std::list<const QDate *> &CNewWellPath::FistDates() const
{
  return m_FistDates;
}

bool CNewWellPath::CanExportToFist() const
{
  const CModelBase &model = (const CModelBase&)(Model());
  if(!model.IsResult())
          return false;
  return ! FistExportPoints().empty();
}

std::list<CNewWellPoint> & CNewWellPath::FistExportPoints()
{
  return m_FistExportPoints;
}

const std::list<CNewWellPoint> & CNewWellPath::FistExportPoints() const
{
  return const_cast<CNewWellPath*>(this)->FistExportPoints();
}

void CNewWellPath::MudGradRefDepth(double newval)
{
  m_dMudGradRefDepth=newval;
}

double CNewWellPath::MudGradRefDepth() const
{
  return m_dMudGradRefDepth;
}

// CNewWellPathEntry implementation ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CNewWellPathEntry::CNewWellPathEntry(C3DModel &model) 
: BASE_CLASS(MD_NEW_WELLPATH, IDI_WELLPATH, getStringTableEntry(IDS_TREE_NEW_WELLPATHS), model)
, m_bDrawWellInfo(true)
{
}

CNewWellPathEntry::~CNewWellPathEntry() 
{
/*
while (EntryNodes().begin() != EntryNodes().end())
{
(*EntryNodes().begin())->destroy(); // Normally things work with delete
}
*/
}

bool CNewWellPathEntry::CanExport() const
{
  return !EntryNodes().empty();
}

// wjrx mantis 3322
void CNewWellPathEntry::ToggleDrawWellInfo()
{
  DrawWellInfo(!DrawWellInfo());
}

// wjrx mantis 3322
void CNewWellPathEntry::DrawWellInfo(bool bDraw)
{
  if(bDraw != m_bDrawWellInfo)
  {
  m_bDrawWellInfo= bDraw;
  CNewWellPathEntry::TNodeSet stWellPaths = EntryNodes();
  CNewWellPathEntry::TNodeSet::iterator it;
  for(it = stWellPaths.begin(); it != stWellPaths.end(); ++it)
      (*it)->DrawWellInfo(bDraw);
  }
}

// wjrx mantis 3322
bool CNewWellPathEntry::DrawWellInfo() const
{
  return m_bDrawWellInfo;
}


bool CNewWellPathEntry::CanInsertNew() const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
  return pModel && pModel->CanInsertNewWellPath();
}

bool CNewWellPathEntry::ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model, const CQuantity::UNIT& unit)
{
  QString sPath,sFile;
  SplitPathAndFileName(strFileName,sPath,sFile);
  std::auto_ptr <IProgressBase> prog( _g->prog()->create(eProgress::Geo, "") );

  std::vector<std::string> messages;

  if(sFile.right(4).compare(".MWR", Qt::CaseInsensitive) == 0) {
  CNewWellPathInput *input = new CNewWellPathInput(strFileName, sFile.left(sFile.length() - 4), model, unit, dlg);

  /*const CNewWellPath *pWellPath =*/ CNewWellPathInput::CreateWellPath(input, model, &messages);

  if (messages.size())
      _m()->msg(messages);

  } else if((sFile.right(4).compare(".DAT", Qt::CaseInsensitive) == 0) ||
          (sFile.right(4).compare(".TXT", Qt::CaseInsensitive) == 0)) {
    // Load plain txt file
  CNewWellPathInput *input = new CNewWellPathInput(sFile, model); 
    
    CWellPathInputImportFormat format(input);
    CElementSetFile file( format );
    if(!file.Open(strFileName, *prog)) {
      input->Destroy();
      _m()->msg(file.ErrorMessage());
    }
  else
  {
      /*const CNewWellPath *pWellPath =*/ CNewWellPathInput::CreateWellPath(input, model, &messages);

      if (messages.size())
    _m()->msg(messages);
  }
  } else if(sFile.right(4).compare(".XLS", Qt::CaseInsensitive) == 0 || sFile.right(5).left(4).compare(".XLS", Qt::CaseInsensitive) == 0) {
    CElementSetExcelFile file( (CModelBase&)model, true );
    CGraphNode::TNodeVec nodeVec;
    file.ReadSheets(strFileName.toStdString().c_str(), dlg, nodeVec);
  } else if(sFile.right(3).compare(".MX", Qt::CaseInsensitive) == 0) {

    // Load GoCad File
    CGoCadWellFile file;
    if(file.Open(strFileName, *prog)) {

      for(int k=0; k < file.wellSize();k++)
      {
        QString wellName = file.wellName(k);

        CNewWellPathInput* input = new CNewWellPathInput(wellName, model);
        int size = file.pointSize(k);

        QVector<geo::CPoint> vcPoint(size);

        for(int i=0 ; i< size;i++)
        {
          vcPoint[i]=file.pointAt(k,i);
        }
        input->InitFromPointArray(vcPoint);

    /*const CNewWellPath *pWellPath =*/ CNewWellPathInput::CreateWellPath(input, model, &messages);

    if (messages.size())
          _m()->msg(messages);
      }
      
    } else {
      _m()->msg(file.ErrorMessage());
    }
  } else {

  }

  return true;
}

void CNewWellPathEntry::addPointSet2DataStorage(const CGraphNode::TNodeVec& nodeVec, CModelBase &model)
{
  if (model.parentModel())
  return;

  for (CGraphNode::TNodeVec::const_iterator node = nodeVec.begin();
  node != nodeVec.end(); ++node)
  {
  if (!*node)
      continue;

  CNewWellPath* pWellPath = static_cast <CNewWellPath*> (*node);

  // Add the wellpath data as pointset to the datastorage
  // The wellpath data and the pointset are separate data!
  //

  // Create pointset
  CPointSet *ps=
      new CPointSet (pWellPath->Name(), model, 0, CPointSet::DIM_3D);

  // Fill pointset with wellpath points
  std::vector<double> empty;
  for (unsigned int ii= 0; ii<pWellPath->WellPointSize(); ++ii)
      ps->PushBack( pWellPath->WellPoint(ii), empty);

  // Assign pointset to datastorage
  ps->pointSetType(IPointSet::WELLPATH);  //waij TFS 92430
  ps->reParent(model.GraphEntry(MD_BASE_POINTSET));
  }
}

bool CNewWellPathEntry::ImportFile(const QString &strFileName, IProgressBase &dlg, CModelBase &model)
{
  CGraphNode::TNodeVec nodeVec;
  std::vector<std::string> messages;

  try
  {
  QString sPath,sFile;
  SplitPathAndFileName(strFileName,sPath,sFile);
  if((sFile.right(4).compare(".DAT", Qt::CaseInsensitive) == 0) ||
      (sFile.right(4).compare(".TXT", Qt::CaseInsensitive) == 0))
  {
      // Load plain txt file
      CNewWellPathInput* input = new CNewWellPathInput(sFile, model); 

      CWellPathInputImportFormat format(input);
      CElementSetFile file( format );
      std::auto_ptr <IProgressBase> prog(_g->prog()->create(eProgress::Geo, ""));
      if(!file.Open(strFileName, *prog))
      {
    input->Destroy();
    _m()->msg(file.ErrorMessage());
    return false;
      }
      
      const CNewWellPath *pWellPath = CNewWellPathInput::CreateWellPath(input, model, &messages);

      if (pWellPath)
    nodeVec.push_back(const_cast<CNewWellPath *>(pWellPath));
  }
  else if
  ( sFile.right(4).compare(".XLS", Qt::CaseInsensitive) == 0
  || sFile.right(5).left(4).compare(".XLS", Qt::CaseInsensitive) == 0)
  {
      CElementSetExcelFile file( (CModelBase&)model, true );
      file.ReadSheets(strFileName.toStdString().c_str(), dlg, nodeVec);

      for (size_t i = 0; i < nodeVec.size(); ++i)
      {
    const CNewWellPath *pWellPath = 0;
    if (dynamic_cast<CNewWellPathInput *>(nodeVec[i]))
          pWellPath = CNewWellPathInput::CreateWellPath(static_cast<CNewWellPathInput *>(nodeVec[i]), model, &messages);

    nodeVec[i] = const_cast<CNewWellPath *>(pWellPath);
      }
  }
  }
  catch (CProgressCancel *p)
  {
  delete p;
  for (CGraphNode::TNodeVec::iterator node = nodeVec.begin();
      node != nodeVec.end(); ++node)
  {
      delete *node;
  }
  return false;
  }

  addPointSet2DataStorage(nodeVec, model);

  if (messages.size() > 0)
  _m()->msg(messages);

  return true;
}

bool CNewWellPathEntry::ImportPetrelFile(const QString &strFileName,
  IProgressBase &dlg, CModelBase &model, const CQuantity::UNIT& lateralUnit,
  const CQuantity::UNIT& depthUnit)
{
  CGraphNode::TNodeVec nodeVec;
  QString sPath,sFile;
  SplitPathAndFileName(strFileName,sPath,sFile);

  // Load plain txt file
  CNewWellPathInput *input = new CNewWellPathInput(sFile, model);
  CImportPetrel PetrelImporter(strFileName, input, dlg, lateralUnit, depthUnit);

  if ( ! PetrelImporter.Import() )
  {
  input->Destroy();
  _m()->msg(PetrelImporter.Message());
  return false;
  }
  else if ( PetrelImporter.Warning() )
  {
  _m()->msg(PetrelImporter.Message());
  }

  std::vector<std::string> messages;
  const CNewWellPath *pWellPath = CNewWellPathInput::CreateWellPath(input, model, &messages);

  if (pWellPath)
  {
  nodeVec.push_back(const_cast<CNewWellPath *>(pWellPath));
  addPointSet2DataStorage(nodeVec, model);
  }

  return true;
}

bool CNewWellPathEntry::CanExportToFist() const
{
  CNewWellPathEntry::TNodeSet stWellPaths = EntryNodes();
  CNewWellPathEntry::TNodeSet::iterator it;
  for(it = stWellPaths.begin(); it != stWellPaths.end(); ++it)
  if ( (*it)->CanExportToFist() ) return true;
  return false;
}

// Drag and drop properties; waij TFS 92430
bool CNewWellPathEntry::CanConnectItem(const CGraphNode& item) const
{
  if ( !CanInsertNew() )
  return false;

  const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&item);
  if( pPointSet && (pPointSet->pointSetType() == IPointSet::INPUT) )
  {
  return true;
  }

  const CNewWellPath *wellpath = dynamic_cast<const CNewWellPath *>(&item);
  if (wellpath)
  {
  if (wellpath->IsLinkedTo(*this) && !wellpath->Links<CGroupNode>().empty())
      return true;
  }

  return false;
}


bool CNewWellPathEntry::ConnectItem(const CGraphNode& item)
{
  assert(CanConnectItem(item));

  CFemAppModel *pModel = dynamic_cast<CFemAppModel*>(&Model());
  const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&item);
  const CNewWellPath *pWellPath = dynamic_cast<const CNewWellPath *>(&item);

  if (pPointSet)
  {
  assert(pPointSet && (pPointSet->pointSetType() == IPointSet::INPUT));

  const CNewWellPathInput *input = static_cast<const CNewWellPathInput *>(pPointSet);
  /*CNewWellPath *pWellPath =*/ new CNewWellPath(*input, *pModel);
  }
  else if (pWellPath)
  {
  CGroupNode::RemoveGroups(const_cast<CNewWellPath&>(*pWellPath));
  }

  return true;
}

const QString CNewWellPathEntry::DEFAULT_NEWWELL_NAME =
  QObject::tr("New Wellpath (ps) %1");
const size_t CNewWellPathEntry::DEFAULT_INITIAL_NUMBER = 1;

bool less(const QDate * first, const QDate * second)
{
  if ( first == 0 ) return false;
  if ( second == 0 ) return true;

  return *first < *second;
}

bool equal(const QDate* first, const QDate* second)
{
  return ::less(first,second)==false && ::less(second,first)==false;
}

void CNewWellPath::AddDefaultFistPoints
(std::list<CNewWellPoint>& FistPoints) const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());
  if(!pModel->IsMesh())
  {
  return;
  }

  std::auto_ptr <IProgressBase>
  wait(_g->prog()->create(eProgress::Wait));

  const std::list<CNewFormationSection>& sections = FormationSections();

  if(sections.empty())
  return;

  double delta = 0.1;
  const int NrOfPointsInFormation = 5; //number of points in formation

  std::list<CNewFormationSection>::const_iterator it= sections.begin();

  while(it != sections.end())
  {
  CNewFormationSection section = *it;

  CFormationBase::TFormationType formtype = section.Formation()->FormationType();
  if ( section.Formation()->Depleting() &&
  (formtype == CFormationBase::FT_SAND || formtype == CFormationBase::FT_SANDSTONE || formtype == CFormationBase::FT_NOTSPECIFIED) )
  {

      CNewGeoWellPoint above(*this,section.Top().TMD()+delta);
      CNewGeoWellPoint below(*this,section.Bottom().TMD()-delta);

      if ( ! above.EqualPointExist( FistPoints) )
    above.AddPoint(FistPoints);

      if( ! below.EqualPointExist( FistPoints) )
    below.AddPoint(FistPoints);


      double delta_tmd = (below.TMD() - above.TMD())/(NrOfPointsInFormation-1);
      for(int i=0 ; i<NrOfPointsInFormation-2 ;i++)
      {
    double tmd = above.TMD() + delta_tmd*(i+1);
    CNewGeoWellPoint point(*this,tmd);

    if( !point.EqualPointExist( FistPoints) )
          point.AddPoint(FistPoints);
      }
  }


  ++it;
  }
  FistPoints.unique();
}

void CNewWellPath::AddDefaultDstaborPoints
(std::list<CNewWellPoint>& DstaborPoints) const
{
  const CModelBase *pModel = dynamic_cast<const CModelBase*>(&Model());

  if(!pModel->IsMesh())
  {
  return;
  }

  std::auto_ptr <IProgressBase>
  wait(_g->prog()->create(eProgress::Wait));

  const std::list<CNewFormationSection>& sections = FormationSections();

  if (sections.empty())
  return;

  double delta = 0.1;
  const int NrOfPointsInFormation = 5; //number of points in formation

  std::list<CNewFormationSection>::const_iterator it= sections.begin();

  while(it != sections.end())
  {
  CNewFormationSection section = *it;

  CNewGeoWellPoint above(*this,section.Top().TMD()+delta);
  CNewGeoWellPoint below(*this,section.Bottom().TMD()-delta);

  if ( ! above.EqualPointExist( DstaborPoints) )
      above.AddPoint(DstaborPoints);

  if( ! below.EqualPointExist( DstaborPoints) )
      below.AddPoint(DstaborPoints);

  double delta_tmd = (below.TMD() - above.TMD())/(NrOfPointsInFormation-1);
  for(int i=0 ; i<NrOfPointsInFormation-2 ;i++)
  {
      double tmd = above.TMD() + delta_tmd*(i+1);
      CNewGeoWellPoint point(*this,tmd);

      if( !point.EqualPointExist( DstaborPoints) )
    point.AddPoint(DstaborPoints);
  }
  ++it;
  }

  DstaborPoints.unique();
}

void CNewWellPath::ImportLAS(const QString& fileName)
{
  CLogASCIIStandardFile f(*this);
  if(f.Read(fileName.toStdString().c_str()))
  {
  }
  else
  {
  _m()->msg(f.Error());
  }
}

bool CNewWellPath::CanImportLAS() const
{
  return true;
}

void CNewWellPath::UpdateDsbAnalysisPointsInScene()
{
  m_DsbPoints.RemoveAll();

  if(m_bShowDsbPointsInScene)
  {
  std::list<CNewWellPoint> &points= DStaborPoints(); 
  std::list<CNewWellPoint>::const_iterator itmp= points.begin();

  while(itmp != points.end())
  {
      m_DsbPoints.PushBack( *itmp );
      ++itmp;
  }
  }
}

void CNewWellPath::ShowDsbAnalysisPointsInScene(bool bShow)
{
  m_bShowDsbPointsInScene=bShow;
  UpdateDsbAnalysisPointsInScene();

  Modified();
}

bool CNewWellPath::ShowDsbAnalysisPointsInScene() const
{
  return m_bShowDsbPointsInScene;
}

CNewDstaborResultMap& CNewWellPath::DsbResults()
{
  return m_DsbResults;
}

const CNewDstaborResultMap& CNewWellPath::DsbResults() const
{
  return m_DsbResults;
}

bool CNewWellPath::DsbCanCalculate(const CFormationBase& formation) const
{
  if(formation.FormationType() == CFormationBase::FT_SHALE        || 
     formation.FormationType() == CFormationBase::FT_NOTSPECIFIED || //mantis 2541 wjrx
     formation.FormationType() == CFormationBase::FT_LIMESTONE)      //mantis 2618 wedx
      return true;

  return false;
}

const geo::CPolyLine& CNewWellPath::PolyLine() const
{
  return m_WellPathPolyLine;
}

void CNewWellPath::GetIntersectionPointsWithPlane(geo::CPtrArray<CNewGeoWellPoint>& /*points*/,const geo::CPlane& /*plane*/) const
{
  //TODO htg// 
}

// 
// ---------------------------------------------------------------------------
//
CNewGeoWellPoint::CNewGeoWellPoint
( const CNewWellPath& wellpath
, const double& tmd
)
: CNewWellPoint(wellpath,tmd)
, m_bInit(false)
, m_model(dynamic_cast<const CModelBase*>(&wellpath.Model()))
{
}

CNewGeoWellPoint::CNewGeoWellPoint(const CNewGeoWellPoint& rhs)
: CNewWellPoint(rhs)
, m_bInit(rhs.m_bInit)
, m_model(rhs.m_model)
{
}

CNewGeoWellPoint& CNewGeoWellPoint::operator=(const CNewGeoWellPoint& rhs)
{
  CNewWellPoint::operator=(rhs);
  m_model = rhs.m_model;
  m_bInit=rhs.m_bInit;
  return *this;
}

void CNewGeoWellPoint::InitPoint() const
{
  m_bInit = true;
  m_pFormation = 0;

  // Get the point from the mesh
  std::vector<int> vcElement = m_model->Mesh().Mesh().ElementsAt(*this);
  for(size_t i = 0; i < vcElement.size(); i++)
  {
  const geo::IElement& element = m_model->Mesh().Mesh().Element(vcElement[i]);
  if(dynamic_cast<const geo::IBody*>(&element))
  {
      if(element.Contains(*this,true))
      {
    addElement(&element);
    m_pFormation = const_cast<CFormationBase*>(m_model->Mesh().Formation(element));
      }
  }
  }
}

CNewGeoWellPoint::CNewGeoWellPoint
( CModelBase& model
, const CNewWellPoint& wellpoint
)
: CNewWellPoint(wellpoint)
, m_bInit(false)
, m_model(&model)
{
}

/*virtual*/ bool CNewGeoWellPoint::Defined() const
{
  if(!Formation())
  return false;

  return CNewWellPoint::Defined();
}

/*virtual*/ void CNewGeoWellPoint::Invalidate()
{
  m_bInit=false;
  m_pFormation=0;
}

const CFormationBase* CNewGeoWellPoint::Formation() const
{
  if(!m_bInit)
  InitPoint();

  return m_pFormation;
}


CNewGeoWellPoint::~CNewGeoWellPoint()
{
}

//
// ----------------------------------------------------------------------------
//
CNewFormationSection::CNewFormationSection
( CNewWellPathBase& wellPath 
, const CFormationBase *pFormation
, const double &topTMD
, const double &bottomTMD
)
: INewWellSection ( wellPath )
, m_pFormation( pFormation)
, m_Top(wellPath, topTMD)
, m_Bottom(wellPath, bottomTMD)
{
}

//
// ----------------------------------------------------------------------------
//
size_t CAngleInfoVector::Size() const
{
  return m_AziInc.size();
}

CAngleQuantity CAngleInfoVector::Azimuth(size_t index) const
{
  if ( index < m_AziInc.size() )
  return m_AziInc[index].first;
  return CAngleQuantity();
}

CAngleQuantity CAngleInfoVector::Inclination(size_t index) const
{
  if ( index < m_AziInc.size() )
  return m_AziInc[index].second;
  return CAngleQuantity();
}

void CAngleInfoVector::PushBack(const double &azi, const double &inc)
{
  m_AziInc.push_back( std::make_pair(azi, inc) );
}
void CAngleInfoVector::Clear()
{
  m_AziInc.clear();
}

//******************************  CResultMap **********************************
CNewWellPointResultKey::CNewWellPointResultKey
( const CNewWellPoint& point
, const CDepletionStage& stage
, const CAnalysisType& antype
)
: m_point(point), m_stage(stage), m_antype(antype)
{
}

bool CNewWellPointResultKey::operator<( const CNewWellPointResultKey& rhs) const
{
  if(m_point > rhs.m_point)
  return false;

  if(m_point < rhs.m_point)
  return true;

  if(&m_stage > &rhs.m_stage)
  return false;

  if(&m_stage < &rhs.m_stage)
  return true;

  if(m_antype > rhs.m_antype)
  return false;

  if(m_antype < rhs.m_antype)
  return true;

  //equal point
  return false;
}

