
#include <memory>

#include "WellCasingRTCI.h"

#include "WellCasingModel.h"
#include "WellPath.h"
#include "NewWellPath.h"
#include "WellModelEntryTypes.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "WellPoint.h"
#include "Hexahedron.h"
#include "ModifiedHint.h"
#include "IParallelInitializationCallback.h"

static const double DEF_RTCI_ANGLE   = 20;
static const double DEF_RTCI_AZIMUTH = 0;
static const CWellCasingRTCI::TRtciDir DEF_RTCI_WINDING = CWellCasingRTCI::RD_CLOCKWISE;

///// CWellCasingRTCI

CWellCasingRTCI::CWellCasingRTCI(CFemAppModel& model)
: CColorNode(model),
  m_dAngle(DEF_RTCI_ANGLE),
  m_dAzimuth(DEF_RTCI_AZIMUTH),
  m_WindingDirection(DEF_RTCI_WINDING),
  m_pPolyLine(0),
  m_pLines(0)
{
}

CWellCasingRTCI::CWellCasingRTCI(const QString& strName, CWellCasingModel& model)
: CColorNode(strName, model),
  m_dAngle(DEF_RTCI_ANGLE),
  m_dAzimuth(DEF_RTCI_AZIMUTH),
  m_WindingDirection(DEF_RTCI_WINDING),
  m_pPolyLine(0),
  m_pLines(0)
{
  reParent(model.GraphEntry(MD_WELLCASING_RTCI));
  LinkTo(model.Mesh());
}

CWellCasingRTCI::CWellCasingRTCI(const CWellCasingRTCI& rhs)
: CColorNode(rhs),
  m_pPolyLine(0),
  m_pLines(0)
{
  operator=(rhs);
}

CWellCasingRTCI::~CWellCasingRTCI()
{
  InvalidateCache();
}

CWellCasingRTCI& CWellCasingRTCI::operator=(const CWellCasingRTCI& rhs)
{
  if(!operator==(rhs))
  {
    Name(rhs.Name());
    m_dAngle           = rhs.m_dAngle;
    m_dAzimuth         = rhs.m_dAzimuth;
    m_WindingDirection = rhs.m_WindingDirection;

    InvalidateCache();
    Modified();
  }

  return *this;
}

bool CWellCasingRTCI::operator==(const CWellCasingRTCI& rhs) const
{
  return (
    Name() == rhs.Name()                     &&
    fabs(m_dAngle - rhs.m_dAngle)     < 1e-6 &&
    fabs(m_dAzimuth - rhs.m_dAzimuth) < 1e-6 &&
    m_WindingDirection == rhs.m_WindingDirection);
}

void CWellCasingRTCI::Angle(double dAngle)
{
  m_dAngle = dAngle;
}

double CWellCasingRTCI::Angle() const
{
  return m_dAngle;
}

void CWellCasingRTCI::StartAzimuth(double dAzimuth)
{
  m_dAzimuth = dAzimuth;
}

double CWellCasingRTCI::StartAzimuth() const
{
  return m_dAzimuth;
}

void CWellCasingRTCI::WindingDirection(TRtciDir dir)
{
  m_WindingDirection = dir;
}

CWellCasingRTCI::TRtciDir CWellCasingRTCI::WindingDirection() const
{
  return m_WindingDirection;
}

double CWellCasingRTCI::AzimuthAt(double dAHD) const
{
  double dAzi = DEG2RAD(m_dAzimuth);
  dAHD -= CasingModel().StartPosition().AHD();

  if(m_WindingDirection == RD_CLOCKWISE)
    dAzi += 2 * PI * dAHD / Rate();
  else
    dAzi -= 2 * PI * dAHD / Rate();

  if(dAzi >= 2 * PI)
    dAzi -= int(dAzi / (2 * PI)) * 2 * PI;
  else if(dAzi < 0)
    dAzi += (int(-dAzi / (2 * PI)) + 1) * 2 * PI;

  assert(dAzi >= 0 && dAzi < 2 * PI);

  return dAzi;
}

