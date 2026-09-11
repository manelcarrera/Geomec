// GocadExport.cpp: implementation of the CGocadExport class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#if _MSC_VER <= 1200 // we don't support VS6.0 or before anymore
#error This version of the Visual Studio compiler is no longer supported
#endif
#endif // WIN32

#include <memory>

#include "3DFormation.h"
#include "3DHorizon.h"
#include "ElementGroup.h"
#include "FieldFactor.h"
#include "FormationVolume.h"
#include "Global.h"
#include "GlobalMessage.h"
#include "GocadExport.h"
#include "HorizonBase.h"
#include "IProgressFactory.h"
#include "ITetrahedron.h"
#include "ModelBase.h"
#include "SurfaceDesc.h"
#include "TetMeshBase.h"
#include "TetSurface.h"
#include "TetraHorizonBase.h"
#include "unitnode.h"

// (local) indices of the nodes of the 6 tetras in a hexa element
static const int hexa_tetra_index[6][4] = {{0, 1, 2, 5}, {0, 5, 2, 4}, {5, 6, 2, 4},
                                           {0, 2, 3, 4}, {4, 6, 2, 3}, {4, 7, 6, 3}};

// dummy for fast access in case tetra indices are needed
static const int tetra_tetra_index[4] = {0, 1, 2, 3};

///// CGocadExport::CObjectLess

template <class OBJECT>
bool CGocadExport::CObjectLess<OBJECT>::operator()(const OBJECT *pLhs, const OBJECT *pRhs) const {
  // trivial
  if (pLhs == pRhs)
    return false;

  // try Less methods first
  if (pLhs->Less(*pRhs))
    return true;
  if (pRhs->Less(*pLhs))
    return false;

  // names
  if (pLhs->Name() < pRhs->Name())
    return true;
  if (pRhs->Name() < pLhs->Name())
    return false;

  // last resort, pointer values
  return pLhs < pRhs;
}

///// CGocadExport::CNodeBase

CGocadExport::CNodeBase::CNodeBase() : m_nId(0) {}

CGocadExport::CNodeBase::CNodeBase(const CNodeBase &rhs) : m_nId(rhs.m_nId) {}

CGocadExport::CNodeBase::~CNodeBase() {}

int CGocadExport::CNodeBase::Id() const { return m_nId; }

void CGocadExport::CNodeBase::Id(int id) { m_nId = id; }

void CGocadExport::CNodeBase::AppendValues(FILE *fp, const TElementValueVec &vcElementValues, int nLocalIndex) const {
  for (size_t i = 0; i < vcElementValues.size(); ++i) {
    const IValueDomainScalar::TValueVec &vcValues = vcElementValues[i];
    assert(nLocalIndex >= 0 && nLocalIndex < vcValues.size());

    const geo::CValue &val = vcValues[nLocalIndex];
    double dVal = val.Valid() ? val.Value() : -99999;
    fprintf(fp, " %g", dVal);
  }
}

///// CGocadExport::CVertex

CGocadExport::CVertex::CVertex(const geo::INode &node) : m_node(node) {}

CGocadExport::CVertex::CVertex(const CVertex &rhs) : CNodeBase(rhs), m_node(rhs.m_node) {}

void CGocadExport::CVertex::Write(FILE *fp, int iUnit, const TElementValueVec &vcElementValues, int nLocalIndex) const {
  assert(Id() > 0);

  double x = (iUnit == 1) ? m_node.X() * FF_FACTOR_LENGTH : m_node.X();
  double y = (iUnit == 1) ? m_node.Y() * FF_FACTOR_LENGTH : m_node.Y();
  double z = (iUnit == 1) ? m_node.Z() * FF_FACTOR_LENGTH : m_node.Z();

  fprintf(fp, "%sVRTX %d %12f %12f %12f", vcElementValues.empty() ? "" : "P", Id(), y, x, z); // gocad swaps x and y!
  AppendValues(fp, vcElementValues, nLocalIndex);
  fprintf(fp, "\n");
}

///// CGocadExport::CAtom

CGocadExport::CAtom::CAtom(const CVertex &vertex) : m_vertex(vertex) {}

void CGocadExport::CAtom::Write(FILE *fp, int /*iUnit*/, const TElementValueVec &vcElementValues,
                                int nLocalIndex) const {
  assert(Id() > 0);
  fprintf(fp, "%sATOM %d %d", vcElementValues.empty() ? "" : "P", Id(), m_vertex.Id());
  AppendValues(fp, vcElementValues, nLocalIndex);
  fprintf(fp, "\n");
}

///// CGocadExport::CTetra

CGocadExport::CTetra::CTetra(const geo::IElement &element, CNodeBase **pNode) : m_element(element) {
  int i;
  for (i = 0; i < 4; ++i)
    m_pNode[i] = pNode[i];
}

CGocadExport::CTetra::~CTetra() {}

void CGocadExport::CTetra::Write(FILE *fp, const IValueDomainScalar::TValueVec &vcAverageElementValues,
                                 const QString *pstrFormationName) const {
  fprintf(fp, "TETRA");

  for (int i = 0; i < 4; ++i) {
    assert(m_pNode[i]->Id() > 0);
    fprintf(fp, " %d", m_pNode[i]->Id());
  }

  for (size_t i = 0; i < vcAverageElementValues.size(); ++i) {
    const geo::CValue &val = vcAverageElementValues[i];
    fprintf(fp, " %g", val.Valid() ? val.Value() : -99999);
  }

  if (pstrFormationName)
    fprintf(fp, " %s", pstrFormationName->toStdString().c_str());

  fprintf(fp, "\n");
}

const geo::IElement &CGocadExport::CTetra::Element() const { return m_element; }

///// CGocadExport::CTrgl

CGocadExport::CTrgl::CTrgl(CNodeBase **pNode) {
  int i;
  for (i = 0; i < 3; ++i)
    m_pNode[i] = pNode[i];
}

CGocadExport::CTrgl::~CTrgl() {
  int i;
  for (i = 0; i < 3; ++i)
    delete m_pNode[i];
}

