// RpnObject.cpp: implementation of the CRpnObject class.
//
//////////////////////////////////////////////////////////////////////

#include "rpnobject.h"
#include "RpnConstant.h"
#include "RpnTernaryOperation.h"
#include "RpnUnitConverter.h"
#include "rpnbinaryoperation.h"
#include "rpnconstantoperand.h"
#include "rpnunaryoperation.h"

namespace rpn {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRpnObject::CRpnObject() {}

CRpnObject::~CRpnObject() {}

CRpnObject::TValueVec CRpnObject::Value(const geo::IElement &el, UNIT unit) const {
  TValueVec vcRet(el.NrOfPoints());
  for (int i = 0; i < el.NrOfPoints(); i++) {
    vcRet[i] = Value(el.Point(i), unit);
  }

  return vcRet;
}

CRpnObject *CRpnObject::LoadRpnObject(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack) {
  CRpnObject *pRet = 0;
  int nType = -1;
  stream >> nType;
  switch (nType) {
  case OT_CONSTANT:
    pRet = new CRpnConstantOperand();
    break;
  case OT_UNARY:
    pRet = new CRpnUnaryOperation();
    break;
  case OT_BINARY:
    pRet = new CRpnBinaryOperation();
    break;
  case OT_TERNARY:
    pRet = new CRpnTernaryOperation();
    break;
  case OT_OPERAND:
    pRet = new CRpnOperand();
    break;
  case OT_RPN_CONSTANT:
    pRet = new CRpnConstant();
    break;
  case OT_UNITCONVERTER:
    pRet = new CRpnUnitConverter();
    break;
  }

  assert(pRet);
  pRet->LoadStream(stream, version, stack);
  return pRet;
}

void CRpnObject::SaveStream(std::stringstream &stream) {
  int nType = ObjectType();
  stream << nType << " ";
}

void CRpnObject::LoadStream(std::stringstream & /*stream*/, CStreamVersion & /*version*/, CRpnStack & /*stack*/) {}

} // namespace rpn