double CWellCasingRTCI::AzimuthAt(int nStep) const
{
  double dFactor;
  if(m_WindingDirection == RD_CLOCKWISE)
  {
    dFactor = double(nStep) / CasingModel().Mesh().TangentialElements();
  }
  else
  {
    if(nStep == 0)
      nStep = CasingModel().Mesh().TangentialElements();
    dFactor = 1 - (double(nStep) / CasingModel().Mesh().TangentialElements());
  }

  return 2 * PI * dFactor;
}

geo::CPoint CWellCasingRTCI::PointAt(double dAHD) const
{
  double dAzi = AzimuthAt(dAHD);
  double r = RadiusAt(dAzi);

  // point in reference plane
  geo::CPoint pt(r * cos(dAzi), r * sin(dAzi), 0);

  geo::CVector vecNormal = DirectionAt(dAHD);

  // get transformation matrix at this point
  geo::CMatrix matTransform;
  if ( CasingModel().WellPath() )
  {
    well::CWellPoint ptWell=
      well::CWellPoint( *(CasingModel().WellPath()), dAHD);

    matTransform =
      CasingModel().Mesh().Create3DGridTransformationMatrix(ptWell, vecNormal);
  }
  else // wjrx mantis 3401
  {
    CNewWellPoint ptWell=
      CNewWellPoint( *(CasingModel().NewWellPath()), dAHD);

    matTransform =
      CasingModel().Mesh().Create3DGridTransformationMatrix(ptWell, vecNormal);
  }
  // transform point
  pt.Transform(matTransform);

  return pt;
  
}

double CWellCasingRTCI::AHDAt(int nStep, int nAddCycles) const
{
  double dAHD = CasingModel().StartPosition().AHD() +
    (nAddCycles + double(nStep) / CasingModel().Mesh().TangentialElements()) * Rate();

  double dStartAHD = (DEG2RAD(m_dAzimuth) / (2 * PI)) * Rate();
  if(m_WindingDirection == RD_CLOCKWISE)
    dAHD -= dStartAHD;
  else
    dAHD += dStartAHD - Rate();

  return dAHD;
/*
  double dFactor;
//  dAzimuth -= DEG2RAD(m_dAzimuth);

  if(m_WindingDirection == RD_CLOCKWISE)
  {
    dFactor = double(nStep) / CasingModel().Mesh().TangentialElements();
//    while(dFactor > 1)
//      dFactor -= 1;
  }
  else
  {
    if(nStep == 0)
      nStep = CasingModel().Mesh().TangentialElements();
    dFactor = 1 - (double(nStep) / CasingModel().Mesh().TangentialElements());
//    while(dFactor >= 1)
//      dFactor -= 1;
  }

//  while(dFactor < 0)
//    dFactor += 1;

  return CasingModel().StartPosition().AHD() + (nAddCycles + dFactor) * Rate();
*/
}

geo::CPoint CWellCasingRTCI::PointAt(int nStep, int nAddCycles) const
{
  double dAzimuth = AzimuthAt(nStep);
  double r = OuterRadius(); // at the steps it's always the casing radius

  double dAHD = AHDAt(nStep, nAddCycles);
  if(dAHD < CasingModel().StartPosition().AHD())
    return geo::CPoint();


  // point in reference plane
  geo::CPoint pt(r * cos(dAzimuth), r * sin(dAzimuth), 0);

  geo::CVector vecNormal = DirectionAt(dAHD);

  // get transformation matrix at this point
  geo::CMatrix matTransform;
  if ( CasingModel().WellPath() )
  {
    well::CWellPoint ptWell= 
      well::CWellPoint( *(CasingModel().WellPath()), dAHD);

    matTransform =
      CasingModel().Mesh().Create3DGridTransformationMatrix(ptWell, vecNormal);
  }
  else // wjrx mantis 3401
  {
    CNewWellPoint ptWell=
      CNewWellPoint( *(CasingModel().NewWellPath()), dAHD);

    matTransform =
      CasingModel().Mesh().Create3DGridTransformationMatrix(ptWell, vecNormal);
  }

  // transform point
  pt.Transform(matTransform);

  return pt;
}

