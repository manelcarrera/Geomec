// RpnValueSet.h: interface for the CRpnValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNVALUESET_H__5ACEF0B6_DB34_406A_A657_B0F759E8D4F9__INCLUDED_)
#define AFX_RPNVALUESET_H__5ACEF0B6_DB34_406A_A657_B0F759E8D4F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecIValueSet.h"
#include "rpnstack.h"

// The RPNValueSet is a valueset which execute the formula of the RPN Caculator for
// each element or each point of the point- or element set. The RPN function can also
// have IValueSet operands which inserted in the formala by the CValueSetProxy.

class CRpnValueSet : public IValueSet
{
	rpn::CRpnStack* m_pStack;
	void SaveStream(std::stringstream& stream);
	void LoadStream(std::stringstream& stream);
  std::set<const CValueComponent*> m_stUsers;

  int m_model;

public:
	typedef IValueDomainScalar::TValueVec TValueVec;			// Vector of return values
	typedef IValueDomainScalar::TValue TValue;
	typedef IValueComponentBase::UNIT UNIT;

	// The coordinate proxy 
	class CValueSetCoordinateProxy : public rpn::CRpnStack::CCoordinateProxy
	{
		CRpnValueSet* m_pValueSet;
	public:
		CValueSetCoordinateProxy(CRpnValueSet& value_set, unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId);
		CValueSetCoordinateProxy(CRpnValueSet& value_set, unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor);
		CValueSetCoordinateProxy(CRpnValueSet& value_set, const QString& sName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId);
		CValueSetCoordinateProxy(rpn::CRpnStack& stack, const QString& sProxyId);		// Constructor for loading
		virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
		void ValueSet(CRpnValueSet& value_set);
		virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
		virtual unsigned int IconId() const;
		virtual int TypeId() const;
		virtual bool Recursive(TParentSet stParent) const;
		virtual bool Defined() const { return true; }
		virtual bool exists() const { return (m_pValueSet != 0); }
	};

	// The valuesetproxy allows other valueset as operand in the formula.
	class CValueSetProxy : public rpn::CRpnOperand::IValueProxy 
	{
	public:
		bool showInDialog() const;
		// ValueSet observer observes the valueset (duhhhhh) and when the
		// valuesetobserver disconnects the valueset distroys.
		class CValueSetObserver : public CGraphNode
		{
			typedef std::set<CValueSetProxy*> TValueProxySet;
			TValueProxySet m_stProxy;
			IValueSet& m_value_set;
		public:
			// Construction
			CValueSetObserver(IValueSet& value_set);
			CValueSetObserver(IValueSet& value_set, CValueSetProxy& proxy);
			CValueSetObserver(const CValueSetObserver& rhs, CValueSetProxy& proxy);
			virtual ~CValueSetObserver();
			CValueSetObserver& operator=(const CValueSetObserver& rhs);
			const IValueSet& ValueSet() const;
			IValueSet& ValueSet();
			virtual void OnNeighbourDeleted(const CGraphNode& item);
			virtual unsigned int IconId() const;
			virtual unsigned int TypeId() const;
			void Attach(CValueSetProxy& proxy);
			void Detach(CValueSetProxy& proxy);
			bool Empty() const;
			void SwapProxy();
		};
	private:
		CValueSetObserver* m_pObserver; 
		int m_nIndex;
	public:
		// Construction with an IValueSet and a RpnStack
		CValueSetProxy(IValueSet& value_set, rpn::CRpnStack& NewStack, bool bObserverIsCopy = false);
		// Construction with the TagText (the name) of value set and the index
		// of in the pointset of the ValueSet. (Using ValueSetSize and ValueSet)
		// This constructor is used during loading. Only the TagText function is
		// works then so that formula can recognize this value proxy. After the 
		// whole pointset is loaded and the value proxy is enabled so the m_pObserver
		// is valid and all functions of the value proxy can be accessed.
		CValueSetProxy(int nIndex, const QString& sProxyId, rpn::CRpnStack& NewStack);
		CValueSetProxy(rpn::CRpnStack& NewStack, const QString& sProxyId);
		CValueSetProxy(const CValueSetProxy& rhs, rpn::CRpnStack& NewStack);
		void CreateCopy(const CValueSetProxy& source);
		void SetObserver(CValueSetObserver* pObserver);
		CValueSetObserver& Observer();
		const CValueSetObserver& Observer() const;
		void Validate();
		virtual ~CValueSetProxy();
		virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
		const IValueSet& ValueSet() const;
		IValueSet& ValueSet();
		void AttachPointSet(IPointSet& point_set);
		virtual QString TextTag() const;
		virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
		virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
		virtual unsigned int IconId() const;
		virtual int TypeId() const;
		virtual void LoadStream(std::stringstream& stream, CStreamVersion& version);
		virtual void SaveStream(std::stringstream& stream);
		virtual bool Recursive(TParentSet stParent) const;
    virtual bool Defined() const { return true; }
		virtual bool exists() const { return (m_pObserver != 0); }
	};

	void InsertFormula(const CRpnValueSet& rhs);

	// Construction
	CRpnValueSet(IPointSet& point_set);
	CRpnValueSet(IPointSet& point_set, const CRpnValueSet& value_set);
	CRpnValueSet(const rpn::CRpnStack& stack);
	CRpnValueSet(const CRpnValueSet& rhs);
	virtual ~CRpnValueSet();

  virtual CRpnValueSet* clone();

	void UpdateProxy();
	void EnableProxy(const CModelBase* enablingModel);

	// Assignment
	bool operator==(const CRpnValueSet& rhs) const;
	CRpnValueSet& operator=(const CRpnValueSet& rhs);

	// Modifing the rpn stack
	void RpnStack(const rpn::CRpnStack& rhs);
	const rpn::CRpnStack& RpnStack() const;

	// Apperance in tree
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	// Saving and loading
	virtual bool Empty() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

	// IValueSet overrides
	virtual int ValueSize() const;
	virtual TValue Min() const;
	virtual	TValue Max() const;
  virtual TValue ValuePoint(const geo::IPoint& pt, geo::IParallelInitializationCallback *cb) const;
  virtual TValueVec ValueElement(const geo::IElement& element, geo::IParallelInitializationCallback *cb) const;

	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourDeleted(const CGraphNode &node);

  bool Used() const;
  virtual bool CanDestroy() const;

  rpn::CRpnStack* getStack() const;
  void setStack(rpn::CRpnStack* stack);

  ACCEPT_GEOMECMODELVISITORS(VisitRpnValueSet);
};

#endif // !defined(AFX_RPNVALUESET_H__5ACEF0B6_DB34_406A_A657_B0F759E8D4F9__INCLUDED_)
