// MeshResultTree.h: interface for the CMeshResultTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHRESULTTREE_H__979527D7_2F08_498B_8E76_E8F6D928DF05__INCLUDED_)
#define AFX_MESHRESULTTREE_H__979527D7_2F08_498B_8E76_E8F6D928DF05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The mesh result tree supplies 'mesh results' to analyse the mesh. 
// The depth result (MR_DEPTH) is always available and shows the depth of the mesh. The 
// volume result (MR_VOLUME) shows the volume of each element and is only available when 
// the mesh is build. The maximum edge result (MR_MAX_EDGE) shows the maximum edge of an element
// and is only available when the mesh is build. The average edge result (MR_MEAN_EDGE)
// calculates the average edge length of the edges of each element. The surface result 
// (MR_SURFACE) shows the surface of each element.
class CMeshResultTree;
class CResultInfo;

#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "IValueDataInterface.h"
#include "GeomecModelVisitor.h"

class CMeshResult : public IValueComposite  
{
public:
	// Result descriptor // Note that is used for saving purposes.
	class CMeshResultComponent : public IValueComponentBase
	{
	public:

    typedef IValueDataInterfaceScalarTempl<CMeshResultComponent> IValueDataInterfaceMeshResultScalar;

		class CResultValueSet : public geo::IValueSet
		{
			const CMeshResultComponent& m_component;
			const geo::IMesh& m_mesh;
			CQuantity::UNIT m_unit;
			std::pair<double, double> MinMaxEdge( const geo::IElement& element ) const;
			double MeanEdge( const geo::IElement& element ) const;
		public:
			CResultValueSet(const CMeshResultComponent& component, const geo::IMesh& mesh, CQuantity::UNIT = CQuantity::SI_UNIT);
			virtual const geo::IElementSet &ElementSet() const;
      virtual void ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
			CQuantity::UNIT Unit() const;
			virtual void Empty();
			virtual bool IsEmpty() const;
		protected:
			virtual int PushBack(const TValueVec &values);
		};
		typedef std::set<const geo::IPoint*, geo::ICoordinate::CCoordinateLess> TLine;
		typedef std::set<TLine> TLineSet;
	private:
		mutable CResultValueSet* m_pSIValueSet;		// Supplies SI value to the frame work
		mutable CResultValueSet* m_pFieldValueSet;	// Supplies Field value to the frame work
		void AddEdges(const geo::IFace& face, TLineSet& stLine) const;
  protected:
    IValueDataInterfaceMeshResultScalar m_Data;
	public:
		CMeshResultComponent(unsigned int uName, CMeshResult& parent);
		virtual ~CMeshResultComponent();

    virtual TValueComponentType Type() const;
    virtual const IValueDataInterfaceScalar& ScalarData() const;
    virtual IValueDataInterfaceScalar& ScalarData();

    virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;
		virtual	QString UnitName(const UNIT unit) const;
		virtual bool Defined() const;
		virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
		virtual bool Empty() const;
		virtual long SavedItems() const;
    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
		virtual QString ExportLabel() const;
//		virtual geo::CValue Value(const geo::IElement& mesh_element, int nNodeIndex, UNIT unit) const;	
		const geo::IValueSet& ValueSet(const CQuantity::UNIT unit) const;
		virtual bool CanMap(const COpenGLNode& node) const;
    virtual bool CanComputeOnPoints() const;

    virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
	};

public:
	// Construction ...
	CMeshResult(unsigned int uName, int result_type, CMeshResultTree& model);
	virtual ~CMeshResult();

	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
	virtual bool Less(const CGraphNode &node) const;

  virtual bool isMeshResult() const;

	// Export functions
	bool CanExport() const;

  ACCEPT_GEOMECMODELVISITORS(VisitMeshResult);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

};

class CMeshResultTree : public CGraphEntryTemp<CMeshResult>
{
public:
	CMeshResultTree(CModelBase& model);
	bool CanExport() const;
        bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMeshResultTree);
        static const std::set<std::string> &getNames();
private:
        static std::set<std::string> m_stExportLabels;
};



#endif // !defined(AFX_MESHRESULTTREE_H__979527D7_2F08_498B_8E76_E8F6D928DF05__INCLUDED_)
