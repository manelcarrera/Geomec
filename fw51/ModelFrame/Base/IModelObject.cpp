 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
//
// imodelobject.cpp: implementation of the IModelObject class.
//
#include <QDebug>
#include <qcolor.h>
#include <set>
#include "IModelObject.h"
#include "DocumentBase.h"
#include "IProgressBase.h"
#include "IModelStream.h"
#include "dimple.h"
#include "StreamVersion.h"
#include "IModelObjectFactory.h"
#include "ModelOperation.h"
#include "TextUndo.h"
#include "DestroyUndo.h"
#include "CreateUndo.h"
#include "ColorUndo.h"
#include "ReferenceUndo.h"
#include "SimpleUndo.h"
#include <typeinfo>


/*!
  Class to sort the children of the IModelObject
*/

class CModelObjectLess
{
public:
  bool operator()(const IModelObject* A, const IModelObject* B) const
  { return A->less(*B); }
};

/*!
  This is the default constructor before a store or restore operation has
  taken place.
  Construction of the object must be followed by a call to create().

  \sa create
*/
IModelObject::IModelObject()
: m_color(qRgb(122, 122, 122)),
  m_text( QString::null ),
  m_created( false ),
  m_destroyed( false )
{
}

/*!
  This is the standard constructor of any node. Construction of the object must be followed by a call to create().

  \sa create
*/
IModelObject::IModelObject( const QString& text)
: QObject(0),
  m_color(qRgb(122, 122, 122)),
  m_text(text),
  m_created( false ),
  m_destroyed( false )
{
  setObjectName( text );
}

/*!
  Destroys the object and releases all allocated resources.

  Deletion of the object must be done by a call to destroy().
  Do not use delete directly, for it will not notify and cleanup the object's
  environment!

  \sa destroy
*/
IModelObject::~IModelObject()
{
  // destroy must have been called...
#if 0
  if ( m_created )
    DIA_ASSERT( m_destroyed );
#endif
  DIA_ASSERT( parent() == 0 );
}

CUndoOperation*
IModelObject::constructCreateUndoOperation( IModelObject& object,
                      const QString& description )
{
  return new CCreateUndo( object, description );
}

CUndoOperation*
IModelObject::constructDestroyUndoOperation( IModelObject& location,
                                              IModelObject& object,
                                              const QString& description )
{
  return new CDestroyUndo( location, object, description );
}

/*!
  Creates the modelobject at the specified document location.
  Construction should always be followed by a call to this function.
  The purpose is to split object construction from attaching to the environment.
  This way clients have type information available during signal handling.
  Supplying a parent is not compulsory. The document instance never has a
  parent. A non-document object without a parent can not use document services
  like undo/redo.
*/
void IModelObject::create( IModelObject*       newParent /* = 0 */,
                           const IModelObject* before /* = 0 */ )
{
  DIA_ASSERT( !m_created );
  m_created = true;

  if ( newParent ) {
  CModelOperation operation( *newParent );

  DIA_ASSERT( parent() == 0 ); // must not have been created yet

  addToParent( newParent, before );

  if ( operation.stackEnabled() )
      operation.push( constructCreateUndoOperation( *this,
                                          tr( "Create %1" ).arg( text() )  ) );

  // execute specialized creation code
  onCreate(); // call here, before COperation gets destructed.

  newParent->childInserted( *this );
  } else {

  // execute specialized creation code
  onCreate();
  }
}

void IModelObject::addToParent( IModelObject*       newParent,
                               const IModelObject* before /* = 0 */ )
{
  DIA_ASSERT( m_created );
  DIA_ASSERT( newParent );

  // Connect
  setParent( newParent );
  DIA_ASSERT( parent() == newParent );

  int index = newParent->m_children.count(); // equiv to append
  if ( before ) {
  index = newParent->m_children.indexOf( const_cast<IModelObject*>( before ));
  DIA_ASSERT( index != -1 );
  }
  newParent->m_children.insert( index, this );
}

/*!
  This function is automatically called after the modelobject has been created.
  Derivatives can override this function to supply specialized creation code.
*/
void IModelObject::onCreate()
{
}

/*!
  This function is automatically called just before the modelobject will be
  destroyed. Derivatives can override this function to supply specialized
  destruction code.
*/
void IModelObject::onDestroy()
{
  DIA_ASSERT( m_destroyed ); // must only be called while destroying
}

