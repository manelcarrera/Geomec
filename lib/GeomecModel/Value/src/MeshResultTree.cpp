// MeshResultTree.cpp: implementation of the CMeshResultTree class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshResultTree.h"
#include "CrossSection.h"
#include "FieldFactor.h"
#include "FormationBase.h"
#include "GeoSurface.h"
#include "HorizonBase.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "NonMeshedSurface.h"
#include "ResultTree.h"
#include "WellCasingCement.h"
#include "WellCasingCementInterface.h"
#include "WellCasingRTCI.h"
#include "WellCasingSteel.h"

#include "HexaMesh.h"
#include "IMesh.h"
#include "NonMeshedSurface.h"
#include "ResultInfo.h"
#include "TetraMesh.h"
#include "TextTagUtils.h"
#include "ValueMapper.h"
#include "hexameshregion.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Implementation of CMeshResult::CMeshResultComponent::CResultValueSet
//////////////////////////////////////////////////////////////////////

CMeshResult::CMeshResultComponent::CResultValueSet::CResultValueSet(const CMeshResultComponent &component,
                                                                    const geo::IMesh &mesh, CQuantity::UNIT unit)
    : m_component(component), m_mesh(mesh), m_unit(unit) {}

const geo::IElementSet &CMeshResult::CMeshResultComponent::CResultValueSet::ElementSet() const { return m_mesh; }

CQuantity::UNIT CMeshResult::CMeshResultComponent::CResultValueSet::Unit() const { return m_unit; }

std::pair<double, double>
CMeshResult::CMeshResultComponent::CResultValueSet::MinMaxEdge(const geo::IElement &element) const {
  std::pair<double, double> ret(DBL_MAX, -DBL_MAX);
  for (int i = 0; i < element.NrOfLines(); i++) {
    double dDistance = element.Line(i).Length();
    ret.first = std::min(ret.first, dDistance);
    ret.second = std::max(ret.second, dDistance);
  }
  return ret;
}

double CMeshResult::CMeshResultComponent::CResultValueSet::MeanEdge(const geo::IElement &element) const {
  double dSum = 0;
  for (int i = 0; i < element.NrOfLines(); i++)
    dSum += element.Line(i).Length();

  return dSum / element.NrOfLines();
}

