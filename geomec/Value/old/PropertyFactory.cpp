// PropertyFactory.cpp: implementation of the CPropertyFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif // _MSC_VER
#include "IValueSet.h"
#include "PointSet.h"
#include "PropertyFactory.h"
#include "geomec.h"
#include "result.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of IFormat
//////////////////////////////////////////////////////////////////////
CPropertyFactory::IFormat::IFormat(CPropertyFactory &factory) : m_factory(factory) {}

const CPropertyFactory &CPropertyFactory::IFormat::Factory() const { return m_factory; }

CPropertyFactory &CPropertyFactory::IFormat::Factory() { return m_factory; }

enum CToken::TokenRet CPropertyFactory::IFormat::ParseHeaderTag(const CString &sHeaderTag) {
  return Factory().ParseHeaderTag(sHeaderTag);
}

//////////////////////////////////////////////////////////////////////
// Implementation of INodalFormat
//////////////////////////////////////////////////////////////////////
CPropertyFactory::INodalFormat::INodalFormat(CPropertyFactory &factory) : IFormat(factory), m_nHeaderCount(0) {
  if (Factory().State() == WRITE_NONE) {
    for (CGraphNode::iterator it = Factory().PointSet().begin(); it != Factory().PointSet().end(); it++) {
      IValueComposite *pComposite = dynamic_cast<IValueComposite *>(*it);
      if (pComposite) {
        for (int nMode = 0; nMode < pComposite->ModeSize(); nMode++) {
          for (int nComponent = 0; nComponent < pComposite->ComponentSize(nMode); nComponent++) {
            IValueComponent *pComponent = &pComposite->Component(nComponent, nMode);
            if (pComponent->Defined()) {
              m_vcComponent.push_back(pComponent);
            } // Defined
          }
        }
      }
    }
  }
}

int CPropertyFactory::INodalFormat::ColumnSize() const {
  int nRet = 0;
  switch (Factory().State()) {
  case READ_NODES:
  case READ_ELEMENTS:
  case READ_READY:
    ASSERT(Factory().State() == READ_NODES);
    nRet = Factory().PointSet().Dimension();
    nRet += Factory().PointSet().NodalValueSetSize();
    break;
  case WRITE_NONE:
  case WRITE_HEADER:
  case WRITE_VALUE:
  case WRITE_READY:
    if (Factory().PointSet().Dimension() == IPointSet::DIM_3D)
      nRet = 3 + m_vcComponent.size();
    else
      nRet = 2 + m_vcComponent.size();
    break;
  default:
    ASSERT(FALSE);
    break;
  }

  return nRet;
}

int CPropertyFactory::INodalFormat::RowSize() const { return Factory().PointSet().PointSize(); }

CPropertyFactory::INodalFormat::TDoubleVec CPropertyFactory::INodalFormat::Row(int nRow) const {
  // Check format
  TDoubleVec vcRet;
  const geo::IPoint &point = Factory().PointSet().PointAt(nRow);
  vcRet.push_back(point.X());
  vcRet.push_back(point.Y());

  if (Factory().PointSet().Dimension() == IPointSet::DIM_3D) {
    vcRet.push_back(point.Z());
  }

  for (int i = 0; i < m_vcComponent.size(); i++) {
    const IValueComponent *pComponent = m_vcComponent[i];
    const CGeomecDoc *pDoc = dynamic_cast<const CGeomecDoc *>(&pComponent->Model().Document());
    ASSERT(pDoc);
    vcRet.push_back(pComponent->ValuePoint(point, pDoc->UnitNode().Unit()).Value());
  }

  ASSERT(vcRet.size() == ColumnSize());

  return vcRet;
}

void CPropertyFactory::INodalFormat::AppendRow(const TDoubleVec &vcRow) {
  if (Factory().Format() == NODAL) {
    ASSERT(Factory().State() == READ_NODES);
    int nDim = Factory().PointSet().Dimension();
    ASSERT(nDim == m_mpCoordinate.size());

    geo::CPoint point(vcRow[m_mpCoordinate[COORD_X]], vcRow[m_mpCoordinate[COORD_Y]]);
    TDoubleVec vcValue;
    int nStart = 2;
    if (Factory().PointSet().Dimension() == IPointSet::DIM_3D) {
      point.Z(vcRow[m_mpCoordinate[COORD_Z]]);
      nStart = 3;
    }

    while (nStart < vcRow.size()) {
      vcValue.push_back(vcRow[nStart]);
      nStart++;
    }

    CPointSet *pPointSet = dynamic_cast<CPointSet *>(&Factory().PointSet());
    pPointSet->PushBack(point, vcValue);
  }
}