double CWellCasingRTCI::RadiusAt(double dAzimuth) const
{
  // the angle of a single segment
  double dSegmentAngle = 2 * PI / CasingModel().Mesh().TangentialElements();

  int nSegmentsSkip = int(dAzimuth / dSegmentAngle);

  // angle in the segment
  double dSegAzi = dAzimuth - nSegmentsSkip * dSegmentAngle;
  assert(dSegAzi >= 0 && dSegAzi <= dSegmentAngle);

  // the angle with the center line of the triangle
  double dGamma = fabs(dSegmentAngle / 2. - dSegAzi);

  return (OuterRadius()) * cos(dSegmentAngle / 2.) / cos(dGamma);
}

double CWellCasingRTCI::OuterRadius() const
{
  return CasingModel().Mesh().CasingDiameter() / 2;
}

double CWellCasingRTCI::Rate() const
{
  double dCircum = PI * 2 * OuterRadius();
  return dCircum * tan(DEG2RAD(m_dAngle));
}

double CWellCasingRTCI::Length() const
{
  return PolyLine().GetLength();
}

const geo::IPolyLine& CWellCasingRTCI::PolyLine() const
{
  if(!m_pPolyLine)
    GenerateCache();

  assert(m_pPolyLine);
  return *m_pPolyLine;
}

size_t CWellCasingRTCI::SegmentSize() const
{
  if(m_vcSegments.empty())
    GenerateCache();

  return m_vcSegments.size();
}

int CWellCasingRTCI::DisplayListSize() const
{
  if(CasingModel().Mesh().IsMesh())
    return 1;

  return 0;
}

const geo::IObject& CWellCasingRTCI::DisplayList(int nIndex) const
{
  if(nIndex == 0)
    return LineArray();

  return m_arHexas;
}

geo::CPoint CWellCasingRTCI::Min() const
{
  return CasingModel().Mesh().Min();
}

geo::CPoint CWellCasingRTCI::Max() const
{
  return CasingModel().Mesh().Max();
}

unsigned int CWellCasingRTCI::IconId() const
{
  return IDI_WELLCASINGRTCI;
}

unsigned int CWellCasingRTCI::TypeId() const
{
  return 0;
}

void CWellCasingRTCI::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(&node == &CasingModel().Mesh() && (uHint == MeshCleared || uHint == MeshCreated))
  {
    InvalidateCache();
    Modified();
  }
}

long CWellCasingRTCI::SavedItems() const
{
  return 1;
}

void CWellCasingRTCI::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CColorNode::SaveStream(stream, progress);
  stream << m_dAngle;
  stream << m_dAzimuth;
  stream << int(m_WindingDirection == RD_CLOCKWISE ? 1 : 0);
  progress.Step();
}

void CWellCasingRTCI::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CColorNode::LoadStream(stream, version, progress);
  stream >> m_dAngle;
  stream >> m_dAzimuth;
  int n;
  stream >> n;
  m_WindingDirection = (n == 1 ? RD_CLOCKWISE : RD_ANTICLOCKWISE);

  reParent(CasingModel().GraphEntry(MD_WELLCASING_RTCI));
  LinkTo(CasingModel().Mesh());

  progress.Step();
}

CWellCasingModel& CWellCasingRTCI::CasingModel()
{
  return static_cast<CWellCasingModel&>(Model());
}

const CWellCasingModel& CWellCasingRTCI::CasingModel() const
{
  return static_cast<const CWellCasingModel&>(Model());
}

namespace
{

// map intersection point info against AHD
typedef struct
{
  geo::CPoint point;
  std::pair<const geo::CHexahedron*, const geo::CHexahedron*> prHexas;
  double ahd;
} TIntersectionInfo;

} // anonymous namespace

