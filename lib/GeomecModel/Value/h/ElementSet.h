// ElementSet.h: interface for the CElementSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTSET_H__DB0FC035_D7A6_45E6_ACAE_F4F72551C30E__INCLUDED_)
#define AFX_ELEMENTSET_H__DB0FC035_D7A6_45E6_ACAE_F4F72551C30E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPointSet.h"

class CElementValueSet;
class CFormationBase;
class CGoCadVolume;
class CEclipseFile;

#include "ivaluecomposite.h"
#include "Mesh.h"
#include "ExportFormat.h"
#include "GeomecModelVisitor.h"

namespace geo {
  class IMesh;
}

#include "IElementSet.h"
#include "IQuadrilateral.h"

class IElementSet : public IPointSet
{
	friend class CElementValueSet;
	typedef std::vector<CElementValueSet*> TElementValueSetVec;
	TElementValueSetVec m_vcElementValueSet;
//protected:
///	virtual void OnNeighbourDeleted(const CGraphNode &node);

public:
	typedef std::map<const geo::IElement*, double> TWeightMap;
	typedef IValueComposite::TWeightingType TWeightingType;

	IElementSet(const QString& sName, CFemAppModel& model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit);
	IElementSet(CFemAppModel& model);
	IElementSet(const IElementSet& rhs);
	virtual ~IElementSet();

	virtual bool HasElements() const { return true; }

	// Compare and assignment
	bool operator==(const IElementSet &rhs) const;
	IElementSet& operator=(const IElementSet& rhs);

	// Display lists
	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

	// Value set
	virtual int ValueSetSize() const;
	virtual const IValueSet& ValueSet(int nIndex) const;
	virtual IValueSet& ValueSet(int nIndex);

	// Element valueset
	virtual int ElementValueSetSize() const;
	virtual const CElementValueSet& ElementValueSet(int nIndex) const;
	virtual CElementValueSet& ElementValueSet(int nIndex);
	int AddElementValueSet();

	// Graph Notification
	virtual void OnNeighbourDeleted(const CGraphNode &item);

	// We only load the base of IPointSet after 3.0.4
	virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
	virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
	virtual long SavedItems() const;

	// Element access
	virtual const geo::IElementSet &ElementSet() const = 0;
	virtual geo::IElementSet &ElementSet() = 0;
	virtual bool PointInConvexHull(const geo::IPoint& pt) const; 

	virtual int PointSize() const { return ElementSet().PointSize();}
	virtual const geo::IPoint& PointAt(int nIndex) const { return ElementSet().Point(nIndex);}
	virtual std::vector<int> PointAt(const geo::IPoint& /*pt*/) const{ std::vector<int> ret; return ret;}

	// Creating node and elements
	virtual int RegisterPoint( const geo::IPoint& point ) = 0;
	typedef std::vector<int> TNodeVec;
	virtual int CreateElement(const TNodeVec& vcIndex) = 0;

	virtual geo::CPoint Max() const { return ElementSet().Max();}
	virtual geo::CPoint Min() const { return ElementSet().Min();}

  bool CanAssignToBoundary() const;
	void AssignToBoundary();

	// Export functions
	virtual void Export(const IExportFormat::CExportArg& arg, const QString& fileName);

	virtual void OnCoordinateUnitChanged(CQuantity::UNIT /*old_unit*/, CQuantity::UNIT /*new_unit*/){};

	// Get weight-factors for given node in elementset for the adjacent elements
	// Each node in the elementset is linked to one or more elements
	// The resulting map maps those elements to the weight factors for a value exactly on the given node
	// The given formation is only used in the (derived) mesh elementset, it is ignored in the implementation of this class
	virtual TWeightMap WeightFactors(int nNode, TWeightingType nWeightingType, const CFormationBase *pFormation = 0) const;

	virtual bool AnyUnitConversionDefined() const { return IPointSet::AnyUnitConversionDefined();} //wjrx mantis 3125

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  ACCEPT_GEOMECMODELVISITORS(VisitIElementSet);
};

class CElementSet : public IElementSet
{
public:
	typedef enum eElementType {	HEXA = 0,		// Hexahedron element
								TETRA,			// Tetrahedron element
								UNKNOWN,		// Unknown element type
								ECLIPSE,		// Eclipse voxel
								TRIANGLE_2D,	// 2D Triangle element
								TRIANGLE_3D,	// 3D Triangle element (not supported yet)
								LINE_2D,		// 2D Line element
								QUAD_2D,        // 2D Quad element (not supported yet)
								QUAD_3D         // 3D Quad element (not supported yet)
								} TElementType;