void CMeshResult::CMeshResultComponent::CResultValueSet::ElementValues(
    TValueVec &values, int iElementIndex, geo::IParallelInitializationCallback * /*cb*/) const {
  assert(values.size() == m_mesh.Element(iElementIndex).NrOfPoints());
  const geo::IElement &mesh_element = ElementSet().Element(iElementIndex);

  // Depth gives us multiple values for each element
  if (m_component.Parent().Index() == RC_MR_DEPTH) {
    const CModelBase &model = dynamic_cast<const CModelBase &>(m_component.Model());
    for (int i = 0; i < mesh_element.NrOfPoints(); i++) {
      if (Unit() == CQuantity::SI_UNIT)
        values[i] = geo::CValue(model.Depth(mesh_element.Point(i)));
      else
        values[i] = geo::CValue(model.Depth(mesh_element.Point(i)) * FF_FACTOR_LENGTH);
    }

    return;
  }

  // Use this one when we have a single value
  geo::CValue value;

  const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&mesh_element);
  if (pBody) {
    // Calculate the volume
    if (m_component.Parent().Index() == RC_MR_VOLUME) {
      if (Unit() == CQuantity::SI_UNIT)
        value = geo::CValue(pBody->Volume());
      else
        value = geo::CValue(pBody->Volume() * pow(FF_FACTOR_LENGTH, 3));
    }

    if (m_component.Parent().Index() == RC_MR_SURFACE) {
      value = geo::CValue(0);
      for (int i = 0; i < pBody->NrOfFaces(); i++)
        value += geo::CValue(pBody->Face(i).Area());

      if (Unit() == CQuantity::FIELD_UNIT)
        value = value * geo::CValue(FF_FACTOR_LENGTH * FF_FACTOR_LENGTH);
    }

    if (m_component.Parent().Index() == RC_MR_SHAPE_FACTOR) {
      typedef std::set<int> TIndexSet;
      typedef std::map<int, TIndexSet> TLineMap;
      TLineMap mpLine;
      for (int i = 0; i < pBody->NrOfLines(); i++) {
        assert(pBody->LinePointIndices(i).size() == 2);
        // Insert line 1->2 and line 2->1
        TLineMap::iterator it = mpLine.insert(TLineMap::value_type(pBody->LinePointIndices(i)[0], TIndexSet())).first;
        it->second.insert(pBody->LinePointIndices(i)[1]);
        it = mpLine.insert(TLineMap::value_type(pBody->LinePointIndices(i)[1], TIndexSet())).first;
        it->second.insert(pBody->LinePointIndices(i)[0]);
      }

      // Now we have a line map
      for (TLineMap::iterator it = mpLine.begin(); it != mpLine.end(); it++) {
        assert(it->second.size() == 3);
        TIndexSet::const_iterator it_s = it->second.begin();
        geo::CVector v0(pBody->Point(*it_s), pBody->Point(it->first));
        it_s++;
        geo::CVector v1(pBody->Point(*it_s), pBody->Point(it->first));
        it_s++;
        geo::CVector v2(pBody->Point(*it_s), pBody->Point(it->first));
        geo::CVector normal(v1.CrossProduct(v2));
        geo::CValue val1(normal.Length() / (v0.Length() * v2.Length()));
        geo::CMatrix matrix(3, 3);
        matrix.Value(0, 0, v0.X());
        matrix.Value(0, 1, v1.X());
        matrix.Value(0, 2, v2.X());
        matrix.Value(1, 0, v0.Y());
        matrix.Value(1, 1, v1.Y());
        matrix.Value(1, 2, v2.Y());
        matrix.Value(2, 0, v0.Z());
        matrix.Value(2, 1, v1.Z());
        matrix.Value(2, 2, v2.Z());

        geo::CValue val2(fabs(matrix.GetDeterminant()) / (v0.Length() * normal.Length()));
        double dAngleVal = value.Min(val1.Min(val2)).Value();
        if (dAngleVal > 1.)
          dAngleVal = 1.;
        else if (dAngleVal < -1.)
          dAngleVal = -1.;
        value = geo::CValue((360 / (2 * PI)) * asin(dAngleVal));
      }
    }
  }

  const geo::IInterfaceElement *pInterface = dynamic_cast<const geo::IInterfaceElement *>(&mesh_element);
  if (pInterface) {
    if (m_component.Parent().Index() == RC_MR_SURFACE) {
      if (Unit() == CQuantity::SI_UNIT)
        value = geo::CValue(pInterface->Size());
      else
        value = geo::CValue(pInterface->Size() * FF_FACTOR_LENGTH * FF_FACTOR_LENGTH);
    }
  }

  const geo::IFace *pFace = dynamic_cast<const geo::IFace *>(&mesh_element);
  if (pFace) {
    if (m_component.Parent().Index() == RC_MR_SURFACE) {
      if (Unit() == CQuantity::SI_UNIT)
        value = geo::CValue(pFace->Area());
      else
        value = geo::CValue(pFace->Area() * FF_FACTOR_LENGTH * FF_FACTOR_LENGTH);
    }
  }

  if (m_component.Parent().Index() == RC_MR_MIN_ANGLE)
    value = geo::CValue(mesh_element.MinMaxAngles().first);

  if (m_component.Parent().Index() == RC_MR_MAX_ANGLE)
    value = geo::CValue(mesh_element.MinMaxAngles().second);

  if (m_component.Parent().Index() == RC_MR_MEAN_EDGE) {
    if (Unit() == CQuantity::SI_UNIT)
      value = geo::CValue(MeanEdge(mesh_element));
    else
      value = geo::CValue(MeanEdge(mesh_element) * FF_FACTOR_LENGTH);
  }

  if (m_component.Parent().Index() == RC_MR_MAX_EDGE) {
    if (Unit() == CQuantity::SI_UNIT)
      value = geo::CValue(MinMaxEdge(mesh_element).second);
    else
      value = geo::CValue(MinMaxEdge(mesh_element).second * FF_FACTOR_LENGTH);
  }

  if (m_component.Parent().Index() == RC_MR_EDGE_RATIO) {
    std::pair<double, double> min_max = MinMaxEdge(mesh_element);
    if (fabs(min_max.second) > EPS)
      value = geo::CValue(min_max.first / min_max.second);
  }

  for (size_t i = 0; i < values.size(); i++)
    values[i] = value;
}

void CMeshResult::CMeshResultComponent::CResultValueSet::Empty() { assert(false); }

bool CMeshResult::CMeshResultComponent::CResultValueSet::IsEmpty() const { return false; }

