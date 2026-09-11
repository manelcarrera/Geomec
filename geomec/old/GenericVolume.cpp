// GenericVolume.cpp: implementation of the CGenericVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "attripointsetdlg.h"
#include "ELementSet.h"
#include "ValueTypeFactory.h"
#include "GenericVolume.h"
#include "IPointSet.h"
#include "BaseEntryTypes.h"
#include "ElementValueSet.h"
#include "ModelBase.h"
#include "ProgressDlg.h"
#include "EclipseBody.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenericVolume::CGenericVolume(geo::CMesh3D &Volume, const CString &sName, 
                 CGraphModel &model, 
                 CQuantity::UNIT coordinate_unit,
                 CQuantity::UNIT value_unit,
                 eElementType type)
:IElementSet(sName, model, coordinate_unit, value_unit), m_eElType(type)
{
  TPointSetEntry *pEntry = (TPointSetEntry*)(model.GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

CGenericVolume::CGenericVolume(const CGoCadVolume& volume, CGraphModel &model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit)
:IElementSet(volume.Header().Name().c_str(), model, coordinate_unit, value_unit)
{
  ASSERT(volume.ElementSize() > 0);
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
        int nIndex = m_mesh.RegisterPoint(volume.Element(i).Point(j), false);
        it = mpPoint.insert(std::pair<int, int>(volume.Element(i).PointIndex(j), nIndex)).first;
      }
      
      vcIndex[j] = it->second;
    }

    VERIFY(i == OnCreateElement(vcIndex));
  }

  // Add the properties
  for(int nProperty = 0; nProperty < volume.Header().PropertySize(); nProperty++)
  {
    // Create an element pointset
    CElementValueSet *pValueSet = new CElementValueSet(volume.Header().Property(nProperty).Name().c_str(),
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
  
  TPointSetEntry *pEntry = (TPointSetEntry*)(model.GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

CGenericVolume::CGenericVolume(const CString& sName, CGraphModel &model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit)
: IElementSet(sName, model, coordinate_unit, value_unit)
{
  TPointSetEntry *pEntry = (TPointSetEntry*)(model.GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

CGenericVolume::CGenericVolume(const CGenericVolume& rhs)
: IElementSet(rhs), m_eElType(rhs.m_eElType)
{
  // Initialize the mesh with the nodes
  for(int i = 0; i < rhs.m_mesh.PointSize(); i++)
  {
    VERIFY(i == m_mesh.RegisterPoint(rhs.m_mesh.Point(i), false));
  }

  // Copy the elements
  for(i = 0; i < rhs.m_mesh.ElementSize(); i++)
  {
    // Get point indices of the element
    ASSERT(rhs.m_mesh.Element(i).IndexingElementSet() == &rhs.m_mesh);
    std::vector<int> vcIndex(rhs.m_mesh.Element(i).NrOfPoints());
    for(int j = 0; j < rhs.m_mesh.Element(i).NrOfPoints(); j++)
      vcIndex[j] = rhs.m_mesh.Element(i).PointIndex(j);

    VERIFY(i == OnCreateElement(vcIndex));
  }
}
/*
int CGenericVolume::CreateElement(const std::vector<const geo::IPoint*> &vcPoints)
{
  geo::IElement *pElement;
  //int nIndex = m_mesh.ElementSize();

  switch(m_eElType)
  {
  case HEXA:
    pElement = new geo::CHexahedron(m_mesh, vcPoints);
    break;
  case TETRA:
    {
      const geo::CPoint &p1 = *vcPoints[0];
      const geo::CPoint &p2 = *vcPoints[1];
      const geo::CPoint &p3 = *vcPoints[2];
      const geo::CPoint &p4 = *vcPoints[3];
      if(geo::ITetrahedron::CheckOrientation(p1, p2, p3, p4))
        pElement = new geo::CTetrahedron(m_mesh, vcPoints);
      else
        pElement = new geo::CTetrahedron(m_mesh, *vcPoints[0], *vcPoints[1], *vcPoints[3], *vcPoints[2]);
    }
    break;
  default:
    assert(false);
  }

  return pElement->Index();
}
*/
int CGenericVolume::OnCreateElement(const std::vector<int>& vcIndex)
{
  geo::IElement *pElement;
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
  default:
    assert(false);
  }

  return pElement->Index();
}

CGenericVolume::CGenericVolume(CGraphModel &model)
:IElementSet(model), m_eElType(UNKNOWN)
{
}

CGenericVolume::CGenericVolume(const CGenericVolume& volume, CModelBase& model, TPROGRESS& progress, std::map<const CValueType*, CValueType*>& mpValueType)
: IElementSet(model)
{
}

CGenericVolume::CGenericVolume(const CEclipseFile &file, CModelBase &model)
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
            vcPoints[i] = Mesh().RegisterPoint(cell.Point(i));
          }

          new CEclipseBody(Mesh(), vcPoints);
        }
        else // the cell is not added so, this is cell that pinches out
          ptset.insert(geo::CPoint(X, Y, Z));
      }
    }
  }

  for(int nProperty = 0; nProperty < file.PropertySize(); nProperty++)
  {
    
    CElementValueSet *pValueSet = new CElementValueSet(file.PropertyName(nProperty).c_str(), CQuantity::SI_UNIT, *this);
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
    CValueTypeFactory factory(model);
    CValueType *pType = factory.BuildValueType(*this, file.PropertyName(nProperty).c_str(), file.PropertyName(nProperty).c_str());
    if(pType)
    {
      pType->Component().LinkTo(*pValueSet);
    }
  }
  Modified();

  Name(_T("Eclipse cells"));
  Color(RGB(255, 0, 0));

  TPointSetEntry *pEntry = (TPointSetEntry*)(model.GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

CGenericVolume::~CGenericVolume()
{

}

void CGenericVolume::AppendContextMenu(CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("&Export"),*(new CSingleCommandTemplate<CGenericVolume>(*this,&CGenericVolume::Export)));
  invoker.AddSeparator();
  IElementSet::AppendContextMenu(invoker);
}

