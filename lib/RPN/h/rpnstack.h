// rpnstack.h: interface for the CRpnStack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNSTACK_H__3ABACA91_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_RPNSTACK_H__3ABACA91_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphPtr.h"
#include "RpnOperand.h"
#include "StreamVersion.h"
#include "rpnobject.h"
#include <vector>

namespace rpn {

class CRpnConstantOperand;

class CRpnVersion : public CStreamVersion {
public:
  CRpnVersion() {}
  CRpnVersion(int nMajor, int nMinor, int nRevision) : CStreamVersion(nMajor, nMinor, nRevision) {}
  CRpnVersion(int nMajor, int nMinor) : CStreamVersion(nMajor, nMinor) {}
};

class CRpnStack : public CRpnObject {
public:
  // The coordinate proxy allows operand representing the coordinates in the formula.
  class CCoordinateProxy : public CRpnOperand::IValueProxy {
  public:
    enum COORDINATE { CO_X, CO_Y, CO_Z };

  protected:
    enum COORDINATE m_coor;
    QString m_sName;

  public:
    CCoordinateProxy(unsigned int uName, CRpnStack &stack, COORDINATE coor, const QString &sProxyId);
    CCoordinateProxy(unsigned int uName, CRpnStack &stack, COORDINATE coor);
    CCoordinateProxy(const QString &sName, CRpnStack &stack, COORDINATE coor, const QString &sProxyId);
    CCoordinateProxy(CRpnStack &stack, const QString &sProxyId); // Constructor for loading
    virtual QString TextTag() const;
    virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
    virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
    COORDINATE Coordinate() const { return m_coor; }
    virtual void SaveStream(std::stringstream &stream);
    virtual void LoadStream(std::stringstream &stream, CStreamVersion &version);
    virtual bool Defined() const { return true; }
  };

  class CValueComponentProxy : public CRpnOperand::IValueProxy {
    typedef CGraphPtr<IValueComponentBase> TComponent;
    TComponent m_component;

  public:
    CValueComponentProxy(IValueComponentBase &component);
    bool Valid() const;
    virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
    virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
  };

  // External operands
  typedef std::set<CRpnOperand *> TOperandSet;
  typedef std::pair<CRpnOperand::IValueProxy *, TOperandSet> TProxy;
  typedef std::map<QString, TProxy> TProxyMap;
  TProxyMap m_mpProxy;

public:
  std::vector<CRpnObject *> m_vcRpnStack;
  CRpnConstantOperand *m_pConstant;

public:
  // Construction
  CRpnStack();
  CRpnStack(const CRpnStack &rhs);
  virtual ~CRpnStack();

  // Stack handling
  int StackSize() const;
  const CRpnObject &Top() const;
  CRpnObject &Top();
  CRpnObject &Pop();
  void Push(CRpnObject &object);
  const CRpnObject &StackAt(int nIndex) const;
  void Swap(int nIndex1, int nIndex2);
  virtual void Expand(std::list<std::string> &lstMessages);
  virtual CRpnObject *GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const;

  // Value ...
  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;

  virtual QString Formula() const;
  virtual void Clear(CRpnStack &stack);
  virtual CRpnObject *Clone(CRpnStack &NewStack) const;

  // Save functions
  virtual void SaveStream(std::stringstream &stream);
  virtual void LoadStream(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack);
  virtual eObjectType ObjectType() const;

  // External operands and value proxy
  CRpnOperand::IValueProxy *AttachOperand(CRpnOperand &operand, const QString &sProxyId);
  void DetachOperand(CRpnOperand &operand);
  QString AttachValueProxy(CRpnOperand::IValueProxy &proxy, const QString &sProxyId);
  QString AttachValueProxy(CRpnOperand::IValueProxy &proxy);
  void DetachValueProxy(CRpnOperand::IValueProxy &proxy);
  CRpnOperand::IValueProxy *ValueProxy(const QString &sProxyId);
  typedef std::vector<CRpnOperand::IValueProxy *> TValueProxyVec;
  TValueProxyVec ValueProxies() const;
  void RemoveAllValueProxy();
  CFemAppModel *FindModel(CFemAppModel &model);
  void AttachToModel(CFemAppModel &model);
  virtual bool Recursive(TParentSet stParent = TParentSet()) const;
  std::vector<QString> UnAssignedOperands() const;
  void SetUnassignedOperand(const QString &sOperand, CRpnOperand::IValueProxy &proxy);

  bool Defined() const;
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet &stObjects) const;

  // Constants
  bool IsConstant() const;
  CRpnConstantOperand &Constant();
  void Enter();
};

} // namespace rpn

#endif // !defined(AFX_RPNSTACK_H__3ABACA91_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
