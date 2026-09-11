#include "ResultExporter.h"

#include "PointSet.h"
#include "ModelBase.h"
#include "FormationBase.h"
#include "FormationPlane.h"
#include "3DHorizon.h"
#include "IProgressBase.h"
#include "CoordinateSet.h"
#include "WellCasingRTCI.h"
#include "HexaFormation.h"
#include "NonMeshedSurface.h"
#include "WellPathCenterPointSet.h"

#include "Global.h"
#include "IProgressFactory.h"

///// IResultExporter
IResultExporter::IResultExporter(CModelBase& model)
: m_model(model)
{
}

IResultExporter::~IResultExporter()
{
}

void IResultExporter::Export2LAS(const QString& /*sFileName*/,
                                 const IExportFormat::TDataVec& /*vcData*/,
                                 const IExportFormat::CExportArg /*arg*/) const
{
}

CModelBase& IResultExporter::Model() const
{
  return m_model;
}

// protected

template <typename TExportFormat>
  bool IResultExporter::exportExcel(TExportFormat& exportFormat,
  const QString& sFileName, bool bAppend) const
{
  CElementSetExcelFile file(exportFormat);
  std::auto_ptr <IProgressBase> prog;

  try
  {
  prog.reset(_g->prog()->create(eProgress::Geo, sFileName));
  file.Write(sFileName, *prog, bAppend);
  }

  catch (CProgressCancel* p)
  {
  delete p;
  return true;
  }

  return false;
}

template <typename TExportFormat>
  bool IResultExporter::exportASCII(TExportFormat& exportFormat,
  const QString& sFileName, bool bAppend) const
{
  CElementSetFile file(exportFormat);
  std::auto_ptr <IProgressBase> prog;

  try
  {
  prog.reset(_g->prog()->create(eProgress::Geo, sFileName));
  file.Save(sFileName, *prog, bAppend);
  }

  catch (CProgressCancel* p)
  {
  delete p;
  file.Close();
  return true;
  }

  return false;
}

///// CElementSetExporter
CElementSetExporter::CElementSetExporter(CModelBase& model, const std::vector<const IElementSet*>& vcElementset)
: IResultExporter(model),
  m_vcElementset(vcElementset)
{
}

void CElementSetExporter::ExportExcel(const QString& sFileName,
                                      const IExportFormat::TDataVec& vcData,
                                      const IExportFormat::CExportArg arg,
                                      bool bAppend) const
{
  for(size_t i = 0; i < m_vcElementset.size(); ++i)
  {
  CElementSetExportFormat format(*m_vcElementset[i], vcData, arg);

  if (exportExcel(format, sFileName, bAppend))
  {
      break;
  }

  bAppend = true;
  }
}

void CElementSetExporter::ExportASCII(const QString& sFileName,
                                      const IExportFormat::TDataVec& vcData,
                                      const IExportFormat::CExportArg arg,
                                      bool bAppend) const
{
  for(size_t i = 0; i < m_vcElementset.size(); ++i)
  {
  CElementSetExportFormat format(*m_vcElementset[i], vcData, arg);

  if (exportASCII(format, sFileName, bAppend))
  {
      break;
  }

  bAppend = true;
  }
}

const COpenGLNode* CElementSetExporter::GetFirstNode() const
{
  return (m_vcElementset.empty() ? 0 : m_vcElementset[0]);
}

///// CFormationExporter
CFormationExporter::CFormationExporter(CModelBase& model, const std::vector<const CFormationBase*>& vcFormation)
: IResultExporter(model),
  m_vcFormation(vcFormation)
{
}

