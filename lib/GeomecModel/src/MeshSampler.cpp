
#include "MeshSampler.h"
#include "HexaMesh.h"
#include "CoordinateSet.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "resourceIDI.h"
#include "IProgressFactory.h"
#include "IBody.h"
#include "colornode.h"

// Target point set source types

ITargetPointsetSource::ITargetPointsetSource()
{
}

ITargetPointsetSource::~ITargetPointsetSource()
{
}

const double* ITargetPointsetSource::MaximumDepth() const
{
  return 0;
}


// generate from corners and nr of points
CGridPointsetSource::CGridPointsetSource(const geo::IPoint& ptCorner1, const geo::IPoint& ptCorner2, int nPointsNorthing, int nPointsEasting)
: m_vcPoints(nPointsNorthing * nPointsEasting)
{
  int i;
  int j;
  int nDivN = nPointsNorthing - 1;
  if(nDivN < 1) nDivN = 1;
  int nDivE = nPointsEasting - 1;
  if(nDivE < 1) nDivE = 1;
  for(i = 0; i < nPointsNorthing; ++i)
  {
  for(j = 0; j < nPointsEasting; ++j)
  {
      m_vcPoints[j * nPointsNorthing + i] = geo::CPoint(ptCorner1.X() + i * (ptCorner2.X() - ptCorner1.X()) / nDivN,
                            ptCorner1.Y() + j * (ptCorner2.Y() - ptCorner1.Y()) / nDivE);
  }
  }
}

// generate from hexa mesh grid
CGridPointsetSource::CGridPointsetSource(const CHexaMesh& hexamesh)
{
  geo::CCoordinateSet<geo::CPoint> stPoints;

  int i;
  for(i = 0; i < hexamesh.Mesh().ElementSize(); ++i)
  {
  geo::CPoint pt = hexamesh.Mesh().Element(i).MidPoint();
  pt.Z(0);
  if(stPoints.insert(pt).second)
      m_vcPoints.push_back(pt);
  }
}

int CGridPointsetSource::PointSize() const
{
  return m_vcPoints.size();
}

const geo::IPoint& CGridPointsetSource::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcPoints.size());
  return m_vcPoints[nIndex];
}


CPointsetSource::CPointsetSource(const IPointSet& pointset)
: m_pointset(pointset),
  m_dMaxDepth(0),
  m_pMaxDepth(0)
{
}

CPointsetSource::CPointsetSource(const IPointSet& pointset, double dMaxDepth)
: m_pointset(pointset),
  m_dMaxDepth(dMaxDepth),
  m_pMaxDepth(&m_dMaxDepth)
{
}

int CPointsetSource::PointSize() const
{
  return m_pointset.PointSize();
}

const geo::IPoint& CPointsetSource::Point(int nIndex) const
{
  return m_pointset.PointAt(nIndex);
}

const double* CPointsetSource::MaximumDepth() const
{
  return m_pMaxDepth;
}


double CPillarPos::m_dZ = 0;

CPillarPos::CPillarPos(double dX, double dY)
: m_dX(dX),
  m_dY(dY)
{
}

CPillarPos::CPillarPos(const CPillarPos& rhs)
: m_dX(rhs.m_dX),
  m_dY(rhs.m_dY)
{
}

CPillarPos& CPillarPos::operator=(const CPillarPos& rhs)
{
  m_dX = rhs.m_dX;
  m_dY = rhs.m_dY;
  return *this;
}

const double &CPillarPos::X() const
{
  return m_dX;
}

const double &CPillarPos::Y() const
{
  return m_dY;
}

const double &CPillarPos::Z() const
{
  return m_dZ;
}

void CPillarPos::X(const double &/*dX*/)
{
  assert(false);
}

void CPillarPos::Y(const double &/*dY*/)
{
  assert(false);
}

void CPillarPos::Z(const double &/*dZ*/)
{
  assert(false);
}

void CPillarPos::AssertValid() const
{
}

