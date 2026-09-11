 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ObjectDispatcher.cpp: implementation of the CObjectDispatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "MeshNodeGroup.h"
#include "VectorTempl.h"
#include "IElementSet.h"
#include "Value.h"
#include "IProgressBase.h"
#include "Line.h"
#include "IRectangle.h"
#include "ITriangle.h"
#include "Polygon.h"
#include "Hexahedron.h"
#include "ITetrahedron.h"
#include "IBox.h"
#include "ISphere.h"
#include "ICircle.h"
#include "IConus.h"
#include "IPlane.h"
#include "IElementSet.h"
#include "IPolyLine.h"
#include "NearestNeighbour.h"
#include "Tying.h"
#include "IMesh.h"
#include "IVolume.h"
#include "ISurface.h"
#include "IPointElement.h"
#include "IInterfaceElement.h"
#include "IQuadrilateral.h"

#include "DispatchVisitorBase.h"
#include "ObjectDispatcher.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CObjectDispatcher::CObjectDispatcher()
{
}

CObjectDispatcher::~CObjectDispatcher()
{
}

// dispatches the object and delegates to handler to handle the dispatched object
bool CObjectDispatcher::Dispatch(const IObject &object, CDispatchVisitorBase &handler)
{
	// coordinates
	const IPoint *pPoint = dynamic_cast<const IPoint*> (&object);
	if(pPoint && handler.HandlePoint(*pPoint)) return true;

	const IVector *pVector = dynamic_cast<const IVector*> (&object);
	if(pVector && handler.HandleVector(*pVector)) return true;

	const ICoordinate *pCoord = dynamic_cast<const ICoordinate*> (&object);
	if(pCoord && handler.HandleCoordinate(*pCoord)) return true;

	// lines
	const ILine *pLine = dynamic_cast<const ILine*> (&object);
	if(pLine && handler.HandleLine(*pLine)) return true;

	// faces
	const CPolygon *pPolygon = dynamic_cast<const CPolygon*> (&object);
	if(pPolygon && handler.HandlePolygon(*pPolygon)) return true;

	const ITriangle *pTriangle = dynamic_cast<const ITriangle*> (&object);
	if(pTriangle && handler.HandleTriangle(*pTriangle)) return true;

	const IRectangle *pRect = dynamic_cast<const IRectangle*> (&object);
	if(pRect && handler.HandleRectangle(*pRect)) return true;

	const IQuadrilateral *pQuad = dynamic_cast<const IQuadrilateral*> (&object);
	if(pQuad && handler.HandleQuadrilateral(*pQuad)) return true;

  const IFace *pFace = dynamic_cast<const IFace*> (&object);
  if(pFace && handler.HandleFace(*pFace)) return true;
  
	// interface
	const IInterfaceElement *pInterface = dynamic_cast<const IInterfaceElement*> (&object);
	if(pInterface && handler.HandleInterface(*pInterface)) return true;
	

	// plane
	const IPlane *pPlane = dynamic_cast<const IPlane*> (&object);
	if(pPlane && handler.HandlePlane(*pPlane)) return true;

	// circle
	const ICircle *pCircle = dynamic_cast<const ICircle*> (&object);
	if(pCircle && handler.HandleCircle(*pCircle)) return true;

	// conus
	const IConus *pConus = dynamic_cast<const IConus*> (&object);
	if(pConus && handler.HandleConus(*pConus)) return true;

	// sphere
	const ISphere *pSphere = dynamic_cast<const ISphere*> (&object);
	if(pSphere && handler.HandleSphere(*pSphere)) return true;

	// bodies
	const IBox *pBox = dynamic_cast<const IBox*> (&object);
	if(pBox && handler.HandleBox(*pBox)) return true;

	const ITetrahedron *pTetrahedron = dynamic_cast<const ITetrahedron*> (&object);
	if(pTetrahedron && handler.HandleTetrahedron(*pTetrahedron)) return true;

	const CHexahedron *pHexahedron = dynamic_cast<const CHexahedron*> (&object);
	if(pHexahedron && handler.HandleHexahedron(*pHexahedron)) return true;

	const IBody *pBody = dynamic_cast<const IBody*> (&object);
	if(pBody && handler.HandleBody(*pBody)) return true;

	// point element
	const IPointElement *pPointElement = dynamic_cast<const IPointElement*> (&object);
	if(pPointElement && handler.HandlePointElement(*pPointElement)) return true;

	// element
	const IElement *pElement = dynamic_cast<const IElement*> (&object);
	if(pElement && handler.HandleElement(*pElement)) return true;

	// elementsets
	const ISurface *pSurface = dynamic_cast<const ISurface*> (&object);
	if(pSurface && handler.HandleSurface(*pSurface)) return true;

	const IVolume *pVolume = dynamic_cast<const IVolume*> (&object);
	if(pVolume && handler.HandleVolume(*pVolume)) return true;

	const IMesh *pMesh = dynamic_cast<const IMesh*> (&object);
	if(pMesh && handler.HandleMesh(*pMesh)) return true;

	const IPolyLine *pPolyLine = dynamic_cast<const IPolyLine*> (&object);
	if(pPolyLine && handler.HandlePolyLine(*pPolyLine)) return true;

	const IElementSet *pElementSet = dynamic_cast<const IElementSet*> (&object);
	if(pElementSet && handler.HandleElementSet(*pElementSet)) return true;

	// array
	const IArray *pArray = dynamic_cast<const IArray*> (&object);
	if(pArray && handler.HandleArray(*pArray)) return true;

	const CMeshNodeGroup *pMeshNodeGroup = dynamic_cast<const CMeshNodeGroup*> (&object);
	if(pMeshNodeGroup && handler.HandleMeshNodeGroup(*pMeshNodeGroup)) return true;

	// object
	if(handler.HandleObject(object)) return true;

	// not handled
	return false;
}

} // namespace geo