void CWellCasingRTCI::GenerateCache() const
{
  typedef std::map<double, TIntersectionInfo> TIntersectionsMap;
  TIntersectionsMap mpIntersections;

  std::auto_ptr <IProgressBase> waitcursor(_g->prog()->create(eProgress::Wait));

  double alpha = 2 * PI / CasingModel().Mesh().TangentialElements();

  int iOuterHexaIndex = 0;

  // wjrx mantis 3401
  bool bNewWellPath= false;
  int nDefPoints1= 0;
  nDefPoints1 = CasingModel().Mesh().DefinitionPointSize();
  if ( nDefPoints1 == 0 )
  {
    bNewWellPath= true;
    nDefPoints1 = CasingModel().Mesh().NewDefinitionPointSize();
  }

  int nProgressSteps = nDefPoints1 + int(CasingModel().Mesh().TangentialElements() * (CasingModel().EndPosition().AHD() - CasingModel().StartPosition().AHD()) / Rate());

	std::auto_ptr <IProgressBase> pMainFrm( _g->prog()->create(eProgress::MainFrame ));

	pMainFrm->StatusMessage("Generating RTCI");
	pMainFrm->AddSteps(nProgressSteps);


  if ( bNewWellPath == false )
  {
    // *** intersections with horizontal lines ***

    int i;
    const well::CWellPoint* pPrev = 0;
    for(i = 0; i < nDefPoints1; ++i)
    {
      const well::CWellPoint& ptWell = CasingModel().Mesh().DefinitionPoint(i);

      const well::CWellPoint* pNext = 0;
      if(i < nDefPoints1 - 1)
        pNext = &CasingModel().Mesh().DefinitionPoint(i + 1);

      assert(pPrev || pNext);

      double dAHD = ptWell.TMD().Value();

      if(dAHD >= CasingModel().StartPosition().AHD() && dAHD <= CasingModel().EndPosition().AHD())
      {
        double dAzi = AzimuthAt(dAHD);
        int iSegmentIndex = int(dAzi / alpha);
        assert(iSegmentIndex >= 0 && iSegmentIndex < CasingModel().Mesh().TangentialElements());

        TIntersectionInfo IntInfo;
        IntInfo.point = PointAt(dAHD);
        IntInfo.ahd = dAHD;
        const geo::IElement* pFirst = 0;
        const geo::IElement* pSecond = 0;
        if(i > 0)
          pFirst = CasingModel().Mesh().GetElement(i - 1, iSegmentIndex, iOuterHexaIndex);
        if(i < nDefPoints1 - 1)
          pSecond = CasingModel().Mesh().GetElement(i, iSegmentIndex, iOuterHexaIndex);

        assert(!pFirst  || dynamic_cast<const geo::CHexahedron*>(pFirst));
        assert(!pSecond || dynamic_cast<const geo::CHexahedron*>(pSecond));

        IntInfo.prHexas.first  = static_cast<const geo::CHexahedron*>(pFirst);
        IntInfo.prHexas.second = static_cast<const geo::CHexahedron*>(pSecond);

        mpIntersections.insert(TIntersectionsMap::value_type(dAHD, IntInfo));
      }

      pPrev = &ptWell;

      pMainFrm->Step();
    }
  }
  else // bNewWellPath //wjrx mantis 3041
  {
    // *** intersections with horizontal lines ***

    int i;
    const CNewWellPoint* pPrev = 0;
    for(i = 0; i < nDefPoints1; ++i)
    {
      const CNewWellPoint& ptWell = CasingModel().Mesh().NewDefinitionPoint(i);

      const CNewWellPoint* pNext = 0;
      if(i < nDefPoints1 - 1)
        pNext = &CasingModel().Mesh().NewDefinitionPoint(i + 1);

      assert(pPrev || pNext);

      double dAHD = ptWell.TMD();

      if(dAHD >= CasingModel().StartPosition().AHD() && dAHD <= CasingModel().EndPosition().AHD())
      {
        double dAzi = AzimuthAt(dAHD);
        int iSegmentIndex = int(dAzi / alpha);
        assert(iSegmentIndex >= 0 && iSegmentIndex < CasingModel().Mesh().TangentialElements());

        TIntersectionInfo IntInfo;
        IntInfo.point = PointAt(dAHD);
        IntInfo.ahd = dAHD;
        const geo::IElement* pFirst = 0;
        const geo::IElement* pSecond = 0;
        if(i > 0)
          pFirst = CasingModel().Mesh().GetElement(i - 1, iSegmentIndex, iOuterHexaIndex);
        if(i < nDefPoints1 - 1)
          pSecond = CasingModel().Mesh().GetElement(i, iSegmentIndex, iOuterHexaIndex);

        assert(!pFirst  || dynamic_cast<const geo::CHexahedron*>(pFirst));
        assert(!pSecond || dynamic_cast<const geo::CHexahedron*>(pSecond));

        IntInfo.prHexas.first  = static_cast<const geo::CHexahedron*>(pFirst);
        IntInfo.prHexas.second = static_cast<const geo::CHexahedron*>(pSecond);

        mpIntersections.insert(TIntersectionsMap::value_type(dAHD, IntInfo));
      }

      pPrev = &ptWell;

      pMainFrm->Step();
    }
  }

  // *** intersections with vertical lines ***

//  double dAHD = dFirstAHD;
//  double dAzi = dFirstAzi;

  int nStep = 0;
  int nCycle = 0;
  double dAHD = AHDAt(nStep, nCycle);
  while(dAHD < CasingModel().StartPosition().AHD())
  {
    ++nStep;
    if(nStep == CasingModel().Mesh().TangentialElements())
    {
      nStep = 0;
      ++nCycle;
    }
    dAHD = AHDAt(nStep, nCycle);
  }

  while(dAHD < CasingModel().EndPosition().AHD())
  {
    int iGrid = -1;
    int nDefPoints2 = 0;
    nDefPoints2 = CasingModel().Mesh().DefinitionPointSize();
    if ( nDefPoints2 > 0 ) 
    {
      int i;
      for(i = 1; i < nDefPoints2; ++i)
      {
        const well::CWellPoint& pt1 = CasingModel().Mesh().DefinitionPoint(i - 1);
        const well::CWellPoint& pt2 = CasingModel().Mesh().DefinitionPoint(i);
        if(dAHD >= pt1.TMD() && dAHD <= pt2.TMD())
        {
          iGrid = i - 1;
          break;
        }
      }
    }
    else //wjrx mantis 3401
    {
      nDefPoints2 = CasingModel().Mesh().NewDefinitionPointSize();
      int i;
      for(i = 1; i < nDefPoints2; ++i)
      {
        const CNewWellPoint& pt1 = CasingModel().Mesh().NewDefinitionPoint(i - 1);
        const CNewWellPoint& pt2 = CasingModel().Mesh().NewDefinitionPoint(i);
        if(dAHD >= pt1.TMD() && dAHD <= pt2.TMD())
        {
          iGrid = i - 1;
          break;
        }
      }
    }

    assert(iGrid != -1);

    int iSegmentIndex1;
    int iSegmentIndex2;
    int nSegments = CasingModel().Mesh().TangentialElements();

    if(WindingDirection() == RD_CLOCKWISE)
    {
      iSegmentIndex2 = nStep;
      iSegmentIndex1 = iSegmentIndex2 - 1;
      if(iSegmentIndex1 < 0)
        iSegmentIndex1 += nSegments;
    }
    else
    {
      iSegmentIndex2 = nSegments - nStep - 1;
      iSegmentIndex1 = iSegmentIndex2 + 1;
      if(iSegmentIndex1 >= nSegments)
        iSegmentIndex1 -= nSegments;
    }

    assert(iSegmentIndex1 >= 0 && iSegmentIndex1 < CasingModel().Mesh().TangentialElements());
    assert(iSegmentIndex2 >= 0 && iSegmentIndex2 < CasingModel().Mesh().TangentialElements());

    TIntersectionInfo IntInfo;
    IntInfo.point = PointAt(nStep, nCycle);
    IntInfo.ahd = AHDAt(nStep, nCycle);
    if(!IntInfo.point.Empty())
    {
      const geo::IElement* pFirst = 0;
      const geo::IElement* pSecond = 0;

      pFirst  = CasingModel().Mesh().GetElement(iGrid, iSegmentIndex1, iOuterHexaIndex);
      pSecond = CasingModel().Mesh().GetElement(iGrid, iSegmentIndex2, iOuterHexaIndex);
/*
      if(WindingDirection() == RD_ANTICLOCKWISE)
      {
        // swap first and second
        const geo::IElement* pSwap = pFirst;
        pFirst = pSecond;
        pSecond = pSwap;
      }
*/
      assert(!pFirst  || dynamic_cast<const geo::CHexahedron*>(pFirst));
      assert(!pSecond || dynamic_cast<const geo::CHexahedron*>(pSecond));

      IntInfo.prHexas.first  = static_cast<const geo::CHexahedron*>(pFirst);
      IntInfo.prHexas.second = static_cast<const geo::CHexahedron*>(pSecond);

      mpIntersections.insert(TIntersectionsMap::value_type(dAHD, IntInfo));
    }

    ++nStep;
    if(nStep == CasingModel().Mesh().TangentialElements())
    {
      ++nCycle;
      nStep = 0;
    }

    dAHD = AHDAt(nStep, nCycle);

    pMainFrm->Step();
  }

  pMainFrm.reset( _g->prog()->create(eProgress::MainFrame ) );

  pMainFrm->StatusMessage("Creating RTCI segments");
  pMainFrm->AddSteps(mpIntersections.size());
  // *** create segments ***

  double dLengthPos = 0;
  const TIntersectionInfo* pPrevInfo = 0;

  TIntersectionsMap::const_iterator it;
  for(it = mpIntersections.begin(); it != mpIntersections.end(); ++it)
  {
    const TIntersectionInfo& IntInfo = it->second;

    if(pPrevInfo)
    {
      // create a segment
      double dNextLengthPos = geo::CVector(pPrevInfo->point, IntInfo.point).Length() + dLengthPos;
 //     assert(pPrevInfo->prHexas.second == IntInfo.prHexas.first);
//      assert(IntInfo.prHexas.first != 0);

      if(IntInfo.prHexas.first != 0)
      {
        int nIndex = int(m_vcSegments.size());
        m_vcSegments.push_back(new CSegmentInfo(nIndex, *IntInfo.prHexas.first, dLengthPos, dNextLengthPos, pPrevInfo->point, IntInfo.point, pPrevInfo->ahd, IntInfo.ahd));
#ifdef _DEBUG
        m_arHexas.PushBack((geo::CHexahedron&)*IntInfo.prHexas.first);
#endif
      }

      dLengthPos = dNextLengthPos;
    }

    pPrevInfo = &IntInfo;

    pMainFrm->Step();
  }

  assert(!m_pPolyLine);
  m_pPolyLine = new CPolyLine(*this);

  assert(!m_pLines);
  m_pLines = new geo::CPtrArray<geo::ILine>(m_pPolyLine->LineSize());
  for(int i = 0; i < m_pPolyLine->LineSize(); ++i)
    m_pLines->Object(i, const_cast<geo::ILine&>(m_pPolyLine->Line(i)));
}

