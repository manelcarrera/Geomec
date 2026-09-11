// GM3MeshZone.h: interface for the CMeshZone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GM3MESHZONE_H__F20CE3FB_7573_4A95_AEF5_81841F60733F__INCLUDED_)
#define AFX_GM3MESHZONE_H__F20CE3FB_7573_4A95_AEF5_81841F60733F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IProgressBase.h"
#include "point.h"
#include <vector>

namespace gm 
{
class C3DDatabase;
class C3DModel;

class C3DMeshZone  
{
  int m_nExponentX;
  int m_nExponentY;
  int m_nIndex;
  std::vector<geo::CPoint> m_vcPoint;
public:
  C3DMeshZone();
  virtual ~C3DMeshZone();

  int Index() const;
  int ExponentX() const;
  int ExponentY() const;
  int Size() const;
  const geo::IPoint& Point(int nIndex) const;

  void ReadMeshZone(C3DDatabase &db,
            const C3DModel& model,
            const long lMeshZoneKey,
            IProgressBase &callback);
};

}

#endif // !defined(AFX_GM3MESHZONE_H__F20CE3FB_7573_4A95_AEF5_81841F60733F__INCLUDED_)