void CGocadExport::CTrgl::Write(FILE *fp) const {
  fprintf(fp, "TRGL");
  int i;
  for (i = 0; i < 3; ++i) {
    assert(m_pNode[i]->Id() > 0);
    fprintf(fp, " %d", m_pNode[i]->Id());
  }

  fprintf(fp, "\n");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGocadExport::CGocadExport(CModelBase &model)
    : m_model(model), m_bLinear(model.ResultRegister().Linear()), m_bNonLinear(model.ResultRegister().NonLinear()),
      m_bHeat(model.ResultRegister().Heat()), m_bMixture(model.ResultRegister().Mixture()),
      m_bMixtureContainment(model.ResultRegister().MixtureContainment()), m_iUnit(0), m_bVectorESize(true),
      m_bTensorESize(false), m_bAverage(false), m_bSplitFormations(false), m_fp(0), m_bFormationNames(false) {
  // Intialize the unit
  CUnitNode unitNode;
  switch (unitNode.Unit()) {
  case CQuantity::SI_UNIT:
    m_iUnit = 0;
    break;
  case CQuantity::FIELD_UNIT:
    m_iUnit = 1;
    break;
  case CQuantity::SI_USER_UNIT:
  case CQuantity::US_UNIT:
    break;
  };
}

CGocadExport::CGocadExport(const CGocadExport &rhs)
    : m_model(rhs.m_model), m_stTimeSteps(rhs.m_stTimeSteps), m_stFormations(rhs.m_stFormations),
      m_stHorizons(rhs.m_stHorizons), m_stPointSets(rhs.m_stPointSets),
      m_stMaterialParameterComponents(rhs.m_stMaterialParameterComponents),
      m_stResultComponents(rhs.m_stResultComponents), m_bLinear(rhs.m_bLinear), m_bNonLinear(rhs.m_bNonLinear),
      m_bHeat(rhs.m_bHeat), m_bMixture(rhs.m_bMixture), m_bMixtureContainment(rhs.m_bMixtureContainment),
      m_iUnit(rhs.m_iUnit), m_bVectorESize(rhs.m_bVectorESize), m_bTensorESize(rhs.m_bTensorESize),
      m_bAverage(rhs.m_bAverage), m_bSplitFormations(rhs.m_bSplitFormations), m_fp(0),
      m_bFormationNames(rhs.m_bFormationNames) {}

CGocadExport::~CGocadExport() { ClearElements(); }

CGocadExport &CGocadExport::operator=(const CGocadExport &rhs) {
  assert(&m_model == &rhs.m_model);

  m_stTimeSteps = rhs.m_stTimeSteps;
  m_stPointSets = rhs.m_stPointSets;
  m_stFormations = rhs.m_stFormations;
  m_stHorizons = rhs.m_stHorizons;
  m_stMaterialParameterComponents = rhs.m_stMaterialParameterComponents;
  m_stResultComponents = rhs.m_stResultComponents;
  m_bLinear = rhs.m_bLinear;
  m_bNonLinear = rhs.m_bNonLinear;
  m_bHeat = rhs.m_bHeat;
  m_bMixture = rhs.m_bMixture;
  m_bMixtureContainment = rhs.m_bMixtureContainment;
  m_iUnit = rhs.m_iUnit;
  m_bVectorESize = rhs.m_bVectorESize;
  m_bTensorESize = rhs.m_bTensorESize;
  m_bAverage = rhs.m_bAverage;
  m_bSplitFormations = rhs.m_bSplitFormations;
  m_bFormationNames = rhs.m_bFormationNames;

  return *this;
}

bool CGocadExport::operator==(const CGocadExport &rhs) const {
  assert(&m_model == &rhs.m_model);

  return (m_stTimeSteps == rhs.m_stTimeSteps && m_stPointSets == rhs.m_stPointSets &&
          m_stFormations == rhs.m_stFormations && m_stHorizons == rhs.m_stHorizons &&
          m_stMaterialParameterComponents == rhs.m_stMaterialParameterComponents &&
          m_stResultComponents == rhs.m_stResultComponents && m_bLinear == rhs.m_bLinear &&
          m_bNonLinear == rhs.m_bNonLinear && m_bHeat == rhs.m_bHeat && m_bMixture == rhs.m_bMixture &&
          m_bMixtureContainment == rhs.m_bMixtureContainment && m_iUnit == rhs.m_iUnit &&
          m_bVectorESize == rhs.m_bVectorESize && m_bTensorESize == rhs.m_bTensorESize &&
          m_bAverage == rhs.m_bAverage && m_bSplitFormations == rhs.m_bSplitFormations &&
          m_bFormationNames == rhs.m_bFormationNames);
}

void CGocadExport::Export(const QString &sPath, const QString &title) {
  m_fp = fopen(sPath.toStdString().c_str(), "w");
  if (!m_fp) {
    QString msg;
    msg = QString("Failed to open file '%1' for writing").arg(sPath);
    _m()->msg(msg);
    return;
  }

  int nBodies = 0;
  int nFaces = 0;
  int nPoints = 0;

  for (TFormationSet::iterator it = m_stFormations.begin(); it != m_stFormations.end(); ++it) {
    const C3DFormation &form = (C3DFormation &)(**it);
    int nVol = form.VolumeSize();
    int i;
    for (i = 0; i < nVol; ++i) {
      const CFormationVolume &vol = form.Volume(i);
      const geo::CBodyGroup &bodies = vol.Volume();
      nBodies += bodies.BodySize();
    }
  }

  for (THorizonSet::iterator ith = m_stHorizons.begin(); ith != m_stHorizons.end(); ++ith) {
    const CHorizonBase &hor = **ith;
    assert(hor.Slip()); // should be a fault
    assert(dynamic_cast<const C3DHorizon *>(&hor));
    const C3DHorizon &hor3d = static_cast<const C3DHorizon &>(hor);
    nFaces += 2 * hor3d.BodyFaceSize(); // need two passes per horizon
  }

  for (TPointSetSet::iterator itp = m_stPointSets.begin(); itp != m_stPointSets.end(); ++itp) {
    const CPointSet &ptSet = **itp;
    nPoints += ptSet.PointSize();
  }

  std::auto_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Exporting GoCad file..."));
  prog->AddSteps(nBodies + nFaces + nPoints);

  SortComposites();

  try {
    // export solids
    if (SplitFormations()) {
      TFormationSet stFormations = m_stFormations;
      try {
        for (TFormationSet::iterator it = stFormations.begin(); it != stFormations.end(); ++it) {
          m_stFormations.clear();
          m_stFormations.insert(*it);
          ExportSolid(*prog, title);
        }
      } catch (CProgressCancel *) {
        m_stFormations = stFormations;
        throw;
      }

      m_stFormations = stFormations;
    } else {
      if (!m_stFormations.empty())
        ExportSolid(*prog, title);
    }

    // export surfaces
    for (THorizonSet::iterator ith = m_stHorizons.begin(); ith != m_stHorizons.end(); ++ith) {
      const C3DHorizon &hor = static_cast<const C3DHorizon &>(**ith);
      assert(hor.Slip());
      ExportFault(hor, *prog);
    }

    ExportVSets(*prog);
  } catch (CProgressCancel *c) {
    delete c;
  }

  fclose(m_fp);
}

void CGocadExport::ExportSolid(IProgressBase &prog, const QString &title) {
  WriteSolidHeader(title);
  WritePropertiesHeader(true, **m_stFormations.begin());
  WriteFormations(prog);
  WriteSolidSurfaces();
  WriteTrailer();
}

void CGocadExport::ExportFault(const C3DHorizon &hor, IProgressBase &prog) {
  WriteFaultHeader(hor);
  WritePropertiesHeader(false, hor);
  WriteFault(hor, prog);
  WriteTrailer();
}

void CGocadExport::ExportVSets(IProgressBase &prog) { WritePointSets(prog); }

const CModelBase &CGocadExport::Model() const { return m_model; }

CModelBase &CGocadExport::Model() { return m_model; }

void CGocadExport::AddDepletionStage(const CDepletionStage &stage) { m_stTimeSteps.insert(&stage); }

void CGocadExport::RemoveDepletionStage(const CDepletionStage &stage) { m_stTimeSteps.erase(&stage); }

bool CGocadExport::DepletionStage(const CDepletionStage &stage) const {
  return (m_stTimeSteps.find(&stage) != m_stTimeSteps.end());
}

void CGocadExport::AddPointSet(const CPointSet &ptSet) { m_stPointSets.insert(&ptSet); }

void CGocadExport::RemovePointSet(const CPointSet &ptSet) { m_stPointSets.erase(&ptSet); }

void CGocadExport::AddFormation(const CFormationBase &formation) { m_stFormations.insert(&formation); }

void CGocadExport::RemoveFormation(const CFormationBase &formation) { m_stFormations.erase(&formation); }

void CGocadExport::AddHorizon(const CHorizonBase &horizon) { m_stHorizons.insert(&horizon); }

void CGocadExport::RemoveHorizon(const CHorizonBase &horizon) { m_stHorizons.erase(&horizon); }

bool CGocadExport::PointSet(const CPointSet &ptSet) { return (m_stPointSets.find(&ptSet) != m_stPointSets.end()); }

bool CGocadExport::Formation(const CFormationBase &formation) const {
  return (m_stFormations.find(&formation) != m_stFormations.end());
}

bool CGocadExport::Horizon(const CHorizonBase &horizon) const {
  return (m_stHorizons.find(&horizon) != m_stHorizons.end());
}

bool CGocadExport::HasPointSets() const { return !m_stPointSets.empty(); }

bool CGocadExport::HasFormations() const { return !m_stFormations.empty(); }

bool CGocadExport::HasHorizons() const { return !m_stHorizons.empty(); }

bool CGocadExport::HasFaults() const {
  THorizonSet::const_iterator it;
  for (it = m_stHorizons.begin(); it != m_stHorizons.end(); ++it) {
    if ((*it)->Slip())
      return true;
  }

  return false;
}

const CFormationBase &CGocadExport::FirstFormation() const {
  assert(HasFormations());
  return **m_stFormations.begin();
}

const CHorizonBase &CGocadExport::FirstFault() const {
  const CHorizonBase *pHor = 0;

  THorizonSet::const_iterator it;
  for (it = m_stHorizons.begin(); it != m_stHorizons.end(); ++it) {
    if ((*it)->Slip()) {
      pHor = *it;
      break;
    }
  }

  return *pHor;
}

void CGocadExport::Linear(bool bOn) { m_bLinear = bOn; }

bool CGocadExport::Linear() const { return m_bLinear; }

void CGocadExport::NonLinear(bool bOn) { m_bNonLinear = bOn; }

bool CGocadExport::NonLinear() const { return m_bNonLinear; }

void CGocadExport::Heat(bool bOn) { m_bHeat = bOn; }

bool CGocadExport::Heat() const { return m_bHeat; }

void CGocadExport::Mixture(bool bOn) { m_bMixture = bOn; }

bool CGocadExport::Mixture() const { return m_bMixture; }

void CGocadExport::MixtureContainment(bool bOn) { m_bMixtureContainment = bOn; }

bool CGocadExport::MixtureContainment() const { return m_bMixtureContainment; }

void CGocadExport::Unit(int iUnit) { m_iUnit = iUnit; }

int CGocadExport::Unit() const { return m_iUnit; }

void CGocadExport::VectorESize(bool bOn) { m_bVectorESize = bOn; }

bool CGocadExport::VectorESize() const { return m_bVectorESize; }

void CGocadExport::TensorESize(bool bOn) { m_bTensorESize = bOn; }

bool CGocadExport::TensorESize() const { return m_bTensorESize; }

void CGocadExport::Average(bool bOn) { m_bAverage = bOn; }

bool CGocadExport::Average() const { return m_bAverage; }

void CGocadExport::SplitFormations(bool bOn) { m_bSplitFormations = bOn; }

bool CGocadExport::SplitFormations() const { return m_bSplitFormations; }

void CGocadExport::FormationNames(bool bOn) { m_bFormationNames = bOn; }

bool CGocadExport::FormationNames() const { return m_bFormationNames; }

QString CGocadExport::FixName(const QString &name) const {
  QString s(name);
  s = s.trimmed();

  int i;
  for (i = 0; i < s.length(); ++i) {
    if (s[i] == ' ')
      s[i] = '_';
  }

  return s;
}

void CGocadExport::ClearElements() {
  for (size_t i = 0; i < m_vcElements.size(); ++i)
    delete m_vcElements[i];

  m_vcElements.clear();

  for (TNodeMap::iterator it = m_mpNodes.begin(); it != m_mpNodes.end(); ++it)
    delete it->second;

  m_mpNodes.clear();
  m_stUniqueNodes.clear();
}

void CGocadExport::WriteAxisUnit() {
  assert(m_iUnit == 0 || m_iUnit == 1);
  QString sUnit;
  if (m_iUnit == 0)
    sUnit = "m";
  else
    sUnit = "ft";
  fprintf(m_fp, "AXIS_UNIT \"%s\" \"%s\" \"%s\"\n", sUnit.toStdString().c_str(), sUnit.toStdString().c_str(),
          sUnit.toStdString().c_str());
  fprintf(m_fp, "ZPOSITIVE Depth\n");
}

void CGocadExport::WriteVSetHeader(const CPointSet &ptSet) {
  fprintf(m_fp, "GOCAD VSet 1\n");
  fprintf(m_fp, "HEADER {\n");
  fprintf(m_fp, "name:%s\n", ptSet.Name().toStdString().c_str());
  fprintf(m_fp, "}\n");
  fprintf(m_fp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n");
  fprintf(m_fp, "NAME Default\n");
  fprintf(m_fp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n");
  WriteAxisUnit();
  fprintf(m_fp, "END_ORIGINAL_COORDINATE_SYSTEM\n");
}

void CGocadExport::WriteSolidHeader(const QString &title) {
  fprintf(m_fp, "GOCAD TSolid 1\n");
  fprintf(m_fp, "HEADER {\n");

  QString strTitle;

  if (m_bSplitFormations) {
    assert(m_stFormations.size() == 1);
    strTitle = (*m_stFormations.begin())->Name();
  } else {
    if (title.right(4) == ".gm4" || title.right(4) == ".gm5")
      strTitle = title.left(title.length() - 4);
    else if (title.right(3) == ".so")
      strTitle = title.left(title.length() - 3);
    else
      strTitle = title;
  }

  fprintf(m_fp, "name:%s\n", FixName(strTitle).toStdString().c_str());
  fprintf(m_fp, "}\n");
  fprintf(m_fp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n");
  fprintf(m_fp, "NAME Default\n");
  fprintf(m_fp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n");

  WriteAxisUnit();

  fprintf(m_fp, "END_ORIGINAL_COORDINATE_SYSTEM\n");
}

void CGocadExport::WriteFaultHeader(const C3DHorizon &hor) {
  fprintf(m_fp, "GOCAD TSurf\n");
  fprintf(m_fp, "HEADER {\n");
  fprintf(m_fp, "name:%s\n", FixName(hor.Name()).toStdString().c_str());
  fprintf(m_fp, "}\n");

  WriteAxisUnit();
}

void CGocadExport::WritePropertiesHeader(bool bSolid, const COpenGLNode &object) {
  bool bWriteFormationNames = m_bFormationNames && bSolid;
  if (m_stMaterialParameterComponents.size() || m_stResultComponents.size() || m_stFullVectors.size() ||
      m_stFullTensors.size() || bWriteFormationNames) {
    QString strSolidProperties = "TETRA_PROPERTIES";
    QString strSolidNoDataValues = "TETRA_NO_DATA_VALUES";
    QString strSolidESizes = "TETRA_ESIZES";
    bool bAtLeastOneSolidActive = false;

    QString strNonSolidProperties = "PROPERTIES";
    QString strNonSolidNoDataValues = "NO_DATA_VALUES";
    QString strNonSolidESizes = "ESIZES";
    bool bAtLeastOneNonSolidActive = false;

    QString &strProperties = (bSolid && m_bAverage) ? strSolidProperties : strNonSolidProperties;
    QString &strNoDataValues = (bSolid && m_bAverage) ? strSolidNoDataValues : strNonSolidNoDataValues;
    QString &strESizes = (bSolid && m_bAverage) ? strSolidESizes : strNonSolidESizes;
    bool &bAtLeastOneActive = (bSolid && m_bAverage) ? bAtLeastOneSolidActive : bAtLeastOneNonSolidActive;

    TResultComponentSet::iterator itr;
    for (itr = m_stResultComponents.begin(); itr != m_stResultComponents.end(); ++itr) {
      const IResult *pResult = dynamic_cast<const IResult *>((*itr).first);
      if (pResult) {
        const CDepletionStage *pStage = &m_model.InitialDepletionStage();
        while (pStage) {
          if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
            if (m_bLinear) {
              const IResultComponent *pResultComponent =
                  pResult->ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, (*itr).second);
              if (pResultComponent && pResultComponent->CanMap(object)) {
                strProperties += " " + FixName(pResultComponent->ExportLabel());
                strNoDataValues += " -99999";
                strESizes += " 1";
                bAtLeastOneActive = true;
              }
            }

            if (m_bNonLinear) {
              const IResultComponent *pResultComponent =
                  pResult->ResultComponent(*pStage, CAnalysisType::AT_NONLIN, 0, (*itr).second);
              if (pResultComponent && pResultComponent->CanMap(object)) {
                strProperties += " " + FixName(pResultComponent->ExportLabel());
                strNoDataValues += " -99999";
                strESizes += " 1";
                bAtLeastOneActive = true;
              }
            }

            if (m_bHeat) {
              const IResultComponent *pResultComponent =
                  pResult->ResultComponent(*pStage, CAnalysisType::AT_HEAT, 0, (*itr).second);
              if (pResultComponent && pResultComponent->CanMap(object)) {
                strProperties += " " + FixName(pResultComponent->ExportLabel());
                strNoDataValues += " -99999";
                strESizes += " 1";
                bAtLeastOneActive = true;
              }
            }

            if (m_bMixture) {
              const IResultComponent *pResultComponent =
                  pResult->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE, 0, (*itr).second);
              if (pResultComponent && pResultComponent->CanMap(object)) {
                strProperties += " " + FixName(pResultComponent->ExportLabel());
                strNoDataValues += " -99999";
                strESizes += " 1";
                bAtLeastOneActive = true;
              }
            }

            if (m_bMixtureContainment) {
              const IResultComponent *pResultComponent =
                  pResult->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, 0, (*itr).second);
              if (pResultComponent && pResultComponent->CanMap(object)) {
                strProperties += " " + FixName(pResultComponent->ExportLabel());
                strNoDataValues += " -99999";
                strESizes += " 1";
                bAtLeastOneActive = true;
              }
            }
          }

          if (pStage->Last())
            pStage = 0;
          else
            pStage = &pStage->Next();
        }
      } else if ((*itr).first->Component((*itr).second).CanMap(object)) {
        strProperties += " " + FixName((*itr).first->Component((*itr).second).ExportLabel());
        strNoDataValues += " -99999";
        strESizes += " 1";
        bAtLeastOneActive = true;
      }
    }

    for (itr = m_stMaterialParameterComponents.begin(); itr != m_stMaterialParameterComponents.end(); ++itr) {
      const IResult *pResult = dynamic_cast<const IResult *>((*itr).first);
      if (pResult) {
        const CDepletionStage *pStage = &m_model.InitialDepletionStage();
        while (pStage) {
          if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
            const IResultComponent *pResultComponent =
                pResult->ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, (*itr).second);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(pResultComponent->ExportLabel());
              strNoDataValues += " -99999";
              strESizes += " 1";
              bAtLeastOneActive = true;
            }
          }

          if (pStage->Last())
            pStage = 0;
          else
            pStage = &pStage->Next();
        }
      } else if ((*itr).first->Component((*itr).second).CanMap(object)) {
        strProperties += " " + FixName((*itr).first->Component((*itr).second).ExportLabel());
        strNoDataValues += " -99999";
        strESizes += " 1";
        bAtLeastOneActive = true;
      }
    }

    TVectorSet::iterator itv;
    for (itv = m_stFullVectors.begin(); itv != m_stFullVectors.end(); ++itv) {
      QString strLabel = (*itv)->VectorExportLabel();
      const CDepletionStage *pStage = &m_model.InitialDepletionStage();
      while (pStage) {
        if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
          QString strStage;
          strStage = QString("_D%1").arg(pStage->Index());
          if (m_bLinear) {
            const IResultComponent *pResultComponent = (*itv)->ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_L");
              strNoDataValues += " -99999";
              strESizes += " 3";
              bAtLeastOneActive = true;
            }
          }

          if (m_bNonLinear) {
            const IResultComponent *pResultComponent = (*itv)->ResultComponent(*pStage, CAnalysisType::AT_NONLIN, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_N");
              strNoDataValues += " -99999";
              strESizes += " 3";
              bAtLeastOneActive = true;
            }
          }

          if (m_bHeat) {
            const IResultComponent *pResultComponent = (*itv)->ResultComponent(*pStage, CAnalysisType::AT_HEAT, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_L");
              strNoDataValues += " -99999";
              strESizes += " 3";
              bAtLeastOneActive = true;
            }
          }

          if (m_bMixture) {
            const IResultComponent *pResultComponent =
                (*itv)->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_M");
              strNoDataValues += " -99999";
              strESizes += " 3";
              bAtLeastOneActive = true;
            }
          }

          if (m_bMixtureContainment) {
            const IResultComponent *pResultComponent =
                (*itv)->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_C");
              strNoDataValues += " -99999";
              strESizes += " 3";
              bAtLeastOneActive = true;
            }
          }
        }

        if (pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }
    }

    TTensorSet::iterator itt;
    for (itt = m_stFullTensors.begin(); itt != m_stFullTensors.end(); ++itt) {
      QString strPre;
      QString strMid;
      QString strPost;
      const ITensorGroup *pParent = static_cast<const ITensorGroup *>((*itt)->Parent());
      if (pParent->PreExportNameId())
        strPre = getStringTableEntry(pParent->PreExportNameId());
      if (pParent->ExportNameId())
        strMid = getStringTableEntry(pParent->ExportNameId());
      if (pParent->PostExportNameId())
        strPost = getStringTableEntry(pParent->PostExportNameId());
      QString strLabel = strPre + strMid + strPost;
      const CDepletionStage *pStage = &m_model.InitialDepletionStage();
      while (pStage) {
        if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
          QString strStage;
          strStage = QString("_D%1").arg(pStage->Index());
          if (m_bLinear) {
            const IResultComponent *pResultComponent = (*itt)->ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_L");
              strNoDataValues += " -99999";
              strESizes += " 6";
              bAtLeastOneActive = true;
            }
          }

          if (m_bNonLinear) {
            const IResultComponent *pResultComponent = (*itt)->ResultComponent(*pStage, CAnalysisType::AT_NONLIN, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_N");
              strNoDataValues += " -99999";
              strESizes += " 6";
              bAtLeastOneActive = true;
            }
          }

          if (m_bHeat) {
            const IResultComponent *pResultComponent = (*itt)->ResultComponent(*pStage, CAnalysisType::AT_HEAT, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_L");
              strNoDataValues += " -99999";
              strESizes += " 6";
              bAtLeastOneActive = true;
            }
          }

          if (m_bMixture) {
            const IResultComponent *pResultComponent =
                (*itt)->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_M");
              strNoDataValues += " -99999";
              strESizes += " 6";
              bAtLeastOneActive = true;
            }
          }

          if (m_bMixtureContainment) {
            const IResultComponent *pResultComponent =
                (*itt)->ResultComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, 0, 0);
            if (pResultComponent && pResultComponent->CanMap(object)) {
              strProperties += " " + FixName(strLabel + strStage + "_C");
              strNoDataValues += " -99999";
              strESizes += " 6";
              bAtLeastOneActive = true;
            }
          }
        }

        if (pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }
    }

    if (bWriteFormationNames) {
      strSolidProperties += " FormationName";
      strSolidNoDataValues += " -99999";
      strSolidESizes += " 1";
      bAtLeastOneSolidActive = true;
    }

    if (bAtLeastOneSolidActive) {
      fprintf(m_fp, "%s\n", strSolidProperties.toStdString().c_str());
      fprintf(m_fp, "%s\n", strSolidNoDataValues.toStdString().c_str());
      fprintf(m_fp, "%s\n", strSolidESizes.toStdString().c_str());
    }

    if (bAtLeastOneNonSolidActive) {
      fprintf(m_fp, "%s\n", strNonSolidProperties.toStdString().c_str());
      fprintf(m_fp, "%s\n", strNonSolidNoDataValues.toStdString().c_str());
      fprintf(m_fp, "%s\n", strNonSolidESizes.toStdString().c_str());
    }
  }
}