void CWellCasingRTCI::InvalidateCache()
{
  size_t i;
  for(i = 0; i < m_vcSegments.size(); ++i)
  {
    delete m_vcSegments[i];
  }

  m_vcSegments.clear();

  m_arHexas.Clear();

  delete m_pLines;
  m_pLines = 0;

  delete m_pPolyLine;
  m_pPolyLine = 0;
}

geo::CVector CWellCasingRTCI::DirectionAt(double dAHD) const
{
  int nDefPoints = 0;
  nDefPoints = CasingModel().Mesh().DefinitionPointSize();
  if ( nDefPoints>0 )
  {
    int i;
    const well::CWellPoint* pPrev = 0;
    for(i = 0; i < nDefPoints; ++i)
    {
      const well::CWellPoint& ptWell = CasingModel().Mesh().DefinitionPoint(i);

      const well::CWellPoint* pNext = 0;
      if(i < nDefPoints - 1)
        pNext = &CasingModel().Mesh().DefinitionPoint(i + 1);
      assert(pPrev || pNext);

      if(fabs(ptWell.TMD() - dAHD) < EPS)
      {
        // the average normal vector of the well at this point
        geo::CVector vecNormal = geo::CVector::NullVector;
        if(pPrev)
          vecNormal += geo::CVector(*pPrev, ptWell).UnitVector();
        if(pNext)
          vecNormal += geo::CVector(ptWell, *pNext).UnitVector();
        return vecNormal.UnitVector();
      }

      if(pPrev && pPrev->TMD() < dAHD && ptWell.TMD() > dAHD)
        return geo::CVector(*pPrev, ptWell).UnitVector();

      pPrev = &ptWell;
    }
  }
  else // wjrx mantis 3401
  {
    nDefPoints = CasingModel().Mesh().NewDefinitionPointSize();
    int i;
    const CNewWellPoint* pPrev = 0;
    for(i = 0; i < nDefPoints; ++i)
    {
      const CNewWellPoint& ptWell = CasingModel().Mesh().NewDefinitionPoint(i);

      const CNewWellPoint* pNext = 0;
      if(i < nDefPoints - 1)
        pNext = &CasingModel().Mesh().NewDefinitionPoint(i + 1);
      assert(pPrev || pNext);

      if(fabs(ptWell.TMD() - dAHD) < EPS)
      {
        // the average normal vector of the well at this point
        geo::CVector vecNormal = geo::CVector::NullVector;
        if(pPrev)
          vecNormal += geo::CVector(*pPrev, ptWell).UnitVector();
        if(pNext)
          vecNormal += geo::CVector(ptWell, *pNext).UnitVector();
        return vecNormal.UnitVector();
      }

      if(pPrev && pPrev->TMD() < dAHD && ptWell.TMD() > dAHD)
        return geo::CVector(*pPrev, ptWell).UnitVector();

      pPrev = &ptWell;
    }
  }

  assert(false);
  return geo::CVector();
}