bool CPillarPos::Empty() const
{
  return false;
}


CPillar::CPillarPoint::CPillarPoint(const CConstIterator& iter)
: m_dX(iter.m_pPillar->Position().X()),
  m_dY(iter.m_pPillar->Position().Y()),
  m_dZ(iter.m_it->first)
{
}

const double& CPillar::CPillarPoint::X() const
{
  return m_dX;
}

const double& CPillar::CPillarPoint::Y() const
{
  return m_dY;
}

const double& CPillar::CPillarPoint::Z() const
{
  return m_dZ;
}

void CPillar::CPillarPoint::X(const double& /*dX*/)
{
  assert(false);
}

void CPillar::CPillarPoint::Y(const double& /*dY*/)
{
  assert(false);
}

void CPillar::CPillarPoint::Z(const double& /*dZ*/)
{
  assert(false);
}

void CPillar::CPillarPoint::AssertValid() const
{
}

bool CPillar::CPillarPoint::Empty() const
{
  return false;
}


CPillar::CConstIterator::CConstIterator()
: m_pPillar(0)
{
}

CPillar::CConstIterator::CConstIterator(const CPillar& pillar, std::map<double, const geo::IBody*>::const_iterator it)
: m_pPillar(&pillar),
  m_it(it)
{
}

CPillar::CConstIterator::CConstIterator(const CConstIterator& rhs)
: m_pPillar(rhs.m_pPillar),
  m_it(rhs.m_it)
{
}

CPillar::CConstIterator& CPillar::CConstIterator::operator=(const CConstIterator& rhs)
{
  m_pPillar = rhs.m_pPillar;
  m_it = rhs.m_it;
  return *this;
}

bool CPillar::CConstIterator::operator==(const CConstIterator& rhs)
{
  assert(m_pPillar == rhs.m_pPillar);
  return (m_it == rhs.m_it);
}

bool CPillar::CConstIterator::operator!=(const CConstIterator& rhs)
{
  return !operator==(rhs);
}

CPillar::CConstIterator& CPillar::CConstIterator::operator++() // pre-increment
{
  assert(m_pPillar != 0);
  ++m_it;
  return *this;
}

CPillar::CConstIterator CPillar::CConstIterator::operator++(int) // post-increment
{
  CConstIterator iter(*this);
  ++(*this);
  return iter;
}

CPillar::CConstIterator& CPillar::CConstIterator::operator--() // pre-decrement
{
  assert(m_pPillar != 0);
  --m_it;
  return *this;
}

CPillar::CConstIterator CPillar::CConstIterator::operator--(int) // post-decrement
{
  CConstIterator iter(*this);
  --(*this);
  return iter;
}

std::pair<CPillar::CPillarPoint, const geo::IBody*> CPillar::CConstIterator::operator*()
{
  return std::make_pair(CPillarPoint(*this), m_it->second);;
}


CPillar::CPillar()
{
}

CPillar::CPillar(const CPillarPos& pos)
: m_dX(pos.X()),
  m_dY(pos.Y())
{
}

CPillarPos CPillar::Position() const
{
  return CPillarPos(m_dX, m_dY);
}

CPillar::const_iterator CPillar::begin() const
{
  return CConstIterator(*this, m_mpInterval.begin());
}

CPillar::const_iterator CPillar::end() const
{
  return CConstIterator(*this, m_mpInterval.end());
}

CPillar::size_type CPillar::size() const
{
  return m_mpInterval.size();
}

std::pair<CPillar::const_iterator, bool> CPillar::insert(double dZ)
{
  std::pair<TIntervalMap::iterator, bool> prInsert = m_mpInterval.insert(TIntervalMap::value_type(dZ, (const geo::IBody*)0));
  return std::make_pair(const_iterator(*this, prInsert.first), prInsert.second);
}