CString CPropertyFactory::INodalFormat::ColumnLabel(int nColumn) const {
  if (nColumn == 0)
    return "NORTHING";
  if (nColumn == 1)
    return "EASTING";
  int nIndex = 0;
  if (Factory().PointSet().Dimension() == IPointSet::DIM_3D) {
    if (nColumn == 2)
      return "DEPTH";
    nIndex = nColumn - 3;
  } else
    nIndex = nColumn - 2;

  ASSERT(nIndex > -1);
  ASSERT(nIndex < m_vcComponent.size());

  return m_vcComponent[nIndex]->ExportLabel();
}

enum CToken::TokenRet CPropertyFactory::INodalFormat::ParseHeaderTag(const CString &sHeaderTag) {
  if (sHeaderTag.CompareNoCase("NODES") == 0) {
    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("Northing") == 0) {
    if (Factory().State() != CPropertyFactory::READ_NODES) {
      Factory().FormatError("Expected 'NODES' before 'NORTHING'");
      return CToken::FAIL;
    }
    m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_X, m_nHeaderCount));
    m_nHeaderCount++;
    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("Easting") == 0) {
    if (Factory().State() != CPropertyFactory::READ_NODES) {
      Factory().FormatError("Expected 'NODES' before 'EASTING'");
      return CToken::FAIL;
    }
    m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_Y, m_nHeaderCount));
    m_nHeaderCount++;
    return CToken::OK_CONTINUE;
  }

  if ((sHeaderTag.CompareNoCase("Depth") == 0) || (sHeaderTag.CompareNoCase("Z") == 0)) {
    if (Factory().State() != CPropertyFactory::READ_NODES) {
      Factory().FormatError("Expected 'NODES' before 'DEPTH'");
      return CToken::FAIL;
    }
    m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_Z, m_nHeaderCount));
    m_nHeaderCount++;
    if (!Factory().IsPointSet() && (Factory().Format() == NODAL)) {
      Factory().CreatePointSet(m_mpCoordinate.size(), 0);
    }
    return CToken::OK_CONTINUE;
  }

  if ((sHeaderTag.CompareNoCase("UNIT") == 0)) {
    return CToken::OK_CONTINUE;
  }

  if ((sHeaderTag.CompareNoCase("SI") == 0)) {
    Factory().Unit(CQuantity::SI_UNIT);
    return CToken::OK_CONTINUE;
  }

  if ((sHeaderTag.CompareNoCase("FIELD") == 0)) {
    Factory().Unit(CQuantity::FIELD_UNIT);
    return CToken::OK_CONTINUE;
  }

  if (!Factory().IsPointSet() && (Factory().Format() == NODAL)) {
    if ((m_mpCoordinate.find(COORD_X) == m_mpCoordinate.end()) ||
        (m_mpCoordinate.find(COORD_Y) == m_mpCoordinate.end())) {
      Factory().FormatError("Coordinates not defined before values");
      return CToken::FAIL;
    }

    Factory().CreatePointSet(m_mpCoordinate.size(), 0);
  }

  return IFormat::ParseHeaderTag(sHeaderTag);
}

void CPropertyFactory::INodalFormat::ColumnSize(int nSize) {
  // No header available
  ASSERT(Factory().State() == READ_NONE);
  ASSERT(Factory().Format() == UNDEFINED);
  ASSERT(nSize > 2);
  Factory().CreatePointSet(-1, nSize);

  // Default we create our point map
  ASSERT(m_mpCoordinate.size() == 0);
  m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_Y, 0));
  m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_X, 1));
  if (Factory().PointSet().Dimension() == IPointSet::DIM_3D)
    m_mpCoordinate.insert(TCoordinateMap::value_type(COORD_Z, 2));
}

//////////////////////////////////////////////////////////////////////
// Implementation of Property Factory
//////////////////////////////////////////////////////////////////////
void CPropertyFactory::DefineValueSet(UINT uHeaderID) {}
void CPropertyFactory::DefineComponent(UINT uHeaderID, UINT uValueTypeID, UINT uComponentIndex, UINT uMode) {
  CString sHeaderTag;
  sHeaderTag.Format(uHeaderID);

  TValueTypeMap::iterator iter = m_mpValueType.find(sHeaderTag);
  if (iter == m_mpValueType.end()) {
    // Insert the value type
    iter = m_mpValueType.insert(TValueTypeMap::value_type(sHeaderTag, TValueType(uValueTypeID, TComponentVec()))).first;
  }

  ASSERT(iter->second.first == uValueTypeID);

  TComponentVec vcComponent = iter->second.second;

  if (vcComponent.size() <= (uComponentIndex + 1))
    vcComponent.resize(uComponentIndex + 1);

  vcComponent[uComponentIndex] = uComponentIndex;

  iter->second.second = vcComponent;

  THeaderMap::iterator it = m_mpHeader.find(uValueTypeID);
  if (it == m_mpHeader.end()) {
    it = m_mpHeader.insert(THeaderMap::value_type(uValueTypeID, THeaderTagVec())).first;
  }

  ASSERT(it->first == uValueTypeID);

  TComponentVec vcHeaderTag = it->second;

  if (vcHeaderTag.size() <= (uComponentIndex + 1))
    vcHeaderTag.resize(uComponentIndex + 1);

  vcHeaderTag[uComponentIndex] = uHeaderID;

  it->second = vcHeaderTag;
}

