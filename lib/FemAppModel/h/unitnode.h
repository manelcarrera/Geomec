// unitnode.h: interface for the CUnitNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITNODE_H__AF6313F3_E7A3_4B38_89D0_57E68AB53537__INCLUDED_)
#define AFX_UNITNODE_H__AF6313F3_E7A3_4B38_89D0_57E68AB53537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphNode.h"
#include "SingleQuantity.h"

class CUnitNode : public CGraphNode {
  IQuantityDouble::UNIT m_unit;

protected:
  // Unit settings are store under "Settings", "UNIT"
  virtual void ReadRegister();  // Read unit settings in register
  virtual void StoreRegister(); // Store unit settings in register
public:
  typedef IQuantityDouble::UNIT TUnitType;
  CUnitNode(TUnitType unit = IQuantityDouble::SI_UNIT);

  TUnitType Unit() const;
  void Unit(TUnitType unit);

  QString UnitName(TUnitType unit) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitUnitNode(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitUnitNode(*this); }
};

#endif // !defined(AFX_UNITNODE_H__AF6313F3_E7A3_4B38_89D0_57E68AB53537__INCLUDED_)
