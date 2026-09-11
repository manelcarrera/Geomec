 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// documentbase.cpp: implementation of the CDocumentBase class.
//
//////////////////////////////////////////////////////////////////////
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <cassert>
#include "DocumentBase.h"
#include "QtDataStream.h"
#include "StreamVersion.h"
#include "ModelOperation.h"
#include "ModelOperationStack.h"
#include "ModelMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDocumentBase::CDocumentBase( const QString& name )
: IModelObject( name ),
  m_bOpen(false)
{
  m_operationStack = new CModelOperationStack( *this );
  m_operationStack->setEnabled( false );
  connect( m_operationStack, SIGNAL( operationStarted() ),
           this,             SIGNAL( onBeginOperation() ) );
  connect( m_operationStack, SIGNAL( operationFinished() ),
           this,             SIGNAL( onEndOperation() ) );
}

CDocumentBase::~CDocumentBase()
{
	assert(!isOpen());
	assert(childSize() == 0);
	assert(referenceSize() == 0);
  delete m_operationStack; // remove when parented via QObject
}

void CDocumentBase::setPathName(const QString &name)
{
	COperation operation(*this);
	m_path_name = name;
}

const QString& CDocumentBase::pathName() const
{
  return m_path_name;
}

CModelOperationStack& CDocumentBase::operationStack()
{
  return *m_operationStack;
}

/*!
	The CreateDefaults() override allows the client create default object during newDocument() function.
*/
void CDocumentBase::createDefaults()
{
}

void CDocumentBase::stateChanged( IModelObject& /*origin*/ )
{
	emit onStateChanged();
}
		
void CDocumentBase::displayChanged( IModelObject& origin )
{
  emit onDisplayChanged( origin );
}

void CDocumentBase::propertyChanged( IModelObject& origin )
{
  emit onPropertyChanged( origin );
}

void CDocumentBase::geometryChanged( IModelObject& /*origin*/ )
{
	emit onGeometryChanged();
}

void CDocumentBase::parameterChanged( IModelObject& /*origin*/ )
{
	emit onParameterChanged();
}

void CDocumentBase::childInserted( IModelObject& origin, IModelObject& child )
{
	emit onChildInserted( origin, child );
}

void CDocumentBase::childRemoved( IModelObject& origin, IModelObject& child )
{
	emit onChildRemoved( origin, child );
}

void CDocumentBase::referenceInserted( const IModelObject& /*origin*/,
                                       const IModelObject& /*ref*/ )
{
	emit onReferenceInserted();
}

void CDocumentBase::referenceRemoved( const IModelObject& /*origin*/,
                                      const IModelObject& /*ref*/ )
{
	emit onReferenceRemoved();
}

void CDocumentBase::error( IModelObject& /*origin*/, const QString& description )
{
	emit onError( description );
}

void CDocumentBase::warning( IModelObject& /*origin*/, const QString& description )
{
        emit onWarning( description );
}

void CDocumentBase::open()
{
	m_bOpen = true;
}

/*!
	Returns true if the user can create and use a new, empty document.
*/
bool CDocumentBase::canCreateEmptyDocument() const
{
  return true;
}

CStreamVersion CDocumentBase::lastReadableVersion() const
{
  // Default is current document version. Derived classes can implement specific
  // behavior.
  return documentVersion();
}

/*!
 * @returns True if the given version can be read in by the application. The
 *          default implementation returns true if the given version matches
 *          the version returned by documentVersion(). Derived classes can
 *          reimplement this method to allow reading in other versions
 *          (naturally, these versions must still be detected and dealt with
 *          while reading in the 'restore' methods in the model).
 */
bool CDocumentBase::canReadVersion( const CStreamVersion& version ) const
{
  return ( version >= lastReadableVersion() ) &&
         ( version <= documentVersion() );
}

