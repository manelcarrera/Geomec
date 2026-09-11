// GraphNode.h: interface for the CGraphNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHNODE_H__88FEC093_9A89_4B0F_9F93_583579727099__INCLUDED_)
#define AFX_GRAPHNODE_H__88FEC093_9A89_4B0F_9F93_583579727099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CContextMenuInvoker;
class CDelegate;
class CMenu;

#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "ICommand.h"
#include "TypeInfo.h"
#include "ModifiedHint.h"

#include "GraphNodeVisitor.h"

//The GeoItem is an abstact class called by the view.
//Abstration of the DAG.
//
//RTTI must be enabled this is a specific c++ dependancy
//
//A GeoItem cannot be derived from two GeoItems (cq. iostream 
//library ios, istream, ostream, iostream.) Heirarchically 
//this means GeoItems can be considered as a tree.
//
//DAG Rules for every node:
// - need 'connect to' operation that may fail
// - need 'disconnect from' operation. This never fails. No 
//links means destroy self.
// - need 'destroy' operation. When a GeoItem is destroyed 
//all neighbours _must_ be informed of this. The neighbours 
//of the  GeoItem _must_ at least remove the reference.
//
//Construction a name and a link. This must be enforced by 
//derived classes.
//##ModelId=3B653D0101C7
class CGraphNode
{
public:	// Public classes and typedefs ...

	// Less operator compares on pointers. This operator is also used for the
	// neighbour set of the graph node.
	class CPtrLess
	{
		public:
		template<class T>
		bool operator()(const T *pItem1, const T *pItem2) const
		{	
			return pItem1 < pItem2;		
		}
	};

	// Less operator uses the customised less function of the graphnode it selves. 
	class CLess
	{
		public:
		template<class T>
		bool operator()(const T *pItem1, const T *pItem2) const
		{	
			return pItem1->Less(*pItem2);
		}
	};

	typedef std::set<CGraphNode*, CPtrLess> TPtrSet;
	typedef TPtrSet::iterator iterator;
	typedef TPtrSet::const_iterator const_iterator;
	typedef std::vector<CGraphNode*> TNodeVec;

  // Status for delayed handling (eg, in OIV); for ease we use <= 1 as the "current set"
  enum eHandleStatus
  {
    NO_ACTION     = 0,
    TO_BE_DELETED = 1,
    FORCE_DELETE,
    TO_BE_ADDED
  };

private:
	// If we have a copy ...
	CGraphNode* m_pSource;
	mutable CGraphNode* m_pCopy;
private:
	QString m_strName;
	TNodeVec m_vcChild;
	TNodeVec m_vcReference;

  mutable enum eHandleStatus m_eStatus;

	CGraphNode();
protected:
	// Deratives only ....
	CGraphNode(const QString& strInstanceName);
	CGraphNode(unsigned int uInstanceName);

	// These functions only called by the framework and by upper classes.
	// They can only overided for extra functionality
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &node);

	// Handle with care !!! 
	virtual void UnLinkAll();

 
  // Copy constructor and assignment
	CGraphNode(const CGraphNode &rhs);

	// IModelObject emulation
	CGraphNode* m_parent;
public:	
	// Functionality for copying nodes
	virtual bool operator==(const CGraphNode& node) const;
	CGraphNode& operator=(const CGraphNode &rhs);

	// Copy mode ...
	bool IsCopied() const;
	bool IsCopy() const;

	// Access to source and copy ...
	const CGraphNode& Source() const;
	CGraphNode& Source();
	const CGraphNode& Copy() const;
	CGraphNode& Copy();

	// Functions for linkage ..
	void UnLink(CGraphNode &rhs);
    void LinkTo(CGraphNode &rhs);
	bool IsLinkedTo(const CGraphNode &rhs) const;	

  enum eHandleStatus HandleStatus() const;
  void HandleStatus(enum eHandleStatus status);
	
  template <class TYPE>
  std::set<TYPE*> Links() const;

	std::size_t childSize() const;
  const CGraphNode& childAt(size_t nIndex) const;
  CGraphNode& childAt(size_t nIndex);
	int isChild( const CGraphNode& node ) const;

	std::size_t referenceSize() const;
  const CGraphNode& referenceAt(size_t nIndex) const;
  CGraphNode& referenceAt(size_t nIndex);
	int isReferenced( const CGraphNode& node ) const;
	
	// Construction / Destruction ...

	virtual ~CGraphNode();

	virtual void Modified(enum ModifiedHint uHint = Default);


	// Identification
	virtual const QString &Name() const;
	virtual void Name(const QString &strName);
  virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;

	// Function to support drag and drop ...
	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;
  virtual void OnDragLeave(const CGraphNode& item) const;

	// Functions to delete an item ...
	virtual bool Destroy();
	virtual bool CanDestroy() const;
	virtual bool Less(const CGraphNode &node) const;
	virtual void OnDeselect(CGraphNode* NewSeleceted);
	
	virtual void AssertValid() const;
// IModelObject emulation
	CGraphNode* parent() const { return m_parent; }
	void create( CGraphNode* new_parent );
	void reParent(CGraphNode* new_parent);

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitGraphNode(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitGraphNode(*this); }

private:
  class CDelegateFactory
  {
  public:
    typedef CDelegate* (*TFactory) (CGraphNode*);

    static CDelegateFactory* getInstance();

    std::string Register(const std::type_info& tinfo, TFactory createMethod);
    CDelegate* Create(CGraphNode* node);

  private:
    typedef std::map <TypeInfo, TFactory> TFactoryMap;
    TFactoryMap m_mpFactory;
  };

  CDelegate* m_delegate;

protected:
  bool m_modifiedByAttributesDialog;

public:
  static std::string Register(const std::type_info& tinfo,
    CDelegateFactory::TFactory createMethod);

  CDelegate* getDelegate();
};

typedef CGraphNode::TPtrSet TGraphNodeSet;
typedef CSingleCommandTemplate<CGraphNode> TGraphNodeCommand;

template <class TYPE>
std::set<TYPE*> CGraphNode::Links() const
{
  std::set<TYPE*> stRet;

  TNodeVec::const_iterator it;
  for(it = m_vcChild.begin(); it != m_vcChild.end(); ++it)
  {
    TYPE* t = dynamic_cast<TYPE*>(*it);
    if(t)
      stRet.insert(t);
  }

  for(it = m_vcReference.begin(); it != m_vcReference.end(); ++it)
  {
    TYPE* t = dynamic_cast<TYPE*>(*it);
    if(t)
      stRet.insert(t);
  }

  TYPE* t = dynamic_cast<TYPE*>(m_parent);
  if(t)
    stRet.insert(t);

  return stRet;
}




#endif // !defined(AFX_GRAPHNODE_H__88FEC093_9A89_4B0F_9F93_583579727099__INCLUDED_)