void CPillar::DetectElements(const geo::IMesh& mesh, bool bInclude)
{
  TIntervalMap::iterator it;
  geo::CPoint ptPrev;
  for(it = m_mpInterval.begin(); it != m_mpInterval.end(); ++it)
  {
  geo::CPoint pt(m_dX, m_dY, it->first);
  if(!ptPrev.Empty())
  {
      geo::CPoint ptMid(pt.X(), pt.Y(), (pt.Z() + ptPrev.Z()) / 2);
      std::set<int> stCandidates = mesh.Candidates(ptMid);
      std::set<int>::iterator its;
      for(its = stCandidates.begin(); its != stCandidates.end(); ++its)
      {
    const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&mesh.Element(*its));
    if(pBody && pBody->Contains(ptMid, true))
    {
          it->second = pBody;
          break;
    }
      }

      assert(it->second != 0);
  }
  else if (bInclude)
  {
      std::set<int> stCandidates = mesh.Candidates(pt);
      std::set<int>::iterator its;
      for (its = stCandidates.begin(); its != stCandidates.end(); ++its)
      {
    const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&mesh.Element(*its));
    if (pBody && pBody->Contains(pt, true))
    {
          it->second = pBody;
          break;
    }
      }

      assert(it->second != 0);
  }

  ptPrev = pt;
  }
}

long CPillar::SavedItems() const
{
  return m_mpInterval.size();
}

void CPillar::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress)
{
  stream << m_dX;
  stream << m_dY;

  stream << int(m_mpInterval.size());
  for(TIntervalMap::iterator it = m_mpInterval.begin(); it != m_mpInterval.end(); ++it)
  {
  stream << it->first;
  stream << int(it->second ? it->second->Index() : -1);
  progress.Step();
  }
}

void CPillar::LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& /*version*/, CStorageNode::TPROGRESS& progress, const geo::IMesh& mesh)
{
  stream >> m_dX;
  stream >> m_dY;

  int nIntervals;
  stream >> nIntervals;
  for(int i = 0; i < nIntervals; ++i)
  {
  double dInterval;
  stream >> dInterval;
  int nIndex;
  stream >> nIndex;
  const geo::IBody* pBody = (nIndex >= 0 ? static_cast<const geo::IBody*>(&mesh.Element(nIndex)) : 0);
  m_mpInterval.insert(TIntervalMap::value_type(dInterval, pBody));
  progress.Step();
  }
}


CPillarMap::CPillarMap(CFemAppModel& model)
: CColorNode(model)
{
}

CPillarMap::CPillarMap(const QString& strName, CModelBase& model)
: CColorNode(strName, model)
{
}

CPillarMap::~CPillarMap()
{
  InvalidateDrawCache();
}

const CPillarPos& CPillarMap::AddPillar(const geo::IPoint& position)
{
  InvalidateDrawCache();
  CPillarPos pos(position.X(), position.Y());
  TPillarMap::iterator it = m_mpPillars.insert(TPillarMap::value_type(pos, CPillar(pos))).first;
  return it->first;
}

void CPillarMap::AddPoint(const CPillarPos& pos, double dZ)
{
  InvalidateDrawCache();
  // pos must have been added with AddPillar
  TPillarMap::iterator it = m_mpPillars.find(pos);
  assert(it != m_mpPillars.end());
  it->second.insert(dZ);
}

int CPillarMap::DisplayListSize() const
{
  return 2;
}

const geo::IObject& CPillarMap::DisplayList(int nIndex) const
{
  BuildDrawCache();

  if(nIndex == 0)
  return m_arDrawPoints;
  return m_arDrawLines;
}

bool CPillarMap::Empty() const
{
  return false;
}

long CPillarMap::SavedItems() const
{
  long lRet = CColorNode::SavedItems();
  for(TPillarMap::const_iterator it = m_mpPillars.begin(); it != m_mpPillars.end(); ++it)
  lRet += it->second.SavedItems();

  return lRet;
}