bool CDocumentBase::newDocument()
{  
  bool bUndoRedoEnable = operationStack().isEnabled();
	operationStack().setEnabled( false );	// Disable undo redo to prevent unwanted change of the stack

	assert(!isOpen());
	createContainers();
	createDefaults();
	open();

  operationStack().setEnabled( bUndoRedoEnable );
	return true;
}

void CDocumentBase::closeDocument()
{
  emit closing();

    // Disable undo redo to prevent unwanted change of the stack  
	operationStack().setEnabled( false );

	assert(isOpen());
	m_bOpen = false;
	destroy();
}

bool CDocumentBase::saveDocument(const QString& sPath, IProgressBase& progress) 
{
	COperation operation(*this);
	
	bool bUndoRedoEnable = operationStack().isEnabled();
	operationStack().setEnabled( false );	// Disable undo redo to prevent unwanted change of the stack
	
	assert(isOpen());
	QFile file(sPath);
	if(!file.open(QIODevice::WriteOnly))
	{
		operationStack().setEnabled( bUndoRedoEnable );
		return false;
	}
	
  // Do setPathName() first - it class IModelObject::setText(), which means the
  // path name is saved by IModelObject::store(). If setPathName() is called
  // _after_ saving, the _old_ path is stored in the file.
  setPathName(sPath);

  QDataStream qt_stream(&file) ;
	CQtDataStream stream(qt_stream);
	stream << documentType();
	documentVersion().store(stream);
	store(stream, progress);
	storeReferences( stream );
	operationStack().setEnabled( bUndoRedoEnable );
  operationStack().setClean();
	return true;
}

/*!
	The default load document function reads a stream file and calls
	CreateContainers before the streaming is started.
	The function returns true if loading the document was succesfull. If
	loading failed, report the reason by means of a call or multiple calls
	to error().

	Override this function for custom loading.
*/
bool CDocumentBase::loadDocument(const QString& sPath, IProgressBase& progress)
{
  assert(!isOpen());

  QFile file(sPath);
  if(!file.open(QIODevice::ReadOnly))
  {
    QDir d = QDir::current();
    throw TModelError( tr("Could not open file %1 for reading").arg(d.filePath(sPath)), 7050 );
  }

  QDataStream qt_stream(&file);
  CQtDataStream stream( qt_stream );

  QString sDocumentType;
  stream >> sDocumentType;
  if(sDocumentType != documentType())
  {	
    throw TModelError( tr("File is not a Diana Document"), 7051 );
  }

  CStreamVersion file_version;
  file_version.restore(stream);
  if ( !canReadVersion( file_version ) )
  {
    CStreamVersion lastSupported = lastReadableVersion();
    throw TModelError( tr( "Could not read '%1': The reported file format "
                           "version is %2, but only versions %3 and above can "
                           "be read by this version of the application." )
                       .arg( sPath )
                       .arg( file_version.GetVersionString() )
                       .arg( lastSupported.GetVersionString() ),
                       7052 );
  }

  bool bUndoRedoEnable = operationStack().isEnabled();
  operationStack().setEnabled( false );

  createContainers();

  restore(stream, file_version, progress);
  restoreReferences( stream, file_version );
  setPathName(sPath);
  open();

  operationStack().setEnabled( bUndoRedoEnable );
  operationStack().setClean();

  return true;
}

bool CDocumentBase::isOpen()
{
  return m_bOpen;
}

const QString& CDocumentBase::documentDescription() const
{
	return m_document_description;
}

void CDocumentBase::documentDescription(const QString& sDescription)
{
	m_document_description = sDescription;
}

void CDocumentBase::restore(TStream& stream, const CStreamVersion& file_version, IProgressBase& indicator)
{
	IModelObject::restore(stream, file_version, indicator);
	stream >> m_document_description;
}

void CDocumentBase::store(TStream& stream, IProgressBase& indicator, bool includeChildren ) const
{
	IModelObject::store(stream, indicator, includeChildren );
	stream << m_document_description;
}

