// ElementSet.cpp: implementation of the CElementSet class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "BaseEntryTypes.h"
#include "ElementValueSet.h"
#include "ElementSet.h"
#include "PropertyExcel.h"
#include "PropertyFile.h"
#include "EclipseBody.h"
#include "ValueTypeFactory.h"
#include "ExportFormat.h"
#include "FvGocadVolume.h"
#include "FvEclipseFile.h"
#include "BoundaryBase.h"
#include "NewArchiveStdStringStream.h"
#include "resourceIDS.h"
#include "GeomecStringTable.h"
#include "StreamVersion.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "Hexahedron.h"
#include "ITetrahedron.h"
#include "Tetrahedron.h"
#include "resourceIDI.h"
#include "GlobalMessage.h"
#include "ICacheInterface.h"
#include "CacheInterfaceDefault.h"
#include "ModelBase.h"
#include "MPKernel.h"
#include "ValueMapper.h"

#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IElementSet::IElementSet(const QString& sName, CFemAppModel& model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit)
: IPointSet(sName, model, coordinate_unit, value_unit)
{
}

IElementSet::IElementSet(CFemAppModel& model)
: IPointSet(model)
{
}

IElementSet::IElementSet(const IElementSet& rhs)
: IPointSet(rhs)
{
  // Make value set copies ....
  for(int i = 0; i < rhs.ElementValueSetSize(); i++)
    new CElementValueSet(*rhs.m_vcElementValueSet[i]);

  assert(rhs.ElementValueSetSize() == ElementValueSetSize());
}

IElementSet::~IElementSet()
{
  // Kill valuesets ....
  std::vector<CGraphNode*> vcValueSet;

  for(int i = 0; i < ElementValueSetSize(); i++)
  {
    if(ElementValueSet(i).IsCopy() == IsCopy())
    {
      vcValueSet.push_back(&ElementValueSet(i));
    }
  }

  for(size_t i = 0; i < vcValueSet.size(); i++)
  {
    CGraphNode *pNode = vcValueSet[i];
    delete pNode;
  }
}

bool IElementSet::PointInConvexHull(const geo::IPoint& pt) const
{
  return ElementSet().ElementsAt(pt).size() > 0;
}

int IElementSet::DisplayListSize() const
{ 
  return 1;
}

const geo::IObject& IElementSet::DisplayList(int nIndex) const
{
  assert(nIndex == 0);
  return ElementSet();
}

int IElementSet::ValueSetSize() const
{
  return IPointSet::ValueSetSize() + ElementValueSetSize();
}

const IValueSet& IElementSet::ValueSet(int nIndex) const
{
  if(nIndex < IPointSet::ValueSetSize())
    return IPointSet::ValueSet(nIndex);

  return ElementValueSet(nIndex - NodalValueSetSize());
}

IValueSet& IElementSet::ValueSet(int nIndex)
{
  if(nIndex < IPointSet::ValueSetSize())
    return IPointSet::ValueSet(nIndex);

  return ElementValueSet(nIndex - IPointSet::ValueSetSize());
}

int IElementSet::ElementValueSetSize() const
{
  return m_vcElementValueSet.size();
}

const CElementValueSet& IElementSet::ElementValueSet(int nIndex) const
{
  assert(nIndex > - 1);
  assert(nIndex < m_vcElementValueSet.size());

  if(IsCopy())
    return (const CElementValueSet&) m_vcElementValueSet[nIndex]->Copy();

  return *m_vcElementValueSet[nIndex];
}


CElementValueSet& IElementSet::ElementValueSet(int nIndex)
{
  assert(nIndex > - 1);
  assert(nIndex < m_vcElementValueSet.size());

  if(IsCopy())
    return (CElementValueSet&) m_vcElementValueSet[nIndex]->Copy();

  return *m_vcElementValueSet[nIndex];
}

int IElementSet::AddElementValueSet()
{
  int nIndex = ValueSetSize();

  // Create new name
  QString sName;
  QString stringTableEntry = getStringTableEntry(IDS_PROPERTY_NAME);
  sName = QString(stringTableEntry).arg(nIndex);
  new CElementValueSet(sName, CQuantity::SI_UNIT, *this);

  return nIndex;	
}

