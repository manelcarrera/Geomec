// ImportFormat.cpp: implementation of the CImportFormat class.
//
//////////////////////////////////////////////////////////////////////
#include "ImportFormat.h"
#include "ModelBase.h"
#include "NodalValueSet.h"
#include "ElementValueSet.h"
#include "ValueTypeFactory.h"
#include "PointSet.h"
#include "MeshBase.h"
#include "ResultRegister.h"
#include "NewWellPathInput.h"
#include "UnitConversion.h"
#include "FvInputStream.h"
#include "GeomecTime.h"
#include "IPointSet.h"
#include "ElementSet.h"
#include "FieldFactor.h"
#include "IInterfaceElement.h"

//////////////////////////////////////////////////////////////////////
// Implementation of IImportFormat
//////////////////////////////////////////////////////////////////////
IImportFormat::IImportFormat(CModelBase& model) 
: m_pValueType(0), 
  m_nNextComponent(-1), 
  m_model(model)
{
  CreateAliases();
}

const CModelBase& IImportFormat::Model() const
{
  return m_model;
}

void IImportFormat::AddTime(std::string str, int year, int month)
{
  m_mpDepTimes.insert(std::map<std::string, CGeomecTime>::value_type(str, CGeomecTime(year, month)));
}

void IImportFormat::CreateAliases()
{
  if(m_ValueTypeAliases.size() == 0)
  {
  // for now we only have an alias for the delta effective stress
  m_ValueTypeAliases.insert(std::map<std::string, std::string>::value_type("Dlt_EStress", "Stress"));
  m_ValueTypeAliases.insert(std::map<std::string, std::string>::value_type("Dlt_Stress", "Stress"));
  m_ValueTypeAliases.insert(std::map<std::string, std::string>::value_type("EStress", "Stress"));
  m_ValueTypeAliases.insert(std::map<std::string, std::string>::value_type("Dlt_Pressure", "Pressure"));
  m_ValueTypeAliases.insert(std::map<std::string, std::string>::value_type("Dlt_Temperature", "Temperature"));
  }
}

/*!
  Creates a value type based on the type name and puts it a map.
  \param vcType Stringlist format [TYPE] [NAME1] [NAME2] ... [UNIT]
  Throws an exception when argument list incorrect or the value type name does not exist
*/
CValueType* IImportFormat::CreateValueType(const TStringVec& vcType)
{
  if( vcType.size() == 0 )
  throw CReadException("Type of the property expected.");
  if( vcType.size() == 1 )
  throw CReadException("Name of the property expected.");
 
  QString valueType;
  if(m_ValueTypeAliases.find(vcType[0].toStdString()) != m_ValueTypeAliases.end())
  {
  std::map<std::string, std::string>::iterator it =
      m_ValueTypeAliases.find(vcType[0].toStdString());
  valueType = (it->second).c_str();
  }
  else
  {
  valueType = vcType[0].toStdString().c_str();
  }

  bool unitNameAvailable= false;
  bool invalidUnit = false;
  IUnitConversion *pConversion= NULL;
  if (  vcType.size() == 9 // tensor + unit name
     || vcType.size() == 8 // tensor
     || vcType.size() == 6 // vector + unit name
     || vcType.size() == 5 // vector
     || vcType.size() == 3 // scalar + unit name
     || vcType.size() == 2 // scalar
     )
  {
  QString valueTypeName;

  valueTypeName = getStringTableEntry(IDS_ET_FRACT_TENSOR);

  if (( vcType.size() == 9 // tensor + unit name
       || vcType.size() == 6 // vector + unit name
       || vcType.size() == 3 // scalar + unit name
       ) && (valueType != valueTypeName))
  {
      // PROPERTY Temperature "T1" C
      // valueSize= 3-2 values to be converted
      //
      // PROPERTY Displacement "D_x" "D_y" "D_z" "D" m
      // valueSize= 6-3 values to be converted
      //
      unitNameAvailable= true;
      int valueSize= vcType.size() - ( vcType.size() > 3 ? 3 : 2 );

      for (int ii= 0; ii< valueSize; ++ii)
      {
    pConversion= IUnitConversion::Create(vcType[ii+1].toStdString(), vcType[vcType.size()-1].toStdString());
    if (pConversion)
          PointSet().AddUnitConversion(vcType[ii + 1].toStdString(), pConversion);
    else
          invalidUnit = true;
      }
  }
  else
  {
      // PROPERTY Temperature "T1"
      // valueSize= 2-1 values to be converted
      //
      // PROPERTY Displacement "D_x" "D_y" "D_z" "D"
      // valueSize= 5-2 values to be converted
      //
      unitNameAvailable= false;
  }
  }

  if (unitNameAvailable && invalidUnit)
  return 0;

  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  CValueType* pRet = factory->BuildValueType(PointSet(), valueType, vcType[1]);

  if (!pRet)
  return 0;

  int conversionArg= unitNameAvailable ? 1:0;
  if( vcType.size() != pRet->ComponentSize() + 1 + conversionArg &&
      vcType.size() != pRet->ComponentSize() + 2 + conversionArg )
  {
  throw CReadException(QString("%1 type requires at least %2 name labels.")
      .arg(vcType[0]).arg(pRet->ComponentSize()));
  }
  
  for(int i = 0; i < pRet->ComponentSize(); i++) {
          if(!m_mpComponent.insert(TComponentMap::value_type(vcType[i + 1], &pRet->Component(i) )).second)
                  throw CReadException(QString("Column name %1 does already exist.").arg(vcType[i + 1]));
  }

  if( vcType.size() == pRet->ComponentSize() + 2 + conversionArg) 
  pRet->Name(vcType[pRet->ComponentSize() + 1 ] );

  return pRet;
}

