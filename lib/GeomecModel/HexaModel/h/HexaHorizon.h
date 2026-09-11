// HexaHorizon.h: interface for the CHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXAHORIZON_H__176948F5_D657_4F88_B586_292069B8CF3B__INCLUDED_)
#define AFX_HEXAHORIZON_H__176948F5_D657_4F88_B586_292069B8CF3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DHorizon.h"
#include "GeomecModelVisitor.h"

//Implements GeoItem for the horizon
//
//The childeren of a horizon are surfaces. The only object a 
//horizon accepts are surfaces (and maybe pointset which can 
//be meshed to a surface.)
//
//A horizon separates the formations from each other. A 
//horizon is represented by a collection of surfaces. The 
//surfaces of a horizon can't cross each other or can't be a 
//subset of each other. A surface of a horizon hasn't double 
//z co-ordinates on the same xy value. The edge of same every 
//edge of a horizon is always connected to a boundery, fault 
//or another horizon. An horizon  Horizons cannot cross each 
//other.
//##ModelId=3BC55D620073
class CHexaFormation;
class CHexaModel;
class CHexaHorizon : public C3DHorizon
{
private:
	class CFaceXYLess
	{
	public:
		bool operator()(const geo::IFace* f1, const geo::IFace* f2) const;
	};

	typedef std::set<const geo::IFace*, CFaceXYLess> TFaceSet;
	mutable TFaceSet m_stHexFaces;
	const geo::CElementGroup* m_pInterfaceElementGroup;
public:
	// construction
	CHexaHorizon(CFemAppModel& model);
	CHexaHorizon(CSurfaceBase &surface, CFemAppModel& model, bool bSlip = false, bool bAttachToModel = true);
	CHexaHorizon(const QString& strInstanceName, const double& dDepth, CFemAppModel& model, bool bAttachToModel = true);
	CHexaHorizon(const CHexaHorizon &rhs);
	virtual ~CHexaHorizon();

	CHexaFormation* UpperFormation() const;
	CHexaFormation* LowerFormation() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;
	virtual int DisplayListSize() const;

	virtual const geo::CElementGroup* InterfaceElementGroup() const;
	void InterfaceElementGroup(const geo::CElementGroup& group);

	// Assignment
	bool operator==(const CHexaHorizon& rhs) const;
	CHexaHorizon& operator=(const CHexaHorizon& rhs);

	void MoveUp();
	void MoveDown();
	bool CanMove() const;

	const geo::IFace* FindXYFace(const geo::IFace& face) const;

	virtual bool Destroy();
	bool LocalDestroy(CHexaFormation* pUpper, CHexaFormation* pLower,
		const CHexaFormation* selection);
	virtual bool CanDestroy() const;

	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;
	virtual void OnNeighbourModified(const CGraphNode &item, enum ModifiedHint uHint);

	virtual bool Less(const CGraphNode &node) const;

	// Save and load
	virtual long SavedItems() const;

	// Stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	
	IValueDomainScalar::TValueVec CalculatePorePressure(const geo::IBody& body) const;

  virtual int MeshedSurfaceSize() const;
  virtual const geo::ISurface& MeshedSurface(int nIndex) const;

	virtual int MeshFaceSize() const;  
	virtual const geo::IFace &MeshFace(int nIndex) const;

	virtual int BodyFaceSize() const;
	virtual const geo::IFace &BodyFace(int nIndex) const;

	virtual bool CanSlip() const;

  virtual bool IsTopHorizon() const;

  void AttachToEntry();

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif //_DEBUG

  ACCEPT_GEOMECMODELVISITORS(VisitHexaHorizon);

};



//typedef CGraphEntryTemp<CHorizon> THorizonEntry;
class CHexaHorizonEntry : public CStorageNodeEntry<CHexaHorizon> 
{
public:
	typedef CSingleCommandTemplate<CHexaHorizonEntry> THexaHorizonEntryCommand;
	void InsertHorizon(CHexaHorizon &horizon);
	// Sorted vector of horizons
	typedef std::vector<CHexaHorizon*> THorizonVec; 

	// Construction ....
	CHexaHorizonEntry(CHexaModel& model);
	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;
	THorizonVec Horizons() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaHorizonEntry);
};

/*
typedef CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, CHexaHorizon, CHorizonObserver<true>, IDI_TRI_CHECKED, true, DELETE_ITEM> THexaHorizonEntryObserver;
class CHexaHorizonEntryObserver : public THexaHorizonEntryObserver
{
public:
	CHexaHorizonEntryObserver(CHexaHorizonEntry& entry, CTreeCtrl& ctrl, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	virtual CTreeNode* InsertChild(CHexaHorizon& t);
	virtual void OnChildModified(CTreeNode &child);			// Called when child is modified
};

typedef CEnumerationBranch<CFaultPressure, CValueCompositeObserver, false, FIXED_ITEM> TFaultPressureEnumerator;
class CFaultPressureEnumerator : public TFaultPressureEnumerator
{
public:
	CFaultPressureEnumerator(CTreeNode& parent);
//	virtual bool OnFilter(const CFaultPressure& faultpressure) const;
//	virtual bool Enable();
};
*/

#endif // !defined(AFX_HEXAHORIZON_H__176948F5_D657_4F88_B586_292069B8CF3B__INCLUDED_)