IElementSet::TWeightMap IElementSet::WeightFactors(int /*nNode*/, TWeightingType /*nWeightingType*/, const CFormationBase* /*pFormation*/) const
{
  return TWeightMap();
}

bool IElementSet::operator==(const IElementSet &rhs) const
{
  if (!IPointSet::operator ==(rhs))
    return false;

  // Validate Nodal value sets
  for(int i = 0; i < ElementValueSetSize(); i++)
  {
    assert(ElementValueSet(i).IsCopied());
    if(!((CElementValueSet&)ElementValueSet(i) == ((CElementValueSet&)ElementValueSet(i).Copy())))
      return false;
  }

  return true;
}

IElementSet& IElementSet::operator=(const IElementSet& rhs)
{
  // The valuesets which have no copy are deleted from the copy 
  // and now from the source ...
  int i;
  for(i = 0; i < ElementValueSetSize(); i++)
  {
    assert(!ElementValueSet(i).IsCopy());
    if(!ElementValueSet(i).IsCopied())
      delete &ElementValueSet(i);
  }

  IPointSet::operator=(rhs);

  m_vcElementValueSet = rhs.m_vcElementValueSet;

  // Validate all
  for(i = 0; i < ElementValueSetSize(); i++)
  {
    assert(ElementValueSet(i).IsCopied());
    ElementValueSet(i) = ((CElementValueSet&) ElementValueSet(i).Copy());
  }

  return *this;
}

void IElementSet::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  // New from version 3.0.11 saving of element value sets
  if(CStreamVersion(3, 0, 10) < version)
  {
    int nValueSetSize;
    stream >> nValueSetSize;
    for(int i = 0; i < nValueSetSize; i++)
    {
      CElementValueSet *pValueSet = new CElementValueSet(*this);
      pValueSet->LoadStream(stream, version, progress);
    }
    assert(nValueSetSize == ElementValueSetSize());
  }

  // In the versions before 3.0.5 of nothing was save in IPointSet, but there is implementation
  // now. So refer to the base of IPointSet
  if(version < CStreamVersion(3, 0, 5))
    CColorNode::LoadStream(stream, version, progress);
  else
    IPointSet::LoadStream(stream, version, progress);
}

void IElementSet::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
  // Save the element valueset size
  stream << ElementValueSetSize();
  for(int i = 0; i < ElementValueSetSize(); i++)
  {
    ElementValueSet(i).FileIndex(NodalValueSetSize() + RpnValueSetSize() + i + 1);
    ElementValueSet(i).SaveStream(stream, progress);
  }

  // Save base first
  IPointSet::SaveStream(stream, progress);
}

long IElementSet::SavedItems() const
{
  long lRet = IPointSet::SavedItems();
  for(int i = 0; i < ElementValueSetSize(); i++)
  {
    lRet += ElementValueSet(i).SavedItems();
  }

  return lRet;
}

void IElementSet::OnNeighbourDeleted(const CGraphNode &item) 
{
  IPointSet::OnNeighbourDeleted(item);

  // Search vector for nodal valuesets
  for(size_t i = 0; i < m_vcElementValueSet.size(); i++)
  {
    if(m_vcElementValueSet[i] == &item)
    {
      // Item found remove and exit
      m_vcElementValueSet.erase(m_vcElementValueSet.begin() + i);
      return;
    }
  }
}

bool IElementSet::CanAssignToBoundary() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return model.Boundary().CanAssignElementSet(*this);
}

// wjrx improved zoom-in mantis 2421
//
// Assign /Data Storage/Point_Element sets/3D/<a captured scaling box>/
// /Properties/D?_L D values to 
// /Tetrahedron/Boundary/Boundary Conditions/Depletion Stage ?
// Where ? is the number of the depletion stage.
//
// Likewise, U values are assigned to Prescribed Displacement
//
void IElementSet::AssignToBoundary()
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  model.Boundary().AssignElementSet(*this);
}