void IImportFormat::AppendColumn(const QString& sName)
{
  IValueComponentBase *pComponent = 0;
  QString sValueSetName = sName;
  if(m_mpComponent.size() == 0) {
    if(m_pValueType)
    {
    // We expect a component ....
    if(m_pValueType->Component(m_nNextComponent).ExportLabel().compare(sName, Qt::CaseInsensitive) == 0)
    {
      // wjrx mantis 2778 sValueSetName.Format("Column%d", ColumnSize() + 1);
      // Create value type and link to it
      IValueSet *pValueSet = OnCreateValueSet(sName);
      assert(pValueSet);
      m_pValueType->Component(m_nNextComponent).LinkTo(*pValueSet);
      m_vcValueSet.push_back(ColumnSize());
      if((int)m_pValueType->ComponentSize() == (m_nNextComponent + 1))
      {
        // No components of this value type expected
        m_pValueType = 0;
        m_nNextComponent = 0;
      }
      else
        m_nNextComponent++;
      return ; // Handled
    }
    else
    {
      throw CReadException(QString("Expecting component %1 of %2 not %3")
                             .arg(m_nNextComponent).arg(m_pValueType->Name()).arg(sName));
    }
    }

    // Search in the factory
    const CValueTypeFactory *factory = CValueTypeFactory::instance();
    m_pValueType = factory->BuildValueType(PointSet(), sName, sName);

  if(!m_pValueType)
  {
      // allow linking with value type based on type name
      // try without depletion stage and analysis type suffix
      int idx = -1;
      int nxtidx = 0;
      do
      {
    nxtidx = sName.indexOf("_D", idx + 1);
    if(nxtidx != -1 && nxtidx < sName.length() - 2 && sName[nxtidx + 2].isDigit())
          idx = nxtidx;
      } while(nxtidx != -1);

      if(idx != -1)
      {
    QString sBaseName = sName.left(idx);
    m_pValueType = factory->BuildValueType(PointSet(), sBaseName, sName);
      }
  }

    if(m_pValueType)
    {
    //wjrx mantis 2777 // sValueSetName.Format("Column%d", ColumnSize() + 1);		
    pComponent = &m_pValueType->Component();
    if(m_pValueType->ComponentSize() == 1)
      m_pValueType = 0;
    else
      m_nNextComponent = 1;
    }
  } else {
    // Search for column
    QString sUpperName(sName);
    sUpperName = sUpperName.toUpper();
    if( m_mpComponent.find( sUpperName ) != m_mpComponent.end() ) {
      pComponent = m_mpComponent.find( sName )->second;
    }

  }		
   
  // Create a default column
  IValueSet *pValueSet = OnCreateValueSet(sValueSetName);

  //  set the correct time for the component:
  std::map<std::string, CGeomecTime>::iterator it;
  for(it = m_mpDepTimes.begin(); it != m_mpDepTimes.end(); it++)
  {
    if(sName.indexOf(it->first.c_str()) != -1)
    {
      pValueSet->Time(it->second.GetYear(), it->second.GetMonth());
      break;
    }

  }
    
  m_vcValueSet.push_back(ColumnSize());
  if(pComponent) pComponent->LinkTo(*pValueSet);
}

//////////////////////////////////////////////////////////////////////
// Implementation of INodalFormat
//////////////////////////////////////////////////////////////////////

CNodalImportFormat::CNodalImportFormat(IElementSet& element_set)
: IImportFormat((CModelBase&)element_set.Model()),
  m_nNodeIndex(-1),
  m_pElementSet(&element_set),
  m_pPointSet(0)
{
}

CNodalImportFormat::CNodalImportFormat(CPointSet& point_set)
: IImportFormat((CModelBase&)point_set.Model()),
  m_nNodeIndex(-1),
  m_pElementSet(0),
  m_pPointSet(&point_set)
{
}

    // Access to element set if any ...
IElementSet& CNodalImportFormat::ElementSet()
{
  assert(m_pElementSet);
  return *m_pElementSet;
}

const IElementSet& CNodalImportFormat::ElementSet() const
{
  assert(m_pElementSet);
  return *m_pElementSet;
}

/*!
  Appends a header string to format
*/
void CNodalImportFormat::AppendColumn(const QString& sName)
{
  if((sName.compare("Depth", Qt::CaseInsensitive) == 0) || (sName.compare("Z", Qt::CaseInsensitive) == 0)) 
    SetCoordinate(CT_DEPTH);
  else if((sName.compare("Northing", Qt::CaseInsensitive) == 0) || (sName.compare("X", Qt::CaseInsensitive) == 0))
    SetCoordinate(CT_NORTHING);
  else if((sName.compare("Easting", Qt::CaseInsensitive) == 0) || (sName.compare("Y", Qt::CaseInsensitive) == 0))
    SetCoordinate(CT_EASTING);
  else if(sName.compare("T", Qt::CaseInsensitive) == 0)
    SetCoordinate(CT_T);
  else if(sName.compare("N", Qt::CaseInsensitive) == 0)
    SetCoordinate(CT_N);
  else if(sName.compare("Index", Qt::CaseInsensitive) == 0)	{
    if(m_pElementSet)	// Index is only a relevant keyword in the elementset
    {
      if(m_nNodeIndex != -1)
        throw CReadException("Index keyword must unique in the header of the nodal section");

      m_nNodeIndex = ColumnSize();
      assert(m_nNodeIndex == (ColumnSize() - 1));
    }
  } else {
    IImportFormat::AppendColumn(sName);
  }
}

void CNodalImportFormat::SetCoordinate(COORDINATE_TYPE coordinate_type)
{
  // Do checks on the consistency of the model
  if((coordinate_type == CT_T) || (coordinate_type == CT_N))
    throw CReadException("2D elementset or pointset can not be used in 3D model");

  if((AxisSystem() == AXIS2D) && ((coordinate_type == CT_EASTING) || (coordinate_type == CT_NORTHING)))
    throw CReadException("3D label found in 2D file"); 

  if((AxisSystem() == AXIS3D) && ((coordinate_type == CT_T) || (coordinate_type == CT_N)))
    throw CReadException("2D label found in 3D file"); 

  std::set<COORDINATE_TYPE> stCoordinate;
  for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
    VERIFY(stCoordinate.insert(it->second).second);
  if(!stCoordinate.insert(coordinate_type).second)
    throw CReadException("Coordinate keyword must be unique in the header of the nodal section");

  int nColumn = ColumnSize();
  VERIFY(m_mpCoordinate.insert(TCoordinateMap::value_type(nColumn, coordinate_type)).second);

  // Process coordinates for pointset
  if(m_pPointSet)
  {
    // Create column
    QString sName;
    if(coordinate_type == CT_DEPTH) sName = getStringTableEntry(IDS_RC_DEPTH);
    if(coordinate_type == CT_NORTHING) sName = getStringTableEntry(IDS_RC_NORTHING);
    if(coordinate_type == CT_EASTING) sName = getStringTableEntry(IDS_RC_EASTING);
    IValueSet *pValueSet1 = OnCreateValueSet(sName);
    int nIndex = -1;
    for(int i = 0; i < m_pPointSet->NodalValueSetSize(); i++)
      if(&m_pPointSet->NodalValueSet(i) == pValueSet1) nIndex = i;
    assert(nIndex == ColumnSize() - 1);

    // Assign coordinates pointset if axis system is known
    if(AxisSystem() != UNKNOWN)
    {
      assert(m_pPointSet->Coordinates().ComponentSize() == 3);
      for(int i = 0; i < m_pPointSet->Coordinates().ComponentSize(); i++)
      {
        CValueComponent* pComponent = dynamic_cast<CValueComponent*>(&m_pPointSet->Coordinates().Component(i));
        assert(pComponent);
        IValueSet *pValueSet2 = 0;
        for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
        {
          if((i == 0) && (((it->second == CT_DEPTH) && (AxisSystem() == AXIS2D)) || (it->second == CT_NORTHING)))
            pValueSet2 = &m_pPointSet->NodalValueSet(it->first);
          if((i == 1) && ((it->second == CT_T) || (it->second == CT_EASTING)))
            pValueSet2 = &m_pPointSet->NodalValueSet(it->first);
          if((i == 2) && (((it->second == CT_DEPTH) && (AxisSystem() == AXIS3D)) || (it->second == CT_N)))
            pValueSet2 = &m_pPointSet->NodalValueSet(it->first);
        }

        if(pValueSet2 && !pComponent->IsLinkedTo(*pValueSet2))
          pComponent->LinkTo(*pValueSet2);
      }
    }
  }
}