CPropertyFactory::CPropertyFactory(CGraphModel &model)
    : CValueTypeFactory(model), m_state(READ_NONE), m_format(UNDEFINED), m_pPointSet(0), m_pNodalFormat(0),
      m_unit(CQuantity::SI_UNIT), m_pElementFormat(0) {
  m_pNodalFormat = new INodalFormat(*this);
}

CPropertyFactory::CPropertyFactory(const IPointSet &point_set, enum FORMAT format)
    : CValueTypeFactory(((IPointSet &)point_set).Model()), m_state(WRITE_NONE), m_format(format), m_pNodalFormat(0),
      m_unit(CQuantity::SI_UNIT), m_pElementFormat(0) {
  m_pPointSet = const_cast<IPointSet *>(&point_set);
  IElementSet *pElementSet = dynamic_cast<IElementSet *>(m_pPointSet);

  switch (format) {
  case UNDEFINED:
    ASSERT(FALSE); // Not an input format
    break;
  case NODAL:
    m_pNodalFormat = new INodalFormat(*this);
    break;
  case ELEMENT:
    ASSERT(FALSE);
    ASSERT(pElementSet);
    m_pNodalFormat = new INodalFormat(*this);
    //		m_pElementFormat = new
    break;
  }
}

CPropertyFactory::~CPropertyFactory() {
  delete m_pNodalFormat;
  delete m_pElementFormat;
}

IPointSet &CPropertyFactory::PointSet() {
  ASSERT(m_pPointSet);
  return *m_pPointSet;
}

const IPointSet &CPropertyFactory::PointSet() const {
  ASSERT(m_pPointSet);
  return *m_pPointSet;
}

BOOL CPropertyFactory::IsPointSet() const { return m_pPointSet != 0; }

BOOL CPropertyFactory::AddValueComposite(const IValueComposite &composite) {
  //	ASSERT((Status() == FACTORY_IDLE) || (Status() == FACTORY_WRITE));
  //	m_status = FACTORY_WRITE;
  //	m_vcComposite.push_back(const_cast<IValueComposite*>(&composite));
  ASSERT(FALSE);
  return TRUE;
}

enum CPropertyFactory::FORMAT CPropertyFactory::Format() const { return m_format; }

enum CPropertyFactory::STATE CPropertyFactory::State() const { return m_state; }

void CPropertyFactory::FormatError(const CString &sErrorMessage) { m_state = FORMAT_ERROR; }

const CPropertyFactory::IFormat &CPropertyFactory::Format(enum FORMAT format) const {
  switch (format) {
  case UNDEFINED:
    ASSERT(FALSE);
    break;
  case NODAL:
    ASSERT(m_pNodalFormat);
    return *m_pNodalFormat;
  case ELEMENT:
    ASSERT(Format() == ELEMENT);
    ASSERT(m_pElementFormat);
    return *m_pElementFormat;
    break;
  default:
    ASSERT(FALSE);
  }

  return *m_pElementFormat;
}

CPropertyFactory::IFormat &CPropertyFactory::Format(enum FORMAT format) {
  switch (format) {
  case UNDEFINED:
    ASSERT(FALSE);
    break;
  case NODAL:
    ASSERT(m_pNodalFormat);
    return *m_pNodalFormat;
  case ELEMENT:
    ASSERT(Format() == ELEMENT);
    ASSERT(m_pElementFormat);
    return *m_pElementFormat;
    break;
  default:
    ASSERT(FALSE);
  }

  return *m_pElementFormat;
}

int CPropertyFactory::CommentSize() const { return m_vcComment.size(); }

CString CPropertyFactory::Comment(int nIndex) const {
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < CommentSize());
  return m_vcComment[nIndex];
}