void IElementSet::Export(const IExportFormat::CExportArg& arg, const QString& fileName)
{
      CElementSetExportFormat format(*this, ValueTypes(), arg);
      if(fileName.right(4).compare(".xls", Qt::CaseInsensitive) == 0 || fileName.right(5).left(4).compare(".xls", Qt::CaseInsensitive) == 0)
      {
        
        CElementSetExcelFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create(eProgress::Geo, fileName));
          file.Write(fileName.toStdString().c_str(), *prog);
        } catch (CProgressCancel* p) {
          delete p;
          // TODO : Terminate excel file
        }

      }
      else
      {
        CElementSetFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create(eProgress::Geo, ""));
          file.Save(fileName, *prog);
        } catch (CProgressCancel* p)
        {
          delete p;
          file.Close();
        }
      }
}

bool IElementSet::PrepareMapping(const geo::IElementSet *)
{
  return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementSet::CLineElement::CLineElement(geo::IMesh& mesh, int nPoint1, int nPoint2)
: m_mesh(mesh)
{
  m_nodes[0] = nPoint1;
  m_nodes[1] = nPoint2;
  m_nIndex = mesh.RegisterElement(*this);
}

int CElementSet::CLineElement::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_nodes[nIndex];
}

size_t CElementSet::CLineElement::NrOfNodes() const
{
  return 2;
}

const geo::INode& CElementSet::CLineElement::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_mesh.Node(PointIndex(nIndex));
}

void CElementSet::CLineElement::Node(int nIndex, const geo::IPoint& point)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  m_mesh.Node(PointIndex(nIndex), point);
}

const geo::IElementSet* CElementSet::CLineElement::IndexingElementSet() const
{
  return &m_mesh;
}

int CElementSet::CLineElement::Index() const
{
  return m_nIndex;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementSet::CRectangleElement::CRectangleElement(geo::IMesh& mesh, int nPoint1, int nPoint2, int nPoint3, int nPoint4)
: m_mesh(mesh)
{
  m_nodes[0] = nPoint1;
  m_nodes[1] = nPoint2;
  m_nodes[2] = nPoint3;
  m_nodes[3] = nPoint4;
  m_nIndex = mesh.RegisterElement(*this);
}

int CElementSet::CRectangleElement::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_nodes[nIndex];
}

size_t CElementSet::CRectangleElement::NrOfNodes() const
{
  return 4;
}

const geo::INode& CElementSet::CRectangleElement::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_mesh.Node(PointIndex(nIndex));
}

void CElementSet::CRectangleElement::Node(int nIndex, const geo::IPoint& point)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  m_mesh.Node(PointIndex(nIndex), point);
}

const geo::IElementSet* CElementSet::CRectangleElement::IndexingElementSet() const
{
  return &m_mesh;
}

int CElementSet::CRectangleElement::Index() const
{
  return m_nIndex;
}

CElementSet::CElementSet(const QString &sName, 
             eElementType type,
             const TPointVec& vcPoint,
             const TElementVec& vcElement,
             CFemAppModel &model)
