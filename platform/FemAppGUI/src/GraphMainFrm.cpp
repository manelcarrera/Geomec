// GraphMainFrm.cpp: implementation of the CGraphMainFrm class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "GraphNode.h"
#include "GraphPtr.h"
#include "GraphMainFrm.h"
#include <afxcmn.h>



#define PROGRESS_WIDTH 250
/////////////////////////////////////////////////////////////////////////////
// CGraphMainFrm construction/destruction

//##ModelId=3B653D020272
CGraphMainFrm::CGraphMainFrm(): m_pProgress(0)
{
  // TODO: add member initialization code here
  
}

void CGraphMainFrm::Copy(CGraphNode& node)
{
  if(!node.IsLinkedTo(m_clip_board))
    m_clip_board.LinkTo(node);
  assert(m_clip_board.Ptr() == &node);
}

CGraphNode* CGraphMainFrm::Paste()
{
  return m_clip_board.Ptr();
}

const CGraphNode* CGraphMainFrm::Paste() const
{
  return m_clip_board.Ptr();
}

void CGraphMainFrm::CreateProgress(const CProgressStackItem &item)
{
  assert(!m_pProgress);
  m_pProgress = new CProgressCtrl();

  // Get the dimensions of the first part of the status bar.
  CRect rect;
  StatusBar().GetItemRect(0, &rect);

  // Take a wild guess for the label width
  int nLabelWidth = rect.Width() - PROGRESS_WIDTH;
  nLabelWidth = min(nLabelWidth, item.Caption().GetLength() * 8);

  StatusBar().SetPaneText (0, item.Caption(), TRUE);

  int nBarWidth = min(rect.Width(), PROGRESS_WIDTH);

  CRect rcBar(rect.left + nLabelWidth, rect.top, rect.left + nBarWidth + nLabelWidth, rect.bottom);
  m_pProgress->Create(PBS_SMOOTH|WS_VISIBLE|WS_CHILD, rcBar, &StatusBar(), 1);

  m_pProgress->SetRange32(0, item.MaxPos());
  m_pProgress->SetPos(item.Pos());
}

void CGraphMainFrm::DestroyProgress()
{
  assert(m_pProgress);
  m_pProgress->DestroyWindow();
  delete m_pProgress;
  m_pProgress = 0;
}

void CGraphMainFrm::StartProgress(const CString& sCaption, int nSteps)
{
  if(m_ProgressStack.empty()) m_sStatusText = StatusBar().GetPaneText(0);
  else DestroyProgress();

  m_ProgressStack.push(CProgressStackItem(sCaption, nSteps));
  CreateProgress(m_ProgressStack.top());
}

void CGraphMainFrm::ProgressStep()
{
  if(m_ProgressStack.top().Pos() % m_ProgressStack.top().SkipPos() == 0)
  {
    assert(m_pProgress);
    assert(!m_ProgressStack.empty()); // must create first

    m_pProgress->SetPos(m_ProgressStack.top().Pos());
  }

  m_ProgressStack.top().Step();
}

void CGraphMainFrm::EndProgress()
{
  assert(!m_ProgressStack.empty()); // no current progress
  DestroyProgress();
  m_ProgressStack.pop();

  if(m_ProgressStack.empty()) StatusBar().SetPaneText(0, m_sStatusText, TRUE);
  else CreateProgress(m_ProgressStack.top());
}

// CProgressStackItem implementation
CGraphMainFrm::CProgressStackItem::CProgressStackItem(const CString &caption, int maxpos)
: m_pos(0), m_maxpos(maxpos), m_caption(caption), m_skippos(m_maxpos / 1000)
{
  if(m_skippos == 0)
    m_skippos = 1;
}

CGraphMainFrm::CProgressStackItem::~CProgressStackItem()
{
}

void CGraphMainFrm::CProgressStackItem::Step()
{
//	assert(m_pos < m_maxpos);
  m_pos++;
}

int CGraphMainFrm::CProgressStackItem::Pos() const
{
  return m_pos;
}

int CGraphMainFrm::CProgressStackItem::MaxPos() const
{
  return m_maxpos;
}

int CGraphMainFrm::CProgressStackItem::SkipPos() const
{
  return m_skippos;
}

const CString &CGraphMainFrm::CProgressStackItem::Caption() const
{
  return m_caption;
}