void CGocadExport::WritePointSets(IProgressBase &prog) {
  TPointSetSet::iterator it;
  for (it = m_stPointSets.begin(); it != m_stPointSets.end(); ++it) {
    WritePointSet(**it, prog);
  }
}

void CGocadExport::WritePointSet(const CPointSet &ptSet, IProgressBase &prog) {
  WriteVSetHeader(ptSet);
  WritePropertiesHeader(false, ptSet);
  const char *name = ptSet.Name().toStdString().c_str();
  prog.StatusMessage(QString("Writing pointset %1").arg(name));

  for (int i = 0; i < ptSet.PointSize(); ++i) {
    const geo::IPoint &pt = ptSet.PointAt(i);
    IValueDomainScalar::TValueVec values;
    GetPointValues(pt, values, ptSet);
    double x = (m_iUnit == 1) ? pt.X() * FF_FACTOR_LENGTH : pt.X();
    double y = (m_iUnit == 1) ? pt.Y() * FF_FACTOR_LENGTH : pt.Y();
    double z = (m_iUnit == 1) ? pt.Z() * FF_FACTOR_LENGTH : pt.Z();
    if (values.size() > 0)
      fprintf(m_fp, "PVRTX %d %f %f %f", i + 1, y, x, z); // gocad swaps x and y!
    else
      fprintf(m_fp, "VRTX %d %f %f %f", i + 1, y, x, z); // gocad swaps x and y!
    for (size_t j = 0; j < values.size(); ++j) {
      if (values[j].Valid())
        fprintf(m_fp, " %g", values[j].Value());
      else
        fprintf(m_fp, " -99999");
    }
    fprintf(m_fp, "\n");
    prog.Step();
  }
  WriteTrailer();
}