:IElementSet(sName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_eElType(type)
{
  // Register all points
  for(size_t i = 0; i < vcPoint.size(); i++)
    m_mesh.RegisterNode(vcPoint[i]);

  for(int i = 0; i < vcElement.size(); i++)
    VERIFY(i == CreateElement(vcElement[i]));

  assert(model.GraphEntry(MD_BASE_POINTSET));
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

CElementSet::CElementSet(const CGoCadVolume& volume, CFemAppModel &model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit)
:IElementSet(volume.Header().Name(), model, coordinate_unit, value_unit)
{
  assert(volume.ElementSize() > 0);
  switch(volume.Element(0).NrOfPoints())
  {
  case 4:
    m_eElType = TETRA;
    break;
  case 8:
    m_eElType = HEXA;
    break;
  }

  // Register the elements
  std::map<int, int> mpPoint;
  for(int i = 0; i < volume.ElementSize(); i++)
  {
    // Get point indices of the element
    std::vector<int> vcIndex(volume.Element(i).NrOfPoints());
    for(int j = 0; j < volume.Element(i).NrOfPoints(); j++)
    {
      // Need to register?
      std::map<int, int>::iterator it = mpPoint.find(volume.Element(i).PointIndex(j));
      if(it == mpPoint.end())
      {
        int nIndex = m_mesh.RegisterNode(volume.Element(i).Point(j), false);
        it = mpPoint.insert(std::pair<int, int>(volume.Element(i).PointIndex(j), nIndex)).first;
      }
      
      vcIndex[j] = it->second;
    }

    VERIFY(i == CreateElement(vcIndex));
  }

  // Add the properties
  for(int nProperty = 0; nProperty < volume.Header().PropertySize(); nProperty++)
  {
    // Create an element pointset
    CElementValueSet *pValueSet = new CElementValueSet(volume.Header().Property(nProperty).Name(),
                               CQuantity::SI_UNIT,
                               *this);
    for(int i = 0; i < ElementSet().ElementSize(); i++)
    {
      std::vector<geo::CValue> vcValue(ElementSet().Element(i).NrOfPoints());
      for(int j = 0; j < ElementSet().Element(i).NrOfPoints(); j++)
      {
        vcValue[j] = geo::CValue(volume.Header().Property(nProperty).Value(ElementSet().Element(i).Point(j))[0]);
      }
      VERIFY(i == pValueSet->PushBack(vcValue));
    }
  }
  
  assert(model.GraphEntry(MD_BASE_POINTSET));
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

CElementSet::CElementSet(const QString& sName, CFemAppModel &model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit)
: IElementSet(sName, model, coordinate_unit, value_unit)
{
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

CElementSet::CElementSet(const CElementSet& rhs)
: IElementSet(rhs), m_eElType(rhs.m_eElType)
{
  // Initialize the mesh with the nodes
  int i;
  for(i = 0; i < rhs.m_mesh.PointSize(); i++)
  {
    VERIFY(i == m_mesh.RegisterNode(rhs.m_mesh.Point(i), false));
  }

  // Copy the elements
  for(i = 0; i < rhs.m_mesh.ElementSize(); i++)
  {
    // Get point indices of the element
    assert(rhs.m_mesh.Element(i).IndexingElementSet() == &rhs.m_mesh);
    std::vector<int> vcIndex(rhs.m_mesh.Element(i).NrOfPoints());
    for(int j = 0; j < rhs.m_mesh.Element(i).NrOfPoints(); j++)
      vcIndex[j] = rhs.m_mesh.Element(i).PointIndex(j);

    VERIFY(i == CreateElement(vcIndex));
  }
}

int CElementSet::CreateElement(const TNodeVec& vcIndex)
{
  geo::IElement *pElement = 0;
  //int nIndex = m_mesh.ElementSize();

  switch(m_eElType)
  {
  case HEXA:
    pElement = new geo::CHexahedron(m_mesh, vcIndex);
    break;
  case TETRA:
    {
      const geo::CPoint &p1 = Mesh().Point(vcIndex[0]);
      const geo::CPoint &p2 = Mesh().Point(vcIndex[1]);
      const geo::CPoint &p3 = Mesh().Point(vcIndex[2]);
      const geo::CPoint &p4 = Mesh().Point(vcIndex[3]);
      if(geo::ITetrahedron::CheckOrientation(p1, p2, p3, p4))
        pElement = new geo::CTetrahedron(m_mesh, vcIndex);
      else
        pElement = new geo::CTetrahedron(m_mesh, vcIndex[0], vcIndex[1], vcIndex[3], vcIndex[2]);
    }
    break;
  case ECLIPSE:
    pElement = new CEclipseBody(m_mesh, vcIndex);
    break;
  case LINE_2D:
    assert(vcIndex.size() == 2);
    pElement = new CLineElement(m_mesh, vcIndex[0], vcIndex[1]);
    break;
  case TRIANGLE_2D:
  case TRIANGLE_3D:
    assert(vcIndex.size() == 3);
    pElement = new geo::CTriangle(m_mesh, vcIndex[0], vcIndex[1], vcIndex[2]);
    break;
  case QUAD_2D:
  case QUAD_3D:
    assert(vcIndex.size() == 4);
    pElement = new CRectangleElement(m_mesh, vcIndex[0], vcIndex[1], vcIndex[2], vcIndex[3]);
    break;

  default:
    assert(false);
  }

  return pElement->Index();
}

int CElementSet::RegisterPoint( const geo::IPoint& point )
{
  return m_mesh.RegisterNode(point, false);
}

CElementSet::CElementSet(CFemAppModel &model)
:IElementSet(model), m_eElType(UNKNOWN)
{
}

CElementSet::CElementSet(const CElementSet& /*volume*/, CModelBase& model, TPROGRESS& /*progress*/, std::map<const CValueType*, CValueType*>& /*mpValueType*/)
: IElementSet(model)
{
}

CElementSet::CElementSet(const CEclipseFile &file, CModelBase &model)
: IElementSet(model), m_eElType(ECLIPSE)
{
  std::set<geo::CPoint> ptset;
  for(int X = 0; X < file.SizeX(); X++)
  {
    for(int Y = 0; Y < file.SizeY(); Y++)
    {
      for(int Z = 0; Z < file.SizeZ(); Z++)
      {
        CEclipseCell cell = file.Cell(X, Y, Z);
        if(!cell.IsPinchOut())
        {
          std::vector<int> vcPoints(cell.NrOfPoints());
          for(int i = 0; i < cell.NrOfPoints(); i++)
          {
            vcPoints[i] = Mesh().RegisterNode(cell.Point(i));
          }

          new CEclipseBody(Mesh(), vcPoints);
        }
        else // the cell is not added so, this is cell that pinches out
          ptset.insert(geo::CPoint(X, Y, Z));
      }
    }
  }

  for(size_t nProperty = 0; nProperty < file.PropertySize(); nProperty++)
  {
    
    CElementValueSet *pValueSet = new CElementValueSet(file.PropertyName(nProperty), CQuantity::SI_UNIT, *this);
    for(int X = 0; X < file.SizeX(); X++)
    {
      for(int Y = 0; Y < file.SizeY(); Y++)
      {
        for(int Z = 0; Z < file.SizeZ(); Z++)
        {
          std::set<geo::CPoint>::iterator it = ptset.find(geo::CPoint(X, Y, Z)); // check to see if the current cell is one of the cells that pinches out....
          if(it == ptset.end()) 
            pValueSet->PushBack(file.PropertyValue(nProperty, X, Y, Z)); //No? Then add the appropriate property Luke!
        }
      }
    }
    const CValueTypeFactory *factory = CValueTypeFactory::instance();
    CValueType *pType = factory->BuildValueType(*this, file.PropertyName(nProperty), file.PropertyName(nProperty));
    if(pType)
    {
      pType->Component().LinkTo(*pValueSet);
    }
  }
  Modified();

  Name("Eclipse cells");
  Color(qRgb(255, 0, 0));

  TPointSetEntry *pEntry = (TPointSetEntry*)(model.GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

CElementSet::~CElementSet()
{
  m_mesh.Clear();
}

void CElementSet::ElementType(eElementType type)
{
  m_eElType = type;
}

bool CElementSet::operator==(const CElementSet& rhs) const
{
  return IElementSet::operator ==(rhs);
}

void CElementSet::OnCoordinateUnitChanged(CQuantity::UNIT old_unit, CQuantity::UNIT new_unit)
{
  CLengthQuantity lq;
  int i;
  for(i = 0; i < m_mesh.NodeSize(); i++)
  {
    const geo::IPoint &pt = m_mesh.Node(i);
    m_mesh.Node(i, geo::CPoint(lq.Convert(pt.X(), new_unit, old_unit),
                   lq.Convert(pt.Y(), new_unit, old_unit),
                   lq.Convert(pt.Z(), new_unit, old_unit)));
  }
}

CElementSet::CElementSet(const std::set<const geo::IElement*> &stElements,
             const QString &sName, 
             eElementType type,
             CFemAppModel &model)
:IElementSet(sName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_eElType(type)
{
  std::set<const geo::IElement*>::const_iterator it;
  for(it = stElements.begin(); it != stElements.end(); it++)
  {
    for(int i = 0; i < (*it)->NrOfPoints(); i++)
    {
      m_mesh.RegisterNode((*it)->Point(i));
    }
  }

  for(it = stElements.begin(); it != stElements.end(); it++)
  {
    std::vector<int> vcIndex((*it)->NrOfPoints());
    for(int i = 0; i < vcIndex.size(); i++)
      vcIndex[i] = m_mesh.NodeIndex((*it)->Point(i))[0];

    CreateElement(vcIndex);
  }
  assert(model.GraphEntry(MD_BASE_POINTSET));
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

CElementSet::CElementSet(const std::vector<const geo::IElement*> &vcElements,
             const QString &sName, 
             eElementType type,
             CFemAppModel &model)
:IElementSet(sName, model, CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_eElType(type)
{
  int j;
  for(size_t i = 0; i < vcElements.size(); i++)
  {
    for(j = 0; j < vcElements[i]->NrOfPoints(); j++)
    {
      m_mesh.RegisterNode(vcElements[i]->Point(j));
    }
  }

  for(size_t i = 0; i < vcElements.size(); i++)
  {
    std::vector<int> vcIndex(vcElements[i]->NrOfPoints());
    for(j = 0; j < vcIndex.size(); j++)
      vcIndex[j] = m_mesh.NodeIndex(vcElements[i]->Point(j))[0];

    CreateElement(vcIndex);
  }
  assert(model.GraphEntry(MD_BASE_POINTSET));
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

CElementSet& CElementSet::operator=(const CElementSet& rhs)
{
  IElementSet::operator =(rhs);
  m_mesh.Clear();

  geo::ICacheInterface &ci = m_mesh.CacheInterface();
  m_mesh.CacheInterface(geo::CCacheInterfaceDefault::Instance());
  
  // Initialize the mesh with the nodes
  int i;
  for(i = 0; i < rhs.m_mesh.PointSize(); i++)
  {
    VERIFY(i == m_mesh.RegisterNode(rhs.m_mesh.Point(i), false));
  }

  // Copy the elements
  for(i = 0; i < rhs.m_mesh.ElementSize(); i++)
  {
    // Get point indices of the element
    assert(rhs.m_mesh.Element(i).IndexingElementSet() == &rhs.m_mesh);
    std::vector<int> vcIndex(rhs.m_mesh.Element(i).NrOfPoints());
    for(int j = 0; j < rhs.m_mesh.Element(i).NrOfPoints(); j++)
      vcIndex[j] = rhs.m_mesh.Element(i).PointIndex(j);

    VERIFY(i == CreateElement(vcIndex));
  }

  ci.Resize(rhs.m_mesh.ElementSize());
  m_mesh.CacheInterface(ci);

  return *this;
}

const geo::IElementSet &CElementSet::ElementSet() const
{
  return m_mesh;
}

geo::IElementSet &CElementSet::ElementSet()
{
  return m_mesh;
}

unsigned int CElementSet::IconId() const
{
  return IDI_ELEMENT_SET;
}

unsigned int CElementSet::TypeId() const
{
  return IDT_ELEMENTSET;
}

QString CElementSet::TypeName() const
{
  return getStringTableEntry(IDS_ELEMENTSET);
}

int CElementSet::ElementPointSize() const
{
  switch(m_eElType)
  {
  case HEXA:
    return 8;
  case TETRA:
    return 4;
  case UNKNOWN:
    return -1;
  case TRIANGLE_2D:
  case TRIANGLE_3D:
    return 3;
  case LINE_2D:
    return 2;
  case QUAD_2D:
  case QUAD_3D:
    return 4;
  default:
    assert(false);
    break;
  }

  return -1;
}

IPointSet::DIMENSION CElementSet::Dimension() const
{
  assert(m_eElType != UNKNOWN);

//	if( m_eElType == LINE_2D ) return DIM_1D;
  if( m_eElType == TRIANGLE_2D ||
    m_eElType == QUAD_2D ) return DIM_2D;
  return DIM_3D;
}

bool CElementSet::Empty() const
{
  return !(m_mesh.ElementSize() > 0);
}

void CElementSet::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // wjrx mantis 3459
  //
  if((version >= CStreamVersion(3,7,29) && version < CStreamVersion(3, 8, 0)) ||
     (version >= CStreamVersion(4, 0, 1)))
  {
    int val;
    stream >> val;
    m_coordinate_unit= (CDoubleQuantity::UNIT )val;
    stream >> val;
    ValueUnit((CDoubleQuantity::UNIT )val);
  }

  // Load point data first
  int nPointSize1;
  stream >> nPointSize1;
  int i;
  for(i = 0; i < nPointSize1; i++)
  {
    double x, y, z;
    stream >> x;
    stream >> y;
    stream >> z;
    VERIFY(i == m_mesh.RegisterNode(geo::CPoint(x, y, z), false));
    progress.Step();
  }

  // Load element type
  int nElementType;
  stream >> nElementType;
  m_eElType = (eElementType)nElementType;

  // Load element data
  int nElementSize;
  stream >> nElementSize;

  m_mesh.reserveElementSize(nElementSize);

  for(i = 0; i < nElementSize; i++)
  {
    // Load element data
    int nPointSize2;
    stream >> nPointSize2;
    std::vector<int> vcPoint(nPointSize2);
    for(int j = 0; j < nPointSize2; j++)
    {
      stream >> vcPoint[j];
    
    }
    progress.Step();
    VERIFY(i == CreateElement(vcPoint));
  }

  IElementSet::LoadStream(stream, version, progress);

  if (version < CStreamVersion(4, 1, 52 ))
  {
  if (CoordinateUnit() == CQuantity::FIELD_UNIT)
  {
      m_coordinate_unit = CQuantity::SI_UNIT;
  }

  if (ValueUnit(true) == CQuantity::FIELD_UNIT)
  {
      ValueUnit(CQuantity::SI_UNIT);
  }
  }

  reParent(Model().GraphEntry(MD_BASE_POINTSET));
}

void CElementSet::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // wjrx mantis 3459 CStreamVersion >= 3.7.29
  //

  stream << CoordinateUnit();
  stream << ValueUnit();

  // Save points
  int nPointSize = m_mesh.PointSize();
  stream << nPointSize;
  int i;
  for(i = 0; i < PointSize(); i++)
  {
    stream << m_mesh.Point(i).X() << m_mesh.Point(i).Y() << m_mesh.Point(i).Z();
    progress.Step();
  }

  // Save element size
  int nElementType = m_eElType;
  stream << nElementType;

  // Save element data
  stream << m_mesh.ElementSize();
  for(i = 0; i < m_mesh.ElementSize(); i++)
  {
    const geo::IElement& element = m_mesh.Element(i);
    stream << element.NrOfPoints();
    for(int j = 0; j < element.NrOfPoints(); j++)
      stream << element.PointIndex(j);
    progress.Step();
  }

  IElementSet::SaveStream(stream, progress);
}

long CElementSet::SavedItems() const
{
  long lRet = IElementSet::SavedItems();
  lRet += m_mesh.PointSize();
  lRet += m_mesh.ElementSize();
  return lRet;
} 

void CElementSet::ZAxis(enum Z_AXIS axis)
{
  if(IPointSet::ZAxis() != axis)
  {
    for(int i = 0; i < m_mesh.PointSize(); i++)
    {
      m_mesh.Node(i, geo::CPoint(m_mesh.Point(i).X(), m_mesh.Point(i).Y(), -m_mesh.Point(i).Z()));	
    }
  }

  IElementSet::ZAxis(axis);
}

bool CElementSet::CanConnectItem(const CGraphNode& item) const
{
  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  if(pValueType)
    return true;

  return IElementSet::CanConnectItem(item);
}

bool CElementSet::ConnectItem(const CGraphNode& item)
{
  assert(CanConnectItem(item));

  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
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
      sMessage = QString("Convert component %1 of %2 %3 to element set %4.").
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

        progress->AddSteps(ElementSet().ElementSize() * pCopy->ComponentSize());

        // Copy the property
        for(int nComponent = 0; nComponent < pCopy->ComponentSize(); nComponent++)
        {
          CElementValueSet *pValueSet = new CElementValueSet(*this);
          pValueSet->LinkTo(pCopy->Component(nComponent));

          // Walk over elements
          int nElement;
          for(nElement = 0; nElement < ElementSet().ElementSize(); nElement++)
          {
            pValueSet->PushBack(pValueType->Component(nComponent).ScalarData().ValueElement(ElementSet().Element(nElement)));
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

  return ConnectItem(item);
}

bool CElementSet::PrepareMapping(const geo::IElementSet *pElementSet)
{
  return CValueMapper::instance()->PrepareMappingElementSet(pElementSet, &Mesh());
}

