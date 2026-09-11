// PointSet.cpp: implementation of the CPointSet class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "PointSet.h"
#include "BaseEntryTypes.h"
#include "NodalValueSet.h"
#include "ValueTypeFactory.h"
#include "PropertyFile.h"
#include "PropertyExcel.h"
#include "FieldFactor.h"
#include "UnitConversion.h"
#include "GlobalMessage.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "GeomecStringTable.h"
#include "resourceIDI.h"
#include "ValueTypes.h"
#include "ConvexHull.h"
#include "ConvexHull_2D.h"
#include "CylindricHull.h"
#include "resourceIDS.h"
#include "StreamVersion.h"
#include "ElementSet.h"
#include "RpnValueSet.h"
#include "ModelBase.h"
#include "Sphere.h"
#include "Conus.h"
#include "ConsistencyGuard.h"

//////////////////////////////////////////////////////////////////////
// CCoordinateType
//////////////////////////////////////////////////////////////////////
CPointSet::CCoordinateType::CCoordinateType(CPointSet& point_set, 
                      IPointSet::DIMENSION dim, 
                      bool bCreateValueSet)
: CValueType(point_set, "Coordinates"), m_data(0.5), m_point_set(point_set), m_bDestroy(true)
{
  assert(dim != IPointSet::DIM_1D);
//			2D	3D
//	DIM_1D	y	-
//	DIM_2D	xy  xy	
//	DIM_3D	xyz	xyz
  for(int i = 0; i < 3; i++)
  {
    bool bCreate = false;
    QString sName;
    switch(i)
    {
    case 0:
      bCreate = (dim == IPointSet::DIM_2D) || (dim == IPointSet::DIM_3D);
      sName = getStringTableEntry(IDS_RC_NORTHING);
      break;
    case 1:
      bCreate = true;
      sName = getStringTableEntry(IDS_RC_EASTING);
      break;
    case 2:
      bCreate = (dim == IPointSet::DIM_3D);
      sName = getStringTableEntry(IDS_RC_DEPTH);
      break;
    default:
      assert(false);
      break;
    }

    if(bCreate && bCreateValueSet)
    {
      CNodalValueSet* pValueSet = new CNodalValueSet(sName, CQuantity::SI_UNIT, PointSet());
      new CCoordinateComponent(sName.toStdString().c_str(), *this, *pValueSet, i);
    }
    else
      new CCoordinateComponent(sName.toStdString().c_str(), *this, i);
  }

  assert(CanDestroy());
}

CPointSet::CCoordinateType::~CCoordinateType()
{
  Invalidate();
}

IPointSet::DIMENSION CPointSet::CCoordinateType::Dimension() const
{
  if( Component(2).Defined() )
  return IPointSet::DIM_3D;
  return IPointSet::DIM_2D;
}

void CPointSet::CCoordinateType::Dimension(IPointSet::DIMENSION dim)
{
  if (Dimension() == IPointSet::DIM_3D && dim == IPointSet::DIM_2D)
  {
  CValueComponent* pComponent = dynamic_cast<CValueComponent*>(&Component(2));

  if (pComponent)
  {
      if (pComponent->Defined())
    pComponent->UnLink(pComponent->ValueSet());

      assert(!pComponent->Defined());
  }
  }
}

bool CPointSet::CCoordinateType::CanDestroy() const
{
  return m_bDestroy;
}

void CPointSet::CCoordinateType::DestroyEnable(bool bCanDestroy)
{
  m_bDestroy = bCanDestroy;
}

std::vector<int> CPointSet::CCoordinateType::NearestNeighbourIndex(const geo::IPoint &pt) const
{
  assert(!Empty());
  Update();
  return m_data.NearestNeighbourIndex(pt);
}

int CPointSet::CCoordinateType::PointSize() const
{
  if(Component(1).Defined())
    Update();
  
  return m_data.Size();
}

//##ModelId=3C74E3EB010C
const geo::IPoint& CPointSet::CCoordinateType::Point(int nIndex) const
{
  Update();
  return (const geo::IPoint&)(m_data.ObjectBase(nIndex));
}


void CPointSet::CCoordinateType::SetPoint(int nIndex, const geo::IPoint& pt)
{
  Invalidate();
  double value[3];
  value[0] = pt.X();
  value[1] = pt.Y();
  value[2] = pt.Z();

  for(unsigned int i = 0; i < ComponentSize(); i++)
  {
    CValueComponent* pComponent = dynamic_cast<CValueComponent*>(&Component(i));
    assert(pComponent);
    if(pComponent->Defined())
    {
      CNodalValueSet *pValueSet = dynamic_cast<CNodalValueSet*>( &pComponent->ValueSet() );
      assert(pValueSet);
      pValueSet->Value(nIndex, geo::CValue(value[i]));
    }
  }
}

geo::CNearestNeighbour& CPointSet::CCoordinateType::NearestNeighbour()
{
  Update();
  return m_data;
}

const geo::CNearestNeighbour& CPointSet::CCoordinateType::NearestNeighbour() const
{
  Update();
  return m_data;
}

unsigned int CPointSet::CCoordinateType::IconId() const
{
   return IDI_VALUETYPE_COORDINATE;
}

unsigned int CPointSet::CCoordinateType::TypeId() const
{
  return IDT_VALUETYPE_COORDINATE;
}

QString CPointSet::CCoordinateType::TypeName() const
{
  return getStringTableEntry(IDS_VALUENAME_COORDINATE);
}

void CPointSet::CCoordinateType::Update() const
{
  // Do we need to update?
  if(m_data.Size() == 0)
  {
    // Yes, we do ...
    const CNodalValueSet* value_set[3];
    int nSize = -1;
    for(int i = 0; i < 3; i++)
    {
      value_set[i] = 0;
      const CCoordinateComponent& component = (const CCoordinateComponent&)Component(i);
      if(component.Defined())
      {
        value_set[i] = dynamic_cast<const CNodalValueSet*>(&component.ValueSet());
        nSize = value_set[i]->ValueSize();
      }
    }
    assert(nSize >= 0);

    double dFactor = 1;
  if (PointSet().CoordinateUnit() == CQuantity::FIELD_UNIT)
    dFactor = 1 * FF_FACTOR_LENGTH;

    // Fill the vector
    while(m_data.Size() < nSize)
    {
      int nIndex = m_data.Size();
      geo::CPoint* pPoint = new geo::CPoint(0, 0, 0);
      if(value_set[0])
        pPoint->X(dFactor * value_set[0]->Value(nIndex).Value());

      if(value_set[1])
        pPoint->Y(dFactor * value_set[1]->Value(nIndex).Value());

      if(value_set[2])
      {
        if(PointSet().ZAxis() == IPointSet::Z_UP)
          pPoint->Z(-1 * dFactor * value_set[2]->Value(nIndex).Value());
        else
          pPoint->Z(dFactor * value_set[2]->Value(nIndex).Value());
      }

      m_data.PushBack(*pPoint);
    };
  }
}

void CPointSet::CCoordinateType::Invalidate()
{
  // Delete indexed points
  for(size_t i = 0; i < m_data.Size(); i++)
    delete &m_data.ObjectBase(i);
  m_data.Clear();
}

void CPointSet::CCoordinateType::PrepareMapping()
{
  Update();
  m_data.PrepareMapping();
}


//////////////////////////////////////////////////////////////////////
// CCoordinateType::CValueComponent
//////////////////////////////////////////////////////////////////////
CPointSet::CCoordinateType::CCoordinateComponent::CCoordinateComponent(const QString& sName, CCoordinateType& value_type, int uComponentIndex)
: CValueComponent(sName, value_type, uComponentIndex, 0)
{
}