int CMeshResult::CMeshResultComponent::CResultValueSet::PushBack(const TValueVec & /*values*/) {
  assert(FALSE);
  return -1;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CMeshResult::CMeshResultComponent
//////////////////////////////////////////////////////////////////////

CMeshResult::CMeshResult(unsigned int uName, int result_type, CMeshResultTree &tree)
    : IValueComposite(uName, tree.Model()) {
  Index(result_type);
  new CMeshResultComponent(uName, *this);
  reParent(&tree);
}

CMeshResult::~CMeshResult() {}

bool CMeshResult::Empty() const { return false; }

long CMeshResult::SavedItems() const { return 0; }

unsigned int CMeshResult::IconId() const { return IDI_RESULT; }

unsigned int CMeshResult::TypeId() const { return Index(); }

bool CMeshResult::Less(const CGraphNode &node) const {
  const CMeshResult *pResult = dynamic_cast<const CMeshResult *>(&node);
  if (pResult)
    return Index() < pResult->Index();
  return IValueComposite::Less(node);
}

bool CMeshResult::isMeshResult() const { return true; }

bool CMeshResult::CanExport() const {
  const CModelBase *pModel = dynamic_cast<const CModelBase *>(&Model());
  assert(pModel);
  return pModel->IsMesh();
}

bool CMeshResult::ValidName(const std::string &name, CResultInfo &resultInfo) const {
  std::string meshResultName = Component(0).ExportLabel().toStdString();
  if (meshResultName == name) {
    resultInfo.Result(this);
    return true;
  }

  return false;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CMeshResultComponent
//////////////////////////////////////////////////////////////////////

CMeshResult::CMeshResultComponent::CMeshResultComponent(unsigned int uName, CMeshResult &parent)
    : IValueComponentBase(uName, parent), m_pSIValueSet(0), m_pFieldValueSet(0), m_Data(*this) {}

CMeshResult::CMeshResultComponent::~CMeshResultComponent() {
  delete m_pFieldValueSet;
  delete m_pSIValueSet;
}

IValueComponentBase::TValueComponentType CMeshResult::CMeshResultComponent::Type() const {
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar &CMeshResult::CMeshResultComponent::ScalarData() const { return m_Data; }

IValueDataInterfaceScalar &CMeshResult::CMeshResultComponent::ScalarData() { return m_Data; }

unsigned int CMeshResult::CMeshResultComponent::IconId() const { return IDI_RESULT; }

unsigned int CMeshResult::CMeshResultComponent::TypeId() const { return 0; }

QString CMeshResult::CMeshResultComponent::UnitName(const UNIT unit) const {
  QString sRet;
  if (unit == CQuantity::FIELD_UNIT) {
    switch (Parent().Index()) {
    case RC_MR_VOLUME:
      sRet = getStringTableEntry(IDS_UNIT_FIELD_VOLUME);
      break;
    case RC_MR_DEPTH:
    case RC_MR_NORTHING:
    case RC_MR_EASTING:
    case RC_MR_MAX_EDGE:
    case RC_MR_MEAN_EDGE:
    case RC_MR_AHD:
      sRet = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);
      break;
    case RC_MR_SURFACE:
      sRet = getStringTableEntry(IDS_UNIT_FIELD_AREA);
      break;
    case RC_MR_EDGE_RATIO:
      break;

    case RC_MR_MAX_ANGLE:
    case RC_MR_SHAPE_FACTOR:
    case RC_MR_MIN_ANGLE:
    case RC_MR_AZIMUTH:
    case RC_MR_INCLINATION:
      sRet = getStringTableEntry(IDS_UNIT_FIELD_ANGLE);
      break;
    default:
      assert(FALSE);
      break;
    };
  } else {
    switch (Parent().Index()) {
    case RC_MR_VOLUME:
      sRet = getStringTableEntry(IDS_UNIT_SI_VOLUME);
      break;
    case RC_MR_DEPTH:
    case RC_MR_NORTHING:
    case RC_MR_EASTING:
    case RC_MR_MAX_EDGE:
    case RC_MR_MEAN_EDGE:
    case RC_MR_AHD:
      sRet = getStringTableEntry(IDS_UNIT_SI_LENGTH);
      break;
    case RC_MR_SURFACE:
      sRet = getStringTableEntry(IDS_UNIT_SI_AREA);
      break;
    case RC_MR_EDGE_RATIO:
      break;
    case RC_MR_MAX_ANGLE:
    case RC_MR_SHAPE_FACTOR:
    case RC_MR_MIN_ANGLE:
    case RC_MR_AZIMUTH:
    case RC_MR_INCLINATION:
      sRet = getStringTableEntry(IDS_UNIT_SI_ANGLE);
      break;
    default:
      assert(FALSE);
      break;
    };
  }

  return sRet;
}

const geo::IValueSet &CMeshResult::CMeshResultComponent::ValueSet(const CQuantity::UNIT unit) const {
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  if (unit == CQuantity::SI_UNIT) {
    if (m_pSIValueSet == 0)
      m_pSIValueSet = new CResultValueSet(*this, model.Mesh().Mesh(), unit);
    return *m_pSIValueSet;
  }

  if (m_pFieldValueSet == 0)
    m_pFieldValueSet = new CResultValueSet(*this, model.Mesh().Mesh(), unit);
  return *m_pFieldValueSet;
}

QString CMeshResult::CMeshResultComponent::ExportLabel() const {
  QString sLabel;
  switch (Parent().Index()) {
  case RC_MR_VOLUME:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_VOLUME);
    break;
  case RC_MR_DEPTH:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_DEPTH);
    break;
  case RC_MR_MAX_EDGE:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_MAX_EDGE);
    break;
  case RC_MR_MEAN_EDGE:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_AVERAGE_EDGE);
    break;
  case RC_MR_SURFACE:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_AREA);
    break;
  case RC_MR_EDGE_RATIO:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_MIN_MAX_EDGE);
    break;
  case RC_MR_MAX_ANGLE:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_MAX_ANGLE);
    break;
  case RC_MR_MIN_ANGLE:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_MIN_ANGLE);
    break;
  case RC_MR_SHAPE_FACTOR:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_SHAPE_FACTOR);
    break;
  case RC_MR_NORTHING:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_NORTHING);
    break;
  case RC_MR_EASTING:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_EASTING);
    break;
  case RC_MR_AHD:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_AHD);
    break;
  case RC_MR_AZIMUTH:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_AZIMUTH);
    break;
  case RC_MR_INCLINATION:
    sLabel = getStringTableEntry(IDS_MESH_RESULT_INCLINATION);
    break;
  default:
    assert(FALSE);
  }
  return sLabel;
}