bool CNodalImportFormat::IsCoordinate(COORDINATE_TYPE coordinate_type) const
{
  for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
    if(it->second == coordinate_type) return true;
  return false;
}

/*!
  Validate the nodal format section. CReadException is throwed when header is not consistent.
*/
void CNodalImportFormat::ValidateColumns() 
{
  if(AxisSystem() == UNKNOWN)
    throw CReadException("Axis system is not properly defined in the nodal header section.");
  if(m_pElementSet)
  {
    if(m_nNodeIndex == -1)
      throw CReadException("Node index is not defined for element in the nodal header section.");
  } else if(AxisSystem() == AXIS3D) {
    if(!IsCoordinate(CT_EASTING))
      throw CReadException("Coordinate \"Easting\" is missing in the nodal header section.");
    if(!IsCoordinate(CT_NORTHING))
      throw CReadException("Coordinate \"Northing\" is missing in the nodal header section.");
  } else 	{
    if(!IsCoordinate(CT_T))
      throw CReadException("Coordinate \"T\" is missing in the nodal header section.");
  }
  // Check if the announced valuetypes occur in the columns
  for ( TComponentMap::iterator it = m_mpComponent.begin(); it != m_mpComponent.end(); ++it ) {
    QString ComponentName = it->first.toStdString().c_str();
    bool exist = false;
    for ( int i = 0; i < PointSet().ValueSetSize(); ++i ) {
      const IValueSet& valueSet = PointSet().ValueSet( i );
      if ( valueSet.Name().compare( ComponentName, Qt::CaseInsensitive ) == 0 ) {
        exist = true;
        break;
      }
    }
    if ( !exist ) {
      throw CReadException( QString( "Column \"%1\" is missing in the nodal header section." ).arg( ComponentName ) );
    }
  }
}

/*!
  We validate nothing here
*/
void CNodalImportFormat::ValidateRows() 
{
  if (m_pPointSet)
  {
  m_pPointSet->CoordinateUnit(CDoubleQuantity::SI_UNIT);
  m_pPointSet->ValueUnit(CDoubleQuantity::SI_UNIT);
  m_pPointSet->RemoveAllUnitConversions();

  if (AxisSystem() != UNKNOWN)
      Model().ConvertSet(m_pPointSet, AxisSystem() == AXIS3D);

  QStringList l;
  for (size_t s = 0; s < m_pPointSet->ValueSetSize(); ++s)
  {
      if (m_pPointSet->ValueSet(s).Component() && 
      !m_pPointSet->ValueSet(s).Component()->IsInRange(m_pPointSet->ValueSet(s)))
    l.append(QString("Component \"%1\" has values out of range.").arg(m_pPointSet->ValueSet(s).Component()->Name()));
    
  }
  if (!l.empty())
      throw CReadException(l.join("\n"));
  }
}

int CNodalImportFormat::ColumnSize() const
{
  if(m_pElementSet)
  {
    int nRet = m_mpCoordinate.size();

    if(m_nNodeIndex != -1)
      nRet++;

    return nRet + m_pElementSet->NodalValueSetSize();
  }
  
  return m_pPointSet->ValueSetSize();
}

void CNodalImportFormat::AppendRow(const TValueVec &vcRow)
{
  if(vcRow.size() != ColumnSize()) {
    throw CReadException(QString("Expected %1 value; read %2 values in nodal section")
                             .arg(ColumnSize()).arg(vcRow.size()));
  }

  // Are there NANs in the coordinate section?
  for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
    if(!vcRow[it->first].Valid()) throw CReadException("NAN values in coordinate definition");

  QStringList l;

  // Okay register the points on the 
  if(m_pElementSet)
  {
    double x, y, z = 0;
    for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
    {
      switch(it->second)
      {
      case CT_NORTHING:
        x = vcRow[it->first].Value();
        break;
      case CT_T:
      case CT_EASTING:
        y = vcRow[it->first].Value();
        break;
      case CT_N:
        z = vcRow[it->first].Value();
        break;
      case CT_DEPTH:
        if(AxisSystem() == AXIS3D)
          z = vcRow[it->first].Value();
        else
          x = vcRow[it->first].Value();
        break;
      default:
        assert(false);
        break;
      }
    }

    assert(m_nNodeIndex > -1);
    if(!vcRow[m_nNodeIndex].Valid())
      throw CReadException("NAN values in node index definition");
    int nIndex = (int)vcRow[m_nNodeIndex].Value();
    int nMeshIndex;
    //if( m_pElementSet->CoordinateUnit() == CQuantity::FIELD_UNIT ) {
      //nMeshIndex = m_pElementSet->RegisterPoint(geo::CPoint(x,y,z) / FF_FACTOR_LENGTH);
    //} else {
      nMeshIndex = m_pElementSet->RegisterPoint(geo::CPoint(x,y,z));
    //}
    if(!m_mpPoint.insert(TPointMap::value_type(nIndex, nMeshIndex)).second)
      throw CReadException("Point index not unique");

  for (size_t i = 0; i < m_pElementSet->ValueSetSize(); ++i)
      if (!m_pElementSet->ValueSet(i).Component()->IsInRange(m_pElementSet->ValueSet(i)))
    l.append(QString("Component \"%1\" has values out of range.").arg(m_pPointSet->ValueSet(i).Component()->Name()));
  }
  else {
    assert(m_pPointSet);
    m_pPointSet->PushBack(vcRow, true);

  for (size_t i = 0; i < m_pPointSet->ValueSetSize(); ++i)
  {
    if( m_pPointSet->ValueSet(i).Component() )
    {
      if (!m_pPointSet->ValueSet(i).Component()->IsInRange(m_pPointSet->ValueSet(i)))
      l.append(QString("Component \"%1\" has values out of range.").arg(m_pPointSet->ValueSet(i).Component()->Name()));
    }
    else
    {
      l.append( QString("Component: nullptr") );
    }
  }
  }

  if (!l.empty())
  throw CReadException(l.join("\n"));
}