CPointSet::CCoordinateType::CCoordinateComponent::CCoordinateComponent(const QString& sName, CCoordinateType& value_type, CNodalValueSet& value_set, int uComponentIndex)
: CValueComponent(sName, value_type, value_set, uComponentIndex, 0)
{
}

QString CPointSet::CCoordinateType::CCoordinateComponent::UnitName(const CQuantity::UNIT unit) const
{
  if(unit == CQuantity::SI_UNIT)
    return QString("m");
  return QString("ft");
}

double CPointSet::CCoordinateType::CCoordinateComponent::FieldFactor() const
{
  return FF_FACTOR_LENGTH;
}

QString CPointSet::CCoordinateType::CCoordinateComponent::ExportLabel() const
{
  QString sRet;
  switch(ComponentIndex())
  {
  case 0:
    sRet = getStringTableEntry(IDS_ET_COORDINATE_X);
    break;
  case 1:
    sRet = getStringTableEntry(IDS_ET_COORDINATE_Y);
    break;
  case 2:
    sRet = getStringTableEntry(IDS_ET_COORDINATE_Z);
    break;
  default:
    break;
  };

  return sRet;
}

double CPointSet::CCoordinateType::CCoordinateComponent::ValueAt(int nIndex, CQuantity::UNIT /*unit*/) const
{
  const CNodalValueSet& value_set = dynamic_cast<const CNodalValueSet&>(ValueSet());
  return value_set.Value(nIndex).Value();
}

//////////////////////////////////////////////////////////////////////
// CPointSEt
//////////////////////////////////////////////////////////////////////
CPointSet::CPointSet(CFemAppModel& model)
: IPointSet(model)
, m_pValueType(0)
, m_pHull_3D(0)
, m_pHull_2D(0)
, m_pHull_Cylindric(0)
, m_display_hull(0)
, m_display_cylindrichull(0)
, m_pointSetType(POINTSET)
, m_ResizeHull(0.0)
, m_bTriedCreatingHull(false)
{
}

CPointSet::CPointSet(const QString &strName, 
           CFemAppModel& model, 
           const std::vector<CPointSet::TLineVec> vcData, 
           const CPointSet::DIMENSION dim,
           bool bCreateValueSetForCoordinates)
: IPointSet(strName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), 
  m_pValueType(0),
  m_pHull_3D(0), 
  m_pHull_2D(0), 
  m_pHull_Cylindric(0),
  m_display_hull(0),
  m_display_cylindrichull(0),
  m_pointSetType(POINTSET),
  m_ResizeHull(0.0),
  m_bTriedCreatingHull(false)
{
  // Create the coordinate system ...
  m_pValueType = new CCoordinateType(*this, dim, bCreateValueSetForCoordinates);
  assert((ValueSetSize() == 2) || (ValueSetSize() == 3));
  assert(vcData.size() > ValueSetSize());

  // Add the value sets till we can transfer the data
  while(NodalValueSetSize() < vcData[0].size())
    AddNodalValueSet();

  // Add the shit ...
  for(size_t i = 0; i < vcData.size(); i++)
    PushBack(vcData[i]);

  reParent(model.GraphEntry(MD_BASE_POINTSET));
}


CPointSet::CPointSet(const QString &sName, 
           CFemAppModel& model,
           int nValueSetSize, 
           const CPointSet::DIMENSION dim,
           bool bCreateValueSetForCoordinates,
           bool bLinkToEntry)
: IPointSet(sName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), 
  m_pValueType(0),
  m_pHull_3D(0), 
  m_pHull_2D(0), 
  m_pHull_Cylindric(0),
  m_display_hull(0),
  m_display_cylindrichull(0),
  m_pointSetType(POINTSET),
  m_ResizeHull(0.0),
  m_bTriedCreatingHull(false)
{
  assert((dim == DIM_3D) || (dim == DIM_2D) || (dim == DIM_1D));

  m_pValueType = new CCoordinateType(*this, dim, bCreateValueSetForCoordinates);

  for(int i = 0; i < nValueSetSize; i++)
    AddNodalValueSet();

  if(bLinkToEntry)
    reParent(model.GraphEntry(MD_BASE_POINTSET));
}

//##ModelId=3C74E3EB010A
CPointSet::~CPointSet()
{
  // Invalidate convex hull
  if(!IsCopy()) DestroyHull();
  if(m_pValueType)
    m_pValueType->DestroyEnable(true);

  for(size_t i = 0; i < m_vcElementPoints.size(); ++i)
  delete m_vcElementPoints[i];

  // Kill valuetypes
  std::vector<CValueType*> vcValueType = ValueTypes();
  for(int i = vcValueType.size() - 1; i >= 0; --i)
  {
    if(vcValueType[i]->IsCopy() == IsCopy())
      vcValueType[i]->Destroy();
  }
}

void CPointSet::pointSetType(TPointSetType pointSetType, bool bChangeHull)
{
  bool bActOnModified = bChangeHull && m_pointSetType != pointSetType;

  m_pointSetType = pointSetType;

  if (m_pointSetType == WELLPATH && m_ResizeHull <= 0)
  m_ResizeHull = 100; // default radius

  if (bActOnModified)
  {
  DestroyHull();
  Modified();
  }
}

//##ModelId=3C74E3EB0159
unsigned int CPointSet::IconId() const
{
  return IDI_POINTSET;
}

//##ModelId=3C74E3EB014E
unsigned int CPointSet::TypeId() const
{
  return 0;
}

//##ModelId=3C74E3EB00FC
CPointSet::CPointSet(const CPointSet &rhs)
: IPointSet(rhs),  
  m_pValueType(rhs.m_pValueType),
  m_pHull_3D(0), 
  m_pHull_2D(0), 
  m_pHull_Cylindric(0),
  m_display_hull(0),
  m_display_cylindrichull(0),
  m_pointSetType( rhs.m_pointSetType ),
  m_ResizeHull(rhs.m_ResizeHull),
  m_bTriedCreatingHull(rhs.m_bTriedCreatingHull)
{
  assert(m_pValueType->IsCopied());
  m_pValueType->DestroyEnable(false);
}

void CPointSet::ZAxis(Z_AXIS axis)
{
  assert(Dimension() == DIM_3D);
  assert(IsCopy());
  IPointSet::ZAxis(axis);
}

//##ModelId=3C74E3EB0138
bool CPointSet::operator==(const CPointSet &rhs) const
{
  if(!(  IPointSet::operator==(rhs) ))
    return false;

  if(Dimension() != rhs.Dimension())
    return false;

  if (ResizeHullValue() != rhs.ResizeHullValue())
    return false;

  return true;
}

//##ModelId=3C74E3EB013D
bool CPointSet::CanDestroy() const
{ 
  // We can be destroyed under the condition that no of our clouds is used ...
  std::vector<CValueType*> vcValueType = ValueTypes();

  for(size_t i = 0; i < vcValueType.size(); i++)
  {
    if(!vcValueType[i]->CanDestroy())
      return false;
  }

  return true;
}

//##ModelId=3C74E3EB012D
void CPointSet::OnNewNeighbour(const CGraphNode &node) 
{
  if(dynamic_cast<const CValueType*>(&node))
    Modified();

  CGraphNode::OnNewNeighbour(node);
}

//##ModelId=3C74E3EB015D
bool CPointSet::EqualPoints(const CPointSet &/*set*/) const
{
/*
  std::vector<CPoint*> vcDiff;

  std::set_difference(m_data.begin(),
            m_data.end(),
            set.m_data.begin(), 
            set.m_data.end(),
            std::back_inserter(vcDiff),
            CPointLess());

  if(vcDiff.size() > 0)
    return false;
*/
  return true;

}