void CPillarMap::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // save the pillar map
  stream << int(m_mpPillars.size());
  for(TPillarMap::iterator it = m_mpPillars.begin(); it != m_mpPillars.end(); ++it)
  {
  // save the pillar (contains the position)
  it->second.SaveStream(stream, progress);
  }

  CColorNode::SaveStream(stream, progress);
}

void CPillarMap::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const geo::IMesh& mesh = model.Mesh().Mesh();

  // load the pillar map
  int nPillars;
  stream >> nPillars;
  for(int i = 0; i < nPillars; ++i)
  {
  CPillar pillar;
  pillar.LoadStream(stream, version, progress, mesh);
  m_mpPillars.insert(TPillarMap::value_type(pillar.Position(), pillar));
  }

  CColorNode::LoadStream(stream, version, progress);
}

unsigned int CPillarMap::IconId() const
{
  return IDI_POINTSET;
}

unsigned int CPillarMap::TypeId() const
{
  return 0;
}

void CPillarMap::DetectElements(const geo::IMesh& mesh, bool bInclude)
{
  TPillarMap::iterator it;
  for(it = m_mpPillars.begin(); it != m_mpPillars.end(); ++it)
  it->second.DetectElements(mesh, bInclude);
}

CPillarMap::const_iterator CPillarMap::begin() const
{
  return m_mpPillars.begin();
}

CPillarMap::const_iterator CPillarMap::end() const
{
  return m_mpPillars.end();
}

size_t CPillarMap::size() const
{
  return m_mpPillars.size();
}

void CPillarMap::InvalidateDrawCache() const
{
  for(size_t i = 0; i < m_arDrawLines.Size(); ++i)
  delete &m_arDrawLines.Object(i);
  m_arDrawLines.Clear();
  for(size_t i = 0; i < m_arDrawPoints.Size(); ++i)
  delete &m_arDrawPoints.Object(i);
  m_arDrawPoints.Clear();
}

void CPillarMap::BuildDrawCache() const
{
  if(m_arDrawPoints.Empty() && m_arDrawLines.Empty())
  {
  TPillarMap::const_iterator it;
  size_t startidx = 0;
  for(it = m_mpPillars.begin(); it != m_mpPillars.end(); ++it)
  {
      const CPillar& pillar = it->second;
      CPillar::const_iterator itp;
      for(itp = pillar.begin(); itp != pillar.end(); ++itp)
    m_arDrawPoints.PushBack(*new geo::CPoint((*itp).first));
      if(m_arDrawPoints.Size() > 1)
    m_arDrawLines.PushBack(*new geo::CLine(m_arDrawPoints.Object(startidx), m_arDrawPoints.Object(m_arDrawPoints.Size() - 1)));
      startidx = m_arDrawPoints.Size();
  }
  }
}


CMeshSampler::CMeshSampler(const ITargetPointsetSource& source, const CMeshBase& mesh)
: m_source(source),
  m_mesh(mesh)
{
}

CPointSet* CMeshSampler::GeneratePointset(const QString& name, CModelBase& model, IProgressBase* pProgress, bool bStorePointsOutsideMesh)
{
  CPointSet* pPointSet = new CPointSet(name, model, 0, CPointSet::DIM_3D);
  CPointsetGenerator gen(*pPointSet, bStorePointsOutsideMesh);
  Generate(gen, pProgress);
  return pPointSet;
}

CPillarMap* CMeshSampler::GeneratePillarMap(const QString& name, CModelBase& model, IProgressBase* pProgress, bool bInclude)
{
  CPillarMap* pPillarMap = new CPillarMap(name, model);
  CPillarMapGenerator gen(*pPillarMap);
  Generate(gen, pProgress);
  pPillarMap->DetectElements(model.Mesh().Mesh(), bInclude);
  return pPillarMap;
}

static int sign(double dVal)
{
  if(dVal > 0)
  return 1;
  else if(dVal < 0)
  return -1;

  return 0;
}

namespace
{

class CEps
{
public:
  double operator()()
  {
  return 1e-8;
  }
};

} // anonymous namespace

