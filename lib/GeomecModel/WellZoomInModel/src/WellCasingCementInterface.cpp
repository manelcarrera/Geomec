
#include "WellCasingCementInterface.h"

#include "WellCasingModel.h"
#include "WellCasingSteel.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "ModifiedHint.h"

namespace
{

const QRgb LIGHT_BROWN = qRgb(181, 101, 29);

}

CWellCasingCementInterface::CWellCasingCementInterface(const QString& strName, CWellCasingModel& model)
: TBase(strName, model),
  m_bShowMidpoints(false),
  m_color(LIGHT_BROWN),
  m_sliptype(USER)
{
  LinkTo(model.Mesh());
}

CWellCasingCementInterface::CWellCasingCementInterface(CWellCasingModel& model)
: TBase(model),
  m_bShowMidpoints(false),
  m_color(LIGHT_BROWN),
  m_sliptype(USER)
{
  LinkTo(model.Mesh());
}

bool CWellCasingCementInterface::operator==(const CWellCasingCementInterface& rhs) const
{
  if(m_color != rhs.m_color)
  return false;

  return TBase::operator==(rhs);
}

CWellCasingCementInterface& CWellCasingCementInterface::operator=(const CWellCasingCementInterface& rhs)
{
  TBase::operator=(rhs);
  m_color = rhs.m_color;
  Modified();

  return *this;
}

unsigned int CWellCasingCementInterface::IconId() const
{
  return IDI_WELLCASINGMODEL;
}

unsigned int CWellCasingCementInterface::TypeId() const
{
  return 0;
}

QString CWellCasingCementInterface::TypeName() const
{
  // TODO MAGIC
  return "Well";
}

bool CWellCasingCementInterface::Less(const CGraphNode& node) const
{
  if(dynamic_cast<const CWellCasingSteel*>(&node))
  return false;

  return true;
}

void CWellCasingCementInterface::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(&node == &(static_cast<const CModelBase&>(Model())).Mesh() && (uHint == MeshCleared || uHint == MeshCreated))
  {
  m_centerpoints.Clear();
  Modified();
  }

  TBase::OnNeighbourModified(node, uHint);
}

bool CWellCasingCementInterface::Empty() const
{
  return false;
}

long CWellCasingCementInterface::SavedItems() const
{
  return 0;
}

int CWellCasingCementInterface::DisplayListSize() const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  if(model.Mesh().IsMesh())
  return 1;

  return 0;
}

const geo::IObject& CWellCasingCementInterface::DisplayList(int /*nIndex*/) const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  if(model.Mesh().IsMesh())
  {
  if(m_bShowMidpoints)
  {
      if(m_centerpoints.Empty())
    m_centerpoints.Create(model.Mesh().CementInterfaceElements());
      return m_centerpoints.Get();
  }
  else
  {
      return model.Mesh().CementInterfaceElements();
  }
  }

  assert(false);
  geo::IObject* pBogus = 0;
  return *pBogus;
}

CWellCasingCementInterface::TColor CWellCasingCementInterface::Color() const
{
  return m_color;
}

void CWellCasingCementInterface::Color(TColor nColor)
{
  m_color = nColor;
}

void CWellCasingCementInterface::OnShowElements()
{
  if(m_bShowMidpoints)
  {
  m_bShowMidpoints = false;
  Modified();
  }
}

void CWellCasingCementInterface::OnShowMidpoints()
{
  if(!m_bShowMidpoints)
  {
  m_bShowMidpoints = true;
  Modified();
  }
}

void CWellCasingCementInterface::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  int sliptype = int(m_sliptype);
  stream << sliptype;
  TBase::SaveStream(stream, progress);
}

void CWellCasingCementInterface::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  if(version >= CStreamVersion(4, 1, 29))
  {
  int sliptype;
  stream >> sliptype;
  m_sliptype = TSlipType(sliptype);
  }

  TBase::LoadStream(stream, version, progress);
}

CWellCasingCementInterface::TSlipType CWellCasingCementInterface::SlipType() const
{
  return m_sliptype;
}

void CWellCasingCementInterface::SlipType(TSlipType sliptype)
{
  if(m_sliptype != sliptype)
  {
  m_sliptype = sliptype;
  Modified();
  }
}


/////

CWellCasingCementInterfaceMaterialServer::CWellCasingCementInterfaceMaterialServer(TWellCasingCementInterfaceMaterialServerParent& parent, const CDepletionStage& stage)
: TBase(parent, stage)
{
  reParent(&parent);
  LinkTo(const_cast<CDepletionStage&>(stage));
}

CInterfaceMaterial* CWellCasingCementInterfaceMaterialServer::FindIndexInEntry(int nIndex) const
{
  CInterfaceMaterialEntry& material_entry = (CInterfaceMaterialEntry&)*((CModelBase&)Model()).GraphEntry(MD_BASE_INTERFACEMATERIAL);
  assert(material_entry.FindIndex(nIndex));
  return material_entry.FindIndex(nIndex);
}