// calculate the bounding box of the point set and assign to min and max.
// mantis 2401 wjrx
//
void CPointSet::BBox(geo::IPoint &min, geo::IPoint &max) const
{
  assert(!Empty());

  min= PointAt(0);
  max= PointAt(0);

  for (int ii= 1; ii< PointSize(); ++ii)
  {
  const geo::IPoint &pt= PointAt(ii);
  if ( min.X() > pt.X() ) min.X( pt.X() );
  if ( min.Y() > pt.Y() ) min.Y( pt.Y() );
  if ( min.Z() > pt.Z() ) min.Z( pt.Z() );
  if ( max.X() < pt.X() ) max.X( pt.X() );
  if ( max.Y() < pt.Y() ) max.Y( pt.Y() );
  if ( max.Z() < pt.Z() ) max.Z( pt.Z() );
  }
}

// determine if point pt is in the bounding box (min,max)
// no longer used for mantis 2401 wjrx
//
bool CPointSet::InBBox(const geo::IPoint &pt, geo::IPoint &min, geo::IPoint &max) const
{
  assert( !pt.Empty() );
  assert( !min.Empty() );
  assert( !max.Empty() );

  // wedx 12072007: take dimension into account when determining whether point is inside bounding box
  int nDim = (int)Dimension();

  if ( pt.X() < min.X() ) return false;
  if ( pt.X() > max.X() ) return false;

  if(nDim > 1)
  {
    if ( pt.Y() < min.Y() ) return false;
    if ( pt.Y() > max.Y() ) return false;

    if(nDim > 2)
    {
      if ( pt.Z() < min.Z() ) return false;
      if ( pt.Z() > max.Z() ) return false;
    }
  }

  return true;
}

bool CPointSet::PointInConvexHull(const geo::IPoint& pt) const
{
  if(!m_pHull_2D && !m_pHull_3D && !m_pHull_Cylindric)
  {
  CreateHull();
  }

  if(m_pHull_3D)
  {
  bool b= false;
  try
  {
      b= m_pHull_3D->Contains(pt, true);
  }
  catch (const char* msg)
  {
      _m()->warn(msg);
  }
  catch(...)
  {
      _m()->warn("Unknown Convex Hull 'Contains' problem, please report.");
  }
  return b;
  }
  else if (m_pHull_2D)
  {
  return (m_pHull_2D->ContainsXY(pt, true) == true);
  }
  else if (m_pHull_Cylindric)
  {
  return m_pHull_Cylindric->Contains(pt, true);
  }

  return false;
}

//##ModelId=3C74E3EB013B
CPointSet::DIMENSION CPointSet::Dimension() const
{
  return m_pValueType->Dimension();
}

bool CPointSet::HasHull() const
{
  int nDim = 0;
  unsigned int i;
  for(i = 0; i < m_pValueType->ComponentSize(); i++) {
    if(m_pValueType->Component(i).Defined()) 
      nDim++; 
  }

  switch(nDim)
  {
  case 3:
  return (m_pHull_3D != 0);
  case 2:
  return (m_pHull_2D != 0);
  default:
  assert(false);
  }

  return false;
}

geo::CCylindricHull *CPointSet::CreateCylindricHull(bool& /* bCanceled */) const
{
  std::vector<const geo::IPoint *> points(PointSize());

  for (int i = 0; i < PointSize(); ++i)
  points[i] = &PointAt(i);

  return new geo::CCylindricHull(points, m_ResizeHull);
}

geo::CConvexHull_2D *CPointSet::Create2DHull(bool& /* bCanceled */, int nDisabledDim) const
{
  geo::CConvexHull_2D *hull = new geo::CConvexHull_2D(m_pValueType->NearestNeighbour(), nDisabledDim);
  if (m_ResizeHull > 0)
  hull->ResizeHull(m_ResizeHull);

  return hull;
}

geo::CConvexHull    *CPointSet::Create3DHull(bool& bCanceled) const
{
  geo::CConvexHull *hull = 0;

  std::auto_ptr <IProgressBase> prog;

  try
  {
  hull = new geo::CConvexHull(m_pValueType->NearestNeighbour());
  hull->SetValidDigits(1.0);
  prog.reset(_g->prog()->create(eProgress::Geo, "Generating convex hull..."));
  // Introduce progress bar, because this can take a while...
  bool bCreated = hull->Calculate(prog.get());

  if (!bCreated) throw ((const char *)("Convex Hull failed its consistency checks"));

  if (m_ResizeHull > 0) hull->ResizeHull(m_ResizeHull);
  }
  catch (CProgressCancel *e)
  {
  delete e;
  delete hull;
  hull = 0;
  bCanceled = true;
  }
  catch (...)
  {
  delete hull;
  hull = 0;
  }

  return hull;
}

bool CPointSet::CreateHull(CPointSetEntry::CreateHullCallback cb) const
{
  if (m_bTriedCreatingHull)
  return true;

  if (pointSetType() == TIME_DEPTH)
  return true;

  int nDim = 0;
  int nDisabled = -1;
  unsigned int i;
  for(i = 0; i < m_pValueType->ComponentSize(); i++) {
    if(m_pValueType->Component(i).Defined()) 
      nDim++; 
    else 
      nDisabled = i;
  }

  std::auto_ptr <IProgressBase> prog;

  bool bCanceled = false;

  switch(nDim)
  {
  case 3:
    assert(m_pHull_3D == 0);
  assert(m_pHull_Cylindric == 0);

  if (pointSetType() == POINTSET)
  {
      m_pHull_3D = Create3DHull(bCanceled);

      if (!bCanceled && !m_pHull_3D)
      {
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    CConsistencyGuard *guard = const_cast<CModelBase&>(model).GetConsistencyGuard();

    if (guard->AllowMovePointsets())
    {
          const_cast<CPointSet *>(this)->pointSetType(WELLPATH);
          if (cb)
      cb();
    }
      }
  }

  if (pointSetType() == WELLPATH || pointSetType() == INPUT)
      m_pHull_Cylindric = CreateCylindricHull(bCanceled);

  if (!bCanceled && !m_pHull_Cylindric && !m_pHull_3D)
      _m()->warn(QString("Failed to create a hull for pointset '%1'").arg(Name()));

    break;
  case 2:
    assert(nDisabled > -1);
    assert(m_pHull_2D == 0);
  m_pHull_2D = Create2DHull(bCanceled, nDisabled);
    break;
  default:
       assert(FALSE);
  }

  if (!bCanceled)
  {
  m_bTriedCreatingHull = true;
  return true;
  }
  else
  return false;
}

void CPointSet::DestroyHull()
{
  m_bbox_min = geo::CPoint();
  m_bbox_max = geo::CPoint();

  delete m_pHull_3D;
  m_pHull_3D = 0;

  delete m_pHull_2D;
  m_pHull_2D = 0;

  delete m_pHull_Cylindric;
  m_pHull_Cylindric = 0;

  m_display_hull.ClearAndDelete();
  m_display_cylindrichull.ClearAndDelete();

  m_bTriedCreatingHull = false;
}



//##ModelId=3C74E3EB0119
std::vector<int> CPointSet::PointAt(const geo::IPoint &pt) const
{
  assert(!Empty());
  return m_pValueType->NearestNeighbourIndex(pt);
}

//##ModelId=3C74E3EB010F
int CPointSet::PointSize() const
{
  if(m_pValueType)
    return m_pValueType->PointSize();
  return 0;	// We have no points
}

//##ModelId=3C74E3EB010C
const geo::IPoint& CPointSet::PointAt(int nIndex) const
{
  // return the real reference if we have it
  if(m_vcElementPoints.size() == m_pValueType->PointSize())
  return *m_vcElementPoints[nIndex];

  assert(!Empty());
  return m_pValueType->Point(nIndex);
}

int CPointSet::DisplayListSize() const
{ 
  return static_cast<const CModelBase&>(Model()).ShowConvexHulls() ? 2 : 1;
}

