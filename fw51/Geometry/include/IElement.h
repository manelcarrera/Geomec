 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IElement.h: interface for the IElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IELEMENT_H__945505EA_3611_4B7E_ABC7_C47892A2E773__INCLUDED_)
#define AFX_IELEMENT_H__945505EA_3611_4B7E_ABC7_C47892A2E773__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "IObject.h"
#include "Value.h"
#include "VectorTempl.h"
#include "Matrix.h"
#include "ElementPoint.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

class IElementSet;
class CValueSet;
class CLine;
class INode;
class ICacheInterface;
class CElementCacheObject;

typedef enum _WeightingType
{
  WT_NONE,    // no weighting
  WT_VOIGHT,  // volume weighted
  WT_REUSS    // inverse volume weighted
} TWeightingType;


class GEOMETRY_EXPORT  IElement : public IObject  
{
public:
	typedef std::vector<double> TDoubleVec;
	typedef std::vector<std::pair<TDoubleVec, double> > TIntPtVec;

protected:
	IElement();
	void InsertCorners(const IElement &elem, CPtrArray<IPoint> &result) const;
	bool ContainsInvalidValue(const std::vector<CValue> &values) const;

	// returns a "relative" EPS for isoparametric coordinates
	double IsoEps() const;
	TDoubleVec MakeVec(const double& d1, const double& d2, const double& d3) const;
	TDoubleVec MakeVec(const double& d1, const double& d2) const;

public:
	virtual ~IElement();
	//	PRE		: TRUE
	//  POST	: Return value 
	//				0	No element set is assiocated with the element
	//				Ptr Elementset is given. Index() and PointIndex(int) apply to the given elementset.
	virtual const IElementSet* IndexingElementSet() const;
	//  PRE		: IndexingElementSet() returns valid ptr
	//	POST	: Index in the IndexingElementSet is returned
	//			: -1 is returned when IndexingElementSet() == 0
	virtual int Index() const;
	//  PRE		: IndexingElementSet() returns valid ptr
	//	POST	: Index of the point in the IndexingElementSet is returned
	//			: -1 is returned when IndexingElementSet() == 0
	// An element should always be able to return points up to NrOfMeshNodes or NrOfPoints,
	// whichever is larger
	virtual int PointIndex(int nIndex) const;
	// The mid point function returns the middle of the element. Default the mean of the points is given, but
	// this implementation can be overiden for each element implementation
	virtual CElementPoint MidPoint() const;

	virtual ICacheInterface& CacheInterface();

	// An element should always be able to return points up to NrOfMeshNodes or NrOfPoints,
	// whichever is larger
	virtual const IPoint &Point(int nIndex) const = 0;
	virtual void Point(int nIndex, const IPoint &pt) = 0;//Substitute point at nIndex with pt.....
	virtual int NrOfPoints() const = 0;

	// the order of this element (1 = linear, 2 = quadratic, 3 = cubic, etc.)
	virtual size_t Order() const = 0;

	// The NrOfMeshNodes function returns the number of nodes as DIANA sees them
	// This can include higher-order nodes and doubled nodes in interface elements
	virtual size_t NrOfNodes() const;
	virtual const INode& Node(int nIndex) const;
	virtual void Node(int nIndex, const IPoint& point);

	// Return the lines of the elements
	virtual int NrOfLines() const = 0;
	virtual const ILine& Line(int nIndex) const = 0;

	// integration points
	virtual int IntegrationPointSize() const;
	virtual const TDoubleVec& IntegrationPointCoords(int nIndex) const;
	virtual const double& IntegrationPointWeight(int nIndex) const;

	// return the name of the side with the given index
	// returns 0 if not applicable (e.g. for line and point elements)
	virtual const char* SideName(int nIndex) const;

    // returns volume, area or length (depending on type)
	// the default implementation makes use of ShapeFunctionDerived and
	// Jacobian, but it can be overridden for an easier implementation
	virtual double Size() const;

	virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;
	virtual bool operator < (const IElement &rhs) const;

	// old style, for the (old) DianaCore library
	virtual std::string Type() const;

	// IObject Inf...
	virtual CPoint Max() const;
	virtual CPoint Min() const;
	virtual void Move(const IVector &vec);
	virtual void Rotate(const IVector &vec, const double &dAngleDeg);
	virtual void Transform(const IMatrix &matrix);
	virtual void AssertValid() const;
	virtual bool Empty() const;