void CGenericVolume::ElementType(eElementType type)
{
  m_eElType = type;
}

bool CGenericVolume::operator==(const CGenericVolume& rhs) const
{
  return IElementSet::operator ==(rhs);
}

CGenericVolume& CGenericVolume::operator=(const CGenericVolume& rhs)
{
  IElementSet::operator =(rhs);
  m_mesh.Clear();

  // Initialize the mesh with the nodes
  for(int i = 0; i < rhs.m_mesh.PointSize(); i++)
  {
    VERIFY(i == m_mesh.RegisterPoint(rhs.m_mesh.Point(i), false));
  }

  // Copy the elements
  for(i = 0; i < rhs.m_mesh.ElementSize(); i++)
  {
    // Get point indices of the element
    ASSERT(rhs.m_mesh.Element(i).IndexingElementSet() == &rhs.m_mesh);
    std::vector<int> vcIndex(rhs.m_mesh.Element(i).NrOfPoints());
    for(int j = 0; j < rhs.m_mesh.Element(i).NrOfPoints(); j++)
      vcIndex[j] = rhs.m_mesh.Element(i).PointIndex(j);

    VERIFY(i == OnCreateElement(vcIndex));
  }
  return *this;
}

const geo::IElementSet &CGenericVolume::ElementSet() const
{
  return m_mesh;
}

geo::IElementSet &CGenericVolume::ElementSet()
{
  return m_mesh;
}

UINT CGenericVolume::IconId() const
{
  return IDI_ELEMENT_SET;
}

UINT CGenericVolume::TypeId() const
{
  return IDT_ELEMENTSET;
}

CString CGenericVolume::TypeName() const
{
  CString str;
  str.LoadString(IDS_ELEMENTSET);
  return str;
}

int CGenericVolume::DisplayListSize() const
{
  return 1;
}

const geo::IObject& CGenericVolume::DisplayList(int nIndex) const
{
  assert(nIndex == 0);
  assert(m_mesh.ElementSize() > 0);
  return m_mesh;
}

int CGenericVolume::ElementPointSize() const
{
  switch(m_eElType)
  {
  case HEXA:
    return 8;
  case TETRA:
    return 4;
  case UNKNOWN:
    return -1;
  default:
    ASSERT(FALSE);
    break;
  }

  return -1;
}

IPointSet::DIMENSION CGenericVolume::Dimension() const
{
  return IPointSet::DIM_3D;
}