void IModelObject::setText(const QString& new_text)
{
  if( new_text != m_text ) {
  CModelOperation operation( *this );
  if ( operation.stackEnabled() )
      operation.push( new CTextUndo( *this, m_text,
                                     tr( "Rename \"%1\"" ).arg( m_text ) ) );
  m_text = new_text;
  displayChanged();
  }
}

/*!
  Returns the name of text of the object
*/
QString IModelObject::text() const
{
  return m_text;
}

void IModelObject::stateChanged()
{
  CModelOperation operation( *this );

  stateChanged( *this );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refStateChanged( *this );
  }
  if ( operation.stackEnabled() )
     operation.push( new CSimpleUndo( *this, "State changed ") );
}

/*!
  The state of an object is changed and invalidates storage.
*/
void IModelObject::stateChanged( IModelObject& origin )
{
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->stateChanged( origin );
  emit onStateChanged();
}

/*!
  Override to handle state changes of referenced objects.
*/
void IModelObject::refStateChanged( IModelObject& /*ref*/ )
{
}

void IModelObject::displayChanged()
{
  displayChanged( *this );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refDisplayChanged( *this );
  }
}

/*!
  The display properties of an object have changed.
  Storage is not invalidated!
*/
void IModelObject::displayChanged( IModelObject& origin )
{
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->displayChanged( origin );
  emit onDisplayChanged( origin );
}

/*!
  Override to handle display changes of referenced objects.
*/
void IModelObject::refDisplayChanged( IModelObject& /*ref*/ )
{
}

void IModelObject::propertyChanged()
{
  propertyChanged( *this );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refPropertyChanged( *this );
  }
}

/*!
  A property of the object has changed and the storage is invalidated.
*/
void IModelObject::propertyChanged( IModelObject& origin )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->propertyChanged( origin );
  emit onPropertyChanged( *this );
}

/*!
  Override to handle property changes of referenced objects.
*/
void IModelObject::refPropertyChanged( IModelObject& /*ref*/ )
{
}

void IModelObject::geometryChanged()
{
  geometryChanged( *this );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refGeometryChanged( *this );
  }
}

/*!
  The geometry of the object has changed.
  Storage (and mesh) are invalidated.
*/
void IModelObject::geometryChanged( IModelObject& origin )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->geometryChanged( origin );
  emit onGeometryChanged();
}

/*!
  Override to handle geometry changes of referenced objects.
*/
void IModelObject::refGeometryChanged( IModelObject& /*ref*/ )
{
}

void IModelObject::parameterChanged()
{
  parameterChanged( *this );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refParameterChanged( *this );
  }
}

/*!
  A model parameter is changed.
  Results and storage are invalidated.
*/
void IModelObject::parameterChanged( IModelObject& origin )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->parameterChanged( origin );
  emit onParameterChanged();
}

/*!
  Override to handle parameter changes of referenced objects.
*/
void IModelObject::refParameterChanged( IModelObject& /*ref*/ )
{
}

void IModelObject::childInserted( IModelObject& child )
{
  childInserted( *this, child );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refChildInserted( *this, child );
  }
}

/*!
  A child is inserted and the storage is invalidated.
*/
void IModelObject::childInserted( IModelObject& origin, IModelObject& child )
{
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->childInserted( origin, child );
  emit onChildInserted( origin, child );
}

/*!
  Override to handle child insertion in referenced objects.
*/
void IModelObject::refChildInserted( IModelObject& /*ref*/, IModelObject& /*child*/ )
{
}

void IModelObject::childRemoved( IModelObject& child )
{
  childRemoved( *this, child );
  for ( QList<IModelObject*>::const_iterator it = m_links.begin();
    it != m_links.end();
    ++it ) {
  (*it)->refChildRemoved( *this, child );
  }
}

/*!
  A child is removed and the storage is invalidated.
*/
void IModelObject::childRemoved(  IModelObject& origin, IModelObject& child )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->childRemoved( origin, child );
  emit onChildRemoved( origin, child );
}

/*!
  Override to handle child removal from referenced objects.
*/
void IModelObject::refChildRemoved( IModelObject& /*ref*/, IModelObject& /*child*/ )
{
}

/*!
  Sorts the children of a model object
*/
void IModelObject::sortChildren()
{
  DIA_ASSERT( m_created );
  COperation operation(*this);
  typedef std::multiset<IModelObject*, CModelObjectLess> TModelObjectSet;
  TModelObjectSet set;
  int i;
  for( i = 0; i < childSize(); i++ )
  set.insert(&childAt(i));

  i = 0;
  for( TModelObjectSet::iterator it = set.begin(); it != set.end(); it++ ) {
  m_children.replace( i, *it );
  i++;
  }

  onChildOrderChanged();
}

