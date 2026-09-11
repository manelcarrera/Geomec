 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ModelObjectObserver.cpp: implementation of the CModelObjectObserver class.
//
//////////////////////////////////////////////////////////////////////
#include <cassert>
#include <set>
#include <QObject>
#include "IModelObject.h"
#include "ModelObjectObserver.h"
#include "DocumentBase.h"
#include "dimple.h"
#include "ICommandInvoker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelObjectObserver::CModelObjectObserver(IModelObject& observed_object, CModelObjectObserver* parent)
: QObject(parent),
  m_stateChanged(false),
  m_displayChanged(false),
  m_propertyChanged(false),
  m_geometryChanged(false),
  m_parameterChanged(false),
  m_childOrderChanged(false),
  m_childInserted(false),	
  m_childRemoved(false),
  m_referenceInserted(false),
  m_referenceRemoved(false),
  m_observed_object(observed_object),
  m_select(false) 
{
	connect(&observed_object, SIGNAL(onStateChanged()), this, SLOT(stateChanged()));
	connect(&observed_object, SIGNAL(onDisplayChanged(IModelObject&)), this, SLOT(displayChanged()));
	connect(&observed_object, SIGNAL(onPropertyChanged()), this, SLOT(propertyChanged()));
	connect(&observed_object, SIGNAL(onGeometryChanged()), this, SLOT(geometryChanged()));
	connect(&observed_object, SIGNAL(onParameterChanged()), this, SLOT(parameterChanged()));
	connect(&observed_object, SIGNAL(onChildOrderChanged()), this, SLOT(childOrderChanged()));
	connect(&observed_object, SIGNAL(onChildInserted(IModelObject&, IModelObject&)), this, SLOT(childInserted()));
	connect(&observed_object, SIGNAL(onChildRemoved(IModelObject&, IModelObject&)), this, SLOT(childRemoved()));
	connect(&observed_object, SIGNAL(onReferenceInserted()), this, SLOT(referenceInserted()));
	connect(&observed_object, SIGNAL(onReferenceRemoved()), this, SLOT(referenceRemoved()));
	connect(&observed_object, SIGNAL(onDestroyed()), this, SLOT(object_destroyed()));
	connect(&observed_object, SIGNAL(onError(const QString&)), this, SLOT(error(const QString&)));
        connect(&observed_object, SIGNAL(onWarning(const QString&)), this, SLOT(warning(const QString&)));
	if ( observed_object.document() ) {
		connect(observed_object.document(), SIGNAL(onEndOperation()), this, SLOT(applyChangeSignals()));
	}
}

CModelObjectObserver::~CModelObjectObserver()
{

}


void CModelObjectObserver::applyChangeSignals()
{
	// Apply sigal in fixed order
	if(m_childInserted)
		onChildInserted();
	if(m_childRemoved)
		onChildRemoved();
	if(m_childOrderChanged)
		onChildOrderChanged();
	if(m_referenceInserted)
		onReferenceInserted();
	if(m_referenceRemoved)
		onReferenceRemoved();
	if(m_stateChanged)
		onStateChanged();
	if(m_displayChanged)
		onDisplayChanged();
	if(m_geometryChanged)
		onGeometryChanged();
	if(m_parameterChanged)
		onParameterChanged();
	if( !m_errors.empty() )
		onError( m_errors );
	if( !m_warnings.empty() )
		onWarning( m_warnings );

		// Reset all the flags
	m_stateChanged = false;
	m_displayChanged = false;
	m_propertyChanged = false;
	m_geometryChanged = false;
	m_parameterChanged = false;
	m_childOrderChanged = false;
	m_childInserted = false;	
	m_childRemoved = false;
	m_referenceInserted = false;
	m_referenceRemoved = false;
	m_errors.clear();
	m_warnings.clear();
}

void CModelObjectObserver::stateChanged()
{
	m_stateChanged = true;
}

void CModelObjectObserver::displayChanged()
{
	m_displayChanged = true;
}

void CModelObjectObserver::propertyChanged()
{
	m_propertyChanged = true;
}

void CModelObjectObserver::geometryChanged()
{
	m_geometryChanged = true;
}

void CModelObjectObserver::parameterChanged()
{
	m_parameterChanged = true;
}

void CModelObjectObserver::childOrderChanged()
{
	m_childOrderChanged = true;
}

void CModelObjectObserver::childInserted()
{
	m_childInserted = true;
}

void CModelObjectObserver::childRemoved()
{
	m_childRemoved = true;
}

void CModelObjectObserver::referenceInserted()
{
	m_referenceInserted = true;
}

void CModelObjectObserver::referenceRemoved()
{
	m_referenceRemoved = true;
}

void CModelObjectObserver::error( const QString& description )
{
	m_errors.append( description );
}

void  CModelObjectObserver::warning( const QString& description )
{
	m_warnings.append( description );
}

void CModelObjectObserver::object_destroyed()
{
	onDestroyed();
	delete this;
}

void CModelObjectObserver::onStateChanged()
{
}

void CModelObjectObserver::onDisplayChanged()
{
}

void CModelObjectObserver::onPropertyChanged()
{
}

void CModelObjectObserver::onGeometryChanged()
{
}

void CModelObjectObserver::onParameterChanged()
{
}

void CModelObjectObserver::onChildOrderChanged()
{
}

void CModelObjectObserver::onChildInserted()
{
	updateChildren();
}

