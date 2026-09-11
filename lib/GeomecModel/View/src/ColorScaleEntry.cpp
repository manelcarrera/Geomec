// ComponentEntry.cpp: implementation of the CColorScaleEntry class.
//
//////////////////////////////////////////////////////////////////////


#include "ColorScaleEntry.h"
#include "ColorGradient.h"
#include "HotSpot.h"
#include "ModelBase.h" 
#include "resourceIDS.h"
#include "StreamVersion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CColorScaleEntry::CColorScaleEntry(CModelBase& model)
: IColorScaleEntry(model),	m_pGlobal(0), m_pHotSpot(0), m_color_scale(LOCAL)
{
  // The values of the third and fourth argument are not relevant since the
  // last parameter is false!

 	m_pLocal = new CColorGradient(IDS_LOCAL_COLORSCALE, model, 0, 0, qRgb(0,0,255), qRgb(255, 0, 0), false);

  // wjrx mantis 2560 + mantis 2584
  m_pGlobal= new CColorGradient(Model(), "rainbow");

  new CColorGradient(Model(), "rainbow_rev.");
  new CColorGradient(Model(), "4_bit_dark");
  new CColorGradient(Model(), "4_bit_bright");
  new CColorGradient(Model(), "4_bit_dark_rev.");
  new CColorGradient(Model(), "blue_red");
  new CColorGradient(Model(), "4_bit_bright_rev.");
  new CColorGradient(Model(), "grey_scale_rev.");
  new CColorGradient(Model(), "red_blue");
  new CColorGradient(Model(), "grey_scale");
}

CColorScaleEntry::~CColorScaleEntry()
{
  delete m_pLocal;
}

const CHotSpot* CColorScaleEntry::HotSpot() const
{
  return m_pHotSpot;
}

CHotSpot* CColorScaleEntry::HotSpot()
{
  return m_pHotSpot;
}

const CColorGradient* CColorScaleEntry::GlobalColorGradient() const
{
  return m_pGlobal;
}

CColorGradient* CColorScaleEntry::GlobalColorGradient()
{
  return m_pGlobal;
}

const CColorGradient* CColorScaleEntry::LocalColorGradient() const
{
  return m_pLocal;
}

CColorGradient* CColorScaleEntry::LocalColorGradient()
{
  return m_pLocal;
}

const CColorGradient* CColorScaleEntry::CurrentColorGradient() const
{
  switch (m_color_scale)
  {
  case LOCAL:
    return m_pLocal;
  case GLOBAL:
    return m_pGlobal;
  default:
    return 0;
  }
}

CColorGradient* CColorScaleEntry::CurrentColorGradient()
{
  switch (m_color_scale)
  {
  case LOCAL:
    return m_pLocal;
  case GLOBAL:
    return m_pGlobal;
  default:
    return 0;
  }
}

void CColorScaleEntry::SelectHotSpot(CHotSpot& hot_spot)
{
  assert(IsLinkedTo(hot_spot));
  m_pHotSpot = &hot_spot;
  m_color_scale = HOTSPOT;
  Modified();
}

void CColorScaleEntry::SelectGlobalColorGradient(CColorGradient& gradient)
{
  assert(gradient.IsGlobal());
  assert(IsLinkedTo(gradient));
  m_pGlobal = &gradient;
  m_color_scale = GLOBAL;
  Modified();
}



void CColorScaleEntry::OnNeighbourDeleted(const CGraphNode &node)
{
  // Is one of our selection deleting
  if(m_pGlobal == &node)
  {
    m_pGlobal = 0;
    
    // Try to select an other global node
    TNodeSet stNode = EntryNodes();
    TNodeSet::iterator it = stNode.begin();
    while((it != stNode.end()) && (m_pGlobal == 0))
    {
      CColorGradient *pGradient = dynamic_cast<CColorGradient*>(*it);
      if(pGradient && pGradient->IsGlobal() && pGradient != m_pLocal)
      {
        m_pGlobal = pGradient;
      }
      it++;
    }

    // Are currently selecting our the global gradient
    if(ColorScaleType() == GLOBAL)
    {
      if(m_pGlobal == 0)
        m_color_scale = LOCAL;
    }

    Modified();
  }

  if(m_pHotSpot == &node)
  {
    m_pHotSpot = 0;
    
    // Try to select an other global node
    TNodeSet stNode = EntryNodes();
    TNodeSet::iterator it = stNode.begin();
    while((it != stNode.end()) && (m_pHotSpot == 0))
    {
      CHotSpot *pHotSpot = dynamic_cast<CHotSpot*>(*it);
      if(pHotSpot)
      {
        m_pHotSpot = pHotSpot;
      }
      it++;
    }

    if(ColorScaleType() == HOTSPOT)
    {
      if(m_pHotSpot == 0)
        m_color_scale = LOCAL;
    }

    Modified();
  }

  IColorScaleEntry::OnNeighbourDeleted(node);
}

#define CS_TYPE_GLOBAL 0
#define CS_TYPE_HOTSPOT 1