void CPointSet::SetPoint(int nIndex, const geo::IPoint& pt)
{
  if(nIndex < m_vcElementPoints.size())
  {
  const geo::CElementPoint* pElmPoint = dynamic_cast<const geo::CElementPoint*>(&pt);
  if(pElmPoint)
  {
      delete m_vcElementPoints[nIndex];
      m_vcElementPoints[nIndex] = new geo::CElementPoint(*pElmPoint);
  }
  }

  assert(!Empty());
  m_pValueType->SetPoint(nIndex, pt);
}

//##ModelId=3C74E3EB011C
const geo::IObject& CPointSet::DisplayList(int nIndex) const
{
  assert(nIndex >= 0 && nIndex <= 1);

  if (nIndex == 1)
  {
  assert(static_cast<const CModelBase&>(Model()).ShowConvexHulls());

  if (!HasHull())
      CreateHull();

  if (m_pHull_Cylindric)
  {
      if (m_display_cylindrichull.Size() == 0)
      {
    const geo::IPoint *prevPoint = 0;

    for (geo::CPtrArray<geo::IPoint>::const_iterator it = m_pValueType->NearestNeighbour().begin(); it != m_pValueType->NearestNeighbour().end(); ++it)
    {
          const geo::IPoint *curPoint = (*it);

          if (prevPoint)
          {
      geo::CVector v(*curPoint - *prevPoint);
      m_display_cylindrichull.PushBack(*new geo::CConus(*prevPoint, v.UnitVector(), m_pHull_Cylindric->Radius(), v.Length(), 1));
          }

          m_display_cylindrichull.PushBack(*new geo::CSphere(*curPoint, m_pHull_Cylindric->Radius()));

          prevPoint = curPoint;
    }
      }

      return m_display_cylindrichull; // TODO: we return it, but OIV can't handle it yet
  }
  else
  {
      if (m_display_hull.Size() == 0)
      {
    // Using lines, as they get picked up by OIV right away. For 3D it may not be ideal in the viewing, but it should give a decent idea
    // of whether the convex hull is correct, which is the goal
    if (m_pHull_2D)
    {
          for (int i = 0; i < m_pHull_2D->NrOfLines(); ++i)
      m_display_hull.PushBack(*new geo::CLine(m_pHull_2D->Line(i)));
    }
    else if (m_pHull_3D)
    {
          geo::CPoint centerPoint = m_pHull_3D->MidPoint();

          for (int i = 0; i < m_pHull_3D->NrOfFaces(); ++i)
          {
      for (int j = 0; j < m_pHull_3D->Face(i).NrOfLines(); ++j)
      {
              geo::CLine *line = new geo::CLine(m_pHull_3D->Face(i).Line(j));
              for (int k = 0; k < 2; ++k)
        const_cast<geo::IPoint&>(line->Point(k)).Set(line->Point(k).X() + centerPoint.X(), line->Point(k).Y() + centerPoint.Y(), line->Point(k).Z() + centerPoint.Z());
              m_display_hull.PushBack(*line);
      }
          }
    }
      }

      return m_display_hull;
  }
  }

  assert(nIndex == 0);
  return m_pValueType->NearestNeighbour();
}

//##ModelId=3C74E3EB015B
CPointSet& CPointSet::operator=(const CPointSet& rhs)
{
  assert(rhs.IsCopy());

  assert(PointSize() == rhs.PointSize());

  ResizeHullValue( rhs.ResizeHullValue() );

  // The valuetypes which have no copy are deleted from the copy 
  // and now from the source ...
  std::vector<CValueType*> vcValueType = ValueTypes();
  for(size_t i = 0; i < vcValueType.size(); i++)
  {
    assert(!vcValueType[i]->IsCopy());
    if(!vcValueType[i]->IsCopied())
      delete vcValueType[i];
  }

  IPointSet::operator=(rhs);

  m_pValueType->Invalidate();

  AssertValid();

  m_bTriedCreatingHull = rhs.m_bTriedCreatingHull;

  return *this;
}

/*virtual*/ void CPointSet::OnCoordinateUnitChanged(CQuantity::UNIT /*old_unit*/,CQuantity::UNIT /*new_unit*/)
{
  // 
}

void CPointSet::ResizeHullValue(double d)
{ 
  if (m_ResizeHull == d)
  return;

  m_ResizeHull = d;
  // we don't have to call DestroyHull() as the resize distance isn't (and never was) used during creation; but we do need to notify the cylindric hull, if we have it
  // actually, we need to notify any of them
  if (m_pHull_3D)
  m_pHull_3D->ResizeHull(d);
  if (m_pHull_2D)
  m_pHull_2D->ResizeHull(d);
  if (m_pHull_Cylindric)
  m_pHull_Cylindric->Radius(m_ResizeHull);
}


//##ModelId=3C74E3EB00B0
int CPointSet::PushBack(const std::vector<double> &vcRow, bool bConvertValue)
{
  AssertValid();

  // Create dimension and index
  AssertValid();
  assert(vcRow.size() == NodalValueSetSize());
  assert(vcRow.size() > 0);

  int nIndex = NodalValueSet(0).ValueSize();

  // Add point to value set
  for(int i = 0; i < NodalValueSetSize(); i++)
  {
  // wjrx mantis 3340
  //
  const IUnitConversion *pConversion= 
      UnitConversion( ValueSet(i).Name().toStdString());

  if ( pConversion )
      NodalValueSet(i).PushBack( 
    geo::CValue(pConversion->ConvertToSi( vcRow[i])));
  else if (bConvertValue && ValueUnit(true) == CDoubleQuantity::FIELD_UNIT)
      NodalValueSet(i).PushBack(vcRow[i] / NodalValueSet(i).Component()->FieldFactor());
  else
      NodalValueSet(i).PushBack(geo::CValue(vcRow[i]));
  }

  DestroyHull();

  return nIndex;
}

int CPointSet::PushBack(const std::vector<geo::CValue> &vcRow, bool bConvertValue)
{
  AssertValid();

  // Create dimension and index
  AssertValid();
  assert(vcRow.size() == NodalValueSetSize());
  assert(vcRow.size() > 0);

  int nIndex = NodalValueSet(0).ValueSize();

  // Add point to value set
  for(int i = 0; i < NodalValueSetSize(); i++)
  {
  // wjrx mantis 3340
  //
  const IUnitConversion *pConversion= 
      UnitConversion( ValueSet(i).Name().toStdString());

  if ( pConversion && vcRow[i].Valid() )
      NodalValueSet(i).PushBack( 
    geo::CValue(pConversion->ConvertToSi( vcRow[i].Value())));
  else
  {
      if (vcRow[i].Valid() && bConvertValue && ValueUnit(true) == CDoubleQuantity::FIELD_UNIT)
    NodalValueSet(i).PushBack(vcRow[i] / NodalValueSet(i).Component()->FieldFactor());
      else
    NodalValueSet(i).PushBack(vcRow[i]);
  }
  }

  DestroyHull();

  return nIndex;
}

