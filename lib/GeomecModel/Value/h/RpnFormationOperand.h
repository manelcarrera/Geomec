// RpnFormationOperand.h: interface for the CRpnFormationOperand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNFORMATIONOPERAND_H__0914B00B_BFA8_4F98_8C0D_D271B8236FF1__INCLUDED_)
#define AFX_RPNFORMATIONOPERAND_H__0914B00B_BFA8_4F98_8C0D_D271B8236FF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFormationBase;
class CMeshBase;

#include "GraphNode.h"
#include "RpnOperand.h"
#include "GeomecModelVisitor.h"

// Formation observer observes inserted formation and when the
// formation observer disconnects the valuecomponent distroys.
class CFormationObserver : public CGraphNode
{
	rpn::CRpnOperand::IValueProxy& m_proxy;
	CFormationBase& m_formation;
public:
	// Construction
  CFormationObserver(CFormationBase& formation, rpn::CRpnOperand::IValueProxy& proxy);
	const CFormationBase& Formation() const;
	CFormationBase& Formation();
	virtual void OnNeighbourDeleted(const CGraphNode& item);
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

  ACCEPT_GEOMECMODELVISITORS(VisitFormationObserver);
};

class IRpnFormationProxyBase : public rpn::CRpnOperand::IValueProxy
{
private:
  CFormationObserver* m_pObserver;
  int m_nIndex;

public:
	// Pre-defined return values
	typedef IValueDomainScalar::TValueVec TValueVec;			// Vector of return values
	typedef IValueDomainScalar::TValue TValue;
	typedef IValueComponentBase::UNIT UNIT;

public:
  IRpnFormationProxyBase(CFormationBase& formation, rpn::CRpnStack& NewStack);
	IRpnFormationProxyBase(rpn::CRpnStack& NewStack, const QString& sProxyId);
	IRpnFormationProxyBase(const IRpnFormationProxyBase& proxy, rpn::CRpnStack& NewStack);
  ~IRpnFormationProxyBase();
	const CFormationBase& Formation() const;
	CFormationBase& Formation();
  int Index() const;
  virtual CFemAppModel *FindModel(CFemAppModel& model);
	virtual void AttachToModel(CFemAppModel& model);

	void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version);
	virtual void SaveStream(std::stringstream& stream);
	virtual void LoadStream(std::stringstream& stream, CStreamVersion& version);
};

class CRpnFormationProxy : public IRpnFormationProxyBase
{
protected:
	virtual int TypeId() const;
public:
	// Construction with a name, an IValueComponentBase and a RpnStack
	CRpnFormationProxy(CFormationBase& formation, rpn::CRpnStack& NewStack);
	// Constructor for file loading. After the constuctor is called the load function should
	// called after construction. When the model is fully loaded the cross references are 
	// resolved by calling the Enable function.
	CRpnFormationProxy(rpn::CRpnStack& NewStack, const QString& sProxyId);
	CRpnFormationProxy(const CRpnFormationProxy& proxy, rpn::CRpnStack& NewStack);
	virtual ~CRpnFormationProxy();
	virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
	virtual QString TextTag() const;
	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual unsigned int IconId() const;
	virtual bool Recursive(TParentSet stParent) const;
  virtual bool Defined() const { return true; }
  virtual bool exists() const { return true; }
};

class CRpnFormationThicknessProxy : public IRpnFormationProxyBase
{
public:
  CRpnFormationThicknessProxy(CFormationBase& formation, rpn::CRpnStack& NewStack);
  CRpnFormationThicknessProxy(rpn::CRpnStack& stack, const QString& sProxyId);
	CRpnFormationThicknessProxy(const CRpnFormationThicknessProxy& proxy, rpn::CRpnStack& NewStack);
  virtual ~CRpnFormationThicknessProxy();
	virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
	virtual QString TextTag() const;
	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual unsigned int IconId() const;
	virtual bool Recursive(TParentSet stParent) const;
  virtual bool Defined() const;
  virtual bool exists() const;
protected:
	virtual int TypeId() const;
};

// IsReservoir?
class CRpnReservoirProxy : public rpn::CRpnOperand::IValueProxy  
{
private:
	CMeshBase* m_pMesh;
	bool IsElementDepleting(const geo::IElement& element) const;
protected:
	virtual int TypeId() const;
public:
	// Pre-defined return values
	typedef IValueDomainScalar::TValueVec TValueVec;			// Vector of return values
	typedef IValueDomainScalar::TValue TValue;
	typedef IValueComponentBase::UNIT UNIT;

  // Construction with a name, an IValueComponentBase and a RpnStack
	CRpnReservoirProxy(CMeshBase& mesh, rpn::CRpnStack& NewStack);
	CRpnReservoirProxy(CMeshBase& mesh, rpn::CRpnStack& NewStack, const QString& sProxyId);
	CRpnReservoirProxy(rpn::CRpnStack& stack, const QString& sProxyId);
	virtual ~CRpnReservoirProxy();
	virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
	virtual QString TextTag() const;
	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual unsigned int IconId() const;
	void Mesh(CMeshBase& mesh);
	virtual void SaveStream(std::stringstream& /*stream*/) { }
	virtual void LoadStream(std::stringstream& /*stream*/, CStreamVersion& /*version*/) {}
	virtual void AttachToModel(CFemAppModel& model);
	virtual bool Recursive(TParentSet stParent) const;
  virtual bool Defined() const { return true; }
  virtual bool exists() const { return true; }
};

#endif // !defined(AFX_RPNFORMATIONOPERAND_H__0914B00B_BFA8_4F98_8C0D_D271B8236FF1__INCLUDED_)
