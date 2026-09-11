/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DispatchVisitorBase.cpp: implementation for the IVisitor class.
//

#include "DispatchVisitorBase.h"
#include "IObject.h"
#include "IPoint.h"
#include "IVector.h"
#include "ICoordinate.h"
#include "ILine.h"
#include "IPolygon.h"
#include "ITriangle.h"
#include "IQuadrilateral.h"
#include "IRectangle.h"
#include "IInterfaceElement.h"
#include "IPlane.h"
#include "ICircle.h"
#include "IConus.h"
#include "ISphere.h"
#include "IBox.h"
#include "ITetrahedron.h"
#include "IHexahedron.h"
#include "ISurface.h"
#include "IPointElement.h"
#include "IVolume.h"
#include "IMesh.h"
#include "IPolyLine.h"
#include "IElementSet.h"
#include "BodyGroup.h"
#include "MeshNodeGroup.h"

namespace geo {



IVisitor::IVisitor()
{
}

IVisitor::~IVisitor()
{
}


bool IVisitor::HandlePoint(const IPoint &Point)
{
	return HandleCoordinate(Point);
}

bool IVisitor::HandleVector(const IVector &Vector)
{
	return HandleCoordinate(Vector);
}

bool IVisitor::HandleCoordinate(const ICoordinate &Coord)
{
	return HandleObject(Coord);
}

bool IVisitor::HandleLine(const ILine &Line)
{
	return HandleElement(Line);
}

bool IVisitor::HandlePolygon(const IPolygon &Polygon)
{
	return HandleFace(Polygon);
}

bool IVisitor::HandleTriangle(const ITriangle &Triangle)
{
	return HandleFace(Triangle);
}

bool IVisitor::HandleRectangle(const IRectangle &Rect)
{
	return HandleQuadrilateral(Rect);
}

bool IVisitor::HandleFace(const IFace &Face)
{
	return HandleElement(Face);
}

bool IVisitor::HandleQuadrilateral(const IQuadrilateral &Quad)
{
	return HandleFace(Quad);
}

bool IVisitor::HandleInterface(const IInterfaceElement &interfaceElement)
{
	return HandleElement(interfaceElement);
}

bool IVisitor::HandlePlane(const IPlane &Plane)
{
	return HandleObject(Plane);
}

bool IVisitor::HandleCircle(const ICircle &Circle)
{
	return HandleObject(Circle);
}

bool IVisitor::HandleConus(const IConus &Conus)
{
	return HandleObject(Conus);
}

bool IVisitor::HandleSphere(const ISphere &Sphere)
{
	return HandleObject(Sphere);
}

bool IVisitor::HandleBox(const IBox &Box)
{
	return HandleHexahedron(Box);
}

bool IVisitor::HandleTetrahedron(const ITetrahedron &Tetrahedron)
{
	return HandleBody(Tetrahedron);
}

bool IVisitor::HandleHexahedron(const IHexahedron &Hexahedron)
{
	return HandleBody(Hexahedron);
}

bool IVisitor::HandleBody(const IBody &Body)
{
	return HandleElement(Body);
}

bool IVisitor::HandlePointElement(const IPointElement &PointElement)
{
	return HandleElement(PointElement);
}

bool IVisitor::HandleElement(const IElement &Element)
{
	return HandleObject(Element);
}

bool IVisitor::HandleSurface(const ISurface &Surface)
{
	return HandleElementSet(Surface);
}

bool IVisitor::HandleVolume(const IVolume &Volume)
{
	return HandleElementSet(Volume);
}

bool IVisitor::HandleMesh(const IMesh &Mesh)
{
	return HandleElementSet(Mesh);
}

bool IVisitor::HandlePolyLine(const IPolyLine &PolyLine)
{
	return HandleElementSet(PolyLine);
}

bool IVisitor::HandleElementSet(const IElementSet &ElementSet)
{
	return HandleObject(ElementSet);
}

bool IVisitor::HandleArray(const IArray &Array)
{
	return HandleObject(Array);
}

bool IVisitor::HandleObject(const IObject &Object)
{
	return HandleDefault(Object);
}



bool CVisitorBase::HandleBodyGroup(const CBodyGroup &BodyGroup)
{
	return HandleVolume(BodyGroup);
}

bool CVisitorBase::HandlePolygonPtrArray(const CPolygonPtrArray &PolygonPtrArray)
{
	return HandleArray(PolygonPtrArray);
}

bool CVisitorBase::HandleMeshNodeGroup(const CMeshNodeGroup &MeshNodeGroup)
{
	return HandleObject(MeshNodeGroup);
}



}; // namespace geo