//##ModelId=3C74E3EB00BB
int CPointSet::PushBack(const geo::IPoint &point, const std::vector<double> &vcRow, bool bConvertValue)
{
  AssertValid();

  int nIndex = NodalValueSet(0).ValueSize();
  int nRow = 0;
  int nCoordinate = 0;

  const geo::CElementPoint* pElmPoint = dynamic_cast<const geo::CElementPoint*>(&point);
  if(pElmPoint)
  m_vcElementPoints.push_back(new geo::CElementPoint(*pElmPoint));

  for(int i = 0; i < NodalValueSetSize(); i++)
  {
  const IUnitConversion *conversion= UnitConversion( NodalValueSet(i).Name().toStdString());

  CNodalValueSet& value_set = NodalValueSet( i );
  if(value_set.Component() == &Coordinates().Component(0))
  {
      value_set.PushBack(point.X());
      nCoordinate++;
  }
  else if(value_set.Component() == &Coordinates().Component(1))
  {
      value_set.PushBack(point.Y());
      nCoordinate++;
  }
  else if(value_set.Component() == &Coordinates().Component(2))
  {
      value_set.PushBack(point.Z());
      nCoordinate++;
  }
  else
  {
      if ( conversion )
      {
    double val= conversion->ConvertToSi( vcRow[nRow] );
    value_set.PushBack(val);
      }
      else if (bConvertValue && ValueUnit(true) == CDoubleQuantity::FIELD_UNIT)
    NodalValueSet(i).PushBack(vcRow[i] / NodalValueSet(i).Component()->FieldFactor());
      else
    value_set.PushBack(vcRow[nRow]);	

      nRow++;
  }
  }

  assert((nRow + nCoordinate) == NodalValueSetSize());

  DestroyHull();

  return nIndex;
}

void CPointSet::CoordinateUnit(CQuantity::UNIT unit)
{
  // Our source data is always SI. 
  // If m_unit = SI, the source data was SI and interpret as SI
  IPointSet::CoordinateUnit(unit);
}


#ifdef _DEBUG
void CPointSet::AssertValid() const
{
  IPointSet::AssertValid();

  int i;
  for(i = 0; i < ValueSetSize(); i++)
  {
    assert(IsLinkedTo(ValueSet(i)));
    ValueSet(i).AssertValid();
  }

  for(unsigned int i = 0; i < Coordinates().ComponentSize(); i++)
  {
    if((Dimension() == DIM_3D) || (i < 2))
    {
      if(IsCopy())
      {
        assert(m_pValueType->IsCopied());
        const CValueType *pValueType = dynamic_cast<const CValueType*>(&m_pValueType->Copy());
        assert(pValueType);
        pValueType->Component(i).Defined();
      }
//			else
//				assert(m_pValueType->Component(i).Defined());
    }
  }

}
#endif //_DEBUG

bool CPointSet::Empty() const
{
  return m_pValueType == 0;
}

CPointSet::CCoordinateType& CPointSet::Coordinates()
{
  return *m_pValueType;
}

const CPointSet::CCoordinateType& CPointSet::Coordinates() const
{
  return *m_pValueType;
}

void CPointSet::LoadPointSetData(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  assert(m_pValueType == 0);
  // Save the pointset data first
  // Dimension
  int nDimension;
  stream >> nDimension;
  m_pValueType = new CCoordinateType(*this,							// PointSet
                     (DIMENSION)nDimension,			// Dimension
                     version < CStreamVersion(3, 0, 15));	// From version 3,0,15 we don't create value sets
  // Coordinate Unit
  int nUnit;
  stream >> nUnit;
  CoordinateUnitEx((CQuantity::UNIT)nUnit);

  // New in 3.0.1 value units
  if(CStreamVersion(3, 0, 0) < version)
  {
    stream >> nUnit;
    ValueUnit((CQuantity::UNIT)nUnit);
  }

  if(version < CStreamVersion(3, 0, 15))
  {
    // Loading points
    QString strTemp;
    stream >> strTemp;
    int iPointSize;
    stream >> iPointSize;
    for (int count = 0; count < iPointSize; count++)
    {
      double X,Y,Z;
      stream >> X >> Y >> Z;
      if( Dimension() == DIM_2D )
        PushBack( geo::CPoint(X,Y), std::vector<double>() );
      else
        PushBack( geo::CPoint(X,Y,Z), std::vector<double>() );

      progress.Step();
    }
  }
}

void CPointSet::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  // Notify user of point set loading
  assert( Empty() );
  std::string pr("Loading pointset ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  // From version 3.0.5 we read the point set data first
  if(CStreamVersion(3, 0, 4) < version)
    LoadPointSetData(stream, version, progress);

  // In the old version we read the base first
  IPointSet::LoadStream(stream,version,progress);

  // From version 3.0.5 the RpnValueSets and the NodalValueSets are saved
  // by the base class. 
  if(version < CStreamVersion(3, 0, 5))
  {
    LoadPointSetData(stream, version, progress);

    // And load the adapters
    int iResultCount;
    stream >> iResultCount;
    for (int count = 0; count < iResultCount; count++)
    {
      // The result adapter only saves the name and index (CStorageNode::LoadStream(..) base
      // implementation). We only support this, because we don't want a corrupt stream ...
      QString sName;
      stream >> sName;
      int iIndex;
      stream >> iIndex;
    }
  }

  if (version < CStreamVersion(4, 1, 49))
  {
  int dimension = 0;
  if (CoordinateUnit() == CQuantity::FIELD_UNIT)
  {
      switch (Dimension())
      {
      case DIM_1D:
    dimension = 1;
    break;
      case DIM_2D:
    dimension = 2;
    break;
      case DIM_3D:
    dimension = 3;
    break;
      }
      for (int i = 0; i < dimension; ++i)
      {
    CNodalValueSet& set = NodalValueSet(i);
    if (set.Component()->Defined())
    {
          double factor = 1 / set.Component()->FieldFactor();
          int size = set.ValueSize();
          for (int j = 0; j < size; ++j)
          {
      const IValueSet::TValue& val = set.Value(j);
      if (val.Valid())
              set.Value(j, val * factor);
          }
    }
      }
      CoordinateUnit(CQuantity::SI_UNIT);
  }

  if (ValueUnit() == CQuantity::FIELD_UNIT)
  {
      for (int i = dimension; i < NodalValueSetSize(); ++i)
      {
    CNodalValueSet& set = NodalValueSet(i);
    // if the property is unassigned, we won't be able to do a conversion (no component)
    // we still set the unit to SI and accept that when the property is assigned, it probably has the wrong value
    // this is per request by Peter Fokker
    if (set.Component() && set.Component()->Defined())
    {
          double factor = 1 / set.Component()->FieldFactor();
          int size = set.ValueSize();
          for (int j = 0; j < size; ++j)
          {
      const IValueSet::TValue& val = set.Value(j);
      if (val.Valid())
              set.Value(j, val * factor);
          }
    }
      }
      ValueUnit(CQuantity::SI_UNIT);
  }
  }

  if (version < CStreamVersion(3, 0, 100))
  {
    m_ResizeHull= 0.0;
  }
  else
  {
    stream >> m_ResizeHull;
  }

  if(version >= CStreamVersion(4, 1, 23))
  {
  int nHasHull;
  stream >> nHasHull;

  if(nHasHull)
  {
      progress.StatusMessage(QObject::tr("Loading convex hull..."));
    m_pHull_3D = new geo::CConvexHull(m_pValueType->NearestNeighbour());
      m_pHull_3D->Load(stream, progress);
  }
  }

  if(version >= CStreamVersion(4, 1, 26))
  {
  int nHasHull;
  stream >> nHasHull;

  if(nHasHull)
  {
      progress.StatusMessage(QObject::tr("Loading convex hull..."));
      m_pHull_2D = new geo::CConvexHull_2D;
      m_pHull_2D->Load(stream, progress);
  }
  }

  // waij TFS 92430
  if(version >= CStreamVersion(4, 1, 25))
  {
    int isWellPath;
    stream >> isWellPath;

  if (version <= CStreamVersion(4, 1, 39))
  {
      m_pointSetType = (isWellPath != 0) ? WELLPATH : POINTSET;
  }
  else
  {
      switch (isWellPath)
      {
      case 0:
    m_pointSetType = POINTSET;
    break;
      case 1:
    m_pointSetType = WELLPATH;
    break;
      case 2:
    m_pointSetType = INPUT;
    break;
      case 3:
    m_pointSetType = TIME_DEPTH;
    break;
      default:
    assert(false);
    break;
      }
  }
  }

  reParent(Model().GraphEntry(MD_BASE_POINTSET));

  AssertValid();
  assert(!Empty());
}

