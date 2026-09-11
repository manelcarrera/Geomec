 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IModelNodeReceiver.h: interface for the IModelNodeReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMODELNODERECEIVER_H__57E786EA_8466_440E_99B2_2D0D160DBB1A__INCLUDED_)
#define AFX_IMODELNODERECEIVER_H__57E786EA_8466_440E_99B2_2D0D160DBB1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>
#include <qtextstream.h>

class IModelObject;
class CDocumentBase;


namespace test_lib
{


class CModelObjectReceiver : public QObject
{
	Q_OBJECT
protected:
	IModelObject& m_object;
  QTextStream&  m_os;
public:
	CModelObjectReceiver(IModelObject& object, QTextStream &os);
	virtual ~CModelObjectReceiver();
private slots:
	void onStateChanged();		//*!< Emitted when the state of the object is changed.
	void onDisplayChanged();	//*!< Emitted when the display properties of an object are changed.
	void onPropertyChanged();	//*!< The a property of the object is changed.
	void onGeometryChanged();	//*!< The geometry of a the object is changed.
	void onParameterChanged();	//*!< A model parameter is changed and the results.
	void onChildOrderChanged();	//*!< The order of child nodes changed
	void onChildInserted();		//*!< A child is inserted.
	void onChildRemoved();		//*!< A child is removed
	void onReferenceInserted();	//*!< A reference is inserted.
	void onReferenceRemoved();	//*!< A reference is removed
	void onDestroyed();			//*!< Transmitting object destroyed
	void onError( const QString& description ); //*!< An error occurred
        void onWarning( const QString& description ); //*!< A warning occurred
};

class CDocumentReceiver : public CModelObjectReceiver
{
	Q_OBJECT
public:
	CDocumentReceiver(CDocumentBase& object, QTextStream &os);
private slots:
	void onBeginOperation();
	void onEndOperation();
};

}

#endif // !defined(AFX_IMODELNODERECEIVER_H__57E786EA_8466_440E_99B2_2D0D160DBB1A__INCLUDED_)
