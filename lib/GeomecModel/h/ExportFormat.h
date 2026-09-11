// PropertyFile.h: interface for the CPropertyFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTFORMAT_H__INCLUDED_)
#define AFX_EXPORTFORMAT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueType.h"
#include "AnalysisType.h"
#include "NewWellPoint.h"

class IValueComponentBase;
class CHotSpot;
class CPointSet;
class IElementSet;
class CModelBase;
class CFormationBase;
class CFormationPlane;
class CDepletionStage;
class CHorizonBase;
class CResultRegister;
class CCrossSection;
class CNewWellPath;
class CWellCasingRTCI;
class CValueType;
class CNonMeshedSurface;

#include "SingleQuantity.h"

class IExportFormat
{
public:
	// Type defs
	typedef std::vector<const IValueComponentBase*> TDataVec;
	typedef std::vector<CValueType*>		    TValueTypeVec;
	typedef enum AXIS_SYSTEM { AXIS3D, AXIS2D } TAxisSystem;
	typedef enum OUTPUT_TYPE { OT_INTEGER, OT_DOUBLE, OT_STRING } TOutputType;
	typedef CQuantity::UNIT UNIT;

  enum TElementExportType { FULL_ELEMENT = 0, CENTER_ELEMENT = 1, CENTER_WELL = 2};
  enum TWellPathExportType { INPUT_POINTS = 0, MESH_INTERSECTION_POINTS = 1 };

	/*!
	  Arguments for options of the exports
	*/
	class CExportArg
	{
	public:
		CExportArg(	UNIT					unit = CQuantity::SI_UNIT,	
					TAxisSystem				axis_system = AXIS3D,
					CValueType::TMapEnum    map_type = CValueType::MT_NONE,
					const CHotSpot*         pHotSpot = 0,
					const CDepletionStage*  pDeformation = 0,
          const CAnalysisType&    antype = CAnalysisType::AT_NONLIN,
					const CResultRegister*  pResultRegister = 0,
          TElementExportType elementCenterPoints = FULL_ELEMENT,
          TWellPathExportType wellpathPoints = INPUT_POINTS,
					bool                    bOldFormat = false,
          bool                    bFormationNames = false,
          bool                    bGVT = false)
		: m_unit(unit), 
		  m_axis_system(axis_system), 
		  m_map_type(map_type), 
		  m_pHotSpot(pHotSpot), 
		  m_pDeformation(pDeformation), 
		  m_antype(antype), 
		  m_pResultRegister(pResultRegister),
		  m_bOldFormat(bOldFormat),
		  m_elementCenterPoints(elementCenterPoints),
      m_wellpathPoints(wellpathPoints),
      m_bFormationNames(bFormationNames),
      m_bGVT(bGVT)
    {
    }

		UNIT				   m_unit;			   // Unit system
		TAxisSystem			   m_axis_system;	   // Axis system of the export
		CValueType::TMapEnum   m_map_type;		   // Map type
		const CHotSpot*        m_pHotSpot;		   // Hot sport or not
		const CDepletionStage* m_pDeformation;     // Depletion stage of deformation
		CAnalysisType          m_antype;		   // Analysis type of deformation
		const CResultRegister* m_pResultRegister;  // Result register of deformation
		bool                   m_bOldFormat;
		TElementExportType m_elementCenterPoints; // export only center points of elements
    TWellPathExportType m_wellpathPoints;
    bool m_bFormationNames;
    bool m_bGVT;
	};
	
	// Construction
	IExportFormat(const TDataVec& vcData, const CExportArg& arg);
	IExportFormat(const TValueTypeVec& vcData, const CExportArg& arg);
  virtual ~IExportFormat() {}

	virtual const CHotSpot* HotSpot() const = 0;
	virtual int RowSize() const = 0;
	virtual int ColumnSize() const;
	virtual TOutputType OutputType(int nRow, int nColumn) const;
	virtual double DoubleAt(int nRow, int nColumn) const = 0;
	virtual QString StringAt(int nRow, int nColumn) const;
	virtual int IntegerAt(int nRow, int nColumn) const = 0;
	virtual bool HasRowComment(int nRow) const = 0;
	virtual QString RowComment(int nRow) const = 0;
	virtual QString ExportType() const = 0;
	virtual bool HasExportName() const { return false; }
	virtual QString ExportName() const { assert( false ); return 0; }
  virtual bool isGVT() const = 0;
	
	// Access to the data values
	size_t DataValueSize() const;
	const IValueComponentBase& DataValue(size_t nIndex) const;