void CPointSet::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{

  // Save all the valueset that we have
  std::string pr("Saving Pointset ");
  pr += Name().toStdString();
  progress.StatusMessage(pr.c_str());

  // From version 3.0.5 the valuesets are saved in the IPointSet base
  // class. 


  // Save dimension
  int iDim = Dimension();
  stream << iDim;

  // Save units of the coordinates
  int iCoordinateUnit = IPointSet::CoordinateUnit();
  stream << iCoordinateUnit;

  // In database version 3.0.1 we have also value unit system tag
  int iValueUnit = IPointSet::ValueUnit();
  stream << iValueUnit;

  IPointSet::SaveStream(stream,progress);

  stream << m_ResizeHull;

  // save the hull
  stream << int(m_pHull_3D ? 1 : 0);
  if(m_pHull_3D)
  {
  progress.StatusMessage(QObject::tr("Saving convex hull..."));
  m_pHull_3D->Save(stream, progress);
  }

  stream << int(m_pHull_2D ? 1 : 0);
  if(m_pHull_2D)
  {
  progress.StatusMessage(QObject::tr("Saving convex hull..."));
  m_pHull_2D->Save(stream, progress);
  }

  // waij TFS 92430
  stream << int(m_pointSetType);
}

long CPointSet::SavedItems() const
{
  long lRet = IPointSet::SavedItems();
  if(m_pHull_3D)
  lRet += m_pHull_3D->SaveProgressSize();

  return lRet;
}

bool CPointSet::CanConnectItem(const CGraphNode& item) const
{
  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&item);
  if(pValueType)
    return true;
  if(pPointSet)
  {
    if(pPointSet == this)
      return false;
    return true;
  }

  return IPointSet::CanConnectItem(item);
}

bool CPointSet::ConnectItem(const CGraphNode& item)
{
  assert(CanConnectItem(item));

  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&item);

  if(pValueType)
  {
    // Do we have overlap on the other pointset?
    if(Overlap(pValueType->PointSet()))
    {
      // Do the dialog
      QString sSource = "point set";
      if(dynamic_cast<const IElementSet*>(&pValueType->PointSet()))
        sSource = "element set";
      QString sMessage;
      sMessage = QString("Convert component %1 of %2 %3 to point set %4.").
              arg(pValueType->Name()).
              arg(sSource).
              arg(pValueType->PointSet().Name()).
              arg(Name());


      if(_m()->msg(sMessage, MB_YESNO) == IDYES)
      {
        // Create dialog
        std::auto_ptr <IProgressBase> progress(_g->prog()->create(eProgress::Geo, "", false));

        // Create property
    const CValueTypeFactory *factory = CValueTypeFactory::instance();
        CValueType *pCopy = factory->BuildValueType(*this, pValueType->TypeId(), pValueType->Name());
        assert(pCopy);

        progress->AddSteps(PointSize() * pCopy->ComponentSize());

        // Copy the property
        for(unsigned int nComponent = 0; nComponent < pCopy->ComponentSize(); nComponent++)
        {
          CNodalValueSet *pValueSet = new CNodalValueSet(*this);
          pValueSet->LinkTo(pCopy->Component(nComponent));
          assert(pValueSet->ValueSize() == PointSize());

          // Walk over elements
          int nPoint;
          for(nPoint = 0; nPoint < PointSize(); nPoint++)
          {
            pValueSet->Value(nPoint, pValueType->Component(nComponent).ScalarData().ValuePoint(PointAt(nPoint)));
            progress->Step();
          }
        }

        Modified();
      }
        
      return true;
    }
    else
      _m()->msg("Element set do not overlap");
  }

  if(pPointSet)
  {
    CPointSet *pTarget =  new CPointSet("Merged", Model(), 0, CPointSet::DIM_3D);
    Merge(*pTarget, *pPointSet);

  }
  return IPointSet::ConnectItem(item);
}

std::vector<CValueType*> CPointSet::SharedValueTypes(const CPointSet &source) const
{
  std::vector<CValueType*> this_valuetypes = ValueTypes();
  std::vector<CValueType*> source_valuetypes = source.ValueTypes();
  if(this_valuetypes.size() == 0 || source_valuetypes.size() == 0)
    return std::vector<CValueType*>();

  std::map<unsigned int, int> type_counts_this;
  std::map<unsigned int, int> type_counts_source;
  std::map<unsigned int, int>::iterator it, it2;
  
  for(size_t i = 0; i < this_valuetypes.size(); i++)
  {
    it = type_counts_this.find(this_valuetypes[i]->TypeId());
    if(it != type_counts_this.end())
      it->second++;
    else
      type_counts_this.insert(std::map<unsigned int, int>::value_type(this_valuetypes[i]->TypeId(), 1));
  }

  for(size_t i = 0; i < source_valuetypes.size(); i++)
  {
    it = type_counts_source.find(source_valuetypes[i]->TypeId());
    if(it != type_counts_source.end())
      it->second++;
    else
      type_counts_source.insert(std::map<unsigned int, int>::value_type(source_valuetypes[i]->TypeId(), 1));
  }

  it = type_counts_this.begin();
  while(it != type_counts_this.end())
  {
    if(it->second > 1)
    {
      type_counts_this.erase(it);
      it = type_counts_this.begin();
      continue;
    }

    it++;
  }

  it = type_counts_source.begin();
  while(it != type_counts_source.end())
  {
    if(it->second > 1)
    {
      type_counts_source.erase(it);
      it = type_counts_source.begin();
      continue;
    }

    it++;
  }

  std::vector<CValueType*> vcRet;
  for(it = type_counts_this.begin(); it != type_counts_this.end(); it++)
  {
    for(size_t i = 0; i < this_valuetypes.size(); i++)
    {
      if(this_valuetypes[i]->TypeId() == it->first)
        vcRet.push_back(this_valuetypes[i]);
    }
  }
/*
  std::vector<unsigned int> vcRet;
  for(it = type_counts_this.begin(); it != type_counts_this.end(); it++)
  {
    if(type_counts_source.find(it->first) != type_counts_source.end())
      vcRet.push_back(it->first);
  }
*/
  return vcRet;
}

void CPointSet::GetValueTypesNoAveraging(CPointSet &target, const CPointSet &ptset, std::vector<CValueType*> vcTypes) const 
{
  CValueType *pVT;
  CValueType *pVTtemp_this = 0;
  CValueType *pVTtemp_source = 0;

  const CValueTypeFactory *fact = CValueTypeFactory::instance();

  std::vector<CValueType*> this_valuetypes = ValueTypes();
  std::vector<CValueType*> source_valuetypes = ptset.ValueTypes();

  
  int k;
  for(size_t i = 0; i < vcTypes.size(); i++)
  {
    pVT = fact->BuildValueType(target, vcTypes[i]->TypeId(), vcTypes[i]->TypeName());

    for(size_t j = 0; j < this_valuetypes.size(); j++)
    {
      if(this_valuetypes[j]->TypeId() == vcTypes[i]->TypeId())
      {
        pVTtemp_this = this_valuetypes[j];
        break;
      }
    }

    for(size_t j = 0; j < source_valuetypes.size(); j++)
    {
      if(source_valuetypes[j]->TypeId() == vcTypes[i]->TypeId())
      {
        pVTtemp_source = source_valuetypes[j];
        break;
      }
    }
        
  if (pVTtemp_this) {
      for (unsigned int j = 0; j < pVTtemp_this->ComponentSize(); j++)
      {
    CNodalValueSet *pTargetValSet = new CNodalValueSet(target);
    for (k = 0; k < PointSize(); k++)
    {
          pTargetValSet->Value(k, pVTtemp_this->Component(j).ScalarData().ValuePoint(PointAt(k)));
    }
    if (pVTtemp_source)
    {
          for (k = PointSize(); k < ptset.PointSize(); k++)
          {
      pTargetValSet->Value(k, pVTtemp_source->Component(j).ScalarData().ValuePoint(ptset.PointAt(k)));
          }
    }
    pVT->Component(j).LinkTo(*pTargetValSet);
      }
  }
  }
}