void CMeshSampler::Generate(IGenerator& generator, IProgressBase* pProgress)
{
  if(pProgress)
  pProgress->AddSteps(m_source.PointSize());
  geo::CPoint max = m_mesh.Max();
  geo::CPoint min = m_mesh.Min();
  
  const double* pMaxDepth = m_source.MaximumDepth();
  if(pMaxDepth && *pMaxDepth < max.Z())
  max.Z(*pMaxDepth);
  for(int i = 0; i < m_source.PointSize(); ++i)
  {
  geo::CCoordinateSet<geo::CElementPoint, CEps> intersects;
  const geo::IPoint &samplePoint = m_source.Point(i);
  if(samplePoint.X() > m_mesh.Max().X() ||
       samplePoint.Y() > m_mesh.Max().Y() ||
       samplePoint.X() < m_mesh.Min().X() ||
       samplePoint.Y() < m_mesh.Min().Y())
  {
      if(generator.AddPointsOutsideMesh())
    intersects.insert(geo::CElementPoint(samplePoint));
  }
  else
  {
      min.X(samplePoint.X());
      min.Y(samplePoint.Y());
      max.X(samplePoint.X());
      max.Y(samplePoint.Y());
      geo::CLine line(min, max);
      std::set<int> elements = m_mesh.ElementSet().Candidates(min, max);
     
      std::set<int>::iterator it;
   
      for(it = elements.begin(); it != elements.end(); ++it)
      {
     
    const geo::IBody *el = dynamic_cast<const geo::IBody*>(&m_mesh.ElementSet().Element(*it));
    if(el)
    {
          geo::CPoint ptMin(el->Min());
          geo::CPoint ptMax(el->Max());
          if(ptMin.X() <= samplePoint.X() && ptMax.X() >= samplePoint.X() &&
             ptMin.Y() <= samplePoint.Y() && ptMax.Y() >= samplePoint.Y())
          {
      int j;
      for(j = 0; j < el->NrOfFaces(); ++j)
      {
              const geo::IFace& face = el->Face(j);
              if(face.NrOfPoints() == 3)
              {
        // intersection
        const geo::IPoint& pt1 = face.Point(0);
        const geo::IPoint& pt2 = face.Point(1);
        const geo::IPoint& pt3 = face.Point(2);
        double x = samplePoint.X();
        double y = samplePoint.Y();
        double x1 = pt1.X();
        double y1 = pt1.Y();
        double z1 = pt1.Z();
        double x2 = pt2.X();
        double y2 = pt2.Y();
        double z2 = pt2.Z();
        double x3 = pt3.X();
        double y3 = pt3.Y();
        double z3 = pt3.Z();
        double rpx = x2 - x1;
        double rpy = y2 - y1;
        double rpz = z2 - z1;
        double qpx = x3 - x1;
        double qpy = y3 - y1;
        double qpz = z3 - z1;

        // get normal z, if that's zero there is no intersection
        double nz = rpx*qpy - rpy*qpx;
        if(fabs(nz) > EPS)
        {
                  // check whether x,y is inside triangle pt1, pt2, pt3
                  double A12 = x*y1 + x1*y2 + x2*y - x*y2 - x2*y1 - x1*y;
                  double A23 = x*y2 + x2*y3 + x3*y - x*y3 - x3*y2 - x2*y;
                  double A31 = x*y3 + x3*y1 + x1*y - x*y1 - x1*y3 - x3*y;

                  // if values 'are' zero, point is on that line
                  bool bOn12 = (fabs(A12) < EPS*EPS);
                  bool bOn23 = (fabs(A23) < EPS*EPS);
                  bool bOn31 = (fabs(A31) < EPS*EPS);
                  assert(!(bOn12 && bOn23 && bOn31)); // can't be on all three lines

                  if(
                     (bOn12 && (bOn23 || bOn31 || sign(A23) == sign(A31))) ||
                     (bOn23 && (bOn12 || bOn31 || sign(A12) == sign(A31))) ||
                     (bOn31 && (bOn12 || bOn23 || sign(A12) == sign(A23))) ||
                     (sign(A12) == sign(A23) && sign(A12) == sign(A31))
          )
                  {
          // get normal coordinates
          double nx = rpy*qpz - rpz*qpy;
          double ny = rpz*qpx - rpx*qpz;

          // normalize normal
          double l = sqrt(nx*nx + ny*ny + nz*nz);
          nx /= l;
          ny /= l;
          nz /= l;

          // get z value
          double z = (nx*(x - x1) + ny*(y - y1)) / -nz + z1;
          // cap z value
          if(z >= min.Z() - EPS && z <= max.Z() + EPS)
          {
                      z = std::max(z, min.Z());
                      z = std::min(z, max.Z());

                      geo::CCoordinateSet<geo::CElementPoint, CEps>::iterator its =
            intersects.insert(geo::CElementPoint(std::set<const geo::IElement*>(), x, y, z)).first;
                      its->AddElement(*el);
          }
                  }
        }
              }
              else
              {
        geo::CPoint pt = face.Intersection(line);
        if(!pt.Empty())
        {
                  geo::CCoordinateSet<geo::CElementPoint, CEps>::iterator its = intersects.insert(geo::CElementPoint(pt)).first;
                  its->AddElement(*el);
        }
              }
      }
          }
    }
      }
  }

  if(pProgress)
  {
      try
      {
    pProgress->Step();
      }
      catch(CProgressCancel* p)
      {
    delete p;
    throw;
      }
  }

  if(!intersects.empty())
  {
      generator.OnNewSourcePoint(samplePoint);
      geo::CCoordinateSet<geo::CElementPoint, CEps>::iterator pointIt;
      for(pointIt = intersects.begin(); pointIt != intersects.end(); ++pointIt)
      {
    generator.AddPoint(pointIt->Z(), pointIt->Elements());
      }
  }
  }
}

