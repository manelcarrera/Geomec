#include "WellPathIntermediate.h"


///// CCachingIntermediate
CCachingIntermediate::CCachingIntermediate()
: m_bHasColor(false),
  m_bHasDrawInfo(false),
  m_pIntermediate(0)
{
}

CCachingIntermediate::~CCachingIntermediate()
{
}

void CCachingIntermediate::Detach()
{
  if(m_pIntermediate)
  m_pIntermediate->Detach();
}

void CCachingIntermediate::AddSection(well::IWellSection& section,
                                                 SECTION_TYPE type,
                                                 QRgb color,
                                                 int iLineWidth,
                                                 bool bVisible,
                                                 const QString& strDescription)
{
  if(m_pIntermediate)
  m_pIntermediate->AddSection(section, type, color, iLineWidth, bVisible, strDescription);
  SectionInfo si;
  si.pSection = &section;
  si.type = type;
  si.color = color;
  si.linewidth = iLineWidth;
  si.visible = bVisible;
  si.description = strDescription;
  m_lstSectionInfo.push_back(si);
  VERIFY(connect(&section, SIGNAL(OnDestroy(const well::IWellSection&)), this, SLOT(OnSectionDestroyed(const well::IWellSection&))));
}

void CCachingIntermediate::AddPoint(well::CWellPoint& point,
                                               QRgb color,
                                               int iPointSize,
                                               bool bVisible)
{
  if(m_pIntermediate)
  m_pIntermediate->AddPoint(point, color, iPointSize, bVisible);
  PointInfo pi;
  pi.pPoint = &point;
  pi.color = color;
  pi.pointsize = iPointSize;
  pi.visible = bVisible;
  m_lstPointInfo.push_back(pi);
  VERIFY(connect(&point, SIGNAL(OnDestroy(const well::CWellPoint&)), this, SLOT(OnPointDestroyed(const well::CWellPoint&))));
}

void CCachingIntermediate::Color(QRgb color)
{
  if(m_pIntermediate)
  m_pIntermediate->Color(color);
  m_color = color;
  m_bHasColor = true;
}

void CCachingIntermediate::RemoveAllPoints()
{
  if(m_pIntermediate)
  m_pIntermediate->RemoveAllPoints();
  m_lstPointInfo.clear();
}

void CCachingIntermediate::SetSectionDescription(well::IWellSection& section, const QString& strDescription)
{
  if(m_pIntermediate)
  m_pIntermediate->SetSectionDescription(section, strDescription);
  TSectionInfoList::iterator it;
  for(it = m_lstSectionInfo.begin(); it != m_lstSectionInfo.end(); ++it)
  {
  if(it->pSection == &section)
      it->description = strDescription;
  }
}

bool CCachingIntermediate::IsVisible() const
{
  if(m_pIntermediate)
  return m_pIntermediate->IsVisible();
  return false;
}

void CCachingIntermediate::OnNeighbourDeleted(const CGraphNode& node)
{
  if(m_pIntermediate)
  m_pIntermediate->OnNeighbourDeleted(node);
}

bool CCachingIntermediate::OnNewNeighbour(const CGraphNode& node)
{
  if(m_pIntermediate)
  return m_pIntermediate->OnNewNeighbour(node);
  return false;
}
 // returns whether update must be triggered
void CCachingIntermediate::DrawInfo(bool bDraw)
{
  if(m_pIntermediate)
  m_pIntermediate->DrawInfo(bDraw);
  m_bDrawInfo = bDraw;
  m_bHasDrawInfo = true;
}

bool CCachingIntermediate::DrawInfo() const
{
  if(m_pIntermediate)
  return m_pIntermediate->DrawInfo();
  return m_bDrawInfo;
}

/*
void CCachingIntermediate::Clear()
{
  // clear cache
  m_lstSectionInfo.clear();
  m_lstPointInfo.clear();
  m_bHasColor = false;
  m_bHasDrawInfo = false;
}
*/
void CCachingIntermediate::SetIntermediate(CWellPath::IIntermediate* pIntermediate)
{
  if(pIntermediate != m_pIntermediate)
  {
  if(pIntermediate)
      UpdateIntermediate(*pIntermediate);
  m_pIntermediate = pIntermediate;
  }
}

void CCachingIntermediate::UpdateIntermediate(IIntermediate& intermediate)
{
  assert(&intermediate != this);

  for(TSectionInfoList::iterator it = m_lstSectionInfo.begin(); it != m_lstSectionInfo.end(); ++it)
  {
  SectionInfo& si = *it;
  intermediate.AddSection(*si.pSection, si.type, si.color, si.linewidth, si.visible,si.description);
  }

  for(TPointInfoList::iterator it = m_lstPointInfo.begin(); it != m_lstPointInfo.end(); ++it)
  {
  PointInfo& pi = *it;
  intermediate.AddPoint(*pi.pPoint, pi.color, pi.pointsize, pi.visible);
  }

  if(m_bHasColor)
  intermediate.Color(m_color);

  if(m_bHasDrawInfo)
  intermediate.DrawInfo(m_bDrawInfo);
}

void CCachingIntermediate::OnSectionDestroyed(const well::IWellSection& section)
{
  TSectionInfoList::iterator it = m_lstSectionInfo.begin();
  while(it != m_lstSectionInfo.end())
  {
  if(it->pSection == &section)
      it = m_lstSectionInfo.erase(it);
  else
      ++it;
  }
}

void CCachingIntermediate::OnPointDestroyed(const well::CWellPoint& wellpoint)
{
  TPointInfoList::iterator it = m_lstPointInfo.begin();
  while(it != m_lstPointInfo.end())
  {
  if(it->pPoint == &wellpoint)
      it = m_lstPointInfo.erase(it);
  else
      ++it;
  }
}
