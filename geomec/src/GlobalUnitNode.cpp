#include "StdAfx.h"

#include "GlobalUnitNode.h"
#include "FemAppDoc.h"

CGlobalUnitNode::CGlobalUnitNode()
: m_unitNode(retrieveGlobalUnitNode())
{
}

const CUnitNode& CGlobalUnitNode::unitNode() const
{
  return m_unitNode;
}

// private

// static

const CUnitNode& CGlobalUnitNode::retrieveGlobalUnitNode()
{
  POSITION position = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate *docTemplate = AfxGetApp()->GetNextDocTemplate(position);

  position = docTemplate->GetFirstDocPosition();

  CFemAppDoc* femAppDoc =
  dynamic_cast <CFemAppDoc*> (docTemplate->GetNextDoc(position));

  assert(femAppDoc != 0);

  return femAppDoc->UnitNode();
}
