// PropertyFile.cpp: implementation of the CPropertyFile class.
//
//////////////////////////////////////////////////////////////////////

#include "ExportFormat.h"
#include "3DHorizon.h"
#include "CrossSection.h"
#include "DCasingDefines.h"
#include "ElementGroup.h"
#include "FieldFactor.h"
#include "FormationPlane.h"
#include "Hexahedron.h"
#include "HorizonBase.h"
#include "HotSpot.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "NonMeshedSurface.h"
#include "PointSet.h"
#include "Result.h"
#include "TSSurface.h"
#include "WellCasingRTCI.h"
#include "resourceIDS.h"

static QString FixName(const QString &strName) {
  QString strRet = strName;

  // replace spaces by underscores
  int i;
  for (i = 0; i < strRet.length(); ++i) {
    if (strRet[i] == ' ')
      strRet[i] = '_';
  }

  return strRet;
}

//////////////////////////////////////////////////////////////////////
// Implementation of IExportFormat
//////////////////////////////////////////////////////////////////////
IExportFormat::IExportFormat(const TDataVec &vcData, const CExportArg &arg)
    : m_arg(arg), m_vcData(eliminateDuplicates(vcData)) {}

IExportFormat::IExportFormat(const TValueTypeVec &vcData, const CExportArg &arg) : m_arg(arg), m_vcValueType(vcData) {
  for (size_t i = 0; i < vcData.size(); i++) {
    for (unsigned int j = 0; j < vcData[i]->ComponentSize(); j++) {
      m_vcData.push_back(&vcData[i]->Component(j));
    }
  }
}

QString IExportFormat::LoadFromString(unsigned int uString) const { return getStringTableEntry(uString); }

int IExportFormat::ColumnSize() const { return m_vcData.size(); }

IExportFormat::TOutputType IExportFormat::OutputType(int /*nRow*/, int /*nColumn*/) const { return OT_STRING; }

QString IExportFormat::StringAt(int nRow, int nColumn) const {
  assert(nRow == 0);
  if (ValueTypeSize() > 0) {
    if (DataValue(nColumn).Parent().ComponentSize() > 1)
      return QString(DataValue(nColumn).Name());
    else
      return QString(DataValue(nColumn).Parent().Name());
  }
  if (dynamic_cast<const CValueType *>(&DataValue(nColumn).Parent()))
    return QString(DataValue(nColumn).Parent().Name());
  QString sRet;
  if (HotSpot())
    sRet = "HS_";
  sRet += DataValue(nColumn).ExportLabel();
  return sRet;
}

size_t IExportFormat::DataValueSize() const { return m_vcData.size(); }

const IValueComponentBase &IExportFormat::DataValue(size_t nIndex) const {
  assert(nIndex < m_vcData.size());
  return *m_vcData[nIndex];
}

IExportFormat::TOutput IExportFormat::DataOutput(const geo::IValue &value) const {
  TOutput ret;
  if (value.Valid()) {
    if (HotSpot()) {
      ret.first = OT_STRING;
      if (HotSpot()->CenterIsTransparent()) {
        if (value.Value() < HotSpot()->MinMaxValue().first)
          ret.second.sString = LoadFromString(IDS_ET_HOTSPOT_MIN);
        else if (value.Value() > HotSpot()->MinMaxValue().second)
          ret.second.sString = LoadFromString(IDS_ET_HOTSPOT_MAX);
        else
          ret.second.sString = LoadFromString(IDS_ET_HOTSPOT_COLD);
      } else {
        if (value.Value() < HotSpot()->MinMaxValue().first)
          ret.second.sString = LoadFromString(IDS_ET_HOTSPOT_COLD);
        else if (value.Value() > HotSpot()->MinMaxValue().second)
          ret.second.sString = LoadFromString(IDS_ET_HOTSPOT_HOT);
        else
          ret.second.sString =
              QString("%1%").arg((((value.Value() - HotSpot()->MinMaxValue().first) /
                                   (HotSpot()->MinMaxValue().second - HotSpot()->MinMaxValue().first)) *
                                  100.0));
      }
    } else {
      ret.first = OT_DOUBLE;
      ret.second.dDouble = value.Value();
    }
  } else {
    ret.first = OT_STRING;
    ret.second.sString = LoadFromString(IDS_ET_NAN);
  }

  return ret;
}

size_t IExportFormat::ValueTypeSize() const { return m_vcValueType.size(); }

const CValueType &IExportFormat::ValueType(size_t nIndex) const { return *m_vcValueType[nIndex]; }

// private

// static