void CGocadExport::WriteFormations(IProgressBase &prog) {
  ClearElements();
  m_nCurrentId = 0;

  TFormationSet::iterator it;
  for (it = m_stFormations.begin(); it != m_stFormations.end(); ++it) {
    const C3DFormation &form = (const C3DFormation &)(**it);

    WriteFormation(form, prog);
  }
}

void CGocadExport::WriteFormation(const C3DFormation &form, IProgressBase &prog) {
  size_t nElementStartIndex = m_vcElements.size();

  prog.StatusMessage(QString("Writing formation %1").arg(form.Name()));

  // write formation header
  QString strFixedFormationName = FixName(form.Name());
  fprintf(m_fp, "TVOLUME %s\n", strFixedFormationName.toStdString().c_str());

  int nVol = form.VolumeSize();
  for (int i = 0; i < nVol; ++i) {
    const CFormationVolume &vol = form.Volume(i);
    WriteVolume(vol, prog);
  }

  // write elements for this volume
  for (size_t i = nElementStartIndex; i < m_vcElements.size(); ++i) {
    const geo::IElement &element = m_vcElements[i]->Element();
    IValueDomainScalar::TValueVec vcAverageElementValues;

    if (m_bAverage) {
      TElementValueVec vcElementValues;
      GetElementValues(element, vcElementValues, form);

      for (size_t j = 0; j < vcElementValues.size(); ++j) {
        const IValueDomainScalar::TValueVec &vcValues = vcElementValues[j];
        assert(vcValues.size() == element.NrOfNodes());

        double dTotal = 0;
        bool bValid = true;
        for (size_t k = 0; k < vcValues.size() && bValid; ++k) {
          if (vcValues[k].Valid())
            dTotal += vcValues[k].Value();
          else
            bValid = false;
        }

        if (bValid)
          vcAverageElementValues.push_back(geo::CValue(dTotal / vcValues.size()));
        else
          vcAverageElementValues.push_back(geo::CValue());
      }
    }

    m_vcElements[i]->Write(m_fp, vcAverageElementValues, m_bFormationNames ? &strFixedFormationName : 0);
  }

  // contents of m_vcElements is kept until complete mesh is written for the VRTX/ATOM bookkeeping!
}

