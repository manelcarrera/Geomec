// RpnConstant.h: interface for the CRpnConstant class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNCONSTANT_H__F39184C0_8034_42CB_AA66_860D001116A1__INCLUDED_)
#define AFX_RPNCONSTANT_H__F39184C0_8034_42CB_AA66_860D001116A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnobject.h"

namespace rpn {

class CRpnConstant : public CRpnObject  
{
public:
	enum RPN_CONSTANT { CON_PI, CON_E };
private:
	RPN_CONSTANT m_constant;
public:
	CRpnConstant();
	CRpnConstant(CRpnStack &stack, RPN_CONSTANT constant);
	virtual ~CRpnConstant();

	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual QString Formula() const;
	virtual void Clear(CRpnStack &stack);

	virtual void SaveStream(std::stringstream& stream);
	virtual void LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack);
	virtual eObjectType ObjectType() const;
	virtual CRpnObject* Clone(CRpnStack& NewStack) const;
	virtual bool Recursive(TParentSet stParent = TParentSet()) const;

  virtual bool Defined() const { return true; }
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet& /*stObjects*/) const {}

  virtual CRpnObject* GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const;
};

}

#endif // !defined(AFX_RPNCONSTANT_H__F39184C0_8034_42CB_AA66_860D001116A1__INCLUDED_)