///// IGenerator
CMeshSampler::IGenerator::IGenerator()
{
}

CMeshSampler::IGenerator::~IGenerator()
{
}

bool CMeshSampler::IGenerator::AddPointsOutsideMesh() const
{
  return false;
}


///// CPointsetGenerator
CMeshSampler::CPointsetGenerator::CPointsetGenerator(CPointSet& pointset, bool bStorePointsOutsideMesh)
: m_pointset(pointset),
  m_bStorePointsOutsideMesh(bStorePointsOutsideMesh),
  m_pCurrentPosition(0)
{
}

void CMeshSampler::CPointsetGenerator::OnNewSourcePoint(const geo::IPoint& point)
{
  m_pCurrentPosition = &point;
}

void CMeshSampler::CPointsetGenerator::AddPoint(double dZ, const std::set<const geo::IElement*>& stElements)
{
  assert(m_pCurrentPosition != 0);
  m_pointset.PushBack(geo::CElementPoint(stElements, m_pCurrentPosition->X(), m_pCurrentPosition->Y(), dZ), std::vector<double>());
}

bool CMeshSampler::CPointsetGenerator::AddPointsOutsideMesh() const
{
  return m_bStorePointsOutsideMesh;
}


///// CPillarMapGenerator
CMeshSampler::CPillarMapGenerator::CPillarMapGenerator(CPillarMap& pillarmap)
: m_pillarmap(pillarmap),
  m_pCurrentPosition(0)
{
}

void CMeshSampler::CPillarMapGenerator::OnNewSourcePoint(const geo::IPoint& point)
{
  m_pCurrentPosition = &m_pillarmap.AddPillar(point);
}

void CMeshSampler::CPillarMapGenerator::AddPoint(double dZ, const std::set<const geo::IElement*>& /*stElements*/)
{
  assert(m_pCurrentPosition != 0);
  m_pillarmap.AddPoint(*m_pCurrentPosition, dZ);
}
