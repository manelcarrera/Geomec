// SurfaceComposite.h: interface for the CSurfaceComposite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACECOMPOSITE_H__9CF5C694_179D_407F_A564_F5737709948A__INCLUDED_)
#define AFX_SURFACECOMPOSITE_H__9CF5C694_179D_407F_A564_F5737709948A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorNode.h"

//The surface composite is the base class for horizons and 
//faults, because they all have surfaces. This class is 
//responsible for managing the surfaces. The class also takes 
//care of the drag and drop of the surfaces.
class CSurface;
//##ModelId=3D633E5002AB
class CSurfaceComposite : public CColorNode
{
public:
	//##ModelId=3D633E5002CB
	virtual ~CSurfaceComposite();

	//##ModelId=3D635C820231
	typedef std::vector<CSurface*> TSurfaceVec;
	//##ModelId=3D635C820175
	TSurfaceVec Surfaces() const;
};

#endif // !defined(AFX_SURFACECOMPOSITE_H__9CF5C694_179D_407F_A564_F5737709948A__INCLUDED_)