bool CMeshResult::CMeshResultComponent::Defined() const {
  const CModelBase *pModel = dynamic_cast<const CModelBase *>(&Model());
  assert(pModel);

  return (Parent().Index() == RC_MR_DEPTH) || (Parent().Index() == RC_MR_NORTHING) ||
         (Parent().Index() == RC_MR_EASTING) || (Parent().Index() == RC_MR_SURFACE) || (pModel->IsMesh());
}

IValueDomainScalar::TMinMax CMeshResult::CMeshResultComponent::MinMax(IProgressBase & /*progressBase*/,
                                                                      const CQuantity::UNIT /*unit*/) const {
  return IValueDomainScalar::TMinMax();
}

IValueDomainScalar::TValue CMeshResult::CMeshResultComponent::Average(IProgressBase &, const CQuantity::UNIT) const {
  return IValueDomainScalar::TValue();
}

bool CMeshResult::CMeshResultComponent::Empty() const { return false; }

long CMeshResult::CMeshResultComponent::SavedItems() const {
  assert(FALSE);
  return 0;
}

void CMeshResult::CMeshResultComponent::AddEdges(const geo::IFace &face, TLineSet &stLine) const {
  // Create a set with unique edges
  for (int i = 0; i < face.NrOfPoints(); i++) {
    TLine line;
    if (i == 0) {
      line.insert(&face.Point(face.NrOfPoints() - 1));
      line.insert(&face.Point(0));
    } else {
      line.insert(&face.Point(i - 1));
      line.insert(&face.Point(i));
    }
    stLine.insert(line);
  }
}