void IModelObject::referenceInserted( const IModelObject& ref )
{
  referenceInserted( *this, ref );
}

/*!
  A reference is inserted and the storage is invalidated.
*/
void IModelObject::referenceInserted( const IModelObject& origin,
                                      const IModelObject& ref)
{
  DIA_ASSERT( m_created );
  COperation operation(*this);
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->referenceInserted( origin, ref );
  emit onReferenceInserted();
}

void IModelObject::referenceRemoved( const IModelObject& ref )
{
  referenceRemoved( *this, ref );
}

/*!
  A reference is removed and the storage is invalidated.
*/
void IModelObject::referenceRemoved( const IModelObject& origin,
                                     const IModelObject& ref)
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->referenceRemoved( origin, ref);
  emit onReferenceRemoved();
}

void IModelObject::error( const QString& description )
{
  error( *this, description );
}

/*!
  An error occurred.
*/
void IModelObject::error( IModelObject& origin, const QString& description )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->error( origin, description );
  emit onError( description );
}

void IModelObject::warning( const QString& description )
{
  warning( *this, description );
}

/*!
  A warning occurred.
*/
void IModelObject::warning( IModelObject& origin, const QString& description )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = (IModelObject*)parent();
  if ( pParent ) pParent->warning( origin, description );
  emit onWarning( description );
}

const CDocumentBase* IModelObject::document() const
{
  const QObject *pObject = this;
  while(pObject->parent()) pObject = pObject->parent();
  return dynamic_cast<const CDocumentBase*>( pObject );
}

CDocumentBase* IModelObject::document()
{
  QObject *pObject = this;
  while(pObject->parent()) pObject = pObject->parent();
  return dynamic_cast<CDocumentBase*>( pObject );
}

int IModelObject::index() const
{
  DIA_ASSERT( m_created );
  if( parent() ) {
  IModelObject* pParent = dynamic_cast<IModelObject*>(parent());
  if ( pParent ) {
      int i = pParent->m_children.indexOf( const_cast<IModelObject*>( this ) );
      DIA_ASSERT( i != -1 );
      return i;
  }
  }
  return -1;
}

bool IModelObject::less(const IModelObject& rhs) const
{
  DIA_ASSERT( m_created );
  return text().compare(rhs.text()) < 0;
}

/*!
  Stores the tree location of all references in stream.
  Existence of the document is compulsory.
*/
void IModelObject::storeReferences( TStream& stream ) const
{
  DIA_ASSERT( document() );

  int size = referenceSize();
  stream << size;
  int i;
  for ( i = 0; i < size; ++i ) {
  CModelLocation location( referenceAt( i ) );
  location.store( stream );
  }
  for ( i = 0; i < childSize(); ++i ) childAt( i ).storeReferences( stream );
}

/*!
  Restores the references whose document locations are stored in stream.
  Existence of the document is compulsory.
*/
void IModelObject::restoreReferences( TStream&        stream,
                                      const CStreamVersion& fileVersion )
{
  DIA_ASSERT( document() );

  int size;
  stream >> size;
  int i;
  for ( i = 0; i < size; ++i ) {
  CModelLocation location;
  location.restore( stream, fileVersion );
  IModelObject* object = location.getObject( *document() );
  // Check if reference already exists
  const IModelObject* before = 0;
  if ( i < referenceSize() )
      before = &referenceAt( i );
  if ( before != object ) insertReference( *object, before );
  }
  while ( referenceSize() > size ) removeReference( referenceSize() - 1 );

  for ( i = 0; i < childSize(); ++i )
  childAt( i ).restoreReferences( stream, fileVersion );
}

void IModelObject::reParent( IModelObject*       new_parent,
                             const IModelObject* before )
{
  DIA_ASSERT( m_created );
  IModelObject* pParent = dynamic_cast<IModelObject*>(parent());
  IModelObject* operationLocation = new_parent;
  if ( !operationLocation ) operationLocation = pParent;
  if ( !operationLocation ) return; //No current and not new parent, weird

  if ( pParent ) {
  CModelOperation operation( *this );
  // Disconnect
  pParent->childRemoved( *this );
  if ( operation.stackEnabled() )
    operation.push( constructDestroyUndoOperation( *pParent,
                            *this,
                            "Delete old parent" ) );

  pParent->m_children.removeOne( this );
  setParent( 0 );
  }
  if ( new_parent ) {
  CModelOperation operation( *new_parent );

  DIA_ASSERT( m_created ); // No need to set m_created here
  addToParent( new_parent, before );

  new_parent->childInserted(*this);

  if ( operation.stackEnabled() )
      operation.push( constructCreateUndoOperation( *this, tr( "reParent" ) ) );
  }
}

