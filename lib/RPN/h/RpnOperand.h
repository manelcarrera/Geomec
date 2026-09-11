// RpnOperand.h: interface for the CRpnOperand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNOPERAND_H__C35D2585_112A_4AAC_8CA3_96CFC0F9286E__INCLUDED_)
#define AFX_RPNOPERAND_H__C35D2585_112A_4AAC_8CA3_96CFC0F9286E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rpnobject.h"

namespace rpn {

class CRpnStack;
///
/// The RpnOperand class allows insertion of external value sources
/// in the rpn formula. The external
/// value sources should be derived from IValueProxy. The proxies
/// should supply a TextTag. This text
/// tag is used saving and loading purposes. Before the loading
/// procedure the IValueProxy should be
/// available and the load procedure searches for the tag in the stack. 
///
class CRpnOperand : public CRpnObject 
{
public:
  class IValueProxy 
  {
    friend class CRpnOperand;
    friend class CRpnStack;
    CRpnStack* m_pStack;
    QString m_sProxyId;
  protected:
    void SaveString(const QString& string, std::stringstream& stream);
    QString LoadFromString(std::stringstream& stream);
  CRpnStack *Stack() const;
    virtual int TypeId() const = 0;	///< Used by the less function in the dialog
  public:
    IValueProxy(CRpnStack& stack);
    IValueProxy(CRpnStack&, const QString& sProxyId);
    virtual ~IValueProxy();
    virtual IValueProxy* Clone(CRpnStack& NewStack) const = 0;
    virtual QString TextTag() const = 0;
    virtual TValue Value(const geo::IPoint &pt, UNIT unit) const = 0;
    virtual TValueVec Value(const geo::IElement &el, UNIT unit) const = 0;
    virtual unsigned int IconId() const = 0;
  virtual CFemAppModel *FindModel(CFemAppModel& model);
    virtual void AttachToModel(CFemAppModel& model);
    const QString& ProxyId() const;
    virtual void SaveStream(std::stringstream& stream) = 0;
    virtual void LoadStream(std::stringstream& stream, CStreamVersion& version) = 0;
    virtual bool HasProperties() const;
    virtual bool Properties();
    virtual bool CanDelete() const;
    virtual void Delete();
    bool operator<(const IValueProxy& rhs) const;
    typedef std::set<const CRpnObject*> TParentSet;
    virtual bool Recursive(TParentSet stParent) const = 0;
    virtual bool showInDialog() const { return true;}
  virtual bool Defined() const = 0;
  virtual bool exists() const = 0;
  virtual CRpnObject *GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const;
  };
private:
  IValueProxy* m_pProxy;
  CRpnStack* m_pStack;
  std::string m_sDescription;
public:
  CRpnOperand();
  CRpnOperand(CRpnStack& stack, IValueProxy& operand);
  virtual ~CRpnOperand();

  // Value
  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
  virtual QString Formula() const;
  virtual void Clear(CRpnStack &stack);
  virtual CRpnObject* Clone(CRpnStack& NewStack) const;
  virtual eObjectType ObjectType() const;
  void ClearProxy();
  void Proxy(IValueProxy& proxy);
  const IValueProxy* Proxy() const;

  // Save functions
  virtual void SaveStream(std::stringstream& stream);
  virtual void LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack);
  virtual bool Recursive(TParentSet stParent = TParentSet()) const;

  virtual bool Defined() const;
  virtual bool exists() const;
  virtual void CollectUsedObjects(TObjectSet& /*stObjects*/) const {}
  virtual CRpnObject* GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const;
};

}

#endif // !defined(AFX_RPNOPERAND_H__C35D2585_112A_4AAC_8CA3_96CFC0F9286E__INCLUDED_)