  class CLineElement : public geo::ILine
	{
		geo::IMesh& m_mesh;
		int m_nodes[2];
		int m_nIndex;

	public:
		CLineElement(geo::IMesh& mesh, int nPoint1, int nPoint2);
		virtual size_t Order() const { return 1; }
		virtual int PointIndex(int nIndex) const;
		virtual size_t NrOfNodes() const;
		virtual const geo::INode& Node(int nIndex) const;
		virtual void Node(int nIndex, const geo::IPoint& point);
		virtual const geo::IElementSet* IndexingElementSet() const;
		virtual int Index() const;
    virtual int ElementType() const { return LINE_2D; }
	};
	class CRectangleElement : public geo::IQuadrilateral
	{
		geo::IMesh& m_mesh;
		int m_nodes[4];
		int m_nIndex;

	public:
		CRectangleElement(geo::IMesh& mesh, int nPoint1, int nPoint2, int nPoint3, int nPoint4);
		virtual size_t Order() const { return 1; }
		virtual int PointIndex(int nIndex) const;
		virtual size_t NrOfNodes() const;
		virtual const geo::INode& Node(int nIndex) const;
		virtual void Node(int nIndex, const geo::IPoint& point);
		virtual const geo::IElementSet* IndexingElementSet() const;
		virtual int Index() const;
    virtual int ElementType() const { return QUAD_3D; }
	};
private:
	geo::CMesh m_mesh;
	eElementType m_eElType;

public:
	typedef std::vector<geo::CPoint> TPointVec;
	typedef std::vector<int> TElement;
	typedef std::vector<TElement> TElementVec;
	CElementSet(const QString &sName, 
				eElementType type,
				const TPointVec& vcPoint,
				const TElementVec& vcElement,
				CFemAppModel &model);
	CElementSet(const CGoCadVolume& volume, 
				   CFemAppModel &model, 
				   CQuantity::UNIT coordinate_unit, 
				   CQuantity::UNIT value_unit);
	CElementSet(const QString& sName, 
				   CFemAppModel &model, 
				   CQuantity::UNIT coordinate_unit, 
				   CQuantity::UNIT value_unit);
	CElementSet(const CElementSet& volume, CModelBase& model, TPROGRESS& progress, std::map<const CValueType*, CValueType*>& mpValueType);
	CElementSet(CFemAppModel &model);
	CElementSet(const CElementSet& rhs);
	CElementSet(const CEclipseFile &file, CModelBase &model);
	CElementSet(const std::set<const geo::IElement*> &stElements,
						 const QString &sName, 
						 eElementType type,
						 CFemAppModel &model);
	CElementSet(const std::vector<const geo::IElement*> &vcElements,
						 const QString &sName, 
						 eElementType type,
						 CFemAppModel &model);

	virtual ~CElementSet();

	// Mesh
	geo::CMesh& Mesh() { return m_mesh; }

	// Assignment
	bool operator==(const CElementSet& rhs) const;
	CElementSet& operator=(const CElementSet& rhs);
	
	virtual const geo::IElementSet &ElementSet() const;
	virtual geo::IElementSet &ElementSet();
	virtual void OnCoordinateUnitChanged(CQuantity::UNIT old_unit, CQuantity::UNIT new_unit);


	// Pure virtual from CGraphNode *******************************
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;
	// ************************************************************

	// Pure virtual from IPointSet ********************************
	virtual IPointSet::DIMENSION Dimension() const;
	// ************************************************************
	
	virtual bool Empty() const;
	virtual void ZAxis(enum Z_AXIS axis);

	int ElementPointSize() const;

// Save and load stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
	eElementType ElementType() const {return m_eElType;}
	void ElementType(eElementType type);
	virtual bool CanConnectItem(const CGraphNode& item) const;
	virtual bool ConnectItem(const CGraphNode& item);

	virtual int RegisterPoint( const geo::IPoint& point );
	virtual int CreateElement(const TNodeVec& vcIndex);

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  ACCEPT_GEOMECMODELVISITORS(VisitElementSet);
};

#endif // !defined(AFX_ELEMENTSET_H__DB0FC035_D7A6_45E6_ACAE_F4F72551C30E__INCLUDED_)