/*!
  This function is an override of the QObject::insertChild function.
  Note that only IModelObject instances can be inserted.
  The insertChild function is the same as the reParent(function)
*/
void IModelObject::insertChild(QObject *pObject)
{
  DIA_ASSERT( m_created );
  IModelObject *pModelObject = dynamic_cast<IModelObject*>(pObject);
  DIA_ASSERT(pModelObject);
  pModelObject->reParent(this);
}

/*!
  This function is an override of the QObject::removeChild function. The removeChild function destroys the child.
*/
void IModelObject::removeChild(QObject *pObject)
{
  DIA_ASSERT( m_created );
  IModelObject *pModelObject = dynamic_cast<IModelObject*>(pObject);
  DIA_ASSERT(pModelObject);
  pModelObject->destroy();
}

int IModelObject::childSize() const
{
  DIA_ASSERT( m_created );
  return m_children.count();
}

const IModelObject& IModelObject::childAt( int index ) const
{
  DIA_ASSERT( m_created );
  DIA_ASSERT( index >= 0 );
  DIA_ASSERT( index < childSize() );
  const IModelObject* object = m_children.at( index );
  DIA_ASSERT( object );
  return *object;
}

IModelObject& IModelObject::childAt( int index )
{
  DIA_ASSERT( m_created );
  DIA_ASSERT( index >= 0 );
  DIA_ASSERT( index < childSize() );
  IModelObject* object = m_children.at( index );
  DIA_ASSERT( object );
  return *object;
}

/*!
  Returns the first child for which text() equals text.
  If no child matches, a null pointer is returned.
*/
const IModelObject* IModelObject::findChild( const QString& text ) const
{
  for ( int i = 0; i < childSize(); ++i )
  if ( childAt( i ).text() == text ) return &childAt( i );
  return 0;
}

/*!
  Returns the first child for which text() equals text.
  If no child matches, a null pointer is returned.
*/
IModelObject* IModelObject::findChild( const QString& text )
{
  for ( int i = 0; i < childSize(); ++i )
  if ( childAt( i ).text() == text ) return &childAt( i );
  return 0;
}

/*!
  Total number of reference attached to the node
*/
int IModelObject::referenceSize() const
{
  return m_references.count();
}

/*!
  Returns reference based on the reference index
*/
const IModelObject& IModelObject::referenceAt( int index ) const
{
  DIA_ASSERT( index < referenceSize() );
  DIA_ASSERT( index >= 0);
  const IModelObject* object = m_references.at( index );
  DIA_ASSERT( object );
  return *object;
}

int IModelObject::insertReference( const IModelObject& object, const IModelObject* insertBefore )
{
  CModelOperation operation( *this );
  if ( operation.stackEnabled() )
  operation.push( new CReferenceUndo( *this, object, 0 ) );

  DIA_ASSERT( isReferenced( object ) == -1 );
  int index = referenceSize(); // equivalent to append
  if ( insertBefore ) {
  index = m_references.indexOf( insertBefore );
  DIA_ASSERT( index != -1 );
  }
  m_references.insert( index, &object );
  object.m_links.append( this );
  referenceInserted( object );
  return index;
}

/*!
  Return the index of the reference and -1 when the object is not referenced.
*/
int IModelObject::isReferenced( const IModelObject& object ) const
{
  return m_references.indexOf( &object );
}

void IModelObject::removeReference( int index )
{
  DIA_ASSERT( index < referenceSize() );
  DIA_ASSERT( index >= 0 );

  CModelOperation operation( *this );

  const IModelObject* ref = m_references.takeAt( index );
  if ( operation.stackEnabled() )
  operation.push( new CReferenceUndo( *this, *ref, index, 0 ) );
  bool succes = ref->m_links.removeOne( this );
  DIA_ASSERT( succes );

  referenceRemoved( *ref );
}

void IModelObject::removeReference( const IModelObject& object )
{
  DIA_ASSERT( m_created );
  removeReference( isReferenced( object ) );
}

int IModelObject::geometrySize() const
{
  DIA_ASSERT( m_created );
  return 0;
}

QRgb IModelObject::color() const
{
  return m_color;
}