void CColorScaleEntry::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  if(version >= CStreamVersion(3, 7, 7))
  {
  int nSystem;
  stream >> nSystem;
    TNodeSet stNode = EntryNodes();
  int i;
  for(i = 0; i < nSystem; ++i)
  {
      QString sName;
      stream >> sName;
      bool bFound = false;
      TNodeSet::iterator it;
      for(it = stNode.begin(); it != stNode.end(); ++it)
      {
    if((*it)->Name() == sName)
    {
          bFound = true;
          (*it)->LoadStream(stream, version, progress);
          break;
    }
      }

      // if it's not found we must still load it, so load regular color scale
      if(!bFound)
      {
    CColorGradient* pGradient = new CColorGradient(Model());
    pGradient->LoadStream(stream, version, progress);
      }
  }
  }

  int nSize;
  stream >> nSize;
  for(int i = 0; i < nSize; i++)
  {
    IColorScaleNode *pNode = 0;
    int nType;
    stream >> nType;
    switch(nType)
    {
    case CS_TYPE_GLOBAL:
      pNode = new CColorGradient(Model());
      break;
    case CS_TYPE_HOTSPOT:
      pNode = new CHotSpot(Model());
      break;
    default:
      break;
    }

    assert(pNode);

 		pNode->LoadStream(stream, version, progress);
  }

  int nGlobalSel;
  int nHotSpotSel;
  stream >> nGlobalSel;
  stream >> nHotSpotSel;

  TNodeSet stNode = EntryNodes();
  for(TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    if((*it)->Index() == nGlobalSel)
      m_pGlobal = dynamic_cast<CColorGradient*>(*it);
    if((*it)->Index() == nHotSpotSel)
      m_pHotSpot = dynamic_cast<CHotSpot*>(*it);
  }
}

void CColorScaleEntry::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress)
{
  TNodeSet stNode = EntryNodes();
  int nSize = stNode.size();

  int nNumber = 1;

  // system color gradients are saved separately
  std::vector<CColorGradient*> vcSystem;
  TNodeSet::const_iterator it;
  for(it = stNode.begin(); it != stNode.end(); it++)
  {
    CColorGradient *p = dynamic_cast<CColorGradient*>(*it);
    if(p && p->SystemColorGradient())
      vcSystem.push_back(p);
  }

  stream << int(vcSystem.size());

  for(size_t i = 0; i < vcSystem.size(); ++i)
  {
  // need the name for reference when loading
  vcSystem[i]->SetIndex(nNumber++);
  stream << vcSystem[i]->Name();
  vcSystem[i]->SaveStream(stream, progress);
  }

  nSize -= vcSystem.size();

  stream << nSize;

  // Number and save node
  for(it = stNode.begin(); it != stNode.end(); it++)
  {
    // Is it a gradient or scale
    const CColorGradient *p= dynamic_cast<const CColorGradient*>(*it);
    if ( p )
    {
      if ( ! p->SystemColorGradient() ) //wjrx mantis 2560
      {
    // Number the node
    (*it)->SetIndex(nNumber++);
    
    stream << CS_TYPE_GLOBAL;
        (*it)->SaveStream(stream, progress);
      }
    } else if(dynamic_cast<const CHotSpot*>(*it))
    {
      // Number the node
      (*it)->SetIndex(nNumber++);
      
      stream << CS_TYPE_HOTSPOT;
      (*it)->SaveStream(stream, progress);
    } else
    { 
      assert(false); 
    }
  }

  // Save selection
  if(GlobalColorGradient() == 0)
    stream << -1;
  else
    stream << GlobalColorGradient()->Index();
  
  if(HotSpot() == 0)
    stream << -1;
  else
    stream << HotSpot()->Index();
} 

long CColorScaleEntry::SavedItems() const
{
  long nRet = 0;
  TNodeSet stNode = EntryNodes();
  for(TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    nRet += (*it)->SavedItems();
  }

  return nRet;
}

CColorScaleEntry::COLORSCALE_TYPE CColorScaleEntry::ColorScaleType() const
{
  return m_color_scale;
}

const IColorScaleNode& CColorScaleEntry::CurrentColorScale() const
{
  switch(m_color_scale)
  {
  case LOCAL:
    assert(m_pLocal);
    return *m_pLocal;
    break;
  case GLOBAL:
    assert(m_pGlobal);
    return *m_pGlobal;
    break;
  case HOTSPOT:
    assert(m_pHotSpot);
    return *m_pHotSpot;
    break;
  default:
    assert(false);	// Unknown
    break;
  }

  assert(false);
  return *m_pLocal;
}

IColorScaleNode& CColorScaleEntry::CurrentColorScale()
{
  switch(m_color_scale)
  {
  case LOCAL:
    assert(m_pLocal);
    return *m_pLocal;
    break;
  case GLOBAL:
    assert(m_pGlobal);
    return *m_pGlobal;
    break;
  case HOTSPOT:
    assert(m_pHotSpot);
    return *m_pHotSpot;
    break;
  default:
    assert(false);	// Unknown
    break;
  }

  assert(false);
  return *m_pLocal;
}

bool CColorScaleEntry::ColorScaleType(const COLORSCALE_TYPE type)
{
  // Can we proceed?
  switch(m_color_scale)
  {
  case LOCAL:
    assert(m_pLocal);	// We should always default to local
    break;
  case GLOBAL:
    if(m_pGlobal == 0)	// Cannot proceed
      return false;
    break;
  case HOTSPOT:
    if(m_pHotSpot == 0)	// Cannot proceed
      return false;
    break;
  default:
    assert(false);	// Unknown
    break;
  }

  if(m_color_scale != type)
  {
    m_color_scale = type;
    Modified();	// Send a modified to the world
  }

  return true;
}
