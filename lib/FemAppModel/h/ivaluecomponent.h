// ivaluecomponent.h: interface for the IValueComponentBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVALUECOMPONENT_H__D3CC45EC_DCF4_4D17_82A0_D9B261BF260E__INCLUDED_)
#define AFX_IVALUECOMPONENT_H__D3CC45EC_DCF4_4D17_82A0_D9B261BF260E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IValueComposite;
class COpenGLNode;

#include "IElement.h"
#include "IElementSet.h"
#include "ISymbol.h"
#include "IValueDataInterface.h"
#include "IValueDomain.h"
#include "SingleQuantity.h"
#include "StorageNode.h"

class CTensor;

namespace geo {

class IParallelInitializationCallback;

} // namespace geo

class IValueComponentBase : public CStorageNode {
protected:
  IValueComponentBase(IValueComposite &composite, unsigned int uComponent = 0, unsigned int uMode = 0);
  IValueComponentBase(unsigned int uName, IValueComposite &composite, unsigned int uComponent = 0,
                      unsigned int uMode = 0);
  IValueComponentBase(const QString &sName, IValueComposite &composite, unsigned int uComponent = 0,
                      unsigned int uMode = 0);

public:
  enum TValueComponentType : int { SCALAR = 0, VECTOR, TENSOR, OTHER };

  // Pre-defined return values
  typedef IQuantityDouble::UNIT UNIT;

  IValueComponentBase(const IValueComponentBase &rhs);

  virtual TValueComponentType Type() const = 0;

  // ScalarData is the default throughout the program, so (for now) we insist it is present
  virtual const IValueDataInterfaceScalar &ScalarData() const = 0;
  virtual IValueDataInterfaceScalar &ScalarData() = 0;

  virtual const IValueDataInterfaceVector *VectorData() const;
  virtual IValueDataInterfaceVector *VectorData();

  virtual const IValueDataInterfaceTensor *TensorData() const;
  virtual IValueDataInterfaceTensor *TensorData();

  // general interface to other data types
  virtual const IValueDataInterface *Data(int type = 0) const;
  virtual IValueDataInterface *Data(int type = 0);

  // Parent
  const IValueComposite &Parent() const;
  IValueComposite &Parent();

  // Indices of the component by the parent. These functions are search functions and are inefficient.
  int ComponentIndex() const;
  int ModeIndex() const;

  // Export label
  virtual QString ExportLabel() const = 0;

  // Unit description ...
  virtual QString UnitName(const UNIT unit = IQuantityDouble::SI_UNIT) const = 0;

  // Value discription ...
  virtual bool Defined() const = 0;
  int MinMaxSteps(const geo::IObject &object) const;

  // Can map on opengl node
  virtual bool CanMap(const COpenGLNode &node) const;
  virtual bool CanComputeOnPoints() const;

  // Component name
  IValueComponentBase &operator=(const IValueComponentBase &rhs);
  bool operator==(const IValueComponentBase &rhs) const;

  virtual bool Less(const CGraphNode &node) const;

  virtual bool NeedParallelInitializationCallback() const;
  virtual geo::IParallelInitializationCallback *
  GetParallelInitializationCallback(); // factory method, caller owns the pointer
  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
  virtual void FinishMapping();

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitIValueComponentBase(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitIValueComponentBase(*this); }
};

#endif // !defined(AFX_IVALUECOMPONENT_H__D3CC45EC_DCF4_4D17_82A0_D9B261BF260E__INCLUDED_)