IPointSet& CNodalImportFormat::PointSet()
{
  if(m_pElementSet)
    return *m_pElementSet;
  return *m_pPointSet;
}
  
IValueSet* CNodalImportFormat::OnCreateValueSet(const QString& strName)
{
  // Register the value type
  CNodalValueSet *pValueSet = new CNodalValueSet(strName, CQuantity::SI_UNIT, PointSet()); 
  return pValueSet;

}

int CNodalImportFormat::MeshNodeIndex(int nFileIndex) const
{
  TPointMap::const_iterator it = m_mpPoint.find(nFileIndex);
  if(it != m_mpPoint.end())
    return it->second;
  return -1;
}

CNodalImportFormat::TAxisSystem CNodalImportFormat::AxisSystem() const
{
  for(TCoordinateMap::const_iterator it = m_mpCoordinate.begin(); it != m_mpCoordinate.end(); it++)
  {
    switch(it->second)
    {
    case CT_NORTHING:
      return AXIS3D;
    case CT_EASTING:
      return AXIS3D;
    case CT_T:
      return AXIS2D;
    case CT_N:
      return AXIS2D;
    case CT_DEPTH:
      break;
    }
  }

  return UNKNOWN;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CResultFormat
//////////////////////////////////////////////////////////////////////
CResultImportFormat::CResultImportFormat(CModelBase &model)
: IImportFormat(model),
  m_nElementNodeColumn(0), 
  m_Unit(CQuantity::SI_UNIT),
  m_vcElementRead(model.Mesh().Mesh().ElementSize(), false),
  m_nCurrentNode(0),
  m_pCurrentElement(0)
{
}

CResultImportFormat::~CResultImportFormat()
{
  for(size_t i = 0; i < m_vcResultDescriptions.size(); i++) delete m_vcResultDescriptions[i];
}

const geo::IElement& CResultImportFormat::Element() const
{
  assert(m_pCurrentElement);
  return *m_pCurrentElement;
}

/*!
  Appends the headers tags for the results.
*/
void CResultImportFormat::AppendColumn(const QString &sName)
{
  if(sName.compare("ElementNr", Qt::CaseInsensitive) == 0)
  {
    if(ColumnSize() != 0)
      throw CReadException("Keyword \"ElementNr\" must be the first keyword in the header.");
    m_nElementNodeColumn++;
  } else if(sName.compare("NodeNr", Qt::CaseInsensitive) == 0) {
    if(ColumnSize() != 1)
      throw CReadException("Keyword \"NodeNr\" must be the second keyword in the header.");
    m_nElementNodeColumn++;
  } else {
    ProcessResult( sName, ColumnSize() );
  }
}

int CResultImportFormat::ColumnSize() const
{
  return m_vcResultDescriptions.size() + m_nElementNodeColumn;
}

IPointSet& CResultImportFormat::PointSet()
{
  assert(false);
  IPointSet *pPS = 0;
  return *pPS;
}

/*!
  Append a row with number to the result format
*/
void CResultImportFormat::AppendRow(const TValueVec &vcRow)
{
  if(vcRow.size() != ColumnSize()) {
    throw CReadException(QString("Expected %1 values, read %2 values in results section.")
                         .arg(ColumnSize()).arg(vcRow.size() - 2));
  }
  
  if(!vcRow[0].Valid() || !vcRow[1].Valid()) 	{
    throw CReadException("Invalid element or node index detected.");
  }

  // here we subtract 1 again since internally the mesh is 0-based....
  int nCurrentElement = vcRow[0].Value() - 1;
  // Check element index
  if( nCurrentElement < 0 || nCurrentElement > Model().Mesh().Mesh().ElementSize() - 1 ) {
    throw CReadException(QString("Element index %1 does not exist.").arg( nCurrentElement + 1));
  }
  
  if( m_pCurrentElement ) {
    if( m_pCurrentElement->Index() != nCurrentElement ) {
      // Element change ... check nr of nodes
      if( m_nCurrentNode != m_pCurrentElement->NrOfNodes()) {
        throw CReadException(QString("%1 nodal values supplied for element %2 instead of %3")
                             .arg(m_nCurrentNode)
                   .arg(m_pCurrentElement->Index() + 1)
                   .arg(m_pCurrentElement->NrOfNodes()));
      }
      // Element is correctly handled ...
      m_vcElementRead[m_pCurrentElement->Index()] = true;
      m_nCurrentNode = 0;
    }
  }
  
  // Set element
  m_pCurrentElement = &Model().Mesh().Mesh().Element(nCurrentElement);

  // this is a global node index, but we want the local node index (on element level)
  // also subtract 1 since internally the mesh is 0-based....
  int nGlobalNodeIndex = vcRow[1].Value() - 1;
  // get the right node
  // wedx, mantis #2563:
  // Sometimes the same global node index is provided more than once for fault elements
  // that end inside a formation. In that case we don't know which local node index
  // is meant. Actually, information was lost.
  // This code assumes that the nodes are provided in the local node order.
  std::vector<int> vcOptions; // vector of local node indices that are an option given the global node index

  for(int i = 0; i < m_pCurrentElement->NrOfNodes(); i++)
  {	
    if(m_pCurrentElement->Node(i).Index() == nGlobalNodeIndex)
      vcOptions.push_back(i);
  }

  if(vcOptions.size() == 0)
  {
    throw CReadException(QString("Node %1 in element %2 not found in mesh.")
                         .arg((int)vcRow[1].Value()).arg((int)vcRow[0].Value()));
  }

  int nLocalNodeIndex = -1;

  if(vcOptions.size() == 1)
  nLocalNodeIndex = vcOptions[0];
  else
  {
  for(size_t i = 0; i < vcOptions.size(); ++i)
  {
      if(vcOptions[i] == m_nCurrentNode)
      {
    nLocalNodeIndex = vcOptions[i];
    break;
      }
  }

  if(nLocalNodeIndex == -1)
      nLocalNodeIndex = vcOptions[0];
  }

  bool bResultValueAdded = false;
  for(size_t i = 2; i < vcRow.size(); i++)
  {
    // Retrieve result description and insert the value
    CResultDescription& rd = *m_vcResultDescriptions[i - 2];
    if(rd.AddResultValue( nLocalNodeIndex, vcRow[i] ))
      bResultValueAdded = true;
  }

  if(!bResultValueAdded)
  throw CReadException(QObject::tr("No result values specified for element %1").arg(m_pCurrentElement->Index()));

  m_nCurrentNode++;
}

void CResultImportFormat::ProcessResult(const QString &sResult, int /*nColumn*/)
{
  QString sResultName = sResult.left((sResult.indexOf("_") != -1 ? sResult.indexOf("_") : sResult.length()));
  sResultName = sResultName.toLower();

  // Detect result name
  int nResultType = ResultTypeIndex( sResultName );
  if(nResultType == -1) {
    // Result name is not recognised, ignore
  	m_vcResultDescriptions.push_back( new CResultDescription(*this) );
  return;
  }

  // Dectect fault result
  bool bFault = sResultName.indexOf("fault") != -1;

  // Dectect result type
  CAnalysisType antype;
  if(!antype.FromExportCharacter(sResult.right(1).at(0).toLatin1()))
    throw CReadException(QString("Unknown calculation type detected in %1.").arg(sResult));
/*
  if(IsFault(nResultType))
  {
    const CTetraModel *pModel = dynamic_cast<const CTetraModel*>(&Model());
    if(!pModel || ((CTetraSubHorizonEntry*)pModel->GraphEntry(MD_TETRA_SUB_FAULT))->EntryNodes().size() <= 0)
    {
      error.Format("Fault results detected but model does not contain faults.");
      return false;
    }
  }
*/
  int nDepStage = ExtractDepletionStage(sResult);
  if(nDepStage < 0 || nDepStage > Model().NrOfDepletionStages())
    throw CReadException(QString("Invalid depletionstage number detected in %1.").arg(sResult));

  m_vcResultDescriptions.push_back( new CResultDescription(*this, nResultType, bFault, nDepStage, antype) );
}

int CResultImportFormat::ExtractDepletionStage(const QString &sResult) const
{
  // e.g. sResult = DisplacementN_D0_L
  QString sResultName = sResult.left((sResult.indexOf("_") != -1 ? sResult.indexOf("_") : sResult.length()));
  // sResultName = DisplacementN
  QString temp = sResult.right(sResult.length() - sResultName.length());
  // temp = _D0_L
  temp = temp.mid(2, temp.length() - 4);
  //temp = temp.Mid(temp.GetLength() - 4, 3);
  // temp = 0
  return temp.toInt();
}

IValueSet* CResultImportFormat::OnCreateValueSet(const QString& /*strName*/)
{
  assert(false);
  IValueSet *pVS = 0;
  return pVS;
}

void CResultImportFormat::ValidateColumns()
{
  // TODO// Some extra validations...
}

/*!
  Validates the whole file after reading
*/
void CResultImportFormat::ValidateRows()
{
  if( m_nCurrentNode != m_pCurrentElement->NrOfNodes()) {
     throw CReadException(QString("%1 nodal values supplied for element %2 instead of %3")
                           .arg(m_nCurrentNode + 1)
                 .arg(m_pCurrentElement->Index())
                 .arg(m_pCurrentElement->NrOfNodes()));
  }
  // Last element correctly handled
  m_vcElementRead[m_pCurrentElement->Index()] = true;

  QString sMessage("At least element ");
  int nMissing = 0;
  for(size_t i = 0; i < m_vcElementRead.size(); i++) {
    if(!m_vcElementRead[i] && nMissing < 4) {
      sMessage += QString("%1 ").arg(i+1);
      nMissing++;
    }
  }
  sMessage += "are missing the result file.";
      
  if(nMissing > 0) throw CReadException( sMessage );
}

/*!
  Returns the index in the result register CI_... 
  Returns -1 when the results is not recognized.
*/
int CResultImportFormat::ResultTypeIndex(const QString& typeName) const
{
  if( typeName == "displacementn") return CI_DISPLA_X;
  if( typeName == "displacemente") return CI_DISPLA_Y;
  if( typeName == "displacementv") return CI_DISPLA_Z;
  
  if( typeName == "pressure") return CI_POREPRES;

  if( typeName == "estressnn") return CI_STRESS_XX;
  if( typeName == "estressee") return CI_STRESS_YY;
  if( typeName == "estressvv") return CI_STRESS_ZZ;
  if( typeName == "estressne") return CI_STRESS_XY;
  if( typeName == "estressev") return CI_STRESS_YZ;
  if( typeName == "estressnv") return CI_STRESS_XZ;

  if( typeName == "strainnn") return CI_STRAIN_XX;
  if( typeName == "strainee") return CI_STRAIN_YY;
  if( typeName == "strainvv") return CI_STRAIN_ZZ;
  if( typeName == "strainne") return CI_STRAIN_XY;
  if( typeName == "strainev") return CI_STRAIN_YZ;
  if( typeName == "strainnv") return CI_STRAIN_XZ;

  if( typeName == "strainplasticnn") return CI_PSTRAIN_XX;
  if( typeName == "strainplasticee") return CI_PSTRAIN_YY;
  if( typeName == "strainplasticvv") return CI_PSTRAIN_ZZ;
  if( typeName == "strainplasticne") return CI_PSTRAIN_XY;
  if( typeName == "strainplasticev") return CI_PSTRAIN_YZ;
  if( typeName == "strainplasticnv") return CI_PSTRAIN_XZ;

  if( typeName == "straincreepnn") return CI_CSTRAIN_XX;
  if( typeName == "straincreepee") return CI_CSTRAIN_YY;
  if( typeName == "straincreepvv") return CI_CSTRAIN_ZZ;
  if( typeName == "straincreepne") return CI_CSTRAIN_XY;
  if( typeName == "straincreepev") return CI_CSTRAIN_YZ;
  if( typeName == "straincreepnv") return CI_CSTRAIN_XZ;

  if( typeName == "faultdisplacementn") return CI_STRAIN_XX;
  if( typeName == "faultdisplacemente") return CI_STRAIN_YY;
  if( typeName == "faultdisplacementv") return CI_STRAIN_ZZ;
  
  if( typeName == "faultestressnormal") return CI_PSTRAIN_XX;

  if( typeName == "faultestressn") return CI_STRESS_XX;
  if( typeName == "faultestresse") return CI_STRESS_YY;
  if( typeName == "faultestressv") return CI_STRESS_ZZ;

  if( typeName == "faultpressure") return CI_POREPRES;

  if( typeName == "temperature") return CI_TEMPERATURE;

  if( typeName == "preconpressure") return CI_PRECON;

  return -1;
}

CResultImportFormat::CResultDescription::CResultDescription(CResultImportFormat& parent)
: m_parent(parent),
  m_bIgnore(true)
{
}

CResultImportFormat::CResultDescription::CResultDescription(CResultImportFormat& parent, 
                              int nResultType, 
                              bool bFault, 
                              int nDepStage, 
                              const CAnalysisType& antype)
: m_nResultType(nResultType),
  m_parent(parent),
  m_nDepStage(nDepStage),
  m_antype(antype),
  m_bFault(bFault),
  m_bIgnore(false)
{
}

/*!
  Returns true when the result is a pressure type
*/
bool CResultImportFormat::CResultDescription::IsPressure() const
{
  return m_nResultType == CI_POREPRES;
}

bool CResultImportFormat::CResultDescription::IsPreconPressure() const
{
  return m_nResultType == CI_PRECON;
}

/*!
  Returns true when the result is a stress type and false otherwise
*/
bool CResultImportFormat::CResultDescription::IsStress() const
{
  if(( m_nResultType >= CI_STRESS_XX ) &&
   ( m_nResultType <= CI_STRESS_XZ )) 
 	  return true;

  return false;
}

/*!
  Returns true when the result is a strain type and false otherwise
*/
bool CResultImportFormat::CResultDescription::IsStrain() const
{
  if(( m_nResultType >= CI_STRAIN_XX ) &&
   ( m_nResultType <= CI_STRAIN_XZ )) 
 	  return true;

  if(( m_nResultType >= CI_PSTRAIN_XX ) &&
   ( m_nResultType <= CI_PSTRAIN_XZ )) 
 	  return true;

  if(( m_nResultType >= CI_CSTRAIN_XX ) &&
   ( m_nResultType <= CI_CSTRAIN_XZ )) 
 	  return true;

  return false;
}

bool CResultImportFormat::CResultDescription::IsFaultElement() const
{
  return dynamic_cast<const geo::IInterfaceElement*>(&m_parent.Element()) != 0;
}

bool CResultImportFormat::CResultDescription::IsFault() const
{
  return m_bFault;
}


/*!
  Returns the correction factor for the field value
*/
double CResultImportFormat::CResultDescription::FieldFactor() const
{
  if(( m_nResultType >= CI_STRESS_XX ) &&
   ( m_nResultType <= CI_STRESS_XZ )) return FF_FACTOR_STRESS;

  if( m_nResultType == CI_POREPRES ) return FF_FACTOR_STRESS;

  if(( m_nResultType >= CI_DISPLA_X ) &&
   ( m_nResultType <= CI_DISPLA_Z )) return FF_FACTOR_LENGTH;

  if(( m_nResultType >= CI_STRAIN_XX ) &&
   ( m_nResultType <= CI_STRAIN_ZZ ) &&
   IsFault() ) return FF_FACTOR_LENGTH;

  if(m_nResultType == CI_PRECON)
  return FF_FACTOR_STRESS;

  return 1.0;
}

/*!
  Adds a result value in a point of an element
*/
bool CResultImportFormat::CResultDescription::AddResultValue(int nLocalNodeIndex, const geo::CValue &val)
{
  if(m_bIgnore)
  return false;

  geo::CValue value(val);
  if(!value.Valid())
  value = 0; // same behaviour as import from filos

  bool bRet = (IsFault() == IsFaultElement());
  if(bRet)
  {
    assert( value.Valid() );
    // convert the value if necessary
    if(m_parent.Unit() == CQuantity::FIELD_UNIT) {
      m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                   m_parent.Element().Index(),
                                   nLocalNodeIndex,
                                   this->m_nDepStage,
                                   m_antype,
                                   value.Value()/FieldFactor());
    } else {
      assert(m_parent.Unit() == CQuantity::SI_UNIT);
      if(IsStress()) {
        m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                     m_parent.Element().Index(),
                                     nLocalNodeIndex,
                                     this->m_nDepStage,
                                     m_antype,
                                     value.Value() * (-1.0 / 1E6)); // convert from Pa to MPa and switch sign
      } else if(IsPressure()) {
        m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                     m_parent.Element().Index(),
                                     nLocalNodeIndex,
                                     this->m_nDepStage,
                                     m_antype,
     													           value.Value() * (1.0 / 1E6)); // convert from Pa to MPa sign switch, must be pressure
      } else if( IsStrain() ) {
        m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                     m_parent.Element().Index(),
                                     nLocalNodeIndex,
                                     this->m_nDepStage,
                                     m_antype,
                                     -value.Value() ); // Strains are interpreted as negative signs
      } else if(IsPreconPressure()) {
        m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                     m_parent.Element().Index(),
                                     nLocalNodeIndex,
                                     this->m_nDepStage,
                                     m_antype,
     													       value.Value() * (1.0 / 1E6)); // convert from Pa to MPa sign switch, must be pressure
      } else {

        m_parent.Model().ResultRegister().AddResultFromImport( m_nResultType,
                                     m_parent.Element().Index(),
                                     nLocalNodeIndex,
                                     this->m_nDepStage,
                                     m_antype,
                                     value.Value() );
      }
    }
  }

  return bRet;
}
//////////////////////////////////////////////////////////////////////
// Implementation of IElementFormat
//////////////////////////////////////////////////////////////////////
CElementImportFormat::CElementImportFormat(CNodalImportFormat& nodal_format) 
: IImportFormat((CModelBase&)nodal_format.ElementSet().Model()),
  m_nodal_format(nodal_format), 
  m_nNodeIndex(-1), 
  m_nElementIndex(-1),
  m_nNodeNr(-1)
{
}