void CGocadExport::WriteFault(const C3DHorizon &hor, IProgressBase &prog) {
  const char *name = hor.Name().toStdString().c_str();
  prog.StatusMessage(QString("Writing fault %1").arg(name));

  int nNode = 0;
  typedef std::map<const geo::INode *, CVertex *> TNodeVertexMap;
  TNodeVertexMap mpNodeVertex;

  const geo::CElementGroup *pInterfaces = hor.InterfaceElementGroup();
  assert(pInterfaces != 0);

  std::vector<CTrgl *> vcTriangles(pInterfaces->ElementSize());

  fprintf(m_fp, "TFACE\n");

  for (int i = 0; i < pInterfaces->ElementSize(); ++i) {
    const geo::IElement &elm = pInterfaces->Element(i);

    TElementValueVec vcElementValues;
    GetElementValues(elm, vcElementValues, hor);

    int j;
    assert(elm.NrOfNodes() == 6); // interface: 3 + 3 nodes
    CNodeBase *nodes[3];
    for (j = 0; j < 3; ++j) {
      const geo::INode &node = elm.Node(j);
      std::pair<TNodeVertexMap::iterator, bool> prInsert =
          mpNodeVertex.insert(TNodeVertexMap::value_type(&node, (CVertex *)0));
      if (prInsert.second) {
        CVertex *pVertex = new CVertex(node);
        prInsert.first->second = pVertex;
        nodes[j] = pVertex;
        nodes[j]->Id(++nNode);
        nodes[j]->Write(m_fp, m_iUnit, vcElementValues, j);
      } else {
        nodes[j] = new CVertex(*prInsert.first->second);
      }
    }

    vcTriangles[i] = new CTrgl(nodes);
    prog.Step();
  }

  for (size_t i = 0; i < vcTriangles.size(); ++i) {
    vcTriangles[i]->Write(m_fp);
    delete vcTriangles[i];
    prog.Step();
  }
}

