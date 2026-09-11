
#include "WellCasingCement.h"

#include "WellCasingModel.h"
#include "WellCasingCementInitialMaterial.h"
#include "BodyGroup.h"
#include "ModifiedHint.h"

namespace
{

const QRgb DARK_GRAY = qRgb(169, 169, 169);

} // anonymous namespace

CWellCasingCement::CWellCasingCement(const QString& strName, CWellCasingModel& model)
: TBase(strName, model),
  m_bShowMidpoints(false)
{
  Color(DARK_GRAY);
  LinkTo(model.Mesh());
}

CWellCasingCement::CWellCasingCement(CWellCasingModel& model)
: TBase(model),
  m_bShowMidpoints(false)
{
  Color(DARK_GRAY);
  LinkTo(model.Mesh());
}

CWellCasingCement::~CWellCasingCement()
{
  InvalidateCache();
}

unsigned int CWellCasingCement::IconId() const
{
  return IDI_WELLCASINGMODEL;
}

unsigned int CWellCasingCement::TypeId() const
{
  return 0;
}

QString CWellCasingCement::TypeName() const
{
  // TODO MAGIC
  return "Well";
}

bool CWellCasingCement::Less(const CGraphNode& /*node*/) const
{
  return false;
}

void CWellCasingCement::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(&node == &(static_cast<const CModelBase&>(Model())).Mesh() && (uHint == MeshCleared || uHint == MeshCreated))
  {
    m_centerpoints.Clear();
    Modified();
  }
  else
  {
    // if we changed the cement, then our cache is no longer valid
    // because the cache is only relevant during Diana run, we don't bother to find out what changed
    // that would take longer than filling the cache with this one material
    InvalidateCache();
  }

  TBase::OnNeighbourModified(node, uHint);
}

bool CWellCasingCement::Empty() const
{
  return false;
}

void CWellCasingCement::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if(version < CStreamVersion(3, 7, 20))
  {
	  // Link to depletionstage entry
	  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
    COpenGLNode::LoadStream(stream, version, progress);

    if(version >= CStreamVersion(3, 7, 9))
    {
      CModelBase& model = static_cast<CModelBase&>(Model());
      CDepletionStageEntry::iterator it;
      for(it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
        LoadMaterial(*it, stream, version, progress);
    }

    CreatePressuresAndTemperatures();
  }
  else
  {
    CFormationBase::LoadStream(stream, version, progress);
  }
}

int CWellCasingCement::DisplayListSize() const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  if(model.Mesh().IsMesh())
    return 1;

  return 0;
}

const geo::IObject& CWellCasingCement::DisplayList(int /*nIndex*/) const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  if(model.Mesh().IsMesh())
  {
    if(m_bShowMidpoints)
    {
      if(m_centerpoints.Empty())
        m_centerpoints.Create(model.Mesh().CementElements());
      return m_centerpoints.Get();
    }
    else
    {
      return model.Mesh().CementElements();
    }
  }

  assert(FALSE);
  geo::IObject* pBogus = 0;
  return *pBogus;
}

const CWellCasingCementInitialMaterial& CWellCasingCement::GetCementMaterial(const CFFMaterial& base) const
{
  CWellCasingCementInitialMaterial* pMat = new CWellCasingCementInitialMaterial(base);
  std::pair<TMaterialSet::iterator, bool> prInserted = m_stMaterials.insert(pMat);
  if(!prInserted.second)
    delete pMat;

  return **prInserted.first;
}

void CWellCasingCement::OnShowElements()
{
  if(m_bShowMidpoints)
  {
    m_bShowMidpoints = false;
    Modified();
  }
}

void CWellCasingCement::OnShowMidpoints()
{
  if(!m_bShowMidpoints)
  {
    m_bShowMidpoints = true;
    Modified();
  }
}


///// CWellCasingCement::CMaterialLess
bool CWellCasingCement::CMaterialLess::operator()(const CWellCasingCementInitialMaterial* m1, const CWellCasingCementInitialMaterial* m2) const
{
  return (*m1) < (*m2);
}


void CWellCasingCement::InvalidateCache()
{
  for (TMaterialSet::iterator it = m_stMaterials.begin(); it != m_stMaterials.end(); ++it)
    delete *it;
  m_stMaterials.clear();
}