void CElementImportFormat::AppendColumn(const QString& sName)
{
  if(sName.compare("NodeIndex", Qt::CaseInsensitive) == 0) {
    if(m_nNodeIndex != -1)
      throw CReadException("Keyword \"NodeIndex\" must be unique in the header");

    m_nNodeIndex = ColumnSize();
  } else if(sName.compare("NodeNr", Qt::CaseInsensitive) == 0 || sName.compare("nNodeNr", Qt::CaseInsensitive) == 0){
    // due to a previous bug it is possible that there are files that have "nNodeNr"
    // in stead of "NodeNr" as a column name, that's why we check for both....
    if(m_nNodeNr != -1)
      throw CReadException("Keyword \"NodeNr\" must be unique in the header");
    m_nNodeNr = ColumnSize();
  } else if(sName.compare("ElementIndex", Qt::CaseInsensitive) == 0 || sName.compare("ElementNr", Qt::CaseInsensitive) == 0){
    // Index of the element ...
    if(m_nElementIndex != -1) {
      throw CReadException(QString("Keyword %1 must be unique in the header").arg(sName));
    }
    m_nElementIndex = ColumnSize();
  } else {
    IImportFormat::AppendColumn(sName);
  }
}

void CElementImportFormat::ValidateColumns() 
{
  if(m_nNodeIndex == -1)
    throw CReadException("Keyword \"NodeIndex\" must be in the header");
}