enum CToken::TokenRet CPropertyFactory::ParseHeaderTag(const CString &sHeaderTag) {
  if (sHeaderTag.CompareNoCase("ELEMENTSET") == 0) {
    FormatError("Element set not supported yet");
    return CToken::FAIL;

    if ((m_state != READ_NONE) || (m_format != UNDEFINED)) {
      FormatError("Format was already defined; not expecting ELEMENTSET");
      return CToken::FAIL;
    }

    m_format = ELEMENT;

    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("POINTSET") == 0) {
    if ((m_state != READ_NONE) || (m_format != UNDEFINED)) {
      //			ErrorMessage("Format was already defined; not expecting POINTSET");
      return CToken::FAIL;
    }

    m_format = NODAL;
    m_state = READ_NODES;

    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("NODES") == 0) {
    if (m_format == UNDEFINED) {
      //			ErrorMessage("Format was not defined; expecting POINTSET or ELEMENTSET before NODES");
      return CToken::FAIL;
    }

    m_state = READ_NODES;

    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("ELEMENTS") == 0) {
    if (m_format == UNDEFINED) {
      //			ErrorMessage("Format was not defined; expecting ELEMENTSET before ELEMENTS");
      return CToken::FAIL;
    }

    if (m_format == NODAL) {
      //			throw new CFormatException(CFormationException::FORMAT_ERROR,
      //									   "Format POINTSET not supporting the keyword ELEMENTS");
      return CToken::FAIL;
    }

    m_state = READ_ELEMENTS;

    return CToken::OK_CONTINUE;
  }

  if (sHeaderTag.CompareNoCase("END") == 0) {
    if (m_format == UNDEFINED) {
      //			throw new CFormatException(CFormationException::FORMAT_ERROR,
      //									   "Format was not defined before keyword END");
      return CToken::FAIL;
    }

    if (!((m_state == READ_ELEMENTS) || (m_state == READ_NODES))) {
      //			throw new CFormatException(CFormationException::FORMAT_ERROR,
      //									   "Elements or nodes not defined by END statement");
      return CToken::FAIL;
    }

    m_state = READ_READY;

    return CToken::OK_CONTINUE;
  }

  TValueTypeMap::iterator it = m_mpValueType.find(sHeaderTag);
  if (it != m_mpValueType.end()) {
    //		if(m_pPointSet == 0)
    //		{
    //			if(!CreatePointSet())
    //				return FALSE;
    //		}

    if (ValueTypeAvailable(it->second.first)) {
      CValueType *pValueType = BuildValueType(*m_pPointSet, it->second.first, m_pPointSet->Name());
      ASSERT(pValueType);
      for (int i = 0; i < pValueType->ComponentSize(); i++) {
        int nIndex = m_pPointSet->AddNodalValueSet();
        pValueType->Component(i).LinkTo(m_pPointSet->ValueSet(nIndex));
        return CToken::OK_CONTINUE;
      }
    }
  }

  //
  if (m_stValueSet.find(sHeaderTag) != m_stValueSet.end()) {
    //		if(m_pPointSet == 0)
    //		{
    //			if(!CreatePointSet())
    //				return FALSE;
    //		}

    int nIndex = m_pPointSet->AddNodalValueSet();
    m_pPointSet->ValueSet(nIndex).Name(sHeaderTag);
    return CToken::OK_CONTINUE;
  }

  return CToken::FAIL;
}

CString CPropertyFactory::HeaderTag(IValueComponent &component) const {
  CString sRet;
  THeaderMap::const_iterator it = m_mpHeader.find(component.Parent().TypeNameId());
  ASSERT(it != m_mpHeader.end());
  sRet.LoadString(it->second[component.ComponentIndex()]);
  return sRet;
}

BOOL CPropertyFactory::CreatePointSet(int nDim, int nNodalValueSetSize) {
  switch (Format()) {
  case UNDEFINED:
    if (nDim == -1) {
      if (nNodalValueSetSize == 3)
        nDim = 2;
      else
        nDim = OnSetPointSetDimension();

      nNodalValueSetSize = nNodalValueSetSize - nDim;

      m_state = READ_NODES;
    }
    break;
  case NODAL:
    break;
  default:
    ASSERT(FALSE);
    return FALSE;
    break;
  }

  ASSERT((nDim == 2) || (nDim == 3));

  if (nDim == 2)
    m_pPointSet = new CPointSet(OnSetPointSetName(), Model(), nNodalValueSetSize, CPointSet::DIM_2D);
  else
    m_pPointSet = new CPointSet(OnSetPointSetName(), Model(), nNodalValueSetSize, CPointSet::DIM_3D);

  m_format = NODAL;
  m_pPointSet->CoordinateUnit(m_unit);
  m_pPointSet->ValueUnit(m_unit);
  return TRUE;
}
