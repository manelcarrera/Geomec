// FistExport.cpp: implementation of the CFistExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "FistExport.h"
#include "modelbase.h"
#include "NewWellPath.h"
#include "FormationBase.h"
#include "BaseEntryTypes.h"
#include "materialserver.h"
#include "ValueTypes.h"
#include "ResultTree.h"
#include "ResultRegister.h"

// this function converts a date into a double: yyyy.ddd in which yyyy is the year and ddd is the 
// nr of the day, so 31st of januari 2004 will be 2004.031. 31st december 2004 is 2004.366 since 2004
// is a leap year.
static double ConvertDate(const QDate &date)
{
  double yr = date.year();
  QDate dummy(date.year() - 1, 12, 31); // 31st december in the year before the year of "date".
  double days = dummy.daysTo(date);

  return yr + (days / 1000.0);
}


/////

CFistExport::CLayer::CLayer(CFistExport& parent,
              QDomElement xmlparent,
              const well::CWellPoint& wellpoint,
              double dStartAHD,
              double dEndAHD,
              const CModelBase& model,
              TFormationNameMap& mpFormationNames)
: m_parent(parent),
  m_wellpoint(wellpoint),
  m_pFormation(0),
  m_pElement(0),
  m_dStartAHD(dStartAHD),
  m_dEndAHD(dEndAHD),
  m_model(model),
  m_mpFormationNames(mpFormationNames)
{
  FindFormationAndElement();
  CreateLayerDirectory(xmlparent);
}

const well::CWellPoint& CFistExport::CLayer::WellPoint() const
{
  return m_wellpoint;
}

const CFormationBase& CFistExport::CLayer::Formation() const
{
  assert(m_pFormation);
  return *m_pFormation;
}

double CFistExport::CLayer::StartAHD() const
{
  return m_dStartAHD;
}

double CFistExport::CLayer::EndAHD() const
{
  return m_dEndAHD;
}

const QString& CFistExport::CLayer::Name() const
{
  return m_strName;
}

void CFistExport::CLayer::FindFormationAndElement()
{
  std::vector<int> vcElementIndices = m_model.Mesh().Mesh().ElementsAt(m_wellpoint);
  assert(!vcElementIndices.empty());

  // pick arbitrary element if on an edge
  m_pElement = &m_model.Mesh().Mesh().Element(vcElementIndices[0]);
  m_pFormation = m_model.Mesh().Formation(*m_pElement);

  assert(m_pElement != 0);
  assert(m_pFormation != 0);
}