IExportFormat::TDataVec IExportFormat::eliminateDuplicates(const TDataVec &dataVector) {
  TDataVec cleansedData = dataVector;

  // eliminate duplicates

  TDataVec::iterator data = cleansedData.begin();

  while (data != cleansedData.end()) {
    if (((*data)->ExportLabel() == getStringTableEntry(IDS_MESH_RESULT_DEPTH)) ||
        ((*data)->ExportLabel() == getStringTableEntry(IDS_MESH_RESULT_NORTHING)) ||
        ((*data)->ExportLabel() == getStringTableEntry(IDS_MESH_RESULT_EASTING))) {
      cleansedData.erase(data);
      data = cleansedData.begin();
    } else {
      ++data;
    }
  }

  return cleansedData;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CNodalExportFormat
//////////////////////////////////////////////////////////////////////
CNodalExportFormat::CNodalExportFormat(const CPointSet &point_set,  // PointSet to export
                                       const TValueTypeVec &vcData, // Data on the pointset
                                       const CExportArg &arg,       // Export options
                                       bool export2LAS)
    : IExportFormat(vcData, arg), m_pPointSet(&point_set), m_pElementSetFormat(0), m_pNewWellPath(0),
      m_row_cache_index(-1), m_bAddFormationNames(arg.m_bFormationNames), m_export2LAS(export2LAS),
      m_iteratorWellPathPosition(-1), m_data_offset(0) {
  // Remove the 'unwanted' coordinate value type
  size_t i = 0;
  while (i < m_vcValueType.size()) {
    const CValueType *pValueType = m_vcValueType[i];
    if (&point_set.Coordinates() == pValueType)
      m_vcValueType.erase(m_vcValueType.begin() + i);
    else
      i++;
  };
  i = 0;
  while (i < m_vcData.size()) {
    if (&point_set.Coordinates() == &m_vcData[i]->Parent())
      m_vcData.erase(m_vcData.begin() + i);
    else
      i++;
  };
}

CNodalExportFormat::CNodalExportFormat(const CPointSet &point_set,
                                       const TDataVec &vcData, // Data on the pointset
                                       const CExportArg &arg,  // Arguments for the export
                                       bool export2LAS)
    : IExportFormat(vcData, arg), m_pPointSet(&point_set), m_pElementSetFormat(0), m_pNewWellPath(0),
      m_row_cache_index(-1), m_bAddFormationNames(arg.m_bFormationNames), m_export2LAS(export2LAS),
      m_iteratorWellPathPosition(-1), m_data_offset(0) {}

CNodalExportFormat::CNodalExportFormat(const IElementSetExportFormat &element_set_format,
                                       const TDataVec &vcData, // Data on the pointset
                                       const CExportArg &arg,  // Arguments for the export
                                       bool export2LAS)
    : IExportFormat(vcData, arg), m_pPointSet(0), m_pElementSetFormat(&element_set_format), m_pNewWellPath(0),
      m_row_cache_index(-1), m_bAddFormationNames(false), m_export2LAS(export2LAS), m_iteratorWellPathPosition(-1),
      m_data_offset(0) {}

CNodalExportFormat::CNodalExportFormat(const IElementSetExportFormat &element_set_format,
                                       const TValueTypeVec &vcData, // Data on the pointset
                                       const CExportArg &arg,       // Arguments for the export
                                       bool export2LAS)
    : IExportFormat(vcData, arg), m_pPointSet(0), m_pElementSetFormat(&element_set_format), m_pNewWellPath(0),
      m_row_cache_index(-1), m_bAddFormationNames(false), m_export2LAS(export2LAS), m_iteratorWellPathPosition(-1),
      m_data_offset(0) {}

// wjrx mantis 3322
CNodalExportFormat::CNodalExportFormat(const CNewWellPath &well_path,
                                       const TDataVec &vcData, // Data on the pointset
                                       const CExportArg &arg,  // Arguments for the export
                                       bool export2LAS)
    : IExportFormat(vcData, arg), m_pPointSet(0), m_pElementSetFormat(0), m_pNewWellPath(&well_path),
      m_row_cache_index(-1), m_bAddFormationNames(arg.m_bFormationNames), m_export2LAS(export2LAS),
      m_iteratorWellPathPosition(-1), m_data_offset(0) {}

QString CNodalExportFormat::ExportType() const {
  if (HotSpot())
    return "HOTSPOT";
  if (m_pElementSetFormat)
    return m_pElementSetFormat->ExportType();
  return "POINTSET";
}

bool CNodalExportFormat::HasExportName() const { return (m_pNewWellPath != 0); }

QString CNodalExportFormat::ExportName() const {
  if (m_pNewWellPath)
    return QString(m_pNewWellPath->Name());

  assert(false);
  return 0;
}

bool CNodalExportFormat::isGVT() const { return m_arg.m_bGVT; }

const CHotSpot *CNodalExportFormat::HotSpot() const { return m_arg.m_pHotSpot; }

double CNodalExportFormat::ConvCoor(const double &dValue, UNIT unit) const {
  if (unit == CQuantity::SI_UNIT)
    return dValue;
  return FF_FACTOR_LENGTH * dValue;
}

namespace {

const int WELL_PATH_XLS_COLUMNS = 5;
const int WELL_PATH_LAS_COLUMNS = 6;

} // anonymous namespace

void CNodalExportFormat::CreateCache(int nRow) const {
  m_row_cache.resize(ColumnSize());

  // When the row is zero, the header is on the output and we have string type in the whole row
  if (nRow == 0) {
    for (size_t i = 0; i < m_row_cache.size(); i++)
      m_row_cache[i].first = OT_STRING;
  }

  int nCount = 0;
  bool bHaveAllValues = false;
  geo::CPoint point;
  const CNewWellPoint *pNewWellPoint = 0;
  const geo::IPoint *pElementPoint = 0;
  if (m_pElementSetFormat) {
    // Define the index of node
    if (nRow == 0)
      m_row_cache[0].second.sString = "Index";
    else {
      m_row_cache[0].first = OT_INTEGER;
      if (m_arg.m_bOldFormat)
        m_row_cache[0].second.nInteger = m_pElementSetFormat->NodeIndex(nRow - 1) - 1;
      else
        m_row_cache[0].second.nInteger = m_pElementSetFormat->NodeIndex(nRow - 1);
    }

    assert(m_arg.m_axis_system == AXIS3D);
    if (nRow == 0) {
      m_row_cache[1].second.sString = LoadFromString(IDS_RC_EASTING);
      m_row_cache[2].second.sString = LoadFromString(IDS_RC_NORTHING);
      if (!m_pElementSetFormat->TwoDimensional())
        m_row_cache[3].second.sString = LoadFromString(IDS_RC_DEPTH);
    } else {
      m_pElementSetFormat->Node(point, nRow - 1);
      m_row_cache[1].first = OT_DOUBLE;
      m_row_cache[2].first = OT_DOUBLE;
      m_row_cache[1].second.dDouble = ConvCoor(point.Y(), Unit());
      m_row_cache[2].second.dDouble = ConvCoor(point.X(), Unit());
      if (!m_pElementSetFormat->TwoDimensional()) {
        m_row_cache[3].first = OT_DOUBLE;
        m_row_cache[3].second.dDouble = ConvCoor(point.Z(), Unit());
      }
    }
    nCount = 3;
    if (!m_pElementSetFormat->TwoDimensional())
      nCount = 4;

    assert(nCount > 2);
  } else if (m_pPointSet) {
    if (nRow > 0) {
      pElementPoint = &m_pPointSet->PointAt(nRow - 1);
      point = *pElementPoint;
    }

    for (unsigned int i = 0; i < m_pPointSet->Coordinates().ComponentSize(); i++) {
      if (m_pPointSet->Coordinates().Component(i).Defined()) {
        if ((nRow == 0) && (i == 0))
          m_row_cache[nCount].second.sString = LoadFromString(IDS_RC_NORTHING);
        if ((nRow == 0) && (i == 1))
          m_row_cache[nCount].second.sString = LoadFromString(IDS_RC_EASTING);
        if ((nRow == 0) && (i == 2))
          m_row_cache[nCount].second.sString = LoadFromString(IDS_RC_DEPTH);
        if (nRow > 0) {
          m_row_cache[nCount].first = OT_DOUBLE;
          if (i == 0)
            m_row_cache[nCount].second.dDouble = ConvCoor(point.X(), Unit());
          if (i == 1)
            m_row_cache[nCount].second.dDouble = ConvCoor(point.Y(), Unit());
          if (i == 2)
            m_row_cache[nCount].second.dDouble = ConvCoor(point.Z(), Unit());
        }
        nCount++;
      }
    }
  } else // wellpath
  {
    assert(m_pNewWellPath);
    assert(m_arg.m_axis_system == AXIS3D);

    nCount = m_export2LAS ? WELL_PATH_LAS_COLUMNS : WELL_PATH_XLS_COLUMNS;
    bHaveAllValues = m_arg.m_wellpathPoints == MESH_INTERSECTION_POINTS && IExportFormat::ColumnSize() > 0;

    if (bHaveAllValues && m_wellPathValues.size() < IExportFormat::ColumnSize()) {
      m_wellPathValues.resize(IExportFormat::ColumnSize());
      for (int i = 0; i < IExportFormat::ColumnSize(); ++i) {
        const_cast<CNewWellPath *>(m_pNewWellPath)->MapValues(DataValue(i), Unit(), m_wellPathValues[i]);
      }
    }

    if (nRow == 0) {
      if (m_export2LAS) {
        m_row_cache[0].second.sString = "DEPTH";
        m_row_cache[1].second.sString = LoadFromString(IDS_RC_EASTING);
        m_row_cache[2].second.sString = LoadFromString(IDS_RC_NORTHING);
        m_row_cache[3].second.sString = "TVDKB";
        m_row_cache[4].second.sString = "Inclination";
        m_row_cache[5].second.sString = "Azimuth";
      } else {
        m_row_cache[0].second.sString = LoadFromString(IDS_RC_EASTING);
        m_row_cache[1].second.sString = LoadFromString(IDS_RC_NORTHING);
        m_row_cache[2].second.sString = "DEPTH";
        m_row_cache[3].second.sString = "AHD";
        m_row_cache[4].second.sString = "TVDKB";
      }
    } else // row != 0
    {
      int pos = nRow - 1;
      if (m_arg.m_wellpathPoints == MESH_INTERSECTION_POINTS) {
        if (bHaveAllValues) {
          if (pos > m_wellPathValues[0].size() - 1)
            return;
        } else // no values selected; use point list iterator
        {
          const std::list<CNewWellPoint> &points_l = (const_cast<CNewWellPath *>(m_pNewWellPath))->MeshIntersecPoints();
          int size = points_l.size();
          if (pos > size - 1)
            return;
          m_iteratorWellPath = points_l.begin();
          std::advance(m_iteratorWellPath, pos);
        }
      } // MESH_INTERSECTION_POINTS

      pNewWellPoint = m_arg.m_wellpathPoints == INPUT_POINTS
                          ? new CNewWellPoint(m_pNewWellPath->WellPoint(pos))
                          : (bHaveAllValues ? &m_wellPathValues[0][pos].first : &*m_iteratorWellPath);

      point = *pNewWellPoint;

      m_row_cache[0].first = OT_DOUBLE;
      m_row_cache[1].first = OT_DOUBLE;
      m_row_cache[2].first = OT_DOUBLE;
      m_row_cache[3].first = OT_DOUBLE;
      m_row_cache[4].first = OT_DOUBLE;

      if (m_export2LAS) {
        m_row_cache[0].second.dDouble = ConvCoor(pNewWellPoint->Z(), Unit());
        m_row_cache[1].second.dDouble = ConvCoor(pNewWellPoint->Y(), Unit());
        m_row_cache[2].second.dDouble = ConvCoor(pNewWellPoint->X(), Unit());
        m_row_cache[3].second.dDouble = ConvCoor(pNewWellPoint->TVD(), Unit());

        m_row_cache[4].first = OT_DOUBLE;
        m_row_cache[5].first = OT_DOUBLE;

        CAngleQuantity azimuth, inclination;

        pNewWellPoint->AzimuthInclination(azimuth, inclination);

        m_row_cache[4].second.dDouble = inclination;
        m_row_cache[5].second.dDouble = azimuth;
      } else {
        m_row_cache[0].second.dDouble = ConvCoor(pNewWellPoint->Y(), Unit());
        m_row_cache[1].second.dDouble = ConvCoor(pNewWellPoint->X(), Unit());
        m_row_cache[2].second.dDouble = ConvCoor(pNewWellPoint->Z(), Unit());
        m_row_cache[3].second.dDouble = ConvCoor(pNewWellPoint->TMD(), Unit());
        m_row_cache[4].second.dDouble = ConvCoor(pNewWellPoint->TVD(), Unit());
      }
    }
  }

  m_data_offset = nCount;

  for (int i = 0; i < IExportFormat::ColumnSize(); i++) {
    if (nRow == 0) {
      assert(IExportFormat::OutputType(nRow, i) == OT_STRING);
      m_row_cache[nCount + i].first = OT_STRING;
      m_row_cache[nCount + i].second.sString = IExportFormat::StringAt(nRow, i);
    } else {
      assert(!point.Empty());
      // some results (e.g. wellpath axial strain) require the actual wellpoint,
      // so provide it if available
      const geo::IPoint *pPoint = &point;
      if (pNewWellPoint)
        pPoint = pNewWellPoint;
      else if (pElementPoint)
        pPoint = pElementPoint;
      if (bHaveAllValues) // wellpath
        m_row_cache[nCount + i] = DataOutput(m_wellPathValues[i][nRow - 1].second);
      else // other
        m_row_cache[nCount + i] = DataOutput(DataValue(i).ScalarData().ValuePoint(*pPoint, Unit()));
    }
  }

  if (m_bAddFormationNames) {
    if (nRow == 0) {
      m_row_cache[nCount + IExportFormat::ColumnSize()].first = OT_STRING;
      m_row_cache[nCount + IExportFormat::ColumnSize()].second.sString = "FormationName";
    } else {
      m_row_cache[nCount + IExportFormat::ColumnSize()].first = OT_STRING;
      assert(!point.Empty());

      if (pNewWellPoint && pNewWellPoint->Formation()) {
        m_row_cache[nCount + IExportFormat::ColumnSize()].second.sString = pNewWellPoint->Formation()->Name();
      } else {
        const geo::IPoint *pPoint = &point;
        if (pNewWellPoint)
          pPoint = pNewWellPoint;
        else if (pElementPoint)
          pPoint = pElementPoint;
        std::vector<int> vcElementIndices = Model().Mesh().Mesh().ElementsAt(*pPoint);
        if (vcElementIndices.empty())
          m_row_cache[nCount + IExportFormat::ColumnSize()].second.sString = "<NONE>";
        else {
          const geo::IElement &elm = Model().Mesh().Mesh().Element(vcElementIndices[0]); // pick first in list
          const CFormationBase &formation = *Model().Mesh().Formation(elm);
          QString strName = FixName(formation.Name());
          m_row_cache[nCount + IExportFormat::ColumnSize()].second.sString = strName;
        }
      }
    }
  }

  m_row_cache_index = nRow;
  if (pNewWellPoint && m_arg.m_wellpathPoints == INPUT_POINTS)
    delete pNewWellPoint;
}

void CNodalExportFormat::DestroyCache() const {
  m_row_cache.clear();
  m_row_cache_index = -1;
}

CNodalExportFormat::TOutputType CNodalExportFormat::OutputType(int nRow, int nColumn) const {
  if (nRow != m_row_cache_index)
    CreateCache(nRow);
  assert(nColumn >= 0);
  assert(nColumn < m_row_cache.size());
  return m_row_cache[nColumn].first;
}

QString CNodalExportFormat::StringAt(int nRow, int nColumn) const {
  if (nRow != m_row_cache_index)
    CreateCache(nRow);
  assert(nColumn >= 0);
  assert(nColumn < m_row_cache.size());
  assert(m_row_cache[nColumn].first == OT_STRING);
  return m_row_cache[nColumn].second.sString;
}

int CNodalExportFormat::IntegerAt(int nRow, int nColumn) const {
  if (nRow != m_row_cache_index)
    CreateCache(nRow);
  assert(nColumn >= 0);
  assert(nColumn < m_row_cache.size());
  assert(m_row_cache[nColumn].first == OT_INTEGER);
  return m_row_cache[nColumn].second.nInteger;
}

double CNodalExportFormat::DoubleAt(int nRow, int nColumn) const {
  if (nRow != m_row_cache_index)
    CreateCache(nRow);
  assert(nColumn >= 0);
  assert(nColumn < m_row_cache.size());
  assert(m_row_cache[nColumn].first == OT_DOUBLE);
  return m_row_cache[nColumn].second.dDouble;
}

int CNodalExportFormat::RowSize() const {
  if (m_pElementSetFormat)
    return m_pElementSetFormat->NodeSize() + 1;
  if (m_pPointSet)
    return m_pPointSet->PointSize() + 1;
  assert(m_pNewWellPath);
  {
    return (m_arg.m_wellpathPoints == INPUT_POINTS ? m_pNewWellPath->WellPointSize()
                                                   : m_pNewWellPath->MapValuesSize()) +
           1;
  }
}

int CNodalExportFormat::ColumnSize() const {
  int nRet = IExportFormat::ColumnSize();

  if (m_pElementSetFormat) {
    if (m_pElementSetFormat->TwoDimensional())
      nRet += 3;
    else
      nRet += 4;
  } else if (m_pPointSet) {
    for (unsigned int i = 0; i < m_pPointSet->Coordinates().ComponentSize(); i++)
      if (m_pPointSet->Coordinates().Component(i).Defined())
        nRet++;
  } else {
    assert(m_pNewWellPath);

    if (m_export2LAS) {
      nRet += WELL_PATH_LAS_COLUMNS;
    } else {
      nRet += WELL_PATH_XLS_COLUMNS;
    }
  }

  if (m_bAddFormationNames)
    ++nRet;

  return nRet;
}

const CModelBase &CNodalExportFormat::Model() const {
  if (m_pPointSet)
    return (CModelBase &)m_pPointSet->Model();
  if (m_pElementSetFormat)
    return (CModelBase &)m_pElementSetFormat->Model();
  assert(m_pNewWellPath);
  return (CModelBase &)m_pNewWellPath->Model();
}

CNodalExportFormat::UNIT CNodalExportFormat::Unit() const { return m_arg.m_unit; }

bool CNodalExportFormat::HasRowComment(int /*nRow*/) const { return false; }

QString CNodalExportFormat::RowComment(int /*nRow*/) const {
  // We don't use row comment
  return QString();
}

int CNodalExportFormat::getDataOffset() const { return m_data_offset; }

//////////////////////////////////////////////////////////////////////
// Implementation of IElementSetExportFormat
//////////////////////////////////////////////////////////////////////
IElementSetExportFormat::IElementSetExportFormat(const TDataVec &vcNodalData, const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IExportFormat(vcElementData, arg), m_nodal_format(*this, vcNodalData, arg), m_element_cache_index(-1) {}

IElementSetExportFormat::IElementSetExportFormat(const TValueTypeVec &vcNodalData,   // Data for the node section
                                                 const TValueTypeVec &vcElementData, // Data for the element section
                                                 const CExportArg &arg)              // Export options
    : IExportFormat(vcElementData, arg), m_nodal_format(*this, vcNodalData, arg), m_element_cache_index(-1) {}

void IElementSetExportFormat::InitElementNodeSize() const {
  assert(m_mpElementNodeSize.size() == 0);
  int nCurrentNodeSize = -1;
  for (int nElement = 0; nElement < ElementSize(); nElement++) {
    const geo::IElement &element = Element(nElement);
    if (element.NrOfPoints() != nCurrentNodeSize) {
      VERIFY(m_mpElementNodeSize.insert(TElementNodeSizeMap::value_type(nElement, element.NrOfPoints())).second);
      nCurrentNodeSize = element.NrOfPoints();
    }
  }
}

void IElementSetExportFormat::CreateCache(int nElementIndex) const {
  assert(nElementIndex != m_element_cache_index);
  // Resize the cache if nessecarry
  if (m_element_cache.size() != ColumnSize())
    m_element_cache.resize(ColumnSize());

  for (size_t i = 0; i < m_element_cache.size(); i++) {
    if (m_element_cache[i].size() != Element(nElementIndex).NrOfPoints())
      m_element_cache[i].resize(Element(nElementIndex).NrOfPoints());
  }

  // Fill the cache
  for (size_t i = 0; i < DataValueSize(); i++) {
    IValueDomainScalar::TValueVec vcValue(Element(nElementIndex).NrOfPoints());
    const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&Element(nElementIndex));
    const IResultComponent *pResult = dynamic_cast<const IResultComponent *>(&DataValue(i));
    if (pResult && pBody)
      vcValue = pResult->ValueElement(Element(nElementIndex), m_nodal_format.Unit());
    else
      vcValue = DataValue(i).ScalarData().ValueElement(Element(nElementIndex), m_nodal_format.Unit());

    // Put it in the data in a vector
    for (size_t j = 0; j < vcValue.size(); j++) {
      m_element_cache[i + 2][j] = DataOutput(vcValue[j]);
    }
  }

  for (int i = 0; i < Element(nElementIndex).NrOfPoints(); i++) {
    m_element_cache[0][i].first = OT_INTEGER;
    if (m_arg.m_bOldFormat) {
      m_element_cache[0][i].second.nInteger = i;
    } else {
      if (Element(nElementIndex).IndexingElementSet())
        m_element_cache[0][i].second.nInteger = Element(nElementIndex).Index() + 1;
      else
        m_element_cache[0][i].second.nInteger = nElementIndex + 1;
    }
    m_element_cache[1][i].first = OT_INTEGER;
    if (m_arg.m_bOldFormat) {
      m_element_cache[1][i].second.nInteger = ElementNodeIndex(nElementIndex, i);
    } else {
      m_element_cache[1][i].second.nInteger = ElementNodeIndex(nElementIndex, i) + 1;
    }
  }

  m_element_cache_index = nElementIndex;
}

void IElementSetExportFormat::DestroyCache() const {
  m_element_cache.clear();
  m_element_cache_index = -1;
}

const CNodalExportFormat &IElementSetExportFormat::NodalExportFormat() const { return m_nodal_format; }

IElementSetExportFormat::TElementIndex IElementSetExportFormat::ElementIndex(int nRow) const {
  assert(ElementSize() > 0);
  TElementIndex ret(0, 0);
  int nLowerRowBound = 0;
  int nUpperRowBound = 0;

  // Calculate node map if nescesarry
  if (m_mpElementNodeSize.size() == 0)
    InitElementNodeSize();
  TElementNodeSizeMap::const_iterator it = m_mpElementNodeSize.begin();
  do {
    int nFirstIndex = it->first;
    int nNodeSize = it->second;
    it++;
    int nLastIndex = ElementSize();
    if (it != m_mpElementNodeSize.end())
      nLastIndex = it->first;
    assert(nFirstIndex < nLastIndex);
    nUpperRowBound += (nLastIndex - nFirstIndex) * nNodeSize;
    if (((nRow - 1) < nUpperRowBound) && (nLowerRowBound <= (nRow - 1))) {
      ret.first = nFirstIndex + ((nRow - 1) - nLowerRowBound) / nNodeSize;
      ret.second = ((nRow - 1) - nLowerRowBound) % nNodeSize;
    }
    nLowerRowBound = nUpperRowBound;

  } while (it != m_mpElementNodeSize.end());

  return ret;
}

int IElementSetExportFormat::RowSize() const {
  int nRet = 0;
  if (ElementSize() > 0) {
    // Calculate node map if nescesarry
    if (m_mpElementNodeSize.size() == 0)
      InitElementNodeSize();
    TElementNodeSizeMap::const_iterator it = m_mpElementNodeSize.begin();
    do {
      int nFirstIndex = it->first;
      int nNodeSize = it->second;
      it++;
      int nLastIndex = ElementSize();
      if (it != m_mpElementNodeSize.end())
        nLastIndex = it->first;
      assert(nFirstIndex < nLastIndex);
      nRet += (nLastIndex - nFirstIndex) * nNodeSize;
    } while (it != m_mpElementNodeSize.end());
  }

  return nRet + 1;
}

int IElementSetExportFormat::ColumnSize() const { return IExportFormat::ColumnSize() + 2; }

IElementSetExportFormat::TOutputType IElementSetExportFormat::OutputType(int nRow, int nColumn) const {
  if (nRow == 0)
    return OT_STRING;
  TElementIndex index = ElementIndex(nRow);
  if (index.first != m_element_cache_index)
    CreateCache(index.first);
  assert(nColumn >= 0);
  assert(nColumn < m_element_cache.size());
  return m_element_cache[nColumn][index.second].first;
}

QString IElementSetExportFormat::StringAt(int nRow, int nColumn) const {
  if ((nRow == 0) && (nColumn == 0)) {
    if (m_arg.m_bOldFormat)
      return "NodeNr";
    else
      return "ElementNr";
  }
  if ((nRow == 0) && (nColumn == 1))
    return "NodeIndex";
  if ((nRow == 0) && (nColumn > 1))
    return IExportFormat::StringAt(nRow, nColumn - 2);

  TElementIndex index = ElementIndex(nRow);
  if (index.first != m_element_cache_index)
    CreateCache(index.first);
  assert(nColumn >= 0);
  assert(nColumn < m_element_cache.size());
  assert(m_element_cache[nColumn][index.second].first == OT_STRING);
  return m_element_cache[nColumn][index.second].second.sString;
}

int IElementSetExportFormat::IntegerAt(int nRow, int nColumn) const {
  TElementIndex index = ElementIndex(nRow);
  if (index.first != m_element_cache_index)
    CreateCache(index.first);
  assert(nColumn >= 0);
  assert(nColumn < m_element_cache.size());
  assert(m_element_cache[nColumn][index.second].first == OT_INTEGER);
  return m_element_cache[nColumn][index.second].second.nInteger;
}

double IElementSetExportFormat::DoubleAt(int nRow, int nColumn) const {
  TElementIndex index = ElementIndex(nRow);
  if (index.first != m_element_cache_index)
    CreateCache(index.first);
  assert(nColumn >= 0);
  assert(nColumn < m_element_cache.size());
  assert(m_element_cache[nColumn][index.second].first == OT_DOUBLE);
  return m_element_cache[nColumn][index.second].second.dDouble;
}

QString IElementSetExportFormat::ElementType() const {
  QString sType = Element(0).Type().c_str();
  if (sType.toUpper() == "HX24L")
    return "HEXA8";
  if (sType.toUpper() == "TE12L")
    return "TETRA4";
  if (sType.toUpper() == "Q24IF")
    return "IQUAD8";
  if (sType.toUpper() == "T18IF")
    return "ITRI6";
  if (sType.toUpper() == "T6EPS")
    return "TRI3"; // Triangle in 2D
  if (sType.toUpper() == "L8IF")
    return "ILINE4"; // 2D interface element
  if (dynamic_cast<const geo::ILine *>(&Element(0)) != 0)
    return "LINE2";

  if (dynamic_cast<const geo::ITriangle *>(&Element(0)) != 0)
    return "TRI3";

  if (dynamic_cast<const geo::IQuadrilateral *>(&Element(0)) != 0)
    return "QUAD4";

  return "UNKNOWN";
}

bool IElementSetExportFormat::HasRowComment(int /*nRow*/) const { return false; }

QString IElementSetExportFormat::RowComment(int /*nRow*/) const {
  assert(false);
  return QString();
}

const CHotSpot *IElementSetExportFormat::HotSpot() const { return m_nodal_format.HotSpot(); }

bool IElementSetExportFormat::isGVT() const { return m_nodal_format.isGVT(); }

//////////////////////////////////////////////////////////////////////
// Implementation of CElementSetExportFormat
//////////////////////////////////////////////////////////////////////

/*!
  Add an element set to the format.
  \param mpNode      The local node map to collect nodes
  \param element_set The element set to export
  \param sName       The name of the elementset
*/
void CElementSetExportFormat::AddElementSet(TNodeMap &mpNode, const geo::IElementSet &element_set,
                                            const QString &sName) {
  // assert( m_vcNode.size() == 0 ); //wjrx mantis 2950 This allowed now!
  m_vcElementSet.push_back(TElementSet(&element_set, sName));
  for (int e = 0; e < element_set.ElementSize(); e++) {
    const geo::IElement &element = element_set.Element(e);
    for (int n = 0; n < element.NrOfPoints(); n++) {
      mpNode.insert(TNode(element.PointIndex(n), &element.Point(n)));
    }
  }
}

/*!
  Add the node in the set to the node vector.
*/
void CElementSetExportFormat::AddNodes(const TNodeMap &mpNode) {
  assert((m_vcNode.size() == 0) || (m_vcElementSet.size() > 1));
  for (TNodeMap::const_iterator it = mpNode.begin(); it != mpNode.end(); it++) {
    m_vcNode.push_back(*it);
  }
}

// Member function to allow easy access to the TNode type.

bool CElementSetExportFormat::sortNodeOnIndex(const TNode &lhs, const TNode &rhs) { return lhs.first < rhs.first; }

CElementSetExportFormat::CElementSetExportFormat(const IElementSet &element_set, const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(TDataVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(element_set.Model())),
      m_bTwoDimensional(element_set.Dimension() == IPointSet::DIM_2D), m_bUseMesh(true), m_bAddFormationNames(false) {
  TNodeMap mpNode;
  AddElementSet(mpNode, element_set.ElementSet(), element_set.Name().toStdString().c_str());
  AddNodes(mpNode);
}

CElementSetExportFormat::CElementSetExportFormat(const IElementSet &element_set, const TValueTypeVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(TValueTypeVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(element_set.Model())),
      m_bTwoDimensional(element_set.Dimension() == IPointSet::DIM_2D), m_bUseMesh(true), m_bAddFormationNames(false) {
  TNodeMap mpNode;
  AddElementSet(mpNode, element_set.ElementSet(), element_set.Name().toStdString().c_str());
  AddNodes(mpNode);
}

CElementSetExportFormat::CElementSetExportFormat(const TFormationVec &vcFormation, const TDataVec &vcNodalData,
                                                 const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(vcNodalData, vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>((*vcFormation.begin())->Model())), m_bUseMesh(true),
      m_bAddFormationNames(arg.m_bFormationNames) {
  m_bTwoDimensional = false;

  TNodeMap mpNode;
  // Walk over formations and fill data
  for (size_t i = 0; i < vcFormation.size(); i++) {
    const CFormationBase &formation = *vcFormation[i];
    for (int j = 0; j < formation.ElementSetSize(); j++) {
      AddElementSet(mpNode, formation.ElementSet(j).ElementSet(), formation.Name().toStdString().c_str());
    }
  }
  AddNodes(mpNode);

  assert(m_vcElementSet.size() > 0);
}

CElementSetExportFormat::CElementSetExportFormat(const CFormationBase &formation, const TDataVec &vcNodalData,
                                                 const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(vcNodalData, vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(formation.Model())), m_bUseMesh(true),
      m_bAddFormationNames(arg.m_bFormationNames) {
  m_bTwoDimensional = false;

  TNodeMap mpNode;
  for (int i = 0; i < formation.ElementSetSize(); i++) {
    AddElementSet(mpNode, formation.ElementSet(i).ElementSet(), formation.Name().toStdString().c_str());
  }
  AddNodes(mpNode);

  assert(m_vcElementSet.size() > 0);
}

CElementSetExportFormat::CElementSetExportFormat(const CFormationPlane &formation_plane, const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(TDataVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(formation_plane.Model())), m_bUseMesh(true),
      m_bAddFormationNames(false) {
  m_bTwoDimensional = false;

  TNodeMap mpNode;
  AddElementSet(mpNode, formation_plane.PlaneSurface(), formation_plane.Name().toStdString().c_str());
  AddNodes(mpNode);

  assert(m_vcElementSet.size() > 0);
}

// wjrx mantis 2950
CElementSetExportFormat::CElementSetExportFormat(const TFormationPlaneVec &formation_plane_vec,
                                                 const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(TDataVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(formation_plane_vec[0]->Model())), m_bUseMesh(true),
      m_bAddFormationNames(false) {
  m_bTwoDimensional = false;

  for (size_t ii = 0; ii < formation_plane_vec.size(); ++ii) {
    TNodeMap mpNode;
    AddElementSet(mpNode, formation_plane_vec[ii]->PlaneSurface(),
                  formation_plane_vec[ii]->Name().toStdString().c_str());
    AddNodes(mpNode);

    assert(m_vcElementSet.size() > 0);
  }
}

/*!
  Exports the interface element group of an horizon.
*/
CElementSetExportFormat::CElementSetExportFormat(const THorizonVec &vcHorizon, const TDataVec &vcNodalData,
                                                 const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(vcNodalData, vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(vcHorizon[0]->Model())), m_bAddFormationNames(false) {
  m_bTwoDimensional = false;

  TNodeMap mpNode;
  for (size_t i = 0; i < vcHorizon.size(); i++) {
    const CHorizonBase &horizon = *vcHorizon[i];

    if (horizon.Slip()) {
      assert(horizon.InterfaceElementGroup());
      AddElementSet(mpNode, *horizon.InterfaceElementGroup(), horizon.Name().toStdString().c_str());
      m_bUseMesh = true;
    } else {
      const C3DHorizon &hor3D = dynamic_cast<const C3DHorizon &>(horizon);
      AddElementSet(mpNode, hor3D.AsElementSet(), hor3D.Name().toStdString().c_str());
      m_bUseMesh = false;
    }
  }
  AddNodes(mpNode);

  assert(m_vcElementSet.size() > 0);
}

/*!
  Exports RTCI data
*/
CElementSetExportFormat::CElementSetExportFormat(const CElementSetExportFormat::TRTCIVec &vcRTCI,
                                                 const IExportFormat::TDataVec & /*vcNodalData*/,
                                                 const IExportFormat::TDataVec &vcElementData,
                                                 const IExportFormat::CExportArg &arg)
    : IElementSetExportFormat(TDataVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(vcRTCI[0]->Model())), m_bTwoDimensional(false), m_bUseMesh(false),
      m_bAddFormationNames(false) {
  for (size_t i = 0; i < vcRTCI.size(); ++i) {
    TNodeMap mpNode;
    AddElementSet(mpNode, vcRTCI[i]->PolyLine(), vcRTCI[i]->Name().toStdString().c_str());
    AddNodes(mpNode);
    assert(m_vcElementSet.size() > 0);
  }
}

/*!
  Exports a cross section.
*/
CElementSetExportFormat::CElementSetExportFormat(const CCrossSection &xsec, const TDataVec &vcNodalData,
                                                 const TDataVec &vcElementData,
                                                 const CExportArg &arg) // Arguments for the export
    : IElementSetExportFormat(vcNodalData, vcElementData, arg), m_model(dynamic_cast<const CModelBase &>(xsec.Model())),
      m_bUseMesh(true), m_bAddFormationNames(false) {
  m_bTwoDimensional = false;

  TNodeMap mpNode;
  AddElementSet(mpNode, *xsec.ExportSurface(), xsec.Name().toStdString().c_str());
  AddNodes(mpNode);

  assert(m_vcElementSet.size() > 0);
}

/*!
  Exports non-meshed surface data
*/
CElementSetExportFormat::CElementSetExportFormat(const TNonMeshedSurfaceVec &vcSurface,
                                                 const TDataVec & /*vcNodalData*/, const TDataVec &vcElementData,
                                                 const CExportArg &arg)
    : IElementSetExportFormat(TDataVec(), vcElementData, arg),
      m_model(dynamic_cast<const CModelBase &>(vcSurface[0]->Model())), m_bTwoDimensional(false), m_bUseMesh(false),
      m_bAddFormationNames(false) {
  for (size_t i = 0; i < vcSurface.size(); ++i) {
    TNodeMap mpNode;
    AddElementSet(mpNode, static_cast<const geo::IElementSet &>(vcSurface[i]->DisplayList(0)),
                  vcSurface[i]->Name().toStdString().c_str());
    AddNodes(mpNode);
    assert(m_vcElementSet.size() > 0);
  }
}

/*!
  Export the basic results
*/
void CElementSetExportFormat::sortNodeVector() { std::sort(m_vcNode.begin(), m_vcNode.end(), sortNodeOnIndex); }

QString CElementSetExportFormat::ExportType() const {
  assert(m_vcElementSet.size());

  if (HotSpot())
    return "HOTSPOT";
  if (dynamic_cast<const geo::ISurface *>(m_vcElementSet[0].first) && DataValueSize() == 0)
    return "SURFACE";
  if (NodeSize())
    return "ELEMENTSET";
  return "RESULTS";
}

int CElementSetExportFormat::NodeSize() const { return m_vcNode.size(); }

int CElementSetExportFormat::NodeIndex(int nIndex) const { return m_vcNode[nIndex].first + 1; }

bool CElementSetExportFormat::TwoDimensional() const { return m_bTwoDimensional; }

void CElementSetExportFormat::Node(geo::IPoint &target, int nIndex) const {
  if (m_arg.m_pResultRegister && m_bUseMesh) {
    assert(m_arg.m_pDeformation);
    int nPointIndex = m_vcNode[nIndex].first;
    m_arg.m_pResultRegister->Node(target, *m_arg.m_pDeformation, m_arg.m_antype, nPointIndex);
  } else {
    target = *m_vcNode[nIndex].second;
  }
}

int CElementSetExportFormat::ElementSize() const {
  int nRet = 0;
  for (size_t i = 0; i < m_vcElementSet.size(); i++) {
    nRet += m_vcElementSet[i].first->ElementSize();
  }

  return nRet;
}

bool CElementSetExportFormat::HasRowComment(int nRow) const {
  // wedx 26072007: Shell requested that row comments be removed from the export format
  // but it might be necessary to reintroduce them in the old format

  if (nRow > 0 && m_arg.m_bOldFormat)
    if (ElementIndex(nRow).second == 0)
      return true;

  return false;
}

QString CElementSetExportFormat::RowComment(int nRow) const {
  int nElement = ElementIndex(nRow).first;
  int nPos = 0;

  for (size_t i = 0; i < m_vcElementSet.size(); i++) {
    if ((nPos <= nElement) && (nElement < nPos + m_vcElementSet[i].first->ElementSize()))
      return m_vcElementSet[i].second;
    nPos += m_vcElementSet[i].first->ElementSize();
  }

  assert(false);
  return "";
}

int CElementSetExportFormat::ElementNodeIndex(int nElement, int nNode) const {
  const geo::IElement &element = Element(nElement);
  //  assert(element.IndexingElementSet());
  return element.PointIndex(nNode);
}

const geo::IElement &CElementSetExportFormat::Element(int nElement) const {
  int nPos = 0;

  for (size_t i = 0; i < m_vcElementSet.size(); i++) {
    if ((nPos <= nElement) && (nElement < nPos + m_vcElementSet[i].first->ElementSize()))
      return m_vcElementSet[i].first->Element(nElement - nPos);
    nPos += m_vcElementSet[i].first->ElementSize();
  }

  assert(false);

  geo::IElement *pBogus = 0;
  return *pBogus;
}

int CElementSetExportFormat::ColumnSize() const {
  if (m_bAddFormationNames)
    return IElementSetExportFormat::ColumnSize() + 1;

  return IElementSetExportFormat::ColumnSize();
}

CElementSetExportFormat::TOutputType CElementSetExportFormat::OutputType(int nRow, int nColumn) const {
  if (m_bAddFormationNames && nColumn == IElementSetExportFormat::ColumnSize())
    return OT_STRING;

  return IElementSetExportFormat::OutputType(nRow, nColumn);
}

QString CElementSetExportFormat::StringAt(int nRow, int nColumn) const {
  if (m_bAddFormationNames && nColumn == IElementSetExportFormat::ColumnSize()) {
    if (nRow == 0) {
      return QString("FormationName");
    } else {
      // return the (fixed) formation name
      const geo::IElement &elm = Element(ElementIndex(nRow).first);
      const CFormationBase &formation = *Model().Mesh().Formation(elm);
      QString strName = FixName(formation.Name().toStdString().c_str());

      return strName;
    }
  }

  return IElementSetExportFormat::StringAt(nRow, nColumn);
}

const CModelBase &CElementSetExportFormat::Model() const { return m_model; }

int CElementSetExportFormat::getDataOffset() const { return 2; }