void CElementImportFormat::ValidateRows() 
{
  PointSet().CoordinateUnit(CDoubleQuantity::SI_UNIT);
  PointSet().ValueUnit(CDoubleQuantity::SI_UNIT);
  PointSet().RemoveAllUnitConversions();

  // Convert to 2D?
  if (m_nodal_format.AxisSystem() != CNodalImportFormat::UNKNOWN)
  Model().ConvertSet(&PointSet(), m_nodal_format.AxisSystem() == CNodalImportFormat::AXIS3D);
}

int CElementImportFormat::ColumnSize() const
{
  int nRet = 0;
  if(m_nNodeNr != -1)
    nRet++;
  if(m_nNodeIndex != -1)
    nRet++;
  if(m_nElementIndex != -1)
    nRet++;

  return nRet + m_vcValueSet.size();
}

void CElementImportFormat::AppendRow(const TValueVec &vcRow)
{
  assert(m_nNodeIndex != -1);
  // Cycles 
  if(vcRow.size() != ColumnSize()) {
    throw CReadException(QString("Expected %1 values; read %2 values in element section")
           .arg(ColumnSize()).arg( vcRow.size() ));
  }

  // Grow if necessary 
  if(m_vcElementData.size() == 0)
    m_vcElementData.resize(ColumnSize());

  // Just start to add
  for(size_t i = 0; i < vcRow.size(); i++)
    m_vcElementData[i].push_back(vcRow[i]);

  // Write?
  int nPointSize = 3; // Surface by default
  const CElementSet* pElementSet = dynamic_cast<const CElementSet*>(&m_nodal_format.ElementSet());
  if(pElementSet) nPointSize = pElementSet->ElementPointSize();
  if(m_vcElementData[0].size() == nPointSize)
  {
    // Note that m_nElementIndex is just ignored...
    // Check point counter first 
    if(m_nNodeNr != -1)
    {
      for(size_t i = 0; i < m_vcElementData[m_nNodeNr].size(); i++)
      {
        if(!m_vcElementData[m_nNodeNr][i].Valid() || (i != (size_t)m_vcElementData[m_nNodeNr][i].Value()))
          throw CReadException("Node number not consistent with element point size");
      }
    }

    // Create element first
    std::vector<int> vcNode;

    for(size_t i = 0; i < m_vcElementData[m_nNodeIndex].size(); i++)
    {
      if(!m_vcElementData[m_nNodeIndex][i].Valid())
        throw CReadException("Node number is a NAN");
      int nMeshIndex = m_nodal_format.MeshNodeIndex((int)m_vcElementData[m_nNodeIndex][i].Value());
      if(nMeshIndex == -1)
        throw CReadException("Node index not found under node section");
      vcNode.push_back(nMeshIndex);
    }
    int nElement = m_nodal_format.ElementSet().CreateElement(vcNode);
    if(nElement == -1) throw CReadException("Wrong number of element nodes supplied.");



    // Write the value
    for(size_t i = 0; i < m_vcValueSet.size(); i++)
    {
      // wjrx mantis 3340
      //
      const IUnitConversion *pConversion= 
      m_nodal_format.ElementSet().UnitConversion( PointSet().ValueSet(i).Name().toStdString());

      if (pConversion)
      {
    for (size_t jj = 0; jj < m_vcElementData[m_vcValueSet[i]].size(); ++jj)
    {
          m_vcElementData[m_vcValueSet[i]][jj].Value(
      pConversion->
      ConvertToSi(m_vcElementData[m_vcValueSet[i]][jj].Value()));
    }
      }
      else if (m_nodal_format.ElementSet().ValueUnit(true) == CQuantity::FIELD_UNIT)
      {
    double factor = m_nodal_format.ElementSet().ValueSet(m_vcValueSet[i] - 2).Component()->FieldFactor();
    for (size_t jj = 0; jj < m_vcElementData[m_vcValueSet[i]].size(); ++jj)
    {
          m_vcElementData[m_vcValueSet[i]][jj].Value(m_vcElementData[m_vcValueSet[i]][jj].Value() / factor);
    }
      }

      m_nodal_format.ElementSet().ElementValueSet(i).PushBack(m_vcElementData[m_vcValueSet[i]]);
    }

    m_vcElementData.clear();
  }
}

