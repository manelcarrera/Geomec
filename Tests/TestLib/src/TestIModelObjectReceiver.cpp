 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IModelObject.h"
#include "TestIModelObjectReceiver.h"
#include "DocumentBase.h"
#include "qtextstream.h"


namespace test_lib
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelObjectReceiver::CModelObjectReceiver(IModelObject& object, QTextStream &os) 
: m_object(object), m_os(os)
{
  connect(&object, SIGNAL(onStateChanged()), this, SLOT(onStateChanged()));
  connect(&object, SIGNAL(onDisplayChanged(IModelObject&)), this, SLOT(onDisplayChanged()));
  connect(&object, SIGNAL(onPropertyChanged(IModelObject&)), this, SLOT(onPropertyChanged()));
  connect(&object, SIGNAL(onGeometryChanged()), this, SLOT(onGeometryChanged()));
  connect(&object, SIGNAL(onParameterChanged()), this, SLOT(onParameterChanged()));
  connect(&object, SIGNAL(onChildOrderChanged()), this, SLOT(onChildOrderChanged()));
  connect(&object, SIGNAL(onChildInserted(IModelObject&, IModelObject&)), this, SLOT(onChildInserted()));
  connect(&object, SIGNAL(onChildRemoved(IModelObject&, IModelObject&)), this, SLOT(onChildRemoved()));
  connect(&object, SIGNAL(onReferenceInserted()), this, SLOT(onReferenceInserted()));
  connect(&object, SIGNAL(onReferenceRemoved()), this, SLOT(onReferenceRemoved()));
  connect(&object, SIGNAL(onDestroyed()), this, SLOT(onDestroyed()));
  connect(&object, SIGNAL(onError(const QString&)), this, SLOT(onError(const QString&)));
  connect(&object, SIGNAL(onWarning(const QString&)), this, SLOT(onWarning(const QString&)));
}

CModelObjectReceiver::~CModelObjectReceiver()
{
}

void CModelObjectReceiver::onStateChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onStateChanged" << endl;
}

void CModelObjectReceiver::onDisplayChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onDisplayChanged" << endl;
}

void CModelObjectReceiver::onPropertyChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onPropertyChanged" << endl;
}

void CModelObjectReceiver::onGeometryChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onGeometryChanged" << endl;
}

void CModelObjectReceiver::onParameterChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onParameterChanged" << endl;
}

void CModelObjectReceiver::onChildOrderChanged()
{
  m_os << "Modelobject " << m_object.text() << " fired onChildOrderChanged" << endl;
}

void CModelObjectReceiver::onChildInserted()
{
  m_os << "Modelobject " << m_object.text() << " fired onChildInserted" << endl;
}

void CModelObjectReceiver::onChildRemoved()
{
  m_os << "Modelobject " << m_object.text() << " fired onChildRemoved" << endl;
}

void CModelObjectReceiver::onReferenceInserted()
{
  m_os << "Modelobject " << m_object.text() << " fired onReferenceInserted" << endl;
}

void CModelObjectReceiver::onReferenceRemoved()
{
  m_os << "Modelobject " << m_object.text() << " fired onReferenceRemoved" << endl;
}

void CModelObjectReceiver::onDestroyed()
{
  m_os << "Modelobject " << m_object.text() << " fired destroyed" << endl;
  delete this;
}

void CModelObjectReceiver::onError( const QString& description )
{
  m_os << "Modelobject " << m_object.text() << " fired onError" << endl;
  m_os << "\tdescription: \"" << description << "\"" << endl;
}

void CModelObjectReceiver::onWarning( const QString& description )
{
  m_os << "Modelobject " << m_object.text() << " fired onWarning" << endl;
  m_os << "\tdescription: \"" << description << "\"" << endl;
}

CDocumentReceiver::CDocumentReceiver(CDocumentBase& object, QTextStream &os)
: CModelObjectReceiver(object, os)
{
  connect(&object, SIGNAL(onBeginOperation()), this, SLOT(onBeginOperation()));
  connect(&object, SIGNAL(onEndOperation()), this, SLOT(onEndOperation()));
}

void CDocumentReceiver::onBeginOperation()
{
  m_os << "Document " << m_object.text() << " fired onBeginOperation" << endl;
}

void CDocumentReceiver::onEndOperation()
{
  m_os << "Document " << m_object.text() << " fired onEndOperation" << endl;
}



}