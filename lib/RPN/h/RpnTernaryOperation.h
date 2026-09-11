// RpnTernaryOperation.h: interface for the CRpnTernaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNTERNARYOPERATION_H__B4B97982_18E4_49EF_B447_0A54E62AC2F0__INCLUDED_)
#define AFX_RPNTERNARYOPERATION_H__B4B97982_18E4_49EF_B447_0A54E62AC2F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnobject.h"

// The Ternary Rpn operator requires tree operands. for the time being only the IF operator
// is defined.

namespace rpn {

class CRpnStack;
class CRpnTernaryOperation : public CRpnObject {
  CRpnObject *m_pOperandA;
  CRpnObject *m_pOperandB;
  CRpnObject *m_pOperandC;
  TValue Operation(const TValue &value_a, const TValue &value_b, const TValue &value_c) const;

public:
  enum RPN_OPERATION { OP_IF };

private:
  RPN_OPERATION m_operation;

public:
  // Construction ....
  CRpnTernaryOperation();
  CRpnTernaryOperation(CRpnStack &stack, RPN_OPERATION operation);
  virtual ~CRpnTernaryOperation();

  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
  virtual QString Formula() const;
  virtual void Clear(CRpnStack &stack);

  eObjectType ObjectType() const;
  virtual CRpnObject *Clone(CRpnStack &NewStack) const;

  // Save functions
  virtual void SaveStream(std::stringstream &stream);
  virtual void LoadStream(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack);
  virtual bool Recursive(TParentSet stParent = TParentSet()) const;

  virtual bool Defined() const { return m_pOperandA->Defined() && m_pOperandB->Defined() && m_pOperandC->Defined(); }
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet &stObjects) const;
  virtual CRpnObject *GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const;
};

} // namespace rpn

#endif // !defined(AFX_RPNTERNARYOPERATION_H__B4B97982_18E4_49EF_B447_0A54E62AC2F0__INCLUDED_)