  size_t ValueTypeSize() const;
  const CValueType& ValueType(size_t nIndex) const;
	virtual int getDataOffset() const = 0;
	virtual void DestroyCache() const = 0;
protected:
	QString LoadFromString(unsigned int uString) const;
	struct TOutputValue { QString sString; double dDouble; int nInteger; };
	typedef std::pair<TOutputType, TOutputValue> TOutput;
	TOutput DataOutput( const geo::IValue& value ) const;
	CExportArg m_arg;
	TValueTypeVec m_vcValueType;
	TDataVec m_vcData;
private:
  static TDataVec eliminateDuplicates(const TDataVec& dataVector);
};

class IElementSetExportFormat;
class CNodalExportFormat : public IExportFormat
{
	const CPointSet* m_pPointSet; // We export a pointset
	const IElementSetExportFormat*	m_pElementSetFormat; // We export an elementset
	const CNewWellPath* m_pNewWellPath; // We Export a new well path // wjrx mantis 3322

  typedef std::pair<CNewWellPoint, geo::CValue> TPointValue; // same definition as in wellpath, cannot include that header though (TODO: extract def to separate file)
  typedef std::vector<TPointValue> TPointValueVector;
  mutable std::vector<TPointValueVector> m_wellPathValues;

  mutable std::list<CNewWellPoint>::const_iterator m_iteratorWellPath;
	mutable int m_row_cache_index; // Index of the current row cache
	mutable std::vector<TOutput>    m_row_cache; // Row cache
	bool m_bAddFormationNames;
  bool m_export2LAS;
  mutable int m_iteratorWellPathPosition;
  double ConvCoor(const double& dValue, UNIT unit) const;
	void CreateCache(int nRow) const;
	virtual void DestroyCache() const;
	mutable int m_data_offset;
public:
	CNodalExportFormat(const CPointSet& point_set,		// PointSet to export
					   const TValueTypeVec& vcData,		// Data on the pointset
					   const CExportArg& arg,			// Export options
             bool export2LAS = false);
	CNodalExportFormat(const CPointSet& point_set,		// PointSet to export
					   const TDataVec& vcData,			// Data on the pointset
					   const CExportArg& arg,			// Export options
             bool export2LAS = false);
  CNodalExportFormat(const IElementSetExportFormat& element_set_format,
					   const TDataVec& vcData,			// Data on the pointset
					   const CExportArg& arg,			// Export options
             bool export2LAS = false);
  CNodalExportFormat(const IElementSetExportFormat& element_set_format,
					   const TValueTypeVec& vcData,		// Data on the pointset
					   const CExportArg& arg,			// Export options
             bool export2LAS = false);
  CNodalExportFormat(const CNewWellPath& well_path,		// Export of a new wellpath // wjrx mantis 3322
					   const TDataVec& vcData,			// Data on the pointset
					   const CExportArg& arg,			// Export options
             bool export2LAS = false);

	// The interface of the export format
	virtual int RowSize() const;
	virtual int ColumnSize() const;
	virtual TOutputType OutputType(int nRow, int nColumn) const;
	virtual double DoubleAt(int nRow, int nColumn) const;
	virtual QString StringAt(int nRow, int nColumn) const;
	virtual int IntegerAt(int nRow, int nColumn) const;
	virtual bool HasRowComment(int nRow) const;
	virtual QString RowComment(int nRow) const;
	virtual const CHotSpot* HotSpot() const;
	virtual QString ExportType() const;
	virtual bool HasExportName() const;
	virtual QString ExportName() const;
  virtual bool isGVT() const;

	UNIT Unit() const;
	const CModelBase& Model() const;
	virtual int getDataOffset() const;
};

class IElementSetExportFormat : public IExportFormat
{
	CNodalExportFormat m_nodal_format;
	typedef std::map<int, int> TElementNodeSizeMap;
	mutable TElementNodeSizeMap m_mpElementNodeSize;
	void InitElementNodeSize() const;
	typedef std::vector<TOutput> TElementVec;
	typedef std::vector<TElementVec> TRowVec;
	mutable TRowVec m_element_cache;
	mutable int m_element_cache_index;
	void CreateCache(int nElementIndex) const;
	virtual void DestroyCache() const;
protected:
	typedef std::pair<int, int> TElementIndex;
	TElementIndex ElementIndex(int nRow) const;
public:
	IElementSetExportFormat(const TValueTypeVec& vcNodalData,// Data for the node section
				const TValueTypeVec& vcElementData,// Data for the element section
				const CExportArg& arg);// Export options
	IElementSetExportFormat(const TDataVec& vcNodalData,// Data for the node section
				const TDataVec& vcElementData,// Data for the element section
				const CExportArg& arg);// Export options

	const CNodalExportFormat& NodalExportFormat() const;

	virtual int RowSize() const;
	virtual int ColumnSize() const;
	virtual TOutputType OutputType(int nRow, int nColumn) const;
	virtual double DoubleAt(int nRow, int nColumn) const;
	virtual QString StringAt(int nRow, int nColumn) const;
	virtual int IntegerAt(int nRow, int nColumn) const;
	virtual bool HasRowComment(int nRow) const;
	virtual QString RowComment(int nRow) const;
	virtual const CHotSpot* HotSpot() const;
  virtual bool isGVT() const;
	QString ElementType() const;

