// 3DHorizon.h: interface for the C3DHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DHORIZON_H__B357B67B_7D32_4C4A_A96B_97E0C96F1286__INCLUDED_)
#define AFX_3DHORIZON_H__B357B67B_7D32_4C4A_A96B_97E0C96F1286__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HorizonBase.h"
#include "GeoSurface.h"
#include "LengthQuantity.h"
#include "ITriangle.h"
#include "GeomecModelVisitor.h"

class CSurfaceBase;
class CLengthQuantity;

class C3DHorizon : public CHorizonBase  
{
public:
	typedef std::vector<CSurfaceBase*> TSurfaceVec;

private:
	TSurfaceVec m_vcSurfaces;
	mutable CSurfaceBase::TGeoSurface* m_pConstDepthSurface;
	CLengthQuantity* m_pDepth;
	int m_nDivisionNorthing;
	int m_nDivisionEasting;

	class CElementSet : public geo::IElementSet
	{
	public:
		CElementSet(const C3DHorizon& horizon);
		~CElementSet();

		virtual void AssertValid() const;
		virtual const geo::IPoint &Point(int nIndex) const;
		virtual int PointSize() const;
		virtual const geo::IElement &Element(int nIndex) const;
		virtual int ElementSize() const;
		virtual std::vector<int> Nodes(const geo::IElement &element) const;

	protected:
		virtual geo::IPoint &PointAt(int nIndex);

	private:
		void AddElement(const geo::IElement& element);
		void RegisterElement(const geo::IElement& element);

	private:
		const C3DHorizon& m_horizon;
		std::vector<const geo::IElement*> m_vcElement;
		std::vector<const geo::IPoint*> m_vcPoint;
		typedef std::map<const geo::IPoint*, size_t> TPointIndexMap;
		mutable TPointIndexMap m_mpPointIndexMap;

		class CTriangle : public geo::ITriangle
		{
		public:
			CTriangle(const CElementSet& elementset, const geo::IPoint& p1, const geo::IPoint& p2, const geo::IPoint& p3, int nIndex);

			virtual const geo::IElementSet* IndexingElementSet() const;
			virtual const geo::IPoint &Point(int nIndex) const;
			virtual void Point(int nIndex, const geo::IPoint &pt);
			virtual int NrOfPoints() const;
			virtual size_t Order() const;
			virtual int PointIndex(int nIndex) const;
			virtual int Index() const;

		private:
			const CElementSet& m_elementset;
			std::vector<const geo::IPoint*> m_vcPoint;
			int m_nIndex;
		};

		std::vector<CTriangle*> m_vcOwnElements;
		typedef std::map<const geo::IPoint*, geo::CPoint> TPointMap;
		TPointMap m_mpOwnPoints;
	};

	mutable CElementSet* m_pElementSetCache;

private:
	void InvalidateConstantDepthSurface();
	void OnCreateConstantDepthSurface() const;
	void InvalidateElementSetCache();
	void CreateElementSetCache() const;
  void UpdateTopHorizonState();

protected:
	void MergeSurface(const CSurfaceBase& surface);
  virtual double CalculateAverageDStif() const;

public:
	C3DHorizon(CFemAppModel& model);
	C3DHorizon(CSurfaceBase &surface, CFemAppModel& model, bool bSlip = false, bool bAttachToEntry = true);
	C3DHorizon(const QString& strInstanceName, const double& dDepth, CFemAppModel& model, bool bAttachToEntry = true);
	C3DHorizon(const QString& strInstanceName, CFemAppModel& model, bool bAttachToEntry = true);
	C3DHorizon(const C3DHorizon &rhs);
	virtual ~C3DHorizon();

	bool operator==(const C3DHorizon& rhs) const;
	C3DHorizon& operator=(const C3DHorizon& rhs);

	virtual const geo::IObject& DisplayList(int nIndex) const;
	virtual int DisplayListSize() const;

	virtual int MeshFaceSize() const  {assert(false); return 0; }
	virtual const geo::IFace &MeshFace(int /*nIndex*/) const { assert(false); geo::IFace *pBogus = 0; return *pBogus; }

	virtual int BodyFaceSize() const { assert(false); return 0; }
	virtual const geo::IFace &BodyFace(int /*nIndex*/) const { assert(false); geo::IFace *pBogus = 0; return *pBogus; }

  virtual int MeshedSurfaceSize() const { assert(false); return 0; }
  virtual const geo::ISurface& MeshedSurface(int /*nIndex*/) const { assert(false); geo::ISurface* pBogus = 0; return *pBogus; }

	virtual const dia::IMaterial &InterfaceMaterial(const geo::IElement &element, const CDepletionStage& stage) const;

	void ExportConstantDepthSurface(const QString& fileName);

	bool ConstantDepth() const;
	const CLengthQuantity& Depth() const;
	int DivisionNorthing() const;
	int DivisionEasting() const;
	void Division(int nNorthing, int nEasting);

	const geo::ISurface& GeneratedConstantDepthSurface() const;
	const geo::IElementSet& AsElementSet() const;

	void Depth(const double& depth, const CLengthQuantity::UNIT unit = CLengthQuantity::SI_UNIT);
	virtual bool IsDepthValid(const double& depth, const CLengthQuantity::UNIT unit = CLengthQuantity::SI_UNIT);

	int SurfaceSize() const;
	const CSurfaceBase& Surface(int nIndex) const;
	CSurfaceBase& Surface(int nIndex);

	virtual unsigned int IconId() const;
	virtual void OnNeighbourModified(const CGraphNode &item, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &item);
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual bool CanConnectItem(const CGraphNode& item) const;

	// Save and load
	virtual long SavedItems() const;

	// Stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	
	virtual bool Less(const CGraphNode &node) const;

  virtual bool IsTopHorizon() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif //_DEBUG

  ACCEPT_GEOMECMODELVISITORS(Visit3DHorizon);

};

#endif // !defined(AFX_3DHORIZON_H__B357B67B_7D32_4C4A_A96B_97E0C96F1286__INCLUDED_)