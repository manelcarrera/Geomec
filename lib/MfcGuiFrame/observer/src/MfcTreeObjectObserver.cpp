// MfcTreeObjectObserver.cpp: implementation of the CMfcTreeObjectObserver class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IModelObject.h"
#include "TreeObject.h"
#include "ICommand.h"
#include "ContextMenuInvoker.h"
#include "MfcTreeObjectObserver.h"
#include <qcolor.h>
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMfcTreeObjectObserver::CMfcTreeObjectObserver(IModelObject& observed_object, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
: CModelObjectObserver(observed_object)
{
	m_pTreeObject = new CTreeObject(*this, ctrl, hParent, hInsertAfter);
}

CMfcTreeObjectObserver::CMfcTreeObjectObserver(IModelObject& observed_object, ITreeObject &parent, HTREEITEM hInsertAfter)
: CModelObjectObserver(observed_object)
{
	m_pTreeObject = new CTreeObject(*this, parent, hInsertAfter);
}

CMfcTreeObjectObserver::CMfcTreeObjectObserver(IModelObject& observed_object, CMfcTreeObjectObserver& observer)
: CModelObjectObserver(observed_object, &observer), m_pTreeObject(0)
{

	m_pTreeObject = new CTreeObject(*this, *observer.m_pTreeObject, TVI_LAST);
	observer.m_pTreeObject->SortChildren();
}

CMfcTreeObjectObserver::~CMfcTreeObjectObserver()
{
	delete m_pTreeObject;
}

unsigned int CMfcTreeObjectObserver::onStateIconId() const
{
	return 0;
}

CString CMfcTreeObjectObserver::onName() const
{
	return observedObject().text().toStdString().c_str();
}

void CMfcTreeObjectObserver::appendCommands(ICommandInvoker &invoker)
{
	typedef CGuiCommand<CMfcTreeObjectObserver> TObserverCommand;
	invoker.append(new TObserverCommand("Delete", *this, &CMfcTreeObjectObserver::destroy, &CMfcTreeObjectObserver::canDestroy));
	invoker.appendSeparator();
	invoker.append(new TObserverCommand("Properties", *this, &CModelObjectObserver::showProperties, &CModelObjectObserver::canShowProperties));
}

bool CMfcTreeObjectObserver::canDestroy() const
{
	return observedObject().canDestroy();
}

void CMfcTreeObjectObserver::destroy()
{
	observedObject().destroy();
}

HTREEITEM CMfcTreeObjectObserver::Handle() const
{
	return m_pTreeObject->Handle();
}

void CMfcTreeObjectObserver::onExpand()
{

}

void CMfcTreeObjectObserver::onCollapse()
{

}

void CMfcTreeObjectObserver::updateTreeObject()
{
	m_pTreeObject->Update();
}

void CMfcTreeObjectObserver::select()
{
	m_pTreeObject->Ctrl().SelectItem(m_pTreeObject->Handle());
}

void CMfcTreeObjectObserver::unSelect()
{

}

bool CMfcTreeObjectObserver::isSelected()
{
	return m_pTreeObject->Ctrl().GetSelectedItem() == m_pTreeObject->Handle();
}

CMfcTreeObjectObserver::TColor CMfcTreeObjectObserver::color()
{
	return qRgb(0, 0, 0);
}

bool CMfcTreeObjectObserver::bold()
{
	return false;
}

bool CMfcTreeObjectObserver::enable()
{
	return true;
}