void IModelObject::color(QRgb c)
{
  if ( m_color != c ) {
  CModelOperation operation( *this );
  if ( operation.stackEnabled() )
      operation.push( new CColorUndo( *this, m_color,
                                      tr( "Color \"%1\"" ).arg( m_text ) ) );
  m_color = c;
  displayChanged();
  }
}

const geo::IObject& IModelObject::geometry(int /*nIndex*/) const
{
  DIA_ASSERT( m_created );
  DIA_ASSERT(false);
  geo::IObject* pBogus = 0;
  return *pBogus;
}

bool IModelObject::acceptVisitor(IModelObjectVisitor& /*visitor*/)
{
  DIA_ASSERT( m_created );
  return false;
}

  // Returns true when modelobject does not contain information
bool IModelObject::empty() const
{
  DIA_ASSERT( m_created );
  return true;
}

bool IModelObject::valid() const
{
  DIA_ASSERT( m_created );
  return true;
}

/*!
  Destroys the modelobject and its children and notifies the environment.

  Always use this function to destroy a modelobject; do not use delete.
  The purpose is to split object destruction and detachment from the
  environment.
  This way clients have type information available during signal handling.
*/
void IModelObject::destroy()
{
  DIA_ASSERT( m_created );
  if ( m_destroyed ) return; // No sense in handling a second call
                             // to destroy ( occurrs for instance when
                             // destroy is called from within
                             // childRemoved() or referenceRemoved() )

#if 0
  fprintf( stderr, "destroy: %s(%p) class=%s\n",
  text().toLatin1().data(), this, typeid( *this ).name() );
#endif

  m_destroyed = true;

  { // !!! New scope to end operation before delete this
  CModelOperation operation( *this, tr( "Delete %1" ).arg( text() ) );

  // execute specialized destruction code
  onDestroy();

  // Remove reference from other objects
  while( !m_links.isEmpty() ) m_links.first()->removeReference( *this );

  // Destroy children
  int sz = childSize();
#if 0
  /* Last one first, gives a realy bad preformace on GEOMEC TEMPER table,
       but was once introduced for performance reasons */
  while ( sz > 0 ) childAt( --sz ).destroy();
#else
  while ( sz-- > 0 ) childAt( 0 ).destroy();
#endif

  // Delete references
  while( referenceSize() > 0 ) removeReference(0);

  // Detach from parent
  if ( parent() ) {
      IModelObject* pParent = dynamic_cast<IModelObject*>(parent());
      DIA_ASSERT(pParent);

      pParent->childRemoved(*this);
      if ( operation.stackEnabled() )
    operation.push( constructDestroyUndoOperation( *pParent, *this,
                                          tr( "Delete %1" ).arg( text() ) ) );
      pParent->m_children.removeOne( this );
      setParent( 0 );
#if 0
     /* removeChild is not aware of currentItem in QGList,
    search is always done from start of list */
     fprintf( stderr, "destroy: parent= %s(%p)\n",
          pParent->text().toLatin1().data(),
          pParent );
#endif
  }

  // Emit that we're destroyed
  emit onDestroyed();
  }

  delete this;
}

bool IModelObject::canDestroy() const
{
  DIA_ASSERT( m_created );
  return false;
}

void IModelObject::Connect(const TModelObjectVec& /*object*/)
{
  DIA_ASSERT( m_created );

}

bool IModelObject::CanConnect(const TModelObjectVec& /*object*/)
{
  DIA_ASSERT( m_created );
  return false;
}

void IModelObject::clear()
{
  DIA_ASSERT( m_created );
  COperation operation(*this);
  int sz = childSize();
  while ( sz > 0 ) childAt( --sz ).destroy();
  while ( referenceSize() ) removeReference(0);
}

  // The store and restore functions are used for saving, loading, redo and undo