// the following function merges the this pointset with the ptset pointset, resulting in 
// target pointset
void CPointSet::Merge(CPointSet &target, const CPointSet &ptset) const
{
  // waij TFS 92430 
  target.pointSetType( this->pointSetType() );

//	QString strName;
//	strName = Name() + "_merged_" + ptset.Name();
//	target.Name(strName);
  //target.Dimension(IPointSet::DIM_3D);
  

  /*
  for(i = 0; i < PointSize(); i++)
  {
    target.PushBack(PointAt(i), std::vector<double>());
    total.insert(PointAt(i));
  }

  for(i = 0; i < ptset.PointSize(); i++)
  {
    pr_insert2 = total.insert(ptset.PointAt(i));
    if(!pr_insert2.second)
      continue;
    target.PushBack(ptset.PointAt(i), std::vector<double>());
  }
  
  if(target_typeids.size() == 0)  // the target pointset will not have any valuetype assigned to it.
    return;
*/
  std::vector<CValueType*> vcSharedVT = SharedValueTypes(ptset);

  std::set<geo::CPoint> total; // this set is used to see if points from either of the 
                   // source pointsets are double.
  int i;
  for(i = 0; i < PointSize(); i++)
  {
    target.PushBack(PointAt(i), std::vector<double>());
    total.insert(PointAt(i));
  }

  for(i = 0; i < ptset.PointSize(); i++)
  {
    if(!(total.insert(ptset.PointAt(i))).second)
      continue;
    
    target.PushBack(ptset.PointAt(i), std::vector<double>());
  }

  if(total.size() == PointSize() + ptset.PointSize())
  {
    GetValueTypesNoAveraging(target, ptset, vcSharedVT);
    return;
  }
/*
  CValueType *pVT_this;
  CValueType *pVT_ptset;


  std::pair<std::set<geo::CPoint>::iterator, bool> pr_insert2;
  // this map is a map from a point to a vector of doubles. each double in the vector describes
  // a value for a certain valuetype->component->value. we need to remember which 
  std::map<geo::CPoint, std::vector<double> > mp_total;
  std::map<geo::CPoint, std::vector<double> >::iterator it_map;
  std::pair<std::map<geo::CPoint, std::vector<double> >::iterator, bool> pr_map;
  typedef std::map<geo::CPoint, std::vector<double> >::value_type TMapValTyp;
  for(i = 0; i < this_valuetypes.size(); i++)
  {
    pVT_this = 0;
    pVT_ptset = 0;
    mp_total.clear();
    pr_insert = target_typeids.insert(this_valuetypes[i]->TypeId());
    if(pr_insert.second)
    {
      target_typeids.erase(pr_insert.first);
      continue;
    }

    pVT_this = this_valuetypes[i];
    for(size_t j = 0; j < ptset_valuetypes.size(); j++)
    {
      if(ptset_valuetypes[j]->TypeId() == pVT_this->TypeId())
      {
        pVT_ptset = ptset_valuetypes[j];
        break;
      }
    }
    assert(pVT_this && pVT_ptset);
    assert(pVT_this->TypeId() == pVT_ptset->TypeId());
    
    for(j = 0; j < PointSize(); j++)
    {
      std::vector<double> values;
      for(int k = 0; k < pVT_this->ComponentSize(); k++)
      {
        values.push_back(pVT_this->Component(k).ValuePoint(PointAt(j)).Value());
      }
      mp_total.insert(TMapValTyp(PointAt(j), values));
    }

    for(j = 0; j < ptset.PointSize(); j++)
    {
      std::vector<double> values;
      it_map = mp_total.find(ptset.PointAt(i));
      if(it_map != mp_total.end()) // this does not look ok! shouldn't it be == iso != ??
      {
        for(int k = 0; k < pVT_ptset->ComponentSize(); k++)
        {
          values.push_back(pVT_ptset->Component(k).ValuePoint(ptset.PointAt(j)).Value());
        }
        mp_total.insert(TMapValTyp(ptset.PointAt(j), values));
      }
      else
      {
        std::vector<double> current = (*it_map).second;
        for(int k = 0; k < current.size(); k++)
        {
          double val = current[k];
          current[k] = (current[k] + pVT_ptset->Component(k).ValuePoint(ptset.PointAt(j)).Value()) / 2.0;
        }
      }
  
    }

    // now we have a map with points and vectors. The vectors contain the values for each component of the valuetype.
    // So, now we can fill the target pointset and add the valuetype.
    CValueType *pRet;
    CFemAppModel &model = const_cast<CFemAppModel&>(Model());
    CValueTypeFactory fact(model);
    
    pRet = fact.BuildValueType(target, pVT_this->TypeId(), pVT_this->TypeName());


    int nComponents = (*mp_total.begin()).second.size();
    for(j = 0; j < nComponents; j++)
    {
      CNodalValueSet *pTargetValSet = new CNodalValueSet(target);
      for(it_map = mp_total.begin(); it_map != mp_total.end(); it_map++)
      {
        std::vector<double> values = (*it_map).second;
        pTargetValSet->Value(j, values[j]);
      }
      pRet->Component(i).LinkTo(*pTargetValSet);
    }
  }
  */
}