IPointSet& CElementImportFormat::PointSet()
{
  return m_nodal_format.ElementSet();
}

IValueSet* CElementImportFormat::OnCreateValueSet(const QString& strName)
{
  IValueSet *pValueSet = new CElementValueSet(strName, CQuantity::SI_UNIT, m_nodal_format.ElementSet());
  return pValueSet;
}

/*!
  Set the element type read in the file based on a string
  Throws an exception when element type is not valid
*/ 
void CElementImportFormat::SetElementType(const QString& sElementType)
{
  CElementSet::TElementType type = CElementSet::UNKNOWN;
       if(sElementType.compare("HEXA8", Qt::CaseInsensitive) == 0)
    type = CElementSet::HEXA;
  else if(sElementType.compare("TETRA4", Qt::CaseInsensitive) == 0)
    type = CElementSet::TETRA;
  else if(sElementType.compare("TRI3", Qt::CaseInsensitive) == 0)
    type = CElementSet::TRIANGLE_2D;
  else if(sElementType.compare("QUAD4", Qt::CaseInsensitive) == 0)
    type = CElementSet::QUAD_2D; 
  else if(sElementType.compare("LINE2", Qt::CaseInsensitive) == 0)
    type = CElementSet::LINE_2D;

  if(sElementType.compare("IQUAD8", Qt::CaseInsensitive) == 0 ||
     sElementType.compare("ITRI6",  Qt::CaseInsensitive) == 0 ||
     sElementType.compare("ILINE4", Qt::CaseInsensitive) == 0)
    throw CReadException("The import of interface elements is not supported.");

  CElementSet* pElementSet = dynamic_cast<CElementSet*>(&m_nodal_format.ElementSet());
  if(pElementSet) {
    if( type == CElementSet::UNKNOWN ) 
      throw CReadException(QString("Unknown element type %1").arg(sElementType));
    pElementSet->ElementType( type );
  } else {
    // For a surface we must have triangle 2d
    if( type != CElementSet::TRIANGLE_2D ) 
      throw CReadException(QString("Element type type of a surface must be TRI3"));
  }
}