BOOL CGenericVolume::Empty() const
{
  return !(m_mesh.ElementSize() > 0);
}

bool CGenericVolume::Properties()
{
  CAttriPointSetTemplate<CGenericVolume> dlg(*this);
  return dlg.DoModal() == IDOK;
}

void CGenericVolume::LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress)
{
  // Load point data first
  int nPointSize;
  stream >> nPointSize;
  for(int i = 0; i < nPointSize; i++)
  {
    double x, y, z;
    stream >> x;
    stream >> y;
    stream >> z;
    VERIFY(i == m_mesh.RegisterPoint(geo::CPoint(x, y, z), false));
    progress.Step();
  }

  // Load element type
  int nElementType;
  stream >> nElementType;
  m_eElType = (eElementType)nElementType;

  // Load element data
  int nElementSize;
  stream >> nElementSize;
  for(i = 0; i < nElementSize; i++)
  {
    // Load element data
    int nPointSize;
    stream >> nPointSize;
    std::vector<int> vcPoint(nPointSize);
    for(int j = 0; j < nPointSize; j++)
    {
      stream >> vcPoint[j];
    
    }
    progress.Step();
    VERIFY(i == OnCreateElement(vcPoint));
  }

  IElementSet::LoadStream(stream, version, progress);

  TPointSetEntry *pEntry = (TPointSetEntry*)(Model().GraphEntry(MD_BASE_POINTSET));
  LinkTo(*pEntry);
}

void CGenericVolume::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save points
  int nPointSize = m_mesh.PointSize();
  stream << nPointSize;
  for(int i = 0; i < PointSize(); i++)
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

long CGenericVolume::SavedItems() const
{
  long lRet = IElementSet::SavedItems();
  lRet += m_mesh.PointSize();
  lRet += m_mesh.ElementSize();
  return lRet;
} 

void CGenericVolume::ZAxis(enum Z_AXIS axis)
{
  if(IPointSet::ZAxis() != axis)
  {
    for(int i = 0; i < m_mesh.PointSize(); i++)
    {
      m_mesh.Point(i, geo::CPoint(m_mesh.Point(i).X(), m_mesh.Point(i).Y(), -m_mesh.Point(i).Z()));	
    }
  }

  IElementSet::ZAxis(axis);
}

bool CGenericVolume::CanConnectItem(const CGraphNode& item) const
{
  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  if(pValueType)
    return true;

  return IElementSet::CanConnectItem(item);
}

bool CGenericVolume::ConnectItem(const CGraphNode& item)
{
  ASSERT(CanConnectItem(item));

  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  if(pValueType)
  {
    // Do we have overlap on the other pointset?
    if(Overlap(pValueType->PointSet()))
    {
      // Do the dialog
      CString sSource = "point set";
      if(dynamic_cast<const IElementSet*>(&pValueType->PointSet()))
        sSource = "element set";
      CString sMessage;
      sMessage.Format("Convert component %s of %s %s to element set %s.", 
              pValueType->Name(), 
              sSource, 
              pValueType->PointSet().Name(),
              Name());


      if(AfxMessageBox(sMessage, MB_YESNO) == IDYES)
      {
        // Create dialog
        CProgressDlg progress;
        progress.StartDialog();

        // Create property
        CValueTypeFactory factory(Model());
        CValueType *pCopy = factory.BuildValueType(*this, pValueType->TypeId(), pValueType->Name());
        ASSERT(pCopy);

        // Copy the property
        for(int nComponent = 0; nComponent < pCopy->ComponentSize(); nComponent++)
        {
          CElementValueSet *pValueSet = new CElementValueSet(*this);
          pValueSet->LinkTo(pCopy->Component(nComponent));

          // Walk over elements
          for(int nElement = 0; nElement < ElementSet().ElementSize(); nElement++)
          {
            pValueSet->PushBack(pValueType->Component(nComponent).ValueElement(ElementSet().Element(nElement)));
          }

          progress.OnProgress((WPARAM)SET_PROGRESS, (LPARAM)(nElement / (ElementSet().ElementSize() * 0.01)));
        }

        Modified();

        progress.DestroyWindow();
      }
        
      return true;
    }
    else
      AfxMessageBox("Element set do not overlap");
  }

  return ConnectItem(item);
}