void IModelObject::restore( TStream&              stream,
              const CStreamVersion& file_version,
              IProgressBase&        indicator )
{
  // Local version for local changes
  CStreamVersion version;
  version.restore(stream);

  // Restore the name and the color
  QString previousText = m_text;
  stream >> m_text;
// qDebug() << "Restore: " << m_text;

  QRgb previousColor = m_color;
  int nRed, nGreen, nBlue, nAlpha;
  stream >> nRed;
  stream >> nGreen;
  stream >> nBlue;
  stream >> nAlpha;
  m_color = qRgba(nRed, nGreen, nBlue, nAlpha);

  if ( m_text != previousText || m_color != previousColor ) displayChanged();

  if ( version >= CStreamVersion( 0, 0, 2 ) ) {
  int restoreChildren;
  stream >> restoreChildren;
  if ( restoreChildren ) {
      int nSize;
      stream >> nSize;
      for ( int i = 0; i < nSize; i++ ) {
    QString className;
    stream >> className;
// qDebug() << "Restore: " << m_text << " child " << i << " " << className;
    // Restoring an IModelObject base class instance should not occur
    DIA_ASSERT( className != "IModelObject" );

    // Check if child already exists (e.g. default children like document
    // containers or fixed children
    IModelObject* child = 0;
    IModelObject* before = 0;
    if ( i < m_children.count() ) {
          before = &childAt( i );
          if ( QString( before->metaObject()->className() ) == className )
      child = before;
    }

    bool newChild = ( !child );
    if ( newChild ) {
          child = IModelObjectFactory::create( className.toStdString().c_str() );
          DIA_ASSERT( child );

          child->m_created = true;

          child->addToParent( this, before );
    }
    child->restore(stream, file_version, indicator);
    if ( newChild )
    {
          child->onCreate();
          childInserted( *child );
    }
      }
      if ( m_children.count() != nSize )
    qDebug() << "IModelObject::restore "
                 << metaObject()->className()
                 << " " << m_text
                 << " child count = " <<  m_children.count()
                 << " restored    = " << nSize;
  }
  }

  indicator.Step();
}

void IModelObject::store( TStream&       stream,
                          IProgressBase& indicator,
                          bool           includeChildren ) const
{
  // Local version for local changes
  CStreamVersion version(0, 0, 2);
  version.store(stream);

  // Store name and color
  stream << m_text;
// qDebug() << "Store: m_text = " << m_text;

  stream << qRed(color());
  stream << qGreen(color());
  stream << qBlue(color());
  stream << qAlpha(color());

  stream << (int)includeChildren;
  if ( includeChildren ) {
  stream << (int)childSize();
  for ( int i = 0; i < childSize(); i++ ) {
      QString className = childAt( i ).metaObject()->className();
// qDebug() << "Store: " << m_text << " child " << i << " " << className;
      // Streaming an IModelObject base class instance should not be possible.
      DIA_ASSERT( className != "IModelObject" );

      stream << className;
      childAt( i ).store( stream, indicator, includeChildren );
  }
  }

  indicator.Step();
}

int IModelObject::storeSteps( bool includeChildren ) const
{
  int steps = 1;
  if ( includeChildren )
  for ( int i = 0; i < childSize(); i++ )
      steps += childAt( i ).storeSteps();
  return steps;
}

int IModelObject::isChild(const IModelObject& object) const
{
  DIA_ASSERT( m_created );
  for ( int i = 0; i < childSize(); ++i )
  if ( &childAt(i) == &object )
      return i;
  return -1;
}

bool IModelObject::isChildInTree(const IModelObject& object) const
{
  DIA_ASSERT( m_created );
  int i;
  for ( i=0 ; i< childSize();i++ ) {
  if ( &childAt(i) == &object           ) return true;
  if ( childAt(i).isChildInTree(object) ) return true;
  }
  return false;
}

void IModelObject::moveChildren( const QList<IModelObject*>& children,
                                 int destinationIndex )
{
  if ( destinationIndex < 0 )
  destinationIndex = 0;

  for ( int i = 0; i != children.size(); ++i ) {
  int index = m_children.indexOf( children[i] );
  if ( index == -1 ) continue; // not a child, skip it.
  IModelObject* objToMove = m_children.takeAt( index );

  // If destination is after remove point it shifted one position due to
  // the takeAt() above. Adjust it.
  if ( destinationIndex > index )
      --destinationIndex;

  // Insert at destination index. Append if the destination index is out of
  // bounds.
  int size = m_children.size();
  if ( destinationIndex < size ) {
      m_children.insert( destinationIndex, objToMove );
  }
  else {
      destinationIndex = size;
      m_children.push_back( objToMove );
  }

  // Move next item below the just moved item to keep order as expected
  ++destinationIndex;
  }
  emit onChildOrderChanged();
}

/*virtual*/ QString IModelObject::getInvalidReasonDescription() const
{
  DIA_ASSERT( m_created );
  //derived objects should give reason
  DIA_ASSERT(false);
  DIA_ASSERT(!valid());
  return "Reason for invalid is unknown";
}

/*!
  Returns an iterator to walk over all objects keeping a reference to this
  object.
*/
IModelObject::TModelObjectIterator IModelObject::referenceOrigins() const
{
  return TModelObjectIterator( m_links );
}