void CModelObjectObserver::onChildRemoved()
{
	updateChildren();
}

void CModelObjectObserver::onReferenceInserted()
{
	updateChildren();
}

void CModelObjectObserver::onReferenceRemoved()
{
	updateChildren();
}

void CModelObjectObserver::onDestroyed()
{
}

void CModelObjectObserver::onError( const QStringList& /*descriptions*/ )
{
}

void CModelObjectObserver::onWarning( const QStringList& /*descriptions*/ )
{
}

/*!
	The observed object got a new child. If it needs observation, override
	this function to create and return an observer.
	Return 0 to ignore a child.
	\sa updateChildren
*/
CModelObjectObserver* CModelObjectObserver::insertChild(IModelObject& /*object*/)
{
	return 0;
}
 
int CModelObjectObserver::childSize() const
{
	return observedObject().childSize();
}

IModelObject& CModelObjectObserver::childAt(int nIndex)
{
	return observedObject().childAt(nIndex);
}

bool CModelObjectObserver::isChild(const CModelObjectObserver& observer, bool in_whole_tree /*= true*/) const
{
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(pObserver == &observer)
			return true;
		else if(in_whole_tree)
		{
			if(pObserver->isChild(observer,true))
				return true;
		}
	}
	return false;

}

/*!
	The children of the observed object can be observed by the children
	of this observer. Observers of removed children will be removed.
	New children may get a child observer; this is handled in insertChild.
	\sa insertChild
*/
void CModelObjectObserver::updateChildren()
{
	// Make a set of inserted and to be inserted item
	typedef std::set<IModelObject*> TChildSet;
	
	TChildSet stChild;
	int nChildSize = childSize();
	int i;
	for(i = 0; i < nChildSize; i++)
		stChild.insert(&childAt(i));

	// Delete the things we don't have anymore
	QObjectList object_list = children();
	for(QObjectList::const_iterator it = object_list.begin(); it != object_list.end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(stChild.erase(&pObserver->observedObject()) == 0)
			delete pObserver;
	}

	for(TChildSet::iterator it = stChild.begin(); it != stChild.end(); it++)
	{
		CModelObjectObserver* childObs = insertChild(**it);

		// we rely on child observers being our children to
		// identify them, above. Enforce this here so we don't have
		// to deal with it during construction/initialization
		// elsewhere.
		if ( childObs ) childObs->setParent( this );
	}
}


CModelObjectObserver& CModelObjectObserver::Root() const
{
	CModelObjectObserver* ret = const_cast<CModelObjectObserver*>(this);
	while(ret->parent())
		ret=(CModelObjectObserver*)ret->parent();

	return *ret;
}

bool CModelObjectObserver::deSelectObject(IModelObject& object)
{
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(&pObserver->observedObject() == &object)
		{
      pObserver->deSelect();
			return true;
		}
		else
		{
      if(pObserver->deSelectObject(object))
				return true;
		}
	}
	return false;

}



bool CModelObjectObserver::selectObject(IModelObject& object)
{
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(&pObserver->observedObject() == &object)
		{
			pObserver->select();
			return true;
		}
		else
		{
			if(pObserver->selectObject(object))
				return true;
		}
	}
	return false;
}

void CModelObjectObserver::selectChildren(CModelObjectObserver* exept /*=0*/,bool grand_children /*=true*/)
{
	assert(exept != this);
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(pObserver != exept && !pObserver->isSelected())
		{
			pObserver->select();
			if(grand_children)
				pObserver->selectChildren(exept,grand_children);
		}
	}
}

void CModelObjectObserver::deSelectChildren(CModelObjectObserver* exept /*=0*/,bool grand_children /*=true*/)
{
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(pObserver != exept && pObserver->isSelected())
		{
      pObserver->deSelect();
			if(grand_children)
        pObserver->deSelectChildren(exept,grand_children);
		}
	}
}

CModelObjectObserver* CModelObjectObserver::getObserver(IModelObject& object)
{
	CModelObjectObserver* ret=0;
	for(QObjectList::const_iterator it = children().begin(); it != children().end(); it++)
	{
		CModelObjectObserver* pObserver = (CModelObjectObserver*)(*it);
		if(&pObserver->observedObject() == &object)
			return pObserver;
		else
			ret = pObserver->getObserver(object);

		if(ret)
			return ret;
	}
	return ret;
}

void CModelObjectObserver::appendCommands( ICommandInvoker& mnu )
{
  mnu.append( new TModelObjectObserverCommand( tr( "Delete" ), *this, &CModelObjectObserver::destroy, &CModelObjectObserver::canDestroy ) );
  mnu.appendSeparator();
  mnu.append( new TModelObjectObserverCommand( tr( "Properties..." ), *this, &CModelObjectObserver::showProperties, &CModelObjectObserver::canShowProperties ) );
}

void CModelObjectObserver::destroy()
{
  observedObject().destroy();
}

bool CModelObjectObserver::canDestroy() const
{
  return observedObject().canDestroy();
}

bool CModelObjectObserver::showProperties()
{
	assert(false);
	return false;
}

bool CModelObjectObserver::canShowProperties() const
{
	return false;
}

/*virtual*/ void CModelObjectObserver::select()
{
	m_select=true;
}

/*virtual*/ void CModelObjectObserver::deSelect()
{
	m_select=false; 
}

/*virtual*/ bool CModelObjectObserver::isSelected() const
{
	return m_select;
}