	// ISOPARAMETRIC COORDINATES ARE DEFINED
	// FROM -1 TO 1 OR FROM 0 TO 1

	// returns a matrix of <NrOfNodes()> * <dimension>
	// this asserts false in IElement and should be implemented
	// for each element class
	virtual CMatrix ShapeFunctionDerived(const TDoubleVec& isocoords) const;

	// return the values of the shape functions
	// the vector is <NrOfNodes()> long
	// this functions asserts false in IElement and should be implemented
	// for each element class
	virtual TDoubleVec ShapeFunction(const TDoubleVec& isocoords) const;

	// convert an isocoordinate to a world point
	virtual CPoint IsoToWorld(const TDoubleVec& isocoords) const;


	virtual TDoubleVec IsoShapeFunction(const IPoint &point) const;

	// returns the Jacobian matrix
	CMatrix Jacobian(const TDoubleVec& isocoords) const;
	// same, but now given the coordinates in a matrix (NrOfNodes * 3)
	CMatrix Jacobian(const TDoubleVec& isocoords, const IMatrix& matcoords) const;

	// returns a matrix filled with all coordinates of the element
	// has <NrOfNodes()> columns
	// has 3 (x, y, z) rows
	CMatrix Coordinates() const;

	// returns a matrix of NrOfNodes * (element dimension) with the
	// isoparametric coordinates for each node
	virtual std::vector<TDoubleVec> IsoCoordinates() const;

	double JacobianDeterminant(const IMatrix& J) const;

	virtual CValue InterpolateValue(const TDoubleVec& isocoords, const std::vector<CValue> &values) const;
	virtual CValue InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const;

	// convert world coordinates into isoparametric coordinates
	// this is not always implemented and may assert false
	// in IElement because of this
	// for higher order elements this requires an iterative procedure
	virtual TDoubleVec WorldToIso(const geo::IPoint& point) const;

	// return the value of the influence sphere for each node (area for a 2D object, volume for a 3D object)
	virtual double InfluenceSize(int nNode) const; 

	// return the indices in the given vector that represent elements that (partially) overlap with this element
	virtual std::vector<int> OverlappingElementIndices(std::vector<const IElement*> &vecElements) const;

	typedef std::pair<const IPoint*, CValue> TValuePoint;
	typedef std::vector<TValuePoint> TSourceValueVector;

	virtual std::vector<CValue> MapValues(TSourceValueVector &SourceValues, TWeightingType nWeightingType, const std::vector<double> &WeightFactors) const;
	virtual std::vector<CValue> MapValues(TSourceValueVector &SourceValues, TWeightingType nWeightingType) const;

	virtual void MapValuesPrepare(CElementCacheObject &cached, TWeightingType nWeightingType) const;
	virtual std::vector<CValue> MapValuesFinal(const CElementCacheObject &cached, const std::vector<CValue> &SourceValues, TWeightingType nWeightingType) const;

	// return a representative length for this element
	// length of a line
	// square root of area for a face
	// 3rd square root of volume for a body
	virtual double RepresentativeLength() const = 0;

	// checks whether the element is degenerate (e.g. concave)
	virtual bool Degenerate() const { return false; }

	// the minimum and maximum angles between two edges in a point (mesh quality) in degrees
	// first is minimum
	// second is maximum
	std::pair<double, double> MinMaxAngles() const;

	// replace the node with local index nIndex by the node with mesh node index nNewGlobalMeshNodeIndex
	// this replaces the node only locally in this element, not globally in the mesh
	virtual void ReplaceNode(int nIndex, int nNewGlobalMeshNodeIndex);

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleElement(*this); }

  virtual int ElementType() const { return 0; }

  virtual bool IsElement() const;

private:
	void MapValuesVoightPrepare(CElementCacheObject &cached) const;
	std::vector<double> MapValuesVoightFinal(const CElementCacheObject &cached, const std::vector<CValue> &SourceValues) const;

	std::vector<double> MapValuesVoight(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors) const;
	std::vector<double> MapValuesReuss(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors) const;
	double ReussDerived(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors, const IMatrix &NodeValues, int iIndex) const;


};
}

#endif // !defined(AFX_IELEMENT_H__945505EA_3611_4B7E_ABC7_C47892A2E773__INCLUDED_)