void CGocadExport::AddPointValueFromComponent(const geo::IPoint &point, const IValueComponentBase &comp,
                                              IValueDomainScalar::TValueVec &vcPointValues,
                                              const COpenGLNode &object) const {
  if (comp.CanMap(object)) {
    vcPointValues.push_back(
        comp.ScalarData().ValuePoint(point, m_iUnit == 0 ? IQuantityDouble::SI_UNIT : IQuantityDouble::FIELD_UNIT));
  }
}

void CGocadExport::AddPointValuesFromResult(const geo::IPoint &point, const IResult &result,
                                            const std::vector<int> &indices,
                                            IValueDomainScalar::TValueVec &vcPointValues,
                                            const COpenGLNode &object) const {
  const CDepletionStage *pStage = &m_model.InitialDepletionStage();
  while (pStage) {
    if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
      if (m_bLinear) {
        for (size_t i = 0; i < indices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, indices[i]);
          if (pResultComponent)
            AddPointValueFromComponent(point, *pResultComponent, vcPointValues, object);
        }
      }

      if (m_bNonLinear) {
        for (size_t i = 0; i < indices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_NONLIN, 0, indices[i]);
          if (pResultComponent)
            AddPointValueFromComponent(point, *pResultComponent, vcPointValues, object);
        }
      }

      if (m_bHeat) {
        for (size_t i = 0; i < indices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_HEAT, 0, indices[i]);
          if (pResultComponent)
            AddPointValueFromComponent(point, *pResultComponent, vcPointValues, object);
        }
      }

      if (m_bMixture) {
        for (size_t i = 0; i < indices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_MIXTURE, 0, indices[i]);
          if (pResultComponent)
            AddPointValueFromComponent(point, *pResultComponent, vcPointValues, object);
        }
      }

      if (m_bMixtureContainment) {
        for (size_t i = 0; i < indices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, 0, indices[i]);
          if (pResultComponent)
            AddPointValueFromComponent(point, *pResultComponent, vcPointValues, object);
        }
      }
    }
    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CGocadExport::AddElementValuesFromComponent(const geo::IElement &element, const IValueComponentBase &comp,
                                                 TElementValueVec &vcElementValues, const COpenGLNode &object) const {
  if (comp.CanMap(object)) {
    IValueDomainScalar::TValueVec vcValues =
        comp.ScalarData().ValueElement(element, m_iUnit == 0 ? IQuantityDouble::SI_UNIT : IQuantityDouble::FIELD_UNIT);
    vcElementValues.push_back(vcValues);
  }
}

void CGocadExport::AddElementValuesFromResult(const geo::IElement &element, const IResult &result,
                                              const std::vector<int> &vcIndices, TElementValueVec &vcElementValues,
                                              const COpenGLNode &object) const {
  const CDepletionStage *pStage = &m_model.InitialDepletionStage();
  while (pStage) {
    if (m_stTimeSteps.find(pStage) != m_stTimeSteps.end()) {
      if (m_bLinear) {
        for (size_t i = 0; i < vcIndices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_LINEAR, 0, vcIndices[i]);
          if (pResultComponent)
            AddElementValuesFromComponent(element, *pResultComponent, vcElementValues, object);
        }
      }

      if (m_bNonLinear) {
        for (size_t i = 0; i < vcIndices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_NONLIN, 0, vcIndices[i]);
          if (pResultComponent)
            AddElementValuesFromComponent(element, *pResultComponent, vcElementValues, object);
        }
      }

      if (m_bHeat) {
        for (size_t i = 0; i < vcIndices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_HEAT, 0, vcIndices[i]);
          if (pResultComponent)
            AddElementValuesFromComponent(element, *pResultComponent, vcElementValues, object);
        }
      }

      if (m_bMixture) {
        for (size_t i = 0; i < vcIndices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_MIXTURE, 0, vcIndices[i]);
          if (pResultComponent)
            AddElementValuesFromComponent(element, *pResultComponent, vcElementValues, object);
        }
      }

      if (m_bMixtureContainment) {
        for (size_t i = 0; i < vcIndices.size(); ++i) {
          const IResultComponent *pResultComponent =
              result.ResultComponent(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, 0, vcIndices[i]);
          if (pResultComponent)
            AddElementValuesFromComponent(element, *pResultComponent, vcElementValues, object);
        }
      }
    }

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CGocadExport::GetPointValues(const geo::IPoint &point, IValueDomainScalar::TValueVec &vcPointValues,
                                  const COpenGLNode &object) const {
  TResultComponentSet::const_iterator it;
  for (it = m_stResultComponents.begin(); it != m_stResultComponents.end(); ++it) {
    std::vector<int> indices(1);
    const IResult *pResult = dynamic_cast<const IResult *>((*it).first);
    if (pResult) {
      indices[0] = (*it).second;
      AddPointValuesFromResult(point, *pResult, indices, vcPointValues, object);
    } else {
      const IValueComponentBase &comp = (*it).first->Component((*it).second);
      AddPointValueFromComponent(point, comp, vcPointValues, object);
    }
  }

  TVectorSet::const_iterator itv;
  for (itv = m_stFullVectors.begin(); itv != m_stFullVectors.end(); ++itv) {
    std::vector<int> indices(3);
    indices[0] = 1; // swap X and
    indices[1] = 0; //  Y values
    indices[2] = 2;
    AddPointValuesFromResult(point, **itv, indices, vcPointValues, object);
  }

  TTensorSet::const_iterator itt;
  for (itt = m_stFullTensors.begin(); itt != m_stFullTensors.end(); ++itt) {
    std::vector<int> indices(6);
    indices[0] = 1; // swap XX and
    indices[1] = 0; //  YY values
    indices[2] = 2;
    indices[3] = 3;
    indices[4] = 5; // swap YZ and
    indices[5] = 4; //  XZ values
    AddPointValuesFromResult(point, **itt, indices, vcPointValues, object);
  }
}