const geo::IObject& CWellCasingRTCI::LineArray() const
{
  if(!m_pLines)
    GenerateCache();

  assert(m_pLines);
  return *m_pLines;
}



///// CWellCasingRTCI::CSegmentInfo

CWellCasingRTCI::CSegmentInfo::CSegmentInfo(int nIndex,
                                            const geo::CHexahedron& hexa,
                                            double dStart,
                                            double dEnd,
                                            const geo::IPoint& ptStart,
                                            const geo::IPoint& ptEnd,
                                            double dStartAHD,
                                            double dEndAHD)
: m_nIndex(nIndex),
  m_pHexa(&hexa),
  m_dStart(dStart),
  m_dEnd(dEnd),
  m_ptStart(hexa, ptStart),
  m_ptEnd(hexa, ptEnd),
  m_dStartAHD(dStartAHD),
  m_dEndAHD(dEndAHD),
  m_pVecDirection(0),
  m_pVecTangent(0)
{
//  assert(m_pHexa->Contains(m_ptStart, true));
//  assert(m_pHexa->Contains(m_ptEnd, true));
  m_vcStartShapeFunction = m_pHexa->ShapeFunction(m_pHexa->WorldToIso(m_ptStart));
  m_vcEndShapeFunction = m_pHexa->ShapeFunction(m_pHexa->WorldToIso(m_ptEnd));
}

