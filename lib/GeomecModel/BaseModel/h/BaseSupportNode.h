#ifndef _BASESUPPORTNODE_H_
#define _BASESUPPORTNODE_H_

class CValueType;
class CBaseSupportNode;
class CDepletionStage;

#include "StorageNode.h"
#include "openglnode.h"
#include "GeomecModelVisitor.h"

class CBoundaryBase;

class CSupportDepletion : public CStorageNode
{
public:
	CSupportDepletion(const CDepletionStage& stage, CBaseSupportNode &snode); 
	
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	// Function to support drag and drop ...
	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;

	virtual bool Empty() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion &version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
	virtual bool Less(const CGraphNode &node) const;

	virtual void OnNewNeighbour(const CGraphNode &node);
	void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	void OnNeighbourDeleted(const CGraphNode &node);

  bool IsValueTypeAllowed() const;
	
	inline const CValueType *ValueType() const { return m_pValueType; }
	inline const CDepletionStage& Stage() const { return m_stage; }

  ACCEPT_GEOMECMODELVISITORS(VisitSupportDepletion);

private:
	void InvalidateResults() const;

private:
	const CValueType *m_pValueType; // pointer to the value type dragged onto the dep.stage.
	const CDepletionStage& m_stage;
};

class CBaseSupportNode : public COpenGLNode
{
public:
  CBaseSupportNode(unsigned int uName, CBoundaryBase& boundary);
  CBaseSupportNode(const CBaseSupportNode& rhs);
  virtual ~CBaseSupportNode();

  const CSupportDepletion* SupportDepletion(const CDepletionStage& stage) const;
  CSupportDepletion* SupportDepletion(const CDepletionStage& stage);
	const CValueType* DistriValues(const CDepletionStage &stage) const;
	bool Distributed(const CDepletionStage &stage) const;

  virtual bool IsValueTypeAllowed(const CDepletionStage& stage) const;
  virtual bool IsValidValueTypeId(unsigned int /*uValueType*/) const { assert(FALSE); return false; } // must be overridden

  const CBoundaryBase& Boundary() const;
  CBoundaryBase& Boundary();

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitBaseSupportNode);

private:
	void ClearChildren();
	void CreateChildren();

private:
	std::vector<CSupportDepletion*> m_vcDepStages;
};

#endif // _BASESUPPORTNODE_H_