void CGocadExport::GetElementValues(const geo::IElement &element, TElementValueVec &vcElementValues,
                                    const COpenGLNode &object) const {
  TResultComponentSet::const_iterator itr;
  for (itr = m_stResultComponents.begin(); itr != m_stResultComponents.end(); ++itr) {
    std::vector<int> vcIndices(1);
    const IResult *pResult = dynamic_cast<const IResult *>((*itr).first);
    if (pResult) {
      vcIndices[0] = (*itr).second;
      AddElementValuesFromResult(element, *pResult, vcIndices, vcElementValues, object);
    } else {
      const IValueComponentBase &comp = (*itr).first->Component((*itr).second);
      AddElementValuesFromComponent(element, comp, vcElementValues, object);
    }
  }

  for (itr = m_stMaterialParameterComponents.begin(); itr != m_stMaterialParameterComponents.end(); ++itr) {
    std::vector<int> vcIndices(1);
    const IResult *pResult = dynamic_cast<const IResult *>((*itr).first);
    if (pResult) {
      vcIndices[0] = (*itr).second;
      AddElementValuesFromResult(element, *pResult, vcIndices, vcElementValues, object);
    } else {
      const IValueComponentBase &comp = (*itr).first->Component((*itr).second);
      AddElementValuesFromComponent(element, comp, vcElementValues, object);
    }
  }

  TVectorSet::const_iterator itv;
  for (itv = m_stFullVectors.begin(); itv != m_stFullVectors.end(); ++itv) {
    std::vector<int> vcIndices(3);
    vcIndices[0] = 1; // swap X and
    vcIndices[1] = 0; //  Y values
    vcIndices[2] = 2;
    AddElementValuesFromResult(element, **itv, vcIndices, vcElementValues, object);
  }

  TTensorSet::const_iterator itt;
  for (itt = m_stFullTensors.begin(); itt != m_stFullTensors.end(); ++itt) {
    std::vector<int> vcIndices(6);
    vcIndices[0] = 1; // swap XX and
    vcIndices[1] = 0; //  YY values
    vcIndices[2] = 2;
    vcIndices[3] = 3;
    vcIndices[4] = 5; // swap YZ and
    vcIndices[5] = 4; //  XZ values
    AddElementValuesFromResult(element, **itt, vcIndices, vcElementValues, object);
  }
}

void CGocadExport::CreateNodes(const geo::IElement &element, const int *pNodeIndices, CNodeBase **pNode,
                               const TElementValueVec &vcElementValues) {
  int n;
  for (n = 0; n < 4; ++n) {
    const geo::INode &node = element.Node(pNodeIndices[n]);

    TNodeMap::iterator it = m_mpNodes.find(&node);
    if (it == m_mpNodes.end()) // create new one
    {
      CNodeBase *newNode = nullptr;

      TUniqueNodeSet::iterator uniq = m_stUniqueNodes.find(&node);

      if (uniq ==
          m_stUniqueNodes
              .end()) // if other node with same coordinates doesn't already exist we create new vertex; otherwise atom
      {
        newNode = new CVertex(node);
        m_stUniqueNodes.insert(&node);
      } else {
        TNodeMap::iterator vertex = m_mpNodes.find(*uniq);
        newNode = new CAtom(static_cast<const CVertex &>(*vertex->second));
      }

      TNodeMap::iterator itn = m_mpNodes.insert(TNodeMap::value_type(&node, newNode)).first;
      pNode[n] = itn->second;

      // write the vertex/atom
      pNode[n]->Id(++m_nCurrentId);
      pNode[n]->Write(m_fp, m_iUnit, vcElementValues, n);
    } else {
      pNode[n] = it->second;
    }
  }
}

void CGocadExport::WriteVolume(const CFormationVolume &vol, IProgressBase &prog) {
  const geo::CBodyGroup &bodies = vol.Volume();
  int nBodies = bodies.BodySize();
  int i;
  for (i = 0; i < nBodies; ++i) {
    const geo::IBody &body = bodies.Body(i);
    const geo::ITetrahedron *pTet = dynamic_cast<const geo::ITetrahedron *>(&body);
    const geo::IHexahedron *pHexa = dynamic_cast<const geo::IHexahedron *>(&body);

    assert(pTet || pHexa);

    CNodeBase *pNode[4];

    if (pHexa) {
      assert(pHexa->NrOfNodes() == 8);

      TElementValueVec vcHexaValues;

      if (!m_bAverage)
        GetElementValues(*pHexa, vcHexaValues, vol.Formation());

      // generate the tetras
      int t;
      for (t = 0; t < 6; ++t) {
        TElementValueVec vcElementValues;
        if (!m_bAverage) {
          vcElementValues.resize(vcHexaValues.size());
          for (size_t r = 0; r < vcHexaValues.size(); ++r) {
            // map the hexa values on the tetra
            int n;
            for (n = 0; n < 4; ++n)
              vcElementValues[r].push_back(vcHexaValues[r][hexa_tetra_index[t][n]]);
          }
        }

        // create the nodes
        CreateNodes(*pHexa, hexa_tetra_index[t], pNode, vcElementValues);

        // create the tetra
        m_vcElements.push_back(new CTetra(*pHexa, pNode));
      }
    } else if (pTet) {
      assert(pTet->NrOfNodes() == 4);

      TElementValueVec vcElementValues;
      if (!m_bAverage) {
        // get the values for this element
        GetElementValues(*pTet, vcElementValues, vol.Formation());
      }

      CreateNodes(*pTet, tetra_tetra_index, pNode, vcElementValues);

      // create the element
      m_vcElements.push_back(new CTetra(*pTet, pNode));
    }

    prog.Step();
  }
}