CWellCasingRTCI::CSegmentInfo::~CSegmentInfo()
{
  delete m_pVecDirection;
  delete m_pVecTangent;
}

double CWellCasingRTCI::CSegmentInfo::Start() const
{
  return m_dStart;
}

double CWellCasingRTCI::CSegmentInfo::End() const
{
  return m_dEnd;
}

double CWellCasingRTCI::CSegmentInfo::StartAHD() const
{
  return m_dStartAHD;
}

double CWellCasingRTCI::CSegmentInfo::EndAHD() const
{
  return m_dEndAHD;
}

const geo::IPoint& CWellCasingRTCI::CSegmentInfo::Point(int nIndex) const
{
  if(nIndex == 0)
    return m_ptStart;

  return m_ptEnd;
}

int CWellCasingRTCI::CSegmentInfo::PointIndex(int nIndex) const
{
  assert(nIndex == 0 || nIndex == 1);
  return m_nIndex + nIndex;
}

geo::CValue CWellCasingRTCI::CSegmentInfo::AxialStrain(const geo::IPoint& point, const CStrainTensorValueSet& strain) const
{
  geo::CValue val;
  assert(&point == &m_ptStart || &point == &m_ptEnd);

  if(&point == &m_ptStart)
    AxialStrain(strain, m_ptStart, m_vcStartShapeFunction, val);
  else
    AxialStrain(strain, m_ptStart, m_vcEndShapeFunction, val);

  return val;
}