void CFistExport::CLayer::CreateLayerDirectory(QDomElement xmlparent)
{
  // generate the layer name
  TFormationNameMap::iterator itf = m_mpFormationNames.insert(TFormationNameMap::value_type(m_pFormation, 0)).first;
  m_strName = QString("%1 sublayer %2").arg(m_pFormation->Name()).arg(++(itf->second));

  m_parent.CurrentNode(xmlparent);
  m_parent.CurrentNode(m_parent.AddElement("LAYER"));
  m_parent.AddAttribute("NAME", m_strName);

  // get the material for the element the point resides in
  const CFFMaterial& mat = m_pFormation->ActiveMaterial().Material(*m_pElement);

  if(mat.IsParameter(IDT_VALUETYPE_DILATATION))
    m_parent.AddAttribute("DILATATION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_DILATATION));

  if(mat.IsParameter(IDT_VALUETYPE_FRICTION_ANGLE))
    m_parent.AddAttribute("FRICTION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE));
  else if(mat.IsParameter(IDT_VALUETYPE_INITFRICTION))
  m_parent.AddAttribute("FRICTION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_INITFRICTION));

  if(mat.IsParameter(IDT_VALUETYPE_COHESION))
    m_parent.AddAttribute("COHESIVE_STRENGTH", mat.ParameterValue(IDT_VALUETYPE_COHESION) * 1E6);
  if(mat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
    m_parent.AddAttribute("POISSONS_RATIO", mat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO));
  if(mat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
    m_parent.AddAttribute("YOUNG_MODULUS", mat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1E6);

  m_parent.AddAttribute("ENABLED", true);
}

//--- CNewLayer ---
//
CFistExport::CNewLayer::CNewLayer(CFistExport& parent,
              QDomElement xmlparent,
              const CNewWellPoint& wellpoint,
              double dStartAHD,
              double dEndAHD,
              const CModelBase& model,
              TFormationNameMap& mpFormationNames)
: m_parent(parent),
  m_wellpoint(wellpoint),
  m_pFormation(0),
  m_pElement(0),
  m_dStartAHD(dStartAHD),
  m_dEndAHD(dEndAHD),
  m_model(model),
  m_mpFormationNames(mpFormationNames)
{
  FindFormationAndElement();
  CreateLayerDirectory(xmlparent);
}

const CNewWellPoint& CFistExport::CNewLayer::WellPoint() const
{
  return m_wellpoint;
}

const CFormationBase& CFistExport::CNewLayer::Formation() const
{
  assert(m_pFormation);
  return *m_pFormation;
}

double CFistExport::CNewLayer::StartAHD() const
{
  return m_dStartAHD;
}

double CFistExport::CNewLayer::EndAHD() const
{
  return m_dEndAHD;
}

const QString& CFistExport::CNewLayer::Name() const
{
  return m_strName;
}

void CFistExport::CNewLayer::FindFormationAndElement()
{
  std::vector<int> vcElementIndices = m_model.Mesh().Mesh().ElementsAt(m_wellpoint);
  assert(!vcElementIndices.empty());

  // pick arbitrary element if on an edge
  m_pElement = &m_model.Mesh().Mesh().Element(vcElementIndices[0]);
  m_pFormation = m_model.Mesh().Formation(*m_pElement);

  assert(m_pElement != 0);
  assert(m_pFormation != 0);
}

void CFistExport::CNewLayer::CreateLayerDirectory(QDomElement xmlparent)
{
  // generate the layer name
  TFormationNameMap::iterator itf = m_mpFormationNames.insert(TFormationNameMap::value_type(m_pFormation, 0)).first;
  m_strName = QString("%1 sublayer %2").arg(m_pFormation->Name()).arg(++(itf->second));

  m_parent.CurrentNode(xmlparent);
  m_parent.CurrentNode(m_parent.AddElement("LAYER"));
  m_parent.AddAttribute("NAME", m_strName);

  // get the material for the element the point resides in
  const CFFMaterial& mat = m_pFormation->ActiveMaterial().Material(*m_pElement);

  if(mat.IsParameter(IDT_VALUETYPE_DILATATION))
    m_parent.AddAttribute("DILATATION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_DILATATION));

  if(mat.IsParameter(IDT_VALUETYPE_FRICTION_ANGLE))
    m_parent.AddAttribute("FRICTION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE));
  else if(mat.IsParameter(IDT_VALUETYPE_INITFRICTION))
  m_parent.AddAttribute("FRICTION_ANGLE", mat.ParameterValue(IDT_VALUETYPE_INITFRICTION));

  if(mat.IsParameter(IDT_VALUETYPE_COHESION))
    m_parent.AddAttribute("COHESIVE_STRENGTH", mat.ParameterValue(IDT_VALUETYPE_COHESION) * 1E6);
  if(mat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
    m_parent.AddAttribute("POISSONS_RATIO", mat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO));
  if(mat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
    m_parent.AddAttribute("YOUNG_MODULUS", mat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1E6);

  m_parent.AddAttribute("ENABLED", true);

  const CNewWellPath& wp = static_cast<const CNewWellPath&>(m_wellpoint.WellPath());
  std::list<const QDate *>::const_iterator it; 
  for ( it= wp.FistDates().begin()
      ; it!=wp.FistDates().end()
      ; ++it
      )
  {
  const QDate* pDate = *it;
  m_parent.CurrentNode(m_parent.AddElement("DEPLETION"));
  m_parent.AddAttribute("YEAR", ConvertDate(*pDate), 7);

  // get the pore pressure at the well point at the given date
  const CResultTree& result_tree = m_model.ResultTree();
  double dPressure =
      result_tree.PorePressure().ResultAtTime
      ( m_model.ResultRegister().NonLinear() 
    ? CAnalysisType::AT_NONLIN
    : CAnalysisType::AT_LINEAR
      , CGeomecTime(pDate->year(), pDate->month())
      , m_wellpoint).Value();

  m_parent.AddAttribute("RESERVOIR_PRESSURE", dPressure * 1e6);
  m_parent.AddAttribute("ENABLED", true);

  m_parent.CurrentNode(m_parent.CurrentNode().parentNode());
  }
}
//--- CNewLayer ---


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define NO_VALUE	-1e35

CFistExport::CFistExport
( const CModelBase *pModel
, const CNewWellPath *pNewWellPath
, bool bNewWellPaths
)
: CXMLBase("CASE_TREE")
, m_pModel(pModel)
, m_pNewWellPath(pNewWellPath)
, m_bNewWellPaths(bNewWellPaths)
{
  assert( !(m_pNewWellPath) ); // only one, max.

  if ( pNewWellPath )
  {
  m_NewWellList.push_back(pNewWellPath);
  }
  else if (m_bNewWellPaths) // no specific wellpath selected 
  {
  CNewWellPathEntry::TNodeSet well_paths = ((CNewWellPathEntry*)m_pModel->GraphEntry(MD_NEW_WELLPATH))->EntryNodes();
  for(CNewWellPathEntry::TNodeSet::iterator it = well_paths.begin(); it != well_paths.end(); it++)
  {
      if((*it)->FistExportPoints().empty()== false)
    m_NewWellList.push_back(*it);
  }
  }
  else
  {
  assert(false);
  }
  Init();
}

CFistExport::~CFistExport()
{

}

void CFistExport::InitForNewWellPath()
{
  CreateCaseDirectory();

  TFormationNameMap mpFormationNames;

  const CNewWellPath *pPath = 0;
  std::list<const CNewWellPath*>::iterator itl;
  for(itl = m_NewWellList.begin(); itl != m_NewWellList.end(); ++itl)
  {
  pPath = *itl;
  const std::list<CNewWellPoint> & lstPoints= pPath->FistExportPoints();

  typedef std::vector<CNewGeoWellPoint> TWellPointVec;
  typedef std::pair<const CNewFormationSection*, TWellPointVec> TFormationPointVecPair;
  typedef std::vector<TFormationPointVecPair> TFormationPointVecPairVec;

  TFormationPointVecPairVec vcFormationPoints;
  const CFormationBase* pLastFormation = 0;

  std::list<CNewWellPoint>::const_iterator it;
  for(it = lstPoints.begin(); it != lstPoints.end(); ++it)
  {
      const CNewWellPoint& wellpoint = *it;
      const CNewFormationSection* pFormationSection = pPath->FormationSection(wellpoint);

      if(pFormationSection->Formation() != pLastFormation)
      {
    vcFormationPoints.push_back(std::make_pair(pFormationSection, TWellPointVec()));
    pLastFormation = pFormationSection->Formation();
      }

      assert(!vcFormationPoints.empty());
      TWellPointVec& vcWellPoints = vcFormationPoints[vcFormationPoints.size() - 1].second;
      vcWellPoints.push_back
    ( CNewGeoWellPoint(*const_cast<CModelBase *>(m_pModel), wellpoint));
  }

  for(size_t i = 0; i < vcFormationPoints.size(); ++i)
  {
      const CNewFormationSection& formationSection = *vcFormationPoints[i].first;
      const TWellPointVec& vcWellPoints = vcFormationPoints[i].second;

      double dLayerStartAHD;
      double dLayerEndAHD;

      for(size_t j = 0; j < vcWellPoints.size(); ++j)
      {
    if(j == 0) // first point
          dLayerStartAHD = formationSection.Top().TMD();
    else
          dLayerStartAHD = (vcWellPoints[j].TMD() + vcWellPoints[j-1].TMD()) / 2.0;

    if(j == vcWellPoints.size() - 1) // last point
          dLayerEndAHD = formationSection.Bottom().TMD();
    else
          dLayerEndAHD = (vcWellPoints[j].TMD() + vcWellPoints[j+1].TMD()) / 2.0;

    // create a layer entry for this point
    m_mpNewLayers.insert
    ( TNewLayerMap::value_type
          ( vcWellPoints[j]
          , CNewLayer
      ( *this
      , Root()
      , vcWellPoints[j]
      , dLayerStartAHD
      , dLayerEndAHD
      , *m_pModel
      , mpFormationNames
      )
          )
    );
      }
  }

  CreateWellDirectory(pPath);
  }
}

void CFistExport::Init()
{
  if ( !m_NewWellList.empty() )
  InitForNewWellPath();
}

void CFistExport::CreateCaseDirectory()
{
  CurrentNode(Root());
  CurrentNode(AddElement("CASE")); // add the CASE element to the tree and make it the current node.
  const CDepletionStage &first = m_pModel->InitialDepletionStage();
  const CDepletionStage& last = m_pModel->LastDepletionStage();

  AddAttribute("START_YEAR", ConvertDate(QDate(first.Time().GetYear(), first.Time().GetMonth(), 1)), 7);
  AddAttribute("END_YEAR", ConvertDate(QDate(last.Time().GetYear(), last.Time().GetMonth(), 1)), 7);

  AddAttribute("PERMANENT_DATUM_LEVEL", QString("PDL_MSL"));
  AddAttribute("REFERENCE_LEVEL", QString("REFERENCE_LEVEL_PDL"));
}

void CFistExport::CreateWellDirectory(const CNewWellPath *pPath)
{
  const CResultTree &result_tree = m_pModel->ResultTree();
    const std::list<CNewWellPoint> &point_list= pPath->FistExportPoints();
    const std::list<CNewFormationSection> &form_list = pPath->FormationSections();

  CurrentNode(Root());
  CurrentNode(AddElement("WELL"));
  AddAttribute("NAME", QString(pPath->Name()));
  AddAttribute("USED_IN_CORRELATION", false);
  CurrentNode(AddElement("REFERENCE_DEPTH"));
  AddAttribute("DERRICK_FLOOR", -pPath->GlobalTVD());
  CurrentNode(CurrentNode().parentNode());
  CurrentNode(AddElement("DEVIATION_DATA"));
  AddAttribute("WELLHEAD_X", pPath->GlobalEasting());
  AddAttribute("WELLHEAD_Y", pPath->GlobalNorthing());
  AddAttribute("LEVEL", QString("REFERENCE_LEVEL_PDL"));

    for (int ii= 0; ii< pPath->WellPointSize(); ++ii)
    {
          CurrentNode(AddElement("DEVIATION"));
          AddAttribute("AHD", pPath->WellPoint(ii).TMD());
          if ( ii == 0 )
          {
      // as defined in mantis 3564
      //
      AddAttribute("INCLINATION", pPath->Inclination(0).Value());
      AddAttribute("AZIMUTH", pPath->Azimuth(0).Value());
          }
          else
          {
      AddAttribute("INCLINATION", pPath->Inclination(ii));
      AddAttribute("AZIMUTH", pPath->Azimuth(ii));
          }
          AddAttribute("ENABLED", true);
          CurrentNode(CurrentNode().parentNode());
    }
  
  CurrentNode(CurrentNode().parentNode()); // set to "WELL"
  CurrentNode(AddElement("FORMATION_DATA"));
  AddAttribute("LEVEL", QString("REFERENCE_LEVEL_PDL"));
////////////////////////////////////////////////////////////////////////////	
  // each point has its own layer
  std::list<CNewWellPoint>::const_iterator it_pt;
  for ( it_pt= point_list.begin()
      ; it_pt!=point_list.end()
      ; ++it_pt
      )
  {
  CNewWellPoint wellPoint = *it_pt;
  TNewLayerMap::iterator it = m_mpNewLayers.find(wellPoint);
  assert(it != m_mpNewLayers.end());
  const CNewLayer& layer = it->second;
  const CFormationBase& formation = layer.Formation();
  CFormationBase::TFormationType formtype = formation.FormationType();
  if(formation.Depleting() &&
       (formtype == CFormationBase::FT_SAND || formtype == CFormationBase::FT_SANDSTONE || formtype == CFormationBase::FT_NOTSPECIFIED))
  {
      CurrentNode(AddElement("FORMATION"));
      AddAttribute("LAYER_NAME", layer.Name());
      AddAttribute("TOP_AHD", layer.StartAHD());
      AddAttribute("BOTTOM_AHD", layer.EndAHD());
      AddAttribute("ENABLED", true);
      CurrentNode(CurrentNode().parentNode());
  }
  }
///////////////////////////////////////////////////////////////////////////////

  CurrentNode(CurrentNode().parentNode()); // set to "WELL"
  CurrentNode(AddElement("LOG_DATA"));
  AddAttribute("LEVEL", QString("REFERENCE_LEVEL_PDL"));

  it_pt = point_list.begin();
  while(it_pt != point_list.end())
  {
          const CNewWellPoint *pPoint = &*it_pt;
          CurrentNode(AddElement("LOG"));
          AddAttribute("AHD", pPoint->TMD());
          AddAttribute("ENABLED", true);

          CurrentNode(CurrentNode().parentNode());
          ++it_pt;
  }

  CurrentNode(CurrentNode().parentNode()); // set to "WELL"
  CurrentNode(AddElement("PRODUCING_INTERVAL_DATA"));
  AddAttribute("LEVEL", QString("REFERENCE_LEVEL_PDL"));

    int i;
    for(i = 0, it_pt = point_list.begin(); it_pt != point_list.end(); ++it_pt, ++i)
    {
          CurrentNode(AddElement("PRODUCING_INTERVAL"));
          QPair<double, double> interval = FindProducingInterval(i, pPath);
          AddAttribute("TOP_AHD", interval.first);
          AddAttribute("BOTTOM_AHD", interval.second);
          AddAttribute("STRESSES", QString("STRESS_FULL_3D_EVOLUTION"));
          AddAttribute("ENABLED", true);

          std::list<const QDate *>::const_iterator it_date;
          for ( it_date= pPath->FistDates().begin()
              ; it_date!=pPath->FistDates().end()
              ; ++it_date
              )
          {
      const QDate *pDate = *it_date;
      CurrentNode(AddElement("STRESS_HISTORY"));

      CStressTensor stress = TotalStress( &*it_pt, *pDate);

      AddAttribute("MIN_PRIN_STRESS_X", stress.PrincMinNorth() * 1e6);
      AddAttribute("MIN_PRIN_STRESS_Y", stress.PrincMinEast() * 1e6);
      AddAttribute("MIN_PRIN_STRESS_Z", stress.PrincMinDepth() * 1e6);
      AddAttribute("MIN_PRIN_STRESS_MAG", stress.PrincMinLength() * 1e6);
      AddAttribute("MED_PRIN_STRESS_X", stress.PrincIntermNorth() * 1e6);
      AddAttribute("MED_PRIN_STRESS_Y", stress.PrincIntermEast() * 1e6);
      AddAttribute("MED_PRIN_STRESS_Z", stress.PrincIntermDepth() * 1e6);
      AddAttribute("MED_PRIN_STRESS_MAG", stress.PrincIntermLength() * 1e6);
      AddAttribute("MAX_PRIN_STRESS_X", stress.PrincMaxNorth() * 1e6);
      AddAttribute("MAX_PRIN_STRESS_Y", stress.PrincMaxEast() * 1e6);
      AddAttribute("MAX_PRIN_STRESS_Z", stress.PrincMaxDepth() * 1e6);
      AddAttribute("MAX_PRIN_STRESS_MAG", stress.PrincMaxLength() * 1e6);
      AddAttribute("YEAR", ConvertDate(*pDate), 7);
      AddAttribute("ENABLED", true);

      CurrentNode(CurrentNode().parentNode());
          }

          CurrentNode(CurrentNode().parentNode());
    }
}

// wjrx mantis 3564
QPair<double, double> 
CFistExport::FindProducingInterval(int nIndex, const CNewWellPath *pPath) const
{
  const std::list<CNewWellPoint> &point_list= pPath->FistExportPoints();

  TNewLayerMap::const_iterator it2 = m_mpNewLayers.begin();

  std::list<CNewWellPoint>::const_iterator it;
  int ii;
  for ( ii= 0, it= point_list.begin()
      ; it!=point_list.end()
      ; ++ii, ++it
      )
  {
  if ( ii==nIndex)
  {
      const CNewWellPoint& wellpoint = *it;
      TNewLayerMap::const_iterator it3 = m_mpNewLayers.find(wellpoint);
      assert(it3 != m_mpNewLayers.end());
      break;
  }
  }

  return QPair<double, double>(it2->second.StartAHD(), it2->second.EndAHD());
}

CStressTensor CFistExport::TotalStress(const geo::IPoint *pPoint, const QDate &date) const
{
  const CResultTree &result_tree = m_pModel->ResultTree();
  CAnalysisType antype(m_pModel->ResultRegister().NonLinear() ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR);
  double xx = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_XX).Value();
  double yy = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_YY).Value();
  double zz = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_ZZ).Value();
  double xy = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_XY).Value();
  double yz = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_YZ).Value();
  double zx = result_tree.TotalStress().Components().ResultAtTime(antype,
                      CGeomecTime(date.year(), date.month()),
                      *pPoint,
                      CDoubleQuantity::SI_UNIT, ITensorGroup::CComponentComposite::TC_ZX).Value();

  return CStressTensor(xx, yy, zz, xy, yz, zx);
    
}
