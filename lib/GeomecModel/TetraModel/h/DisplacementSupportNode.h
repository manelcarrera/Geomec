// DisplacementSupportNode.h: interface for the CDisplacementSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLACEMENTSUPPORTNODE_H__44007B2A_91A7_4F4B_AD49_29302D6AFA63__INCLUDED_)
#define AFX_DISPLACEMENTSUPPORTNODE_H__44007B2A_91A7_4F4B_AD49_29302D6AFA63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DSupportNode.h"
#include "GeomecModelVisitor.h"

class CModelBase;

class CDisplacementSupportNode : public C3DSupportNode  
{
	friend class CNodalSupportDlg;

public:
	class CNodalSupportDef
	{
	public:
		class CSupportDef
		{
		public:
			typedef enum _SupportType
			{
				ST_NONE = 0,
				ST_LOCAL,
				ST_GLOBAL
			} TSupportType;

			CSupportDef();
			~CSupportDef();
			bool operator==(const CSupportDef &rhs) const;

			TSupportType SupportType() const;
			void SupportType(TSupportType nType);
			geo::IVector &LocalVector();
			const geo::IVector &LocalVector() const;

			CSupportDef Interpolate(const CSupportDef &rhs, const double &dist, const double &distrhs,
				const geo::IPoint &pt, const geo::IMatrix &MatPreMult, const geo::IPoint &ptReference) const;

			// always returns a vector based on the support type
			// the matrix and points are used if ST_GLOBAL is set
			geo::CVector Vector(const geo::IPoint &pt, const geo::IMatrix &MatPreMult, const geo::IPoint &ptReference) const;

			void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
			void SaveStream(TSTREAM &stream, TPROGRESS &progress);

		private:
			TSupportType m_nSupportType;
			geo::CVector m_vecLocal;
		};

		CNodalSupportDef();
		CNodalSupportDef(const CNodalSupportDef &rhs);
		CNodalSupportDef &operator=(const CNodalSupportDef &rhs);
		bool operator==(const CNodalSupportDef &rhs) const;

		// interpolate a new definition from distances (between this and rhs)
		// the point is the point this definition is valid for
		// the premult matrix is derived from the global tensor
		// the ptReference is the reference point for the global tensor
		CNodalSupportDef Interpolate(const CNodalSupportDef &rhs, const double &dist, const double &distrhs,
			const geo::IPoint &pt, const geo::IMatrix &MatPreMult, const geo::IPoint &ptReference) const;

		const CSupportDef &SupportDef(const CDepletionStage &stage) const;
		CSupportDef &SupportDef(const CDepletionStage &stage);
		void Update(const CDepletionStage &Initial);

		void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress, const CModelBase &model);
		void SaveStream(TSTREAM &stream, TPROGRESS &progress);

	private:
		typedef std::map<const CDepletionStage*, CSupportDef> TSupportMap;
		TSupportMap m_SupportMap;
	};

	CDisplacementSupportNode(CInterfaceBoundary &boundary);
	virtual ~CDisplacementSupportNode();
	virtual bool Less(const CGraphNode &node) const;

	virtual unsigned int IconId() const;

// Save and load stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual bool Empty() const;
	virtual long SavedItems() const;

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	void OnMeshModified();

	virtual COpenGLNode::CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;

	virtual bool UsingGlobalTensor(const CDepletionStage &Stage) const;
	virtual void OnGlobalTensorInputUndefined(const CDepletionStage &Stage) const;

	// returns 0 if not existing for the given point
	const CNodalSupportDef *NodalSupportDef(const geo::IPoint &pt) const;

	const geo::IPoint &ReferencePoint() const;

	static geo::CVector DisplacementFromStrainTensor(const geo::IPoint &pt, const geo::IPoint &ptReference,
		const geo::IMatrix &MatPreMult);

	geo::CVector DisplacementFromDistribution(const geo::IPoint &pt,const CDepletionStage &stage) const;

  virtual bool IsValidValueTypeId(unsigned int uValueType) const;

  typedef std::vector <const geo::IPoint*> TCornerNodeVec;

  TCornerNodeVec getCornerNodes() const;
  void setSelectedPoint(const geo::IPoint *selectedPoint);
  const geo::IPoint* getSelectedPoint() const;

  geo::CMatrix CreateGlobalTensorPreMultMatrix(const CDepletionStage& stage) const;

  ACCEPT_GEOMECMODELVISITORS(VisitDisplacementSupportNode);

private:
	class CNodeDrawDef : public COpenGLNode::CDrawDef
	{
	public:
		CNodeDrawDef(const COpenGLNode& node);
		virtual DrawDecisionVector Color(const geo::IObject &object) const;
		virtual DrawDecisionFloat PointSize() const;
	};

	class CSelectedNodeDrawDef : public COpenGLNode::CDrawDef
	{
	public:
		CSelectedNodeDrawDef(const COpenGLNode& node);
		virtual DrawDecisionVector Color(const geo::IObject &object) const;
		virtual DrawDecisionFloat PointSize() const;
	};

	class CReferenceNodeDrawDef : public COpenGLNode::CDrawDef
	{
	public:
		CReferenceNodeDrawDef(const COpenGLNode& node);
		virtual DrawDecisionVector Color(const geo::IObject &object) const;
		virtual DrawDecisionFloat PointSize() const;
	};

	typedef std::map<const geo::IPoint*, std::vector<const geo::ILine*>, geo::ICoordinate::CCoordinateLess> TPtLinesMap;

	mutable TCornerNodeVec m_vcCornerNodes;
	mutable const geo::IPoint *m_pSelectedPoint;
	mutable const geo::IPoint *m_pReferencePoint;

public:
	typedef std::map<geo::CPoint, CNodalSupportDef> TStageSupportMap;

	TStageSupportMap& getSupports() const;

private:
	mutable TStageSupportMap m_Supports;

	void GetCornerNodes() const;
	void BuildStageSupportMap() const;
	void UpdateStageSupportMap() const;
	void DetermineReferencePoint(const geo::IPoint *pt) const;
	void DetermineGenericReferencePoint(TPtLinesMap &mpLines) const;
	//void ClearChildren();
	//void CreateChildren();
};

#endif // !defined(AFX_DISPLACEMENTSUPPORTNODE_H__44007B2A_91A7_4F4B_AD49_29302D6AFA63__INCLUDED_)