void CGocadExport::WriteSolidSurfaces() {
  // skip for horizon models for now...
  if (!dynamic_cast<const geo::CTetMeshBase *>(&Model().Mesh().Mesh()))
    return;

  // collect all side surfaces from the selected formations
  typedef std::set<const geo::CBodyGroup::CSideSurface *> TSideSurfaceSet;
  TSideSurfaceSet stSideSurfaces;

  typedef std::map<const geo::CBodyGroup::CSideSurface *, const CFormationBase *> TSurface2FormationMap;
  TSurface2FormationMap mpSurface2Formation;

  for (TFormationSet::iterator it = m_stFormations.begin(); it != m_stFormations.end(); ++it) {
    const CFormationBase &f = **it;

    for (int i = 0; i < f.ElementSetSize(); ++i) {
      const CFormationVolume &fv = static_cast<const CFormationVolume &>(f.ElementSet(i));

      for (int j = 0; j < fv.Volume().SideSurfaceSize(); ++j) {
        stSideSurfaces.insert(&fv.Volume().SideSurface(j));
        mpSurface2Formation.insert(TSurface2FormationMap::value_type(&fv.Volume().SideSurface(j), &f));
      }
    }
  }

  CGraphEntry &horizon_entry = *Model().GraphEntry(MD_BASE_HORIZON);
  CGraphEntry::TEntryNodeSet stHorizons = horizon_entry.GraphEntryNodes();

  typedef std::vector<const geo::CBodyGroup::CSideSurface *> TSideSurfaceVec;
  typedef std::map<const C3DHorizon *, TSideSurfaceVec> THorizonMap;
  THorizonMap mpHorizons;

  typedef std::set<const CTetraHorizonBase *> TFaultSet;
  TFaultSet stFaults;

  const geo::CTetMeshBase *pTetMesh = dynamic_cast<const geo::CTetMeshBase *>(&Model().Mesh().Mesh());
  if (pTetMesh) {
    // tetra model
    for (CGraphEntry::TEntryNodeSet::iterator it = stHorizons.begin(); it != stHorizons.end(); ++it) {
      const CTetraHorizonBase &hor = static_cast<const CTetraHorizonBase &>(**it);

      for (int i = 0; i < hor.OutputSurfaceSize(); ++i) {
        const geo::CSurfaceDesc &surfdesc = hor.OutputSurface(i);

        for (int j = 0; j < surfdesc.TetSurfaceSize(); ++j) {
          const geo::CTetSurface &tetsurf = surfdesc.TetSurface(j);

          // only each first surface of a pair
          if (j % 2 == 0 && stSideSurfaces.find(&tetsurf) != stSideSurfaces.end()) {
            if (hor.Slip()) {
              // we want to write faults as a whole; otherwise we might miss parts (TFS 307252)
              // this may give undesirable results when exporting only some formations, as the faults may extend beyond
              // but at least it's complete
              stFaults.insert(&hor);
            } else {
              THorizonMap::iterator itHor = mpHorizons.insert(THorizonMap::value_type(&hor, TSideSurfaceVec())).first;
              itHor->second.push_back(&tetsurf);
            }
          }

          stSideSurfaces.erase(&tetsurf);
        }
      }
    }
  } else {
    // hexa model, enumerate horizons
  }

  fprintf(m_fp, "MODEL\n");

  std::vector<QString> vcModelRegions;

  // write the horizons
  int iTFace = 0;
  for (THorizonMap::iterator ith = mpHorizons.begin(); ith != mpHorizons.end(); ++ith) {
    const C3DHorizon &hor = *ith->first;
    fprintf(m_fp, "SURFACE %s\n", FixName(hor.Name()).toStdString().c_str());

    const TSideSurfaceVec &vcSurfaces = ith->second;
    for (size_t i = 0; i < vcSurfaces.size(); ++i) {
      WriteTFace(*vcSurfaces[i], ++iTFace);
      TSurface2FormationMap::iterator it = mpSurface2Formation.find(vcSurfaces[i]);
      assert(it != mpSurface2Formation.end());
      vcModelRegions.push_back(FixName(it->second->Name()));
    }
  }

  for (TFaultSet::iterator fault = stFaults.begin(); fault != stFaults.end(); ++fault) {
    const CTetraHorizonBase &hor = **fault;
    fprintf(m_fp, "SURFACE %s\n", FixName(hor.Name()).toStdString().c_str());

    WriteTFaceFault(hor, ++iTFace);
  }

  // write remaining surfaces (sides)
  int iSide = 0;
  for (TSideSurfaceSet::iterator its = stSideSurfaces.begin(); its != stSideSurfaces.end(); ++its) {
    fprintf(m_fp, "SURFACE side_%d\n", ++iSide);
    WriteTFace(**its, ++iTFace);
  }

  // write model regions
  for (int i = 0; i < vcModelRegions.size(); ++i)
    fprintf(m_fp, "MODEL_REGION %s %d\n", vcModelRegions[i].toStdString().c_str(), -(i + 1));
}

const CGocadExport::CNodeBase &CGocadExport::Vertex(const geo::INode &node) const {
  TNodeMap::const_iterator it = m_mpNodes.find(&node);
  assert(it != m_mpNodes.end());
  return *it->second;
}

void CGocadExport::WriteTFace(const geo::CBodyGroup::CSideSurface &surface, int idx) {
  fprintf(m_fp, "TFACE %d\n", idx);
  int j;
  for (j = 0; j < surface.FaceSize(); ++j) {
    const geo::IFace &face = surface.Face(j);
    assert(face.NrOfNodes() == 3 || face.NrOfNodes() == 4);
    if (j == 0)
      fprintf(m_fp, "KEYVERTICES %d %d %d\n", Vertex(face.Node(0)).Id(), Vertex(face.Node(1)).Id(),
              Vertex(face.Node(2)).Id());
    fprintf(m_fp, "TRGL %d %d %d\n", Vertex(face.Node(0)).Id(), Vertex(face.Node(1)).Id(), Vertex(face.Node(2)).Id());
    /*
      // quads are separated into two triangles
      if(face.NrOfNodes() == 4)
          fprintf(m_fp, "TRGL %d %d %d\n", face.Node(0).Index(), face.Node(2).Index(), face.Node(3).Index());
    */
  }
}

void CGocadExport::WriteTFaceFault(const CTetraHorizonBase &fault, int idx) {
  const geo::CElementGroup *eltGroup = fault.InterfaceElementGroup();

  assert(eltGroup);

  fprintf(m_fp, "TFACE %d\n", idx);

  for (int i = 0; i < eltGroup->ElementSize(); ++i) {
    const geo::IInterfaceElement &ifElt = static_cast<const geo::IInterfaceElement &>(eltGroup->Element(i));
    const geo::IElement &front = ifElt.Front();

    if (i == 0)
      fprintf(m_fp, "KEYVERTICES %d %d %d\n", Vertex(front.Node(0)).Id(), Vertex(front.Node(1)).Id(),
              Vertex(front.Node(2)).Id());
    fprintf(m_fp, "TRGL %d %d %d\n", Vertex(front.Node(0)).Id(), Vertex(front.Node(1)).Id(),
            Vertex(front.Node(2)).Id());
  }
}

void CGocadExport::WriteTrailer() { fprintf(m_fp, "END\n"); }

bool CGocadExport::HasProperties() const { return false; }

template <class COMPOSITE>
void CGocadExport::FetchComposites(int nComponents, std::set<const COMPOSITE *> &stComposites) {
  TResultComponentSet::iterator it = m_stResultComponents.begin();
  while (it != m_stResultComponents.end()) {
    const IValueComposite *pComposite = (*it).first;

    const IResult *pResult = dynamic_cast<const IResult *>(pComposite);
    if (pResult) {
      if ((!m_bLinear && !m_bNonLinear && !m_bHeat && !m_bMixture && !m_bMixtureContainment) || m_stTimeSteps.empty()) {
        TResultComponentSet::iterator temporary = it++;

        // can only select result sets when both
        // - linear or nonlinear or heat or mixture has been selected
        // - at least one depletion stage has been selected
        m_stResultComponents.erase(temporary);
        continue;
      }
    }

    // is it a COMPOSITE?
    const COMPOSITE *pFullComposite = dynamic_cast<const COMPOSITE *>(pComposite);
    if (pFullComposite) {
      // are all components present?
      bool bFound = true;
      int i;
      for (i = 0; i < nComponents && bFound; ++i)
        bFound = (m_stResultComponents.find(TResultComponent(pComposite, i)) != m_stResultComponents.end());

      if (bFound) {
        // register the full tensor
        stComposites.insert(pFullComposite);

        // remove the components from the set
        for (i = 0; i < nComponents; ++i)
          m_stResultComponents.erase(TResultComponent(pComposite, i));

        // reset the iterator
        it = m_stResultComponents.begin();
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
}

void CGocadExport::SortComposites() {
  m_stFullTensors.clear();
  m_stFullVectors.clear();

  if (m_bTensorESize)
    FetchComposites<ITensorGroup::CComponentComposite>(6, m_stFullTensors);

  if (m_bVectorESize)
    FetchComposites<IVectorResult>(3, m_stFullVectors);
}
