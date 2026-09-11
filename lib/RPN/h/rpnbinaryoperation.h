// rpnbinaryoperation.h: interface for the CRpnBinaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNBINARYOPERATION_H__3ABACA93_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_RPNBINARYOPERATION_H__3ABACA93_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnstack.h"

namespace rpn {

class CRpnBinaryOperation : public CRpnObject {
  CRpnObject *m_pOperandA;
  CRpnObject *m_pOperandB;

public:
  // NOTE: This enumeration is used for saving and loading purposes. Don't remove
  //		 or change order. You can only add operations.
  enum RPN_OPERATION {
    OP_ADD = 0,
    OP_SUB,
    OP_DIV,
    OP_MUL,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_MIN,
    OP_MAX,
    OP_POW,
    OP_OR,
    OP_AND
  };

private:
  RPN_OPERATION m_operation;
  QString Operand(const CRpnObject &object) const;
  TValue Operation(const TValue &value_a, const TValue &value_b) const;
  QString CurrentOperator() const;

public:
  CRpnBinaryOperation();
  CRpnBinaryOperation(CRpnStack &stack, enum RPN_OPERATION operation);
  virtual ~CRpnBinaryOperation();

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

  virtual bool Defined() const { return m_pOperandA->Defined() && m_pOperandB->Defined(); }
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet &stObjects) const;
  virtual CRpnObject *GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const;
};

} // namespace rpn

#endif // !defined(AFX_RPNBINARYOPERATION_H__3ABACA93_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
