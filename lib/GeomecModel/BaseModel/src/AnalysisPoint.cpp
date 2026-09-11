// AnalysisPoint.cpp: implementation of the CAnalysisPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "AnalysisPoint.h"
#include "ValueTypes.h"
#include "GeomecStringTable.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation CAnalysisPoint
//////////////////////////////////////////////////////////////////////

CAnalysisPoint::CAnalysisPoint(CFemAppModel& model)
: CColorNode(model)
{
}

CAnalysisPoint::~CAnalysisPoint()
{

}

CAnalysisPoint::CAnalysisPoint(const QString& sName, const geo::IPoint& point, CModelBase& model)
: CColorNode(sName, model), m_point(point)
{
  assert(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
  reParent(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
}

CAnalysisPoint::CAnalysisPoint(unsigned int uName, const geo::IPoint& point, CModelBase& model)
: CColorNode(uName, model), m_point(point)
{
  assert(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
  reParent(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
}

CAnalysisPoint::CAnalysisPoint(const CAnalysisPoint& rhs)
: CColorNode(rhs), m_point(rhs.m_point)
{

}

bool CAnalysisPoint::operator==(const CAnalysisPoint& rhs)
{
  if(!CColorNode::operator ==(rhs))
    return false;

  return m_point == rhs.m_point;
}

CAnalysisPoint& CAnalysisPoint::operator=(const CAnalysisPoint& rhs)
{
  CColorNode::operator =(rhs);
  m_point = rhs.m_point;
  return *this;
}

void CAnalysisPoint::DefaultName(const geo::IPoint& point)
{
  QString sName;
  sName = QString("P(%1, %2, %3)").arg(point.X(), 6, 'f', 2).arg(point.Y(), 6, 'f', 2).arg(point.Z(), 6, 'f', 2);
  Name(sName);
}

void CAnalysisPoint::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{	
  CColorNode::LoadStream(stream, version, progress);

  double x, y, z;
  stream >> x;
  stream >> y;
  stream >> z;

  m_point = geo::CPoint(x, y, z);

  progress.Step();

  assert(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
  reParent(Model().GraphEntry(MD_BASE_ANALYSIS_POINT));
}

void CAnalysisPoint::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CColorNode::SaveStream(stream, progress);

  stream << m_point.X();
  stream << m_point.Y();
  stream << m_point.Z();

  progress.Step();
}

long CAnalysisPoint::SavedItems() const
{
  return 1 + CColorNode::SavedItems();
}

unsigned int CAnalysisPoint::IconId() const
{
  return IDI_ANALYSIS_POINT;
}

int CAnalysisPoint::DisplayListSize() const
{
  return 1;
}

const geo::IObject& CAnalysisPoint::DisplayList(int nIndex) const
{
  assert(nIndex == 0);
  return m_point;
}
  

const geo::IPoint& CAnalysisPoint::Point() const
{
  return m_point;
}

void CAnalysisPoint::Point(const geo::IPoint& point)
{
  m_point = point;
}

unsigned int CAnalysisPoint::TypeId() const
{
  return IDT_TREE_ANALYSIS_POINTS;
}

QString CAnalysisPoint::TypeName() const
{
  return getStringTableEntry(IDS_TREE_ANALYSIS_POINTS);
}

//////////////////////////////////////////////////////////////////////
// Implementation CAnalysisPointEntry
//////////////////////////////////////////////////////////////////////

CAnalysisPointEntry::CAnalysisPointEntry(CModelBase& model)
: CStorageNodeEntry<CAnalysisPoint>(MD_BASE_ANALYSIS_POINT, IDI_ANALYSIS_POINT_ENTRY, IDS_TREE_ANALYSIS_POINT, model)
{
}
