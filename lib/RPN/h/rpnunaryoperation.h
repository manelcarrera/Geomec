// rpnunaryoperation.h: interface for the CRpnUnaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNUNARYOPERATION_H__3ABACA92_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_RPNUNARYOPERATION_H__3ABACA92_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnstack.h" 

namespace rpn { 

class CRpnUnaryOperation : public CRpnObject
{
	CRpnObject *m_pOperand;
public:
	enum RPN_OPERATION { OP_SQRT, OP_SIN, OP_ASIN, OP_COS, OP_ACOS, OP_TAN, OP_ATAN, 
						 OP_LOG, OP_LN, OP_ABS, OP_NOT, OP_EXP, OP_SINH, OP_COSH, OP_TANH,
						 OP_SIGN, OP_ROUND };
private:
	RPN_OPERATION m_operation;
	TValue Operation(const TValue &value) const;
public:
	CRpnUnaryOperation();
	CRpnUnaryOperation(CRpnStack &stack, enum RPN_OPERATION operation);
	virtual ~CRpnUnaryOperation();

	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual QString Formula() const;
	virtual void Clear(CRpnStack &stack);

	virtual void SaveStream(std::stringstream& stream);
	virtual void LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack);
	virtual eObjectType ObjectType() const;
	virtual CRpnObject* Clone(CRpnStack& NewStack) const;
	virtual bool Recursive(TParentSet stParent = TParentSet()) const;
  virtual bool Defined() const { return m_pOperand->Defined(); }
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet& stObjects) const;
  virtual CRpnObject* GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const;
};

}

#endif // !defined(AFX_RPNUNARYOPERATION_H__3ABACA92_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
