 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMesh3D.h: interface for the IMesh3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMESH3D_H__49599AF9_8E17_4C8F_A617_7D3526C8D526__INCLUDED_)
#define AFX_IMESH3D_H__49599AF9_8E17_4C8F_A617_7D3526C8D526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mesh.h"

#include "GeometryExports.h"

namespace geo { 
class IBody;
class GEOMETRY_EXPORT  CMesh3D  : public CMesh
{
public:
	typedef std::vector<const IBody*> TBodyVec;

	// Construction / Destruction
	CMesh3D();
	virtual ~CMesh3D();

	virtual TBodyVec PointInMesh(const IPoint &point) const;
};
}
#endif // !defined(AFX_IMESH3D_H__49599AF9_8E17_4C8F_A617_7D3526C8D526__INCLUDED_)
