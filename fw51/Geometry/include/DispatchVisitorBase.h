 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DispatchVisitorBase.h: interface for the IVisitor and CVisitorBase classes.
//

#include "GeometryExports.h"
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPATCHVISITORBASE_H__6538175C_53BA_4B86_B5EC_B1DB9247BAA6__INCLUDED_)
#define AFX_DISPATCHVISITORBASE_H__6538175C_53BA_4B86_B5EC_B1DB9247BAA6__INCLUDED_


class CLabelPoint;
class ISymbol;


namespace geo {

class IPoint;
class IVector;
class ICoordinate;
class ILine;
class IPolygon;
class ITriangle;
class IRectangle;
class IFace;
class IPlane;
class ICircle;
class IConus;
class ISphere;
class IBox;
class ITetrahedron;
class IHexahedron;
class IBody;
class IElement;
class ISurface;
class IVolume;
class CBodyGroup;
class IMesh;
class IPolyLine;
class IElementSet;
class IArray;
class CPolygonPtrArray;
class CMeshNodeGroup;
class IPointElement;
class IInterfaceElement;
class IQuadrilateral;
class IObject;



class GEOMETRY_EXPORT IVisitor
{
protected:
	// must be derived
	IVisitor();

public:
	virtual ~IVisitor();

	// default
	virtual bool HandleDefault(const IObject &Object) = 0;

	// coordinates
	virtual bool HandlePoint(const IPoint &Point);
	virtual bool HandleVector(const IVector &Vector);
	virtual bool HandleCoordinate(const ICoordinate &Coord);

	// lines
	virtual bool HandleLine(const ILine &Line);

	// faces
	virtual bool HandlePolygon(const IPolygon &Polygon);
	virtual bool HandleTriangle(const ITriangle &Triangle);
	virtual bool HandleRectangle(const IRectangle &Rect);
	virtual bool HandleFace(const IFace &Face);
	virtual bool HandleQuadrilateral(const IQuadrilateral &Quad);
	
	// interfaces
	virtual bool HandleInterface(const IInterfaceElement &interfaceElement);
	
	// plane
	virtual bool HandlePlane(const IPlane &Plane);

	// circle
	virtual bool HandleCircle(const ICircle &Circle);

	// circle
	virtual bool HandleConus(const IConus &Conus);

	// sphere
	virtual bool HandleSphere(const ISphere &Sphere);

	// bodies
	virtual bool HandleBox(const IBox &Box);
	virtual bool HandleTetrahedron(const ITetrahedron &Tetrahedron);
	virtual bool HandleHexahedron(const IHexahedron &Hexahedron);
	virtual bool HandleBody(const IBody &Body);

	// point element
	virtual bool HandlePointElement(const IPointElement &PointElement);

	// element
	virtual bool HandleElement(const IElement &Element);

	// elementsets
	virtual bool HandleSurface(const ISurface &Surface);
	virtual bool HandleVolume(const IVolume &Volume);
	virtual bool HandleMesh(const IMesh &Mesh);
	virtual bool HandlePolyLine(const IPolyLine &PolyLine);
	virtual bool HandleElementSet(const IElementSet &ElementSet);

	// array
	virtual bool HandleArray(const IArray &Array);

	// object
	virtual bool HandleObject(const IObject &Object);

};


class GEOMETRY_EXPORT CVisitorBase : public IVisitor
{
public:

	// default
	virtual bool HandleDefault(const IObject &/*Object*/) { return false; }

	virtual bool HandleBodyGroup(const CBodyGroup &BodyGroup);
	virtual bool HandlePolygonPtrArray(const CPolygonPtrArray &PolygonPtrArray);
	virtual bool HandleMeshNodeGroup(const CMeshNodeGroup &MeshNodeGroup);

};
typedef CVisitorBase CDispatchVisitorBase;


}; // namespace geo

#endif // !defined(AFX_DISPATCHVISITORBASE_H__6538175C_53BA_4B86_B5EC_B1DB9247BAA6__INCLUDED_)