// wjrx mantis 3186
// The bounding box of the pointset 'this' is subdivided into blocks.
// All values of each type in each block are averaged into single values.
// For each block, a single point is created and and assembled into
// sampledPointSet.
//
bool CPointSet::DoSample(CPointSet &sampledPointSet, double blockSize_north, double blockSize_east, double blockSize_depth, IProgressBase &progress)
{
  progress.StatusMessage("Initialise point set sampling");
  geo::CPoint bbox_min, bbox_max;
  BBox(bbox_min, bbox_max);
  geo::CPoint size(bbox_max.X() - bbox_min.X(), bbox_max.Y() - bbox_min.Y(), bbox_max.Z() - bbox_min.Z());

  unsigned int blocksXcount = 1;
  unsigned int blocksYcount = 1;
  unsigned int blocksZcount = 1;

  // Dimension() is caught in the GUI
  if ( Dimension() < 2 ) return false;

  if (blockSize_north > 0) blocksXcount = size.X() / blockSize_north + 1;
  if (blockSize_east  > 0) blocksYcount = size.Y() / blockSize_east  + 1;
  if (blockSize_depth > 0) blocksZcount = size.Z() / blockSize_depth + 1;

  // a 2d pointset should have one block in z direction
  assert((Dimension() == 2 ? blocksZcount==1 : true ));

  progress.AddSteps( 2+ValueSetSize()+ blocksXcount*blocksYcount );
  QString message = QString("Initialise conversion of %1 points to %2 blocks").arg(PointSize()).arg(blocksXcount * blocksYcount * blocksZcount);
  progress.StatusMessage(message);

  progress.Step();

  // Accumulated CValue-s + counter for each addition
  typedef std::vector< std::vector<geo::CValue> > TVecValues;
  unsigned int nodalValueSetSize = NodalValueSetSize();

  typedef std::vector<TVecValues>   TVec1DValues;
  typedef std::vector<TVec1DValues> TVec2DValues;
  typedef std::vector<TVec2DValues> TVec3DValues;
  TVec3DValues values_per_block(blocksXcount, TVec2DValues(blocksYcount, TVec1DValues(blocksZcount, TVecValues(nodalValueSetSize, std::vector<geo::CValue>()))));

  progress.Step();
  message = QString("Sampling  %1 points to %2 blocks").arg(PointSize()).arg(blocksXcount * blocksYcount * blocksZcount);
  progress.StatusMessage(message);

  for (int jj = 0; jj < ValueSetSize(); ++jj)
  {
  CValueType *p_valueTypeSampled = 0;

  IValueSet &valueSetOrig = ValueSet(jj);
  CNodalValueSet *p_nodalValueSetOrig = dynamic_cast<CNodalValueSet *>(&valueSetOrig);
  CRpnValueSet *p_rpnValueSetOrig = dynamic_cast<CRpnValueSet *>(&valueSetOrig);
  // Attention programmer: add new valueset type here
  assert(p_nodalValueSetOrig != 0 || p_rpnValueSetOrig != 0);

  std::set<IValueComponentBase *> valueTypeSet = valueSetOrig.Links<IValueComponentBase>();

  // p_valueTypeOrig = valueType in original pointset
  CValueType *p_valueTypeOrig= 0;
  for (std::set<IValueComponentBase *>::iterator it = valueTypeSet.begin(); it != valueTypeSet.end(); ++it)
  {
      IValueComponentBase &ivc = **it;
      IValueComposite &parent = ivc.Parent();
      p_valueTypeOrig = dynamic_cast<CValueType *>(&parent);
      if (p_valueTypeOrig)
    break;
  }

  // Create valueType for sampled point set.
  if (p_valueTypeOrig && dynamic_cast<CCoordinateType *>(p_valueTypeOrig) == 0)
  {
      p_valueTypeSampled = CValueTypeFactory::instance()->BuildValueType(sampledPointSet, p_valueTypeOrig->TypeId(), p_valueTypeOrig->Name());
  }

  // Handle valueSet by type
  if (p_nodalValueSetOrig) // It is a NodalValueSet
  {
      if (dynamic_cast<CCoordinateType *>(p_valueTypeOrig) == 0)
      {
    sampledPointSet.AddNodalValueSet();
    CNodalValueSet& newNodalValueSet = sampledPointSet.NodalValueSet(sampledPointSet.NodalValueSetSize() - 1);
    newNodalValueSet.Unit(p_nodalValueSetOrig->Unit());

    if (p_valueTypeSampled)
          newNodalValueSet.LinkTo(p_valueTypeSampled->Component());
      }

      for (int ii = 0; ii < PointSize(); ++ii)
      {
    const geo::IPoint &pt= PointAt(ii);

    // Determine in which subspace block pt can be found
    // x,y,z: values_per_block indexes
    unsigned int x = (unsigned int )((pt.X() - bbox_min.X()) / blockSize_north);
    unsigned int y = (unsigned int )((pt.Y() - bbox_min.Y()) / blockSize_east);
    unsigned int z = (unsigned int )((pt.Z() - bbox_min.Z()) / blockSize_depth);

    geo::CValue value = p_nodalValueSetOrig->Value(ii);

    values_per_block[x][y][z][jj].push_back(value); //accumulate value
      } // all points
  } // nodalValueSet
  else if (p_rpnValueSetOrig) // It is a RPNValueSet
  {
      // Create new RpnValueSet, which is a copy of the original
      CRpnValueSet * p_rpnValueSetSampled = new CRpnValueSet(sampledPointSet, *p_rpnValueSetOrig);
      p_rpnValueSetSampled->Unit(p_rpnValueSetOrig->Unit());
      if (p_valueTypeSampled)
    p_rpnValueSetSampled->LinkTo( p_valueTypeSampled->Component());
  }

  try
  {
      progress.Step();
  }
  catch(CProgressCancel* p)
  {
      delete p;
      return false;
  }
  } // all valuesets

  // tell user # points in sampled pointset
  {
  int counter = 0;
  for (size_t x = 0; x < values_per_block.size(); ++x)
      for (size_t y = 0; y < values_per_block[x].size(); ++y)
    for (size_t z = 0; z < values_per_block[x][y].size(); ++z)
    {
          size_t x_coord_size = values_per_block[x][y][z][0].size();
          if (x_coord_size > 0)
      ++counter; // count x-coordinate

          // sanity check: all value set sizes should be the same
          for (size_t v = 1; v < values_per_block[x][y][z].size(); ++v)
      if (values_per_block[x][y][z][v].size() != x_coord_size)
      {
              message = QString("Found illegal number of values in block[%1][%2][%3], aborting...").arg(x).arg(y).arg(z);
              progress.StatusMessage(message);
              return false;
      }
          // sanity check: all coordinates should be valid
          for (size_t v = 0; v < Dimension(); ++v)
      for (size_t vv = 0; vv < x_coord_size; ++vv)
              if (!values_per_block[x][y][z][v][vv].Valid())
              {
        message = QString("Found invalid coordinates in block[%1][%2][%3], aborting...").arg(x).arg(y).arg(z);
        progress.StatusMessage(message);
        return false;
              }
    }

  message = QString("Selecting %1 points from %2 points").arg(counter).arg(PointSize());
  progress.StatusMessage(message);
  }

  // select values
  for (size_t x = 0; x < values_per_block.size(); ++x)
  for (size_t y = 0; y < values_per_block[x].size(); ++y)
  {
      for (size_t z = 0; z < values_per_block[x][y].size(); ++z)
      {
    size_t x_coord_size = values_per_block[x][y][z][0].size();

    if (x_coord_size > 0)
    {
          std::vector<geo::CValue> vcRow;
          vcRow.reserve(values_per_block[x][y][z].size());

          size_t candidate = x_coord_size >> 1; // we're going to select a point in the middle of the gathered values (heuristically ballpark ok as mean)

          for (size_t v = 0; v < values_per_block[x][y][z].size(); ++v)
          {
      vcRow.push_back(values_per_block[x][y][z][v][candidate]);
          }

          sampledPointSet.PushBack(vcRow);
    }
      }

      try
      {
    progress.Step();
      }
      catch (CProgressCancel* p)
      {
    delete p;
    return false;
      }
  }

  return true;
}

// wjrx mantis 3186
void CPointSet::Sample(const QString& name, const double blockSizeNorth,
  const double blockSizeEast, const double blockSizeDepth)
{
  CPointSet *ps= new CPointSet
  ( name
  , Model() 
  , 0
  , Dimension()
  );

  // waij TFS 92430
  ps->pointSetType(pointSetType());
  ps->CoordinateUnit(CoordinateUnit());
  ps->ValueUnit(ValueUnit());

  std::auto_ptr <IProgressBase> progress(_g->prog()->create(eProgress::Geo, "Poinset sampling"));
  if ( DoSample
         ( *ps
         , blockSizeNorth
         , blockSizeEast
         , blockSizeDepth
         , *progress
         ) == false ) delete ps;
}

void CPointSet::Export(const IExportFormat::CExportArg& arg,
  const QString& fileName)
{
      CNodalExportFormat format(*this, ValueTypes(), arg);
      if(fileName.right(4).compare(".xls", Qt::CaseInsensitive) == 0 || fileName.right(5).left(4).compare(".xls", Qt::CaseInsensitive) == 0)
      {
        CElementSetExcelFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create(eProgress::Geo, fileName));
          file.Write(fileName.toStdString().c_str(), *prog);
        } catch (CProgressCancel* p)
        {
          delete p;
        }
      }
      else
      {
        CElementSetFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create(
      eProgress::Geo, ""));
          file.Save(fileName, *prog);
        } catch (CProgressCancel* p)
        {
          delete p;
          file.Close();
        }
      }
}


bool CPointSet::PrepareMapping(const geo::IElementSet *)
{
  if (!HasHull())
  CreateHull();
  m_pValueType->PrepareMapping();

  return true;
}