/*!
  The wellpath import format constructor
*/

CWellPathInputImportFormat::CWellPathInputImportFormat(CNewWellPathInput *pNewWellPathInput)
: IImportFormat((CModelBase&)pNewWellPathInput->Model())
, m_Unit(UNIT_SI)
, m_column(5, -1)
, m_pNewWellPathInput(pNewWellPathInput)
, m_lastColumnIndex(4)
{
  assert(m_pNewWellPathInput!=0);

/*
  m_column[0] = -1; // Easting
  m_column[1] = -1; // Northing
  m_column[2] = -1; // TVD (Depth)
  m_column[3] = -1; // TMD
  m_column[4] = -1; // TVDKB
*/
}

bool CWellPathInputImportFormat::OnUnit(const QString& sUnit)
{
  if(sUnit.toUpper() == "SI")
    m_Unit = UNIT_SI;
  else if(sUnit.toUpper() == "FIELD")
    m_Unit = UNIT_FIELD;
  else
    return false;

  return true;
}

void CWellPathInputImportFormat::setColumn(int nColumn, const QString &sName)
{
  assert( nColumn >= 0 );

  if (nColumn >= 5)
  {
  m_column.push_back(-1);
  }

  if( m_column[nColumn] == -1 ) {
    m_column[nColumn] = ColumnSize();
  } else {
    throw CReadException(QString("Duplicate name %1 in header.").arg(sName));
  }
}

/*!
  The column header append X, Y, Z, EASTING, NORTHING, TMD, DEPTH are supported
*/
void CWellPathInputImportFormat::AppendColumn(const QString& sName)
{
  if(sName.compare("Easting", Qt::CaseInsensitive) == 0 || sName.compare("Y", Qt::CaseInsensitive) == 0)
    setColumn(0, sName);
  else if(sName.compare("Northing", Qt::CaseInsensitive) == 0 || sName.compare("X", Qt::CaseInsensitive) == 0)
    setColumn(1, sName);
  else if(sName.compare("Depth", Qt::CaseInsensitive) == 0 || 
      sName.compare("Z", Qt::CaseInsensitive) == 0 ||
      sName.compare("TVD", Qt::CaseInsensitive) == 0)
    setColumn(2, sName);
  else if(sName.compare("AHD", Qt::CaseInsensitive) == 0 ||
      sName.compare("TMD", Qt::CaseInsensitive) == 0 || 
      sName.compare("MD", Qt::CaseInsensitive) == 0)
    setColumn(3, sName);
  else if(sName.compare("TVDKB", Qt::CaseInsensitive) == 0)
  setColumn(4, sName);
  else
  {
  setColumn(++m_lastColumnIndex, sName);
  }
}

void CWellPathInputImportFormat::ValidateColumns() 
{
  if( m_column[0] == -1 ) throw CReadException("Easting not specified in the header.");
  if( m_column[1] == -1 ) throw CReadException("Northing not specified in the header.");
  if( m_column[2] == -1 ) throw CReadException("Depth not specified in the header.");
}

void CWellPathInputImportFormat::ValidateRows()
{
  if( m_vcPoint.size() < 2 ) {
    throw CReadException("A well path should be specified by at least two points.");
  }

  assert(m_pNewWellPathInput!=0);

  // wjrx mantis 3185, but no sampling per 2 meters (see mantis 3443)
  m_pNewWellPathInput->InitFromPointArray(m_vcPoint);
}

int CWellPathInputImportFormat::ColumnSize() const
{
  int nRet = 0;
  for(size_t i = 0; i < m_column.size(); i++) 
    if(m_column[i] > -1) nRet++;
  return nRet;
}

void CWellPathInputImportFormat::AppendRow(const TValueVec &vcRow)
{
  if( vcRow.size() != ColumnSize() ) 
    throw CReadException(QString("%1 values expected for instead of %2").arg(ColumnSize()).arg(vcRow.size()));

  for(int i = 0; i < ColumnSize(); i++) {
    if(m_column[i] >= 0 && !vcRow[ m_column[i] ].Valid()) {
      throw CReadException("NAN's are not allowed in well path definitions.");
    } 
  }

  geo::CPoint pt(vcRow[ m_column[1] ].Value(),
           vcRow[ m_column[0] ].Value(),
           vcRow[ m_column[2] ].Value());

  if(m_Unit == UNIT_FIELD)
    pt = pt / FF_FACTOR_LENGTH; // wedx 16012007: go from 'feet' to 'm', so divide

  m_vcPoint.push_back(pt);
}

/*!
  Should not be called anyway!!!!
*/
IValueSet* CWellPathInputImportFormat::OnCreateValueSet(const QString& /*strName*/)
{
  assert(false);
  return 0;
}

/*!
  Should not be called anyway!!!!
*/
IPointSet& CWellPathInputImportFormat::PointSet()
{
  assert(false);

  /*
   * TODO reference to pointer value zero.
   * Remember, the created CPointSet pBogus does not get destructed!
   * see Mantis #3596.
   */

  CPointSet *pBogus = new CPointSet(Model());
  return *pBogus;
}
