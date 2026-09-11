// rpnobject.h: interface for the CRpnObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNOBJECT_H__3ABACA8F_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_RPNOBJECT_H__3ABACA8F_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sstream>

#include "ivaluecomponent.h"

namespace rpn {
class CRpnStack;
class CRpnObject {
public:
  // Some important typedefs about using values
  typedef IValueDomainScalar::TValue TValue;
  typedef IValueDomainScalar::TValueVec TValueVec;
  typedef IValueComponentBase::UNIT UNIT;

  // NOTE: This enumeration is used for saving and loading purposes. Don't remove
  //		 or change order. You can only add operations.
  enum eObjectType { OT_CONSTANT, OT_UNARY, OT_BINARY, OT_TERNARY, OT_OPERAND, OT_RPN_CONSTANT, OT_UNITCONVERTER };

  // Construction / Destruction
  CRpnObject();
  virtual ~CRpnObject();

  // POST : Copied object should be deleted by caller
  virtual CRpnObject *Clone(CRpnStack &NewStack) const = 0;

  // Value
  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const = 0;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const = 0;
  virtual QString Formula() const = 0;
  virtual void Clear(CRpnStack &stack) = 0;

  // Save functions
  virtual void SaveStream(std::stringstream &stream) = 0;
  virtual void LoadStream(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack) = 0;
  virtual eObjectType ObjectType() const = 0;
  CRpnObject *LoadRpnObject(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack);
  typedef std::set<const CRpnObject *> TParentSet;
  virtual bool Recursive(TParentSet stParent = TParentSet()) const = 0;

  virtual bool Defined() const = 0;
  virtual bool exists() const = 0;

  typedef TParentSet TObjectSet;
  virtual void CollectUsedObjects(TObjectSet &stObjects) const = 0;

  virtual CRpnObject *GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const = 0;
};

} // namespace rpn

#endif // !defined(AFX_RPNOBJECT_H__3ABACA8F_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