void CFormationExporter::ExportExcel(const QString& sFileName,
                                     const IExportFormat::TDataVec& vcData,
                                     const IExportFormat::CExportArg arg,
                                     bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  WellPath::CWellPathCenterPointSet
      wellPathCenterPointSet(m_vcFormation, Model());
  CNodalExportFormat format(wellPathCenterPointSet, vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)
  {
  QSharedPointer <CPointSet> pps =
      QSharedPointer <CPointSet> (CreateFormationCenterPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcFormation, IExportFormat::TDataVec(), vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
}

void CFormationExporter::ExportASCII(const QString& sFileName,
                                     const IExportFormat::TDataVec& vcData,
                                     const IExportFormat::CExportArg arg,
                                     bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  WellPath::CWellPathCenterPointSet
      wellPathCenterPointSet(m_vcFormation, Model());
  CNodalExportFormat format(wellPathCenterPointSet, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)
  {
  QSharedPointer <CPointSet> pps =
      QSharedPointer <CPointSet> (CreateFormationCenterPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcFormation, IExportFormat::TDataVec(), vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
}

const COpenGLNode* CFormationExporter::GetFirstNode() const
{
  return (m_vcFormation.empty() ? 0 : m_vcFormation[0]);
}

CPointSet* CFormationExporter::CreateFormationCenterPointSet(const IExportFormat::TDataVec& /*vcData*/) const
{
  int dim = Model().Dimension();
  CPointSet* pps =
  new CPointSet("", Model(), 0, (IPointSet::DIMENSION)dim);

  size_t i;
  for(i = 0; i < m_vcFormation.size(); ++i)
  {
  const CFormationBase* pForm = m_vcFormation[i];
  int iES;
  for(iES = 0; iES < pForm->ElementSetSize(); ++iES)
  {
      int iElm;
      for(iElm = 0; iElm < pForm->ElementSet(iES).ElementSet().ElementSize(); ++iElm)
      {
    const geo::IElement& elm = pForm->ElementSet(iES).ElementSet().Element(iElm);
    geo::CElementPoint pt = elm.MidPoint();
    pps->PushBack(pt, std::vector<double>());
      }
  }
  }

  return pps;
}

///// CFormationPlaneExporter
CFormationPlaneExporter::CFormationPlaneExporter(CModelBase& model, const std::vector<const CFormationPlane*>& vcFormplane)
: IResultExporter(model),
  m_vcFormplane(sortFormationPlanes(vcFormplane))
{
}

void CFormationPlaneExporter::ExportExcel(const QString& sFileName,
                                          const IExportFormat::TDataVec& vcData,
                                          const IExportFormat::CExportArg arg,
                                          bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)
  {
  QSharedPointer <CPointSet> pps =
      QSharedPointer <CPointSet> (CreateFormationPlaneCenterPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcFormplane, vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  assert(false);
  }
}

void CFormationPlaneExporter::ExportASCII(const QString& sFileName,
                                          const IExportFormat::TDataVec& vcData,
                                          const IExportFormat::CExportArg arg,
                                          bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)  
  {
  QSharedPointer <CPointSet> pps =
      QSharedPointer <CPointSet> (CreateFormationPlaneCenterPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcFormplane, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  assert(false);
  }
}

const COpenGLNode* CFormationPlaneExporter::GetFirstNode() const
{
  return (m_vcFormplane.empty() ? 0 : m_vcFormplane[0]);
}

CPointSet* CFormationPlaneExporter::CreateFormationPlaneCenterPointSet(const IExportFormat::TDataVec& vcData) const
{
  int dim = Model().Dimension();
  CPointSet* pps = new CPointSet("", Model(), vcData.size(), (IPointSet::DIMENSION)dim);

  for (size_t jj= 0; jj < m_vcFormplane.size(); ++jj)
  {
  const CFormationPlane* pFormPlane = m_vcFormplane[jj];

  if(pFormPlane->NrOfFaces() <= 0)
      continue;

  for(int ii = 0; ii < pFormPlane->PlaneSurface().FaceSize(); ++ii)
  {
      const geo::CBodyQuadrilateral& quad= 
    dynamic_cast<const geo::CBodyQuadrilateral&>(pFormPlane->PlaneSurface().Face(ii));

      geo::CPoint midPt = quad.MidPoint();

      std::vector<geo::CValue> vcPointData(vcData.size() + dim);
      vcPointData[0] = midPt.X();
      vcPointData[1] = midPt.Y();
      if(dim == 3)
    vcPointData[2] = midPt.Z();

      for(size_t iData = 0; iData < vcData.size(); ++iData)
      {
    vcPointData[iData + dim] =
          quad.InterpolateValue(midPt, vcData[iData]->ScalarData().ValueElement(quad));
      }
      pps->PushBack(vcPointData);
  }
  }

  return pps;
}

namespace
{

bool sortFormationsComparison(const CFormationPlane* lhs,
  const CFormationPlane* rhs)
{
  return lhs->Formation().Less(rhs->Formation());
}

bool sortFormationPlanesComparison(const CFormationPlane* lhs,
  const CFormationPlane* rhs)
{
  return lhs->Less(*rhs);
}

} // anonymous namespace

std::vector <const CFormationPlane*> CFormationPlaneExporter::
  sortFormationPlanes(
  const std::vector <const CFormationPlane*>& formationPlanes)
{
  std::vector <const CFormationPlane*> sortedFormationPlanes(formationPlanes);

  std::sort(sortedFormationPlanes.begin(), sortedFormationPlanes.end(),
  sortFormationsComparison);

  std::vector <const CFormationPlane*> ::iterator
  begin = sortedFormationPlanes.begin(), end = sortedFormationPlanes.begin();
  std::vector <const CFormationPlane*> ::const_iterator
  start = sortedFormationPlanes.begin(), finish = sortedFormationPlanes.end();

  while ((start != finish) || ((start == finish) && (begin != end)))
  {
  if ((end == finish) ||
      ((*begin)->Formation().Index() != (*end)->Formation().Index()))
  {
      std::sort(begin, end, sortFormationPlanesComparison);
      begin = end;
  }
  else
  {
      ++end;
  }

  if (start != finish)
  {
      ++start;
  }
  }

  return sortedFormationPlanes;
}

///// CHorizonExporter
CHorizonExporter::CHorizonExporter(CModelBase& model, const std::vector<const CHorizonBase*>& vcHorizon)
: IResultExporter(model),
  m_vcHorizon(vcHorizon)
{
}

void CHorizonExporter::ExportExcel(const QString& sFileName,
                                   const IExportFormat::TDataVec& vcData,
                                   const IExportFormat::CExportArg arg,
                                   bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)
  {
  QSharedPointer <CPointSet> pps =
  QSharedPointer <CPointSet> (CreateHorizonsPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportExcel(format, sFileName, bAppend);
}
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcHorizon, IExportFormat::TDataVec(), vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  assert(false);
  }
}

void CHorizonExporter::ExportASCII(const QString& sFileName,
                                   const IExportFormat::TDataVec& vcData,
                                   const IExportFormat::CExportArg arg,
                                   bool bAppend) const
{
  if(arg.m_elementCenterPoints == IExportFormat::CENTER_ELEMENT)
  {
  QSharedPointer <CPointSet> pps =
      QSharedPointer <CPointSet> (CreateHorizonsPointSet(vcData));
  CNodalExportFormat format(*pps, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::FULL_ELEMENT)
  {
  CElementSetExportFormat format(m_vcHorizon, IExportFormat::TDataVec(), vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
  else if(arg.m_elementCenterPoints == IExportFormat::CENTER_WELL)
  {
  assert(false);
  }
}

const COpenGLNode* CHorizonExporter::GetFirstNode() const
{
  return (m_vcHorizon.empty() ? 0 : m_vcHorizon[0]);
}

CPointSet* CHorizonExporter::CreateHorizonsPointSet(const IExportFormat::TDataVec& vcNodalData) const
{
  int dim = Model().Dimension();
  CPointSet* pps = new CPointSet("", Model(), vcNodalData.size(), (IPointSet::DIMENSION)dim);

  typedef std::map<int, std::vector<geo::CValue> > TNodalValuesMap;
  TNodalValuesMap mNodalValues;

  for(size_t i = 0; i < m_vcHorizon.size(); ++i)
  {
  const CHorizonBase* pHorizon = m_vcHorizon[i];
  if(pHorizon->Slip())
      continue; // we should not get these here.

  const C3DHorizon& hor3D = dynamic_cast<const C3DHorizon&>(*pHorizon);
  for(int e = 0; e < hor3D.AsElementSet().ElementSize(); ++e)
  {
      const geo::IElement& element= hor3D.AsElementSet().Element(e);
      for(int n = 0; n < element.NrOfPoints(); n++)
      {
    std::pair<TNodalValuesMap::iterator, bool> pr_insert =
          mNodalValues.insert(TNodalValuesMap::value_type(element.PointIndex(n),
      std::vector<geo::CValue>(vcNodalData.size())));

    if(pr_insert.second)
    {
          for(size_t ii = 0; ii < vcNodalData.size(); ++ii)
          {
      pr_insert.first->second[ii] = vcNodalData[ii]->ScalarData().ValuePoint(element.Point(n));
          }
    }
      }
  }
  }
  TNodalValuesMap::iterator it;
  for(it = mNodalValues.begin(); it != mNodalValues.end(); ++it)
  {
  std::vector<geo::CValue> vcPointData(vcNodalData.size() + dim);
  const geo::IPoint &pt = Model().Mesh().Mesh().Point(it->first);

  vcPointData[0] = pt.X();
  vcPointData[1] = pt.Y();
  if(dim == 3)
      vcPointData[2] = pt.Z();

  size_t iData;
  for(iData = 0; iData < it->second.size(); ++iData)
  {
      vcPointData[iData + dim] = it->second[iData];
  }
  pps->PushBack(vcPointData);
  }

  return pps;
}

///// CSlipHorizonExporter
CSlipHorizonExporter::CSlipHorizonExporter(CModelBase& model, const std::vector<const CHorizonBase*>& vcHorizon)
: IResultExporter(model),
  m_vcHorizon(vcHorizon)
{
}

void CSlipHorizonExporter::ExportExcel(const QString& sFileName,
                                       const IExportFormat::TDataVec& vcData,
                                       const IExportFormat::CExportArg arg,
                                       bool bAppend) const
{
  CElementSetExportFormat format(m_vcHorizon, IExportFormat::TDataVec(), vcData, arg);

  exportExcel(format, sFileName, bAppend);
}

void CSlipHorizonExporter::ExportASCII(const QString& sFileName,
                                       const IExportFormat::TDataVec& vcData,
                                       const IExportFormat::CExportArg arg,
                                       bool bAppend) const
{
  CElementSetExportFormat format(m_vcHorizon, IExportFormat::TDataVec(), vcData, arg);

  exportASCII(format, sFileName, bAppend);
}

const COpenGLNode* CSlipHorizonExporter::GetFirstNode() const
{
  return (m_vcHorizon.empty() ? 0 : m_vcHorizon[0]);
}

///// CPointSetExporter
CPointSetExporter::CPointSetExporter(CModelBase& model, const std::vector<const CPointSet*>& vcPointset)
: IResultExporter(model),
  m_vcPointset(vcPointset)
{
}

void CPointSetExporter::ExportExcel(const QString& sFileName,
                         const IExportFormat::TDataVec& vcData,
                         const IExportFormat::CExportArg arg,
                         bool bAppend) const
{
  QSharedPointer <CPointSet> pps =
  QSharedPointer <CPointSet> (CreateCombinedPointSet(vcData));

  if(pps)
  {
  CNodalExportFormat format(*pps, vcData, arg);

  exportExcel(format, sFileName, bAppend);
  }
}

void CPointSetExporter::ExportASCII(const QString& sFileName,
                         const IExportFormat::TDataVec& vcData,
                         const IExportFormat::CExportArg arg,
                         bool bAppend) const
{
  QSharedPointer <CPointSet> pps =
  QSharedPointer <CPointSet> (CreateCombinedPointSet(vcData));

  if(pps)
  {
  CNodalExportFormat format(*pps, vcData, arg);

  exportASCII(format, sFileName, bAppend);
  }
}

const COpenGLNode* CPointSetExporter::GetFirstNode() const
{
  return (m_vcPointset.empty() ? 0 : m_vcPointset[0]);
}

CPointSet* CPointSetExporter::CreateCombinedPointSet(const IExportFormat::TDataVec& /*vcNodalData*/) const
{
  unsigned int dim = 0;
  for(size_t ii = 0; ii < m_vcPointset.size(); ++ii)
  {
  if(dim < m_vcPointset[ii]->Dimension())
      dim = (unsigned int)m_vcPointset[ii]->Dimension();
  }

  CPointSet* pps = new CPointSet("", Model(), 0, (IPointSet::DIMENSION)dim, true, false);

  int nJobs = 1 + (m_vcPointset.size() > 1 ? 1 : 0);

  std::auto_ptr <IProgressBase> progdlg( _g->prog()->create( eProgress::Dual, "Preparing pointset for export", true, nJobs ));

  try
  {

  int nTotalSteps;
  int nCurrentStep;

  geo::CCoordinateSet<const geo::IPoint*> coordSet;
  if(m_vcPointset.size() > 1)
  {
      progdlg->NextJob("Merging pointsets");
      nTotalSteps = 0;
      for(size_t i = 0; i < m_vcPointset.size(); ++i)
    nTotalSteps += m_vcPointset[i]->PointSize();
      nCurrentStep = 0;
      progdlg->AddSteps(nTotalSteps);

      for(size_t ii = 0; ii < m_vcPointset.size(); ++ii)
      {
    for(int jj = 0; jj < m_vcPointset[ii]->PointSize(); ++jj)
    {
          coordSet.insert(&m_vcPointset[ii]->PointAt(jj));
          progdlg->Step();
          ++nCurrentStep;
    }
      }
  }

  progdlg->NextJob("Linking points to mesh elements");
  if(coordSet.empty())
      progdlg->StatusMessage("Linking points to mesh elements");
  nTotalSteps = (coordSet.empty() ? m_vcPointset[0]->PointSize() : coordSet.size());
  progdlg->AddSteps(nTotalSteps);
  nCurrentStep = 0;

  geo::CCoordinateSet<const geo::IPoint*>::iterator it;
  int iPoint;
  const geo::IPoint* pPoint = 0;
  if(coordSet.empty())
  {
      if(m_vcPointset[0]->PointSize() == 0)
      {
    delete pps;
    return 0;
      }
      iPoint = 0;
      pPoint = &m_vcPointset[0]->PointAt(iPoint);
  }
  else
  {
      iPoint = -1; // not used
      it = coordSet.begin();
      pPoint = *it;
  }

  while(pPoint)
  {
      const geo::CElementPoint* pElmPoint = dynamic_cast<const geo::CElementPoint*>(pPoint);
      if(pElmPoint)
      {
    pps->PushBack(*pElmPoint, std::vector<double>());
      }
      else
      {
    const geo::IElement* pElement = 0;
    const std::set<int>& stCandidates = Model().Mesh().Mesh().Candidates(*pPoint);
    std::set<int>::const_iterator itt;
    for(itt = stCandidates.begin(); itt != stCandidates.end(); ++itt)
    {
          if(Model().Mesh().Mesh().Element(*itt).Contains(*pPoint, true))
          {
      pElement = &Model().Mesh().Mesh().Element(*itt);
      break;
          }
    }

    if(pElement)
          pps->PushBack(geo::CElementPoint(*pElement, *pPoint), std::vector<double>());
    else
          pps->PushBack(*pPoint, std::vector<double>());
      }

      progdlg->Step();
      ++nCurrentStep;

      if(coordSet.empty())
      {
    if(++iPoint == m_vcPointset[0]->PointSize())
          pPoint = 0;
    else
          pPoint = &m_vcPointset[0]->PointAt(iPoint);
      }
      else
      {
    if(++it == coordSet.end())
          pPoint = 0;
    else
          pPoint = *it;
      }
  }

  }
  catch(CProgressCancel* c)
  {
  delete c;
  delete pps;
  pps = 0;
  }
  catch(CProgressCancel)
  {
  delete pps;
  pps = 0;
  }

  return pps;
}

///// CRTCIExporter
CRTCIExporter::CRTCIExporter(CModelBase& model, const std::vector<const CWellCasingRTCI*>& vcRTCI)
: IResultExporter(model),
  m_vcRTCI(vcRTCI)
{
}

void CRTCIExporter::ExportExcel(const QString& sFileName,
                const IExportFormat::TDataVec& vcData,
                const IExportFormat::CExportArg arg,
                bool bAppend) const
{
  CElementSetExportFormat format(m_vcRTCI, IExportFormat::TDataVec(), vcData, arg);

  exportExcel(format, sFileName, bAppend);
}

void CRTCIExporter::ExportASCII(const QString& sFileName,
                const IExportFormat::TDataVec& vcData,
                const IExportFormat::CExportArg arg,
                bool bAppend) const
{
  CElementSetExportFormat format(m_vcRTCI, IExportFormat::TDataVec(), vcData, arg);

  exportASCII(format, sFileName, bAppend);
}

const COpenGLNode* CRTCIExporter::GetFirstNode() const
{
  return (m_vcRTCI.empty() ? 0 : m_vcRTCI[0]);
}


///// CNonMeshedSurfaceExporter
CNonMeshedSurfaceExporter::CNonMeshedSurfaceExporter(CModelBase& model, const std::vector<const CNonMeshedSurface*>& vcSurface)
: IResultExporter(model),
  m_vcSurface(vcSurface)
{
}

void CNonMeshedSurfaceExporter::ExportExcel(const QString& sFileName,
                      const IExportFormat::TDataVec& vcData,
                      const IExportFormat::CExportArg arg,
                      bool bAppend) const
{
  CElementSetExportFormat format(m_vcSurface, IExportFormat::TDataVec(), vcData, arg);

  exportExcel(format, sFileName, bAppend);
}

void CNonMeshedSurfaceExporter::ExportASCII(const QString& sFileName,
                      const IExportFormat::TDataVec& vcData,
                      const IExportFormat::CExportArg arg,
                      bool bAppend) const
{
  CElementSetExportFormat format(m_vcSurface, IExportFormat::TDataVec(), vcData, arg);

  exportASCII(format, sFileName, bAppend);
}

const COpenGLNode* CNonMeshedSurfaceExporter::GetFirstNode() const
{
  return (m_vcSurface.empty() ? 0 : m_vcSurface[0]);
}


///// CExporterFactory
CExporterFactory::CExporterFactory(CModelBase& model)
: m_model(model)
{
}

// specialized template method bodies must go in the source file
// otherwise you get error LNK2005 (... already defined)
template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CHorizonBase*>& vcSelection)
{
  if(vcSelection[0]->Slip())
  return new CSlipHorizonExporter(m_model, vcSelection);

  return new CHorizonExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const IElementSet*>& vcSelection)
{
  return new CElementSetExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CFormationBase*>& vcSelection)
{
  return new CFormationExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CFormationPlane*>& vcSelection)
{
  return new CFormationPlaneExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CPointSet*>& vcSelection)
{
  return new CPointSetExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CWellCasingRTCI*>& vcSelection)
{
  return new CRTCIExporter(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CNewWellPath*>& vcSelection)
{
  return new CWellPathExporter<CNewWellPath>(m_model, vcSelection);
}

template<>
const IResultExporter* CExporterFactory::CreateTypedExporter(const std::vector<const CNonMeshedSurface*>& vcSelection)
{
  return new CNonMeshedSurfaceExporter(m_model, vcSelection);
}