IValueDomainScalar::TValue
CMeshResult::CMeshResultComponent::ValuePoint(const geo::IPoint &pt, const UNIT unit,
                                              geo::IParallelInitializationCallback *cb) const {
  if (Parent().Index() == RC_MR_AHD || Parent().Index() == RC_MR_AZIMUTH || Parent().Index() == RC_MR_INCLINATION) {
    const CNewWellPoint *wp = dynamic_cast<const CNewWellPoint *>(&pt);

    if (!wp) {
      const CElementWellPoint *ewp = dynamic_cast<const CElementWellPoint *>(&pt);
      if (ewp)
        wp = dynamic_cast<const CNewWellPoint *>(&ewp->WellPoint());
    }

    if (wp) {
      if (Parent().Index() == RC_MR_AHD) {
        double dVal = wp->TMD();
        if (unit != CQuantity::SI_UNIT)
          dVal *= FF_FACTOR_LENGTH;
        return dVal;
      }

      CAngleQuantity azi, inc;
      wp->AzimuthInclination(azi, inc);

      if (Parent().Index() == RC_MR_AZIMUTH)
        return azi.Value(unit);

      assert(Parent().Index() == RC_MR_INCLINATION);

      return inc.Value(unit);
    }

    // for now, as we currently only accept wellpaths
    return geo::CValue();
  }

  // These value are not available for this result
  if (Parent().Index() != RC_MR_DEPTH && Parent().Index() != RC_MR_NORTHING && Parent().Index() != RC_MR_EASTING)
    return ValueSet(unit).ValuePoint(pt, cb);

  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  double dVal = 0;

  switch (Parent().Index()) {
  case RC_MR_DEPTH:
    dVal = model.Depth(pt);
    break;
  case RC_MR_NORTHING:
    dVal = model.Northing(pt);
    break;
  case RC_MR_EASTING:
    dVal = model.Easting(pt);
    break;
  default:
    assert(false);
  }

  if (unit != CQuantity::SI_UNIT)
    dVal *= FF_FACTOR_LENGTH;

  return IValueDomainScalar::TValue(dVal);
}

bool CMeshResult::CMeshResultComponent::CanComputeOnPoints() const {
  switch (Parent().Index()) {
  case RC_MR_NORTHING:
  case RC_MR_EASTING:
  case RC_MR_DEPTH:
    return true;
  default:
    return false;
  }
}

bool CMeshResult::CMeshResultComponent::PrepareMapping(const geo::IElementSet *pElementSet) {
  if (!(Parent().Index() == RC_MR_DEPTH || Parent().Index() == RC_MR_NORTHING || Parent().Index() == RC_MR_EASTING ||
        Parent().Index() == RC_MR_SURFACE)) {
    ValueSet(CDoubleQuantity::SI_UNIT);
    ValueSet(CDoubleQuantity::FIELD_UNIT);

    bool bInitialized = false;

    if (pElementSet) {
      for (size_t i = 0; i < pElementSet->ElementSize() && !bInitialized; ++i) {
        const geo::IElement &elt = pElementSet->Element(i);
        if (elt.IsElement()) {
          if (elt.NrOfLines() > 0)
            elt.Line(0).Length();

          if (elt.IsBody()) {
            const geo::IBody &body = static_cast<const geo::IBody &>(elt);

            if (body.NrOfFaces() > 0) {
              body.FaceNodeIndices(body.Face(0));
              bInitialized = true;
            }
          }
        }
      }
    }
  }
  return true;
}

bool CMeshResult::CMeshResultComponent::CanMap(const COpenGLNode &node) const {
  const CModelBase &model = static_cast<const CModelBase &>(Model());

  // The depth result can map to everything
  if (Parent().Index() == RC_MR_DEPTH || Parent().Index() == RC_MR_NORTHING || Parent().Index() == RC_MR_EASTING)
    return dynamic_cast<const CHexaMeshRegionBase *>(&node) == 0;

  if (Parent().Index() == RC_MR_SURFACE && dynamic_cast<const CNonMeshedSurface *>(&node))
    return true;

  if (!model.IsMesh())
    return false;

  if (Parent().Index() == RC_MR_SURFACE) {
    if (dynamic_cast<const CHorizonBase *>(&node))
      return true;
    if (dynamic_cast<const CSurfaceBase *>(&node))
      return true;
  }

  // Other objects must have a relationship to the mesh ...
  if (dynamic_cast<const CFormationBase *>(&node))
    return true;
  if (dynamic_cast<const CCrossSection::CCSFormation *>(&node))
    return true;
  if (dynamic_cast<const IFormationElementSet *>(&node))
    return true;
  if (dynamic_cast<const CNewWellPath *>(&node)) // wjrx mantis 3440
    return true;
  if (dynamic_cast<const CWellCasingSteel *>(&node))
    return true;
  if (dynamic_cast<const CWellCasingCementInterface *>(&node))
    return true;
  if (dynamic_cast<const CWellCasingCement *>(&node))
    return true;
  if (dynamic_cast<const CWellCasingRTCI *>(&node))
    return true;
  if (dynamic_cast<const CHexaMesh *>(&node))
    return true;
  if (dynamic_cast<const CTetraMesh *>(&node))
    return true;

  return false;
}