size_t CWellCasingRTCI::CSegmentInfo::Order() const
{
  return 1;
}

void CWellCasingRTCI::CSegmentInfo::AxialStrain(const CStrainTensorValueSet &strain,
                                                const geo::CPoint &/*pt*/,
                                                const geo::IElement::TDoubleVec &vcShapeFunction,
                                                geo::CValue &val) const
{
  // get strain values for the hexa element
  CStrainTensorValueSet::TValueVec vcValues(m_pHexa->NrOfNodes());
  strain.ElementValues(vcValues, m_pHexa->Index(), geo::IParallelInitializationCallback::Sequential);

  size_t i;
  for(i = 0; i < vcValues.size(); ++i)
  {
    if(!strain.IsValid(vcValues[i]))
      return;
  }

  // compute strain tensor for the point
  assert(vcShapeFunction.size() == vcValues.size());
  CStrainTensor strain_tensor = vcValues[0] * vcShapeFunction[0];
  for(i = 1; i < vcValues.size(); i++) {
    strain_tensor += vcValues[i] * vcShapeFunction[i];
  }

  // rotate the strain
  if(!m_pVecDirection)
  {
    assert(!m_pVecTangent);
    m_pVecDirection = new geo::CVector(geo::CVector(m_ptStart, m_ptEnd).UnitVector());
    m_pVecTangent = new geo::CVector(m_pVecDirection->GetNormal().UnitVector());
  }

  assert(m_pVecDirection && m_pVecTangent);

  geo::CVector vecRotatedStrain = strain_tensor.RotateToFace(*m_pVecDirection, *m_pVecTangent);
  val = vecRotatedStrain.X();
}


///// CWellCasingRTCI::CPolyLine

CWellCasingRTCI::CPolyLine::CPolyLine(const CWellCasingRTCI& rtci)
: m_rtci(rtci)
{
}

int CWellCasingRTCI::CPolyLine::LineSize() const
{
  return m_rtci.m_vcSegments.size();
}

const geo::ILine& CWellCasingRTCI::CPolyLine::Line(int Index) const
{
  assert(Index >= 0 && Index < m_rtci.m_vcSegments.size());
  return *m_rtci.m_vcSegments[Index];
}

void CWellCasingRTCI::CPolyLine::Swap(int /*nIndex1*/, int /*nIndex2*/)
{
  assert(false); // not supported
}

void CWellCasingRTCI::CPolyLine::AssertValid() const
{
}

std::vector<int> CWellCasingRTCI::CPolyLine::Nodes(const geo::IElement& element) const
{
  std::vector<int> vcRet;

  int i;
  for(i = 0; i < ElementSize(); ++i)
  {
    if(&Element(i) == &element)
    {
      vcRet.push_back(i);
      vcRet.push_back(i + 1);
      break;
    }
  }

  assert(!vcRet.empty());
  return vcRet;
}

geo::IPoint& CWellCasingRTCI::CPolyLine::PointAt(int nIndex)
{
  return const_cast<geo::IPoint&>(Point(nIndex));
}


/////

CWellCasingRTCIEntry::CWellCasingRTCIEntry(CWellCasingModel& model)
: CStorageNodeEntry<CWellCasingRTCI>(MD_WELLCASING_RTCI, IDI_WELLCASINGRTCI, "RTCI Wires", model)
{
}

namespace
{

const QString DEFAULT_RTCI_NAME = QObject::tr("RTCI %1");
const size_t DEFAULT_INITIAL_NUMBER = 1;

} // anonymous namespace

void CWellCasingRTCIEntry::CreateRTCI()
{
  QString strName = createUniqueName(DEFAULT_RTCI_NAME, DEFAULT_INITIAL_NUMBER);
  CWellCasingModel& model = static_cast<CWellCasingModel&>(Model());
  /*CWellCasingRTCI* pRTCI =*/ new CWellCasingRTCI(strName.toStdString().c_str(), model);
//  if(!pRTCI->Properties())
//    delete pRTCI;
}
