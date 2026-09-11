// CPointMoment.cpp: implementation of the CPointMoment class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCPointMoment.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "INode.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CPointMoment::CPointMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::INode &node)
:	IDirectedLoad(loadcase, size, vecDirection), m_Node(node)
{
}

CPointMoment::~CPointMoment()
{
}

const geo::INode &CPointMoment::Node() const
{
  return m_Node;
}

bool CPointMoment::WriteFilos() const
{
  ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(Direction());

  ftn_int_t idx = Inquire("NODAL", "DIM");
  if(idx < 0) idx = 0;
  idx++;

  assert(!XistIndexed("NODAL/", &idx));

  PushDir();

  ChangeIndexedDir("NODAL/", &idx);

  PutItem("DIR", &idxdir);

  ftn_int_t nodenr = (ftn_int_t) Node().Index() + 1;
  PutItemLength("NODES", &nodenr, 1);

  PutCharItem("TYPE", "RO");

  PutItemLength("VALUES", Values(), ValueSize());

  PopDir();

  return true;
}

} // namespace dia