	// Interface of the elementset format
	virtual const CModelBase& Model() const = 0;
	virtual int NodeSize() const = 0;
	virtual int NodeIndex(int nIndex) const = 0;
	virtual void Node(geo::IPoint& point, int nIndex) const = 0;
	virtual int ElementSize() const = 0;
	virtual int ElementNodeIndex(int nElementIndex, int nLocalNodeIndex) const = 0;
	virtual const geo::IElement& Element(int nIndex) const = 0;
	virtual bool TwoDimensional() const = 0;
};

class CElementSetExportFormat : public IElementSetExportFormat
{
	typedef std::map<int, const geo::IPoint*> TNodeMap;
	typedef std::pair<int, const geo::IPoint*> TNode;
	typedef std::vector<TNode> TNodeVec;
	typedef std::pair<const geo::IElementSet*, QString> TElementSet;
	typedef std::vector<TElementSet> TElementSetVec;
	void AddElementSet( TNodeMap& mpNode, const geo::IElementSet& element_set, const QString& sName );
	void AddNodes( const TNodeMap& mpNode );
  static bool sortNodeOnIndex(const TNode& lhs, const TNode& rhs);

	const CModelBase&      m_model;
	TElementSetVec         m_vcElementSet;
	TNodeVec               m_vcNode;
	bool                   m_bTwoDimensional;
	bool				   m_bUseMesh;
  bool                   m_bAddFormationNames;

public:
	CElementSetExportFormat(const IElementSet& element_set,
				const TValueTypeVec& vcElementSetData,
				const CExportArg& arg);	

	CElementSetExportFormat(const IElementSet& element_set,
				const TDataVec& vcElementData,
				const CExportArg& arg);// Export options

	typedef std::vector<const CFormationBase*> TFormationVec;
	CElementSetExportFormat(const TFormationVec& vcFormation, 
				const TDataVec& vcNodalData,
				const TDataVec& vcElementData,
				const CExportArg& arg);// Export options

        CElementSetExportFormat(const CFormationBase& formation,
				const TDataVec& vcNodalData,		
				const TDataVec& vcElementData,
				const CExportArg& arg);	// Export options

	CElementSetExportFormat(const CFormationPlane &formation_plane,
				const TDataVec& vcElementData,
				const CExportArg& arg);	// Export options

	//wjrx mantis 2950
        typedef std::vector<const CFormationPlane *> TFormationPlaneVec;
        CElementSetExportFormat
        ( const TFormationPlaneVec &formation_plane_vec,
          const TDataVec& vcElementData,
          const CExportArg& arg) ;	// Arguments for the export

	typedef std::vector<const CHorizonBase*> THorizonVec;
	CElementSetExportFormat(const THorizonVec& vcHorizon,
				const TDataVec& vcNodalData,
				const TDataVec& vcElementData,
				const CExportArg& arg);// Export options

  typedef std::vector<const CWellCasingRTCI*> TRTCIVec;
  CElementSetExportFormat(const TRTCIVec& vcRTCI,
				const TDataVec& vcNodalData,
				const TDataVec& vcElementData,
				const CExportArg& arg);// Export options

	CElementSetExportFormat(const CCrossSection &xsec,
		                const TDataVec& vcNodalData,
				const TDataVec& vcElementData,
				const CExportArg& arg);	// Export options

  typedef std::vector<const CNonMeshedSurface*> TNonMeshedSurfaceVec;
  CElementSetExportFormat(const TNonMeshedSurfaceVec& vcSurface,
                          const TDataVec& vcNodalData,
                          const TDataVec& vcElementData,
                          const CExportArg& arg);

//	CElementSetExportFormat(const CResultTree& result_tree,
//							const CExportArg& arg);			// Export options);

	void sortNodeVector();

	// Interface of the elementset format
	virtual int ColumnSize() const;
	virtual TOutputType OutputType(int nRow, int nColumn) const;
	virtual QString StringAt(int nRow, int nColumn) const;

	virtual const CModelBase& Model() const;
	virtual int NodeSize() const;
	virtual int NodeIndex(int nIndex) const;
	virtual void Node(geo::IPoint& point, int nIndex) const;
	virtual int ElementSize() const;
	virtual int ElementNodeIndex(int nElementIndex, int nLocalNodeIndex) const;
	virtual const geo::IElement& Element(int nIndex) const;
	virtual bool HasRowComment(int nRow) const;
	virtual QString RowComment(int nRow) const;
	virtual bool TwoDimensional() const;
	virtual QString ExportType() const;
	virtual int getDataOffset() const;
};

#endif // !defined(AFX_EXPORTFORMAT_H__INCLUDED_)
