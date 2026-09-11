// DCWeightLoad.cpp: implementation of the WeightLoad class.
//
//////////////////////////////////////////////////////////////////////
#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCWeightLoad.h"
#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

// size is gravity acceleration g
CWeightLoad::CWeightLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection)
:	IDirectedLoad(loadcase, size, vecDirection)
{
}

CWeightLoad::~CWeightLoad()
{
}

bool CWeightLoad::WriteFilos() const
{
  ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(Direction());

  PushDir();

  // activate weight load
  ChangeDir("WEIGHT");

  // set model parameters
  ChangeDir("/MODEL");

  assert( ValueSize() == 1 );
  PutItem("GRAVAC", Values());
  PutItem("GRAVDI", &idxdir);

  PopDir();

  return true;
}

} // namespace dia
