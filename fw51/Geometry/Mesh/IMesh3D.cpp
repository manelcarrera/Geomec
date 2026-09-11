 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMesh3D.cpp: implementation of the IMesh3D class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "IBody.h"
#include "IMesh3D.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

CMesh3D::CMesh3D()
{
}

CMesh3D::~CMesh3D()
{
}

CMesh3D::TBodyVec CMesh3D::PointInMesh(const IPoint &point) const
{
  TBodyVec vcRet;

  std::vector<int> vcIndex = ElementsAt(point);
  for(int i = 0; i < vcIndex.size(); i++)
  {
    const IBody* pBody = dynamic_cast<const IBody*>(&Element(vcIndex[i]));
    if(pBody)
      vcRet.push_back(pBody);
  }

  return vcRet;
}

}