IValueDomainScalar::TValueVec
CMeshResult::CMeshResultComponent::ValueElement(const geo::IElement &elm, const UNIT unit,
                                                geo::IParallelInitializationCallback *cb) const {
  assert(Defined());
  IValueDomainScalar::TValueVec vcRet(elm.NrOfPoints());

  bool bWellpathResult =
      Parent().Index() == RC_MR_AHD || Parent().Index() == RC_MR_AZIMUTH || Parent().Index() == RC_MR_INCLINATION;

  if (Parent().Index() == RC_MR_DEPTH || Parent().Index() == RC_MR_NORTHING || Parent().Index() == RC_MR_EASTING ||
      bWellpathResult) {
    for (int i = 0; i < vcRet.size(); i++)
      vcRet[i] = ValuePoint(elm.Point(i), unit);
  } else if (Parent().Index() == RC_MR_SURFACE) {
    double size = elm.Size();
    if (unit == CQuantity::FIELD_UNIT)
      size *= FF_FACTOR_LENGTH * FF_FACTOR_LENGTH;
    for (int i = 0; i < vcRet.size(); i++)
      vcRet[i] = size;
  } else {
    // Not a mesh element. Do element mapping
    ValueSet(unit).ElementValue(elm, vcRet, cb);
  }

  return vcRet;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CMeshResultTree
//////////////////////////////////////////////////////////////////////

CMeshResultTree::CMeshResultTree(CModelBase &model)
    : CGraphEntryTemp<CMeshResult>(MD_BASE_MESH_RESULT, IDI_RESULT_MAP, IDS_RG_MESH_RESULT, model) {
  new CMeshResult(IDS_RG_MESH_RESULT_NORTHING, RC_MR_NORTHING, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_EASTING, RC_MR_EASTING, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_DEPTH, RC_MR_DEPTH, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_MAX_EDGE, RC_MR_MAX_EDGE, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_MEAN_EDGE, RC_MR_MEAN_EDGE, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_SURFACE, RC_MR_SURFACE, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_VOLUME, RC_MR_VOLUME, *this);

  new CMeshResult(IDS_RG_MESH_RESULT_EDGE_RATIO, RC_MR_EDGE_RATIO, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_MAX_ANGLE, RC_MR_MAX_ANGLE, *this);
  new CMeshResult(IDS_RG_MESH_RESULT_MIN_ANGLE, RC_MR_MIN_ANGLE, *this);

  new CMeshResult(IDS_RG_MESH_RESULT_SHAPE_FACTOR, RC_MR_SHAPE_FACTOR, *this);

  if (!dynamic_cast<IWellModel *>(&model)) // for now we only do the wellpath view
  {
    new CMeshResult(IDS_RG_MESH_RESULT_AHD, RC_MR_AHD, *this);
    new CMeshResult(IDS_RG_MESH_RESULT_AZIMUTH, RC_MR_AZIMUTH, *this);
    new CMeshResult(IDS_RG_MESH_RESULT_INCLINATION, RC_MR_INCLINATION, *this);
  }
}

bool CMeshResultTree::CanExport() const {
  CModelBase &model = (CModelBase &)Model();
  return model.IsMesh();
}

std::set<std::string> CMeshResultTree::m_stExportLabels;

const std::set<std::string> &CMeshResultTree::getNames() {
  if (CMeshResultTree::m_stExportLabels.size() == 0) {
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_VOLUME));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_DEPTH));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_MAX_EDGE));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_AVERAGE_EDGE));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_AREA));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_MIN_MAX_EDGE));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_MAX_ANGLE));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_MIN_ANGLE));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_SHAPE_FACTOR));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_NORTHING));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_EASTING));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_AHD));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_AZIMUTH));
    CMeshResultTree::m_stExportLabels.insert(getStringTableEntry(IDS_MESH_RESULT_INCLINATION));
  }
  return CMeshResultTree::m_stExportLabels;
}

bool CMeshResultTree::ValidName(const std::string &name, CResultInfo &resultInfo) const {
  resultInfo.Clear();

  for (size_t ii = 0; ii < childSize(); ++ii) {
    const CMeshResult *pMeshResult = dynamic_cast<const CMeshResult *>(&childAt(ii));

    if (pMeshResult && pMeshResult->ValidName(name, resultInfo))
      return true;
  }

  return false;
}
