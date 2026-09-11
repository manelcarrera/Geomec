#ifndef _Delegate_h_
#define _Delegate_h_

class CGraphNode;

#include <map>
#include <string>

#include <assert.h>

#include "GraphNode.h"

class CDelegate
{
public:
  // make sure all delegates are polymorphic

  virtual ~CDelegate();

  // instances of this method are registered in the factory for creation of
  // specific delegate types

  template <class NODE_TYPE, class DELEGATE_TYPE>
    static CDelegate* Create(CGraphNode* node)
  {
    assert(dynamic_cast <NODE_TYPE*> (node));

    NODE_TYPE* pNode;

    try
    {
      pNode = static_cast <NODE_TYPE*> (node);
    }

    catch (std::bad_cast&)
    {
      assert(false);  // NODE_TYPE is not derived from CGraphNode
      throw;
    }
    return new DELEGATE_TYPE(pNode);
  }

protected:
  CDelegate();

public:
  template <class NODE_TYPE, class DELEGATE_TYPE>
    class CDelegateTypeInfo
  {
  public:
    CDelegateTypeInfo()
    {
      // initialization of the static variable appears to be necessary,
      // otherwise the variable is not instantiated and the type info not
      // registered in the factory...

      NodeTypeName = std::string();
    }

  public:
    static std::string NodeTypeName;
  };

private:
  CDelegate(const CDelegate& rhs);
  CDelegate& operator = (const CDelegate& rhs);
};

// instantiation of the static CDelegate::CDelegateTypeInfo::NodeTypeName
// variable. This causes the node typename and CDelegate::Create method to be
// registered in the factory

template <class NODE_TYPE, class DELEGATE_TYPE>
  std::string CDelegate::CDelegateTypeInfo <NODE_TYPE, DELEGATE_TYPE> ::
    NodeTypeName = CGraphNode::Register(typeid(NODE_TYPE),
      &CDelegate::Create <NODE_TYPE, DELEGATE_TYPE>);

/*!
 * Use this macro for every delegate type so it will be registered in the
 * factory.
 * Must be put in the private section of the delegate class
 */
#define REGISTER_DELEGATE(node_type, delegate_type)                           \
  class CMyDelegateTypeInfo : public CDelegate::CDelegateTypeInfo <node_type, \
    delegate_type>                                                            \
  {                                                                           \
    CMyDelegateTypeInfo(){}                                                   \
  }

/*!
 * For templates the solution requires another action. The relevant
 * documentation is provided in the ISO/IEC C++ 2003 standard, 14.7.1:
 * Unless a member of a class template or a member template has been explicitly
 * instantiated or explicitly specialized, the specialization of the member is
 * implicitly instantiated when the specialization is referenced in a context
 * that requires the member definition to exist; in particular, the
 * initialization (and any associated side-effects) of a static data member
 * does not occur unless the static data member is itself used in a way that
 * requires the definition of the static data member to exist.
 *
 * Use this macro for every template delegate type so it will be registered in
 * the factory.
 * Must be put in the constructor of the delegate class or in the constructor
 * of selected classes when necessary.
 */
#define ACTIVATE_TEMPLATE_DELEGATE(node_type, delegate_type)      \
  if (CDelegate::CDelegateTypeInfo <node_type, delegate_type > :: \
    NodeTypeName == "")                                           \
  {                                                               \
    throw("Template not registered");                             \
  }

#endif // _Delegate_h_
