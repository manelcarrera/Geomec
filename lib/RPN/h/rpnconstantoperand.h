// ConstantOperand.h: interface for the CConstantOperand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTANTOPERAND_H__3ABACA95_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_CONSTANTOPERAND_H__3ABACA95_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnstack.h"
namespace rpn {

class CRpnConstantOperand : public CRpnObject {
  friend class CRpnStack;
  QString m_sNumber;
  CRpnConstantOperand(const CRpnConstantOperand &rhs);

public:
  CRpnConstantOperand();
  virtual ~CRpnConstantOperand();

  void Zero();
  void One();
  void Two();
  void Three();
  void Four();
  void Five();
  void Six();
  void Seven();
  void Eight();
  void Nine();
  void Point();
  void Sign();
  void Scn();

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

  virtual bool Defined() const { return true; }
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet & /*stObjects*/) const {}
  virtual CRpnObject *GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const;
};

} // namespace rpn

#endif // !defined(AFX_CONSTANTOPERAND_H__3ABACA95_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
